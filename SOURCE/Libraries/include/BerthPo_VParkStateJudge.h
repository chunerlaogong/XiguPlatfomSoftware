#ifndef _BERTHPO_VPARK_STATE_JUDGE_H_
#define _BERTHPO_VPARK_STATE_JUDGE_H_

#include <math.h>
#include <stm8l15x.h>
/**************宏变量定义********************/                    
#define GEOMA_DIS_WAVE_STREN_LOW_Z   200         //地磁扰动矢量Z轴分量的最大波动值的下限值
#define GEOMA_DIS_WAVE_ANGLE_LOW_Z   40          //地磁扰动矢量相对于Z轴角度的最大波动值的下限值
#define JUDGE_ADJACENT_ON_UP_VAL     80          //判断相邻车位有占用的上限值
#define CAR_PARK_GEOMA_DIS_LOW       200         //车辆稳定停靠后产生的地磁扰动矢量强度的下限值
#define EMPTY_PARK_MAGNETIC_UP       10          //空车位稳定地磁场强度变化上限值
/***********结构体定义******************/    

typedef struct    //地磁分量
{
   signed short geomaDis_x;   //地磁x轴分量    
   signed short geomaDis_y;   //地磁y轴分量     
   signed short geomaDis_z;   //地磁z轴分量    
   
}SGEOMA_COMPONENT_DEF, *PGEOMA_COMPONENT_DEF;
typedef struct   //地磁本底分量
{
    signed short geomaDisBottom_x;   //x轴强度本底
    signed short geomaDisBottom_y;   //y轴强度本底
    signed short geomaDisBottom_z;   //z轴强度本底
}SGEOMA_BOTTOM_COMPONENT_DEF, *PGEOMA_BOTTOM_COMPONENT_DEF;   
typedef struct   //垂直停车相关信息
{  
   SGEOMA_COMPONENT_DEF sGeomaCompoent;                 //实时地磁分量值
   SGEOMA_BOTTOM_COMPONENT_DEF sGeomaBottomComponent;   //地磁本底分量
   signed long  geomaThreshold;                        //地磁扰动矢量的强度
   double geomaDisAngle_z;                             //地磁扰动矢量与Z轴的夹角
   uint8_t sampleCount;                                //采样次数
}SGEOMA_VPARK_INFO_DEF, *PGEOMA_VPARK_INFO_DEF;
#endif   //_BERTHPO_VPARK_STATE_JUDGE_H_