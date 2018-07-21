#include "BerthPo_SX1280_Common.h"
#include "Delay.h"
#include "Drivers_LED.h"
#include "Drivers_NFC.h"
#include <math.h>
/******************�����ⲿ����******************/
extern SBERTHPO_PARK_STATUS parkStatus;
extern SCONTROL_CONFIG  controlConfig;
extern SNET_MUTUAL_INFO  netMutualInfo;
extern SSENSOR_RM3100 sensorRm3100;
extern SRM3100_GPIO_TypeDef RM3100_GPIO;
extern LED_GPIO_TypeDef LED_GPIO;
extern SCONTROL_SYMPLE tagConfigSymple;

/*******************************************************************************
* Function Name : void BerthPo_WriteParamToFlash(SPARAM_NB_TAG paramNBTag)
* Description   : ϵͳ������Ҫ����ز�����ʼ��
* Author        : YJD
* Input         : NULL
* Output        : NULL
* Other         : NULL
* Date          : 2018.05.11
*******************************************************************************/

void BerthPo_WriteParamToFlash()
{
    netMutualInfo.workStatus = controlConfig.workStatus;
    netMutualInfo.tagID[0] = controlConfig.nodeConfig.idNub[0];  //����ID
    netMutualInfo.tagID[1] = controlConfig.nodeConfig.idNub[1];
    netMutualInfo.tagID[2] = controlConfig.nodeConfig.idNub[2];
	netMutualInfo.tagID[3] = controlConfig.nodeConfig.idNub[3];  //����ID
    netMutualInfo.tagID[4] = controlConfig.nodeConfig.idNub[4];
    netMutualInfo.tagID[5] = controlConfig.nodeConfig.idNub[5];
    netMutualInfo.ledFlag  = controlConfig.nodeConfig.ledFlag;
    netMutualInfo.wdtInterval = controlConfig.paramConfig.wdtInterval;
    netMutualInfo.heartbeatInterval =
        controlConfig.paramConfig.heartbeatInterval;
    netMutualInfo.alarmValid = controlConfig.paramConfig.alarmValid;
    netMutualInfo.getEMBottom_RFFlag =
        controlConfig.paramConfig.getEMBottom_RFFlag;
    netMutualInfo.getEMBottom_RFModThreshold =
        controlConfig.paramConfig.getEMBottom_RFModThreshold;
    netMutualInfo.getEMBottom_RFAngleThreshold =
        controlConfig.paramConfig.getEMBottom_RFAngleThreshold;
    netMutualInfo.EMDataBottom_x = sensorRm3100.rm3100DynamicBoottom.EMDataBottom_x;
    netMutualInfo.EMDataBottom_y = sensorRm3100.rm3100DynamicBoottom.EMDataBottom_y;
    netMutualInfo.EMDataBottom_z = sensorRm3100.rm3100DynamicBoottom.EMDataBottom_z;
    netMutualInfo.fastRouse = controlConfig.paramConfig.fastRouse;
    netMutualInfo.fastRouseAlarm = controlConfig.paramConfig.fastRouseAlarm;
    BerthPo_WriteParamToEEPROM((uint8_t *) & (netMutualInfo), sizeof(SNET_MUTUAL_INFO),
                               48);
}
/*******************************************************************************
* Function Name : void ReadTagParam(SPARAM_NB_TAG paramNBTag)
* Description   : ϵͳ������Ҫ����ز�����ʼ��
* Author        : YJD
* Input         : NULL
* Output        : NULL
* Other         : NULL
* Date          : 2018.05.11
*******************************************************************************/

