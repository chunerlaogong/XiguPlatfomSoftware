
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "BerthPo_ParkStateJudge.h"
#include "BerthPo_RM3100.h"
#include "Drivers_RM3100.h"
#include "BerthPo_Common.h"
#include "BerthPo_Protocol.h"
#include "Drivers_Led.h"
#include "Sleep.h"
#include "Crc.h"
#include "BerthPo_CompileFiles.h"
#include "Drivers_SX1280.h"
#include "BSP_RTC.h"
/***************引用外部变量******************/
extern SBERTHPO_PARK_STATUS parkStatus;
extern SCONTROL_CONFIG  controlConfig;
extern SSENSOR_RM3100 sensorRm3100;
extern LED_GPIO_TypeDef LED_GPIO;
extern SCONTROL_SYMPLE tagConfigSymple;
/*
* 初始化状态判断输入参数
*/
InputInfo BerthPo_InitBerthStateInput()
{

    InputInfo input;
    input.x = (sensorRm3100.rm3100CurrentData.EMDataCurrent_x );
    input.y = (sensorRm3100.rm3100CurrentData.EMDataCurrent_y );
    input.z = (sensorRm3100.rm3100CurrentData.EMDataCurrent_z );

    input.x_b = TYPE_S8(controlConfig.paramConfig.EMDataBottom_x);
    input.y_b = TYPE_S8(controlConfig.paramConfig.EMDataBottom_y);
    input.z_b = TYPE_S8(controlConfig.paramConfig.EMDataBottom_z);

    input.diffOfRM = (sensorRm3100.diffOfRM > 127) ? 127 :
                     (uint8_t)sensorRm3100.diffOfRM;
    input.RM_thr = (uint8_t)
                   controlConfig.paramConfig.getEMBottom_RFModThreshold;
    input.angle_thr = (uint8_t)
                      controlConfig.paramConfig.getEMBottom_RFAngleThreshold;
    return input;
}

