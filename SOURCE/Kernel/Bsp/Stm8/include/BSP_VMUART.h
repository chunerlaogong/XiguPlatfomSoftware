#ifndef _BSP_VM_UART_H_
#define _BSP_VM_UART_H_

#include "stm8l15x.h"
#include "Queue.h"
#include "BSP_GPIO.h"

#define COM1_SW_EN 0
#define COM2_SW_EN 1
#define EMUART_IDLE  0    //串口空闲状态
#define EMUART_RECV  1    //串口接收数据
#define EMUART_SEND  2    //串口发送数据

typedef enum
{
#if  COM1_SW_EN
    COM1_SW,
#endif
#if     COM2_SW_EN
    COM2_SW,
#endif
    COM_SW_MAX,
} COM_SW_PORT;
#define COMSWn    COM_SW_MAX

#if COM1_SW_EN
#define COM1_SW_RX_BUFF_SIZE               64
static uint8_t COM1_SW_RX_BUFF[COM1_SW_RX_BUFF_SIZE] = {0};
static QUEUE8_t COM1_SW_RX_QUEUE = {0};
#endif
#if COM2_SW_EN
#define COM2_SW_RX_BUFF_SIZE               64
static uint8_t COM2_SW_RX_BUFF[COM2_SW_RX_BUFF_SIZE] = {0};
static QUEUE8_t COM2_SW_RX_QUEUE = {0};
#endif
typedef struct
{
    uint8_t status;            //记录串口的工作状态
    uint8_t startRxFlag;       //开始接收标志
    uint8_t startSendFlag ;    //开始发送标志
    uint32_t baudRate;         //串口的波特率
    uint8_t oneRameRcvOK;      //判断一帧数据是否接收OK
    uint8_t isRead;        //记录数据是否已经取出
    uint8_t isSend;        //记录写入的数据是否已经发送
    uint8_t bitSended;     //记录串口已经发送完的数据位个数
    uint8_t bitRecved;     //记录串口已经接收到的数据位个数
    uint8_t sendData;      //发送数据缓冲区
    uint8_t recvData;      //接收数据缓冲区
} EMUART_STRUCT;
//默认模拟串口1使用timer1,模拟串口2使用timer2，主频使用8M 如果要修改，需修改定时器分频，
typedef struct UART_struct
{
    EMUART_STRUCT  EMUART_PARAM;       //串口收发数据参数
    uint8_t       SYS_CLK;             //系统主频时钟大小
#if  COM1_SW_EN
    uint8_t       rxTemp[COM1_SW_RX_BUFF_SIZE];   //接收缓冲区
    void (*Vm1UartSendByte)(uint8_t data);
    void (*Vm1UartSendBytes)(uint8_t* buffer);
#endif
#if  COM2_SW_EN
    uint8_t       rxTemp[COM2_SW_RX_BUFF_SIZE];   //接收缓冲区
    void (*Vm2UartSendByte)(uint8_t data);
    void (*Vm2UartSendBytes)(uint8_t* buffer);
#endif
    uint8_t       pRxTempPos;           //接收缓冲区数据指针
} SW_UART_TypeDef;


typedef enum
{
#if  COM1_SW_EN
    eCOM1_SW_Rx = 0,
    eCOM1_SW_Tx,
#endif
#if COM2_SW_EN
    eCOM2_SW_Rx,
    eCOM2_SW_Tx,
#endif
    eGPIO_UART_SW_MAX_COUNT
} GPIO_COM_SW_ENUM_t;
static PORT_INFO_t sSwUartPortInfo[eGPIO_UART_SW_MAX_COUNT] =
{
#if COM1_SW_EN
    {GPIOB,  GPIO_Pin_1,     3,     eCOM1_SW_Rx}, 
    {GPIOB,  GPIO_Pin_0,     3,     eCOM1_SW_Tx},   
#endif
#if COM2_SW_EN
    {GPIOE,  GPIO_Pin_6,     3,     eCOM2_SW_Rx},  
    {GPIOE,  GPIO_Pin_7,     3,     eCOM2_SW_Tx},
   // {GPIOB,  GPIO_Pin_1,     3,     eCOM2_SW_Rx}, 
   // {GPIOB,  GPIO_Pin_0,     3,     eCOM2_SW_Tx},
#endif
};
static uint8_t * const COM_SW_RX_BUFF[COMSWn] =
{
#if     COM1_SW_EN
    COM1_SW_RX_BUFF,
#endif
#if     COM2_SW_EN
    COM2_SW_RX_BUFF,
#endif
};
static QUEUE8_t *  const COM_SW_RX_QUEUE[COMSWn] =
{
#if     COM1_SW_EN
    &COM1_SW_RX_QUEUE,
#endif
#if     COM2_SW_EN
    &COM2_SW_RX_QUEUE,
#endif

};

