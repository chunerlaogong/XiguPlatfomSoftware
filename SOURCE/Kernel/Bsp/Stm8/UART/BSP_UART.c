/*
********************************************************************************
*
*                                 BSP_Uart.c
*
* File          : BSP_Uart.c
* Version       : V1.0
* Author        : Lhl
* Mode          : Thumb2
* Toolchain     :
* Description   : ������������
*
* History       :
* Date          : 2018.05.08
*******************************************************************************/
#include "stm8l15x_usart.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_it.h"
#include <stm8l15x.h>
#include "BSP_UART.h"
#include "Queue.h"
#include <stdlib.h>

static const uint16_t parityArr[3] = {USART_Parity_No, USART_Parity_Odd, USART_Parity_Even};

/*ϵͳprintf����ʵ��*/
int putchar(int c)
{
    if('\n' == (char)c)
    {
        USART_SendData8(USART1, '\r');
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
    USART_SendData8(USART1, c);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

    return (c);
}


/*******************************************************************************
* Function Name :void BSP_UartOpen(PBSP_UART_CFG_TypeDef pBspUartCfg)
* Description   :���ڼ����ų�ʼ��
* Input         :PBSP_UART_CFG_TypeDef pBspUartCfg
* Output        :
* Other         :
* Date          :2018.05.08
*******************************************************************************/
	

void BSP_UartOpen(PBSP_UART_CFG_TypeDef pBspUartCfg)
{
    USART_WordLength_TypeDef USART_WordLength;
    USART_StopBits_TypeDef USART_StopBits;
    USART_Parity_TypeDef USART_Parity;
    /* Enable UART clock */
    CLK_PeripheralClockConfig((CLK_Peripheral_TypeDef )pBspUartCfg->CLKPeripheralNum , ENABLE);
    USART_DeInit(pBspUartCfg->USARTNum);
    /* Configure USART Tx as alternate function push-pull */
    GPIO_ExternalPullUpConfig(pBspUartCfg->UART_TX_GPIO_BASE, pBspUartCfg->UART_TX_GPIO_Pin, ENABLE);
    /* Configure USART Rx as input floating */
    GPIO_Init(pBspUartCfg->UART_RX_GPIO_BASE, pBspUartCfg->UART_RX_GPIO_Pin, GPIO_Mode_In_PU_No_IT);
    /* USART configuration */
    USART_StopBits = (pBspUartCfg->stop == 2) ? USART_StopBits_2 : USART_StopBits_1;
    USART_WordLength = (pBspUartCfg->data == 9) ? USART_WordLength_9b : USART_WordLength_8b;
    USART_Parity = (pBspUartCfg->parity < 3) ? parityArr[pBspUartCfg->parity] : USART_Parity_No;
    USART_Init(pBspUartCfg->USARTNum, pBspUartCfg->baud, (USART_WordLength_TypeDef )USART_WordLength,
               (USART_StopBits_TypeDef )USART_StopBits, (USART_Parity_TypeDef )USART_Parity,
               (USART_Mode_TypeDef )USART_Mode_Rx | USART_Mode_Tx);
    USART_ITConfig(pBspUartCfg->USARTNum, USART_IT_RXNE, ENABLE);
    USART_ITConfig(pBspUartCfg->USARTNum, USART_IT_IDLE,
                   ENABLE);   //ʹ�ܴ��ڿ����жϣ�����һ֡���ʱ�����ж�
    USART_ITConfig(pBspUartCfg->USARTNum, USART_IT_TC, DISABLE);//�رմ��ڷ�������ж�
    if(pBspUartCfg->USARTNum == USART1)
    {
        SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA, ENABLE);
    }
    /* Enable USART */
    USART_Cmd(pBspUartCfg->USARTNum, ENABLE);


}

/*******************************************************************************
* Function Name : uint32_t BSP_UartRead(uint8_t COMx, uint8_t *buffter, uint32_t len)
* Description   : ���ڶ�������
* Input         : COMx:ͨ��X
* Output        :
* Other         :
* Date          : 2013.07.22
*******************************************************************************/
uint32_t BSP_UartRead(QUEUE8_t *uartRxBufQueue, uint8_t *buffter, uint32_t len)
{
    return QUEUE_PacketOut(uartRxBufQueue, buffter, len);
}

/*******************************************************************************
* Function Name : void BSP_UartWrite(uint8_t COMx, uint8_t *buffter)
* Description   : ���ڷ�������
* Input         : COMx:ͨ��X
* Output        :
* Other         :
* Date          : 2013.07.22
*******************************************************************************/
void BSP_UartSendByte(USART_TypeDef* USARTNum, uint8_t Data)
{
    USART_SendData8(USARTNum, Data);
    /* �ȴ�������� */
    while((USARTNum->SR & 0x80) ==
          0x00);//��ѯ���ͻ��������ֽ��Ƿ��Ѿ����ͳ�ȥ
}

void BSP_UartSendBytes(USART_TypeDef* USARTNum, uint8_t *buffter)
{
    if(buffter == NULL)
    {
        return ;
    }
    while(*buffter != '\0')
    {
        BSP_UartSendByte(USARTNum, *buffter++);    //ѭ�����÷���һ���ַ����� 
    }
}
void BSP_UartSendBytesLen(USART_TypeDef* USARTNum, uint8_t *buffter,
                          uint8_t len)        //�����ڷ��������а�������0x00
{
    if(buffter == NULL)
    {
        return ;
    }

    while(len)
    {
        BSP_UartSendByte(USARTNum, *buffter++);    //ѭ�����÷���һ���ַ�����
        len--;
    }
}











