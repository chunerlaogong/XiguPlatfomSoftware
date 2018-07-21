
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "BerthPo_StatisMagneticDensity.h"
#include "BerthPo_Common.h"
#include "BerthPo_RM3100.h"

/****************变量定义*****************/


/****************变量定义结束************/

void BerthPo_InitStatisSet(PSENSOR_RM3100 pSensor3100,
                           PRM3100_STATISSET pRm3100StatisSet, PRM3100_MAGNETIC pRm3100Magnetic)
{

    memset(&(pSensor3100->statisSubset), 0, sizeof(SRM3100_STATISSET));
    memset(&(pSensor3100->statisSet), 0, sizeof(SRM3100_STATISSET));
    pSensor3100->statisSubset.size = STATIS_SUBSET_SIZE;
    pSensor3100->statisSet.size =    STATIS_SET_SIZE;
    pRm3100Magnetic->state = MAGNETIC_STATE_NULL;
    pRm3100Magnetic->level = MAGNETIC_MIDDLE;
    pRm3100Magnetic->val = 0;

}
void BerthPo_ZeroStatisSet(PRM3100_STATISSET pRm3100StatisSet)
{
    pRm3100StatisSet->len = 0;
}


void BerthPo_EnqueueStatisSet(PSENSOR_RM3100 pSensor3100,
                              PRM3100_STATISSET pRm3100StatisSet, PRM3100_SAMPLE_DATA pRm3100SampleData)
{
    uint8_t n = 0;
    if (pRm3100StatisSet->size == 0)
    {
        return;
    }
    n = MIN_(pRm3100StatisSet->size - 1, pRm3100StatisSet->len);
    pRm3100StatisSet->sampleData[n].x = pRm3100SampleData->x;
    pRm3100StatisSet->sampleData[n].y = pRm3100SampleData->y;
    pRm3100StatisSet->sampleData[n].z = pRm3100SampleData->z;
    pRm3100StatisSet->sampleData[n].diffOfRM = pRm3100SampleData->diffOfRM;
    pRm3100StatisSet->len = MIN_(pRm3100StatisSet->len + 1, pRm3100StatisSet->size);
}

uint8_t BerthPo_QueueFullStatisSet(PRM3100_STATISSET pRm3100StatisSet)
{
    return (pRm3100StatisSet->size == pRm3100StatisSet->len);
}

uint8_t BerthPo_QueueEmptyStatisSet(PRM3100_STATISSET pRm3100StatisSet)
{
    return (pRm3100StatisSet->len == 0);
}
void BerthPo_DequeueStatisSet(PRM3100_STATISSET pRm3100StatisSet)
{
    uint8_t n = 0;
    for (n = 1; n < pRm3100StatisSet->len; n++)
    {
        pRm3100StatisSet->sampleData[n - 1] = pRm3100StatisSet->sampleData[n];
    }
    pRm3100StatisSet->len = MAX_(pRm3100StatisSet->len - 1, 0);
}


