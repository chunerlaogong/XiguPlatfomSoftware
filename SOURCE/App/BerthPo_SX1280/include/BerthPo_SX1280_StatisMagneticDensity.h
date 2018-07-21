#ifndef _STATIS_MAGNETIC_DENSITY_H
#define _STATIS_MAGNETIC_DENSITY_H

#include <stm8l15x.h>
#include "BerthPo_SX1280_Common.h"
/*****************�궨��************************/



/*****************�ṹ�嶨��********************/

/***************************��������*****************************/
void BerthPo_InitStatisSet(PSENSOR_RM3100 pSensor3100, PRM3100_MAGNETIC pRm3100Magnetic); //��ʼ��
void BerthPo_ZeroStatisSet(PRM3100_STATISSET pRm3100StatisSet);
void BerthPo_EnqueueStatisSet(PSENSOR_RM3100 pSensor3100, PRM3100_STATISSET pRm3100StatisSet, PRM3100_SAMPLE_DATA pRm3100SampleData); 
uint8_t BerthPo_QueueFullStatisSet(PRM3100_STATISSET pRm3100StatisSet);
uint8_t BerthPo_QueueEmptyStatisSet(PRM3100_STATISSET pRm3100StatisSet);
void BerthPo_DequeueStatisSet(PRM3100_STATISSET pRm3100StatisSet); 
void BerthPo_CalculateStatisVarianceMean(PRM3100_STATISSET pRm3100StatisSet);
MAGNETIC_LEVEL BerthPo_JudgeMagnLevel(uint8_t val);
uint8_t BerthPo_DealFullSet(PRM3100_STATISSET pRm3100StatisSet);
uint8_t BerthPo_StatisProcess(PSENSOR_RM3100 pSensorRm3100, PRM3100_SAMPLE_DATA pRm3100SampleData, PRM3100_MAGNETIC pRm3100Magnetic, uint8_t cmd/*1:��������*/);    //ͳ��ʵ�� cmd:��������
void BerthPo_InitBottomTimesStatisSet(PSENSOR_RM3100 pSensorRm3100, PRM3100_MAGNETIC pRm3100Magnetic);     //��ʼ���г����޳�����ͳ��
void BerthPo_InitBottomCnsTimeStatisSet(PSENSOR_RM3100 pSensorRm3100, PRM3100_MAGNETIC pRm3100Magnetic);   //��ʼ���޳�����ͳ��
void BerthPo_CalculateBottomStatisVarianceMean(PRM3100_STATISSET pRm3100StatisSet);
void BerthPo_DeleteDataStatisSet(PRM3100_STATISSET pRm3100StatisSet);
uint8_t BerthPo_DealFullBottomSet(PRM3100_STATISSET pRm3100StatisSet);
uint8_t BerthPo_BottomTimesAdjustStatisProcess(PRM3100_SAMPLE_DATA pRm3100SampleData, PSENSOR_RM3100 pSensorRm3100, PRM3100_MAGNETIC pRm3100Magnetic);   //�г����޳�ʱͳ��ʵ��
uint8_t BerthPo_BottomCnsTimeAdjustStatisProcess(PRM3100_SAMPLE_DATA pRm3100SampleData, PSENSOR_RM3100 pSensorRm3100, PRM3100_MAGNETIC pRm3100Magnetic ); 
uint8_t BerthPo_MatchTimesAndCnsTimeStatis(PSENSOR_RM3100 pSensorRm3100, PRM3100_MAGNETIC pRm3100Magnetic);  //�жϳ�ʱ���޳�ͳ�ƺ�ĳһ���޳������Ƿ�һ��
uint8_t BerthPo_GetBottomOffsetValue(PSENSOR_RM3100 pSensorRm3100) ;    //�жϱ���ƫ��ֵ(δ�жϻ�δƫ�ƶ�����0)                                      
#endif