void BerthPo_ReadParamFromFlash()
{
    BerthPo_ReadParamFromEEPROM((uint8_t*) & (netMutualInfo), sizeof(SNET_MUTUAL_INFO),
                                48);
    controlConfig.workStatus =  netMutualInfo.workStatus;
    controlConfig.nodeConfig.idNub[0] =  netMutualInfo.tagID[0];
    controlConfig.nodeConfig.idNub[1] =  netMutualInfo.tagID[1];
    controlConfig.nodeConfig.idNub[2] =  netMutualInfo.tagID[2];
    controlConfig.nodeConfig.ledFlag =  netMutualInfo.ledFlag;
    controlConfig.paramConfig.wdtInterval =  netMutualInfo.wdtInterval;
    controlConfig.paramConfig.heartbeatInterval =
        netMutualInfo.heartbeatInterval;
    controlConfig.paramConfig.alarmValid =  netMutualInfo.alarmValid;
    controlConfig.paramConfig.getEMBottom_RFFlag =
        netMutualInfo.getEMBottom_RFFlag;
    controlConfig.paramConfig.getEMBottom_RFModThreshold =
        netMutualInfo.getEMBottom_RFModThreshold;
    controlConfig.paramConfig.getEMBottom_RFAngleThreshold =
        netMutualInfo.getEMBottom_RFAngleThreshold;
    sensorRm3100.rm3100DynamicBoottom.EMDataBottom_x =  netMutualInfo.EMDataBottom_x;
    sensorRm3100.rm3100DynamicBoottom.EMDataBottom_y =  netMutualInfo.EMDataBottom_y;
    sensorRm3100.rm3100DynamicBoottom.EMDataBottom_z =  netMutualInfo.EMDataBottom_z;
    controlConfig.paramConfig.fastRouse =  netMutualInfo.fastRouse;
    controlConfig.paramConfig.fastRouseAlarm =  netMutualInfo.fastRouseAlarm;
   tagConfigSymple.sendPackCount = (controlConfig.paramConfig.heartbeatInterval *
                              60) / controlConfig.paramConfig.wdtInterval;
    nop();
}
/*******************************************************************************
* Function Name : void BerthPo_ProductTestMode()
* Description   : ϵͳ������Ҫ����ز�����ʼ��
* Author        : YJD
* Input         : NULL
* Output        : NULL
* Other         : NULL
* Date          : 2018.05.11
*******************************************************************************/

void BerthPo_ProductTestMode()
{
    
}

void BerthPo_MakeHead(PFRAME_DATA_TypeDef pframeData)
{

	pframeData->packageHeader.frameHeader[0] |= 0xFE;
	pframeData->packageHeader.frameHeader[1] |= 0x03;
	pframeData->packageHeader.controlType = 0x00;
	pframeData->packageHeader.deviceType = 0x06;   //��Ƶ��λ��
}
/*
 * @name:   Hex_to_ASCII
 * @author: COPY
 * @date:   2017-11-23
 * @brief:
 */
void Hex_to_ASCII(char *pHex, char *pASCII, uint8_t len)
{
    uint8_t i = 0, j = 0;
    char str[100] = {0};
    for(i = 0; i < len; i++)
    {
        str[j] = (*pHex) >> 4;
        str[j + 1] = (*pHex) & 0x0f;
        pHex++;
        j += 2;
    }
    for(i = 0; i < j; i++)
    {
        sprintf((char *)pASCII, "%X", str[i]);
        pASCII++;
    }
}

/*******************************************************************************
* Function Name :void BerthPo_GetRM3100Data(uint8_t* pReadData )
* Description   :��ȡ�ش�����
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          :2018.06.29
*******************************************************************************/

void BerthPo_GetRM3100Data()
{

    uint8_t m_tempBuf[9] = {0};
    long m_reading = 0;
    RM3100_Operation.RM3100_PowerOn(&(RM3100_GPIO));
    DelayMs(100);                       //�ϵ���ʱ�ȴ�R3100׼����С��30us�������ݴ��󣬵���ֵ200us
    enableInterrupts();   //���ж�,���ж϶�����˯�߱����Ѻ�ִ��
    BSP_RtcGoSleep(30);   //rtc˯��ʱ��,15ms
    //  BSP_RtcDeepSleep();
    //RM3100_Operation.RM3100_McuRdRyOff(RM3100_GPIO);        //�صش�оƬ�ж�����
    //RM3100_Operation.RM3100_McuRdRyOn(RM3100_GPIO);         //���ش�оƬ�ж�����
    RM3100_Operation.RM3100_SetMeaureMode(&(RM3100_GPIO), 1);  //���õ�һ����ģʽ*/
    DelayMs(100);
    RM3100_Operation.RM3100_ReadBytes(0x24, &(RM3100_GPIO.gpioI2cStructer),
                                      m_tempBuf, 9);
    RM3100_Operation.RM3100_PowerOff(&(RM3100_GPIO));               //�ر�RM3100���ж˿ڽ���

    m_reading  = *m_tempBuf << 8;
    m_reading |= *(m_tempBuf + 1);
    m_reading  =  m_reading << 8;
    m_reading |= *(m_tempBuf + 2);
    sensorRm3100.rm3100CurrentData.EMDataCurrent_x = (signed short)m_reading /
            MAGNETIC_FACTOR;  //(m_reading >>8); //273.0;

    m_reading  = *(m_tempBuf + 3) << 8;
    m_reading |= *(m_tempBuf + 4);
    m_reading  =  m_reading << 8;
    m_reading |= *(m_tempBuf + 5);
    sensorRm3100.rm3100CurrentData.EMDataCurrent_y = (signed short)m_reading /
            MAGNETIC_FACTOR; // (m_reading >>8); //273.0;

    m_reading  = *(m_tempBuf + 6) << 8;
    m_reading |= *(m_tempBuf + 7);
    m_reading  =  m_reading << 8;
    m_reading |= *(m_tempBuf + 8);
    sensorRm3100.rm3100CurrentData.EMDataCurrent_z = (signed short)m_reading /
            MAGNETIC_FACTOR; // (m_reading >>8); //273.0;
    nop();
}

