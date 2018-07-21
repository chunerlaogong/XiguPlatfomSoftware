#include "BerthPo_Sleep.h"
#include "Drivers_LED.h"
#include "BerthPo_Common.h"
#include "Delay.h"
/***************引用外部变量****************/
extern SBERTHPO_PARK_STATUS parkStatus;
extern SCONTROL_CONFIG  controlConfig;
extern LED_GPIO_TypeDef LED_GPIO;
extern SCONTROL_SYMPLE tagConfigSymple;
void BerthPo_Sleep()
{
    int m_DelayCount = 0;
    if(controlConfig.nodeConfig.mcuSleepFlag != 0x01)
    {
        return;
    }
    /*if(bluetooth_data.BTEnableFlag == 0x01)
    {
        return;
    }*/
    //Led_Operation.LedOff(LED_GPIO, 1);
    //Led_Operation.LedOff(LED_GPIO, 2);
    //UART_disable();
    if (parkStatus.fastGetParkCount > 0)
    {

        m_DelayCount = 2 * 2400;
        parkStatus.fastGetParkCount--;
    }
    else
    {
        m_DelayCount = controlConfig.paramConfig.wdtInterval * 2301;
    }
    //MEG_PRW_clr;
    //ROUSE_IRQ_set;                                                //开外部唤醒中断输入
    enableInterrupts();                                             //开中断,关中断动作在睡眠被唤醒后执行
    BSP_RtcGoSleep(m_DelayCount);                                   //rtc睡眠时间
    //BSP_RtcDeepSleep();                                             //进入睡眠
    //ROUSE_IRQ_clr;                                                //关外部唤醒中断输入
    //UART_enable();
}

void BerthPo_DeepSleep()
{

}

void BerthPo_WakeFromSleep()
{
    /*if((NBAtCommand.NetworkStatus & 0x80) != 0x80)
    {
        return;
    }
    if(bluetooth_data.BTEnableFlag == 0x01)
    {
        return;
    }*/
    /*if(tagConfigSymple.controlConfig.initNB != 0x01)
    {
        return;
    }*/
    if(tagConfigSymple.startUpGetBottom == 1)                            //设备已经被激活过，每次开机获取一次本底，并开启地磁供电
    {	
    	tagConfigSymple.startUpGetBottom = 0;
        DelayMs(10);
        if (BerthPo_SetRm3100Base() == 1)   //获取本底
        {
            //初始化参数
            tagConfigSymple.sendNodeCount = tagConfigSymple.sendPackCount -
                                     1;  //如果校准成功，则在下一轮返回数据供手持机查看
        }
    }
    BerthPo_AlarmJude();
    tagConfigSymple.batteryCount--;   //电池检测唤醒周期计数。
}