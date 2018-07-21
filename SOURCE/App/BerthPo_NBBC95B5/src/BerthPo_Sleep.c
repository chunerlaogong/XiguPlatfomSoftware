#include "BerthPo_Sleep.h"
#include "Drivers_LED.h"
#include "BerthPo_Common.h"
#include "Delay.h"
/***************�����ⲿ����****************/
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
    //ROUSE_IRQ_set;                                                //���ⲿ�����ж�����
    enableInterrupts();                                             //���ж�,���ж϶�����˯�߱����Ѻ�ִ��
    BSP_RtcGoSleep(m_DelayCount);                                   //rtc˯��ʱ��
    //BSP_RtcDeepSleep();                                             //����˯��
    //ROUSE_IRQ_clr;                                                //���ⲿ�����ж�����
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
    if(tagConfigSymple.startUpGetBottom == 1)                            //�豸�Ѿ����������ÿ�ο�����ȡһ�α��ף��������شŹ���
    {	
    	tagConfigSymple.startUpGetBottom = 0;
        DelayMs(10);
        if (BerthPo_SetRm3100Base() == 1)   //��ȡ����
        {
            //��ʼ������
            tagConfigSymple.sendNodeCount = tagConfigSymple.sendPackCount -
                                     1;  //���У׼�ɹ���������һ�ַ������ݹ��ֳֻ��鿴
        }
    }
    BerthPo_AlarmJude();
    tagConfigSymple.batteryCount--;   //��ؼ�⻽�����ڼ�����
}