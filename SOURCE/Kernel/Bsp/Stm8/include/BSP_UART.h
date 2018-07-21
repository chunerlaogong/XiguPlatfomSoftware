/*
********************************************************************************
*
*                                 BSP_Uart.h
*
* File          : BSP_Uart.h
* Version       : V1.0
* Author        : Lhl
* Mode          : Thumb2
* Toolchain     :
* Description   : 串口驱动头文件
*
* History       :
* Date          : 2018.05.10
*******************************************************************************/

#ifndef _BSP_UART_H_
#define _BSP_UART_H_
#include <stm8l15x.h>
#include "Queue.h"

typedef struct    //串口GPIO配置结构体
{
    USART_TypeDef* USARTNum;
    CLK_Peripheral_TypeDef CLKPeripheralNum;
    GPIO_TypeDef* UART_TX_GPIO_BASE;
    uint8_t       UART_TX_GPIO_Pin;
    GPIO_TypeDef* UART_RX_GPIO_BASE;
    uint8_t       UART_RX_GPIO_Pin;
    uint32_t baud;
    uint8_t data;
    uint8_t stop;
    uint8_t parity;
}*PBSP_UART_CFG_TypeDef, SBSP_UART_CFG_TypeDef;
/******************************函数声明*************************************/
void BSP_UartOpen(PBSP_UART_CFG_TypeDef pBspUartCfg);
void BSP_UartSendByte(USART_TypeDef* USARTNum, uint8_t Data);


void BSP_UartSendBytes(USART_TypeDef* USARTNum, uint8_t *buffter);
void BSP_UartSendBytesLen(USART_TypeDef* USARTNum, uint8_t *buffter,
                          uint8_t len);        //适用于发送数据中包含大量0x00
uint32_t BSP_UartRead(QUEUE8_t *uartRxBufQueue,
                      uint8_t *buffter, uint32_t len);
#endif   //_BSP_UART_H_