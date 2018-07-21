#include "BerthPo_SX1280_Common.h"
#include "Delay.h"
#include "Drivers_LED.h"
#include "Drivers_NFC.h"
#include <math.h>
/******************引用外部变量******************/
extern SBERTHPO_PARK_STATUS parkStatus;
extern SCONTROL_CONFIG  controlConfig;
extern SNET_MUTUAL_INFO  netMutualInfo;
extern SSENSOR_RM3100 sensorRm3100;
extern SRM3100_GPIO_TypeDef RM3100_GPIO;
extern LED_GPIO_TypeDef LED_GPIO;
extern SCONTROL_SYMPLE tagConfigSymple;

/*******************************************************************************
* Function Name : void BerthPo_WriteParamToFlash(SPARAM_NB_TAG paramNBTag)
* Description   : 系统运行需要的相关参数初始化
* Author        : YJD
* Input         : NULL
* Output        : NULL
* Other         : NULL
* Date          : 2018.05.11
*******************************************************************************/

void BerthPo_WriteParamToFlash()
{
    netMutualInfo.workStatus = controlConfig.workStatus;
    netMutualInfo.tagID[0] = controlConfig.nodeConfig.idNub[0];  //保存ID
    netMutualInfo.tagID[1] = controlConfig.nodeConfig.idNub[1];
    netMutualInfo.tagID[2] = controlConfig.nodeConfig.idNub[2];
	netMutualInfo.tagID[3] = controlConfig.nodeConfig.idNub[3];  //保存ID
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
* Description   : 系统运行需要的相关参数初始化
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
* Description   : 系统运行需要的相关参数初始化
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
	pframeData->packageHeader.deviceType = 0x06;   //射频泊位宝
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
* Description   :获取地磁数据
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
    DelayMs(100);                       //上电延时等待R3100准备，小于30us读出数据错误，典型值200us
    enableInterrupts();   //开中断,关中断动作在睡眠被唤醒后执行
    BSP_RtcGoSleep(30);   //rtc睡眠时间,15ms
    //  BSP_RtcDeepSleep();
    //RM3100_Operation.RM3100_McuRdRyOff(RM3100_GPIO);        //关地磁芯片中断输入
    //RM3100_Operation.RM3100_McuRdRyOn(RM3100_GPIO);         //开地磁芯片中断输入
    RM3100_Operation.RM3100_SetMeaureMode(&(RM3100_GPIO), 1);  //设置单一测量模式*/
    DelayMs(100);
    RM3100_Operation.RM3100_ReadBytes(0x24, &(RM3100_GPIO.gpioI2cStructer),
                                      m_tempBuf, 9);
    RM3100_Operation.RM3100_PowerOff(&(RM3100_GPIO));               //关闭RM3100所有端口节能

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
* Description   :设置RM3100地磁本底
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
            dataCount = 0;   //用以存储磁场当前值累计值
    int16_t bottomTemp_x = 0, bottomTemp_y = 0, bottomTemp_z = 0;
    controlConfig.paramConfig.getEMBottomFlag = 0;
    for(i = 0 ; i < 4; i++)
    {
        BerthPo_GetRM3100Data();            //取地磁数据
        data_x += sensorRm3100.rm3100CurrentData.EMDataCurrent_x;
        data_y += sensorRm3100.rm3100CurrentData.EMDataCurrent_y;
        data_z += sensorRm3100.rm3100CurrentData.EMDataCurrent_z;
        dataCount++;
        DelayMs(10);
    }
    //xyz值四舍五入，4次采样平均，所有+-2,2/4=0.5
    data_x = (data_x > 0) ? data_x + 2 : data_x - 2;
    data_y = (data_y > 0) ? data_y + 2 : data_y - 2;
    data_z = (data_z > 0) ? data_z + 2 : data_z - 2;

    bottomTemp_x = data_x / dataCount;
    bottomTemp_y = data_y / dataCount;
    bottomTemp_z = data_z / dataCount;

    //复检地磁本底，此时变化应该趋近于零，否则认为初始化错误，应该返回给上级设备错误信息
    for(i = 0; i < 10; i++)
    {
        DelayMs(10);
        BerthPo_GetRM3100Data();   //取地磁数据
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
        if(mRms < 10)//未开根号，减少计算量            未开根号，减少计算量 原来是4 改为9 by HL
        {
            controlConfig.paramConfig.getEMBottomFlag = 1;

            //固定本底值
            controlConfig.paramConfig.EMDataBottom_x =
                bottomTemp_x;          //MagneticFactor;
            controlConfig.paramConfig.EMDataBottom_y =
                bottomTemp_y;          //MagneticFactor;
            controlConfig.paramConfig.EMDataBottom_z =
                bottomTemp_z;          //MagneticFactor;

            //将当前获取的合法本底值作为动态本底值初始值
            sensorRm3100.rm3100DynamicBoottom.EMDataBottom_x =
                controlConfig.paramConfig.EMDataBottom_x;
            sensorRm3100.rm3100DynamicBoottom.EMDataBottom_y =
                controlConfig.paramConfig.EMDataBottom_y;
            sensorRm3100.rm3100DynamicBoottom.EMDataBottom_z =
                controlConfig.paramConfig.EMDataBottom_z;
            sensorRm3100.getTimeBottomCount = 0;

            sensorRm3100.clearBottomBufferFlag =
                1;                     //需要初始化，清除本底缓冲区内的数据
            return 1;
        }
    }
    return 0;
}
void BerthPo_EMDealGeomagneticValue()   //处理地磁数据
{

    int32_t m_diffOfRMold = sensorRm3100.diffOfRM;


    sensorRm3100.rm3100VaryData.EMDataVary_x =
        sensorRm3100.rm3100CurrentData.EMDataCurrent_x -
        sensorRm3100.rm3100DynamicBoottom.EMDataBottom_x;   //X强度变化矢量差,单位
    sensorRm3100.rm3100VaryData.EMDataVary_y =
        sensorRm3100.rm3100CurrentData.EMDataCurrent_y -
        sensorRm3100.rm3100DynamicBoottom.EMDataBottom_y;   //X强度变化矢量差,单位
    sensorRm3100.rm3100VaryData.EMDataVary_z =
        sensorRm3100.rm3100CurrentData.EMDataCurrent_z -
        sensorRm3100.rm3100DynamicBoottom.EMDataBottom_z;   //X强度变化矢量差,单位
	sensorRm3100.diffOfRM =  (unsigned long) (sqrtf(   
												   (long)((sensorRm3100.rm3100VaryData.EMDataVary_x))*(long)((sensorRm3100.rm3100VaryData.EMDataVary_x))
													   +(long)((sensorRm3100.rm3100VaryData.EMDataVary_y))*(long)((sensorRm3100.rm3100VaryData.EMDataVary_y))
														   +4*(long)((sensorRm3100.rm3100VaryData.EMDataVary_z))*(long)((sensorRm3100.rm3100VaryData.EMDataVary_z))));
    //加重Z轴对模值的贡献度
    sensorRm3100.diffOfRM = (int32_t)sensorRm3100.diffOfRM *
                            0.8;                    //Z轴3*3倍--0.66，4*4倍--0.55,2*2倍---0.8倍,2倍---1倍
    sensorRm3100.diffOfRM = TYPE_S8(sensorRm3100.diffOfRM);


    m_diffOfRMold = (m_diffOfRMold > sensorRm3100.diffOfRM) ? m_diffOfRMold
                    -
                    sensorRm3100.diffOfRM : sensorRm3100.diffOfRM - m_diffOfRMold;
    if (m_diffOfRMold >= 4 && parkStatus.currentParkState == 1)
    {
        parkStatus.fastGetParkCount = 3;  //启动次快速检测

    }

}

