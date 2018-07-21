#ifndef _BERTHPO_BOTTOM_ADJUST_H_
#define _BERTHPO_BOTTOM_ADJUST_H_

#include <stm8l15x.h>
#include "BerthPo_StatisMagneticDensity.h"
#include "Drivers_RF6936.h"
#include "BerthPo_Common.h"
/******************��������******************/
static uint8_t S_noParkStartstatisFlag = 0;        // �����޳�״̬����

static SRM3100_MAGNETIC S_bottomTimes;             // ͨ������޳����ݵ�������
static SRM3100_MAGNETIC S_bottomLongTime;          // ͨ����ʱ���޳�����ͳ�Ʊ���
static SRM3100_EMData S_realTimeBottom;            // ʵʱ����ֵ
/***************��������******************/
void BerthPo_GetDynamicBottom();   //ȡ��ʵʱ��̬����
SRM3100_EMData BerthPo_GetRealTimeBottom();
uint8_t BerthPo_SatisfyBottomConstraintCondition( PRM3100_MAGNETIC pRm3100Magnetic);
uint8_t BerthPo_BottomvalAdjustProcess(); 
#endif   //_BERTHPO_BOTTOM_ADJUST_H_