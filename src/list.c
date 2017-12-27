/***
 * @文件名称: list.c
 * @系统名称:
 * @功能说明: 列表操作函数
 *  2012-06-06 创建
 */
#include "list.h"

/**
 * @函数功能: 创建列表
 * @pstruList: 要求创建的列表
 * @返回说明: 成功操作返回0 失败操作返回-1
 */
int CreateList(PLISTSTRU pstruList)
{
    if ( !pstruList)
        return -1;

    pstruList->pData=NULL;
    pstruList->pstruNext=NULL;
    pstruList->pstruPrev=NULL;
    return 0;
}

/**
 * @函数功能: 删除列表
 * @pstruList: 要求删除的列表
 * @bFree: 是否要求释放列表中的pData数据 1要求释放
 * @返回说明: 成功操作返回0 失败操作返回-1
 */
int DeleteList(PLISTSTRU pstruList,int bFree)
{
    PLISTSTRU pstruCur;
    PLISTSTRU pstruHeader;


    if ( !pstruList)
        return -1;

    pstruHeader = pstruList;
    pstruCur=pstruList->pstruNext;
    while(pstruCur)
    {
        pstruList=pstruCur->pstruNext;
        if(bFree)
        {
            free(pstruCur->pData);
        }
        free(pstruCur);
        pstruCur=pstruList;
    }
    pstruHeader->pstruNext = NULL;
    return 0;
}

/**
 * @函数功能: 插入列表
 * @pstruList: 要求插入的列表
 * @pData: 要求插入的数据
 * @返回说明: 成功操作返回0 失败操作返回-1
 */
int InsertIntoList(PLISTSTRU pstruList,void * pData)
{
    PLISTSTRU pstruNew;

    if ( !pstruList)
        return -1;

    while(pstruList->pstruNext)
        pstruList=pstruList->pstruNext;

    pstruNew=malloc(sizeof(LISTSTRU));
    if (!pstruNew)
        return-1;

    pstruNew->pData=pData;
    pstruNew->pstruNext=NULL;
    pstruNew->pstruPrev=pstruList;
    pstruList->pstruNext=pstruNew;
    return 0;
}

/**
 * @函数功能: 删除列表中数据
 * @pstruList: 要删除的列表
 * @pData: 要删除的元素
 * @返回说明: 成功操作返回0 失败操作返回-1
 */
int DeleteInList(PLISTSTRU pstruList,void* pData)
{
    PLISTSTRU pstruCur;

    if ( !pstruList)
        return -1;
    pstruCur=pstruList->pstruNext;
    while(pstruCur)
    {
        if(pstruCur->pData==pData)
        {
            pstruList->pstruNext=pstruCur->pstruNext;
            if(pstruList->pstruNext)
                pstruList->pstruNext->pstruPrev=pstruList;
            free(pstruCur);
            return 0;
        }
        pstruList=pstruCur;
        pstruCur=pstruList->pstruNext;
    }
    return -1;
}


/**
 * @函数功能: 从列表中获取数据
 * @pstruList: 要获取数据的列表
 * @pData: 数据的保存地址
 * @CmpFunc: 数据比较函数
 * @pUserData: 数据比较函数参数
 * @返回说明: 成功操作返回0 失败操作返回-1
 */
int DemandInList(PLISTSTRU pstruList,void* *pData,CmpListDataFunc CmpFunc,void * pUserData)
{
    PLISTSTRU pstruCur;
    /*DETECT_NULL(pData);*/


    if ( !pstruList || !CmpFunc)
        return -1;

    pstruCur=pstruList->pstruNext;
    while(pstruCur)
    {
        if(CmpFunc(pstruCur->pData,pUserData)==0)
        {
            *pData=pstruCur->pData;
            return 0;
        }
        pstruCur=pstruCur->pstruNext;
    }
    return -1;
}


/**
 * @函数功能: 根据任务节点的优先级插入节点
 * @pstruList: 要获取数据的列表
 * @pData: 数据的保存地址
 * @CmpFunc: 数据比较函数
 * @pUserData: 数据比较函数参数
 * @返回说明: 成功操作返回0 失败操作返回-1
 */
int InsertIntoListByOrder(PLISTSTRU pstruList,CmpListDataFunc CmpFunc,void* pInsertData)
{
    PLISTSTRU pstruCur;
    PLISTSTRU pstruNew;

    if ( !pstruList || !CmpFunc)
        return -1;
    pstruCur = pstruList->pstruNext;
    while(pstruCur)
    {
        if(CmpFunc(pstruCur->pData,pInsertData) == 0)
        {
            pstruNew=malloc(sizeof(LISTSTRU));
            if (!pstruNew)
                return-1;
            pstruNew->pData=pInsertData;
            pstruNew->pstruPrev=pstruCur->pstruPrev;
            pstruCur->pstruPrev->pstruNext=pstruNew;
            pstruNew->pstruNext=pstruCur;
            pstruCur->pstruPrev=pstruNew;
            return 0;
        }
        pstruCur=pstruCur->pstruNext;
    }
    InsertIntoList(pstruList,pInsertData);

    return 0;
}


