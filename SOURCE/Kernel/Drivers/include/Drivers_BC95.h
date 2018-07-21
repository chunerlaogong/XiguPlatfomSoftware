/*
********************************************************************************
*
*                                 Drives_BC95.h
*
* File          : Drives_BC95.h
* Version       : V1.0
* Author        : Lhl
* Mode          : Thumb2
* Toolchain     :
* Description   : BC95驱动头文件
*
* History       :
* Date          : 2018.07.09
*******************************************************************************/
#ifndef _DRIVERS_BC95_H_
#define _DRIVERS_BC95_H_
#include "BSP_UART.h"
#define BC95_PWR_SET(BC95_PWR_BASE, BC95_PWR_PIN)                       (BC95_PWR_BASE->DDR    |= BC95_PWR_PIN);\
                                                                        (BC95_PWR_BASE->CR1    |= BC95_PWR_PIN); \
                                                                        (BC95_PWR_BASE->ODR    |= BC95_PWR_PIN)   //置高
#define BC95_RESET_SET(BC95_RESET_BASE, BC95_RESET_PIN)                 (BC95_RESET_BASE->DDR |=BC95_RESET_PIN);\
                                                                        (BC95_RESET_BASE->CR1 |= BC95_RESET_PIN); \
                                                                        (BC95_RESET_BASE->ODR |= BC95_RESET_PIN)  //置高
#define BC95_PWR_CLR(BC95_PWR_BASE, BC95_PWR_PIN)                       (BC95_PWR_BASE->DDR    |=BC95_PWR_PIN);\
                                                                        (BC95_PWR_BASE->CR1    |= BC95_PWR_PIN); \
                                                                        (BC95_PWR_BASE->ODR    &= ~BC95_PWR_PIN)     //置低
#define BC95_RESET_CLR(BC95_RESET_BASE, BC95_RESET_PIN)                 (BC95_RESET_BASE->DDR |=BC95_RESET_PIN);\
                                                                        (BC95_RESET_BASE->CR1 |= BC95_RESET_PIN); \
                                                                        (BC95_RESET_BASE->ODR &= ~BC95_RESET_PIN)    //置低
#define COAP     1
#define UDP      0

#define     MC_ERROR                        -1           
#define     MC_OK                           1           
#define     MC_NONE                         0

typedef struct BC95_GPIO_struct
{
    GPIO_TypeDef*       BC95_PWR_BASE;           //BC95电源脚的GPIO分组
    GPIO_TypeDef*       BC95_RESET_BASE;         //BC95复位键脚的GPIO分组
    uint8_t             BC95_PWR_PIN;            //BC95电源脚的GPIO Pin
    uint8_t             BC95_RESET_PIN;          //BC95复位键脚的GPIO Pin
    SBSP_UART_CFG_TypeDef COM;                   //串口
    uint8_t             maxTryCount;             //发送最大尝试次数
    uint8_t             sendTimes;               //需要有效发送次数
    uint8_t             Type;                    //使用COAP或者UDP
    uint8_t             DestIpv4_1;
    uint8_t             DestIpv4_2;
    uint8_t             DestIpv4_3;
    uint8_t             DestIpv4_4;
    uint16_t            DestPort;
} *PBC95_GPIO_TypeDef, SBC95_GPIO_TypeDef;

struct Drivers_BC95_Config
{
    int (*SocketSendData) (PBC95_GPIO_TypeDef pGpioBC95Structer, uint8_t* Data,
                          uint8_t len);
    int (*NetWork_Manage) (PBC95_GPIO_TypeDef pGpioBC95Structer);
    void (*GetSignal) (PBC95_GPIO_TypeDef pGpioBC95Structer, uint16_t* Signal);
    void (*GetNetTime) (PBC95_GPIO_TypeDef pGpioBC95Structer, uint8_t* Time);
    void (*PowerOff) (PBC95_GPIO_TypeDef pGpioBC95Structer);
    void (*PowerOn) (PBC95_GPIO_TypeDef pGpioBC95Structer);
};
int Drivers_BC95_Manage(PBC95_GPIO_TypeDef pGpioBC95Structer);
void Drivers_GetSignal(PBC95_GPIO_TypeDef pGpioBC95Structer, uint16_t* Signal);
int Drivers_BC95_SendData(PBC95_GPIO_TypeDef pGpioBC95Structer, uint8_t* Data,
                          uint8_t len);
void Drivers_GetNetTime(PBC95_GPIO_TypeDef pGpioBC95Structer, uint8_t* Time);
void Drivers_PowerOn(PBC95_GPIO_TypeDef pGpioBC95Structer);
void Drivers_PowerOff(PBC95_GPIO_TypeDef pGpioBC95Structer);



static const struct Drivers_BC95_Config Drivers_BC95_Operation =
{
    .SocketSendData = Drivers_BC95_SendData,
    .NetWork_Manage = Drivers_BC95_Manage,
    .GetSignal = Drivers_GetSignal,
    .GetNetTime = Drivers_GetNetTime,
    .PowerOff = Drivers_PowerOff,
    .PowerOn = Drivers_PowerOn,
};

#endif
