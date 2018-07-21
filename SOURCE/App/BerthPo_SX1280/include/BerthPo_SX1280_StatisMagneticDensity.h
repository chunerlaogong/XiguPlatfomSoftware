#ifndef _STATIS_MAGNETIC_DENSITY_H
#define _STATIS_MAGNETIC_DENSITY_H

#include <stm8l15x.h>
#include "BerthPo_SX1280_Common.h"
/*****************宏定义************************/



/*****************结构体定义********************/

/***************************函数定义*****************************/
void BerthPo_InitStatisSet(PSENSOR_RM3100 pSensor3100, PRM3100_MAGNETIC pRm3100Magnetic); //初始化
void BerthPo_ZeroStatisSet(PRM3100_STATISSET pRm3100StatisSet);
void BerthPo_EnqueueStatisSet(PSENSOR_RM3100 pSensor3100, PRM3100_STATISSET pRm3100StatisSet, PRM3100_SAMPLE_DATA pRm3100SampleData); 
uint8_t BerthPo_QueueFullStatisSet(PRM3100_STATISSET pRm3100StatisSet);
uint8_t BerthPo_QueueEmptyStatisSet(PRM3100_STATISSET pRm3100StatisSet);
void BerthPo_DequeueStatisSet(PRM3100_STATISSET pRm3100StatisSet); 
void BerthPo_CalculateStatisVarianceMean(PRM3100_STATISSET pRm3100StatisSet);
MAGNETIC_LEVEL BerthPo_JudgeMagnLevel(uint8_t val);
uint8_t BerthPo_DealFullSet(PRM3100_STATISSET pRm3100StatisSet);
uint8_t BerthPo_StatisProcess(PSENSOR_RM3100 pSensorRm3100, PRM3100_SAMPLE_DATA pRm3100SampleData, PRM3100_MAGNETIC pRm3100Magnetic, uint8_t cmd/*1:持续更新*/);    //统计实现 cmd:持续更新
void BerthPo_InitBottomTimesStatisSet(PSENSOR_RM3100 pSensorRm3100, PRM3100_MAGNETIC pRm3100Magnetic);     //初始化有车且无车数据统计
void BerthPo_InitBottomCnsTimeStatisSet(PSENSOR_RM3100 pSensorRm3100, PRM3100_MAGNETIC pRm3100Magnetic);   //初始化无车数据统计
void BerthPo_CalculateBottomStatisVarianceMean(PRM3100_STATISSET pRm3100StatisSet);
void BerthPo_DeleteDataStatisSet(PRM3100_STATISSET pRm3100StatisSet);
uint8_t BerthPo_DealFullBottomSet(PRM3100_STATISSET pRm3100StatisSet);
uint8_t BerthPo_BottomTimesAdjustStatisProcess(PRM3100_SAMPLE_DATA pRm3100SampleData, PSENSOR_RM3100 pSensorRm3100, PRM3100_MAGNETIC pRm3100Magnetic);   //有车且无车时统计实现
uint8_t BerthPo_BottomCnsTimeAdjustStatisProcess(PRM3100_SAMPLE_DATA pRm3100SampleData, PSENSOR_RM3100 pSensorRm3100, PRM3100_MAGNETIC pRm3100Magnetic ); 
uint8_t BerthPo_MatchTimesAndCnsTimeStatis(PSENSOR_RM3100 pSensorRm3100, PRM3100_MAGNETIC pRm3100Magnetic);  //判断长时间无车统计和某一次无车数据是否一致
uint8_t BerthPo_GetBottomOffsetValue(PSENSOR_RM3100 pSensorRm3100) ;    //判断本底偏移值(未判断或未偏移都返回0)                                      
#endif



