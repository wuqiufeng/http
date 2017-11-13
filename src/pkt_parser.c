/***************************************************************
* 名    称: pkt_parser.c
* 创 建 者: 付红竹
* 创建日期: 2017年11月03日
* 描    述:
* 修改记录:
***************************************************************/

#include "pkt_parser.h"


void grint_print_byte(const int8_t* begin, uint16_t len)
{
    int i;
    int8_t* pointer = (int8_t*)begin;
    for(i=0; i<len; i++, pointer++)
        printf("%c", *pointer);
    printf("\n");
}



int32_t is_terminal_device_supported(struct pkt_info_t *pkt_info)
{
    const char *user_agent = NULL;

    if (!pkt_info) {
        return 0;
    }

    user_agent = HTTP_USER_AGENT(&pkt_info->http_info);
    if ((strcasestr(user_agent, "Trident/5.0") != NULL) ||
            (strcasestr(user_agent, "Trident/4.0") != NULL) ||
            (strcasestr(user_agent, "MSIE 9.0") != NULL) ||
            (strcasestr(user_agent, "MSIE 8.0") != NULL) ||
            (strcasestr(user_agent, "MSIE 7.0") != NULL) ||
            (strcasestr(user_agent, "MSIE 6.0") != NULL)) {
        return 0;
    }
    return 1;
}


int32_t http_req_is_mobile(struct http_info_t* http_info)
{
    char* user_agent = NULL;

    if (http_info->user_agent_idx != -1) {
        user_agent = http_info->headers[http_info->user_agent_idx].value;
        if ((strcasestr(user_agent, "iphone") != NULL) ||
            (strcasestr(user_agent, "ios") != NULL)) {
            http_info->request_flag.flag.is_ios = 1;
            return 1;
        }
        else if (strcasestr(user_agent, "android") != NULL){
            http_info->request_flag.flag.is_android = 1;
            return 1;
        }
    }
    return 0;
}

int32_t http_req_is_pc(struct http_info_t* http_info)
{
    char* user_agent = NULL;

    if (http_info->user_agent_idx != -1) {
        user_agent = http_info->headers[http_info->user_agent_idx].value;
        if ((strcasestr(user_agent, "windows") != NULL) ||
            (strcasestr(user_agent, "mac os") != NULL)) {
            return 1;
        }
    }
    return 0;
}

int32_t http_req_is_html(struct http_info_t* http_info)
{
    int32_t i = 0;

    for (i = 0; i < http_info->header_idx; i++) {
        if (strcasecmp(http_info->headers[i].key, "Accept") == 0) {
            if (strncasecmp(http_info->headers[i].value, "text/html", 9) == 0) {
                return 1;
            }
            return 0;
        }
    }

    return 0;
}

int32_t http_req_is_js(struct http_info_t* http_info)
{
    size_t urllen = 0;

    urllen = strlen(http_info->request_path);
    if (urllen < 3) {
        return 0;
    }
    if ((http_info->request_path[urllen-3] == '.') &&
        (http_info->request_path[urllen-2] == 'j') &&
        (http_info->request_path[urllen-1] == 's')) {
        return 1;
    }
    return 0;
}

int32_t http_req_is_video(struct http_info_t* http_info)
{
    size_t urllen = 0;

    urllen = strlen(http_info->request_path);
    if (urllen < 4) {
        return 0;
    }
    if ((http_info->request_path[urllen-4] == '.') &&
        (http_info->request_path[urllen-3] == 'm') &&
        (http_info->request_path[urllen-2] == 'p') &&
        (http_info->request_path[urllen-1] == '4')) {
        return 1;
    }
    if ((http_info->request_path[urllen-4] == '.') &&
        (http_info->request_path[urllen-3] == 'f') &&
        (http_info->request_path[urllen-2] == 'l') &&
        (http_info->request_path[urllen-1] == 'v')) {
        return 1;
    }
    if ((http_info->request_path[urllen-4] == '.') &&
        (http_info->request_path[urllen-3] == 'f') &&
        (http_info->request_path[urllen-2] == '4') &&
        (http_info->request_path[urllen-1] == 'v')) {
        return 1;
    }
    if ((http_info->request_path[urllen-3] == '.') &&
        (http_info->request_path[urllen-2] == 't') &&
        (http_info->request_path[urllen-1] == 's')) {
        return 1;
    }
    return 0;
}

