#include "BerthPo_SX1280_Main.h"
#include "BerthPo_SX1280_Init.h"
#include "BerthPo_SX1280_CompileFiles.h"
#include "BSP_TIM.h"
#include "BSP_GPIO.h"
#include "BSP_UART.h"
#include "Delay.h"
#include "BSP_GPIO_I2C.h"
#include "BerthPo_SX1280_Sleep.h"
#include "Drivers_NFC.h"
#include "Drivers_SX1280.h"
#include "BSP_RTC.h"
SCONTROL_SYMPLE tagConfigSymple = {0};
LED_GPIO_TypeDef LED_GPIO = {0};              //LED控制管脚定义
SRM3100_GPIO_TypeDef RM3100_GPIO = {0};       //RM3100管脚定义
SSENSOR_RM3100 sensorRm3100 = {0};
SNET_MUTUAL_INFO  netMutualInfo = {0};
SCONTROL_CONFIG  controlConfig = {0};
SBERTHPO_PARK_STATUS parkStatus = {0};        //泊位宝标签停车状态

int main()
{
    disableInterrupts();               //关全局中断
    BerthPo_InitMcu();                 //初始化泊位宝MCU
    enableInterrupts();
    BerthPo_InitSysParam();
    enableInterrupts();
    GPIOD->DDR |=  GPIO_Pin_7;
    GPIOD->ODR |=  GPIO_Pin_7;
    DelayMs(100);
    GPIOD->ODR &=  ~GPIO_Pin_7;
#if SX1280_TEST_MODE
    uint8_t m_SX1280TestData[9] = {1, 2, 3, 4, 5, 6, 7, 8};
    while(1)
    {
        DelayMs(2000);
        SX1280_Operation.SX1280_SendBuff(m_SX1280TestData, 8);
    }
#endif
    while(1)
    {
        while(controlConfig.workStatus == BERTHPO_MODE_ACTIVE)
        {
            BerthPo_WakeFromSleep();                   //车位状态判断
            BerthPo_Sleep();
        }
        NFC_INT_SET;                                   //开外部唤醒中断输入
        //deep_sleep();                                //出厂状态,睡眠等待唤醒
        NFC_INT_CLR;
    }
}