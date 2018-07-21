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
* Description   : BSPӲ����ʼ��
*                
* History       :
* Date          : 2018.05.07
*******************************************************************************/
#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_
#include "stm8l15x.h"

/*********************�궨��***********************/
#define BERTHPO_NB 1
/*
*********************************************************************************************************
*                                             PERIPH DEFINES
*********************************************************************************************************
*/
//IO�ø�
#define IO_SET(portInfo)             BSP_GpioSet(portInfo)
//IO�õ�
#define IO_RESET(portInfo)           BSP_GpioReset(portInfo)
//��ȡ��ƽ
#define IO_READ(portInfo)            BSP_GpioRead(portInfo)

#define BSP_CONEOF(a)           ((sizeof(a)) / (sizeof(*a)))
/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/
#ifdef BERTHPO_NB
typedef enum {
    eLED_BT,          //����LED���ƿ�
    ePWR_BT,          //������Դ�����ź�
    eI2C_RM3100,      //�ش�i2Cѡ���ź�
    eGPIO_MAX_COUNT,
}GPIO_ENUM_t;
#endif
typedef  const struct
{
    GPIO_TypeDef    *port;  //IO �˿�
    uint16_t        pin;    //IO ����
    uint8_t         io;     //IO 0 ���   1����   2ģ������
    GPIO_ENUM_t     id;     //IO ID��
} PORT_INFO_t;

void GPIO_Configuration();
void BSP_Init();
void BSP_GpioSet(PORT_INFO_t portInfo);
void BSP_GpioReset(PORT_INFO_t portInfo);
uint8_t BSP_GpioRead(PORT_INFO_t portInfo);













#endif    //_BSP_GPIO_H_
/******************************END*********************************************/