/*
* 泊位状态切换
*/
uint8_t BerthPo_BerthStateSwithProcess()
{
    uint8_t ret = 0;
    static uint8_t m_init = 0xee;
    static BERTH_STATE m_lastState =
        BERTHPO_PARK_STATE_NULL;             //历史车位状态
    BERTH_STATE currentState =
        g_parkState;                      //当前车位状态
    SRM3100_SAMPLE_DATA
    rm3100SampleData;    //临时变量，用于缓存计算过程中的中间值
    //初始化状态判断输入参数
    InputInfo input = BerthPo_InitBerthStateInput();
    // 计算特征值
    BerthPo_CalculateMeagneticFeature(&input, &rm3100SampleData);
    //初次调用或清除缓存标志置一时，清除相关缓存区域
    if (1 == sensorRm3100.clearBottomBufferFlag || m_init == 0xee)
    {
        BerthPo_InitStatisSet(&sensorRm3100, &(sensorRm3100.rm3100MagneticBase));
        BerthPo_InitStatisSet(&sensorRm3100, &(sensorRm3100.rm3100MagneticDensity));
        memset(&S_gNoParkData, 0, sizeof(SRM3100_EMData));
        memset(&(sensorRm3100.rm3100MagneticBaseLower), 0,
               sizeof(SRM3100_MAGNETIC));
        m_init = 0;
    }
    // 此处调用动态本底函数，对动态本底调整功能进行处理
    BerthPo_BottomvalAdjustProcess();
    //当前车位状态
    switch (currentState)
    {
        case BERTHPO_PARK_STATE_NULL://无车

            //如果//当前车位状态与历史车位状态不符，则需要清除记录缓存区中的数据
            if (BERTHPO_PARK_STATE_HAVE == m_lastState)
            {
                BerthPo_InitStatisSet(&sensorRm3100, &
                                      (sensorRm3100.rm3100MagneticBase));      // 无车磁场强度(地球磁场+环境磁场)
                BerthPo_InitStatisSet(&sensorRm3100, &
                                      (sensorRm3100.rm3100MagneticDensity));   // 车辆磁干扰_有车状态下使用

                sensorRm3100.rm3100MagneticBaseLower.val = input.diffOfRM;
                sensorRm3100.rm3100MagneticBaseLower.x = input.x;
                sensorRm3100.rm3100MagneticBaseLower.y = input.y;
                sensorRm3100.rm3100MagneticBaseLower.z = input.z;
            }

            // 统计当前数据，用于计算
            BerthPo_StatisProcess(&sensorRm3100, &rm3100SampleData,
                                  &(sensorRm3100.rm3100MagneticBase), 1);

            // 保存最接近本底的无车统计值
            if (1 == sensorRm3100.rm3100MagneticBase.done)
            {
                // 忽略第一次统计值 防止低磁车辆开始值偏小的情况
                if (BERTHPO_PARK_STATE_NULL ==
                    sensorRm3100.rm3100MagneticBaseLower.state)
                {
                    sensorRm3100.rm3100MagneticBaseLower.state = MAGNETIC_STATE_CONFIRM;
                }
                else if (sensorRm3100.rm3100MagneticBaseLower.val >
                         sensorRm3100.rm3100MagneticBase.val)
                {
                    sensorRm3100.rm3100MagneticBaseLower =
                        sensorRm3100.rm3100MagneticBase;
                }
            }

            // 未完成统计时 保存最接近本底的无车采集值
            if (BERTHPO_PARK_STATE_NULL ==
                sensorRm3100.rm3100MagneticBaseLower.state &&
                sensorRm3100.rm3100MagneticBaseLower.val > input.diffOfRM)
            {
                sensorRm3100.rm3100MagneticBaseLower.val = input.diffOfRM;
                sensorRm3100.rm3100MagneticBaseLower.x = input.x;
                sensorRm3100.rm3100MagneticBaseLower.y = input.y;
                sensorRm3100.rm3100MagneticBaseLower.z = input.z;
            }

            // 如果发生了状态改变，则需要处理后续标志
            if (BerthPo_ChangeConfirm(&input) == STATE_REVERSAL)
            {
                ret = STATE_REVERSAL;
                parkStatus.currentParkState = 1;
                g_parkState = BERTHPO_PARK_STATE_HAVE;
            }
            break;
        case BERTHPO_PARK_STATE_HAVE:    //有车
            //如果当前车位状态与历史车位状态不符，则需要清除记录缓存区中的数据
            if (BERTHPO_PARK_STATE_NULL == m_lastState)
            {
                BerthPo_InitStatisSet(&sensorRm3100, &
                                      (sensorRm3100.rm3100MagneticDensity));// 车辆磁干扰_有车状态下使用
                memset(&(sensorRm3100.rm3100MagneticBaseLower), 0,
                       sizeof(SRM3100_MAGNETIC));
            }

            // 统计当前数据，用于计算
            BerthPo_StatisProcess(&sensorRm3100, &rm3100SampleData,
                                  &(sensorRm3100.rm3100MagneticDensity), 1);

            // 记录最近一次停车统计状态
            S_gParkState = sensorRm3100.rm3100MagneticDensity.state;

            // 如果发生了状态改变，则需要处理后续标志
            if (BerthPo_ChangeConfirm(&input) == STATE_REVERSAL)
            {
                ret = STATE_REVERSAL;
                parkStatus.currentParkState = 0;
                g_parkState = BERTHPO_PARK_STATE_NULL;
            }
            break;
        default:
            break;
    }
    m_lastState = (BERTH_STATE)currentState;           //将当前状态保存到历史状态中
    sensorRm3100.clearBottomBufferFlag = 0;    //清本底缓存标志关闭
    return ret;
}

/*
* 获取变化阈值
*/

/*******************************************************************************
* Function Name :uint8_t SetRm3100Base()
* Description   :设置RM3100地磁本底
* Author        : YJD
* Input         :thr1: 绝对阈值       thr2: 相对阈值
* Output        :
* Other         :
* Date          :2018.05.08
*******************************************************************************/


