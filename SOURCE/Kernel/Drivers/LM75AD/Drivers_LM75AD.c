/*
********************************************************************************
*
*                                 LM75AD.c
*
* File          : LM75AD.c
* Version       : V1.0
* Author        : Lhl
* Mode          : Thumb2
* Toolchain     : 
* Description   : LM75AD��������
*                
* History       :
* Date          : 2018.05.08
*******************************************************************************/
#include "LM75AD.h"
#include "BSP_Uart.h"
#include "APP.h"
#include "Delay.h"
/*******************************************************************************
* Function Name : uint16_t LM75AD_GetTemp(void)
* Description   : ��ȡLM75AD���¶�ֵ (�Ŵ�1000��)
* Author        : Lhl
* Input         : Nothing
* Output        : �¶�
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
int16_t LM75AD_GetTemp(void)
{
    static uint8_t GetTemp[3] = {0};
    static uint16_t LM75AD_Temp = 0;
    GPIO_I2C_TypeDef gpioI2cStructer;
    gpioI2cStructer.GPIO_SCL_BASE = (GPIO_TypeDef*)GPIOC;
    gpioI2cStructer.GPIO_SCL_PIN = GPIO_Pin_0;
    gpioI2cStructer.GPIO_SDA_BASE = (GPIO_TypeDef*)GPIOC;
    gpioI2cStructer.GPIO_SDA_PIN = GPIO_Pin_1;
    gpioI2cStructer.SLAVE_ADDRESS = 0x48;
    BSP_I2cInit(gpioI2cStructer);
    DelayUs(5);
    BSP_I2cReadBytes(0x48, 0x00,gpioI2cStructer,GetTemp, 2);
    LM75AD_Temp = GetTemp[0];
    LM75AD_Temp = (LM75AD_Temp << 8 | GetTemp[1]) >>5;
    if(LM75AD_Temp > 1016)//�ж��Ƿ�Ϊ��ֵ
    {
        LM75AD_Temp = (~LM75AD_Temp) & 0x07ff;
        LM75AD_Temp = LM75AD_Temp + 1;
        LM75AD_Temp = -LM75AD_Temp * 125;
            
    }
    else//���Ǹ�ֵ
    {
        LM75AD_Temp = LM75AD_Temp *125;
    }
    printf("LM75AD��ȡ�����¶�:%d\r\n" , LM75AD_Temp);
    return LM75AD_Temp;
}


