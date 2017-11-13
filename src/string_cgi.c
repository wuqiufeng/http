/***************************************************************
* 名    称: string_cgi.c
* 创 建 者: 付红竹
* 创建日期: 2017年10月31日
* 描    述:
* 修改记录:
***************************************************************/
#include "http_proxy.h"

char* GetBaseNameFromPath(char* pszPath)
{
#ifdef SYSTEM_LINUX
    return basename(pszPath);
#else
    int             nLen;
    int             i;

    nLen=strlen(pszPath);
    for(i=nLen-1;i>=0;i--)
    {
        if(pszPath[i]=='/')
        {
            memcpy(pszPath,pszPath+i+1,nLen-i);
            break;
        }
    }

    return pszPath;
#endif
}