/*******************************************************************************
* Function Name :uint8_t SetRm3100Base()
* Description   :����RM3100�شű���
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          :2018.05.08
*******************************************************************************/
uint8_t BerthPo_SetRm3100Base()
{
    uint8_t i = 0, j = 0;
    unsigned long mRms = 0;
    int32_t data_x = 0, data_y = 0, data_z = 0,
            dataCount = 0;   //���Դ洢�ų���ǰֵ�ۼ�ֵ
    int16_t bottomTemp_x = 0, bottomTemp_y = 0, bottomTemp_z = 0;
    controlConfig.paramConfig.getEMBottomFlag = 0;
    for(i = 0 ; i < 4; i++)
    {
        BerthPo_GetRM3100Data();            //ȡ�ش�����
        data_x += sensorRm3100.rm3100CurrentData.EMDataCurrent_x;
        data_y += sensorRm3100.rm3100CurrentData.EMDataCurrent_y;
        data_z += sensorRm3100.rm3100CurrentData.EMDataCurrent_z;
        dataCount++;
        DelayMs(10);
    }
    //xyzֵ�������룬4�β���ƽ��������+-2,2/4=0.5
    data_x = (data_x > 0) ? data_x + 2 : data_x - 2;
    data_y = (data_y > 0) ? data_y + 2 : data_y - 2;
    data_z = (data_z > 0) ? data_z + 2 : data_z - 2;

    bottomTemp_x = data_x / dataCount;
    bottomTemp_y = data_y / dataCount;
    bottomTemp_z = data_z / dataCount;

    //����شű��ף���ʱ�仯Ӧ���������㣬������Ϊ��ʼ������Ӧ�÷��ظ��ϼ��豸������Ϣ
    for(i = 0; i < 10; i++)
    {
        DelayMs(10);
        BerthPo_GetRM3100Data();   //ȡ�ش�����
        mRms = (sensorRm3100.rm3100CurrentData.EMDataCurrent_x - bottomTemp_x)
               * (sensorRm3100.rm3100CurrentData.EMDataCurrent_x -
                  bottomTemp_x)
               + (sensorRm3100.rm3100CurrentData.EMDataCurrent_y - bottomTemp_y) *
               (sensorRm3100.rm3100CurrentData.EMDataCurrent_y -
                bottomTemp_y)
               + (sensorRm3100.rm3100CurrentData.EMDataCurrent_z - bottomTemp_z) *
               (sensorRm3100.rm3100CurrentData.EMDataCurrent_z -
                bottomTemp_z);
        nop();
        if(mRms < 10)//δ�����ţ����ټ�����            δ�����ţ����ټ����� ԭ����4 ��Ϊ9 by HL
        {
            controlConfig.paramConfig.getEMBottomFlag = 1;

            //�̶�����ֵ
            controlConfig.paramConfig.EMDataBottom_x =
                bottomTemp_x;          //MagneticFactor;
            controlConfig.paramConfig.EMDataBottom_y =
                bottomTemp_y;          //MagneticFactor;
            controlConfig.paramConfig.EMDataBottom_z =
                bottomTemp_z;          //MagneticFactor;

            //����ǰ��ȡ�ĺϷ�����ֵ��Ϊ��̬����ֵ��ʼֵ
            sensorRm3100.rm3100DynamicBoottom.EMDataBottom_x =
                controlConfig.paramConfig.EMDataBottom_x;
            sensorRm3100.rm3100DynamicBoottom.EMDataBottom_y =
                controlConfig.paramConfig.EMDataBottom_y;
            sensorRm3100.rm3100DynamicBoottom.EMDataBottom_z =
                controlConfig.paramConfig.EMDataBottom_z;
            sensorRm3100.getTimeBottomCount = 0;

            sensorRm3100.clearBottomBufferFlag =
                1;                     //��Ҫ��ʼ����������׻������ڵ�����
            return 1;
        }
    }
    return 0;
}
void BerthPo_EMDealGeomagneticValue()   //����ش�����
{

    int32_t m_diffOfRMold = sensorRm3100.diffOfRM;


    sensorRm3100.rm3100VaryData.EMDataVary_x =
        sensorRm3100.rm3100CurrentData.EMDataCurrent_x -
        sensorRm3100.rm3100DynamicBoottom.EMDataBottom_x;   //Xǿ�ȱ仯ʸ����,��λ
    sensorRm3100.rm3100VaryData.EMDataVary_y =
        sensorRm3100.rm3100CurrentData.EMDataCurrent_y -
        sensorRm3100.rm3100DynamicBoottom.EMDataBottom_y;   //Xǿ�ȱ仯ʸ����,��λ
    sensorRm3100.rm3100VaryData.EMDataVary_z =
        sensorRm3100.rm3100CurrentData.EMDataCurrent_z -
        sensorRm3100.rm3100DynamicBoottom.EMDataBottom_z;   //Xǿ�ȱ仯ʸ����,��λ
	sensorRm3100.diffOfRM =  (unsigned long) (sqrtf(   
												   (long)((sensorRm3100.rm3100VaryData.EMDataVary_x))*(long)((sensorRm3100.rm3100VaryData.EMDataVary_x))
													   +(long)((sensorRm3100.rm3100VaryData.EMDataVary_y))*(long)((sensorRm3100.rm3100VaryData.EMDataVary_y))
														   +4*(long)((sensorRm3100.rm3100VaryData.EMDataVary_z))*(long)((sensorRm3100.rm3100VaryData.EMDataVary_z))));
    //����Z���ģֵ�Ĺ��׶�
    sensorRm3100.diffOfRM = (int32_t)sensorRm3100.diffOfRM *
                            0.8;                    //Z��3*3��--0.66��4*4��--0.55,2*2��---0.8��,2��---1��
    sensorRm3100.diffOfRM = TYPE_S8(sensorRm3100.diffOfRM);


    m_diffOfRMold = (m_diffOfRMold > sensorRm3100.diffOfRM) ? m_diffOfRMold
                    -
                    sensorRm3100.diffOfRM : sensorRm3100.diffOfRM - m_diffOfRMold;
    if (m_diffOfRMold >= 4 && parkStatus.currentParkState == 1)
    {
        parkStatus.fastGetParkCount = 3;  //�����ο��ټ��

    }

}