int32_t http_req_is_app(struct http_info_t* http_info)
{
    size_t urllen = 0;

    urllen = strlen(http_info->request_path);
    if (urllen < 4) {
        return 0;
    }
    if ((http_info->request_path[urllen-4] == '.') &&
        (http_info->request_path[urllen-3] == 'a') &&
        (http_info->request_path[urllen-2] == 'p') &&
        (http_info->request_path[urllen-1] == 'k')) {
        return 1;
    }
    return 0;
}



int32_t grint_get_request_flag(struct http_info_t *http_info)
{
    if (http_info->user_agent_idx != -1) {
        if (http_req_is_mobile(http_info)) {
            http_info->request_flag.flag.is_mobile = 1;
        }
        else if(http_req_is_pc(http_info)) {
            http_info->request_flag.flag.is_pc = 1;
        }
    }

    if (http_req_is_html(http_info)) {
        http_info->request_flag.flag.is_html = 1;
    }
    else if (http_req_is_js(http_info)) {
        http_info->request_flag.flag.is_js = 1;
    }

    if (http_req_is_app(http_info)) {
        http_info->request_flag.flag.is_app = 1;
    }
    return 0;
}


void grint_dump_pkt_info(struct pkt_info_t *pkt_info)
{
    int32_t                     i = 0;
    struct http_info_t*         http_info = NULL;

    http_info = &pkt_info->http_info;

    /* http */
    if ((http_info->host_idx >= 0) && (http_info->host_idx < MAX_HEADERS)) {
        DEBUG_LOG("HTTP: host=[%s]", HTTP_HOST(http_info));
    }
    else {
        DEBUG_LOG("host_idx=[%d]", http_info->host_idx);
    }

    DEBUG_LOG("HTTP: path=[%s]", http_info->request_path);
    DEBUG_LOG("HTTP: query=[%s]", http_info->request_query);
    DEBUG_LOG("header number=%d",http_info->header_idx);

    if ((http_info->header_idx >= 0) && (http_info->header_idx <= MAX_HEADERS)) {
        for (i = 0; i < http_info->header_idx; i ++) {
            DEBUG_LOG("header[%d] key=[%s] value=[%s]",
                    i, http_info->headers[i].key, http_info->headers[i].value);
        }
    }
    /* http raw */
    grint_print_byte(pkt_info->http_info.http_raw, pkt_info->http_info.http_len);
    return;
}



int on_message_begin(http_parser* _) {
  (void)_;
  printf("\n***MESSAGE BEGIN***\n\n");
  return 0;
}


int on_headers_complete(http_parser* _) {
  (void)_;
  printf("\n***HEADERS COMPLETE***\n\n");
  return 0;
}

int on_message_complete(http_parser* _) {
  (void)_;
  printf("\n***MESSAGE COMPLETE***\n\n");
  return 0;
}

int on_url(http_parser* _, const char* at, size_t length) {
//  DEBUG_LOG("Url: %.*s\n", (int)length, at);

  struct http_parser_url    u;
  struct pkt_info_t         *pkt_info = NULL;


  if (length > MAX_URL_SIZE) {
      DEBUG_LOG("HTTP: url too long. length=[%zu]", length);
      return -1;
  }
  pkt_info = (struct pkt_info_t*)_->data;

  if(0 == http_parser_parse_url(at, length, 0, &u)) {
      if(u.field_set & (1 << UF_PATH)) {
          memcpy(pkt_info->http_info.request_path, at+u.field_data[UF_PATH].off,
                 u.field_data[UF_PATH].len);
          pkt_info->http_info.request_path[u.field_data[UF_PATH].len] = '\0';
      }
      if(u.field_set & (1 << UF_QUERY)) {
          memcpy(pkt_info->http_info.request_query, at+u.field_data[UF_QUERY].off,
                 u.field_data[UF_QUERY].len);
          pkt_info->http_info.request_query[u.field_data[UF_QUERY].len] = '\0';
      }
  }
  return 0;
}

