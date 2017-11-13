/***************************************************************
* 名    称: http_main.c
* 创 建 者: 付红竹
* 创建日期: 2017年10月31日
* 描    述:
* 修改记录:
***************************************************************/
#include "http_proxy.h"
#include "http_handler.h"
#include "http_parser.h"
#include "pkt_parser.h"
#include "http_keepalive.h"

static int g_nLocalListenSock;
static int nMaxSocket;
static fd_set SockNewSet;

grint_engine_t grint_engine;


/**
 * @brief ShowVersion
 * @param prog
 */
static void ShowVersion(char *prog)
{
    fprintf(stderr, "%s 当前版本为V1.0.0\n", prog);
}

void Usage(char *prog)
{
    fprintf(stderr,"HTTP 进程\n");
    fprintf(stderr,"%s start 启动程序\n"
                   "%s stop  关闭程序\n"
                   "%s -v    显示版本信息\n",prog,prog,prog);
}

int LoadConfigInfo()
{

    return 0;
}


void recv_data(int nSockFd)
{
    char                    szBuffer[1024];
    int                     nReturnValue;
    int                     response_len;
    struct pkt_info_t       pkt_info;
    request_flag_t          *request_flag = NULL;
    char                    *remote_cfg_root = NULL;
    remote_cfg_err_t        result = config_ok;


    DEBUG_LOG("新的连接SockFd[%d]", nSockFd);
    nReturnValue = recv(nSockFd, szBuffer, sizeof(szBuffer), 0);
    if (nReturnValue == 0)
    {
        DEBUG_LOG("socket[%d] server close socker!!!", nSockFd);
        close(nSockFd);

        FD_CLR(nSockFd, &SockNewSet);

        if(nSockFd == nMaxSocket)
            nMaxSocket--;

        return;
    }
//    printf("buf[\n%s\n]", szBuffer);

    memset(&pkt_info, 0, sizeof(struct pkt_info_t));
    pkt_info.http_info.http_raw = szBuffer;
    pkt_info.http_info.http_len = strlen(szBuffer);
    nReturnValue = grint_handle_http(&grint_engine, &pkt_info);
    if (nReturnValue != 0)
    {
        DEBUG_LOG("解析失败[%d]" , nReturnValue);
        return;
    }
    grint_dump_pkt_info(&pkt_info);
    grint_get_request_flag(&pkt_info.http_info);

    request_flag = &pkt_info.http_info.request_flag;
//    DEBUG_LOG("UserAgent=<%s>", HTTP_USER_AGENT(&pkt_info.http_info));
    DEBUG_LOG("PC:%d  MOBILE:%d  HTML:%d  JS:%d  APP:%d  ANDROID:%d  IOS:%d",
                       request_flag->flag.is_pc,
                       request_flag->flag.is_mobile,
                       request_flag->flag.is_html,
                       request_flag->flag.is_js,
                       request_flag->flag.is_app,
                       request_flag->flag.is_android,
                       request_flag->flag.is_ios);

    //is terminal device supported
    if (!is_terminal_device_supported(&pkt_info)) {
        DEBUG_LOG("[%s], terminal device unsupported! User_Agent = <%s>.",
                  inet_ntoa(pkt_info.ip_src),
                  HTTP_USER_AGENT(&pkt_info.http_info));
    }
    while ((result = _config_get_remote(&remote_cfg_root, &response_len))) {
        if (result == remote_cfg_not_exist) {
            DEBUG_LOG("remote config not exist, will set default.");
//            remote_cfg_root = config_set_default();
            break;
        }
//        else {
//            DEBUG_LOG("get remote config fail: %d, retrying...", result);
//            sleep(3);
//            continue;
//        }
        break;
    }
    DEBUG_LOG("remote_cfg_root\n%s", remote_cfg_root);
    DEBUG_LOG("len[%d]", response_len);
    send(nSockFd, remote_cfg_root, response_len, 0);





    return;
}

static void DealReq(void)
{
    int                     nAcceptSock = 0;
    int                     nReturnValue = 0;
    int                     nSocketFd;
    fd_set                  SockSet;
    struct timeval          struTimeOut;
    struct sockaddr_in      strClientAddr;
    socklen_t               nClientAddrLen;
    char                    szClientAddr[INET_ADDRSTRLEN];


    FD_ZERO(&SockNewSet);
    FD_SET(g_nLocalListenSock, &SockNewSet);
    nMaxSocket = g_nLocalListenSock;
    nClientAddrLen = sizeof(strClientAddr);
    while(1)
    {
        memcpy(&SockSet, &SockNewSet, sizeof(fd_set));
        struTimeOut.tv_sec=2;
        struTimeOut.tv_usec=0;
        nReturnValue = select(nMaxSocket+1, &SockSet, NULL, NULL, &struTimeOut);
        if (nReturnValue > 0)
        {
            if(FD_ISSET(g_nLocalListenSock, &SockSet))
            {
                nAcceptSock = accept(g_nLocalListenSock, (struct sockaddr*)&strClientAddr, &nClientAddrLen);
                inet_ntop(AF_INET, &strClientAddr.sin_addr, szClientAddr, sizeof(szClientAddr));
                DEBUG_LOG("received from %s at PORT %d", szClientAddr, ntohs(strClientAddr.sin_port));
                FD_SET(nAcceptSock, &SockNewSet);
                if (nAcceptSock > nMaxSocket)
                   nMaxSocket = nAcceptSock;

                FD_CLR(g_nLocalListenSock, &SockSet);
            }

            for (nSocketFd = 3; nSocketFd<=nMaxSocket; nSocketFd++)
            {
                if(FD_ISSET(nSocketFd, &SockSet))
                {
                    recv_data(nSocketFd);
                }
            }
        }
    }
}

int main(int args, char *argv[])
{
    int         nReturnValue;
    int         nResCode = 0;


    if (args !=2 )
    {
        Usage(argv[0]);
        return -1;
    }


    if (strcmp(argv[1], "-v") == 0)
    {
        ShowVersion(argv[0]);
        return 0;
    }
    if(strcmp(argv[1],"stop")==0)
    {
        StopProg(argv[0]);
        return 0;
    }
    if(strcmp(argv[1],"start")!=0)
    {
        Usage(argv[0]);
        return 0;
    }
    if(TestProgStat(argv[0])==0)
    {
        fprintf(stderr,"[1;31mHTTP 进程已经启动[0;37m\n");
        return -1;
    }
//    if(DaemonStart()!=0)
//    {
//        fprintf(stderr,"HTTP 进程进入后台运行错误\n");
//        return -1;
//    }
    if(CreateIdFile(argv[0])!=0)
    {
        DEBUG_LOG("创建程序ID文件错误");
        return -1;
    }

    memset(&grint_engine, 0, sizeof(grint_engine_t));
    grint_engine.launch_time = time(NULL);

    /* 加载配置信息 */
    nReturnValue = LoadConfigInfo();
    if(nReturnValue != 0)
    {
        DEBUG_LOG("读取配置参数失败[%d]" , nReturnValue);
        return nReturnValue;
    }

    //init http parser
    grint_handle_http_init(&grint_engine.settings);

    nReturnValue = CreateListenSocket(NULL, 8080, &nResCode);
    if(nReturnValue <= 0)
    {
        DEBUG_LOG("创建监听失败[%d]", nReturnValue);
        return nReturnValue;
    }
    g_nLocalListenSock = nReturnValue;


    DEBUG_LOG("LocalListenSock[%d]", g_nLocalListenSock);
    DealReq();


    return 0;
}