uint8_t BerthPo_FixedVCheck()         //�̶�ֵ�жϳ�λ״̬
{

    uint8_t ValueWeight = 0; //��ֵȨ�ؼ���
    short Module_T, Module_TB;
    short Module_XT, Module_YT, Module_ZT;
    short Addit_XYZT, Mul_XYZAT = 1;

    Module_T = abs((short)
                   sensorRm3100.diffOfRM);     //xyzǿ�ȱ仯ʸ�����ƽ���ͣ���λ΢��
    Module_XT = abs((short)((
                                sensorRm3100.rm3100VaryData.EMDataVary_x)));  //Xǿ�ȱ仯ʸ����,��λ΢��
    Module_YT = abs((short)((
                                sensorRm3100.rm3100VaryData.EMDataVary_y)));   //yǿ�ȱ仯ʸ����,��λ΢��
    Module_ZT = abs((short)((
                                sensorRm3100.rm3100VaryData.EMDataVary_z)));   //zǿ�ȱ仯ʸ����,��λ΢��
    Module_TB = (short)
                controlConfig.paramConfig.getEMBottom_RFModThreshold;       //xyzǿ�ȱ仯ʸ�����ƽ���͵��жϷ�ֵ����λ΢��
    //����Ѿ��г���������ֵ0.8���ı�׼�о���������0.8���Ļ���ռ䣬��������ٽ�ʱ��������������
    if ( parkStatus.currentParkState == 1)
    {
        Module_TB = (int16_t)(Module_TB * 0.8);

    }
    if(Module_XT  > Module_TB * 0.6)           ValueWeight = ValueWeight + 1; //
    if(Module_YT  > Module_TB * 0.6)           ValueWeight = ValueWeight + 1; //
    if(Module_ZT  > Module_TB * 0.6)           ValueWeight = ValueWeight + 1; //

    Addit_XYZT = (Module_XT + Module_YT +
                  Module_ZT); //xyzǿ�ȱ仯ʸ�����ۼӺͣ���λ΢��
    if(Addit_XYZT >= Module_TB * 1.5)
        ValueWeight = ValueWeight +
                      1; //��xyz����ǿ�ȱ仯ֵ�ۼӺʹ����趨ǿ�ȷ�ֵ1.5��
    //���������֣��ڵ��г����ų��ֵ�ǿ�ȱ仯
    //��xyz����ǿ�ȱ仯ֵ�˻��ٳ��ԽǶȱ仯ֵ���������趨�Ƕȷ�ֵ6��
    if (Module_XT >= 4)                          Mul_XYZAT = Mul_XYZAT *
                (Module_XT - 2);
    if (Module_YT >= 4)                          Mul_XYZAT = Mul_XYZAT *
                (Module_YT - 2);
    if (Module_ZT >= 4)                          Mul_XYZAT = Mul_XYZAT *
                (Module_ZT - 2);
    if(Mul_XYZAT >= Module_TB * 3)                 ValueWeight = ValueWeight + 1;
    return ValueWeight;
}




