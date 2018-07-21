

#include "BerthPo_BottomValAdjustAdaptive.h"
#include "BerthPo_RM3100.h"
#include "BerthPo_StatisMagneticDensity.h"
#include "BerthPo_Common.h"
#include "BerthPo_CompileFiles.h"
/*********************引用外部变量*****************/

extern SBERTHPO_PARK_STATUS parkStatus;
extern SCONTROL_CONFIG	controlConfig;
extern SSENSOR_RM3100 sensorRm3100;

/*********************变量定义结束*************/
void BerthPo_GetDynamicBottom()   //取得实时动态本底
{

    //动态本底计次加一，通过inf信息将本计数上发
    if (sensorRm3100.getTimeBottomCount > 50)
        sensorRm3100.getTimeBottomCount = 1;
    else
        sensorRm3100.getTimeBottomCount++;

    //将最新的动态本底值赋给全局变量，在本底计算时使用
    sensorRm3100.rm3100DynamicBoottom.EMDataBottom_x =
        S_realTimeBottom.data_x;   //x动态本底值
    sensorRm3100.rm3100DynamicBoottom.EMDataBottom_y =
        S_realTimeBottom.data_y;   //y动态本底值
    sensorRm3100.rm3100DynamicBoottom.EMDataBottom_z =
        S_realTimeBottom.data_z;   //z动态本底值
}
/*
* 获取实时本底值
*/
SRM3100_EMData BerthPo_GetRealTimeBottom()
{

    static uint8_t mInit = 0xee;

    if (1 == sensorRm3100.clearBottomBufferFlag || mInit == 0xee)
    {
        S_realTimeBottom.data_x = (controlConfig.paramConfig.EMDataBottom_x );
        S_realTimeBottom.data_y = (controlConfig.paramConfig.EMDataBottom_y );
        S_realTimeBottom.data_z = (controlConfig.paramConfig.EMDataBottom_z );
        mInit = 0;
        sensorRm3100.getTimeBottomCount = 0;
    }
    return S_realTimeBottom;
}
/*
* 判断是否满足本底约束条件
*/
uint8_t BerthPo_SatisfyBottomConstraintCondition(PRM3100_MAGNETIC
        pRm3100Magnetic)
{

    short xBottom = controlConfig.paramConfig.EMDataBottom_x;
    short yBottom = controlConfig.paramConfig.EMDataBottom_y;
    short zBottom = controlConfig.paramConfig.EMDataBottom_z;
    short x_vary = (pRm3100Magnetic->x - xBottom);   //X强度变化矢量差,单位
    short y_vary = (pRm3100Magnetic->y - yBottom);   //y强度变化矢量差,单位
    short z_vary = (pRm3100Magnetic->z - zBottom);   //z强度变化矢量差,单位

    short diffOfRM = (short)(sqrtf(x_vary * x_vary + y_vary * y_vary + 2 * z_vary *
                                   z_vary));

    x_vary = (pRm3100Magnetic->x -
              S_realTimeBottom.data_x);   //X强度变化矢量差,单位
    y_vary = (pRm3100Magnetic->y -
              S_realTimeBottom.data_y);   //y强度变化矢量差,单位
    z_vary = (pRm3100Magnetic->z -
              S_realTimeBottom.data_z);   //z强度变化矢量差,单位

    short diffOfRM1 = (short)(sqrtf(x_vary * x_vary + y_vary * y_vary + 2 * z_vary *
                                    z_vary));

    if (abs(pRm3100Magnetic->x - S_realTimeBottom.data_x) <= 15 &&
        abs(pRm3100Magnetic->y - S_realTimeBottom.data_y) <= 15 &&
        abs(pRm3100Magnetic->z - S_realTimeBottom.data_z) <= 15 &&
        abs(pRm3100Magnetic->x - xBottom) <= 15 &&
        abs(pRm3100Magnetic->y - yBottom) <= 15 &&
        abs(pRm3100Magnetic->z - zBottom) <= 15 &&
        diffOfRM1 < 15 && diffOfRM < 20)
    {
        return 1;
    }

    return 0;

}

