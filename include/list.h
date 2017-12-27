#ifndef __LIST_H__
#define __LIST_H__

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagLISTSTRU
{
    void * pData;			/* data */
    struct tagLISTSTRU * pstruPrev;	/* reserve */
    struct tagLISTSTRU * pstruNext;	/* next */
}LISTSTRU;
typedef LISTSTRU * PLISTSTRU;

typedef int (*CmpListDataFunc)(void * pListData,void * pUserData);

int CreateList(PLISTSTRU pstruList);
int DeleteList(PLISTSTRU pstruList, int bFree);
int InsertIntoList(PLISTSTRU pstruList, void * pData);
int DeleteInList(PLISTSTRU pstruList, void * pData);
int DemandInList(PLISTSTRU pstruList, void * *pData,CmpListDataFunc CmpFunc, void * pUserData);
int InsertIntoListByOrder(PLISTSTRU pstruList,CmpListDataFunc CmpFunc, void * pData);
int DeleteFromListByObj(PLISTSTRU pstruList,CmpListDataFunc CmpFunc, void * pDeleteData);
int GetListCount(PLISTSTRU pstruList);
int GetPosFromList(PLISTSTRU pstruList,int *pnPos,CmpListDataFunc CmpFunc, void * pUserData);
int InsertIntoListBeforePos(PLISTSTRU pstruList, void * pData,int nPos);
/*
 * 函数定义
 */
#ifdef __cplusplus
}
#endif

#endif
