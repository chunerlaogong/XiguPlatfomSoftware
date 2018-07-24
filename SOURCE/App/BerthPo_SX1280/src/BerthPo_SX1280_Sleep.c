#include "BerthPo_SX1280_Sleep.h"
#include "Drivers_LED.h"
#include "BerthPo_SX1280_Common.h"
#include "BSP_RTC.h"
#include "Drivers_NFC.h"
/***************�����ⲿ����****************/
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
    enableInterrupts();                                             //���ж�,���ж϶�����˯�߱����Ѻ�ִ��
    RTCAlarm_Set(m_DelayCount);                                     //rtc˯��ʱ��
    BSP_RtcDeepSleep();                                             //����˯��
}
void BerthPo_DeepSleep()
{
    //����ģʽͨ����LED1��LED2��˸5�κ󣬽������˯��
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
    GPIO_Init(GPIOD, GPIO_Pin_1, GPIO_Mode_Out_OD_HiZ_Slow);   //�ر�LED
    GPIO_Init(GPIOD, GPIO_Pin_2, GPIO_Mode_Out_OD_HiZ_Slow);
    GPIO_Init(GPIOB, GPIO_Pin_4, GPIO_Mode_Out_OD_HiZ_Slow);   //�ر�flashƬѡ
    NFC_Operation.NFC_PowerOn();
    BSP_RtcDeepSleep();
}
void BerthPo_WakeFromSleep()
{
    if(tagConfigSymple.startUpGetBottom ==
       1)                            //�豸�Ѿ����������ÿ�ο�����ȡһ�α��ף��������شŹ���
    {
        DelayMs(10);
        if (BerthPo_SetRm3100Base() == 1)   //��ȡ����
        {
            //��ʼ������
            tagConfigSymple.sendNodeCount = tagConfigSymple.sendPackCount -
                                            1;  //���У׼�ɹ���������һ�ַ������ݹ��ֳֻ��鿴
        }
        tagConfigSymple.startUpGetBottom = 0;
    }
    BerthPo_AlarmJude();
    tagConfigSymple.batteryCount--;   //��ؼ�⻽�����ڼ�����
}
void BerthPo_NFCCallBack()
{
    controlConfig.workMode = BERTHPO_MODE_ACTIVE;   
	//�豸�����Ժ󣬹ر��ж����ţ���ֹ��μ���
	NFC_Operation.NFC_PowerOff();
	//�豸�Ѿ�����,LED1��˸3��
    for(uint8_t i = 0; i < 3; i++)
    {
        Led_Operation.LedOn(LED_GPIO, 1);
        DelayMs(500);
        Led_Operation.LedOff(LED_GPIO, 1);
        DelayMs(500);
    }
    if(BerthPo_FactoryTest() == 1)   //�����Լ����ͨ����LED2��˸5��
    {
        for(uint8_t i = 0; i < 3; i++)
        {
            Led_Operation.LedOn(LED_GPIO, 2);
            DelayMs(500);
            Led_Operation.LedOff(LED_GPIO, 2);
            DelayMs(500);
            //void BerthPo_InitMcu();        //���³�ʼ��MCU
            //void BerthPo_InitSysParam();   //��ʼ��ϵͳ����
        }
    }
}