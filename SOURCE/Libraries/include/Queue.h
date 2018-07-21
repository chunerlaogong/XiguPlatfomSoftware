/*
********************************************************************************
*
*                                 Queue.h
*
* File          : Queue.h
* Version       : V1.0
* Author        : whq
* Mode          : Thumb2
* Toolchain     : 
* Description   : 队列操作头文件
*                
* History       :
* Date          : 2013.07.22
*******************************************************************************/


#ifndef _QUEUE_H_
#define _QUEUE_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#include "stm8l15x.h"

//队列缓存中最小剩余空间
#define QUEUE_BUFF_LIMIT        24



#define DEBUG_FULL_ASSERT       0



#if     DEBUG_FULL_ASSERT
#define ASSERT_PARAM(a)          ((a) ? (void)0 : ASSERT_FAILED((uint8_t *)__FILE__, __LINE__))
void ASSERT_FAILED(uint8_t* file, uint32_t line);
#else
#define ASSERT_PARAM(a)           
#endif



typedef enum {
    eNORMAL,                //数据正常
    eERROR_HEAD,            //数据头异常
    eERROR_CMD,             //命令字异常
    eERROR_LEN,             //长度异常
    eERROR_VERIFY,          //效验异常
}QUEUE_STATUS_t;            //正则函数返回值类型



typedef volatile struct {
    volatile uint32_t   bufSize;        //缓冲区总大小
    volatile uint8_t    *pStart;        //起始地址
    volatile uint8_t    *pEnd;          //结束地址
    volatile uint8_t    *pBuf;          //缓冲区首地址
}QUEUE8_t;


typedef volatile struct {
    volatile uint32_t   elemSize;       //结构体单元大小
    volatile uint32_t   sumCount;       //结构体单元的最大个数
    volatile uint32_t   start;          //结束结构体地址
    volatile uint32_t   end;            //缓冲区首地址
    volatile void       *pBuf;          //起始结构体地址
}QUEUE_STRUCT_t;



//正则函数要求:
//数据不满足要求时，立即返回，并返回舍弃的数据长度
//取数据正则函数类型
typedef uint32_t QUEUE_SpecifiedRulesCB(uint8_t *pData, uint32_t dataLen, QUEUE_STATUS_t *result);

void QUEUE_Reset(QUEUE8_t *pQ8, uint8_t *pBuf, uint32_t bufSize);
uint16_t QUEUE_GetDataSize(QUEUE8_t *pQ8);

uint32_t QUEUE_PacketCreate(QUEUE8_t *pQ8, uint8_t *pBuf, uint32_t bufSize);
uint32_t QUEUE_PacketIn(QUEUE8_t *pQ8, uint8_t *pData, uint32_t len);
uint32_t QUEUE_PacketOut(QUEUE8_t *pQ8, uint8_t *pData, uint32_t dataLen);
uint32_t QUEUE_PacketLengthGet(QUEUE8_t *pQ8);

uint32_t QUEUE_PacketCharSplit(QUEUE8_t *pQ8, uint8_t splitChar, uint8_t *pData, uint32_t dataLen);
uint32_t QUEUE_PacketStartEndCharSplit(QUEUE8_t *pQ8, uint8_t splitChar, uint8_t *pData, uint32_t dataLen);
uint32_t QUEUE_PacketStartEndDifferentCharSplit(QUEUE8_t *pQ8, uint8_t startChar, uint8_t endChar, uint8_t *pData, uint32_t dataLen);
uint32_t QUEUE_PacketDoubleEndCharSplit(QUEUE8_t *pQ8, uint8_t splitChar1, uint8_t splitChar2, uint8_t *pData, uint32_t dataLen);
uint32_t QUEUE_PacketSpecifiedRulesOut(QUEUE8_t *pQ8, QUEUE_SpecifiedRulesCB *pFun, uint8_t *pData, uint32_t dataLen);


uint32_t QUEUE_StructCreate(QUEUE_STRUCT_t *pQueue, void *pBuf, uint32_t bufSize, uint16_t blkSize);
uint32_t QUEUE_StructIn(QUEUE_STRUCT_t *pQueue, void *pData, uint32_t blkCount);
uint32_t QUEUE_StructOut(QUEUE_STRUCT_t *pQueue, void *pData, uint32_t blkCount);
uint32_t QUEUE_StructCountGet(QUEUE_STRUCT_t *pQueue);
char *itoa(int num, char *str, int radix);
#ifdef  __cplusplus
}
#endif

#endif

