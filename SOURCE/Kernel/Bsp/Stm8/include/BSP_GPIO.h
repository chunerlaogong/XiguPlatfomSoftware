/*
********************************************************************************
*
*                                 bsp.h
*
* File          : bsp.h
* Version       : V1.0
* Author        : 
* Mode          : Thumb2
* Toolchain     : 
* Description   : BSP硬件初始化
*                
* History       :
* Date          : 2018.05.07
*******************************************************************************/
#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_
#include "stm8l15x.h"

/*********************宏定义***********************/
#define BERTHPO_NB 1
/*
*********************************************************************************************************
*                                             PERIPH DEFINES
*********************************************************************************************************
*/
//IO置高
#define IO_SET(portInfo)             BSP_GpioSet(portInfo)
//IO置低
#define IO_RESET(portInfo)           BSP_GpioReset(portInfo)
//读取电平
#define IO_READ(portInfo)            BSP_GpioRead(portInfo)

#define BSP_CONEOF(a)           ((sizeof(a)) / (sizeof(*a)))
/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/
#ifdef BERTHPO_NB
typedef enum {
    eLED_BT,          //蓝牙LED控制口
    ePWR_BT,          //蓝牙电源控制信号
    eI2C_RM3100,      //地磁i2C选择信号
    eGPIO_MAX_COUNT,
}GPIO_ENUM_t;
#endif
typedef  const struct
{
    GPIO_TypeDef    *port;  //IO 端口
    uint16_t        pin;    //IO 引脚
    uint8_t         io;     //IO 0 输出   1输入   2模拟输入
    GPIO_ENUM_t     id;     //IO ID号
} PORT_INFO_t;

void GPIO_Configuration();
void BSP_Init();
void BSP_GpioSet(PORT_INFO_t portInfo);
void BSP_GpioReset(PORT_INFO_t portInfo);
uint8_t BSP_GpioRead(PORT_INFO_t portInfo);













#endif    //_BSP_GPIO_H_
/******************************END*********************************************/