void BerthPo_ChangeThresholdGet(uint8_t *thr1,
                                uint8_t *thr2, InputInfo* pInput)
{

    uint8_t offset = 0;
    //有车或无车时，阈值设定不一致
    // 有车时
    if (BERTHPO_PARK_STATE_HAVE == g_parkState)
    {
        offset = sensorRm3100.rm3100MagneticBase.val;
        offset = MIN_(offset, 10);

        if (MAGNETIC_STATE_NULL == sensorRm3100.rm3100MagneticDensity.state)
        {

            //绝对阈值
            *thr1 = (uint8_t)(pInput->RM_thr * 8 / 10 + offset * offset * 0.0381 + offset *
                              0.1216 + 0.015);

            //相对阈值
            *thr2 = pInput->diffOfRM;
        }
        else    //if (MAGNETIC_STATE_INIT == g_magnetic_density.state) {
        {

            //绝对阈值
            *thr1 = (uint8_t)(MIN_(pInput->RM_thr * 8 / 10 + offset * offset * 0.0381 +
                                   offset
                                   * 0.1216 + 0.015, sensorRm3100.rm3100MagneticDensity.val * 8 / 10));
            *thr1 = MAX_(*thr1, 8);

            //相对阈值
            if (MAGNETIC_STATE_INIT == sensorRm3100.rm3100MagneticDensity.state)
                *thr2 = sensorRm3100.rm3100MagneticDensity.val * 8 / 10;
            else
                *thr2 = MAX_(MIN_(sensorRm3100.rm3100MagneticDensity.val * 5 / 10, 30), 10);
        }
    }
    else
    {
        offset = sensorRm3100.rm3100MagneticBase.val;
        offset = MIN_(offset, 10);

        //绝对阈值
        *thr1 = (uint8_t)(pInput->RM_thr + MAX_(0,
                                                offset * offset * 0.0381 + offset * 0.1216 + 0.015 - 1.0f));

        //相对阈值
        if (MAGNETIC_STATE_NULL == sensorRm3100.rm3100MagneticBase.state)
        {
            *thr2 = (uint8_t)(pInput->diffOfRM * 2);
        }
        else
        {
            if (MAGNETIC_STATE_INIT == sensorRm3100.rm3100MagneticBase.state)
                *thr2 =  (uint8_t)(pInput->RM_thr + MAX_(offset * 0.6, 6));
            else
                *thr2 = (uint8_t)(pInput->RM_thr + MAX_(offset * 0.5, 5));
        }
    }

}

/*
* 计算磁特征
*/
void BerthPo_CalculateMeagneticFeature(InputInfo* pInput,
                                       PRM3100_SAMPLE_DATA pRm3100SampleData)
{
    memset(pRm3100SampleData, 0, sizeof(SRM3100_SAMPLE_DATA));
    pRm3100SampleData->x = pInput->x;
    pRm3100SampleData->y = pInput->y;
    pRm3100SampleData->z = pInput->z;
    pRm3100SampleData->diffOfRM = pInput->diffOfRM;
    // 角度计算

}