int on_header_field(http_parser* _, const char* at, size_t length) {
//  printf("Header field: %.*s\n", (int)length, at);

  struct pkt_info_t* pkt_info = (struct pkt_info_t*)_->data;
  struct http_info_t* http_info = &pkt_info->http_info;

  if (length > MAX_HEADER_KEY_SIZE) {
      length = MAX_HEADER_VALUE_SIZE;
  }
  if (http_info->header_idx >= MAX_HEADERS) {
      DEBUG_LOG("HTTP: too many headers. header_idx=[%d]", http_info->header_idx);
      return 0;
  }

  memcpy(http_info->headers[http_info->header_idx].key, at, length);
  http_info->headers[http_info->header_idx].key[length] = '\0';
  if (strncasecmp(at, "Host", 4) == 0) {
      http_info->host_idx = http_info->header_idx;
  }
  else if (strncasecmp(at, "User-Agent", 10) == 0) {
      http_info->user_agent_idx = http_info->header_idx;
  }
  else if (strncasecmp(at, "Cookie", 6) == 0) {
      http_info->cookie_idx = http_info->header_idx;
  }

  return 0;
}

#if 0
static int make_request_ori_url(struct pkt_info_t* pkt_info)
{
    struct http_info_t* http_info = &pkt_info->http_info;

    snprintf(http_info->request_ori_url, sizeof(http_info->request_ori_url), "http://%s%s",
             HTTP_HOST(http_info), http_info->request_path);
    http_info->request_ori_url[sizeof(http_info->request_ori_url) - 1] = '\0';

    if (http_info->request_query[0] != '\0') {
        grint_strlcat(http_info->request_ori_url, "?", sizeof(http_info->request_ori_url));
        grint_strlcat(http_info->request_ori_url, http_info->request_query, sizeof(http_info->request_ori_url));
    }
    return 0;
}
#endif

int on_header_value(http_parser* _, const char* at, size_t length) {
//  printf("Header value: %.*s\n", (int)length, at);

  struct pkt_info_t* pkt_info = (struct pkt_info_t*)_->data;
  struct http_info_t* http_info = &pkt_info->http_info;

  if (length > MAX_HEADER_VALUE_SIZE) {
      length = MAX_HEADER_VALUE_SIZE;
  }
  if (http_info->header_idx >= MAX_HEADERS) {
      DEBUG_LOG("HTTP: too many headers. header_idx=[%d]", http_info->header_idx);
      return 0;
  }

  memcpy(http_info->headers[http_info->header_idx].value, at, length);
  http_info->headers[http_info->header_idx].value[length] = '\0';

  if (http_info->header_idx == http_info->host_idx) {
//      make_request_ori_url(pkt_info);
  }

  http_info->header_idx++;
  return 0;
}

int on_body(http_parser* _, const char* at, size_t length) {
  (void)_;
  printf("Body: %.*s\n", (int)length, at);
  return 0;
}


int32_t grint_handle_http_init(http_parser_settings* settings)
{
    memset(settings, 0, sizeof(http_parser_settings));
    settings->on_url = on_url;
    settings->on_header_field = on_header_field;
    settings->on_header_value = on_header_value;
//    settings->on_message_begin = on_message_begin;
//    settings->on_headers_complete = on_headers_complete;
//    settings->on_body = on_body;
//    settings->on_message_complete = on_message_complete;
    return 0;
}

/*****************************************************************
 * Description:
 *  解析http报文
 *
 * Parameter:
 *  @grint : grint结构
 *  @pkt_info : 报文信息
 *
 * Return:
 *  0: 合法报文
 *  -1: 非法报文
 *****************************************************************/
int32_t grint_handle_http(void* ctx, struct pkt_info_t *pkt_info)
{
    grint_engine_t      *grint_engine = (grint_engine_t*)ctx;
    size_t              nparsed = 0;
    http_parser         parser;

    pkt_info->http_info.host_idx = -1;
    pkt_info->http_info.user_agent_idx = -1;
    pkt_info->http_info.cookie_idx = -1;

    http_parser_init(&parser, HTTP_REQUEST);
    parser.data = pkt_info;
    nparsed = http_parser_execute(&parser, &grint_engine->settings,
                                  (const char*)pkt_info->http_info.http_raw, pkt_info->http_info.http_len);

    if (nparsed != pkt_info->http_info.http_len) {
        DEBUG_LOG("nparsed[%zu] != pkt_info->http_info.http_len[%d]", nparsed, pkt_info->http_info.http_len);
        grint_dump_pkt_info(pkt_info);
        return -1;
    }

    /* make sure find host */
    if (pkt_info->http_info.host_idx < 0 || pkt_info->http_info.host_idx >= MAX_HEADERS) {
        DEBUG_LOG("host_idx[%d] error.", pkt_info->http_info.host_idx);
        return -1;
    }
    return 0;
}
