#ifndef _DELAY_H_
#define _DELAY_H_

#include "stm8l15x_it.h"
void DelayUs(uint8_t us);   //延时us毫秒，最小值2us
void DelayMs(uint32_t ms);   //延时ms毫秒
#endif   //_DELAY_H_