void BerthPo_CalculateStatisVarianceMean(PRM3100_STATISSET pRm3100StatisSet)
{
    uint8_t n = 0;

    if (pRm3100StatisSet->size <= 0)
    {
        return;
    }
    // 均值
    pRm3100StatisSet->mean = 0;
    int mean = 0;
    for (n = 0; n < pRm3100StatisSet->size; n++)
    {
        mean += pRm3100StatisSet->sampleData[n].diffOfRM;
    }
    pRm3100StatisSet->mean = mean / pRm3100StatisSet->size;

    // 方差
    int var = 0;
    for (n = 0; n < pRm3100StatisSet->size; n++)
    {
        float diff = (pRm3100StatisSet->sampleData[n].diffOfRM -
                      pRm3100StatisSet->mean);
        var += diff * diff;
    }
    pRm3100StatisSet->var = (uint8_t)(var / pRm3100StatisSet->size);

    // 均值
    pRm3100StatisSet->mean_x = pRm3100StatisSet->mean_y = pRm3100StatisSet->mean_z =
                                   0;
    int mean_x = 0, mean_y = 0, mean_z = 0;
    for (n = 0; n < pRm3100StatisSet->size; n++)
    {
        mean_x += pRm3100StatisSet->sampleData[n].x;
        mean_y += pRm3100StatisSet->sampleData[n].y;
        mean_z += pRm3100StatisSet->sampleData[n].z;
    }

    float sign_x = mean_x >= 0 ? 0.5 : -0.5;
    float sign_y = mean_y >= 0 ? 0.5 : -0.5;
    float sign_z = mean_z >= 0 ? 0.5 : -0.5;
    pRm3100StatisSet->mean_x = (int16_t)((float)mean_x / pRm3100StatisSet->size +
                                         sign_x);
    pRm3100StatisSet->mean_y = (int16_t)((float)mean_y / pRm3100StatisSet->size +
                                         sign_y);
    pRm3100StatisSet->mean_z = (int16_t)((float)mean_z / pRm3100StatisSet->size +
                                         sign_z);

}
MAGNETIC_LEVEL BerthPo_JudgeMagnLevel(uint8_t val)
{
    MAGNETIC_LEVEL level;
    if (val > 40)
    {
        level = MAGNETIC_HIGHT;
    }
    else if (val > 20)
    {
        level = MAGNETIC_MIDDLE;
    }
    else
    {
        level = MAGNETIC_LOW;
    }
    return level;
}
uint8_t BerthPo_DealFullSet(PRM3100_STATISSET pRm3100StatisSet)
{

    uint8_t ret = 0;
    // 计算均值方差
    BerthPo_CalculateStatisVarianceMean(pRm3100StatisSet);

    if (pRm3100StatisSet->var < MAX_((uint8_t)(pRm3100StatisSet->mean * 0.1), 2))
    {
        // 清空
        BerthPo_ZeroStatisSet(pRm3100StatisSet);
        ret = 1;
    }
    else
    {
        // 出队
        BerthPo_DequeueStatisSet(pRm3100StatisSet);
    }
    return ret;
}

/*
* 统计实现
*/
uint8_t BerthPo_StatisProcess(PSENSOR_RM3100 pSensorRm3100,
                              PRM3100_SAMPLE_DATA pRm3100SampleData, PRM3100_MAGNETIC pRm3100Magnetic,
                              uint8_t cmd/*1:持续更新*/)
{

    pRm3100Magnetic->done = 0;

    if (MAGNETIC_STATE_CONFIRM == pRm3100Magnetic->state && 0 == cmd)
    {
        return 1;
    }

    // 统计集满
    if (1 == BerthPo_QueueFullStatisSet(&(pSensorRm3100->statisSet)))
    {
        if (1 == BerthPo_DealFullSet(&(pSensorRm3100->statisSet)))
        {
            // 判断强度级别
            pRm3100Magnetic->level = BerthPo_JudgeMagnLevel(pSensorRm3100->statisSet.mean);
            pRm3100Magnetic->state = MAGNETIC_STATE_CONFIRM;
            pRm3100Magnetic->val = pSensorRm3100->statisSet.mean;
            pRm3100Magnetic->x = pSensorRm3100->statisSet.mean_x;
            pRm3100Magnetic->y = pSensorRm3100->statisSet.mean_y;
            pRm3100Magnetic->z = pSensorRm3100->statisSet.mean_z;

            pRm3100Magnetic->done = 1;
        }
    }
    else   // 子集满
    {
        if (1 == BerthPo_QueueFullStatisSet(&(pSensorRm3100->statisSubset)))
        {
            if (1 == BerthPo_DealFullSet(&(pSensorRm3100->statisSubset)))
            {
                SRM3100_SAMPLE_DATA sampleData;
                sampleData.diffOfRM = pSensorRm3100->statisSubset.mean;
                sampleData.x = pSensorRm3100->statisSubset.mean_x;
                sampleData.y = pSensorRm3100->statisSubset.mean_y;
                sampleData.z = pSensorRm3100->statisSubset.mean_z;

                // 插入集
                BerthPo_EnqueueStatisSet(pSensorRm3100, &(pSensorRm3100->statisSet),
                                         &sampleData);

                if (MAGNETIC_STATE_NULL == pRm3100Magnetic->state)
                {
                    // 判断强度级别
                    pRm3100Magnetic->level = BerthPo_JudgeMagnLevel(
                                                 pSensorRm3100->statisSubset.mean);
                    pRm3100Magnetic->state = MAGNETIC_STATE_INIT;
                    pRm3100Magnetic->val = pSensorRm3100->statisSubset.mean;
                    pRm3100Magnetic->x = pSensorRm3100->statisSubset.mean_x;
                    pRm3100Magnetic->y = pSensorRm3100->statisSubset.mean_y;
                    pRm3100Magnetic->z = pSensorRm3100->statisSubset.mean_z;
                }
                else {}
            }
            else {}
        } // end if (1 == queueFullStatisSet(&g_statis_subset))
        else
        {
            // 插入子集
            BerthPo_EnqueueStatisSet(pSensorRm3100, &(pSensorRm3100->statisSubset),
                                     pRm3100SampleData);
        }
    }
    return 1;
}
/*
* 初始化有车且无车数据统计
*/
void BerthPo_InitBottomTimesStatisSet(PSENSOR_RM3100 pSensorRm3100,
                                      PRM3100_MAGNETIC pRm3100Magnetic)
{

    memset(&(pSensorRm3100->statisBottomTimesSet), 0, sizeof(SRM3100_STATISSET));
    pSensorRm3100->statisBottomTimesSet.size = BOTTOM_TIMES_STATIS_SET_SIZE;
    pRm3100Magnetic->state = MAGNETIC_STATE_NULL;
    pRm3100Magnetic->level = MAGNETIC_MIDDLE;
    pRm3100Magnetic->val = 0;
}