static const uint32_t COM_SW_RX_BUFF_SIZE[COMSWn] =
{
#if     COM1_SW_EN
    COM1_SW_RX_BUFF_SIZE,
#endif
#if     COM2_SW_EN
    COM2_SW_RX_BUFF_SIZE,
#endif
};

//设置GPIO为输入
#if COM1_SW_EN
#define COM2_SW_RXD_PORT_IN       sSwUartPortInfo[eCOM1_SW_Rx].port->DDR &= ~(sSwUartPortInfo[eCOM1_SW_Rx].pin);\
                                  sSwUartPortInfo[eCOM1_SW_Rx].port->CR1 |=  (sSwUartPortInfo[eCOM1_SW_Rx].pin);\
                                  sSwUartPortInfo[eCOM1_SW_Rx].port->CR2 &=  (~sSwUartPortInfo[eCOM1_SW_Rx].pin)

//设置为中断输入
#define COM2_SW_RXD_PORT_INT_IN    sSwUartPortInfo[eCOM1_SW_Rx].port->DDR &= ~(sSwUartPortInfo[eCOM1_SW_Rx].pin);\
                                   sSwUartPortInfo[eCOM1_SW_Rx].port->CR1 |=  (sSwUartPortInfo[eCOM1_SW_Rx].pin); \
                                   sSwUartPortInfo[eCOM1_SW_Rx].port->CR2 |=  (sSwUartPortInfo[eCOM1_SW_Rx].pin)
#endif   //COM1_SW_EN

#if COM2_SW_EN
#define COM2_SW_RXD_PORT_IN       sSwUartPortInfo[eCOM2_SW_Rx].port->DDR &= ~(sSwUartPortInfo[eCOM2_SW_Rx].pin);\
                                  sSwUartPortInfo[eCOM2_SW_Rx].port->CR1 |= (sSwUartPortInfo[eCOM2_SW_Rx].pin);\
                                  sSwUartPortInfo[eCOM2_SW_Rx].port->CR2 &=  (~sSwUartPortInfo[eCOM2_SW_Rx].pin);

//设置为中断输入
#define COM2_SW_RXD_PORT_INT_IN    sSwUartPortInfo[eCOM2_SW_Rx].port->DDR &= ~(sSwUartPortInfo[eCOM2_SW_Rx].pin);\
                                   sSwUartPortInfo[eCOM2_SW_Rx].port->CR1 |= (sSwUartPortInfo[eCOM2_SW_Rx].pin); \
                                   sSwUartPortInfo[eCOM2_SW_Rx].port->CR2 |=  (sSwUartPortInfo[eCOM2_SW_Rx].pin)
#endif  //COM2_SW_EN
void BSP_Vm2UartSendByte(uint8_t data);
void BSP_Vm2UartSendBytes(uint8_t* buffer);
static void BSP_TIM4GpioUartConfig();   //定时器1和4作为模拟串口2的控制定时器
static void BSP_TIM1GpioUartConfig();  
static SW_UART_TypeDef sSwUart[COMSWn] =
{
#if COM1_SW_EN
    {
        .Vm1UartSendByte = BSP_Vm1UartSendByte,
        .Vm1UartSendBytes = BSP_Vm1UartSendBytes,
    },
#endif
#if     COM2_SW_EN
    {
        .Vm2UartSendByte = BSP_Vm2UartSendByte,
        .Vm2UartSendBytes = BSP_Vm2UartSendBytes,
    }
#endif
};
#endif    //_BSP_VM_UART_H_