uint8_t BerthPo_FixedVCheck()         //固定值判断车位状态
{

    uint8_t ValueWeight = 0; //阀值权重计数
    short Module_T, Module_TB;
    short Module_XT, Module_YT, Module_ZT;
    short Addit_XYZT, Mul_XYZAT = 1;

    Module_T = abs((short)
                   sensorRm3100.diffOfRM);     //xyz强度变化矢量差的平方和，单位微特
    Module_XT = abs((short)((
                                sensorRm3100.rm3100VaryData.EMDataVary_x)));  //X强度变化矢量差,单位微特
    Module_YT = abs((short)((
                                sensorRm3100.rm3100VaryData.EMDataVary_y)));   //y强度变化矢量差,单位微特
    Module_ZT = abs((short)((
                                sensorRm3100.rm3100VaryData.EMDataVary_z)));   //z强度变化矢量差,单位微特
    Module_TB = (short)
                controlConfig.paramConfig.getEMBottom_RFModThreshold;       //xyz强度变化矢量差的平方和的判断阀值，单位微特
    //如果已经有车，则按照阈值0.8倍的标准判决，即存在0.8倍的缓冲空间，解决参数临界时反复跳动的问题
    if ( parkStatus.currentParkState == 1)
    {
        Module_TB = (int16_t)(Module_TB * 0.8);

    }
    if(Module_XT  > Module_TB * 0.6)           ValueWeight = ValueWeight + 1; //
    if(Module_YT  > Module_TB * 0.6)           ValueWeight = ValueWeight + 1; //
    if(Module_ZT  > Module_TB * 0.6)           ValueWeight = ValueWeight + 1; //

    Addit_XYZT = (Module_XT + Module_YT +
                  Module_ZT); //xyz强度变化矢量差累加和，单位微特
    if(Addit_XYZT >= Module_TB * 1.5)
        ValueWeight = ValueWeight +
                      1; //当xyz各轴强度变化值累加和大于设定强度阀值1.5倍
    //经分析发现，邻道有车干扰出现的强度变化
    //当xyz各轴强度变化值乘积再乘以角度变化值，积大于设定角度阀值6倍
    if (Module_XT >= 4)                          Mul_XYZAT = Mul_XYZAT *
                (Module_XT - 2);
    if (Module_YT >= 4)                          Mul_XYZAT = Mul_XYZAT *
                (Module_YT - 2);
    if (Module_ZT >= 4)                          Mul_XYZAT = Mul_XYZAT *
                (Module_ZT - 2);
    if(Mul_XYZAT >= Module_TB * 3)                 ValueWeight = ValueWeight + 1;
    return ValueWeight;
}




