/*
********************************************************************************
*
*                                 Queue.c
*
* File          : Queue.c
* Version       : V1.0
* Author        : whq
* Mode          : Thumb2
* Toolchain     :
* Description   : 队列操作
*
* History       : 2014.11.29
                    1，修正存入队列时，可能造成读队列数据长度异常

                  2014.12.17
                    1，获取队列中的个数函数，采用逻辑计算，提高效率
                    2，增加规则取数据函数。使用规则函数从队列中取数据

* Date          : 2013.07.22
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "Queue.h"

uint8_t Queue_RxBuffer[256] = {0};
uint8_t Queue_RxCount       = 0;

char *itoa(int num, char *str, int radix)
{
    char  string[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char* ptr = str;
    int denom = 0;  //余数
    int count = -1;
    int i;
    int j;

    while (num >= radix)
    {
        denom   = num % radix;
        num    /= radix;

        *ptr++  = string[denom];
        count++;
    }

    if (num)
    {
        *ptr++ = string[num];
        count++;
    }
    *ptr = '\0';
    j    = count;

    for (i = 0; i < (count + 1) / 2; i++)
    {
        int temp = str[i];
        str[i]   = str[j];
        str[j--] = temp;
    }

    return str;
}

/*******************************************************************************
* Function Name : uint32_t QUEUE_PacketCreate(QUEUE8_t *pQ8, uint8_t *pBuf, uint32_t lenSize)
* Description   : 队列建立
* Input         :   pQ8:    队列
                    pBuf:   队列缓冲区地址
                    bufSize:队列缓冲区大小
* Output        :
* Other         :
* Date          : 2013.08.29
*******************************************************************************/
uint32_t QUEUE_PacketCreate(QUEUE8_t *pQ8, uint8_t *pBuf, uint32_t bufSize)
{
    pQ8->bufSize    = bufSize;
    pQ8->pBuf       = pBuf;
    pQ8->pStart     = pBuf;
    pQ8->pEnd       = pBuf;

    return 0;
}

/*******************************************************************************
* Function Name : uint32_t QUEUE_PacketIn(QUEUE8_t *pQ8, uint8_t *pData, uint32_t len)
* Description   : 数据载入队列
* Input         :   pQ8:    队列
                    pData:  要进队列的数据
                    len:    数据长度
* Output        :
* Other         :
* Date          : 2013.08.29
*******************************************************************************/
uint32_t QUEUE_PacketIn(QUEUE8_t *pQ8, uint8_t *pData, uint32_t len)
{
    volatile uint8_t    *pEnd   = NULL;

    uint32_t            index   = 0;

    pEnd = pQ8->pEnd;

    for (index = 0; index < len; index++)
    {
        if (++pEnd >= pQ8->pBuf + pQ8->bufSize)
        {
            pEnd = pQ8->pBuf;
        }
        if (pEnd == pQ8->pStart)
        {
            break;
        }

        *pQ8->pEnd = *pData++;

        pQ8->pEnd = pEnd;
    }

    return index;
}


/*******************************************************************************
* Function Name : uint32_t QUEUE_PacketOut(QUEUE8_t *pQ8, uint8_t *pData, uint32_t dataLen)
* Description   : 队列中取数据
* Input         :   pQ8:    队列
                    pData:  缓冲区
                    dataLen:缓冲区大小
* Output        :
* Other         :
* Date          : 2013.08.29
*******************************************************************************/
uint32_t QUEUE_PacketOut(QUEUE8_t *pQ8, uint8_t *pData, uint32_t dataLen)
{
    uint32_t            index   = 0;
    volatile uint8_t    *pStart = NULL;

    while ((pQ8->pStart != pQ8->pEnd) && (index < dataLen) &&
           (index < pQ8->bufSize))
    {
        pStart = pQ8->pStart;

        pData[index++] = *pStart++;
        if (pStart >= pQ8->pBuf + pQ8->bufSize)
        {
            pStart = pQ8->pBuf;
        }

        pQ8->pStart = pStart;
    }

    return index;
}
/*******************************************************************************
* Function Name :uint16_t QUEUE_GetDataSize(QUEUE8_t *pQ8)
* Description   :获取队列的size
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          :2018.07.12
*******************************************************************************/

uint16_t QUEUE_GetDataSize(QUEUE8_t *pQ8)
{
	return pQ8->pEnd - pQ8->pStart;
}
/*******************************************************************************
* Function Name :void QUEUE_Reset(QUEUE8_t *pQ8, uint8_t *pBuf, uint32_t bufSize)
* Description   :重置队列
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          :2018.07.12
*******************************************************************************/

void QUEUE_Reset(QUEUE8_t *pQ8, uint8_t *pBuf, uint32_t bufSize)
{
	memset(pBuf, 0, bufSize);
	pQ8->pBuf       = pBuf;
    pQ8->pStart     = pBuf;
    pQ8->pEnd       = pBuf;
	
	
}


#if     DEBUG_FULL_ASSERT

/*******************************************************************************
* Function Name : void ASSERT_FAILED(uint8_t* file, uint32_t line)
* Description   : 异常
* Input         :
* Output        :
* Other         :
* Date          : 2013.08.29
*******************************************************************************/
void ASSERT_FAILED(uint8_t* file, uint32_t line)
{
    uint8_t flg = 1;

    //printf("wrong information 文件:%s 第%d行\r\n", file, line);
    while (flg);
}

#endif

