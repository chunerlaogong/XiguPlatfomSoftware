#ifndef _DRIVERS_I410ES_H_
#define _DRIVERS_I410ES_H_
#include <stm8l15x.h>
#include "Queue.h"
#include "BSP_UART.h"
#define BT_SW_UART 0   //ʹ��ģ�⴮��
#define NO_BACK               0
#define BACK_OK               1
#define BACK_ERROR            2
#define BACK_TIMEOUT          3
#define NEW_MESSAGE           4

#define BT_UART_RX_BUF_SIZE  64


typedef struct
{
    GPIO_TypeDef *BT_POWER_GPIO_BASE;       //����ģ���Դ�ܽ�GPIO
    uint8_t       BT_POWER_GPIO_Pin;        //����ģ���Դ�ܽ�GPIO Pin
    
}*PBT_I410ES_GPIO_TypeDef, SBT_I410ES_GPIO_TypeDef;

typedef struct
{
    void (*BT_I410ES_PowerOn)(PBT_I410ES_GPIO_TypeDef pBTI410ES_GPIO);      //BT_I410ES ��Դʹ��
    void (*BT_I410ES_PowerOff)(PBT_I410ES_GPIO_TypeDef pBTI410ES_GPIO);     //BT_I410ES ��Դ�ر�
    void (*BT_I410ES_PowerReset)(PBT_I410ES_GPIO_TypeDef pBTI410ES_GPIO);     //BT_I410ES ��Դ��λ
    void (*BT_I410ES_Read)(uint8_t *readBuf);        //BT_I410ES ������
    void (*BT_I410ES_Write)(uint8_t *sendBuf, uint16_t sendLen);       //BT_I410ES д����
    void (*BT_I410ES_Init)(void);
	void (*BT_I410ES_BufReset)(void);    //����������ݻ�����
    void (*BT_I410ES_CallBack)();
} BT_I410ES_OPERATION_TypeDef;

void Drivers_BTI410ESPowerOn(PBT_I410ES_GPIO_TypeDef pBT_I410ES_GPIO);
void Drivers_BTI410ESPowerOff(PBT_I410ES_GPIO_TypeDef pBT_I410ES_GPIO);
void Drivers_BTI410ESRead(uint8_t *readBuf);
void Drivers_BTI410ESWrite(uint8_t *sendBuf, uint16_t sendLen);
void Drivers_BTI410ESInit();
void Drivers_BTI410ESBufReset();
void Drivers_BTI410EPowerReset();
//����BT_I410ES ʹ�ܿ��ƹܽ�Ϊ���״̬
//ʹ������
#define BT_I410ES_PWR_ON(pBTI410ES_GPIO) \
                       pBTI410ES_GPIO->BT_POWER_GPIO_BASE->DDR |= pBTI410ES_GPIO->BT_POWER_GPIO_Pin; \
                       pBTI410ES_GPIO->BT_POWER_GPIO_BASE->ODR |= pBTI410ES_GPIO->BT_POWER_GPIO_Pin;\
                       pBTI410ES_GPIO->BT_POWER_GPIO_BASE->CR1 |= pBTI410ES_GPIO->BT_POWER_GPIO_Pin
//�ر�����
#define BT_I410ES_PWR_OFF(pBTI410ES_GPIO) \
                       pBTI410ES_GPIO->BT_POWER_GPIO_BASE->ODR &= (~(pBTI410ES_GPIO->BT_POWER_GPIO_Pin))

//�رմ���BT_UART_TX
#define BT_I410ES_UART_TX_OFF\
                       G_BT_UART_CONFIG.UART_TX_GPIO_BASE->DDR &= (~(G_BT_UART_CONFIG.UART_TX_GPIO_Pin))

//ʹ�ܴ���BT_UART_RX
#define BT_I410ES_UART_RX_OFF\
                       G_BT_UART_CONFIG.UART_RX_GPIO_BASE->DDR &= (~(G_BT_UART_CONFIG.UART_RX_GPIO_Pin))
#endif   //_DRIVERS_I410ES_H_