/*
* 变化判断
*/
uint8_t BerthPo_ChangeJudge(InputInfo* pInput)
{
    uint8_t ret = 0;
    uint8_t thr_absol = 0;  // 绝对阈值
    uint8_t thr_change = 0; // 变化幅度阈值
    int16_t range_RM = 0;
    int16_t range_abs_diff = 0;
    int16_t range_RM1 = 0;

    // 查询阈值
    BerthPo_ChangeThresholdGet(&thr_absol, &thr_change,
                               pInput);

    // 有车时
    if (BERTHPO_PARK_STATE_HAVE == g_parkState)
    {
        // range_RM 降幅
        if (MAGNETIC_STATE_NULL == sensorRm3100.rm3100MagneticDensity.state)
        {
            range_RM = 0;

        }
        else
        {

            // 当前值有车统计的xyz绝对差值
            range_abs_diff = abs(sensorRm3100.rm3100MagneticDensity.x - pInput->x) +
                             \
                             abs(sensorRm3100.rm3100MagneticDensity.y - pInput->y) + \
                             abs(sensorRm3100.rm3100MagneticDensity.z - pInput->z);

            // range_RM 变化值(取与本底的模差值和有车统计的xyz绝对差值的最大值)
            // 变小
            if (sensorRm3100.rm3100MagneticDensity.val > pInput->diffOfRM)
            {

                range_RM = (sensorRm3100.rm3100MagneticDensity.val - pInput->diffOfRM);
                range_RM = 0 - MAX_(range_RM, MIN_(range_abs_diff, range_RM * 2));

            }
            else
            {
                range_RM = (pInput->diffOfRM -
                            sensorRm3100.rm3100MagneticDensity.val); // 变大不使用变化判断
            }
        }

        // xyz值回落到无车时统计值值附近
        range_RM1 = abs(sensorRm3100.rm3100MagneticBase.x - pInput->x) + \
                    abs(sensorRm3100.rm3100MagneticBase.y - pInput->y) + \
                    abs(sensorRm3100.rm3100MagneticBase.z - pInput->z);

        /**********************************有车到无车判断**********************************/

        // 通过基本回到本底附近判断无车
        if (pInput->diffOfRM < thr_absol)
        {
            ret = 1;
        }
        else if (MAGNETIC_STATE_NULL !=
                 sensorRm3100.rm3100MagneticDensity.state
                 &&
                 range_RM1 < pInput->RM_thr / 2 && range_abs_diff > pInput->RM_thr / 2)
        {
            ret = 1;
        }

        // 通过扰动值的变化幅度判断无车
        if ((0 - range_RM) > thr_change)
        {
            ret = 1;
        }

    }
    else
    {
        // 变大
        if (sensorRm3100.rm3100MagneticBase.val < pInput->diffOfRM)
        {

            range_RM = (pInput->diffOfRM - sensorRm3100.rm3100MagneticBase.val);
        }
        else
        {
            range_RM = (sensorRm3100.rm3100MagneticBase.val - pInput->diffOfRM);
        }


        // 当前采样值与上一次采样值(无跳变)的绝对差值
        signed short diff_nopark = abs(S_gNoParkData.data_x - pInput->x) + \
                                   abs(S_gNoParkData.data_y - pInput->y) + \
                                   abs(S_gNoParkData.data_z - pInput->z);

        // xyz值回落到上一次有车值附近(用于停车后干扰导致一直无车的情况)
        signed short diff_park = 0;
        if (S_gParkState != MAGNETIC_STATE_NULL)
        {
            diff_park = abs(sensorRm3100.rm3100MagneticDensity.x - pInput->x) + \
                        abs(sensorRm3100.rm3100MagneticDensity.y - pInput->y) + \
                        abs(sensorRm3100.rm3100MagneticDensity.z - pInput->z);
        }
        else
        {
            diff_park = 127;
        }

        // range_RM 变化值(取与本底的模差值和无车时统计的xyz绝对差值的最大值)
        if (MAGNETIC_STATE_NULL != sensorRm3100.rm3100MagneticBase.state)
        {
            range_abs_diff = abs(sensorRm3100.rm3100MagneticBase.x - pInput->x) + \
                             abs(sensorRm3100.rm3100MagneticBase.y - pInput->y) + \
                             abs(sensorRm3100.rm3100MagneticBase.z - pInput->z);
        }
        range_RM = MAX_(range_RM, range_abs_diff);


        // range_RM1 变化值(取与本底最相近的统计值，无统计值时参考与diff_nopark最大值)
        range_RM1 = abs(sensorRm3100.rm3100MagneticBaseLower.x - pInput->x) + \
                    abs(sensorRm3100.rm3100MagneticBaseLower.y - pInput->y) + \
                    abs(sensorRm3100.rm3100MagneticBaseLower.z - pInput->z);
        if (MAGNETIC_STATE_CONFIRM != sensorRm3100.rm3100MagneticBaseLower.state)
        {
            range_RM1 = MAX_(diff_nopark, range_RM1);
        }

        // 本底偏移值
        uint8_t bottomOffsetValue = BerthPo_GetBottomOffsetValue(&sensorRm3100);
        if (bottomOffsetValue == 0)
        {
            bottomOffsetValue = sensorRm3100.rm3100MagneticBase.val;
        }

        /**********************************无车到有车判断**********************************/
        // 通过与本底模差值判断有车
        if (pInput->diffOfRM > MIN_(thr_absol * 2, 20))
        {
            ret = 1;
        }
        else if (pInput->diffOfRM >= thr_absol)
        {
            if (range_RM1 > pInput->RM_thr)
            {
                ret = 1;
            }
            else if (diff_park < 5 && bottomOffsetValue <= 8)
            {
                ret = 1;
            }
        }

        // 通过与本底(x, y, z)变化幅度判断有车
        if (range_RM > thr_change && pInput->diffOfRM >= 5)
        {
            ret = 1;
        }

        // 通过本底(x, y, z)变化权值判断有车
        if (BerthPo_FixedVCheck() >= 3 && diff_nopark > pInput->RM_thr / 2)
        {
            ret = 1;
        }
    }


    return ret;

}


