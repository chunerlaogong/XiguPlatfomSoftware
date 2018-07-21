#ifndef _DRIVERS_I410ES_H_
#define _DRIVERS_I410ES_H_
#include <stm8l15x.h>
#include "Queue.h"
#include "BSP_UART.h"
#define BT_SW_UART 0   //使用模拟串口
#define NO_BACK               0
#define BACK_OK               1
#define BACK_ERROR            2
#define BACK_TIMEOUT          3
#define NEW_MESSAGE           4

#define BT_UART_RX_BUF_SIZE  64


typedef struct
{
    GPIO_TypeDef *BT_POWER_GPIO_BASE;       //蓝牙模块电源管脚GPIO
    uint8_t       BT_POWER_GPIO_Pin;        //蓝牙模块电源管脚GPIO Pin
    
}*PBT_I410ES_GPIO_TypeDef, SBT_I410ES_GPIO_TypeDef;

typedef struct
{
    void (*BT_I410ES_PowerOn)(PBT_I410ES_GPIO_TypeDef pBTI410ES_GPIO);      //BT_I410ES 电源使能
    void (*BT_I410ES_PowerOff)(PBT_I410ES_GPIO_TypeDef pBTI410ES_GPIO);     //BT_I410ES 电源关闭
    void (*BT_I410ES_PowerReset)(PBT_I410ES_GPIO_TypeDef pBTI410ES_GPIO);     //BT_I410ES 电源复位
    void (*BT_I410ES_Read)(uint8_t *readBuf);        //BT_I410ES 读数据
    void (*BT_I410ES_Write)(uint8_t *sendBuf, uint16_t sendLen);       //BT_I410ES 写数据
    void (*BT_I410ES_Init)(void);
	void (*BT_I410ES_BufReset)(void);    //清除蓝牙数据缓冲区
    void (*BT_I410ES_CallBack)();
} BT_I410ES_OPERATION_TypeDef;

void Drivers_BTI410ESPowerOn(PBT_I410ES_GPIO_TypeDef pBT_I410ES_GPIO);
void Drivers_BTI410ESPowerOff(PBT_I410ES_GPIO_TypeDef pBT_I410ES_GPIO);
void Drivers_BTI410ESRead(uint8_t *readBuf);
void Drivers_BTI410ESWrite(uint8_t *sendBuf, uint16_t sendLen);
void Drivers_BTI410ESInit();
void Drivers_BTI410ESBufReset();
void Drivers_BTI410EPowerReset();
//设置BT_I410ES 使能控制管脚为输出状态
//使能蓝牙
#define BT_I410ES_PWR_ON(pBTI410ES_GPIO) \
                       pBTI410ES_GPIO->BT_POWER_GPIO_BASE->DDR |= pBTI410ES_GPIO->BT_POWER_GPIO_Pin; \
                       pBTI410ES_GPIO->BT_POWER_GPIO_BASE->ODR |= pBTI410ES_GPIO->BT_POWER_GPIO_Pin;\
                       pBTI410ES_GPIO->BT_POWER_GPIO_BASE->CR1 |= pBTI410ES_GPIO->BT_POWER_GPIO_Pin
//关闭蓝牙
#define BT_I410ES_PWR_OFF(pBTI410ES_GPIO) \
                       pBTI410ES_GPIO->BT_POWER_GPIO_BASE->ODR &= (~(pBTI410ES_GPIO->BT_POWER_GPIO_Pin))

//关闭串口BT_UART_TX
#define BT_I410ES_UART_TX_OFF\
                       G_BT_UART_CONFIG.UART_TX_GPIO_BASE->DDR &= (~(G_BT_UART_CONFIG.UART_TX_GPIO_Pin))

//使能串口BT_UART_RX
#define BT_I410ES_UART_RX_OFF\
                       G_BT_UART_CONFIG.UART_RX_GPIO_BASE->DDR &= (~(G_BT_UART_CONFIG.UART_RX_GPIO_Pin))
#endif   //_DRIVERS_I410ES_H_