/**
 * @函数功能: 根据任务节点的任务id移除该该节点
 * @pstruList: 要获取数据的列表
 * @pData: 数据的保存地址
 * @CmpFunc: 数据比较函数
 * @pUserData: 数据比较函数参数
 * @返回说明: 成功操作返回0 失败操作返回-1
 */
int DeleteFromListByObj(PLISTSTRU pstruList,CmpListDataFunc CmpFunc,void* pDeleteData)
{
    PLISTSTRU pstruCur;


    if ( !pstruList)
        return -1;

    pstruCur=pstruList->pstruNext;
    while(pstruCur)
    {
        if(CmpFunc(pstruCur->pData,pDeleteData)== 0)
        {
            pstruList->pstruNext=pstruCur->pstruNext;
            if(pstruList->pstruNext)
                pstruList->pstruNext->pstruPrev=pstruList;
            free(pstruCur->pData);
            free(pstruCur);
            return 0;
        }
        pstruList=pstruCur;
        pstruCur=pstruList->pstruNext;
    }
    return -1;
}


/*********************************************
*函数名称：int GetListCount(PLISTSTRU pstruList)

*功能描述：遍历结点，计算结点总数

*参数说明：

*返回值：

*********************************************/
int GetListCount(PLISTSTRU pstruList)
{
    int nNodeCount=0;

    if ( !pstruList)
        return -1;

    while(pstruList->pstruNext)
    {
        nNodeCount++;
        pstruList=pstruList->pstruNext;
    }
    return nNodeCount;
}


/**
 * @函数功能: 从列表中获取数据的位置
 * @pstruList: 要获取数据的列表
 * @pnPos: 数据的位置地址
 * @CmpFunc: 数据比较函数
 * @pUserData: 数据比较函数参数
 * @返回说明: 成功操作返回0 失败操作返回-1
 */
int GetPosFromList(PLISTSTRU pstruList,int *pnPos,CmpListDataFunc CmpFunc,void* pUserData)
{
    PLISTSTRU pstruCur;
    int nNodeCount=0;

    if ( !pstruList || !CmpFunc)
        return -1;
    pstruCur=pstruList->pstruNext;
    while(pstruCur)
    {
        nNodeCount++;
        if(CmpFunc(pstruCur->pData,pUserData)==0)
        {
            *pnPos=nNodeCount;
            return 0;
        }
        pstruCur=pstruList->pstruNext;
    }
    return 0;
}


/**
 * @函数功能: 插入列表
 * @pstruList: 要求插入的列表
 * @pData: 要求插入的数据
 * @返回说明: 成功操作返回0 失败操作返回-1
 */
int InsertIntoListBeforePos(PLISTSTRU pstruList,void* pData,int nPos)
{
    PLISTSTRU pstruNew;
    int nIndex=1;
    PLISTSTRU pstruCur=NULL;
    if ( !pstruList)
        return -1;
    pstruCur=pstruList;
    if (nPos == 0)
    {
        return -1;
    }
    else
    {
        if (nPos>=1 && nPos<=GetListCount(pstruList))
        {
            while(pstruCur->pstruNext!=NULL)
            {
                if(nPos == nIndex)
                    break;
                nIndex++;
                pstruCur=pstruCur->pstruNext;
            }
            pstruNew=malloc(sizeof(LISTSTRU));
            if (!pstruNew)
                return-1;
            pstruNew->pData=pData;
            pstruNew->pstruPrev=pstruCur->pstruPrev;
            pstruCur->pstruPrev->pstruNext=pstruNew;
            pstruNew->pstruNext=pstruCur;
            pstruCur->pstruPrev=pstruNew;
        }
    }
    return 0;
}


/**
 * @函数功能: 从列表中查找节点
 * @pstruList: 要获取数据的列表
 * @CmpFunc: 数据比较函数
 * @pUserData: 数据比较函数参数
 * @返回说明: 成功操作返回0 失败操作返回-1
 */
int FindInList(PLISTSTRU pstruList,CmpListDataFunc CmpFunc,void* pUserData)
{
    PLISTSTRU pstruCur;

    if ( !pstruList || !CmpFunc)
        return -1;
    pstruCur=pstruList->pstruNext;
    while(pstruCur)
    {
        if(CmpFunc(pstruCur->pData,pUserData)==0)
        {
            return 0;
        }
        pstruCur=pstruCur->pstruNext;
    }
    return -1;
}