/*
* 变化确认(状态反转)
*/
uint8_t BerthPo_ChangeConfirm(InputInfo* pInput)
{
    uint8_t ret = 0;

    if (BerthPo_ChangeJudge(pInput) == 1)
    {
        uint8_t i = 0;

        uint16_t sleepTime = 1024;    //睡眠时间，由车位状态确定，默认为快速
        if (parkStatus.currentParkState == 0)  sleepTime =
                2048; //车位状态决定确认速度，无车-->有车，慢速确认，有车-->无车，快速确认

        ret = STATE_REVERSAL;//默认状态发生了反转

        parkStatus.fastGetParkCount = 0;
        for (i = 0; i < 3; i++)
        {
            Drivers_LedOff(LED_GPIO, 2);
            enableInterrupts();          //开中断,关中断动作在睡眠被唤醒后执行
            RTCAlarm_Set(sleepTime);   //rtc睡眠时间
            //BSP_RtcDeepSleep();          //进入睡眠
            BerthPo_GetRM3100Data();   //取地磁数据
            BerthPo_EMDealGeomagneticValue();                //处理地磁数据

            // 重新初始化input
            InputInfo input_temp = BerthPo_InitBerthStateInput();
            if (BerthPo_ChangeJudge(&input_temp) ==
                0)  //如果快速检测中发现有抖动，则放弃本次检测进程
            {

                parkStatus.fastGetParkCount = 3;   //启动一次快速检测
                ret = 0;    //取消状态反转标志，立即退出检查转态

                break;
            }
        }
        Drivers_LedOff(LED_GPIO, 2);

        // 及时保存无车时数据
        if (1 == parkStatus.currentParkState)
        {
            S_gNoParkData.data_x = pInput->x;
            S_gNoParkData.data_y = pInput->y;
            S_gNoParkData.data_z = pInput->z;
        }
    }
    else
    {
        if (0 == parkStatus.currentParkState)
        {
            S_gNoParkData.data_x = pInput->x;
            S_gNoParkData.data_y = pInput->y;
            S_gNoParkData.data_z = pInput->z;
        }
    }

    return ret;

}
uint8_t BerthPo_SendAlarmPackage(uint8_t frameType)
{
    SFRAME_DATA_TypeDef m_sendPackage;
    uint8_t i = 0;
    memset(&m_sendPackage, 0, sizeof(SFRAME_DATA_TypeDef));
    BerthPo_MakeHead(&m_sendPackage);
    for(i = 0; i < 6; i++)
    {
        m_sendPackage.packageHeader.deviceNumber[i] = controlConfig.nodeConfig.idNumber[i];
    }
    m_sendPackage.packageHeader.frameType = frameType;
    m_sendPackage.txPayLoad[0] =
        controlConfig.paramConfig.alarmValid;        //有效状态
    m_sendPackage.txPayLoad[1] = controlConfig.paramConfig.alarmValid >>
                                 8;     //有效状态
    m_sendPackage.txPayLoad[2] =
        controlConfig.paramConfig.alarmStatus;      //车状态
    m_sendPackage.txPayLoad[3] = controlConfig.paramConfig.alarmStatus >>
                                 8;   //车状态

    m_sendPackage.txPayLoad[4] = ((uint8_t)(
                                      sensorRm3100.rm3100CurrentData.EMDataCurrent_x));   //x实时值
    m_sendPackage.txPayLoad[5] = ((uint8_t)(
                                      sensorRm3100.rm3100CurrentData.EMDataCurrent_y));//y实时值
    m_sendPackage.txPayLoad[6] = ((uint8_t)(
                                      sensorRm3100.rm3100CurrentData.EMDataCurrent_z));//z实时值
    m_sendPackage.txPayLoad[7] = ((uint8_t)((sensorRm3100.diffOfRM))); //模差
    switch (m_sendPackage.txPayLoad[10] >> 6)
    {
        case 0:
        {
            tagConfigSymple.outBottomFlag++;
            tagConfigSymple.outBottomFlag = tagConfigSymple.outBottomFlag % 2;
            m_sendPackage.txPayLoad[8] = tagConfigSymple.outBottomFlag;
            break;
        }
        case 1:
        {
            if (tagConfigSymple.outBottomFlag == 0)
                m_sendPackage.txPayLoad[8] = ((uint8_t)(
                                                  controlConfig.paramConfig.EMDataBottom_x));     //x固定本底值
            else
                m_sendPackage.txPayLoad[8] = ((uint8_t)(
                                                  sensorRm3100.rm3100DynamicBoottom.EMDataBottom_x));                        //x动态本底值
            break;
        }
        case 2:
        {
            if (tagConfigSymple.outBottomFlag == 0)
                m_sendPackage.txPayLoad[8] = ((uint8_t)(
                                                  controlConfig.paramConfig.EMDataBottom_y));     //y固定本底值
            else
                m_sendPackage.txPayLoad[8] = ((uint8_t)(
                                                  sensorRm3100.rm3100DynamicBoottom.EMDataBottom_y));   //y动态本底值
            break;
        }
        case 3:
        {
            if (tagConfigSymple.outBottomFlag == 0)
                m_sendPackage.txPayLoad[8] = ((uint8_t)(
                                                  controlConfig.paramConfig.EMDataBottom_z));//y固定本底值
            else
                m_sendPackage.txPayLoad[8] = ((uint8_t)(
                                                  sensorRm3100.rm3100DynamicBoottom.EMDataBottom_z));//y动态本底值
            break;
        }
    }
    uint8_t frameTotalLens = 12 + m_sendPackage.packageHeader.frameLens;
    uint8_t m_sendBuf[9 + 2 + 12] = {0};
    memcpy(m_sendBuf, (uint8_t *) & (m_sendPackage.packageHeader),
           sizeof(SFRAME_HEAD_TypeDef));
    memcpy(m_sendBuf + sizeof(SFRAME_HEAD_TypeDef),
           (uint8_t *)m_sendPackage.txPayLoad, 9);
    Calculate_Crc16((unsigned char *)m_sendBuf, frameTotalLens - 2);
    Drivers_LedOn(LED_GPIO, 2);
    /*if(Drivers_BC95_Operation.SocketSendData(&BC95_GPIO, m_sendBuf,
            frameTotalLens) == MC_ERROR)
    {
        Drivers_BC95_Operation.NetWork_Manage(&BC95_GPIO);   //发送数据失败，重新组网
        return -1;
    }*/
    return 1;
}
uint8_t BerthPo_GetVccInfo()
{
    uint8_t m_test;
    uint32_t m_vcc;
    m_test = tagConfigSymple.debugInfoFlag << 6;
    switch(tagConfigSymple.debugInfoFlag)//调试信息标志
    {
        case 0:   //电池电压
        {
            if(tagConfigSymple.batteryCount >= 0)
            {
                m_vcc = Drivers_GetBattryVCC();   //取得电池电压
                if(m_vcc == 0)
                {
                    tagConfigSymple.batteryCheckCount++;
                    tagConfigSymple.batteryCount = 8;
                    if(tagConfigSymple.batteryCheckCount >= 4)
                    {
                        nop();
                        tagConfigSymple.batteryCheckCount = 0;
                    }
                }
                else if (m_vcc >= 63)
                {
                    m_vcc = 63;                       //限制显示电压最大值
                    tagConfigSymple.batteryCheckCount = 0;
                }
                else
                {
                    tagConfigSymple.batteryCheckCount = 0;
                }
            }

            m_test = m_test | (uint8_t)m_vcc;
            nop();
            break;
        }
        case 1:   //报警信息-修复本底时间-本底调整次数
        {
            m_test = m_test | (sensorRm3100.getTimeBottomCount % 64);
            break;
        }
        case 2://碰撞次数
        {
            m_test = m_test | (tagConfigSymple.batteryCheckCount % 64);
            break;
        }
        case 3://软件版本
        {
            m_test = m_test | SYSTEM_VERSION;
            break;
        }
    }
    ++(tagConfigSymple.debugInfoFlag);
    tagConfigSymple.debugInfoFlag = (tagConfigSymple.debugInfoFlag % 4);
    return m_test;
}
uint8_t BerthPo_SendCarStatus(uint8_t frameType)
{

    uint8_t m_test = 0;
    uint8_t m_InfData = 0;
    uint16_t tmp[2] = {0};
    controlConfig.paramConfig.alarmStatus & 0x00; //初始化报警位状态
    if(parkStatus.currentParkState == 1)
    {
        controlConfig.paramConfig.alarmStatus =
            controlConfig.paramConfig.alarmStatus | 0x0400;//有车
    }
    else
    {
        controlConfig.paramConfig.alarmStatus =
            controlConfig.paramConfig.alarmStatus | 0x0800;//无车
    }

    if (controlConfig.paramConfig.fastRouseAlarm > 20)
    {
        controlConfig.paramConfig.fastRouseAlarm = 0;
        controlConfig.paramConfig.alarmStatus =
            controlConfig.paramConfig.alarmStatus | 0x4000;//频繁唤醒报警
    }

    m_InfData = BerthPo_GetVccInfo();

    controlConfig.paramConfig.alarmStatus =
        controlConfig.paramConfig.alarmStatus | (uint16_t)
        m_InfData; //将调试信息放入报警信息中，借用报警信息低字节

    controlConfig.paramConfig.alarmValid = 0x4c00 |
                                           (uint16_t)
                                           tagConfigSymple.sendToServerCount++;   //将发送计数放入报警有效位中，借用报警有效信息低字节
    if (tagConfigSymple.sendToServerCount == 0)
    {
        tagConfigSymple.sendToServerCount++;
    }
    m_test = BerthPo_SendAlarmPackage(frameType);
    return m_test;
}

