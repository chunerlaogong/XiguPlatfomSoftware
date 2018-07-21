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
* Description   : BC95����ͷ�ļ�
*
* History       :
* Date          : 2018.07.09
*******************************************************************************/
#ifndef _DRIVERS_BC95_H_
#define _DRIVERS_BC95_H_
#include "BSP_UART.h"
#define BC95_PWR_SET(BC95_PWR_BASE, BC95_PWR_PIN)                       (BC95_PWR_BASE->DDR    |= BC95_PWR_PIN);\
                                                                        (BC95_PWR_BASE->CR1    |= BC95_PWR_PIN); \
                                                                        (BC95_PWR_BASE->ODR    |= BC95_PWR_PIN)   //�ø�
#define BC95_RESET_SET(BC95_RESET_BASE, BC95_RESET_PIN)                 (BC95_RESET_BASE->DDR |=BC95_RESET_PIN);\
                                                                        (BC95_RESET_BASE->CR1 |= BC95_RESET_PIN); \
                                                                        (BC95_RESET_BASE->ODR |= BC95_RESET_PIN)  //�ø�
#define BC95_PWR_CLR(BC95_PWR_BASE, BC95_PWR_PIN)                       (BC95_PWR_BASE->DDR    |=BC95_PWR_PIN);\
                                                                        (BC95_PWR_BASE->CR1    |= BC95_PWR_PIN); \
                                                                        (BC95_PWR_BASE->ODR    &= ~BC95_PWR_PIN)     //�õ�
#define BC95_RESET_CLR(BC95_RESET_BASE, BC95_RESET_PIN)                 (BC95_RESET_BASE->DDR |=BC95_RESET_PIN);\
                                                                        (BC95_RESET_BASE->CR1 |= BC95_RESET_PIN); \
                                                                        (BC95_RESET_BASE->ODR &= ~BC95_RESET_PIN)    //�õ�
#define COAP     1
#define UDP      0

#define     MC_ERROR                        -1           
#define     MC_OK                           1           
#define     MC_NONE                         0

typedef struct BC95_GPIO_struct
{
    GPIO_TypeDef*       BC95_PWR_BASE;           //BC95��Դ�ŵ�GPIO����
    GPIO_TypeDef*       BC95_RESET_BASE;         //BC95��λ���ŵ�GPIO����
    uint8_t             BC95_PWR_PIN;            //BC95��Դ�ŵ�GPIO Pin
    uint8_t             BC95_RESET_PIN;          //BC95��λ���ŵ�GPIO Pin
    SBSP_UART_CFG_TypeDef COM;                   //����
    uint8_t             maxTryCount;             //��������Դ���
    uint8_t             sendTimes;               //��Ҫ��Ч���ʹ���
    uint8_t             Type;                    //ʹ��COAP����UDP
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
