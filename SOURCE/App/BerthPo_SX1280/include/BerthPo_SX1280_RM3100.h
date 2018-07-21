#ifndef _BIRTHPO_SX1280_RM3100_H_
#define _BIRTHPO_SX1280_RM3100_H_

#include <stm8l15x.h>
/****************宏定义************************/

#define MAX_STATIS_SIZE 8     // 最大统计数
#define STATIS_SET_SIZE 4     // 统计集
#define STATIS_SUBSET_SIZE 6  // 统计子集

#define BOTTOM_TIMES_STATIS_SET_SIZE 4     // 有车切无车时数据统计集
#define BOTTOM_TIME_STATIS_SET_SIZE 8      // 无车数据统计集
#define BOTTOM_TIME_STATIS_SUBSET_SIZE 8   // 无车数据统计子集

/****************结构体定义********************/
typedef struct   //RM3100本底值
{

    signed short EMDataBottom_x;                            //x轴强度本底
    signed short EMDataBottom_y;                            //y轴强度本底
    signed short EMDataBottom_z;                            //z轴强度本底
}*PRM3100_BOTTOM, SRM3100_BOTTOM;

typedef struct  //RM3100当前地磁的值
{
    signed short  EMDataCurrent_x;         //xyz轴的地磁值
    signed short  EMDataCurrent_y;
    signed short  EMDataCurrent_z;
}*PRM3100_CURRENT_VAL, SRM3100_CURRENT_VAL;
typedef struct   //RM3100地磁变化值
{
    signed int  EMDataVary_x;   //xyz轴的地磁变化值
    signed int  EMDataVary_y;
    signed int  EMDataVary_z;
}*PRM3100_VARY_VAL, SRM3100_VARY_VAL;

typedef struct
{
    signed short  data_x;
    signed short  data_y;
    signed short  data_z;
}*PRM3100_EMData, SRM3100_EMData;
typedef enum
{
    MAGNETIC_STATE_NULL,
    MAGNETIC_STATE_INIT,
    MAGNETIC_STATE_CONFIRM
} MAGNETIC_STATE;

typedef enum
{
    MAGNETIC_HIGHT,
    MAGNETIC_MIDDLE,
    MAGNETIC_LOW
} MAGNETIC_LEVEL;
typedef struct   // 磁干扰强度
{
    MAGNETIC_STATE state;
    MAGNETIC_LEVEL level;
    uint8_t done;                 //完成一轮有效统计
    uint8_t val;
    int16_t x;
    int16_t y;
    int16_t z;
}*PRM3100_MAGNETIC, SRM3100_MAGNETIC;
typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
    uint8_t diffOfRM;            //模值
}*PRM3100_SAMPLE_DATA, SRM3100_SAMPLE_DATA;
typedef struct
{

    SRM3100_SAMPLE_DATA sampleData[MAX_STATIS_SIZE];
    uint8_t var;
    uint8_t var_x;
    uint8_t var_y;
    uint8_t var_z;
    uint8_t mean;
    int16_t mean_x;
    int16_t mean_y;
    int16_t mean_z;
    uint8_t size;
    uint8_t len;
}*PRM3100_STATISSET, SRM3100_STATISSET;
typedef struct
{
    uint8_t             clearBottomBufferFlag;   //清除本底标志
    uint8_t             getTimeBottomCount;      //取得实时本底次数,如果=63则为报警状态
    SRM3100_BOTTOM      rm3100DynamicBoottom;    //动态本底
    SRM3100_CURRENT_VAL rm3100CurrentData;
    SRM3100_VARY_VAL    rm3100VaryData;
    SRM3100_MAGNETIC    rm3100Magnetic;           //地磁干扰强度
    SRM3100_STATISSET    statisSubset;            //统计子集6
    SRM3100_STATISSET    statisSet;               //统计集 4
    SRM3100_STATISSET    statisBottomTimesSet;    //5
    SRM3100_STATISSET    statisBottomTimeSubset;  //8
    SRM3100_STATISSET    statisBottomtimeSet;     //8
    SRM3100_MAGNETIC     rm3100MagneticBase;      //无车磁场强度(地球磁场+环境磁场)统计信息
    SRM3100_MAGNETIC     rm3100MagneticBaseLower; //无车时最接近本底的统计信息
    SRM3100_MAGNETIC     rm3100MagneticDensity;   //车辆磁干扰的统计信息
    signed long   diffOfRM;                       //阈值
}*PSENSOR_RM3100, SSENSOR_RM3100;
/*******************函数声明**********************/



#endif   //_BIRTHPO_SX1280_RM3100_H_

