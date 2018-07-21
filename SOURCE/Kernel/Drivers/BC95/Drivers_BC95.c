/*
********************************************************************************
*
*                                 BC95.c
*
* File          : BC95.c
* Version       : V1.0
* Author        : Lhl
* Mode          : Thumb2
* Toolchain     :
* Description   : BC95��������
*
* History       :
* Date          : 2018.06.09
*******************************************************************************/
#include <string.h>             //
#include <stdio.h>              //
#include <stdarg.h>             //va_list,va_start,va_end
#include <stdbool.h>            //bool�����ڴ˿���
#include <stdlib.h>             //atoi�����ڴ˿���(ASCII TO Integer)
#include "BSP_Uart.h"
#include "Drivers_BC95.h"
#include "Delay.h"


/**********2018-04-27*********************/
//��ǰ����ģʽ
static uint8_t m_State = 0;//NB����״̬
static uint8_t m_UpdateParameter = 0;//NB��ȡ�źŻ�ʱ���־λ
uint16_t Date[7] = {0};
uint16_t CSQ[2] = {0};

#define COMOk   1
#define NetConnect  2
#define NetConfigOk 3
#define SendData    4
#define SendDataOk  5
#define RebootNB    8
/*******************************************************************************
* Function Name : void DealNBData(char* Buf, uint8_t len)
* Description   : NB�ص�����
* Author        : Lhl
* Input         :
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void DealNBData(char* Buf, uint8_t len)
{
    char buffer[50] = {0};
    if((NULL != strstr(Buf, "+CSQ:")))//��ȡ��ǰNB�ź�ֵ
    {
        for(int i = 0 ; i < 20; i++)
        {
            buffer[i] = Buf[i];
        }
        sscanf(buffer, "\r\n+CSQ:%d,%d", &CSQ[0], &CSQ[1]);
        m_UpdateParameter = 1;//��ȡ���ź�ֵ
        return ;
    }
    if (NULL != strstr((char *)Buf, "CCLK:"))//��ȡ��ǰģ��ʱ��
    {
        for(int i = 0 ; i < 30; i++)
        {
            buffer[i] = Buf[i];
        }
        sscanf(buffer, "\r\n+CCLK:%d/%d/%d,%d:%d:%d+%d", &Date[0], &Date[1], &Date[2],
               &Date[3], &Date[4], &Date[5], &Date[6]);
        m_UpdateParameter = 1;//��ȡ��ʱ��
        return;
    }
    if ((NULL != strstr(Buf, "OK")) && m_State == 0)//ȷ��NB����ͨ�ųɹ�
    {
        m_State = COMOk;
        return ;
    }
    if ((NULL != strstr(Buf, "CEREG:0,1")) && m_State == COMOk)//ȷ��NB��������
    {
        m_State = NetConnect;
        return ;
    }
    if((NULL != strstr(Buf, "OK")) && m_State == NetConnect)//����UDP��COAP���óɹ�
    {
        m_State = NetConfigOk;
        return ;
    }
    if((NULL != strstr(Buf, "OK")) && m_State == NetConfigOk)//�������ݳɹ�
    {
        m_State = SendDataOk;
        return ;
    }

}

/*******************************************************************************
* Function Name : void _RXIRQHandler(uint8_t COM)
* Description   :
* Input         :
* Output        :
* Other         :
* Date          : 2013.08.23
*******************************************************************************/
void _RXIRQHandler()
{
    static char m_Buf[128] = {0};
    static int i = 0;
    volatile unsigned char clear;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t ch = USART1->DR;
        m_Buf[i++] = ch;

    }
    else if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        clear = USART1->SR;
        clear = USART1->DR;
        DealNBData(m_Buf, strlen(m_Buf));
        memset(m_Buf, 0, sizeof(m_Buf));
        i = 0;
    }
}


