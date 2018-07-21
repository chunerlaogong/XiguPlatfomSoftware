#ifndef _BERTHPO_VPARK_STATE_JUDGE_H_
#define _BERTHPO_VPARK_STATE_JUDGE_H_

#include <math.h>
#include <stm8l15x.h>
/**************���������********************/                    
#define GEOMA_DIS_WAVE_STREN_LOW_Z   200         //�ش��Ŷ�ʸ��Z���������󲨶�ֵ������ֵ
#define GEOMA_DIS_WAVE_ANGLE_LOW_Z   40          //�ش��Ŷ�ʸ�������Z��Ƕȵ���󲨶�ֵ������ֵ
#define JUDGE_ADJACENT_ON_UP_VAL     80          //�ж����ڳ�λ��ռ�õ�����ֵ
#define CAR_PARK_GEOMA_DIS_LOW       200         //�����ȶ�ͣ��������ĵش��Ŷ�ʸ��ǿ�ȵ�����ֵ
#define EMPTY_PARK_MAGNETIC_UP       10          //�ճ�λ�ȶ��شų�ǿ�ȱ仯����ֵ
/***********�ṹ�嶨��******************/    

typedef struct    //�شŷ���
{
   signed short geomaDis_x;   //�ش�x�����    
   signed short geomaDis_y;   //�ش�y�����     
   signed short geomaDis_z;   //�ش�z�����    
   
}SGEOMA_COMPONENT_DEF, *PGEOMA_COMPONENT_DEF;
typedef struct   //�شű��׷���
{
    signed short geomaDisBottom_x;   //x��ǿ�ȱ���
    signed short geomaDisBottom_y;   //y��ǿ�ȱ���
    signed short geomaDisBottom_z;   //z��ǿ�ȱ���
}SGEOMA_BOTTOM_COMPONENT_DEF, *PGEOMA_BOTTOM_COMPONENT_DEF;   
typedef struct   //��ֱͣ�������Ϣ
{  
   SGEOMA_COMPONENT_DEF sGeomaCompoent;                 //ʵʱ�شŷ���ֵ
   SGEOMA_BOTTOM_COMPONENT_DEF sGeomaBottomComponent;   //�شű��׷���
   signed long  geomaThreshold;                        //�ش��Ŷ�ʸ����ǿ��
   double geomaDisAngle_z;                             //�ش��Ŷ�ʸ����Z��ļн�
   uint8_t sampleCount;                                //��������
}SGEOMA_VPARK_INFO_DEF, *PGEOMA_VPARK_INFO_DEF;
#endif   //_BERTHPO_VPARK_STATE_JUDGE_H_