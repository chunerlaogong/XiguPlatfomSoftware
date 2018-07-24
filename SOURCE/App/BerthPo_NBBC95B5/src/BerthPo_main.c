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
* Description   : ���������
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
/*********************��������************************/
SCONTROL_SYMPLE tagConfigSymple = {0};
LED_GPIO_TypeDef LED_GPIO = {0};              //LED���ƹܽŶ���
SRM3100_GPIO_TypeDef RM3100_GPIO = {0};       //RM3100�ܽŶ���
SBT_I410ES_GPIO_TypeDef BTI410ES_GPIO = {0};  //����ģ��ܽŶ���
SBC95_GPIO_TypeDef     BC95_GPIO = {0};                         //����NBģ��
SSENSOR_RM3100 sensorRm3100 = {0};
SNET_MUTUAL_INFO  netMutualInfo = {0};
SCONTROL_CONFIG  controlConfig = {0};
SBERTHPO_PARK_STATUS parkStatus = {0};        //��λ����ǩͣ��״̬
extern BT_I410ES_OPERATION_TypeDef BT_I410ES_Operation;

/*******************************************************************************
* Function Name : int main(void)
* Description   : ���������
* Input         :
* Output        :
* Other         :
* Date          : 2018.05.07
*******************************************************************************/
int main(void)
{
    disableInterrupts();               //��ȫ���ж�
    BerthPo_InitMcu();                 //��ʼ����λ��MCU
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
        while(controlConfig.workMode == BERTHPO_MODE_FACTORY)   //����Ĭ�Ͻ������ģʽ
        {
            if(BerthPo_FactoryTest() == 1)   //�������ͨ��
            {
                controlConfig.workMode = BERTHPO_MODE_DEEP_SLEEP;   //����ͨ���������˯��ģʽ
                BerthPo_WriteParamToFlash();   //�������ͨ����־
                BerthPo_DeepSleep();           //�������˯��״̬
            }
        }
        while(controlConfig.workMode == BERTHPO_MODE_ACTIVE)
        {
            BerthPo_WakeFromSleep();   //��λ״̬�ж�
            BerthPo_Sleep();
        }
    }

}








