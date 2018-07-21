#include "BSP_VMUART.h"
#include "stm8l15x_it.h"
#include "BSP_GPIO.h"
#include <stdlib.h>





/******************宏定义********************/


/*******************变量定义*****************/
uint8_t g_vm2UartTxFlag;    //正在发送标志
uint8_t g_vm2UartTxByte;   //扩展串口字节缓冲区
uint8_t g_vm2StartSendFlag = 0;   //开始发送标志



void BSP_VmUartInit()
{
#if COM1_SW_EN
    /*sSwUart[COM1_SW].EMUART_PARAM.status = EMUART_IDLE;
    sSwUart[COM1_SW].EMUART_PARAM.recvData = 0;
    sSwUart[COM1_SW].EMUART_PARAM.bitRecved = 0;
    sSwUart[COM1_SW].EMUART_PARAM.startRxFlag = 0;
    sSwUart[COM1_SW].EMUART_PARAM.baudRate = 9600;
    sSwUart[COM1_SW].SYS_CLK = 8;
    GPIO_Init(sSwUartPortInfo[eCOM1_SW_Rx].port, sSwUartPortInfo[eCOM1_SW_Rx].pin,
              GPIO_Mode_In_FL_IT);   //初始化RX中断
    EXTI_SetPinSensitivity(EXTI_Pin_6, EXTI_Trigger_Falling);
    QUEUE_PacketCreate(COM_SW_RX_QUEUE[COM1_SW], COM_SW_RX_BUFF[COM1_SW],
                       COM_SW_RX_BUFF_SIZE[COM1_SW]);*/
#endif
#if COM2_SW_EN
    sSwUart[COM2_SW].EMUART_PARAM.oneRameRcvOK = 0;
    sSwUart[COM2_SW].EMUART_PARAM.status = EMUART_IDLE;   //初始化为模拟串口空闲状态
    sSwUart[COM2_SW].EMUART_PARAM.recvData = 0;
    sSwUart[COM2_SW].EMUART_PARAM.bitRecved = 0;
    sSwUart[COM2_SW].EMUART_PARAM.startRxFlag = 0;
    sSwUart[COM2_SW].EMUART_PARAM.baudRate = 9600;
    sSwUart[COM2_SW].SYS_CLK = 8;
    GPIO_Init(sSwUartPortInfo[eCOM2_SW_Rx].port, sSwUartPortInfo[eCOM2_SW_Rx].pin,
              GPIO_Mode_In_FL_IT);   //初始化RX中断
    EXTI_SetPinSensitivity(EXTI_Pin_6, EXTI_Trigger_Falling);
    BSP_TIM1GpioUartConfig();   //初始化定时器1        //收数据定时器
    BSP_TIM4GpioUartConfig();   //初始化定时器4        //发数据定时器
    QUEUE_PacketCreate(COM_SW_RX_QUEUE[COM2_SW], COM_SW_RX_BUFF[COM2_SW],
                       COM_SW_RX_BUFF_SIZE[COM2_SW]);
#endif

}

/*******************************************************************************
* Function Name : void BSP_TIM4Config(void)
* Description   : 配置定时器4，作为模拟串口2发数据定时器
* Author        : YJD
* Input         : NULL
* Output        : NULL
* Other         : NULL
* Date          : 2018.06.21
*******************************************************************************/
void BSP_TIM4GpioUartConfig()   //定时器4作为模拟串口1的控制定时器
{
#if COM2_SW_EN
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
    if(sSwUart[COM2_SW].EMUART_PARAM.baudRate == 9600)
    {
        if(sSwUart[COM2_SW].SYS_CLK == 16)
        {
            TIM4_TimeBaseInit(TIM4_Prescaler_16,
                              1 * 85);            //16M-104us中断一次 (1/(16M / 128))*125 = 1ms

        }
        if(sSwUart[COM2_SW].SYS_CLK == 8)
        {
            TIM4_TimeBaseInit(TIM4_Prescaler_8, 1 * 80);          //8M-104us中断一次  //80
        }

    }
    TIM4_ClearFlag(TIM4_FLAG_Update);
    TIM4_ITConfig(TIM4_IT_Update, ENABLE);
#endif
}

/*******************************************************************************
* Function Name : void BSP_TIM1GpioUartConfig()
* Description   : 配置定时器1，作为模拟串口2收数据定时器
* Author        : YJD
* Input         : NULL
* Output        : NULL
* Other         : NULL
* Date          : 2018.06.21
*******************************************************************************/
void BSP_TIM1GpioUartConfig()
{
#if COM2_SW_EN
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);
    if(sSwUart[COM2_SW].EMUART_PARAM.baudRate == 9600)
    {
        if(sSwUart[COM2_SW].SYS_CLK == 8)
        {
            TIM1_TimeBaseInit(7, TIM1_CounterMode_Up, 102, 0);   //102
        }
    }
    TIM1_ARRPreloadConfig(ENABLE);         //使能自动重装
    TIM1_ITConfig(TIM1_IT_Update, ENABLE); //数据更新中断
