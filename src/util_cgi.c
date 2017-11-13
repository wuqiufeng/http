/***************************************************************
* 名    称: util_cgi.c
* 创 建 者: 付红竹
* 创建日期: 2017年10月31日
* 描    述:
* 修改记录:
***************************************************************/
#include "http_proxy.h"


//void grint_print_byte(const int8_t* begin, uint16_t len)
//{
//    int i;
//    int8_t* pointer = (int8_t*)begin;
//    for(i=0; i<len; i++, pointer++)
//        printf("%c", *pointer);
//    printf("\n");
//}

/**
 * CreateIdFile
 * 创建进程号记录文件
 *
 * pszProgName 要创建的进程文件名称
 *
 * Returns 成功0 失败 -1
 */
int CreateIdFile(char* pszProgName)
{
    char            szBaseName[MAX_PATH_LEN];
    char            szFileName[MAX_PATH_LEN];
    FILE            *pstruFile;

    strcpy(szBaseName,pszProgName);
    GetBaseNameFromPath(szBaseName);

    snprintf(szFileName,sizeof(szFileName),"%s/.%sid",getenv("HOME"),szBaseName);
    pstruFile=fopen(szFileName,"w");
    if(pstruFile==NULL)
    {
        DEBUG_LOG("CreateIdFile[%s] 错误![%d],[%s]",szFileName,errno,strerror(errno));
        return -1;
    }
    fprintf(pstruFile,"%d",getpid());
    fclose(pstruFile);

    chmod(szFileName,0600);

    return 0;
}


/**
 * StopProg
 * 停止进程的执行
 *
 * pszProgName 要停止的进程名称
 *
 * Returns 无
 */
void StopProg(char* pszProgName)
{
    char                        szBaseName[MAX_PATH_LEN];
    FILE                        *pstruFile;
    char                        szFileName[MAX_PATH_LEN];
    unsigned int                nProgId;

    strcpy(szBaseName,pszProgName);
    GetBaseNameFromPath(szBaseName);

    snprintf(szFileName,sizeof(szFileName),"%s/.%sid",getenv("HOME"),szBaseName);

    pstruFile=fopen(szFileName,"r");
    if(pstruFile==NULL)
    {
        fprintf(stderr,"关闭%s失败.可能是程序没有启动或者是您没有权限关闭\n",pszProgName);
        return;
    }
    fscanf(pstruFile,"%d",&nProgId);
    if(kill(nProgId,SIGTERM)==-1)
    {
        sleep(1);
        if(kill(nProgId,SIGKILL)==-1)
        {
            fprintf(stderr,"关闭%s失败.可能是程序没有启动或者是您没有权限关闭\n",pszProgName);
        }
    }
    else
    {
        fprintf(stderr,"关闭%s成功\n",pszProgName);
    }
    fclose(pstruFile);
    unlink(szFileName);
}


/**
 * TestProgStat
 * 判断进程是否启动
 *
 * pszProgName 要判断的进程名称
 *
 * Returns 进程已经启动返回0 否则返回-1
 */
int TestProgStat(char* pszProgName)
{
    char                    szBaseName[MAX_PATH_LEN];
    char                    szFileName[MAX_PATH_LEN];
    char                    szShellCmd[MAX_STR_LEN];
    unsigned int            nProgId;
    unsigned int            nTempLen;
    FILE                    *pstruFile;

    strcpy(szBaseName,pszProgName);
    GetBaseNameFromPath(szBaseName);

    snprintf(szFileName,sizeof(szFileName),"%s/.%sid",getenv("HOME"),szBaseName);

    pstruFile=fopen(szFileName, "r");
    if(pstruFile==NULL)
        return -1;

    nProgId=0;
    fscanf(pstruFile,"%d",&nProgId);
    fclose(pstruFile);
    if(nProgId<1)
        return -1;

    memset(szShellCmd,0,sizeof(szShellCmd));
    snprintf(szShellCmd,sizeof(szShellCmd),"ps -e|awk '$1 == %d {print $4}'",nProgId);

    pstruFile=popen(szShellCmd,"r");
    if(pstruFile==NULL)
        return -1;

    fgets(szFileName,sizeof(szFileName),pstruFile);
    fclose(pstruFile);
    nTempLen=strlen(szFileName);
    if(nTempLen<1)
        return -1;
    szFileName[nTempLen-1] = '\0';
    if(strcmp(szFileName,szBaseName)==0)
        return 0;

    return -1;
}

/**
 * DaemonStart
 * 将进程变为后台起动进程
 *
 * Returns 进程启动返回0 否则返回-1
 */
int DaemonStart()
{
    register int nChildProgId;

    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    signal(SIGCHLD,SIG_IGN);

    nChildProgId=fork();
    if(nChildProgId<0)
    {
//        ERROR_LOG("创建子进程错误 %s\n",strerror(errno));
        fprintf(stderr, "创建子进程错误 %s\n",strerror(errno));
        return -1;
    }
    else if(nChildProgId>0)
        exit(0);

    if(setpgrp()==-1)
    {
//        ERROR_LOG("setpgrp 设置进程组错误 %s\n",strerror(errno));
        fprintf(stderr, "setpgrp 设置进程组错误 %s\n", strerror(errno));
        exit(0);
    }
    signal(SIGHUP,SIG_IGN);

    if((nChildProgId=fork())<0)
    {
//        ERROR_LOG("创建第2个子进程错误 %s\n",strerror(errno));
        fprintf(stderr, "创建第2个子进程错误 %s\n",strerror(errno));
        exit(0);
    }
    else if(nChildProgId>0)
        exit(0);

    chdir("/");
    umask(0);

    return 0;
}