/*
* 初始化无车数据统计
*/
void BerthPo_InitBottomCnsTimeStatisSet(PSENSOR_RM3100 pSensorRm3100,
                                        PRM3100_MAGNETIC pRm3100Magnetic)
{

    memset(&(pSensorRm3100->statisBottomtimeSet), 0, sizeof(SRM3100_STATISSET));
    memset(&(pSensorRm3100->statisBottomTimeSubset), 0, sizeof(SRM3100_STATISSET));
    pSensorRm3100->statisBottomTimeSubset.size = BOTTOM_TIME_STATIS_SUBSET_SIZE;
    pSensorRm3100->statisBottomtimeSet.size = BOTTOM_TIME_STATIS_SET_SIZE;
    pRm3100Magnetic->state = MAGNETIC_STATE_NULL;
    pRm3100Magnetic->level = MAGNETIC_MIDDLE;
    pRm3100Magnetic->val = 0;
}
/*
* 计算统计方差和均值
*/
void BerthPo_CalculateBottomStatisVarianceMean(PRM3100_STATISSET
        pRm3100StatisSet)
{
    uint8_t i = 0;

    if (pRm3100StatisSet->size <= 0)
    {
        return;
    }
    // 均值
    pRm3100StatisSet->mean_x = pRm3100StatisSet->mean_y = pRm3100StatisSet->mean_z =
                                   0;
    int mean_x = 0, mean_y = 0, mean_z = 0;
    for (i = 0; i < pRm3100StatisSet->size; i++)
    {
        mean_x += pRm3100StatisSet->sampleData[i].x;
        mean_y += pRm3100StatisSet->sampleData[i].y;
        mean_z += pRm3100StatisSet->sampleData[i].z;
    }
    pRm3100StatisSet->mean_x = mean_x / pRm3100StatisSet->size;
    pRm3100StatisSet->mean_y = mean_y / pRm3100StatisSet->size;
    pRm3100StatisSet->mean_z = mean_z / pRm3100StatisSet->size;

    // 方差
    int var_x = 0, var_y = 0, var_z = 0;
    for (i = 0; i < pRm3100StatisSet->size; i++)
    {
        short diff = (pRm3100StatisSet->sampleData[i].x - pRm3100StatisSet->mean_x);
        var_x += diff * diff;
        diff = (pRm3100StatisSet->sampleData[i].y - pRm3100StatisSet->mean_y);
        var_y += diff * diff;
        diff = (pRm3100StatisSet->sampleData[i].z - pRm3100StatisSet->mean_z);
        var_z += diff * diff;
    }
    pRm3100StatisSet->var_x = TYPE_U8(var_x / pRm3100StatisSet->size);
    pRm3100StatisSet->var_y = TYPE_U8(var_y / pRm3100StatisSet->size);
    pRm3100StatisSet->var_z = TYPE_U8(var_z / pRm3100StatisSet->size);
}
/*
* 剔除数据
*/
void BerthPo_DeleteDataStatisSet(PRM3100_STATISSET rm3100StatisSet)
{
    uint8_t n = 0, k = 0;
    uint8_t maxDevIndex = 0;
    short dev = 0, maxDev = 0;     //偏差
    for (n = 0; n < rm3100StatisSet->len; n++)
    {
        dev =  abs(rm3100StatisSet->sampleData[n].x - rm3100StatisSet->mean_x);
        dev += abs(rm3100StatisSet->sampleData[n].y - rm3100StatisSet->mean_y);
        dev += abs(rm3100StatisSet->sampleData[n].z - rm3100StatisSet->mean_z);
        if (dev > maxDev)
        {
            maxDev = dev;
            maxDevIndex = n;
        }
    }

    // 防止一直去除最新数据
    if (maxDevIndex == rm3100StatisSet->len - 1)
        maxDevIndex = 0;

    for (n = 0, k = 0; n < rm3100StatisSet->len; n++)
    {
        if (n == maxDevIndex)
            continue;
        rm3100StatisSet->sampleData[k++] = rm3100StatisSet->sampleData[n];
    }
    rm3100StatisSet->len = MAX_(rm3100StatisSet->len - 1, 0);
}


