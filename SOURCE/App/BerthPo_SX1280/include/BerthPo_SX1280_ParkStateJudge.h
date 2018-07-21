#ifndef _BERTHPO_PARK_STATE_JUDGE_H_
#define _BERTHPO_PARK_STATE_JUDGE_H_

#include <stm8l15x.h>
#include "BerthPo_SX1280_Common.h"
#include "Drivers_Common.h"
/*****************宏定义*********************/
#define STATE_REVERSAL 0x5a  // 状态反转  

/*****************结构体定义*****************/
	
typedef struct _InputInfo {

	uint8_t diffOfRM;
	int16_t x;
	int16_t y;
	int16_t z;
	int8_t x_b;
	int8_t y_b;
	int8_t z_b;
	uint8_t RM_thr;      // 模值阈值
	uint8_t angle_thr;   // 配置角度阈值
}InputInfo;
/*******************变量定义******************/             
static MAGNETIC_STATE S_gParkState;               // 记录最近一次停车统计状态(用于停车后干扰导致一直无车的情况)
static SRM3100_EMData S_gNoParkData;              // 记录无车状态最新一组采集值
BERTH_STATE g_parkState = BERTHPO_PARK_STATE_NULL;   //车位状态


/*******************函数定义*****************/
InputInfo BerthPo_InitBerthStateInput();
uint8_t BerthPo_BerthStateSwithProcess();
void BerthPo_ChangeThresholdGet(uint8_t *thr1, uint8_t *thr2, InputInfo* pInput);
void BerthPo_CalculateMeagneticFeature(InputInfo* pInput, PRM3100_SAMPLE_DATA pRm3100SampleData);      
uint8_t BerthPo_ChangeJudge(InputInfo* pInput);
uint8_t BerthPo_ChangeConfirm(InputInfo* pInput);
uint8_t BerthPo_SendAlarmPackage(uint8_t frameType);
uint8_t BerthPo_GetVccInfo();
uint8_t BerthPo_SendCarStatus(uint8_t frameType);
void BerthPo_JudgeChangeOfModule();    //车辆状态反转判断
void BerthPo_AlarmJude();              //地磁变动才发数据

#endif   //_BERTHPO_PARK_STATE_JUDGE_H_
