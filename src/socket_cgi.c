/***************************************************************
* 名    称: socket_cgi.c
* 创 建 者: 付红竹
* 创建日期: 2017年10月31日
* 描    述:
* 修改记录:
***************************************************************/

#include "http_proxy.h"

/**
 * CreateListenSocket
 * 创建监听套接字
 *
 * pszIpAddr	服务器的IP地址,如果为空则监听所有地址
 * nPort		服务器监听的端口号
 *
 * Returns	成功 监听套接字 失败-1
 */
int CreateListenSocket(char* pszIpAddr,unsigned int nPort,int *pnResCode)
{
    int                     nListenSock;
    int                     nVal=1;
    int                     nRcvBufSize=64*1024;
    struct sockaddr_in      struInAddr;
    struct hostent          *pstruHost = NULL;


    //int nRcvBufSize=1024*1024*100*2;
    memset(&struInAddr, 0, sizeof(struInAddr));
    if(pszIpAddr)
    {
        if((inet_aton(pszIpAddr,(struct in_addr *)&struInAddr.sin_addr))==0)
        {
            if((pstruHost=gethostbyname(pszIpAddr))==NULL)
            {
                DEBUG_LOG("获取服务器IP地址错误! 错误代码[%d] 错误信息[%s]",
                          h_errno,hstrerror(h_errno));
                *pnResCode=h_errno;
                return -1;
            }
            struInAddr.sin_addr=*(struct in_addr *)pstruHost->h_addr_list[0];
        }
    }
    else
        struInAddr.sin_addr.s_addr=INADDR_ANY;

    struInAddr.sin_port=htons(nPort);
    struInAddr.sin_family=AF_INET;

    if((nListenSock=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        DEBUG_LOG("socket 创建错误!\n\t错误代码=[%d] 错误信息=[%s]",
                  errno,strerror(errno));
        *pnResCode=errno;
        return -1;
    }
    if(setsockopt(nListenSock,SOL_SOCKET,SO_REUSEADDR,&nVal,sizeof(nVal))<0)
    {
        DEBUG_LOG("setsockopt 错误! 错误代码[%d] 错误信息[%s]",
                  errno,strerror(errno));
        close(nListenSock);
        *pnResCode=errno;
        return -1;
    }

    if(setsockopt(nListenSock,SOL_SOCKET,SO_RCVBUF,&nRcvBufSize,sizeof(nRcvBufSize))<0)
    {
        DEBUG_LOG("setsockopt 错误! 错误代码[%d] 错误信息[%s]",
                  errno,strerror(errno));
        close(nListenSock);
        *pnResCode=errno;
        return -1;
    }

    if(bind(nListenSock,(struct sockaddr *)&struInAddr,sizeof(struct sockaddr))==-1)
    {
        DEBUG_LOG("bind 捆绑地址错误! 错误代码[%d] 错误信息[%s]",
                  errno,strerror(errno));
        close(nListenSock);
        *pnResCode=errno;
        return -1;
    }
    if(listen(nListenSock,SOMAXCONN)==-1)
    {
        DEBUG_LOG("listen 监听端口错误! 错误代码[%d] 错误信息[%s]",
                  errno,strerror(errno));
        close(nListenSock);
        *pnResCode=errno;
        return -1;
    }

    return nListenSock;
}
