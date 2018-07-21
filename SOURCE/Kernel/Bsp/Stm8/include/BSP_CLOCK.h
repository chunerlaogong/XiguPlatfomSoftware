#ifndef _BSP_CLOCK_H_
#define _BSP_CLOCK_H_

#include "stm8l15x_conf.h"


/*************º¯Êý¶¨Òå******************/
void BSP_ClkRtcInit(void);
void BSP_SysClkInit(void);
void BSP_RtcGoSleep(uint32_t nS);   //nS ms
void BSP_RtcDeepSleep();
#endif   //_BSP_CLOCK_H_