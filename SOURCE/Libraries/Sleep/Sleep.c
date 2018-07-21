#include "Sleep.h"
#include "Drivers_Common.h"
void NopSleep(void)
{    
    nop();nop();nop();nop();nop();
    nop();nop();nop();nop();nop();
    _HALT();
    nop();nop();nop();nop();nop();
    nop();nop();nop();nop();nop();
   
    disableInterrupts();////关中断
    RTC_WakeUpCmd(DISABLE);//关闭自动唤醒功能
}
void RtcSleep(uint32_t nS)
{
  RTC_WakeUpCmd(DISABLE);//关闭自动唤醒功能
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSI,CLK_RTCCLKDiv_1);
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  RTC_SetWakeUpCounter(nS);
  RTC_WakeUpCmd(ENABLE);//开启自动唤醒功能
  
}

