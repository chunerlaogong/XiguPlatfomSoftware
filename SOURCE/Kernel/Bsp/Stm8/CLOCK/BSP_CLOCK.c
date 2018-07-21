#include "BSP_CLOCK.h"
#include "BSP_Common.h"

/*******************************************************************************
* Function Name :BSP_SysClkInit(void)
* Description   :����ϵͳʱ��
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          :2018.06.11
*******************************************************************************/

void BSP_SysClkInit(void)
{
    CLK_HSICmd(ENABLE);
    CLK_DeInit(); 
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_2);   //CLK_SYSCLKDiv_1	

}
/*******************************************************************************
* Function Name :void BSP_PeriphRtcInit(void) 
* Description   :����ϵͳrtc������ʱ��
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          :2018.06.11
*******************************************************************************/
void BSP_PeriphRtcInit(void)  
{
#ifdef BERTHPO_RF6936 | BERTHPO_NB_BC95_BT | HSPEED_RAIL
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSI,CLK_RTCCLKDiv_1);     //�ڲ�����ʱ��
  /* Wait for LSE clock to be ready */
  while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
  CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  RTC_WakeUpCmd(DISABLE);
  RTC_AlarmCmd(DISABLE);
  RTC_ITConfig(RTC_IT_ALRA, DISABLE);
  RTC_ITConfig(RTC_IT_WUT, DISABLE); 
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1,ENABLE);   //��ʼ������1
#endif
#ifdef BERTHPO_RF6936
  CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,ENABLE);   //for rf6936ģ�� 
#endif
}

/*******************************************************************************
* Function Name :void BSP_PeriphRtcInit(void) 
* Description   :����ϵͳrtc������ʱ��
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          :2018.06.11
*******************************************************************************/

void BSP_RtcGoSleep(uint32_t nS)   //nS ms
{
  RTC_WakeUpCmd(DISABLE);   //�ر��Զ����ѹ���
  CLK_PeripheralClockConfig(CLK_Peripheral_RTC,ENABLE);
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSI,CLK_RTCCLKDiv_1);
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  RTC_SetWakeUpCounter(nS);
  RTC_WakeUpCmd(ENABLE);   //�����Զ����ѹ���
  
}
void BSP_RtcDeepSleep()
{
  
    nop();nop();nop();nop();nop();
    nop();nop();nop();nop();nop();
    _HALT();
    nop();nop();nop();nop();nop();
    nop();nop();nop();nop();nop();
  
    disableInterrupts();      //���ж�
    RTC_WakeUpCmd(DISABLE);   //�ر��Զ����ѹ���
}