INTERRUPT_HANDLER(USART1_RX_TIM5_CC_IRQHandler, 28)
{
    /* In order to detect unexpected events during development,
    it is recommended to set a breakpoint on the following instruction.
    */
    _RXIRQHandler();
}

/*******************************************************************************
* Function Name : static int32_t _PowerOn(void)
* Description   : ����NB��Դ(�ڲ���)
* Author        : Lhl
* Input         :
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void PowerOn(PBC95_GPIO_TypeDef pGpioBC95Structer)
{
    BC95_PWR_CLR(pGpioBC95Structer->BC95_PWR_BASE, pGpioBC95Structer->BC95_PWR_PIN);
    DelayMs(5000);
    BC95_PWR_SET(pGpioBC95Structer->BC95_PWR_BASE, pGpioBC95Structer->BC95_PWR_PIN);
}

/*******************************************************************************
* Function Name : void Drivers_PowerOn(PBC95_GPIO_TypeDef pGpioBC95Structer)
* Description   : ����NB��Դ
* Author        : Lhl
* Input         :
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void Drivers_PowerOn(PBC95_GPIO_TypeDef pGpioBC95Structer)
{
    BC95_PWR_SET(pGpioBC95Structer->BC95_PWR_BASE, pGpioBC95Structer->BC95_PWR_PIN);
    DelayMs(100);
    BSP_UartOpen(&pGpioBC95Structer->COM);
}

/*******************************************************************************
* Function Name : void Drivers_PowerOff(PBC95_GPIO_TypeDef pGpioBC95Structer)
* Description   : �ر�NB��Դ
* Author        : Lhl
* Input         :
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void Drivers_PowerOff(PBC95_GPIO_TypeDef pGpioBC95Structer)
{
    BC95_PWR_CLR(pGpioBC95Structer->BC95_PWR_BASE, pGpioBC95Structer->BC95_PWR_PIN);
}
/*******************************************************************************
* Function Name : static int32_t _CommQuery(void)
* Description   : ���ڷ������ݼ��
* Author        : Lhl
* Input         :
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void CommQuery(PBC95_GPIO_TypeDef pGpioBC95Structer)
{
    uint8_t Buffer[10] = "AT\r\n";
    BSP_UartSendBytes(pGpioBC95Structer->COM.USARTNum, Buffer);
}
/*******************************************************************************
* Function Name : void NetCheck(PBC95_GPIO_TypeDef pGpioBC95Structer)
* Description   : ���NB�Ƿ���������
* Author        : Lhl
* Input         :
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void NetCheck(PBC95_GPIO_TypeDef pGpioBC95Structer)
{
    uint8_t Buffer[12] = "AT+CEREG?\r\n";
    BSP_UartSendBytes(pGpioBC95Structer->COM.USARTNum, Buffer);
}

/*******************************************************************************
* Function Name : void COAPConfig(PBC95_GPIO_TypeDef pGpioBC95Structer)
* Description   : COAPЭ�����ú���
* Author        : Lhl
* Input         :
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void COAPConfig(PBC95_GPIO_TypeDef pGpioBC95Structer)
{
    char Buffer[30] = {0};
    sprintf(Buffer, "AT+NCDP=%d.%d.%d.%d,%d\r\n", pGpioBC95Structer->DestIpv4_1,
            pGpioBC95Structer->DestIpv4_2,
            pGpioBC95Structer->DestIpv4_3, pGpioBC95Structer->DestIpv4_4,
            pGpioBC95Structer->DestPort);//Զ��IP���˿�
    BSP_UartSendBytesLen(pGpioBC95Structer->COM.USARTNum, (uint8_t *)Buffer,
                         strlen(Buffer));
}
/*******************************************************************************
* Function Name : void UDPConfig(PBC95_GPIO_TypeDef pGpioBC95Structer)
* Description   : UDPЭ�����ú���
* Author        : Lhl
* Input         :
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void UDPConfig(PBC95_GPIO_TypeDef pGpioBC95Structer)
{
    char Buffer[30] = {0};
    sprintf(Buffer, "AT+NSOCR=DGRAM,17,%d,1\r\n",
            pGpioBC95Structer->DestPort);//UDPԶ�˶˿�
    BSP_UartSendBytes(pGpioBC95Structer->COM.USARTNum, (uint8_t *)Buffer);
}
/*******************************************************************************
* Function Name : void COAPSendData(PBC95_GPIO_TypeDef pGpioBC95Structer, uint8_t* buffer,
                  uint8_t len)
* Description   : ʹ��COAP��������;
* Author        : Lhl
* Input         :
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void COAPSendData(PBC95_GPIO_TypeDef pGpioBC95Structer, uint8_t* buffer,
                  uint8_t len)
{
    char Buffer[100] = {0};
    sprintf(Buffer, "AT+NMGS=%d,", len / 2);//���ݳ���,����
    for(int i = 0; i < len ; i++)
    {
        Buffer[11 + i] = buffer[i];
    }
    Buffer[11 + len] = 0x0D;
    Buffer[11 + len + 1] = 0x0A;
    BSP_UartSendBytesLen(pGpioBC95Structer->COM.USARTNum, (uint8_t *)Buffer,
                         strlen(Buffer));//ʹ�÷��͹̶����Ⱥ�����������������\0©��
}
/*******************************************************************************
* Function Name : void UDPSendData(PBC95_GPIO_TypeDef pGpioBC95Structer, uint8_t* buffer,
                  uint8_t len)
* Description   : ʹ��UDP��������;
* Author        : Lhl
* Input         :
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void UDPSendData(PBC95_GPIO_TypeDef pGpioBC95Structer, uint8_t* buffer,
                 uint8_t len)
{
    static char Buffer[100] = {0};
    uint8_t BufSize = 0;
    sprintf(Buffer, "AT+NSOST=0,%d.%d.%d.%d,%d,%d,", pGpioBC95Structer->DestIpv4_1,
            pGpioBC95Structer->DestIpv4_2,
            pGpioBC95Structer->DestIpv4_3, pGpioBC95Structer->DestIpv4_4,
            pGpioBC95Structer->DestPort, len / 2);//IP��ַ���˿ڣ����ݳ��ȣ�����
    BufSize = strlen(Buffer);
    for(int i = 0; i < len ; i++)
    {
        Buffer[BufSize + i] = buffer[i];
    }
    Buffer[BufSize + len] = 0x0D;
    Buffer[BufSize + len + 1] = 0x0A;
    BSP_UartSendBytesLen(pGpioBC95Structer->COM.USARTNum, (uint8_t *)Buffer,
                         strlen(Buffer));
}

/*******************************************************************************
* Function Name : void Drivers_GetSignal(PBC95_GPIO_TypeDef pGpioBC95Structer , uint16_t* Signal)
* Description   : ������ȡNB�ź�ǿ��
* Author        : Lhl
* Input         :
* Output        : �ź�ǿ��
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void Drivers_GetSignal(PBC95_GPIO_TypeDef pGpioBC95Structer, uint16_t* Signal)
{
    char Buffer[10] = "AT+CSQ\r\n";
    BSP_UartSendBytes(pGpioBC95Structer->COM.USARTNum, (uint8_t *)Buffer);
    while(!m_UpdateParameter)//�ȴ���������
    {
        DelayMs(10);
    }
    *Signal = CSQ[0];
    m_UpdateParameter = 0;//��ձ�־λ
}

/*******************************************************************************
* Function Name : void Drivers_GetNetTime(PBC95_GPIO_TypeDef pGpioBC95Structer , uint8_t* Time)
* Description   : ������ȡBC95ģ���ȡ������ʱ�䣬������ͬ��MCU�ڲ�RTCʱ��
* Author        : Lhl
* Input         :
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void Drivers_GetNetTime(PBC95_GPIO_TypeDef pGpioBC95Structer, uint8_t* Time)
{
    char Buffer[11] = "AT+CCLK?\r\n";
    BSP_UartSendBytes(pGpioBC95Structer->COM.USARTNum, (uint8_t *)Buffer);
    while(!m_UpdateParameter)//�ȴ���������
    {
        DelayMs(10);
    }
    *Time++ = Date[0];
    *Time++ = Date[1];
    *Time++ = Date[2];
    *Time++ = Date[3] + 8;
    *Time++ = Date[4];
    *Time++ = Date[5];
    *Time++ = Date[6];
    m_UpdateParameter = 0;//��ձ�־λ
}

/*******************************************************************************
* Function Name : int Drivers_BC95_Manage(PBC95_GPIO_TypeDef pGpioBC95Structer)
* Description   : BC95��ѭ���߼����Զ������������󷵻�MC_OK��
* Author        : Lhl
* Input         :
* Output        : 0 or 1
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
int Drivers_BC95_Manage(PBC95_GPIO_TypeDef pGpioBC95Structer)
{
    static uint16_t TimeCount = 0;
    while(m_State != NetConfigOk)
    {
        if(m_State == 0)
        {
            CommQuery(pGpioBC95Structer);   //ȷ�ϴ���ͨ�ųɹ�
            DelayMs(500);
            TimeCount++;
            if(TimeCount >= 100)   //����ͨ��һֱʧ��
            {
                m_State = 0;
                TimeCount = 0;
                return MC_ERROR;
            }
        }
        if(m_State == COMOk)
        {
            NetCheck(pGpioBC95Structer);   //����Ƿ�����
            DelayMs(500);
            TimeCount++;
            if(TimeCount >= 1000)   //��ʱ���������ɹ�������NB
            {
                m_State = RebootNB;
                TimeCount = 0;
            }
        }
        if(m_State == NetConnect)
        {
            if(pGpioBC95Structer->Type == COAP)   //ʹ��COAPЭ������
            {
                COAPConfig(pGpioBC95Structer);
                DelayMs(5000);
            }
            if(pGpioBC95Structer->Type == UDP)   //ʹ��UDPЭ������
            {
                UDPConfig(pGpioBC95Structer);
                while(m_State != NetConfigOk)
                {
                    DelayMs(100);
                }

            }
        }
        if(m_State == NetConfigOk)
        {
            return MC_OK;
        }
        if(m_State == RebootNB)
        {
            PowerOn(pGpioBC95Structer);   //����NB
            m_State = 0;
        }
    }
    return MC_NONE;
}

/*******************************************************************************
* Function Name : int Drivers_BC95_SendData(PBC95_GPIO_TypeDef pGpioBC95Structer, uint8_t* Data,
                          uint8_t len, uint8_t Times)
* Description   : BC95���ݷ��ͺ���
* Author        : Lhl
* Input         :�ṹ�壬���ݣ����ݳ��ȣ����ʹ���
* Output        : ����1��ʾ���ͳɹ�
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
int Drivers_BC95_SendData(PBC95_GPIO_TypeDef pGpioBC95Structer, uint8_t* Data,
                          uint8_t len)
{

    while(pGpioBC95Structer->sendTimes--)
    {
        while(pGpioBC95Structer->maxTryCount--)
        {
            if(pGpioBC95Structer->Type == COAP)
            {
                COAPSendData(pGpioBC95Structer, Data, len);
            }
            if(pGpioBC95Structer->Type == UDP)
            {
                UDPSendData(pGpioBC95Structer, Data, len);
            }
            DelayMs(30);
            if(m_State == SendDataOk)   //�ȴ����ͳɹ�
            {
                break;

            }
        }

    }
	if(m_State != SendDataOk)
	{
		 return MC_ERROR;
	}
    m_State = NetConfigOk;
   	return MC_OK;
}