/*
* 满集处理
*/
uint8_t BerthPo_DealFullBottomSet(PRM3100_STATISSET rm3100StatisSet)
{
    uint8_t ret = 0;
    // 计算均值方差
    BerthPo_CalculateBottomStatisVarianceMean(rm3100StatisSet);
    if (rm3100StatisSet->var_x <= 4 && rm3100StatisSet->var_y <= 4 &&
        rm3100StatisSet->var_z <= 4)
    {
        // 清空
        BerthPo_ZeroStatisSet(rm3100StatisSet);
        ret = 1;
    }
    else
    {
        // 剔除数据
        BerthPo_DeleteDataStatisSet(rm3100StatisSet);
    }
    return ret;
}
/*
* 有车且无车时统计实现
*/
uint8_t BerthPo_BottomTimesAdjustStatisProcess(PRM3100_SAMPLE_DATA
        pRm3100SampleData, PSENSOR_RM3100 pSensorRm3100,
        PRM3100_MAGNETIC rm3100Magnetic)
{

    // 插入子集
    BerthPo_EnqueueStatisSet(pSensorRm3100, &(pSensorRm3100->statisBottomTimesSet),
                             pRm3100SampleData);

    if (1 == BerthPo_QueueFullStatisSet(&(pSensorRm3100->statisBottomTimesSet)))
    {
        if (1 == BerthPo_DealFullBottomSet(&(pSensorRm3100->statisBottomTimesSet)))
        {
            rm3100Magnetic->state = MAGNETIC_STATE_CONFIRM;
            rm3100Magnetic->val = pSensorRm3100->statisBottomTimesSet.mean;
            rm3100Magnetic->x = pSensorRm3100->statisBottomTimesSet.mean_x;
            rm3100Magnetic->y = pSensorRm3100->statisBottomTimesSet.mean_y;
            rm3100Magnetic->z = pSensorRm3100->statisBottomTimesSet.mean_z;
            return 1;
        }
    }
    return 0;
}
/*
* 无车统计实现
*/
uint8_t BerthPo_BottomCnsTimeAdjustStatisProcess(PRM3100_SAMPLE_DATA
        pRm3100SampleData, PSENSOR_RM3100 pSensorRm3100,
        PRM3100_MAGNETIC pRm3100Magnetic )
{
    {
        // 插入到子集
        BerthPo_EnqueueStatisSet(pSensorRm3100,
                                 &(pSensorRm3100->statisBottomTimeSubset), pRm3100SampleData);

        // 子集满
        if (1 == BerthPo_QueueFullStatisSet(&(pSensorRm3100->statisBottomTimeSubset)))
        {
            if (1 == BerthPo_DealFullBottomSet(&(pSensorRm3100->statisBottomTimeSubset)))
            {
                SRM3100_SAMPLE_DATA m_rm3100SampleData;
                m_rm3100SampleData.diffOfRM = pSensorRm3100->statisBottomTimeSubset.mean;
                m_rm3100SampleData.x = pSensorRm3100->statisBottomTimeSubset.mean_x;
                m_rm3100SampleData.y = pSensorRm3100->statisBottomTimeSubset.mean_y;
                m_rm3100SampleData.z = pSensorRm3100->statisBottomTimeSubset.mean_z;

                // 插入到集
                BerthPo_EnqueueStatisSet(pSensorRm3100, &(pSensorRm3100->statisBottomtimeSet),
                                         &m_rm3100SampleData);
            }
        }

        // 统计集满
        if (1 == BerthPo_QueueFullStatisSet(&(pSensorRm3100->statisBottomtimeSet)))
        {
            if (1 == BerthPo_DealFullBottomSet(&(pSensorRm3100->statisBottomtimeSet)))
            {
                pRm3100Magnetic->state = MAGNETIC_STATE_CONFIRM;
                pRm3100Magnetic->val = pSensorRm3100->statisBottomtimeSet.mean;
                pRm3100Magnetic->x = pSensorRm3100->statisBottomtimeSet.mean_x;
                pRm3100Magnetic->y = pSensorRm3100->statisBottomtimeSet.mean_y;
                pRm3100Magnetic->z = pSensorRm3100->statisBottomtimeSet.mean_z;
                return 1;
            }
        }
        return 0;

    }
}
/*
    * 判断长时间无车统计和某一次无车数据是否一致
    */
