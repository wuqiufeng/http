/***************************************************************
* 名    称: http_handler.h
* 创 建 者: 付红竹
* 创建日期: 2017年11月01日
* 描    述:
* 修改记录:
***************************************************************/
#ifndef __HTTP_HANDLER_H__
#define __HTTP_HANDLER_H__

#include "curl/curl.h"
#include "http_proxy.h"


typedef enum _http_req_type_t {
    http_type_get   = 0,
    http_type_post,
    http_type_put,
    http_type_patch,
    http_type_delete,
    http_type_copy,
    http_type_head,
    http_type_options,
    http_type_link,
    http_type_unlink,
    http_type_purge,
    http_type_lock,
    http_type_unlock,
    http_type_propfind,
    http_type_view
} http_req_type_t;

typedef enum _http_timeout_type_t {
    http_connect_timeout = 0,
    http_process_timeout
} http_timeout_type_t;


struct response_trunk {
    char *data;
    u_int32_t tail_idx;
    u_int32_t trunk_size;
};
struct response_header {
    char *data;
    u_int32_t tail_idx;
    u_int32_t header_size;
};

typedef struct _http_handler_t {
    CURL *curl;
    CURLcode code;
    struct curl_slist *headers;
    struct response_trunk trunk;
    struct response_header header_reply;
} http_handler_t;




http_handler_t *http_request_init();
int32_t http_set_url(http_handler_t *handler, const char *url);
int32_t http_set_type(http_handler_t *handler, http_req_type_t type);
int32_t http_set_header(http_handler_t *handler, const char *header);
int32_t http_set_timeout(http_handler_t *handler, http_timeout_type_t type, long timeout);
int32_t http_set_trunksize(http_handler_t *handler, u_int32_t trunksize);
int32_t http_set_body(http_handler_t *handler, void *body, long body_len);
int32_t http_status_code(http_handler_t *handler);
int32_t http_process(http_handler_t *handler);
int32_t http_prepare(http_handler_t *handler);
const char *http_get_error(http_handler_t *handler);
void *http_get_body(http_handler_t *handler);
void *http_get_header(http_handler_t *handler);
u_int32_t http_get_body_len(http_handler_t *handler);
u_int32_t http_get_header_len(http_handler_t *handler);


#endif /* __HTTP_HANDLER_H__ */
