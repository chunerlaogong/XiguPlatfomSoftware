/*
********************************************************************************
*
*                                 bsp.c
*
* File          : bsp.c
* Version       : V1.0
* Author        : whq
* Mode          : Thumb2
* Toolchain     :
* Description   :   系统初始化
                    IO初始化
                    时钟初始化
*
* History       :
* Date          : 2018.05.07
*******************************************************************************/
#include "stm8l15x_gpio.h"
#include "stm8l15x_iwdg.h"
#include "BSP_GPIO.h"

static PORT_INFO_t g_PortInfo[eGPIO_MAX_COUNT] =
{  
    {GPIOE,  GPIO_Pin_1,     3,     ePWR_BT},
    {GPIOD,  GPIO_Pin_1,     3,     eLED_BT},

};
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration()
{
    GPIO_Mode_TypeDef GPIO_Mode;
    PORT_INFO_t *pPort;

    for (pPort = g_PortInfo; pPort < (eGPIO_MAX_COUNT + g_PortInfo); pPort++)
    {
//        RCC_AHB1PeriphClockCmd(pPort->clk, ENABLE);

        if (pPort->io == 0)
            GPIO_Mode = GPIO_Mode_In_FL_No_IT;
        else if (pPort->io == 1)
            GPIO_Mode = GPIO_Mode_In_FL_IT;
        else if (pPort->io == 2)
            GPIO_Mode = GPIO_Mode_Out_OD_Low_Fast;
        else if (pPort->io == 3)
            GPIO_Mode = GPIO_Mode_Out_PP_Low_Fast;
        else if (pPort->io == 4)
            GPIO_Mode = GPIO_Mode_Out_OD_Low_Slow;
        else if (pPort->io == 5)
            GPIO_Mode = GPIO_Mode_Out_OD_HiZ_Fast;
        else if (pPort->io == 6)
            GPIO_Mode = GPIO_Mode_Out_PP_High_Fast;
        else if (pPort->io == 7)
            GPIO_Mode = GPIO_Mode_Out_OD_HiZ_Slow;
        else if (pPort->io == 8)
            GPIO_Mode = GPIO_Mode_Out_PP_High_Slow;

        GPIO_Init(pPort->port, pPort->pin, GPIO_Mode);
    }
}

/*******************************************************************************
* Function Name :void SystemConfig(void)
* Description   :
* Input         :
* Output        :
* Other         :
* Date          :2018.05.07
*******************************************************************************/
void BSP_Init()
{
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_LSI); //选择HSI 16MHZ作为时钟源
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);//1分频
}


/*******************************************************************************
* Function Name : void BSP_GpioSet(int n)
* Description   : IO置1
* Input         :
* Output        :
* Other         :
* Date          : 2013.07.21
*******************************************************************************/
void BSP_GpioSet(PORT_INFO_t portInfo)
{
    portInfo.port->ODR |= portInfo.pin;
}

/*******************************************************************************
* Function Name : void BSP_GpioReset(int n)
* Description   : IO置0
* Input         :
* Output        :
* Other         :
* Date          : 2018.05.07
*******************************************************************************/
void BSP_GpioReset(PORT_INFO_t portInfo)
{
    portInfo.port->ODR &= (~(portInfo.pin));
}

/*******************************************************************************
* Function Name : uint8_t BSP_GpioRead(uint16_t n)
* Description   : 读IO口当前逻辑电平
* Input         : index
* Output        : -1)error   0)reset    1)set
* Other         :
* Date          : 2013.09.26
*******************************************************************************/
uint8_t BSP_GpioRead(PORT_INFO_t portInfo)
{
    return GPIO_ReadInputDataBit(portInfo.port, portInfo.pin);
}

