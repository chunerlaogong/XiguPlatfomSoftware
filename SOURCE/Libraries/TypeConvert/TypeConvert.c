/*
********************************************************************************
*                            Library.c
*
* File          : Library.c
* Version       : V1.0
* Author        : whq
* Mode          : Thumb2
* Toolchain     :                      
* Description   : 库函数
*
* Date          : 2013/12/11
* History		: 
* 
*******************************************************************************/
//#include "stdafx.h"

#include <time.h>
#include <string.h>


#include "TypeConvert.h"



/*******************************************************************************
* Function Name :RECORD_BcdToInt
* Description   :BCD字节转为整型
* Input         :uint8_t val    BCD值
* Output        :uint8_t        INT8值
* Other         :
* Date          :2013/12/11
*******************************************************************************/
uint8_t LIB_BcdToInt8(uint8_t val)
{
    return ((val >> 4) * 10 + (val & 0xF));
}

/*******************************************************************************
* Function Name :RECORD_Int8ToBcd
* Description   :
* Input         :uint8_t val
* Output        :uint8_t
* Other         :
* Date          :2013/12/11
*******************************************************************************/
uint8_t LIB_Int8ToBcd(uint8_t val)
{
    return (((val / 10) << 4) + (val % 10));
}

/*******************************************************************************
* Function Name :RECORD_StringBcdToInt
* Description   :无符号BCD字符串转为整型    (BCD高字节在前)
* Input         :uint8_t * str  BCD字符串
* Input         :uint16_t len   BCD字符串长度
* Output        :uint32_t
* Other         :
* Date          :2013/12/11
*******************************************************************************/
uint32_t LIB_StringBcdToInt(uint8_t *str, uint16_t len)
{
    uint32_t res = 0;

    while (len--)
    {
        res *= 100;
        res += LIB_BcdToInt8(*str++);
    }
    return res;
}

/*******************************************************************************
* Function Name :RECORD_IntToStringBcd
* Description   :整数转为BCD字符串，字符串右对齐 (BCD高字节在前)
* Input         :uint32_t val   整数
* Input         :uint8_t * str  转换后缓冲区
* Input         :uint16_t size  缓冲区空间大小
* Other         :
* Date          :2013/12/11
*******************************************************************************/
void LIB_IntToStringBcd( uint8_t *str, uint16_t size, uint32_t val )
{
    while (size--)
    {
        *(str + size)  = LIB_Int8ToBcd(val % 100);
        val /= 100;
    }
}

/*******************************************************************************
* Function Name :RECORD_Int16ByteReversed
* Description   :16位整型 大小端互转
* Input         :uint16_t val
* Output        :uint16_t
* Other         :
* Date          :2013/12/11
*******************************************************************************/
uint16_t LIB_Int16ByteReversed(uint16_t val)
{
    uint16_t res = 0;

    res = val << 8;
    res += val >> 8;

    return res;
}

/*******************************************************************************
* Function Name :RECORD_Int32ByteReversed
* Description   :32位大小端互转 
* Input         :uint32_t val
* Output        :uint32_t
* Other         :
* Date          :2013/12/11
*******************************************************************************/
uint32_t LIB_Int32ByteReversed(uint32_t val)
{
    uint8_t *p = (uint8_t *)&val;
    uint8_t arr[4] = {0};

    arr[0] = p[3];
    arr[1] = p[2];
    arr[2] = p[1];
    arr[3] = p[0];

    return *(uint32_t *)arr;
}

/*******************************************************************************
* Function Name :RECORD_CheckXOR
* Description   :异或效验
* Input         :uint8_t * pBuf 异或效验数据
* Input         :uint16_t len   数据长度
* Output        :uint8_t        效验结果
* Other         :
* Date          :2013/12/11
*******************************************************************************/
uint8_t LIB_CheckXOR(uint8_t *pBuf, uint16_t len)
{
    uint8_t cXor = 0;

    while (len--)
    {
        cXor ^= *pBuf++;
    }

    return cXor;
}

/*******************************************************************************
* Function Name :RECORD_CheckXOR
* Description   :扩展异或效验
* Input         :uint8_t xor    效验初始值
* Input         :uint8_t * pBuf 异或效验数据
* Input         :uint16_t len   数据长度
* Output        :uint8_t        效验结果
* Other         :
* Date          :2013/12/11
*******************************************************************************/
uint8_t LIB_CheckXORExt(uint8_t cXor, uint8_t *pBuf, uint16_t len)
{
    while (len--)
    {
        cXor ^= *pBuf++;
    }

    return cXor;
}

/*******************************************************************************
* Function Name :RECORD_CheckSum
* Description   :加总效验
* Input         :uint8_t * pBuf 加总效验数据
* Input         :uint16_t len   数据长度
* Output        :uint8_t        加总和
* Other         :
* Date          :2013/12/11
*******************************************************************************/
uint8_t LIB_CheckSum(uint8_t *pBuf, uint16_t len)
{
    uint8_t sum = 0;

    while (len--)
    {
        sum += *pBuf++;
    }

    return sum;
}

