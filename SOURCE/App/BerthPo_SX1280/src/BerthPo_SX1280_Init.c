#include "BerthPo_SX1280_Init.h"
#include "Delay.h"
#include <stdio.h>
#include <stdlib.h>
#include "Drivers_SX1280.h"
#include "BSP_EEPROM.h"
#include "Drivers_NFC.h"
#include "BerthPo_SX1280_Sleep.h"
/***********************引用变量************************/
extern SCONTROL_CONFIG  controlConfig;
extern SNET_MUTUAL_INFO  netMutualInfo;
extern LED_GPIO_TypeDef LED_GPIO;
extern SRM3100_GPIO_TypeDef RM3100_GPIO;
extern SSENSOR_RM3100 sensorRm3100;
extern SCONTROL_SYMPLE tagConfigSymple;
extern NFC_OPERATION_TypeDef NFC_Operation;
/*******************************************************************************
* Function Name : BerthPo_InitMcu()
* Description   : 泊位宝Mcu初始化
* Author        : YJD
* Input         : NULL
* Output        : NULL
* Other         : NULL
* Date          : 2018.05.11
*******************************************************************************/

void BerthPo_InitMcu()
{
    BSP_SysClkInit();      //初始化系统时钟
    BSP_PeriphRtcInit();   //RTC时钟初始化
    //初始化地磁
    RM3100_GPIO.MEG_PRW1_GPIO_BASE =
        GPIOC;             //PC2/PC3/PC4-设置为输出,地磁供电
    RM3100_GPIO.MEG_PRW1_GPIO_Pin = GPIO_Pin_3;
    RM3100_GPIO.MEG_PRW2_GPIO_BASE = GPIOC;
    RM3100_GPIO.MEG_PRW2_GPIO_Pin = GPIO_Pin_2;
    RM3100_GPIO.MEG_PRW3_GPIO_BASE = GPIOC;
    RM3100_GPIO.MEG_PRW3_GPIO_Pin = GPIO_Pin_4;
    RM3100_GPIO.I2C_EN_GPIO_BASE = GPIOA;                //使能I2C
    RM3100_GPIO.I2C_EN_GPIO_Pin = GPIO_Pin_7;
    RM3100_GPIO.MCU_DRDY_GPIO_BASE = GPIOD;
    RM3100_GPIO.MCU_DRDY_GPIO_Pin = GPIO_Pin_6;
    RM3100_GPIO.gpioI2cStructer.GPIO_SDA_BASE = GPIOC;   //配置I2C管脚
    RM3100_GPIO.gpioI2cStructer.GPIO_SDA_PIN = GPIO_Pin_0;
    RM3100_GPIO.gpioI2cStructer.GPIO_SCL_BASE = GPIOC;
    RM3100_GPIO.gpioI2cStructer.GPIO_SCL_PIN = GPIO_Pin_1;
    RM3100_GPIO.gpioI2cStructer.SLAVE_ADDRESS = 0x20;
    RM3100_Operation.RM3100_PowerOn(&(RM3100_GPIO));
    //初始化LED管脚
    LED_GPIO.LED1_GPIO_BASE = GPIOD;
    LED_GPIO.LED1_GPIO_Pin = GPIO_Pin_1;
    LED_GPIO.LED2_GPIO_BASE = GPIOD;
    LED_GPIO.LED2_GPIO_Pin = GPIO_Pin_2;
    //初始化NFC需要在1280之前
    NFC_Operation.NFC_CallBack = BerthPo_NFCCallBack;
    NFC_Operation.NFC_NFCInit();
    NFC_Operation.NFC_PowerOn();
    //初始化SX1280
    SX1280_Operation.SX1280_PowerOn();
    //接通电源LED1和LED2闪烁一次
	Led_Operation.LedOn(LED_GPIO, 1);   
    Led_Operation.LedOn(LED_GPIO, 2);
	DelayMs(500);
	Led_Operation.LedOff(LED_GPIO, 1);   
    Led_Operation.LedOff(LED_GPIO, 2);

}

/*******************************************************************************
* Function Name : void BerthPo_SysParamInit()
* Description   : 系统运行需要的相关参数初始化
* Author        : YJD
* Input         : NULL
* Output        : NULL
* Other         : NULL
* Date          : 2018.05.11
*******************************************************************************/

void BerthPo_InitSysParam()
{
    tagConfigSymple.startUpGetBottom = 1;      //开机初始化获取本底
    BerthPo_ReadParamFromFlash();
    if(netMutualInfo.paraIntFlag[0] == 0x5A
       && netMutualInfo.paraIntFlag[1] == 0xA5
       && netMutualInfo.paraIntFlag[2] == 0x65)
    {
        nop();
    }
    else
    {
        controlConfig.workMode = BERTHPO_MODE_FACTORY;   //初始化为场测模式;
        controlConfig.nodeConfig.idNumber[0] = 0x01;          //初始化泊位宝UID
        controlConfig.nodeConfig.idNumber[1] = 0x02;
        controlConfig.nodeConfig.idNumber[2] = 0x03;
        controlConfig.nodeConfig.idNumber[3] = 0x00;
        controlConfig.nodeConfig.idNumber[4] = 0x00;
        controlConfig.nodeConfig.idNumber[5] = 0x00;
        controlConfig.nodeConfig.keyNumber[0] = 0x01;         //初始化泊位宝KEYID
        controlConfig.nodeConfig.keyNumber[1] = 0x02;
        controlConfig.nodeConfig.keyNumber[2] = 0x03;
        controlConfig.nodeConfig.keyNumber[3] = 0x00;
        controlConfig.nodeConfig.keyNumber[4] = 0x00;
        controlConfig.nodeConfig.keyNumber[5] = 0x00;
        controlConfig.paramConfig.alarmValid =
            0x4C00;    //有效标志,第10位有车，第11位无车，第14位频繁快速唤醒报警
        controlConfig.nodeConfig.ledFlag = 0;             //led开启状态
        controlConfig.paramConfig.getEMBottom_RFFlag =
            0x01;           //0x01为打开地磁检测，
        controlConfig.paramConfig.getEMBottom_RFModThreshold =
            10;     //磁场强度灵敏度
        controlConfig.paramConfig.getEMBottom_RFAngleThreshold =
            10;   //磁场偏转角度灵敏度,10度
        controlConfig.paramConfig.fastRouse =
            0;                       //快速唤醒标志
        controlConfig.paramConfig.fastRouseAlarm =
            0;                      //多次快速唤醒报警标志
        controlConfig.paramConfig.wdtInterval =
            5;                     //WDT固定睡眠时间 unit:s
        controlConfig.paramConfig.heartbeatInterval =
            5;               //发送心跳间隔 unit:min 默认5分钟
        netMutualInfo.paraIntFlag[0] = 0x5A;
        netMutualInfo.paraIntFlag[1] = 0xA5;
        netMutualInfo.paraIntFlag[2] = 0x65;

        BerthPo_WriteParamToFlash();
        //memset(netMutualInfo, 0, sizeof(netMutualInfo));
        //memset(controlConfig, 0, sizeof(controlConfig));
        BerthPo_ReadParamFromFlash();
        nop();
    }

}


