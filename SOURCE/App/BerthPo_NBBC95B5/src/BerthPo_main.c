/*
********************************************************************************
*
*                                 APP.c
*
* File          : APP.c
* Version       : V1.0
* Author        : Yjd
* Mode          :
* Toolchain     :
* Description   : 主程序入口
*
* History       :
* Date          : 2018.05.07
*******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "BerthPo_Init.h"
#include "BerthPo_CompileFiles.h"
#include "BerthPo_Main.h"
#include "BerthPo_NB.h"
#include "Delay.h"
#include "BSP_TIM.h"
#include "BSP_GPIO.h"
#include "Drivers_BC95.h"
#include "BSP_UART.h"
#include "BSP_VMUART.h"
#include "Delay.h"
#include "BSP_GPIO_I2C.h"
#include "Drivers_I410ES.h"
#include "Drivers_NFC.h"
#include "BerthPo_Sleep.h"
/*********************变量定义************************/
SCONTROL_SYMPLE tagConfigSymple = {0};
LED_GPIO_TypeDef LED_GPIO = {0};              //LED控制管脚定义
SRM3100_GPIO_TypeDef RM3100_GPIO = {0};       //RM3100管脚定义
SBT_I410ES_GPIO_TypeDef BTI410ES_GPIO = {0};  //蓝牙模块管脚定义
SBC95_GPIO_TypeDef     BC95_GPIO = {0};                         //定义NB模块
SSENSOR_RM3100 sensorRm3100 = {0};
SNET_MUTUAL_INFO  netMutualInfo = {0};
SCONTROL_CONFIG  controlConfig = {0};
SBERTHPO_PARK_STATUS parkStatus = {0};        //泊位宝标签停车状态
extern BT_I410ES_OPERATION_TypeDef BT_I410ES_Operation;

/*******************************************************************************
* Function Name : int main(void)
* Description   : 主程序入口
* Input         :
* Output        :
* Other         :
* Date          : 2018.05.07
*******************************************************************************/
int main(void)
{
    disableInterrupts();               //关全局中断
    BerthPo_InitMcu();                 //初始化泊位宝MCU
    BerthPo_InitSysParam();
    enableInterrupts();
#if SX1280_TEST_MODE
    uint8_t m_SX1280TestData[9] = {2, 3, 4};
    while(1)
    {
        DelayMs(500);
        SX1280_Operation.SX1280_SendBuff(m_SX1280TestData, 3);
    }
#endif
    while(1)
    {
        //memset(&netMutualInfo, 0, sizeof(netMutualInfo));
        //memset(&controlConfig, 0, sizeof(controlConfig));
        //BerthPo_WriteParamToFlash();
        while(controlConfig.workMode == BERTHPO_MODE_FACTORY)   //开机默认进入产测模式
        {
            if(BerthPo_FactoryTest() == 1)   //产测测试通过
            {
                controlConfig.workMode = BERTHPO_MODE_DEEP_SLEEP;   //产测通过进入深度睡眠模式
                BerthPo_WriteParamToFlash();   //保存产测通过标志
                BerthPo_DeepSleep();           //进入深度睡眠状态
            }
        }
        while(controlConfig.workMode == BERTHPO_MODE_ACTIVE)
        {
            BerthPo_WakeFromSleep();   //车位状态判断
            BerthPo_Sleep();
        }
    }

}








