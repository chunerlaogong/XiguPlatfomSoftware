/*
********************************************************************************
*
*                                 BSP_RTC.h
*
* File          : BSP_RTC.h
* Version       : V1.0
* Author        : Lhl
* Mode          : Thumb2
* Toolchain     : 
* Description   : RTC≥ı ºªØ
*                
* History       :
* Date          : 2018.05.07
*******************************************************************************/
#ifndef _BSP_RTC_H_
#define _BSP_RTC_H_

#include "stm8l15x_rtc.h"
#include "stm8l15x_clk.h"

void Rtc_Init(void);
void GetDateAndTime(RTC_DateTypeDef* RTCGetDate , RTC_TimeTypeDef* RTCGetTime);
void SetDateAndTime(uint8_t Year,uint8_t Month,uint8_t Day,uint8_t Hour,uint8_t Minute,uint8_t Second);
void RTCAlarm_Set(uint16_t Duration);
void ShowTime(void);
void RTCAlarm_Dis(void);
void BSP_RtcDeepSleep(void);
void BSP_RTCAlarmDelayMs(uint32_t ms);
#endif   //_BSP_RTC_H_

