
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "BerthPo_SX1280_ParkStateJudge.h"
#include "BerthPo_SX1280_RM3100.h"
#include "Drivers_RM3100.h"
#include "BerthPo_SX1280_Common.h"
#include "BerthPo_SX1280_Protocol.h"
#include "Drivers_Led.h"
#include "Sleep.h"
#include "Crc.h"
#include "BerthPo_SX1280_CompileFiles.h"
#include "Drivers_SX1280.h"
/***************�����ⲿ����******************/
extern SBERTHPO_PARK_STATUS parkStatus;
extern SCONTROL_CONFIG  controlConfig;
extern SSENSOR_RM3100 sensorRm3100;
extern LED_GPIO_TypeDef LED_GPIO;
extern SCONTROL_SYMPLE tagConfigSymple;
/*
* ��ʼ��״̬�ж��������
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
* ��λ״̬�л�
*/
uint8_t BerthPo_BerthStateSwithProcess()
{
    uint8_t ret = 0;
    static uint8_t m_init = 0xee;
    static BERTH_STATE m_lastState =
        BERTHPO_PARK_STATE_NULL;             //��ʷ��λ״̬
    BERTH_STATE currentState =
        g_parkState;                      //��ǰ��λ״̬
    SRM3100_SAMPLE_DATA
    rm3100SampleData;    //��ʱ���������ڻ����������е��м�ֵ
    //��ʼ��״̬�ж��������
    InputInfo input = BerthPo_InitBerthStateInput();
    // ��������ֵ
    BerthPo_CalculateMeagneticFeature(&input, &rm3100SampleData);
    //���ε��û���������־��һʱ�������ػ�������
    if (1 == sensorRm3100.clearBottomBufferFlag || m_init == 0xee)
    {
        BerthPo_InitStatisSet(&sensorRm3100, &(sensorRm3100.rm3100MagneticBase));
        BerthPo_InitStatisSet(&sensorRm3100, &(sensorRm3100.rm3100MagneticDensity));
        memset(&S_gNoParkData, 0, sizeof(SRM3100_EMData));
        memset(&(sensorRm3100.rm3100MagneticBaseLower), 0,
               sizeof(SRM3100_MAGNETIC));
        m_init = 0;
    }
    // �˴����ö�̬���׺������Զ�̬���׵������ܽ��д���
    BerthPo_BottomvalAdjustProcess();
    //��ǰ��λ״̬
    switch (currentState)
    {
        case BERTHPO_PARK_STATE_NULL://�޳�

            //���//��ǰ��λ״̬����ʷ��λ״̬����������Ҫ�����¼�������е�����
            if (BERTHPO_PARK_STATE_HAVE == m_lastState)
            {
                BerthPo_InitStatisSet(&sensorRm3100, &
                                      (sensorRm3100.rm3100MagneticBase));      // �޳��ų�ǿ��(����ų�+�����ų�)
                BerthPo_InitStatisSet(&sensorRm3100, &
                                      (sensorRm3100.rm3100MagneticDensity));   // �����Ÿ���_�г�״̬��ʹ��

                sensorRm3100.rm3100MagneticBaseLower.val = input.diffOfRM;
                sensorRm3100.rm3100MagneticBaseLower.x = input.x;
                sensorRm3100.rm3100MagneticBaseLower.y = input.y;
                sensorRm3100.rm3100MagneticBaseLower.z = input.z;
            }

            // ͳ�Ƶ�ǰ���ݣ����ڼ���
            BerthPo_StatisProcess(&sensorRm3100, &rm3100SampleData,
                                  &(sensorRm3100.rm3100MagneticBase), 1);

            // ������ӽ����׵��޳�ͳ��ֵ
            if (1 == sensorRm3100.rm3100MagneticBase.done)
            {
                // ���Ե�һ��ͳ��ֵ ��ֹ�ʹų�����ʼֵƫС�����
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

            // δ���ͳ��ʱ ������ӽ����׵��޳��ɼ�ֵ
            if (BERTHPO_PARK_STATE_NULL ==
                sensorRm3100.rm3100MagneticBaseLower.state &&
                sensorRm3100.rm3100MagneticBaseLower.val > input.diffOfRM)
            {
                sensorRm3100.rm3100MagneticBaseLower.val = input.diffOfRM;
                sensorRm3100.rm3100MagneticBaseLower.x = input.x;
                sensorRm3100.rm3100MagneticBaseLower.y = input.y;
                sensorRm3100.rm3100MagneticBaseLower.z = input.z;
            }

            // ���������״̬�ı䣬����Ҫ����������־
            if (BerthPo_ChangeConfirm(&input) == STATE_REVERSAL)
            {
                ret = STATE_REVERSAL;
				parkStatus.currentParkState = 1;
                g_parkState = BERTHPO_PARK_STATE_HAVE;
            }
            break;
        case BERTHPO_PARK_STATE_HAVE:    //�г�
            //�����ǰ��λ״̬����ʷ��λ״̬����������Ҫ�����¼�������е�����
            if (BERTHPO_PARK_STATE_NULL == m_lastState)
            {
                BerthPo_InitStatisSet(&sensorRm3100, &
                                      (sensorRm3100.rm3100MagneticDensity));// �����Ÿ���_�г�״̬��ʹ��
                memset(&(sensorRm3100.rm3100MagneticBaseLower), 0,
                       sizeof(SRM3100_MAGNETIC));
            }

            // ͳ�Ƶ�ǰ���ݣ����ڼ���
            BerthPo_StatisProcess(&sensorRm3100, &rm3100SampleData,
                                  &(sensorRm3100.rm3100MagneticDensity), 1);

            // ��¼���һ��ͣ��ͳ��״̬
            S_gParkState = sensorRm3100.rm3100MagneticDensity.state;

            // ���������״̬�ı䣬����Ҫ����������־
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
    m_lastState = (BERTH_STATE)currentState;           //����ǰ״̬���浽��ʷ״̬��
    sensorRm3100.clearBottomBufferFlag = 0;    //�屾�׻����־�ر�
    return ret;
}

/*
* ��ȡ�仯��ֵ
*/

/*******************************************************************************
* Function Name :uint8_t SetRm3100Base()
* Description   :����RM3100�شű���
* Author        : YJD
* Input         :thr1: ������ֵ       thr2: �����ֵ
* Output        :
* Other         :
* Date          :2018.05.08
*******************************************************************************/


void BerthPo_ChangeThresholdGet(uint8_t *thr1,
                                uint8_t *thr2, InputInfo* pInput)
{

    uint8_t offset = 0;
    //�г����޳�ʱ����ֵ�趨��һ��
    // �г�ʱ
    if (BERTHPO_PARK_STATE_HAVE == g_parkState)
    {
        offset = sensorRm3100.rm3100MagneticBase.val;
        offset = MIN_(offset, 10);

        if (MAGNETIC_STATE_NULL == sensorRm3100.rm3100MagneticDensity.state)
        {

            //������ֵ
            *thr1 = (uint8_t)(pInput->RM_thr * 8 / 10 + offset * offset * 0.0381 + offset *
                              0.1216 + 0.015);

            //�����ֵ
            *thr2 = pInput->diffOfRM;
        }
        else    //if (MAGNETIC_STATE_INIT == g_magnetic_density.state) {
        {

            //������ֵ
            *thr1 = (uint8_t)(MIN_(pInput->RM_thr * 8 / 10 + offset * offset * 0.0381 +
                                   offset
                                   * 0.1216 + 0.015, sensorRm3100.rm3100MagneticDensity.val * 8 / 10));
            *thr1 = MAX_(*thr1, 8);

            //�����ֵ
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

        //������ֵ
        *thr1 = (uint8_t)(pInput->RM_thr + MAX_(0,
                                                offset * offset * 0.0381 + offset * 0.1216 + 0.015 - 1.0f));

        //�����ֵ
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
* ���������
*/
void BerthPo_CalculateMeagneticFeature(InputInfo* pInput,
                                       PRM3100_SAMPLE_DATA pRm3100SampleData)
{
    memset(pRm3100SampleData, 0, sizeof(SRM3100_SAMPLE_DATA));
    pRm3100SampleData->x = pInput->x;
    pRm3100SampleData->y = pInput->y;
    pRm3100SampleData->z = pInput->z;
    pRm3100SampleData->diffOfRM = pInput->diffOfRM;
    // �Ƕȼ���

}


/*
* �仯�ж�
*/
uint8_t BerthPo_ChangeJudge(InputInfo* pInput)
{
    uint8_t ret = 0;
    uint8_t thr_absol = 0;  // ������ֵ
    uint8_t thr_change = 0; // �仯������ֵ
    int16_t range_RM = 0;
    int16_t range_abs_diff = 0;
    int16_t range_RM1 = 0;

    // ��ѯ��ֵ
    BerthPo_ChangeThresholdGet(&thr_absol, &thr_change,
                               pInput);

    // �г�ʱ
    if (BERTHPO_PARK_STATE_HAVE == g_parkState)
    {
        // range_RM ����
        if (MAGNETIC_STATE_NULL == sensorRm3100.rm3100MagneticDensity.state)
        {
            range_RM = 0;

        }
        else
        {

            // ��ǰֵ�г�ͳ�Ƶ�xyz���Բ�ֵ
            range_abs_diff = abs(sensorRm3100.rm3100MagneticDensity.x - pInput->x) +
                             \
                             abs(sensorRm3100.rm3100MagneticDensity.y - pInput->y) + \
                             abs(sensorRm3100.rm3100MagneticDensity.z - pInput->z);

            // range_RM �仯ֵ(ȡ�뱾�׵�ģ��ֵ���г�ͳ�Ƶ�xyz���Բ�ֵ�����ֵ)
            // ��С
            if (sensorRm3100.rm3100MagneticDensity.val > pInput->diffOfRM)
            {

                range_RM = (sensorRm3100.rm3100MagneticDensity.val - pInput->diffOfRM);
                range_RM = 0 - MAX_(range_RM, MIN_(range_abs_diff, range_RM * 2));

            }
            else
            {
                range_RM = (pInput->diffOfRM -
                            sensorRm3100.rm3100MagneticDensity.val); // ���ʹ�ñ仯�ж�
            }
        }

        // xyzֵ���䵽�޳�ʱͳ��ֵֵ����
        range_RM1 = abs(sensorRm3100.rm3100MagneticBase.x - pInput->x) + \
                    abs(sensorRm3100.rm3100MagneticBase.y - pInput->y) + \
                    abs(sensorRm3100.rm3100MagneticBase.z - pInput->z);

        /**********************************�г����޳��ж�**********************************/

        // ͨ�������ص����׸����ж��޳�
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

        // ͨ���Ŷ�ֵ�ı仯�����ж��޳�
        if ((0 - range_RM) > thr_change)
        {
            ret = 1;
        }

    }
    else
    {
        // ���
        if (sensorRm3100.rm3100MagneticBase.val < pInput->diffOfRM)
        {

            range_RM = (pInput->diffOfRM - sensorRm3100.rm3100MagneticBase.val);
        }
        else
        {
            range_RM = (sensorRm3100.rm3100MagneticBase.val - pInput->diffOfRM);
        }


        // ��ǰ����ֵ����һ�β���ֵ(������)�ľ��Բ�ֵ
        signed short diff_nopark = abs(S_gNoParkData.data_x - pInput->x) + \
                                   abs(S_gNoParkData.data_y - pInput->y) + \
                                   abs(S_gNoParkData.data_z - pInput->z);

        // xyzֵ���䵽��һ���г�ֵ����(����ͣ������ŵ���һֱ�޳������)
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

        // range_RM �仯ֵ(ȡ�뱾�׵�ģ��ֵ���޳�ʱͳ�Ƶ�xyz���Բ�ֵ�����ֵ)
        if (MAGNETIC_STATE_NULL != sensorRm3100.rm3100MagneticBase.state)
        {
            range_abs_diff = abs(sensorRm3100.rm3100MagneticBase.x - pInput->x) + \
                             abs(sensorRm3100.rm3100MagneticBase.y - pInput->y) + \
                             abs(sensorRm3100.rm3100MagneticBase.z - pInput->z);
        }
        range_RM = MAX_(range_RM, range_abs_diff);


        // range_RM1 �仯ֵ(ȡ�뱾���������ͳ��ֵ����ͳ��ֵʱ�ο���diff_nopark���ֵ)
        range_RM1 = abs(sensorRm3100.rm3100MagneticBaseLower.x - pInput->x) + \
                    abs(sensorRm3100.rm3100MagneticBaseLower.y - pInput->y) + \
                    abs(sensorRm3100.rm3100MagneticBaseLower.z - pInput->z);
        if (MAGNETIC_STATE_CONFIRM != sensorRm3100.rm3100MagneticBaseLower.state)
        {
            range_RM1 = MAX_(diff_nopark, range_RM1);
        }

        // ����ƫ��ֵ
        uint8_t bottomOffsetValue = BerthPo_GetBottomOffsetValue(&sensorRm3100);
        if (bottomOffsetValue == 0)
        {
            bottomOffsetValue = sensorRm3100.rm3100MagneticBase.val;
        }

        /**********************************�޳����г��ж�**********************************/
        // ͨ���뱾��ģ��ֵ�ж��г�
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

        // ͨ���뱾��(x, y, z)�仯�����ж��г�
        if (range_RM > thr_change && pInput->diffOfRM >= 5)
        {
            ret = 1;
        }

        // ͨ������(x, y, z)�仯Ȩֵ�ж��г�
        if (BerthPo_FixedVCheck() >= 3 && diff_nopark > pInput->RM_thr / 2)
        {
            ret = 1;
        }
    }


    return ret;

}


/*
* �仯ȷ��(״̬��ת)
*/
uint8_t BerthPo_ChangeConfirm(InputInfo* pInput)
{
    uint8_t ret = 0;

    if (BerthPo_ChangeJudge(pInput) == 1)
    {
        uint8_t i = 0;

        uint16_t sleepTime = 1024;    //˯��ʱ�䣬�ɳ�λ״̬ȷ����Ĭ��Ϊ����
        if (parkStatus.currentParkState == 0)  sleepTime =
                2048; //��λ״̬����ȷ���ٶȣ��޳�-->�г�������ȷ�ϣ��г�-->�޳�������ȷ��

        ret = STATE_REVERSAL;//Ĭ��״̬�����˷�ת

        parkStatus.fastGetParkCount = 0;
        for (i = 0; i < 3; i++)
        {
            Drivers_LedOff(LED_GPIO, 2);
            enableInterrupts();          //���ж�,���ж϶�����˯�߱����Ѻ�ִ��
            BSP_RtcGoSleep(sleepTime);   //rtc˯��ʱ��
            //BSP_RtcDeepSleep();          //����˯��
            BerthPo_GetRM3100Data();   //ȡ�ش�����
            BerthPo_EMDealGeomagneticValue();                //�����ش�����

            // ���³�ʼ��input
            InputInfo input_temp = BerthPo_InitBerthStateInput();
            if (BerthPo_ChangeJudge(&input_temp) ==
                0)  //������ټ���з����ж�������������μ�����
            {

                parkStatus.fastGetParkCount = 3;   //����һ�ο��ټ��
                ret = 0;    //ȡ��״̬��ת��־�������˳����ת̬

                break;
            }
        }
        Drivers_LedOff(LED_GPIO, 2);

        // ��ʱ�����޳�ʱ����
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
        m_sendPackage.packageHeader.deviceNumber[i] = controlConfig.nodeConfig.idNub[i];
    }
    m_sendPackage.packageHeader.frameType = frameType;
    m_sendPackage.txPayLoad[0] =
        controlConfig.paramConfig.alarmValid;        //��Ч״̬
    m_sendPackage.txPayLoad[1] = controlConfig.paramConfig.alarmValid >>
                                 8;     //��Ч״̬
    m_sendPackage.txPayLoad[2] =
        controlConfig.paramConfig.alarmStatus;      //��λ״̬
    m_sendPackage.txPayLoad[3] = controlConfig.paramConfig.alarmStatus >>
                                 8;   //��λ״̬

    m_sendPackage.txPayLoad[4] = ((uint8_t)(
                                      sensorRm3100.rm3100CurrentData.EMDataCurrent_x));   //xʵʱֵ
    m_sendPackage.txPayLoad[5] = ((uint8_t)(
                                      sensorRm3100.rm3100CurrentData.EMDataCurrent_y));   //yʵʱֵ
    m_sendPackage.txPayLoad[6] = ((uint8_t)(
                                      sensorRm3100.rm3100CurrentData.EMDataCurrent_z));   //zʵʱֵ
    m_sendPackage.txPayLoad[7] = ((uint8_t)((sensorRm3100.diffOfRM)));   //ģ��
    switch (m_sendPackage.txPayLoad[2] >> 6)
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
                                                  controlConfig.paramConfig.EMDataBottom_x));     //x�̶�����ֵ
            else
                m_sendPackage.txPayLoad[8] = ((uint8_t)(
                                                  sensorRm3100.rm3100DynamicBoottom.EMDataBottom_x));                        //x��̬����ֵ
            break;
        }
        case 2:
        {
            if (tagConfigSymple.outBottomFlag == 0)
                m_sendPackage.txPayLoad[8] = ((uint8_t)(
                                                  controlConfig.paramConfig.EMDataBottom_y));     //y�̶�����ֵ
            else
                m_sendPackage.txPayLoad[8] = ((uint8_t)(
                                                  sensorRm3100.rm3100DynamicBoottom.EMDataBottom_y));   //y��̬����ֵ
            break;
        }
        case 3:
        {
            if (tagConfigSymple.outBottomFlag == 0)
                m_sendPackage.txPayLoad[8] = ((uint8_t)(
                                                  controlConfig.paramConfig.EMDataBottom_z));//z�̶�����ֵ
            else
                m_sendPackage.txPayLoad[8] = ((uint8_t)(
                                                  sensorRm3100.rm3100DynamicBoottom.EMDataBottom_z));//z��̬����ֵ
            break;
        }
    }
    m_sendPackage.packageHeader.frameLens = 9 + 2;
    uint8_t frameTotalLens = 12 + m_sendPackage.packageHeader.frameLens;
    uint8_t m_sendBuf[9 + 2 + 12] = {0};
    memcpy(m_sendBuf, (uint8_t *) & (m_sendPackage.packageHeader),
           sizeof(SFRAME_HEAD_TypeDef));
    memcpy(m_sendBuf + sizeof(SFRAME_HEAD_TypeDef),
           (uint8_t *)m_sendPackage.txPayLoad, 9);
    Calculate_Crc16((unsigned char *)m_sendBuf, frameTotalLens - 2);
    Drivers_LedOn(LED_GPIO, 2);
    uint8_t m_trySendTimes = SX1280_Operation.SX1280_GetMaxTryTimes();
    while(m_trySendTimes--)
    {
        SX1280_Operation.SX1280_SendBuff((uint8_t*)&m_sendBuf,
                                         frameTotalLens);
        m_trySendTimes--;
        if(SX1280_Operation.SX1280_GetTxDoneFlag() == 0x5A)
        {
            SX1280_Operation.SX1280_SetTxDoneFlag(0);
            return 1;
        }
        DelayMs(1);
    }
    return 0;
}
uint8_t BerthPo_GetVccInfo()
{
    uint8_t m_test;
    uint32_t m_vcc;
    m_test = tagConfigSymple.debugInfoFlag << 6;
    switch(tagConfigSymple.debugInfoFlag)//������Ϣ��־
    {
        case 0:   //��ص�ѹ
        {
            if(tagConfigSymple.batteryCount >= 0)
            {
                m_vcc = Drivers_GetBattryVCC();   //ȡ�õ�ص�ѹ
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
                    m_vcc = 63;                       //������ʾ��ѹ���ֵ
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
        case 1:   //������Ϣ-�޸�����ʱ��-���׵�������
        {
            m_test = m_test | (sensorRm3100.getTimeBottomCount % 64);
            break;
        }
        case 2://��ײ����
        {
            m_test = m_test | (tagConfigSymple.batteryCheckCount % 64);
            break;
        }
        case 3://�����汾
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
    controlConfig.paramConfig.alarmStatus & 0x00; //��ʼ������λ״̬
    if(parkStatus.currentParkState == 1)
    {
        controlConfig.paramConfig.alarmStatus =
            controlConfig.paramConfig.alarmStatus | 0x0400;//�г�
    }
    else
    {
        controlConfig.paramConfig.alarmStatus =
            controlConfig.paramConfig.alarmStatus | 0x0800;//�޳�
    }

    if (controlConfig.paramConfig.fastRouseAlarm > 20)
    {
        controlConfig.paramConfig.fastRouseAlarm = 0;
        controlConfig.paramConfig.alarmStatus =
            controlConfig.paramConfig.alarmStatus | 0x4000;//Ƶ�����ѱ���
    }

    m_InfData = BerthPo_GetVccInfo();

    controlConfig.paramConfig.alarmStatus =
        controlConfig.paramConfig.alarmStatus | (uint16_t)
        m_InfData; //��������Ϣ���뱨����Ϣ�У����ñ�����Ϣ���ֽ�

    controlConfig.paramConfig.alarmValid = 0x4c00 |
                                           (uint16_t)
                                           tagConfigSymple.sendToServerCount++;   //�����ͼ������뱨����Чλ�У����ñ�����Ч��Ϣ���ֽ�
    if (tagConfigSymple.sendToServerCount == 0)
    {
        tagConfigSymple.sendToServerCount++;
    }
    m_test = BerthPo_SendAlarmPackage(frameType);
    return m_test;
}

void BerthPo_JudgeChangeOfModule()    //����״̬��ת�ж�
{

    uint8_t m_sendCount =
        3;                                                //���ʹ��� Ĭ�Ϸ���3��
    enableInterrupts();
    //������Ϣǰ���ж�
    if (BerthPo_BerthStateSwithProcess() == STATE_REVERSAL)
    {
        tagConfigSymple.debugInfoFlag = 0;
        printf("car state reversal...\r\n");
        while(m_sendCount > 0)
        {
            Led_Operation.LedOn(LED_GPIO, 1);
            BerthPo_SendCarStatus(0x01);
            tagConfigSymple.sendNodeCount =
                0;                                  //�������ݳɹ�,���¼��㷢�ͼ��
            m_sendCount--;                          //����ʧ���ظ�3��
            enableInterrupts();                                 //���ж�,���ж϶�����˯�߱����Ѻ�ִ��
            //BSP_RtcGoSleep(2048);                               //rtc˯��ʱ��
            //BSP_RtcDeepSleep();                                 //����˯��
        }
    }
    else                                                        //���״̬δ��ת������������������
    {
        printf("car state not reversal...\r\n");
        if ((++tagConfigSymple.sendNodeCount) >=
            tagConfigSymple.sendPackCount)       //�������ƣ��ﵽ��������ʱ��
        {
            Led_Operation.LedOff(LED_GPIO, 1);
            enableInterrupts();                                 //������Ϣǰ���ж�
            BerthPo_SendCarStatus(0x02);                     //����״̬
            tagConfigSymple.sendNodeCount =
                0;                         //�������ݳɹ�,���¼��㷢�ͼ��
            //NB_BC95.nb_recv_data.receive_status |= 0x80;
            //NB_BC95.nb_recv_data.receive_cnt = 0;
        }
    }
}
void  BerthPo_AlarmJude()    //�شű䶯�ŷ�����
{
    if(controlConfig.paramConfig.getEMBottom_RFFlag ==
       0x01)   //���شż�⿪��Ϊ��ʱ��=0x01���ż��شţ�����ֵ��ֱ������
    {
        BerthPo_GetRM3100Data();           //ȡ�ش�����
        BerthPo_EMDealGeomagneticValue();
        BerthPo_JudgeChangeOfModule();     //����״̬��ת�ж�
    }
}