/*******************************************************************************
* Function Name :RECORD_CheckSumExt
* Description   :扩展加总效验
* Input         :uint8_t sum    加总初始值
* Input         :uint8_t * pBuf 加总效验数据
* Input         :uint16_t len   数据长度
* Output        :uint8_t        加总和
* Other         :
* Date          :2013/12/11
*******************************************************************************/
uint8_t LIB_CheckSumExt(uint8_t sum, uint8_t *pBuf, uint16_t len)
{
    while (len--)
    {
        sum += *pBuf++;
    }

    return sum;
}

/*******************************************************************************
* Function Name :LIB_BCDTime2Sec
* Description   :BCD转为整型时间 
* Input         :BCDTIME * time BCD时间
* Output        :uint32_t       整型时间
* Other         :
* Date          :2013/12/12
*******************************************************************************/
uint32_t LIB_BCDTime2Sec(BCDTIME_t *time)
{
    uint32_t lTime;
    struct tm conv = {0};

    conv.tm_year = LIB_BcdToInt8(time->year) + 2000 - 1900;
    conv.tm_mon = LIB_BcdToInt8(time->month)-1;
    conv.tm_mday = LIB_BcdToInt8(time->day);
    conv.tm_hour = LIB_BcdToInt8(time->hour);
    conv.tm_min = LIB_BcdToInt8(time->minute);
    conv.tm_sec = LIB_BcdToInt8(time->seconds);

    lTime = (uint32_t)mktime(&conv);
    //lTime -= g_TimeZone * 60 * 60;	    //convert +8:00 time to UTC from 1970-01-01 00:00:00

    return lTime;
}

/*******************************************************************************
* Function Name :LIB_Sec2BCDTime
* Description   :整型时间转为BCD
* Input         :BCDTIME * tim  BCD缓冲区
* Input         :uint32_t sec   整型时间
* Output        :void
* Other         :
* Date          :2013/12/12
*******************************************************************************/
void LIB_Sec2BCDTime(BCDTIME_t *tim, uint32_t sec)
{
    struct tm conv;
    time_t cTim = sec;

    //cTim += g_TimeZone * 60 * 60;	    //convert UTC time to +8:00 time zone

#ifdef _OS_WINDOWS_

    localtime_s(&conv, &cTim);

#elif defined(_OS_UCOSII_) || defined(_OS_UCOS_III_) || defined(_OS_LINUX_)

    localtime_r(&cTim, &conv);

#else

    localtime_r(&cTim, &conv);
    
#endif

    tim->year = LIB_Int8ToBcd((uint8_t)(conv.tm_year + 1900 - 2000));
    tim->month = LIB_Int8ToBcd((uint8_t)conv.tm_mon+1);
    tim->day = LIB_Int8ToBcd((uint8_t)conv.tm_mday);
    tim->hour = LIB_Int8ToBcd((uint8_t)conv.tm_hour);
    tim->minute = LIB_Int8ToBcd((uint8_t)conv.tm_min);
    tim->seconds = LIB_Int8ToBcd((uint8_t)conv.tm_sec);
}



/*******************************************************************************
* Function Name : int32_t LIB_StatusFilter(STATE_JUDGE_t *pState, int8_t newState, int16_t judgeCount)
* Description   : 信号过滤判定操作  
* Input         :   pState:     信号判定变量
                    newState:   新的状态
                    judgeCount: 判定次数
* Output        : 判定后的状态
* Other         : 
* Date          : 2014.05.23
*******************************************************************************/
int32_t LIB_StatusFilter(STATE_JUDGE_t *pState, int8_t newState, int16_t judgeCount)
{
    if (pState->state != newState)
    {
        pState->judgeCount++;

        if (pState->judgeCount >= judgeCount)
        {
            pState->state = newState;

            pState->judgeCount = 0;
        }
    }
    else
    {
        pState->judgeCount = 0;
    }

    return pState->state;
}


/*******************************************************************************
* Function Name : void LIB_Int32BubbleSorting(int32_t *pData, int16_t len, int16_t mode)
* Description   : 有符号32位整形数据排序
* Input         :   pData:  数据
                    len:    数据总数
                    mode:   0)降序      1)升序
* Output        : 
* Other         : 
* Date          : 2015.09.23
*******************************************************************************/
void LIB_Int32BubbleSorting(int32_t *pData, int16_t len, int16_t mode)
{
    int16_t i       = 0;
    int16_t j       = 0;
    int32_t tmp     = 0;

    if (len < 2) return;

    for (i = 0; i < len - 1; i++)
    {
        for (j = 1; j < len - i; j++)
        {
            if (mode? (pData[j - 1] > pData[j]) : (pData[j - 1] < pData[j]))
            {
                tmp = pData[j - 1];
                pData[j - 1] = pData[j];
                pData[j] = tmp;
            }
        }
    }
}