#endif
}
/*******************************************************************************
* Function Name : void BSP_TIM2GpioUartConfig()
* Description   : 配置定时器2，用于判断定串口数据是否接收完成
* Author        : YJD
* Input         : NULL
* Output        : NULL
* Other         : NULL
* Date          : 2018.06.21
*******************************************************************************/
void BSP_TIM2UartRxCrtlConfig()
{
#if COM2_SW_EN | COM1_SW_EN
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
    TIM2_TimeBaseInit(7, TIM2_CounterMode_Up, 120);   //1ms
    TIM2_ARRPreloadConfig(ENABLE);         //使能自动重装
    TIM2_ITConfig(TIM2_IT_Update, ENABLE); //数据更新中断
    TIM2_Cmd(DISABLE);
    TIM2->CNTRH = 0;      //判断一帧数据是否接收完成定时器清零
    TIM2->CNTRL = 0;
#endif
}

/**
  * @brief External IT PIN6 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI6_IRQHandler, 14)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    /* BSP_I2cWriteBytes(0x0, m_gpioI2cStructer, m_writeBuf, 1);
    // BSP_I2cReadBytes(0x0, m_gpioI2cStructer, m_readBuf, 1);
     DelayMs(15);
     BSP_I2cReadBytes(0x24, m_gpioI2cStructer, m_readBuf, 9);
     printf("m_readBuf = %02x", &m_readBuf[0]);
     EXTI_ClearITPendingBit(EXTI_IT_Pin6);*/
#if COM2_SW_EN
    if((sSwUartPortInfo[eCOM2_SW_Rx].port->IDR & sSwUartPortInfo[eCOM2_SW_Rx].pin)
       == 0)
    {
        sSwUart[COM2_SW].EMUART_PARAM.status = EMUART_RECV;
        COM2_SW_RXD_PORT_IN;
        TIM1_Cmd(ENABLE);

    }
    EXTI_ClearITPendingBit(EXTI_IT_Pin6);
#endif
}

INTERRUPT_HANDLER(EXTI1_IRQHandler, 9)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    /*#if COM1_SW_EN
        if((sSwUartPortInfo[eCOM1_SW_Rx].port->IDR & sSwUartPortInfo[eCOM1_SW_Rx].pin)
           == 0)
        {
            sSwUart[COM1_SW].EMUART_PARAM.status = EMUART_RECV;
            COM1_SW_RXD_PORT_IN;
            TIM1_Cmd(ENABLE);

        }
        EXTI_ClearITPendingBit(EXTI_IT_Pin1);
    #endif*/
}

/**
* @brief TIM1 Update/Overflow/Trigger/Commutation Interrupt routine.
* @param  None
* @retval None
*/
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_COM_IRQHandler,
                  23)         //定时器1作为模拟串口0的控制定时器
{

    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
#if COM2_SW_EN
    if(sSwUart[COM2_SW].EMUART_PARAM.status == EMUART_RECV)
    {

        if(TIM1_GetITStatus(TIM1_IT_Update ) == SET)
        {
            disableInterrupts();
            TIM1_ClearITPendingBit(TIM1_IT_Update);
            TIM1_ClearFlag(TIM1_FLAG_Update);
            if(sSwUart[COM2_SW].EMUART_PARAM.startRxFlag == 0)   //起始位
            {
                sSwUart[COM2_SW].EMUART_PARAM.startRxFlag++;
                sSwUart[COM2_SW].EMUART_PARAM.oneRameRcvOK = 0;
                TIM2_Cmd(ENABLE);
            }
            else
            {
                sSwUart[COM2_SW].EMUART_PARAM.bitRecved++;
                if((sSwUart[COM2_SW].EMUART_PARAM.bitRecved >= 1) &&
                   (sSwUart[COM2_SW].EMUART_PARAM.bitRecved <= 8))   //数据位
                {
                    if((sSwUartPortInfo[eCOM2_SW_Rx].port->IDR & sSwUartPortInfo[eCOM2_SW_Rx].pin)
                       == 0)
                    {

                    }
                    else
                    {
                        sSwUart[COM2_SW].EMUART_PARAM.recvData |= (0x1 <<
                                (sSwUart[COM2_SW].EMUART_PARAM.bitRecved - 1));
                    }
                }
                if((sSwUart[COM2_SW].EMUART_PARAM.bitRecved == 9) &&
                   ((sSwUartPortInfo[eCOM2_SW_Rx].port->IDR &
                     sSwUartPortInfo[eCOM2_SW_Rx].pin)))  //停止位
                {
                    sSwUart[COM2_SW].rxTemp[sSwUart[COM2_SW].pRxTempPos++] =
                        sSwUart[COM2_SW].EMUART_PARAM.recvData;
                    sSwUart[COM2_SW].EMUART_PARAM.recvData = 0;
                    sSwUart[COM2_SW].EMUART_PARAM.bitRecved = 0;
                    if(sSwUart[COM2_SW].pRxTempPos > COM2_SW_RX_BUFF_SIZE)
                    {
                        sSwUart[COM2_SW].pRxTempPos = 0;
                    }
                    /*if(sSwUart[COM2_SW].pRxTempPos == 27)
                    {
                        sSwUart[COM2_SW].pRxTempPos = 0;
                    }*/
                    TIM1_Cmd(DISABLE);    //关闭定时器
                    TIM2->CNTRH = 0;      //判断一帧数据是否接收完成定时器清零
                    TIM2->CNTRL = 0;
                    COM2_SW_RXD_PORT_INT_IN;
                    sSwUart[COM2_SW].EMUART_PARAM.status = EMUART_IDLE;
                }
            }
            enableInterrupts();
        }
    }
#endif

}
/**
* @brief TIM2 Update/Overflow/Trigger/Break /USART2 TX Interrupt routine.
* @param  None
* @retval None
*/
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{
    /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
    */
    TIM2_ClearFlag(TIM2_FLAG_Update);
    sSwUart[COM2_SW].EMUART_PARAM.oneRameRcvOK = 1;
    TIM2_Cmd(DISABLE);
}

