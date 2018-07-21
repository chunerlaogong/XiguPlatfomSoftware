#ifndef _BERTHPO_PARK_STATE_JUDGE_H_
#define _BERTHPO_PARK_STATE_JUDGE_H_

#include <stm8l15x.h>
#include "BerthPo_SX1280_Common.h"
#include "Drivers_Common.h"
/*****************�궨��*********************/
#define STATE_REVERSAL 0x5a  // ״̬��ת  

/*****************�ṹ�嶨��*****************/
	
typedef struct _InputInfo {

	uint8_t diffOfRM;
	int16_t x;
	int16_t y;
	int16_t z;
	int8_t x_b;
	int8_t y_b;
	int8_t z_b;
	uint8_t RM_thr;      // ģֵ��ֵ
	uint8_t angle_thr;   // ���ýǶ���ֵ
}InputInfo;
/*******************��������******************/             
static MAGNETIC_STATE S_gParkState;               // ��¼���һ��ͣ��ͳ��״̬(����ͣ������ŵ���һֱ�޳������)
static SRM3100_EMData S_gNoParkData;              // ��¼�޳�״̬����һ��ɼ�ֵ
BERTH_STATE g_parkState = BERTHPO_PARK_STATE_NULL;   //��λ״̬


/*******************��������*****************/
InputInfo BerthPo_InitBerthStateInput();
uint8_t BerthPo_BerthStateSwithProcess();
void BerthPo_ChangeThresholdGet(uint8_t *thr1, uint8_t *thr2, InputInfo* pInput);
void BerthPo_CalculateMeagneticFeature(InputInfo* pInput, PRM3100_SAMPLE_DATA pRm3100SampleData);      
uint8_t BerthPo_ChangeJudge(InputInfo* pInput);
uint8_t BerthPo_ChangeConfirm(InputInfo* pInput);
uint8_t BerthPo_SendAlarmPackage(uint8_t frameType);
uint8_t BerthPo_GetVccInfo();
uint8_t BerthPo_SendCarStatus(uint8_t frameType);
void BerthPo_JudgeChangeOfModule();    //����״̬��ת�ж�
void BerthPo_AlarmJude();              //�شű䶯�ŷ�����

#endif   //_BERTHPO_PARK_STATE_JUDGE_H_
