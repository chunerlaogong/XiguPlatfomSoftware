#include "BerthPo_Init.h"
#include "BerthPo_BT.h"
#include "Delay.h"
#include <stdio.h>
#include <stdlib.h>
#include "Drivers_BC95.h"
/***********************���ñ���************************/
extern SCONTROL_CONFIG  controlConfig;
extern SNET_MUTUAL_INFO  netMutualInfo;
extern SBT_I410ES_GPIO_TypeDef BTI410ES_GPIO;
extern LED_GPIO_TypeDef LED_GPIO;
extern SRM3100_GPIO_TypeDef RM3100_GPIO;
extern SSENSOR_RM3100 sensorRm3100;
extern BT_I410ES_OPERATION_TypeDef BT_I410ES_Operation;
extern SBC95_GPIO_TypeDef     BC95_GPIO;
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
    //��ʼ���������gpio��,����������
    BTI410ES_GPIO.BT_POWER_GPIO_BASE = GPIOE;
    BTI410ES_GPIO.BT_POWER_GPIO_Pin = GPIO_Pin_1;
    BT_I410ES_Operation.BT_I410ES_PowerOn(&BTI410ES_GPIO);
    BerthPo_BtInit();
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
    RM3100_Operation.RM3100_I2CEnable(&(RM3100_GPIO));
    RM3100_GPIO.gpioI2cStructer.SLAVE_ADDRESS =
        0x20;    //RM3100_I2CEnable�����ڲ��Ѿ���
    //SLAVE_ADDRESS��������һλ,����Ҫ��0x40
    Simulation_I2C.I2cInit(&(RM3100_GPIO.gpioI2cStructer));
    //��ʼ��LED�ܽ�
    LED_GPIO.LED1_GPIO_BASE = GPIOD;
    LED_GPIO.LED1_GPIO_Pin = GPIO_Pin_1;
    LED_GPIO.LED2_GPIO_BASE = GPIOD;
    LED_GPIO.LED2_GPIO_Pin = GPIO_Pin_2;
    //��ʼ��NB-BC95�ܽ�
    BC95_GPIO.BC95_PWR_BASE = GPIOE;
    BC95_GPIO.BC95_PWR_PIN = GPIO_Pin_0;
    BC95_GPIO.BC95_RESET_BASE = GPIOD;
    BC95_GPIO.BC95_RESET_PIN = GPIO_Pin_7;
	BC95_GPIO.Type = UDP;              //UDP/COAP��������
	BC95_GPIO.maxTryCount = 3;         //�����3��
	BC95_GPIO.sendTimes   = 3;         //��Ч���ʹ���
    BC95_GPIO.COM.USARTNum = USART1;
    BC95_GPIO.COM.CLKPeripheralNum = CLK_Peripheral_USART1;
    BC95_GPIO.COM.UART_TX_GPIO_BASE = GPIOA;
    BC95_GPIO.COM.UART_TX_GPIO_Pin = GPIO_Pin_2;
    BC95_GPIO.COM.UART_RX_GPIO_BASE = GPIOA;
    BC95_GPIO.COM.UART_RX_GPIO_Pin = GPIO_Pin_3;
    BC95_GPIO.COM.baud = 9600;
    BC95_GPIO.COM.data = 8;
    BC95_GPIO.COM.stop = 1;
    BC95_GPIO.COM.parity = 0;
    Drivers_BC95_Operation.PowerOn(&BC95_GPIO);
    //��ʼ�����Դ���
    /*SBSP_UART_CFG_TypeDef m_debugUartConfig;
    m_debugUartConfig.USARTNum = USART1;
    m_debugUartConfig.CLKPeripheralNum = CLK_Peripheral_USART1;
    m_debugUartConfig.UART_TX_GPIO_BASE = GPIOA;
    m_debugUartConfig.UART_TX_GPIO_Pin  = GPIO_Pin_2;
    m_debugUartConfig.UART_RX_GPIO_BASE = GPIOA;
    m_debugUartConfig.UART_RX_GPIO_Pin  = GPIO_Pin_3;
    m_debugUartConfig.baud = 115200;
    m_debugUartConfig.data = 8;
    m_debugUartConfig.stop = 1;
    m_debugUartConfig.parity = 0;
    BSP_UartOpen(&m_debugUartConfig);*/

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
    uint32_t m_tagId = 2005;
    if(netMutualInfo.paraIntFlag[0] == 0x5A
       && netMutualInfo.paraIntFlag[1] == 0xA5
       && netMutualInfo.paraIntFlag[2] == 0x65)
    {
        nop();
    }
    else
    {
        controlConfig.workStatus = BERTHPO_MODE_ACTIVE;        //ACTIVATE;
        controlConfig.initNB = 0x01;
        controlConfig.nodeConfig.idNub[0] = (m_tagId & 0xFF);    //��ʼ����λ��ID
        controlConfig.nodeConfig.idNub[1] = (m_tagId >> 8) & 0xff;
        controlConfig.nodeConfig.idNub[2] = (m_tagId >> 16) & 0xff;
     //   controlConfig.nodeConfig.userCode = 10025;             //��ʼ��userCode
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
        uint16_t m_temp[4] = {0};
#if COAP_MODE
        sscanf(DEFAULT_COAP_SERVER_ADDRESS, "%d.%d.%d.%d:%d", &m_temp[0], &m_temp[1],
               &m_temp[2], &m_temp[3], &(netMutualInfo.defaultCoapPort));
        netMutualInfo.defaultCoapIpAddr[0] = m_temp[0];
        netMutualInfo.defaultCoapIpAddr[1] = m_temp[1];
        netMutualInfo.defaultCoapIpAddr[2] = m_temp[2];
        netMutualInfo.defaultCoapIpAddr[3] = m_temp[3];
#endif
#if UDP_MODE
        sscanf(DEFAULT_ORIGIN_SERVER_ADDRESS, "%d.%d.%d.%d:%d", &m_temp[0], &m_temp[1],
               &m_temp[2], &m_temp[3], &(netMutualInfo.defaultUdpPort));
        netMutualInfo.defaultUdpIpAddr[0] = m_temp[0];
        netMutualInfo.defaultUdpIpAddr[1] = m_temp[1];
        netMutualInfo.defaultUdpIpAddr[2] = m_temp[2];
        netMutualInfo.defaultUdpIpAddr[3] = m_temp[3];

#endif
        BerthPo_WriteParamToFlash();
        BerthPo_ReadParamFromFlash();
        nop();
    }

}