/**
  * @brief TIM4 Update/Overflow/Trigger Interrupt routine.
  * @param  None
  * @retval None
  */

INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
#if COM2_SW_EN
    if(sSwUart[COM2_SW].EMUART_PARAM.status == EMUART_SEND)
    {
        disableInterrupts();
        static int i = 0, j = 0, k = 0;
        TIM4_ClearFlag(TIM4_FLAG_Update);
        if(1 == g_vm2StartSendFlag)
        {
            if(k < 2)
            {
                IO_RESET(sSwUartPortInfo[eCOM2_SW_Tx]);
                k++;
            }
            if(k < 12)
            {
                IO_SET(sSwUartPortInfo[eCOM2_SW_Tx]);
                k++;
            }
            else
            {
                k = 0;
                g_vm2StartSendFlag = 0;
            }
        }
        else
        {
            TIM4_Cmd(DISABLE);
            if( i == 1)
            {
                IO_RESET(sSwUartPortInfo[eCOM2_SW_Tx]);
                i++;
            }
            if(i == 0)
            {
                i++;
            }
            if(i == 11)
            {
                IO_SET( sSwUartPortInfo[eCOM2_SW_Tx]);
                i = 1;
                g_vm2UartTxFlag = 0;
                TIM4_Cmd(DISABLE);   //发送完一字节，关闭定时器
                return;
            }
            else
            {
                if((i > 2) && (i < 11))
                {
                    if(j >= 8)
                    {
                        j = 0;
                    }
                    if((g_vm2UartTxByte & 0x01) == 0x01)
                    {
                        IO_SET(sSwUartPortInfo[eCOM2_SW_Tx]);
                    }
                    else
                    {
                        IO_RESET(sSwUartPortInfo[eCOM2_SW_Tx]);
                    }
                    g_vm2UartTxByte /= 2;
                    j++;
                    i++;

                }
            }
            if(i == 2)
            {
                i++;
            }
        }
        disableInterrupts();
        TIM4_Cmd(ENABLE);
    }
#endif
}
/*******************************************************************************
* Function Name : void BSP_SimulatUartSendByte(uint8_t data)
* Description   : 模拟串口发送单字节数据
* Input           : COMx:通道X
* Output         :
* Other          :
* Date            : 2018.06.18
*******************************************************************************/
void BSP_Vm2UartSendByte(uint8_t data)
{
    g_vm2StartSendFlag = 1;
    g_vm2UartTxFlag = 1;
    g_vm2UartTxByte = data;
    TIM4_Cmd(ENABLE);
    while(g_vm2UartTxFlag == 1); //检查是否发送完毕
}
void BSP_Vm2UartSendBytes(uint8_t* buffer)
{
    if(buffer == NULL)
    {
        return ;
    }
    g_vm2StartSendFlag = 1;
    sSwUart[COM2_SW].EMUART_PARAM.status = EMUART_SEND;
    while(*buffer != '\0')
    {
        BSP_Vm2UartSendByte(*buffer++);    /*循环调用发送一个字符函数*/
    }
}
