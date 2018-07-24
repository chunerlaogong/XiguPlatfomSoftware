#include "BerthPo_SX1280_Sleep.h"
#include "Drivers_LED.h"
#include "BerthPo_SX1280_Common.h"
#include "BSP_RTC.h"
#include "Drivers_NFC.h"
/***************引用外部变量****************/
extern SBERTHPO_PARK_STATUS parkStatus;
extern SCONTROL_CONFIG  controlConfig;
extern LED_GPIO_TypeDef LED_GPIO;
extern SCONTROL_SYMPLE tagConfigSymple;
extern NFC_OPERATION_TypeDef NFC_Operation;
void BerthPo_Sleep(void)
{
    int m_DelayCount = 0;
    if (parkStatus.fastGetParkCount > 0)
    {

        m_DelayCount = 2000 + tagConfigSymple.randomDelay;
        parkStatus.fastGetParkCount--;
    }
    else
    {
        m_DelayCount = controlConfig.paramConfig.wdtInterval * 2000 +
                       tagConfigSymple.randomDelay;
    }
    enableInterrupts();                                             //开中断,关中断动作在睡眠被唤醒后执行
    RTCAlarm_Set(m_DelayCount);                                     //rtc睡眠时间
    BSP_RtcDeepSleep();                                             //进入睡眠
}
void BerthPo_DeepSleep()
{
    //产测模式通过，LED1和LED2闪烁5次后，进入深度睡眠
    for(int i = 0; i < 3; i++)
    {
        Led_Operation.LedOn(LED_GPIO, 1);
        Led_Operation.LedOn(LED_GPIO, 2);
        DelayMs(500);
        Led_Operation.LedOff(LED_GPIO, 1);
        Led_Operation.LedOff(LED_GPIO, 2);
        DelayMs(500);
    }
    GPIO_Init(GPIOA, 0XFF, GPIO_Mode_Out_OD_Low_Slow);
    GPIO_Init(GPIOB, 0XFF, GPIO_Mode_Out_OD_Low_Slow);
    GPIO_Init(GPIOC, 0XFF, GPIO_Mode_Out_OD_Low_Slow);
    GPIO_Init(GPIOD, 0XFF, GPIO_Mode_Out_OD_Low_Slow);
    GPIO_Init(GPIOE, 0XFF, GPIO_Mode_Out_OD_Low_Slow);
    GPIO_Init(GPIOF, 0XFF, GPIO_Mode_Out_OD_Low_Slow);
    PWR_UltraLowPowerCmd(ENABLE);
    GPIO_Init(GPIOD, GPIO_Pin_1, GPIO_Mode_Out_OD_HiZ_Slow);   //关闭LED
    GPIO_Init(GPIOD, GPIO_Pin_2, GPIO_Mode_Out_OD_HiZ_Slow);
    GPIO_Init(GPIOB, GPIO_Pin_4, GPIO_Mode_Out_OD_HiZ_Slow);   //关闭flash片选
    NFC_Operation.NFC_PowerOn();
    BSP_RtcDeepSleep();
}
void BerthPo_WakeFromSleep()
{
    if(tagConfigSymple.startUpGetBottom ==
       1)                            //设备已经被激活过，每次开机获取一次本底，并开启地磁供电
    {
        DelayMs(10);
        if (BerthPo_SetRm3100Base() == 1)   //获取本底
        {
            //初始化参数
            tagConfigSymple.sendNodeCount = tagConfigSymple.sendPackCount -
                                            1;  //如果校准成功，则在下一轮返回数据供手持机查看
        }
        tagConfigSymple.startUpGetBottom = 0;
    }
    BerthPo_AlarmJude();
    tagConfigSymple.batteryCount--;   //电池检测唤醒周期计数。
}
void BerthPo_NFCCallBack()
{
    controlConfig.workMode = BERTHPO_MODE_ACTIVE;   
	//设备激活以后，关闭中断引脚，防止多次激活
	NFC_Operation.NFC_PowerOff();
	//设备已经激活,LED1闪烁3次
    for(uint8_t i = 0; i < 3; i++)
    {
        Led_Operation.LedOn(LED_GPIO, 1);
        DelayMs(500);
        Led_Operation.LedOff(LED_GPIO, 1);
        DelayMs(500);
    }
    if(BerthPo_FactoryTest() == 1)   //开机自检测试通过，LED2闪烁5次
    {
        for(uint8_t i = 0; i < 3; i++)
        {
            Led_Operation.LedOn(LED_GPIO, 2);
            DelayMs(500);
            Led_Operation.LedOff(LED_GPIO, 2);
            DelayMs(500);
            //void BerthPo_InitMcu();        //从新初始化MCU
            //void BerthPo_InitSysParam();   //初始化系统参数
        }
    }
}