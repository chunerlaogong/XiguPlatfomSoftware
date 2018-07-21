#include "BerthPo_SX1280_Init.h"
#include "Delay.h"
#include <stdio.h>
#include <stdlib.h>
#include "Drivers_SX1280.h"
/***********************���ñ���************************/
extern SCONTROL_CONFIG  controlConfig;
extern SNET_MUTUAL_INFO  netMutualInfo;
extern LED_GPIO_TypeDef LED_GPIO;
extern SRM3100_GPIO_TypeDef RM3100_GPIO;
extern SSENSOR_RM3100 sensorRm3100;
extern SCONTROL_SYMPLE tagConfigSymple;
/*******************************************************************************
* Function Name : BerthPo_InitMcu()
* Description   : ��λ��Mcu��ʼ��
* Author        : YJD
* Input         : NULL
* Output        : NULL
* Other         : NULL
* Date          : 2018.05.11
*******************************************************************************/

void BerthPo_InitMcu()
{
    BSP_SysClkInit();      //��ʼ��ϵͳʱ��
    BSP_PeriphRtcInit();   //RTCʱ�ӳ�ʼ��
    //��ʼ���ش�
    RM3100_GPIO.MEG_PRW1_GPIO_BASE =
        GPIOC;             //PC2/PC3/PC4-����Ϊ���,�شŹ���
    RM3100_GPIO.MEG_PRW1_GPIO_Pin = GPIO_Pin_3;
    RM3100_GPIO.MEG_PRW2_GPIO_BASE = GPIOC;
    RM3100_GPIO.MEG_PRW2_GPIO_Pin = GPIO_Pin_2;
    RM3100_GPIO.MEG_PRW3_GPIO_BASE = GPIOC;
    RM3100_GPIO.MEG_PRW3_GPIO_Pin = GPIO_Pin_4;
    RM3100_GPIO.I2C_EN_GPIO_BASE = GPIOA;                //ʹ��I2C
    RM3100_GPIO.I2C_EN_GPIO_Pin = GPIO_Pin_7;
    RM3100_GPIO.MCU_DRDY_GPIO_BASE = GPIOD;
    RM3100_GPIO.MCU_DRDY_GPIO_Pin = GPIO_Pin_6;
    RM3100_GPIO.gpioI2cStructer.GPIO_SDA_BASE = GPIOC;   //����I2C�ܽ�
    RM3100_GPIO.gpioI2cStructer.GPIO_SDA_PIN = GPIO_Pin_0;
    RM3100_GPIO.gpioI2cStructer.GPIO_SCL_BASE = GPIOC;
    RM3100_GPIO.gpioI2cStructer.GPIO_SCL_PIN = GPIO_Pin_1;
    RM3100_GPIO.gpioI2cStructer.SLAVE_ADDRESS = 0x20;
    RM3100_Operation.RM3100_PowerOn(&(RM3100_GPIO));
    
    
    //SLAVE_ADDRESS��������һλ,����Ҫ��0x40
    
    //��ʼ��LED�ܽ�
    LED_GPIO.LED1_GPIO_BASE = GPIOD;
    LED_GPIO.LED1_GPIO_Pin = GPIO_Pin_1;
    LED_GPIO.LED2_GPIO_BASE = GPIOD;
    LED_GPIO.LED2_GPIO_Pin = GPIO_Pin_2;
    //��ʼ��SX1280
    GPIOB->DDR = 0;              //����ģʽ
    GPIOB->CR1 = 0xFF;           //����ģʽ�£���������������
    GPIOB->CR2 = 0;              //����ģʽ�£���ֹ�ж�
    SX1280_Operation.SX1280_PowerOn();

}

/*******************************************************************************
* Function Name : void BerthPo_SysParamInit()
* Description   : ϵͳ������Ҫ����ز�����ʼ��
* Author        : YJD
* Input         : NULL
* Output        : NULL
* Other         : NULL
* Date          : 2018.05.11
*******************************************************************************/

void BerthPo_InitSysParam()
{
    DelayMs(200);
    if(netMutualInfo.paraIntFlag[0] == 0x5A
       && netMutualInfo.paraIntFlag[1] == 0xA5
       && netMutualInfo.paraIntFlag[2] == 0x65)
    {
        nop();
    }
    else
    {
        Led_Operation.LedOn(LED_GPIO, 1);
        Led_Operation.LedOn(LED_GPIO, 2);
        controlConfig.workStatus = BERTHPO_MODE_ACTIVE;   //ACTIVATE;
        controlConfig.nodeConfig.idNub[0] = 0x01;         //��ʼ����λ��ID
        controlConfig.nodeConfig.idNub[1] = 0x02;
        controlConfig.nodeConfig.idNub[2] = 0x03;
        controlConfig.nodeConfig.idNub[3] = 0x00;
        controlConfig.nodeConfig.idNub[4] = 0x00;
        controlConfig.nodeConfig.idNub[5] = 0x00;
        controlConfig.paramConfig.alarmValid =
            0x4C00;    //��Ч��־,��10λ�г�����11λ�޳�����14λƵ�����ٻ��ѱ���
        controlConfig.nodeConfig.ledFlag = 0;             //led����״̬
        controlConfig.paramConfig.getEMBottom_RFFlag =
            0x01;           //0x01Ϊ�򿪵شż�⣬
        controlConfig.paramConfig.getEMBottom_RFModThreshold =
            10;     //�ų�ǿ��������
        controlConfig.paramConfig.getEMBottom_RFAngleThreshold =
            10;   //�ų�ƫת�Ƕ�������,10��
        controlConfig.paramConfig.fastRouse =
            0;                       //���ٻ��ѱ�־
        controlConfig.paramConfig.fastRouseAlarm =
            0;                      //��ο��ٻ��ѱ�����־
        controlConfig.paramConfig.wdtInterval =
            5;                     //WDT�̶�˯��ʱ�� unit:s
        controlConfig.paramConfig.heartbeatInterval =
            5;               //����������� unit:min Ĭ��5����
        tagConfigSymple.startUpGetBottom = 1;      //������ʼ����ȡ����
        netMutualInfo.paraIntFlag[0] = 0x5A;
        netMutualInfo.paraIntFlag[1] = 0xA5;
        netMutualInfo.paraIntFlag[2] = 0x65;

        BerthPo_WriteParamToFlash();
        BerthPo_ReadParamFromFlash();
      //  Led_Operation.LedOff(LED_GPIO, 1);
      //  Led_Operation.LedOff(LED_GPIO, 2);
        nop();
    }

}
void BerthPo_CloseAll_Peripheral()
{
	GPIO_Init(GPIOA , 0XFF , GPIO_Mode_Out_OD_Low_Slow);
    GPIO_Init(GPIOB , 0XFF , GPIO_Mode_Out_OD_Low_Slow);
    GPIO_Init(GPIOC , 0XFF , GPIO_Mode_Out_OD_Low_Slow);
    GPIO_Init(GPIOD , 0XFF , GPIO_Mode_Out_OD_Low_Slow);
    GPIO_Init(GPIOE , 0XFF , GPIO_Mode_Out_OD_Low_Slow);
    GPIO_Init(GPIOF , 0XFF , GPIO_Mode_Out_OD_Low_Slow);
	PWR_UltraLowPowerCmd(ENABLE);

}

