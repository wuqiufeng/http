/***************************************************************
* 名    称: http_proxy.h
* 创 建 者: 付红竹
* 创建日期: 2017年10月31日
* 描    述:
* 修改记录:
***************************************************************/

#ifndef __HTTP_PROXY_H__
#define __HTTP_PROXY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#include <sys/select.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "http_parser.h"
#include "cJSON.h"

#define MAX_PATH_LEN            2048
#define MAX_STR_LEN             512
#define MAX_HEADERS             20
#define MAX_URL_SIZE            2048
#define MAX_HEADER_KEY_SIZE     128
#define MAX_HEADER_VALUE_SIZE   512

typedef enum
{
    config_ok               = 0,
    remote_cfg_not_exist    = 601,
    http_request_init_err,
    http_set_url_err,
    http_set_type_err,
    http_set_connect_timeout_err,
    http_set_process_timeout_err,
    http_process_err,
    status_code_not_ok,
    parse_response_err
} remote_cfg_err_t;

typedef struct _grint_engine_t {
//    pthread_rwlock_t plugins_rwlock;
//    cap_send_hdl_t *cs_handlers;
//    grint_engine_stats_t grint_stats;
    http_parser_settings settings;
//    threadpool worker_thpool;
    time_t launch_time;
} grint_engine_t;


extern grint_engine_t grint_engine;


int CreateIdFile(char* pszProgName);
void StopProg(char* pszProgName);
char* GetBaseNameFromPath(char* pszPath);
int TestProgStat(char* pszProgName);
int DaemonStart();

int CreateListenSocket(char* pszIpAddr,unsigned int nPort,int *pnResCode);


//__FUNCTION__
#define DEBUG_LOG(format, args... ) { \
    struct tm           *struTime = NULL;\
    time_t              t = 0;\
    time(&t);\
    struTime = localtime(&t);\
    printf("[%04d-%02d-%02d-%02d:%02d:%02d] [%s %d]"format"\n",\
            struTime->tm_year+1900,\
            struTime->tm_mon+1,\
            struTime->tm_mday,\
            struTime->tm_hour,\
            struTime->tm_min,\
            struTime->tm_sec,\
            __FILE__ , __LINE__, ##args); }
#endif /* __HTTP_PROXY_H__ */
