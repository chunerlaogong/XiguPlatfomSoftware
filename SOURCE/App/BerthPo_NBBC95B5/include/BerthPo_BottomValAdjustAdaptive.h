#ifndef _BERTHPO_BOTTOM_ADJUST_H_
#define _BERTHPO_BOTTOM_ADJUST_H_

#include <stm8l15x.h>
#include "BerthPo_StatisMagneticDensity.h"
#include "Drivers_RF6936.h"
#include "BerthPo_Common.h"
/******************变量定义******************/
static uint8_t S_noParkStartstatisFlag = 0;        // 单次无车状态调整

static SRM3100_MAGNETIC S_bottomTimes;             // 通过多次无车数据调整本底
static SRM3100_MAGNETIC S_bottomLongTime;          // 通过长时间无车数据统计本底
static SRM3100_EMData S_realTimeBottom;            // 实时本底值
/***************函数定义******************/
void BerthPo_GetDynamicBottom();   //取得实时动态本底
SRM3100_EMData BerthPo_GetRealTimeBottom();
uint8_t BerthPo_SatisfyBottomConstraintCondition( PRM3100_MAGNETIC pRm3100Magnetic);
uint8_t BerthPo_BottomvalAdjustProcess(); 
#endif   //_BERTHPO_BOTTOM_ADJUST_H_