/*
* 本底值自适应调整
*/
uint8_t BerthPo_BottomvalAdjustProcess()
{
    static uint8_t mInit = 0xee;
    static BERTH_STATE m_lastParkState = BERTHPO_PARK_STATE_NULL;
    uint8_t m_currentParkStatus = parkStatus.currentParkState;
    SRM3100_SAMPLE_DATA m_sampleData;

    // 校准本底或初次运行 初始化统计信息
    if (1 == sensorRm3100.clearBottomBufferFlag || mInit == 0xee)
    {
        BerthPo_InitBottomTimesStatisSet((&sensorRm3100), &S_bottomTimes);
        BerthPo_InitBottomCnsTimeStatisSet(&(sensorRm3100), &S_bottomLongTime);
        BerthPo_GetRealTimeBottom();
        mInit = 0;
    }
    //取得无车磁场强度(地球磁场+环境磁场)
    m_sampleData.x = sensorRm3100.rm3100MagneticBase.x;
    m_sampleData.y = sensorRm3100.rm3100MagneticBase.y;
    m_sampleData.z = sensorRm3100.rm3100MagneticBase.z;
    m_sampleData.diffOfRM = sensorRm3100.rm3100MagneticBase.val;

    //有车或无车状态分别判断
    switch (m_currentParkStatus)
    {
        case BERTHPO_PARK_STATE_NULL:   //无车
            // 通过多次无车数据调整本底
            if (1 == S_noParkStartstatisFlag)
            {
                if (1 == sensorRm3100.rm3100MagneticBase.done)
                {
                    if (BerthPo_BottomTimesAdjustStatisProcess(&m_sampleData,
                            &(sensorRm3100), &S_bottomTimes) == 1)
                    {
                        // 判断是否满足本底调整条件
                        if (BerthPo_SatisfyBottomConstraintCondition(&S_bottomTimes) == 1)
                        {
                            // 调整本底
                            S_realTimeBottom.data_x = S_bottomTimes.x;   //x动态本底值
                            S_realTimeBottom.data_y = S_bottomTimes.y;   //y动态本底值
                            S_realTimeBottom.data_z = S_bottomTimes.z;   //z动态本底值
                            BerthPo_GetDynamicBottom();                  //取得实时动态本底
                        }
                        else
                        {
                            //报警，超过动态本底可调范围。如果下次可调整，则清除此标志
                            sensorRm3100.getTimeBottomCount = 63;
                        }
                    }
                    S_noParkStartstatisFlag = 0;
                }
            }
            // 通过长时间无车数据统计本底
            if (1 == sensorRm3100.rm3100MagneticBase.done)
            {
                if (BerthPo_BottomCnsTimeAdjustStatisProcess(&m_sampleData,
                        &(sensorRm3100), &S_bottomLongTime) == 1)
                {

                    // 判断和某一次无车数据是否一致
                    if (BerthPo_MatchTimesAndCnsTimeStatis(&(sensorRm3100),
                                                           &S_bottomLongTime) == 1)
                    {
                        // 判断是否满足本底调整条件
                        if (BerthPo_SatisfyBottomConstraintCondition(&S_bottomLongTime) == 1)
                        {
                            // 调整本底
                            S_realTimeBottom.data_x = S_bottomLongTime.x;  //x动态本底值
                            S_realTimeBottom.data_y = S_bottomLongTime.y;  //y动态本底值
                            S_realTimeBottom.data_z = S_bottomLongTime.z;  //z动态本底值

                            BerthPo_GetDynamicBottom();   //取得实时动态本底
                        }
                        else
                        {
                            // 报警，超过动态本底可调范围。如果下次可调整，则清除此标志
                            sensorRm3100.getTimeBottomCount = 63;
                        }
                        // 清空长时间无车统计
                        BerthPo_InitBottomCnsTimeStatisSet(&(sensorRm3100), &S_bottomLongTime);
                    }
                }
            }

            break;
        case BERTHPO_PARK_STATE_HAVE:   //有车

            // 初始化
            if (BERTHPO_PARK_STATE_NULL == m_lastParkState)
                S_noParkStartstatisFlag = 0;

            if (MAGNETIC_STATE_CONFIRM == sensorRm3100.rm3100MagneticDensity.state)
            {
                S_noParkStartstatisFlag = 1;
                // 清空长时间无车统计
                BerthPo_InitBottomCnsTimeStatisSet(&(sensorRm3100), &S_bottomLongTime);
            }
            break;
        default:
            break;
    }
    m_lastParkState = (BERTH_STATE)m_currentParkStatus;
    return 1;

}









