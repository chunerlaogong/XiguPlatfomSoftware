#ifndef _BIRTHPO_SX1280_RM3100_H_
#define _BIRTHPO_SX1280_RM3100_H_

#include <stm8l15x.h>
/****************�궨��************************/

#define MAX_STATIS_SIZE 8     // ���ͳ����
#define STATIS_SET_SIZE 4     // ͳ�Ƽ�
#define STATIS_SUBSET_SIZE 6  // ͳ���Ӽ�

#define BOTTOM_TIMES_STATIS_SET_SIZE 4     // �г����޳�ʱ����ͳ�Ƽ�
#define BOTTOM_TIME_STATIS_SET_SIZE 8      // �޳�����ͳ�Ƽ�
#define BOTTOM_TIME_STATIS_SUBSET_SIZE 8   // �޳�����ͳ���Ӽ�

/****************�ṹ�嶨��********************/
typedef struct   //RM3100����ֵ
{

    signed short EMDataBottom_x;                            //x��ǿ�ȱ���
    signed short EMDataBottom_y;                            //y��ǿ�ȱ���
    signed short EMDataBottom_z;                            //z��ǿ�ȱ���
}*PRM3100_BOTTOM, SRM3100_BOTTOM;

typedef struct  //RM3100��ǰ�شŵ�ֵ
{
    signed short  EMDataCurrent_x;         //xyz��ĵش�ֵ
    signed short  EMDataCurrent_y;
    signed short  EMDataCurrent_z;
}*PRM3100_CURRENT_VAL, SRM3100_CURRENT_VAL;
typedef struct   //RM3100�شű仯ֵ
{
    signed int  EMDataVary_x;   //xyz��ĵشű仯ֵ
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
typedef struct   // �Ÿ���ǿ��
{
    MAGNETIC_STATE state;
    MAGNETIC_LEVEL level;
    uint8_t done;                 //���һ����Чͳ��
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
    uint8_t diffOfRM;            //ģֵ
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
    uint8_t             clearBottomBufferFlag;   //������ױ�־
    uint8_t             getTimeBottomCount;      //ȡ��ʵʱ���״���,���=63��Ϊ����״̬
    SRM3100_BOTTOM      rm3100DynamicBoottom;    //��̬����
    SRM3100_CURRENT_VAL rm3100CurrentData;
    SRM3100_VARY_VAL    rm3100VaryData;
    SRM3100_MAGNETIC    rm3100Magnetic;           //�شŸ���ǿ��
    SRM3100_STATISSET    statisSubset;            //ͳ���Ӽ�6
    SRM3100_STATISSET    statisSet;               //ͳ�Ƽ� 4
    SRM3100_STATISSET    statisBottomTimesSet;    //5
    SRM3100_STATISSET    statisBottomTimeSubset;  //8
    SRM3100_STATISSET    statisBottomtimeSet;     //8
    SRM3100_MAGNETIC     rm3100MagneticBase;      //�޳��ų�ǿ��(����ų�+�����ų�)ͳ����Ϣ
    SRM3100_MAGNETIC     rm3100MagneticBaseLower; //�޳�ʱ��ӽ����׵�ͳ����Ϣ
    SRM3100_MAGNETIC     rm3100MagneticDensity;   //�����Ÿ��ŵ�ͳ����Ϣ
    signed long   diffOfRM;                       //��ֵ
}*PSENSOR_RM3100, SSENSOR_RM3100;
/*******************��������**********************/



#endif   //_BIRTHPO_SX1280_RM3100_H_