/*******************************************************************************
* Function Name : void LIB_Int16BubbleSorting(int32_t *pData, int16_t len, int16_t mode)
* Description   : 有符号16位整形数据排序
* Input         :   pData:  数据
                    len:    数据总数
                    mode:   0)降序      1)升序
* Output        : 
* Other         : 
* Date          : 2015.09.23
*******************************************************************************/
void LIB_Int16BubbleSorting(int16_t *pData, int16_t len, int16_t mode)
{
    int16_t i       = 0;
    int16_t j       = 0;
    int16_t tmp     = 0;

    if (len < 2) return;

    for (i = 0; i < len - 1; i++)
    {
        for (j = 1; j < len - i; j++)
        {
            if (mode? (pData[j - 1] > pData[j]) : (pData[j - 1] < pData[j]))
            {
                tmp = pData[j - 1];
                pData[j - 1] = pData[j];
                pData[j] = tmp;
            }
        }
    }
}

/*******************************************************************************
* Function Name : void LIB_Int8BubbleSorting(int32_t *pData, int16_t len, int16_t mode)
* Description   : 有符号8位整形数据排序
* Input         :   pData:  数据
                    len:    数据总数
                    mode:   0)降序      1)升序
* Output        : 
* Other         : 
* Date          : 2015.09.23
*******************************************************************************/
void LIB_Int8BubbleSorting(int8_t *pData, int16_t len, int16_t mode)
{
    int16_t i       = 0;
    int16_t j       = 0;
    int8_t tmp      = 0;

    if (len < 2) return;

    for (i = 0; i < len - 1; i++)
    {
        for (j = 1; j < len - i; j++)
        {
            if (mode? (pData[j - 1] > pData[j]) : (pData[j - 1] < pData[j]))
            {
                tmp = pData[j - 1];
                pData[j - 1] = pData[j];
                pData[j] = tmp;
            }
        }
    }
}

/*******************************************************************************
* Function Name : void LIB_Uint32BubbleSorting(int32_t *pData, int16_t len, int16_t mode)
* Description   : 无符号32位整形数据排序
* Input         :   pData:  数据
                    len:    数据总数
                    mode:   0)降序      1)升序
* Output        : 
* Other         : 
* Date          : 2015.09.23
*******************************************************************************/
void LIB_Uint32BubbleSorting(uint32_t *pData, int16_t len, int16_t mode)
{
    int16_t i       = 0;
    int16_t j       = 0;
    uint32_t tmp    = 0;

    if (len < 2) return;

    for (i = 0; i < len - 1; i++)
    {
        for (j = 1; j < len - i; j++)
        {
            if (mode? (pData[j - 1] > pData[j]) : (pData[j - 1] < pData[j]))
            {
                tmp = pData[j - 1];
                pData[j - 1] = pData[j];
                pData[j] = tmp;
            }
        }
    }
}

/*******************************************************************************
* Function Name : void LIB_Uint16BubbleSorting(int32_t *pData, int16_t len, int16_t mode)
* Description   : 无符号16位整形数据排序
* Input         :   pData:  数据
                    len:    数据总数
                    mode:   0)降序      1)升序
* Output        : 
* Other         : 
* Date          : 2015.09.23
*******************************************************************************/
void LIB_Uint16BubbleSorting(uint16_t *pData, int16_t len, int16_t mode)
{
    int16_t i       = 0;
    int16_t j       = 0;
    uint16_t tmp    = 0;

    if (len < 2) return;

    for (i = 0; i < len - 1; i++)
    {
        for (j = 1; j < len - i; j++)
        {
            if (mode? (pData[j - 1] > pData[j]) : (pData[j - 1] < pData[j]))
            {
                tmp = pData[j - 1];
                pData[j - 1] = pData[j];
                pData[j] = tmp;
            }
        }
    }
}

/*******************************************************************************
* Function Name : void LIB_Uint8BubbleSorting(int32_t *pData, int16_t len, int16_t mode)
* Description   : 无符号8位整形数据排序
* Input         :   pData:  数据
                    len:    数据总数
                    mode:   0)降序      1)升序
* Output        : 
* Other         : 
* Date          : 2015.09.23
*******************************************************************************/
void LIB_Uint8BubbleSorting(uint8_t *pData, int16_t len, int16_t mode)
{
    int16_t i       = 0;
    int16_t j       = 0;
    uint8_t tmp     = 0;

    if (len < 2) return;

    for (i = 0; i < len - 1; i++)
    {
        for (j = 1; j < len - i; j++)
        {
            if (mode? (pData[j - 1] > pData[j]) : (pData[j - 1] < pData[j]))
            {
                tmp = pData[j - 1];
                pData[j - 1] = pData[j];
                pData[j] = tmp;
            }
        }
    }
}

/******************************END*********************************************/