void BerthPo_JudgeChangeOfModule()    //车辆状态反转判断
{
    uint8_t m_sendCount =
        3;                                                //发送次数 默认发送3次
    enableInterrupts();
    //发送消息前开中断
    if (BerthPo_BerthStateSwithProcess() == STATE_REVERSAL)
    {
        tagConfigSymple.debugInfoFlag = 0;
        //printf("car state reversal...\r\n");
        while(m_sendCount > 0)
        {
            if(BerthPo_SendCarStatus(0x01) == 0)   //如果不成功
            {
                tagConfigSymple.randomDelay = (((uint16_t)
                                                sensorRm3100.rm3100CurrentData.EMDataCurrent_z) + ((uint16_t)
                                                        sensorRm3100.rm3100CurrentData.EMDataCurrent_z) + ((uint16_t)
                                                                sensorRm3100.rm3100CurrentData.EMDataCurrent_z)) %
                                              256; //如果不成功，重新产生一个随机时间
            }
            tagConfigSymple.sendNodeCount =
                0;                                  //发送数据成功,重新计算发送间隔
            m_sendCount--;                          //发送失败重复3次
            enableInterrupts();                     //开中断,关中断动作在睡眠被唤醒后执行
            BSP_RTCAlarmDelayMs(2000 + tagConfigSymple.randomDelay);   //rtc睡眠时间
            BSP_RtcDeepSleep();                     //进入睡眠
        }
    }
    else                                                        //如果状态未翻转，则启动心跳检查程序
    {
        //printf("car state not reversal...\r\n");
        if ((++tagConfigSymple.sendNodeCount) >=
            tagConfigSymple.sendPackCount)       //发包控制，达到心跳发包时间
        {
            if(BerthPo_SendCarStatus(0x02) == 0)   //如果不成功
            {
                tagConfigSymple.randomDelay = (((uint16_t)
                                                sensorRm3100.rm3100CurrentData.EMDataCurrent_z) + ((uint16_t)
                                                        sensorRm3100.rm3100CurrentData.EMDataCurrent_z) + ((uint16_t)
                                                                sensorRm3100.rm3100CurrentData.EMDataCurrent_z)) %
                                              256; //如果不成功，重新产生一个随机时间
            }
            enableInterrupts();                              //发送消息前开中断
            tagConfigSymple.sendNodeCount =
                0;                         //发送数据成功,重新计算发送间隔
        }
    }
}
void  BerthPo_AlarmJude()    //地磁变动才发数据
{
    if(controlConfig.paramConfig.getEMBottom_RFFlag ==
       0x01)   //当地磁检测开关为打开时（=0x01）才检测地磁，其他值则直接跳过
    {
        BerthPo_GetRM3100Data();           //取地磁数据
        BerthPo_EMDealGeomagneticValue();
        BerthPo_JudgeChangeOfModule();     //车辆状态反转判断
    }
}



