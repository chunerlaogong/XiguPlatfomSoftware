#include "Drivers_I410ES.h"
#include "Delay.h"

//��������
BT_I410ES_OPERATION_TypeDef BT_I410ES_Operation =
{
    .BT_I410ES_Init =   Drivers_BTI410ESInit,
    .BT_I410ES_PowerOn = Drivers_BTI410ESPowerOn,
    .BT_I410ES_PowerOff = Drivers_BTI410ESPowerOff,
    .BT_I410ES_Read =  Drivers_BTI410ESRead,
    .BT_I410ES_Write = Drivers_BTI410ESWrite,
    .BT_I410ES_BufReset = Drivers_BTI410ESBufReset,
    .BT_I410ES_PowerReset = Drivers_BTI410EPowerReset,
};
static SBSP_UART_CFG_TypeDef G_BT_UART_CONFIG;
uint8_t        G_BT_uartRxBuf[BT_UART_RX_BUF_SIZE];      //���ڷ��ͻ�����

/*******************************************************************************
* Function Name :void Drivers_BTI410ESInit(QUEUE8_t*    pUartRxQueue, uint8_t* pUartRxBuf)
* Description   :BTI410ES ��ʼ������ģ�鴮�ڲ���
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          :2018.06.28
*******************************************************************************/
void Drivers_BTI410ESInit()
{
    memset(G_BT_uartRxBuf, 0, BT_UART_RX_BUF_SIZE);
    G_BT_UART_CONFIG.USARTNum = USART3;
    G_BT_UART_CONFIG.CLKPeripheralNum = CLK_Peripheral_USART3;
    G_BT_UART_CONFIG.UART_TX_GPIO_BASE = GPIOE;
    G_BT_UART_CONFIG.UART_TX_GPIO_Pin  = GPIO_Pin_6;
    G_BT_UART_CONFIG.UART_RX_GPIO_BASE = GPIOE;
    G_BT_UART_CONFIG.UART_RX_GPIO_Pin  = GPIO_Pin_7;
    G_BT_UART_CONFIG.baud = 115200;
    G_BT_UART_CONFIG.data = 8;
    G_BT_UART_CONFIG.stop = 1;
    G_BT_UART_CONFIG.parity = 0;
    BSP_UartOpen(&G_BT_UART_CONFIG);
}
/*******************************************************************************
* Function Name :void Drivers_BTI410ESPowerOn()
* Description   :BTI410ES ��ģ���Դ��ʹ�ܴ���
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          :2018.06.28
*******************************************************************************/

void Drivers_BTI410ESPowerOn(PBT_I410ES_GPIO_TypeDef pBTI410ES_GPIO)
{
    BT_I410ES_PWR_ON(pBTI410ES_GPIO);
    BT_I410ES_Operation.BT_I410ES_Init();
}
/*******************************************************************************
* Function Name :void Drivers_BTI410ESPowerOff(SpBT_I410ES_GPIO_TypeDef pBT_I410ES_GPIO)
* Description   :BTI410ES �ر�ģ���Դ
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          :2018.06.28
*******************************************************************************/

void Drivers_BTI410ESPowerOff(PBT_I410ES_GPIO_TypeDef pBTI410ES_GPIO)
{
    BT_I410ES_PWR_OFF(pBTI410ES_GPIO);
    USART_DeInit(G_BT_UART_CONFIG.USARTNum);
    CLK_PeripheralClockConfig((CLK_Peripheral_TypeDef )
                              G_BT_UART_CONFIG.CLKPeripheralNum, DISABLE);
    USART_Cmd(G_BT_UART_CONFIG.USARTNum, DISABLE);
}

/*******************************************************************************
* Function Name : void _RXIRQHandler(uint8_t COM)
* Description   : �������ڽ����жϺ���
* Input         :
* Output        :
* Other         :
* Date          : 2013.08.23
*******************************************************************************/
INTERRUPT_HANDLER(TIM3_CC_USART3_RX_IRQHandler, 22)
{
    volatile unsigned char clear;
    static uint8_t m_rxCount = 0;
    if (USART_GetITStatus(G_BT_UART_CONFIG.USARTNum, USART_IT_RXNE) != RESET)
    {
        unsigned char ch = G_BT_UART_CONFIG.USARTNum->DR;
        G_BT_uartRxBuf[m_rxCount++] = ch;
    }
    else if (USART_GetITStatus(G_BT_UART_CONFIG.USARTNum, USART_IT_IDLE) != RESET)
    {
        BT_I410ES_Operation.BT_I410ES_CallBack();
        //memset(G_BT_uartRxBuf, 0, BT_UART_RX_BUF_SIZE);
        clear = G_BT_UART_CONFIG.USARTNum->SR;
        clear = G_BT_UART_CONFIG.USARTNum->DR;
        m_rxCount = 0;
    }
}
/*******************************************************************************
* Function Name : void _RXIRQHandler(uint8_t COM)
* Description   : ��������������
* Input         :
* Output        :
* Other         :
* Date          : 2013.08.23
*******************************************************************************/

void Drivers_BTI410ESRead(uint8_t *readBuf)        //BT_I410ES ������
{
    memcpy(readBuf, G_BT_uartRxBuf, BT_UART_RX_BUF_SIZE);
}
/*******************************************************************************
* Function Name : void _RXIRQHandler(uint8_t COM)
* Description   : ����������д����
* Input         :
* Output        :
* Other         :
* Date          : 2013.08.23
*******************************************************************************/

void Drivers_BTI410ESWrite(uint8_t *sendBuf,
                           uint16_t sendLen)              //BT_I410ES д����
{
    BSP_UartSendBytesLen(G_BT_UART_CONFIG.USARTNum, sendBuf,
                         sendLen);   //�����ڷ��������а�������0x00
}

void Drivers_BTI410ESBufReset()
{
    memset(G_BT_uartRxBuf, 0, BT_UART_RX_BUF_SIZE);
}

void Drivers_BTI410EPowerReset(PBT_I410ES_GPIO_TypeDef pBTI410ES_GPIO)
{
    BT_I410ES_Operation.BT_I410ES_PowerOff(pBTI410ES_GPIO);
    DelayMs(1000);
    BT_I410ES_Operation.BT_I410ES_PowerOn(pBTI410ES_GPIO);
}

