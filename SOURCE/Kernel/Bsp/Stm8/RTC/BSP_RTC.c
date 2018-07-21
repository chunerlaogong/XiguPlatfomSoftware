/*
********************************************************************************
*
*                                 BSP_RTC.c
*
* File          : BSP_RTC.c
* Version       : V1.0
* Author        : Lhl
* Mode          : Thumb2
* Toolchain     : 
* Description   : RTC��������
*                
* History       :
* Date          : 2018.05.07
*******************************************************************************/
#include "BSP_RTC.h"
#include "BSP_Uart.h"

/*******************************************************************************
* Function Name : void GetDateAndTime(RTC_DateTypeDef* RTCGetDate , RTC_TimeTypeDef* RTCGetTime)
* Description   : ��ȡ��ǰRTCʱ��ʱ��
* Author        : Lhl
* Input         : ��ֵ���ڣ�ʱ��
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void GetDateAndTime(RTC_DateTypeDef* RTCGetDate , RTC_TimeTypeDef* RTCGetTime)
{
    while (RTC_WaitForSynchro() != SUCCESS);	// wait the Calander sychonized
    RTC_GetTime(RTC_Format_BIN, RTCGetTime);
    RTC_GetDate(RTC_Format_BIN, RTCGetDate);
}
/*******************************************************************************
* Function Name : void ShowTime(void)
* Description   : ��ӡ��ǰ���ں�ʱ��
* Author        : Lhl
* Input         : ��ֵ���ڣ�ʱ��
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void ShowTime(void)
{
    RTC_DateTypeDef RTCGetDate;
    RTC_TimeTypeDef RTCGetTime;
    while (RTC_WaitForSynchro() != SUCCESS);	// wait the Calander sychonized
    RTC_GetTime(RTC_Format_BIN, &RTCGetTime);
    RTC_GetDate(RTC_Format_BIN, &RTCGetDate);
    printf("����:%d-%d-%d" , RTCGetDate.RTC_Year,RTCGetDate.RTC_Month,RTCGetDate.RTC_Date);
    printf("ʱ��:%d:%d:%d\r\n",RTCGetTime.RTC_Hours,RTCGetTime.RTC_Minutes,RTCGetTime.RTC_Seconds);
}
/*******************************************************************************
* Function Name : void SetDateAndTime(uint8_t Year,uint8_t Month,uint8_t Day,uint8_t Hour,uint8_t Minute,uint8_t Second)
* Description   : ���õ�ǰ���ں�ʱ��
* Author        : Lhl
* Input         : ���ڣ�ʱ��
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void SetDateAndTime(uint8_t Year,uint8_t Month,uint8_t Day,uint8_t Hour,uint8_t Minute,uint8_t Second)
{
	RTC_DateTypeDef RTCSetDate;
	RTC_TimeTypeDef RTCSetTime;
	RTCSetDate.RTC_Year = Year;
	RTCSetDate.RTC_Month = Month;
	RTCSetDate.RTC_Date = Day;
	RTCSetTime.RTC_Hours = Hour;
	RTCSetTime.RTC_Minutes = Minute;
	RTCSetTime.RTC_Seconds = Second;
	RTC_SetTime(RTC_Format_BIN, &RTCSetTime);
	RTC_SetDate(RTC_Format_BIN, &RTCSetDate);
}
/*******************************************************************************
* Function Name : void Rtc_Init(void)
* Description   : ��ʼ��RTC���ã���ʼ��ʱ��Ϊ2018-05-22 16:19:00 ���ڶ�
* Author        : Lhl
* Input         : 
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void Rtc_Init(void)
{	
        RTC_InitTypeDef		RTCInit;
        RTC_DateTypeDef         RTC_DateInit;
        RTC_TimeTypeDef         RTC_TimeInit;
        
        CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);
        CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
        RTC_DeInit();
	RTCInit.RTC_HourFormat = RTC_HourFormat_24;	
	RTCInit.RTC_AsynchPrediv = 124;	
	RTCInit.RTC_SynchPrediv = 303;
	RTC_Init(&RTCInit);
	
	RTC_DateStructInit(&RTC_DateInit);
	RTC_DateInit.RTC_WeekDay =RTC_Weekday_Monday;
	RTC_DateInit.RTC_Date = 22;
	RTC_DateInit.RTC_Month = RTC_Month_May;
	RTC_DateInit.RTC_Year = 18;
	RTC_SetDate(RTC_Format_BIN, &RTC_DateInit);
	
	RTC_TimeStructInit(&RTC_TimeInit);
	RTC_TimeInit.RTC_Hours = 7;
	RTC_TimeInit.RTC_Minutes = 19;
	RTC_TimeInit.RTC_Seconds = 0;
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeInit);
}
/*******************************************************************************
* Function Name : void RTCAlarm_Set(uint16_t Duration)
* Description   : ����RTC���ѹ���
* Author        : Lhl
* Input         : ���붨ʱ���ѵ�ʱ��
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void RTCAlarm_Set(uint16_t Duration)
{
      RTC_WakeUpCmd(DISABLE);
      CLK_PeripheralClockConfig(CLK_Peripheral_RTC,ENABLE);
      CLK_RTCClockConfig(CLK_RTCCLKSource_LSI , CLK_RTCCLKDiv_1);
      RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
      RTC_ITConfig(RTC_IT_WUT,ENABLE);
      RTC_WakeUpCmd(DISABLE);
      RTC_SetWakeUpCounter(Duration); //2375ns*Duration
      RTC_WakeUpCmd(ENABLE);
}

/*******************************************************************************
* Function Name : void RTCAlarm_Dis(void)
* Description   : ʧ��RTC����
* Author        : Lhl
* Input         : 
* Output        :
* Other         :
* Date          : 2018.06.09
*******************************************************************************/
void RTCAlarm_Dis(void)
{
      RTC_WakeUpCmd(DISABLE);
}

/**
  * @brief RTC / CSS_LSE Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler, 4)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  RTC_WakeUpCmd(DISABLE);
  RTC_ClearITPendingBit(RTC_IT_WUT);
//  printf("����RTC�ж�\r\n");
}