uint8_t BerthPo_MatchTimesAndCnsTimeStatis(PSENSOR_RM3100 pSensorRm3100,
        PRM3100_MAGNETIC pRm3100Magnetic)
{
    uint8_t i = 0;

    for (i = 0; i < pSensorRm3100->statisBottomTimesSet.size; i++)
    {
        if (abs(pRm3100Magnetic->x -
                pSensorRm3100->statisBottomTimesSet.sampleData[i].x) <= 3 &&
            abs(pRm3100Magnetic->y - pSensorRm3100->statisBottomTimesSet.sampleData[i].y) <=
            3 &&
            abs(pRm3100Magnetic->z - pSensorRm3100->statisBottomTimesSet.sampleData[i].z) <=
            3)
        {
            return 1;
        }
    }
    return 0;
}

/*
* 判断本底偏移值(未判断或未偏移都返回0)
*/
uint8_t BerthPo_GetBottomOffsetValue(PSENSOR_RM3100 pSensorRm3100)
{

    uint8_t i = 0;
    uint8_t m_offsetCnt = 0;
    uint8_t m_offsetSum = 0;

    for (i = 0; i < pSensorRm3100->statisBottomTimesSet.size; i++)
    {
        if (0 == pSensorRm3100->statisBottomTimesSet.sampleData[i].x &&
            0 == pSensorRm3100->statisBottomTimesSet.sampleData[i].y &&
            0 == pSensorRm3100->statisBottomTimesSet.sampleData[i].z)
        {
            continue;
        }

        m_offsetCnt++;
        m_offsetSum += pSensorRm3100->statisBottomTimesSet.sampleData[i].diffOfRM;
    }
    return ((m_offsetCnt == 0) ? 0 : (uint8_t)(m_offsetSum / m_offsetCnt));
}




