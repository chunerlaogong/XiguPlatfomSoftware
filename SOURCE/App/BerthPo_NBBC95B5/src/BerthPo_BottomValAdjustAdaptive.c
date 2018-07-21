

#include "BerthPo_BottomValAdjustAdaptive.h"
#include "BerthPo_RM3100.h"
#include "BerthPo_StatisMagneticDensity.h"
#include "BerthPo_Common.h"
#include "BerthPo_CompileFiles.h"
/*********************�����ⲿ����*****************/

extern SBERTHPO_PARK_STATUS parkStatus;
extern SCONTROL_CONFIG	controlConfig;
extern SSENSOR_RM3100 sensorRm3100;

/*********************�����������*************/
void BerthPo_GetDynamicBottom()   //ȡ��ʵʱ��̬����
{

    //��̬���׼ƴμ�һ��ͨ��inf��Ϣ���������Ϸ�
    if (sensorRm3100.getTimeBottomCount > 50)
        sensorRm3100.getTimeBottomCount = 1;
    else
        sensorRm3100.getTimeBottomCount++;

    //�����µĶ�̬����ֵ����ȫ�ֱ������ڱ��׼���ʱʹ��
    sensorRm3100.rm3100DynamicBoottom.EMDataBottom_x =
        S_realTimeBottom.data_x;   //x��̬����ֵ
    sensorRm3100.rm3100DynamicBoottom.EMDataBottom_y =
        S_realTimeBottom.data_y;   //y��̬����ֵ
    sensorRm3100.rm3100DynamicBoottom.EMDataBottom_z =
        S_realTimeBottom.data_z;   //z��̬����ֵ
}
/*
* ��ȡʵʱ����ֵ
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
* �ж��Ƿ����㱾��Լ������
*/
uint8_t BerthPo_SatisfyBottomConstraintCondition(PRM3100_MAGNETIC
        pRm3100Magnetic)
{

    short xBottom = controlConfig.paramConfig.EMDataBottom_x;
    short yBottom = controlConfig.paramConfig.EMDataBottom_y;
    short zBottom = controlConfig.paramConfig.EMDataBottom_z;
    short x_vary = (pRm3100Magnetic->x - xBottom);   //Xǿ�ȱ仯ʸ����,��λ
    short y_vary = (pRm3100Magnetic->y - yBottom);   //yǿ�ȱ仯ʸ����,��λ
    short z_vary = (pRm3100Magnetic->z - zBottom);   //zǿ�ȱ仯ʸ����,��λ

    short diffOfRM = (short)(sqrtf(x_vary * x_vary + y_vary * y_vary + 2 * z_vary *
                                   z_vary));

    x_vary = (pRm3100Magnetic->x -
              S_realTimeBottom.data_x);   //Xǿ�ȱ仯ʸ����,��λ
    y_vary = (pRm3100Magnetic->y -
              S_realTimeBottom.data_y);   //yǿ�ȱ仯ʸ����,��λ
    z_vary = (pRm3100Magnetic->z -
              S_realTimeBottom.data_z);   //zǿ�ȱ仯ʸ����,��λ

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
* ����ֵ����Ӧ����
*/
uint8_t BerthPo_BottomvalAdjustProcess()
{
    static uint8_t mInit = 0xee;
    static BERTH_STATE m_lastParkState = BERTHPO_PARK_STATE_NULL;
    uint8_t m_currentParkStatus = parkStatus.currentParkState;
    SRM3100_SAMPLE_DATA m_sampleData;

    // У׼���׻�������� ��ʼ��ͳ����Ϣ
    if (1 == sensorRm3100.clearBottomBufferFlag || mInit == 0xee)
    {
        BerthPo_InitBottomTimesStatisSet((&sensorRm3100), &S_bottomTimes);
        BerthPo_InitBottomCnsTimeStatisSet(&(sensorRm3100), &S_bottomLongTime);
        BerthPo_GetRealTimeBottom();
        mInit = 0;
    }
    //ȡ���޳��ų�ǿ��(����ų�+�����ų�)
    m_sampleData.x = sensorRm3100.rm3100MagneticBase.x;
    m_sampleData.y = sensorRm3100.rm3100MagneticBase.y;
    m_sampleData.z = sensorRm3100.rm3100MagneticBase.z;
    m_sampleData.diffOfRM = sensorRm3100.rm3100MagneticBase.val;

    //�г����޳�״̬�ֱ��ж�
    switch (m_currentParkStatus)
    {
        case BERTHPO_PARK_STATE_NULL:   //�޳�
            // ͨ������޳����ݵ�������
            if (1 == S_noParkStartstatisFlag)
            {
                if (1 == sensorRm3100.rm3100MagneticBase.done)
                {
                    if (BerthPo_BottomTimesAdjustStatisProcess(&m_sampleData,
                            &(sensorRm3100), &S_bottomTimes) == 1)
                    {
                        // �ж��Ƿ����㱾�׵�������
                        if (BerthPo_SatisfyBottomConstraintCondition(&S_bottomTimes) == 1)
                        {
                            // ��������
                            S_realTimeBottom.data_x = S_bottomTimes.x;   //x��̬����ֵ
                            S_realTimeBottom.data_y = S_bottomTimes.y;   //y��̬����ֵ
                            S_realTimeBottom.data_z = S_bottomTimes.z;   //z��̬����ֵ
                            BerthPo_GetDynamicBottom();                  //ȡ��ʵʱ��̬����
                        }
                        else
                        {
                            //������������̬���׿ɵ���Χ������´οɵ�����������˱�־
                            sensorRm3100.getTimeBottomCount = 63;
                        }
                    }
                    S_noParkStartstatisFlag = 0;
                }
            }
            // ͨ����ʱ���޳�����ͳ�Ʊ���
            if (1 == sensorRm3100.rm3100MagneticBase.done)
            {
                if (BerthPo_BottomCnsTimeAdjustStatisProcess(&m_sampleData,
                        &(sensorRm3100), &S_bottomLongTime) == 1)
                {

                    // �жϺ�ĳһ���޳������Ƿ�һ��
                    if (BerthPo_MatchTimesAndCnsTimeStatis(&(sensorRm3100),
                                                           &S_bottomLongTime) == 1)
                    {
                        // �ж��Ƿ����㱾�׵�������
                        if (BerthPo_SatisfyBottomConstraintCondition(&S_bottomLongTime) == 1)
                        {
                            // ��������
                            S_realTimeBottom.data_x = S_bottomLongTime.x;  //x��̬����ֵ
                            S_realTimeBottom.data_y = S_bottomLongTime.y;  //y��̬����ֵ
                            S_realTimeBottom.data_z = S_bottomLongTime.z;  //z��̬����ֵ

                            BerthPo_GetDynamicBottom();   //ȡ��ʵʱ��̬����
                        }
                        else
                        {
                            // ������������̬���׿ɵ���Χ������´οɵ�����������˱�־
                            sensorRm3100.getTimeBottomCount = 63;
                        }
                        // ��ճ�ʱ���޳�ͳ��
                        BerthPo_InitBottomCnsTimeStatisSet(&(sensorRm3100), &S_bottomLongTime);
                    }
                }
            }

            break;
        case BERTHPO_PARK_STATE_HAVE:   //�г�

            // ��ʼ��
            if (BERTHPO_PARK_STATE_NULL == m_lastParkState)
                S_noParkStartstatisFlag = 0;

            if (MAGNETIC_STATE_CONFIRM == sensorRm3100.rm3100MagneticDensity.state)
            {
                S_noParkStartstatisFlag = 1;
                // ��ճ�ʱ���޳�ͳ��
                BerthPo_InitBottomCnsTimeStatisSet(&(sensorRm3100), &S_bottomLongTime);
            }
            break;
        default:
            break;
    }
    m_lastParkState = (BERTH_STATE)m_currentParkStatus;
    return 1;

}









