#include "Sleep.h"
#include "Drivers_Common.h"
void NopSleep(void)
{    
    nop();nop();nop();nop();nop();
    nop();nop();nop();nop();nop();
    _HALT();
    nop();nop();nop();nop();nop();
    nop();nop();nop();nop();nop();
   
    disableInterrupts();////���ж�
    RTC_WakeUpCmd(DISABLE);//�ر��Զ����ѹ���
}
void RtcSleep(uint32_t nS)
{
  RTC_WakeUpCmd(DISABLE);//�ر��Զ����ѹ���
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSI,CLK_RTCCLKDiv_1);
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  RTC_SetWakeUpCounter(nS);
  RTC_WakeUpCmd(ENABLE);//�����Զ����ѹ���
  
}

