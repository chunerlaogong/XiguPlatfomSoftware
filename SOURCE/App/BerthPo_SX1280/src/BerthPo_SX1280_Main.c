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
LED_GPIO_TypeDef LED_GPIO = {0};              //LED���ƹܽŶ���
SRM3100_GPIO_TypeDef RM3100_GPIO = {0};       //RM3100�ܽŶ���
SSENSOR_RM3100 sensorRm3100 = {0};
SNET_MUTUAL_INFO  netMutualInfo = {0};
SCONTROL_CONFIG  controlConfig = {0};
SBERTHPO_PARK_STATUS parkStatus = {0};        //��λ����ǩͣ��״̬

int main()
{
    disableInterrupts();               //��ȫ���ж�
    BerthPo_InitMcu();                 //��ʼ����λ��MCU
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
            BerthPo_WakeFromSleep();                   //��λ״̬�ж�
            BerthPo_Sleep();
        }
        NFC_INT_SET;                                   //���ⲿ�����ж�����
        //deep_sleep();                                //����״̬,˯�ߵȴ�����
        NFC_INT_CLR;
    }
}