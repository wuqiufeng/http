#include "http_handler.h"

static char *http_request_type[] = {
    "GET", "POST", "PUT", "PATCH", "DELETE", "COPY", "HEAD", "OPTIONS", "LINK",
    "UNLINK", "PUREG", "LOCK", "UNLOCK", "PROPFIND", "VIEW"
};

http_handler_t *http_request_init()
{
    http_handler_t *handler = (http_handler_t*)malloc(sizeof(http_handler_t));
    if (handler == NULL) {
        return NULL;
    }
    memset(handler, 0, sizeof(http_handler_t));

    if ((handler->curl = curl_easy_init()) == NULL) {
        free(handler);
        return NULL;
    }

    if ((handler->trunk.data = (char*)malloc(1)) == NULL) {
        curl_easy_cleanup(handler->curl);
        free(handler);
        return NULL;
    }
    handler->trunk.tail_idx = 0;
    handler->trunk.trunk_size = 0;

    return handler;
}

int32_t http_set_User_Agent(http_handler_t *handler, const char *user_agent)
{
    if (handler == NULL || user_agent == NULL) {
        return -1;
    }

    if (curl_easy_setopt(handler->curl, CURLOPT_USERAGENT, user_agent)  != CURLE_OK) {
        return -1;
    }
    return 0;
}


int32_t http_set_url(http_handler_t *handler, const char *url)
{
    if (handler == NULL || url == NULL) {
        return -1;
    }

    if (curl_easy_setopt(handler->curl, CURLOPT_URL, url)  != CURLE_OK) {
        return -1;
    }
    return 0;
}


int32_t http_set_type(http_handler_t *handler, http_req_type_t type)
{
    if (handler == NULL || type < http_type_get || type > http_type_view) {
        return -1;
    }

    if (curl_easy_setopt(handler->curl, CURLOPT_CUSTOMREQUEST, http_request_type[type]) != CURLE_OK) {
        return -1;
    }
    return 0;
}

int32_t http_set_header(http_handler_t *handler, const char *header)
{
    if (handler == NULL || header == NULL) {
        return -1;
    }

    if ((handler->headers = curl_slist_append(handler->headers, header)) == NULL) {
        return -1;
    }
    return 0;
}

int32_t http_set_timeout(http_handler_t *handler, http_timeout_type_t type, long timeout)
{
    if (handler == NULL || timeout < 0) {
        return -1;
    }

    switch (type) {
    case http_connect_timeout:
        curl_easy_setopt(handler->curl, CURLOPT_CONNECTTIMEOUT, timeout);
        break;
    case http_process_timeout:
        curl_easy_setopt(handler->curl, CURLOPT_TIMEOUT, timeout);
        break;
    default:
        return -1;
    }
    return 0;
}

int32_t http_set_trunksize(http_handler_t *handler, u_int32_t trunksize)
{
    if (handler == NULL) {
        return -1;
    }
    handler->trunk.trunk_size = trunksize;
    return 0;
}


int32_t http_set_body(http_handler_t *handler, void *body, long body_len)
{
    if (handler == NULL || body == NULL || body_len <= 0) {
        return -1;
    }

    if (curl_easy_setopt(handler->curl, CURLOPT_POSTFIELDS, body) != CURLE_OK) {
        return -1;
    }
    if (curl_easy_setopt(handler->curl, CURLOPT_POSTFIELDSIZE, body_len) != CURLE_OK) {
        return -1;
    }
    return 0;
}

int32_t http_process(http_handler_t *handler)
{
    if (handler == NULL) {
        return -1;
    }

    if ((handler->code = curl_easy_perform(handler->curl)) != CURLE_OK) {
        return -1;
    }
    return 0;
}

const char *http_get_error(http_handler_t *handler)
{
    return curl_easy_strerror(handler->code);
}

void *http_get_body(http_handler_t *handler)
{
    if (handler == NULL) {
        return NULL;
    }
    return handler->trunk.data;
}

u_int32_t http_get_body_len(http_handler_t *handler)
{
    if (handler == NULL) {
        return -1;
    }
    return handler->trunk.tail_idx;
}

void *http_get_header(http_handler_t *handler)
{
    if (handler == NULL) {
        return NULL;
    }
    return handler->header_reply.data;
}

u_int32_t http_get_header_len(http_handler_t *handler)
{
    if (handler == NULL) {
        return -1;
    }
    return handler->header_reply.tail_idx;
}

static size_t header_function(void *contens, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    size_t growth_size = realsize;
    struct response_header *header = (struct response_header*)userp;

    if (header->header_size) {
        //header size be set
        if (header->tail_idx == header->header_size) {
            return 0;
        }
        growth_size = (header->tail_idx + realsize >= header->header_size ?
                    header->header_size - header->tail_idx : realsize);
    }

    if ((header->data = realloc(header->data, header->tail_idx + growth_size + 1)) == NULL) {
        fprintf(stderr, "not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(header->data[header->tail_idx]), contens, realsize);

    header->tail_idx += realsize;
    header->data[header->tail_idx] = 0;

    return realsize;
}


static size_t write_function(void *contens, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    size_t growth_size = realsize;
    struct response_trunk *trunk = (struct response_trunk*)userp;

    if (trunk->trunk_size) {
        //trunk size be set
        if (trunk->tail_idx == trunk->trunk_size) {
            return 0;
        }
        growth_size = (trunk->tail_idx + realsize >= trunk->trunk_size ?
                    trunk->trunk_size - trunk->tail_idx : realsize);
    }

    if ((trunk->data = realloc(trunk->data, trunk->tail_idx + growth_size + 1)) == NULL) {
        fprintf(stderr, "not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(trunk->data[trunk->tail_idx]), contens, realsize);
    trunk->tail_idx += realsize;
    trunk->data[trunk->tail_idx] = 0;

    return realsize;
}


int32_t http_prepare(http_handler_t *handler)
{
    if (handler == NULL) {
        return -1;
    }

    if (curl_easy_setopt(handler->curl, CURLOPT_HTTPHEADER, handler->headers) != CURLE_OK) {
        return -1;
    }

    if (curl_easy_setopt(handler->curl, CURLOPT_HEADERFUNCTION, header_function) != CURLE_OK) {
        return -1;
    }
    if (curl_easy_setopt(handler->curl, CURLOPT_HEADERDATA, (void*)&handler->header_reply) != CURLE_OK) {
        return -1;
    }

    if (curl_easy_setopt(handler->curl, CURLOPT_WRITEFUNCTION, write_function) != CURLE_OK) {
        return -1;
    }
    if (curl_easy_setopt(handler->curl, CURLOPT_WRITEDATA, (void*)&handler->trunk) != CURLE_OK) {
        return -1;
    }

    return 0;
}


int32_t http_status_code(http_handler_t *handler)
{
    long status_code = 0;

    if (handler == NULL) {
        return -1;
    }
    if (curl_easy_getinfo(handler->curl, CURLINFO_RESPONSE_CODE, &status_code) != CURLE_OK) {
        return -1;
    }
    return status_code;
}

void http_clean(http_handler_t *handler)
{
    if (handler == NULL) {
        return;
    }
    if (handler->trunk.data) {
        free(handler->trunk.data);
        handler->trunk.data = NULL;
        handler->trunk.tail_idx = 0;
    }
    curl_slist_free_all(handler->headers);
    curl_easy_cleanup(handler->curl);
}

void http_request_destroy(http_handler_t *handler)
{
    if (handler == NULL) {
        return;
    }
    free(handler);
}

