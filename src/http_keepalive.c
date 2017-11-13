/***************************************************************
* 名    称: http_keepalive.c
* 创 建 者: 付红竹
* 创建日期: 2017年11月08日
* 描    述:
* 修改记录:
***************************************************************/

#include "http_keepalive.h"

int32_t _config_get_remote(char **remote_cfg_root, int *pResponseLen)
{
    remote_cfg_err_t err = config_ok;
    int response_len = 0;
    char *response_body = NULL;
    char *response_header = NULL;
    char *message = NULL;
    int status_code = 0;
//    char *url = NULL;
    int url_len = 0;
    http_handler_t *http_request = NULL;


    char *url = "http://10.0.0.173:8080/index.php/admin/login";
    url_len = strlen(url);

    //get
    if ((http_request = http_request_init()) == NULL) {
        DEBUG_LOG("http_request_init() error!");
        goto End;
    }
    if (http_set_url(http_request, url) == -1) {
        DEBUG_LOG("http_set_url() error!");
        goto End;
    }
    if (http_set_type(http_request, http_type_get) == -1) {
        DEBUG_LOG("http_set_type() error!");
        goto End;
    }
    if (http_set_timeout(http_request, http_connect_timeout, 3L) == -1) {
        DEBUG_LOG("http_set_timeout() error!");
        goto End;
    }
    if (http_set_timeout(http_request, http_process_timeout, 3L) == -1) {
        DEBUG_LOG("http_set_timeout() error!");
        goto End;
    }

    http_prepare(http_request);
    if (http_process(http_request) != 0) {
        DEBUG_LOG("http_process() error: %s\n", http_get_error(http_request));
        err = http_process_err;
        goto End;
    }


    status_code = http_status_code(http_request);
    if (status_code == 400) {
//        DEBUG_LOG("system_id = %s, remote_cfg not exist!",
//                  grint_engine_cfg.system_id);
        DEBUG_LOG("remote_cfg not exist!");
        err = remote_cfg_not_exist;
        goto End;
    } else if (status_code != 200) {
        DEBUG_LOG("get remote config error: status_code = %d",
                  status_code);
        err = status_code_not_ok;
        goto End;
    }

    response_len = http_get_header_len(http_request);
    response_header = (char*)malloc(response_len+1);
    if (response_header == NULL) {
        DEBUG_LOG("response_body malloc fail");
        return -1;
    }
    memset(response_header, 0, response_len + 1);
    strncpy(response_header, http_get_header(http_request), response_len);
//    DEBUG_LOG("response_header[%s]", response_header);

    response_len = http_get_body_len(http_request);
    response_body = (char*)malloc(response_len + 1);
    if (response_body == NULL) {
        DEBUG_LOG("response_body malloc fail");
        return -1;
    }
    memset(response_body, 0, response_len + 1);
    strncpy(response_body, http_get_body(http_request), response_len);
//    DEBUG_LOG("response_body[%s]", response_body);

//    *remote_cfg_root = cJSON_Parse(response_body);
//    if (*remote_cfg_root == NULL) {
//        DEBUG_LOG("get remote config error: parse response data error!");
//        err = parse_response_err;
//        goto End;
//    }
//    DEBUG_LOG("data:%s\n",cJSON_Print(*remote_cfg_root));
    message = (char*)malloc(http_get_header_len(http_request) + http_get_body_len(http_request));
    memcpy(message, response_header, http_get_header_len(http_request));
    memcpy(message+http_get_header_len(http_request), response_body, http_get_body_len(http_request));
    *remote_cfg_root = message;
    *pResponseLen = http_get_header_len(http_request) + http_get_body_len(http_request);
//    DEBUG_LOG("remote_cfg_root\n%s", remote_cfg_root);


End:
//    if (url) {
//        free(url);
//        url = NULL;
//    }

    if (http_request) {
        http_clean(http_request);
        http_request_destroy(http_request);
        http_request = NULL;
    }

    if (response_body) {
        free(response_body);
        response_body = NULL;
    }

    return err;
}
