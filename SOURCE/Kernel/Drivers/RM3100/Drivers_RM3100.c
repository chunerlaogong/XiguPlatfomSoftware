#include "Drivers_RM3100.h"
#include "BSP_I2C.h"
#include "Sleep.h"
#include "Delay.h"
#include "BSP_RTC.h"
/*******************************************************************************
* Function Name :void Drivers_RM3100_PowerOn(PRM3100_GPIO_TypeDef pRM3100_GPIO)
* Description   :打开RM3100电源
* Author        : YJD
* Input         : RM3100_GPIO_TypeDef RM3100_GPIO
* Output        :
* Other         :
* Date          :2018.06.27
*******************************************************************************/
void Drivers_RM3100_PowerOn(PRM3100_GPIO_TypeDef pRM3100_GPIO)
{
    RM3100_POWER_ON(pRM3100_GPIO);
    Drivers_RM3100_I2CEnable(pRM3100_GPIO);
    RM3100_I2C_SDA_SCL_ON(pRM3100_GPIO->gpioI2cStructer);
    Simulation_I2C.I2cInit(&(pRM3100_GPIO->gpioI2cStructer));
    Drivers_RM3100_I2CEnable(pRM3100_GPIO);
}
/*******************************************************************************
* Function Name :void Drivers_RM3100PowerOff(RM3100_GPIO_TypeDef RM3100_GPIO)
* Description   :关闭RM3100电源
* Author        : YJD
* Input         : RM3100_GPIO_TypeDef RM3100_GPIO
* Output        :
* Other         :
* Date          :2018.06.27
*******************************************************************************/
void Drivers_RM3100_PowerOff(PRM3100_GPIO_TypeDef pRM3100_GPIO)
{
    RM3100_POWER_OFF(pRM3100_GPIO);
    RM3100_I2C_EN_OFF(pRM3100_GPIO);
    RM3100_I2C_SDA_SCL_OFF(pRM3100_GPIO->gpioI2cStructer);
    RM3100_MCURDRY_OFF(pRM3100_GPIO);
}
/*******************************************************************************
* Function Name :void Drivers_RM3100I2CEnable(RM3100_GPIO_TypeDef RM3100_GPIO)
* Description   :RM3100 I2C使能
* Author        : YJD
* Input         : RM3100_GPIO_TypeDef RM3100_GPIO
* Output        :
* Other         :
* Date          :2018.06.27
*******************************************************************************/
void Drivers_RM3100_I2CEnable(PRM3100_GPIO_TypeDef pRM3100_GPIO)
{
    RM3100_I2C_EN_ON(pRM3100_GPIO);

}

/*******************************************************************************
* Function Name :void Drivers_RM3100I2CDisable(RM3100_GPIO_TypeDef RM3100_GPIO)
* Description   :RM3100 I2C禁止
* Author        : YJD
* Input         : RM3100_GPIO_TypeDef RM3100_GPIO
* Output        :
* Other         :
* Date          :2018.06.27
*******************************************************************************/
void Drivers_RM3100_I2CDisable(PRM3100_GPIO_TypeDef pRM3100_GPIO)
{
    RM3100_I2C_EN_OFF(pRM3100_GPIO);
}


/*******************************************************************************
* Function Name :void Drivers_RM3100SetMeaureMode(int mode)
* Description   :RM3100 设置测量模式
* Author        : YJD
* Input         : mode  需要设置的模式1-n
* Output        :
* Other         :
* Date          :2018.06.29
*******************************************************************************/
void Drivers_RM3100_SetMeaureMode(PRM3100_GPIO_TypeDef pRM3100_GPIO,
                                  uint8_t mode)
{
    uint8_t m_writeData[2] = {0};
    switch(mode)
    {
        case 1:
            m_writeData[0] = 0x70;
            BSP_I2cWriteBytes(0, &(pRM3100_GPIO->gpioI2cStructer), m_writeData, 1);
            break;
        default:
            break;
    }
}
/*******************************************************************************
* Function Name :void Drivers_RM3100_McuRdRyOn(RM3100_GPIO_TypeDef RM3100_GPIO)
* Description   :RM3100 MUC_RDRY中断使能
* Author        : YJD
* Input         : mode  需要设置的模式1-n
* Output        :
* Other         :
* Date          :2018.06.29
*******************************************************************************/
void Drivers_RM3100_McuRdRyOn(PRM3100_GPIO_TypeDef pRM3100_GPIO)
{
    RM3100_MCURDRY_ON(pRM3100_GPIO);
}
/*******************************************************************************
* Function Name :void Drivers_RM3100_McuRdRyOff(RM3100_GPIO_TypeDef RM3100_GPIO)
* Description   :RM3100 MUC_RDRY中断禁止
* Author        : YJD
* Input         : mode  需要设置的模式1-n
* Output        :
* Other         :
* Date          :2018.06.29
*******************************************************************************/
void Drivers_RM3100_McuRdRyOff(PRM3100_GPIO_TypeDef pRM3100_GPIO)
{
    RM3100_MCURDRY_ON(pRM3100_GPIO);
}
/*******************************************************************************
* Function Name :uint8_t Drivers_RM3100_TestMode(void)
* Description   :模块测试模式
* Author        : YJD
* Input         :
* Output        : 0-测试通过/1-测试失败
* Other         :
* Date          :2018.06.29
*******************************************************************************/

uint8_t Drivers_RM3100_TestMode(PRM3100_GPIO_TypeDef pRM3100_GPIO)
{

    uint8_t m_tempBuf[9] = {0};
    uint8_t i = 0;
    uint8_t m_trySendTimes = 10;    //尝试10次
    RM3100_Operation.RM3100_PowerOff(
        pRM3100_GPIO);               //关闭RM3100所有端口节能
    Drivers_RM3100_PowerOn(pRM3100_GPIO);
    DelayMs(100);                       //上电延时等待R3100准备，小于30us读出数据错误，典型值200us
    enableInterrupts();   //开中断,关中断动作在睡眠被唤醒后执行
    RTCAlarm_Set(30);   //rtc睡眠时间,15ms
    Drivers_RM3100_SetMeaureMode(pRM3100_GPIO, 1);  //设置单一测量模式
    DelayMs(100);
    while(m_trySendTimes--)
    {
        BSP_I2cReadBytes(0x24, &(pRM3100_GPIO->gpioI2cStructer), m_tempBuf, 9);
        for(i = 0; i < 9; i++)
        {
            if(m_tempBuf[i] != 0)
            {
                return 1;
            }
        }
    }
    return 0;

}



