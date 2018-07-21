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
* Description   : RTC驱动程序
*                
* History       :
* Date          : 2018.05.07
*******************************************************************************/
#include "BSP_RTC.h"
#include "BSP_Uart.h"

/*******************************************************************************
* Function Name : void GetDateAndTime(RTC_DateTypeDef* RTCGetDate , RTC_TimeTypeDef* RTCGetTime)
* Description   : 获取当前RTC时钟时间
* Author        : Lhl
* Input         : 赋值日期，时间
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
* Description   : 打印当前日期和时间
* Author        : Lhl
* Input         : 赋值日期，时间
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
    printf("日期:%d-%d-%d" , RTCGetDate.RTC_Year,RTCGetDate.RTC_Month,RTCGetDate.RTC_Date);
    printf("时间:%d:%d:%d\r\n",RTCGetTime.RTC_Hours,RTCGetTime.RTC_Minutes,RTCGetTime.RTC_Seconds);
}
/*******************************************************************************
* Function Name : void SetDateAndTime(uint8_t Year,uint8_t Month,uint8_t Day,uint8_t Hour,uint8_t Minute,uint8_t Second)
* Description   : 设置当前日期和时间
* Author        : Lhl
* Input         : 日期，时间
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
* Description   : 初始化RTC配置，初始化时间为2018-05-22 16:19:00 星期二
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
* Description   : 配置RTC唤醒功能
* Author        : Lhl
* Input         : 输入定时唤醒的时间
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
* Description   : 失能RTC唤醒
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
//  printf("进入RTC中断\r\n");
}


