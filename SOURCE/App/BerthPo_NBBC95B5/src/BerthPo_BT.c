#include "BerthPo_BT.h"
#include "Drivers_I410ES.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Delay.h"
#include "BerthPo_Common.h"
SBT_INFO_TypeDef G_BTInfo;
SBT_CMD_TypeDef  BT_CMD_Data[2] =
{
    //ָ������              ����Դ���    ��ʱʱ��    ��ȷ����ֵ              �Ƿ񷵻�        ��ʼ״̬    ��������
    {"AT+B GLDN\r",     3,              1,          "\r",               ENABLE,         NO_BACK,        ""},
};

/***************�����ⲿ����*********************/
extern BT_I410ES_OPERATION_TypeDef BT_I410ES_Operation;
extern SNET_MUTUAL_INFO  netMutualInfo;
extern SCONTROL_CONFIG  controlConfig;
extern SSENSOR_RM3100 sensorRm3100;
extern SCONTROL_SYMPLE tagConfigSymple;
extern SBERTHPO_PARK_STATUS parkStatus ;
extern SBT_I410ES_GPIO_TypeDef BTI410ES_GPIO;
/*******************************************************************************
* Function Name :void BerthPo_BTInit()
* Description   :������ʼ������
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          :2018.7.12
*******************************************************************************/
void BerthPo_BTInit(void)
{
    BT_I410ES_Operation.BT_I410ES_CallBack = BerthPo_JudgeBTRxData;
}
/*******************************************************************************
* Function Name :void Drivers_BTI410ESInit(QUEUE8_t*    pUartRxQueue, uint8_t* pUartRxBuf)
* Description   :�ж�����ģ�鴮�ڽ���BUFFER
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          :2018.7.12
*******************************************************************************/
void BerthPo_JudgeBTRxData(void)
{
    uint8_t m_rxTemp[64] = {0};
    Drivers_BTI410ESRead(m_rxTemp);
    if(strstr(m_rxTemp, "AT-B STATE 03") != NULL)
    {
        G_BTInfo.BT_modeStatus = CONNECTED;
    }
    if(strstr(m_rxTemp, "AT-B STATE 01") != NULL)
    {
        G_BTInfo.BT_modeStatus = ADVERTISING;
    }
    if(strstr(m_rxTemp, "AT-B STATE 00") != NULL)
    {
        G_BTInfo.BT_modeStatus = INIT_READY;
    }
    if(strstr(m_rxTemp, "AT-B STATE 04") != NULL)
    {
        G_BTInfo.BT_modeStatus = DISCONNECTED;
    }
    if(strstr(m_rxTemp, "AT-B STATE 05") != NULL)
    {
        G_BTInfo.BT_modeStatus = STANDBY;
    }
    if(strstr(m_rxTemp, "AT-B STATE 11") != NULL)
    {
        G_BTInfo.BT_modeStatus = CENTRAL_SCAN;
    }
    if(strstr(m_rxTemp, "AT-B STATE 13") != NULL)
    {
        G_BTInfo.BT_modeStatus = CENTRAL_CON;
    }
    if(strstr(m_rxTemp, "AT-B STATE 13") != NULL)
    {
        G_BTInfo.BT_modeStatus = CENTRAL_DISCON;
    }
    if(m_rxTemp[0] == 0xFF && m_rxTemp[1] == 0x66)
    {
        G_BTInfo.BT_Rx_mode = DATA_MODE;
        G_BTInfo.BT_RX_flag = 0x01;
    }
    else if(m_rxTemp[0] == 'A' && m_rxTemp[1] == 'T')
    {
        G_BTInfo.BT_Rx_mode = AT_MODE;
        if(strstr(m_rxTemp, BT_CMD_Data[G_BTInfo.currentCmdNum].pTrueBack) != NULL)
        {
            BT_CMD_Data[G_BTInfo.currentCmdNum].BT_Return = BT_BACK_OK;
        }
        else if(strstr(m_rxTemp, "ERROR") != NULL)
        {
            BT_CMD_Data[G_BTInfo.currentCmdNum].BT_Return = BT_BACK_ERROR;
        }
    }
    BT_I410ES_Operation.BT_I410ES_BufReset();
    BerthPo_DealBTData(m_rxTemp);   //����Īά������
}

/*
 *@name:    deal_BT_data
 *@author:  HL
 *@date:    2018-01-18
 *@brief:   ����ָ��
 *@return:
 */
ErrorStatus BerthPo_DealBTData(uint8_t *btRxbuf)
{
    uint8_t m_frameLenTemp ;
    uint8_t m_sendFrameLenTemp;
    uint16_t local_crc = 0, remote_crc = 1;
    if(G_BTInfo.BT_Rx_mode != DATA_MODE)
    {
        return ERROR;
    }
    if(G_BTInfo.BT_RX_flag != 0x01)
    {
        return ERROR;
    }
    G_BTInfo.BT_RX_flag = 0x00;
    PBT_RCV_DATA_TypeDef pBT_rcvData;
    pBT_rcvData = (PBT_RCV_DATA_TypeDef)btRxbuf;
    if(pBT_rcvData->framePrefix0 != 0xFF || pBT_rcvData->framePrefix1 != 0x66)
    {
        return ERROR;   // ֡ͷ����
    }
    m_frameLenTemp = pBT_rcvData->frameLength + 3;
    local_crc = CRC_16((uint8_t *) & (pBT_rcvData->framePrefix0),
                       m_frameLenTemp);    //����CRC��ȷ��
    remote_crc = pBT_rcvData->commandData[pBT_rcvData->frameLength - 2] * 256 +
                 pBT_rcvData->commandData[pBT_rcvData->frameLength - 1];
    if(local_crc != remote_crc)
    {
        return ERROR;   // CRC����
    }
    PBT_RCV_DATA_TypeDef pFeedBack = (PBT_RCV_DATA_TypeDef)G_BTInfo.txBuf;
    memset(G_BTInfo.txBuf, 0, BT_SEND_DATA_SIZE);
    pFeedBack->framePrefix0 = pBT_rcvData->framePrefix0;
    pFeedBack->framePrefix1 = pBT_rcvData->framePrefix1;
    pFeedBack->commandCode = pBT_rcvData->commandCode;
    pFeedBack->protocolVersion = pBT_rcvData->protocolVersion ;

    //TIM4_Count = 0;
    switch (pBT_rcvData->commandCode)
    {
        case 0x01:    //����
            pFeedBack->commandData[0] = 0x01;
            pFeedBack->frameLength = 1 + 2;
            //Calculate_Crc16((uint8_t *)&pFeedBack->framePrefix0, pFeedBack->frameLength + 5 - 2);
            //BT_I410ES_Operation.BT_I410ES_Write((uint8_t*)&feedback, pFeedBack->frameLength + 5);
            //enter_Bootloader();
            nop();
            break;
        case 0x02:    //��IP
            pFeedBack->commandData[0] = netMutualInfo.defaultCoapIpAddr[0];
            pFeedBack->commandData[1] = netMutualInfo.defaultCoapIpAddr[1];
            pFeedBack->commandData[2] = netMutualInfo.defaultCoapIpAddr[2];
            pFeedBack->commandData[3] = netMutualInfo.defaultCoapIpAddr[3];
            pFeedBack->commandData[4] = netMutualInfo.defaultCoapPort % 256;
            pFeedBack->commandData[5] = netMutualInfo.defaultCoapPort / 256;
            pFeedBack->frameLength = 6 + 2;
            m_frameLenTemp = pFeedBack->frameLength + 3;
            Calculate_Crc16((uint8_t *)&pFeedBack->framePrefix0, m_frameLenTemp);
            m_sendFrameLenTemp = pFeedBack->frameLength + 5;
            BT_I410ES_Operation.BT_I410ES_Write((uint8_t*)pFeedBack, m_sendFrameLenTemp);
            break;
        case 0x03:   //дIP
#if COAP_MODE
            netMutualInfo.defaultCoapIpAddr[0] = pBT_rcvData->commandData[0];
            netMutualInfo.defaultCoapIpAddr[1] = pBT_rcvData->commandData[1];
            netMutualInfo.defaultCoapIpAddr[2] = pBT_rcvData->commandData[2];
            netMutualInfo.defaultCoapIpAddr[3] = pBT_rcvData->commandData[3];
            netMutualInfo.defaultCoapPort = pBT_rcvData->commandData[4] +
                                            (pBT_rcvData->commandData[5] * 256);
#elif UDP_MODE
            netMutualInfo.defaultUdpIpAddr[0] = pBT_rcvData->commandData[0];
            netMutualInfo.defaultUdpIpAddr[1] = pBT_rcvData->commandData[1];
            netMutualInfo.defaultUdpIpAddr[2] = pBT_rcvData->commandData[2];
            netMutualInfo.defaultUdpIpAddr[3] = pBT_rcvData->commandData[3];
            netMutualInfo.defaultUdpPort = pBT_rcvData->commandData[4] +
                                           (pBT_rcvData->commandData[5] * 256);
#endif
            pFeedBack->commandData[0] = 0x01;
            pFeedBack->frameLength = 1 + 2;
            m_frameLenTemp = pFeedBack->frameLength + 3;
            Calculate_Crc16((uint8_t *)&pFeedBack->framePrefix0, m_frameLenTemp);
            m_sendFrameLenTemp = pFeedBack->frameLength + 5;
            BT_I410ES_Operation.BT_I410ES_Write((uint8_t*)pFeedBack, m_sendFrameLenTemp);
            BerthPo_WriteParamToFlash();
            break;
        case 0x04:    //��ȡ��̬����
            pFeedBack->commandData[0] = sensorRm3100.rm3100DynamicBoottom.EMDataBottom_x;
            pFeedBack->commandData[1] = sensorRm3100.rm3100DynamicBoottom.EMDataBottom_y;
            pFeedBack->commandData[2] = sensorRm3100.rm3100DynamicBoottom.EMDataBottom_z;
            pFeedBack->frameLength = 3 + 2;
            m_frameLenTemp = pFeedBack->frameLength + 3;
            Calculate_Crc16((uint8_t *)&pFeedBack->framePrefix0, m_frameLenTemp);
            m_sendFrameLenTemp = pFeedBack->frameLength + 5;
            BT_I410ES_Operation.BT_I410ES_Write((uint8_t*)pFeedBack, m_sendFrameLenTemp);
            break;
        case 0x05:    //���ö�̬����
            pFeedBack->frameLength = 1 + 2;
            if(BerthPo_SetRm3100Base() == 1)
            {
                pFeedBack->commandData[0] = 1;
                tagConfigSymple.sendNodeCount--;
            }
            else
            {
                pFeedBack->commandData[0] = 0;
            }
            m_frameLenTemp = pFeedBack->frameLength + 3;
            Calculate_Crc16((uint8_t *)&pFeedBack->framePrefix0, m_frameLenTemp);
            m_sendFrameLenTemp = pFeedBack->frameLength + 5;
            BT_I410ES_Operation.BT_I410ES_Write((uint8_t*)pFeedBack,
                                                m_sendFrameLenTemp);                   //��ȡ����
            break;
        case 0x06:    //��ȡ�û� ID
            //pFeedBack->commandData[0] = controlConfig.nodeConfig.userCode % 256;
            //pFeedBack->commandData[1] = controlConfig.nodeConfig.userCode / 256;
            pFeedBack->frameLength = 2 + 2;
            m_frameLenTemp = pFeedBack->frameLength + 3;
            Calculate_Crc16((uint8_t *)&pFeedBack->framePrefix0, m_frameLenTemp);
            m_sendFrameLenTemp = pFeedBack->frameLength + 5;
            BT_I410ES_Operation.BT_I410ES_Write((uint8_t*)pFeedBack, m_sendFrameLenTemp);
            nop();
            break;
        case 0x07:    //�����û� ID
            //controlConfig.nodeConfig.userCode = pBT_rcvData->commandData[0] +
            //                                    pBT_rcvData->commandData[1] * 256;
            pFeedBack->commandData[0] = 0x01;
            pFeedBack->frameLength = 1 + 2;
            m_frameLenTemp = pFeedBack->frameLength + 3;
            Calculate_Crc16((uint8_t *)&pFeedBack->framePrefix0, m_frameLenTemp);
            m_sendFrameLenTemp = pFeedBack->frameLength + 5;
            BT_I410ES_Operation.BT_I410ES_Write((uint8_t*)pFeedBack, m_sendFrameLenTemp);
            BerthPo_WriteParamToFlash();
            break;
        case 0x08:    //��ȡ��λ��ID
            pFeedBack->commandData[0] = controlConfig.nodeConfig.idNumber[0];
            pFeedBack->commandData[1] = controlConfig.nodeConfig.idNumber[1];
            pFeedBack->commandData[2] = controlConfig.nodeConfig.idNumber[2];
            pFeedBack->frameLength = 3 + 2;
            m_frameLenTemp = pFeedBack->frameLength + 3;
            Calculate_Crc16((uint8_t *)&pFeedBack->framePrefix0, m_frameLenTemp);
            m_sendFrameLenTemp = pFeedBack->frameLength + 5;
            BT_I410ES_Operation.BT_I410ES_Write((uint8_t*)pFeedBack, m_sendFrameLenTemp);
            break;
        case 0x09:    //���ò�λ��ID
            controlConfig.nodeConfig.idNumber[0] = pBT_rcvData->commandData[0];
            controlConfig.nodeConfig.idNumber[1] = pBT_rcvData->commandData[1];
            controlConfig.nodeConfig.idNumber[2] = pBT_rcvData->commandData[2];
            pFeedBack->commandData[0] = 0x01;
            pFeedBack->frameLength = 1 + 2;
            m_frameLenTemp = pFeedBack->frameLength + 3;
            Calculate_Crc16((uint8_t *)&pFeedBack->framePrefix0, m_frameLenTemp);
            m_sendFrameLenTemp = pFeedBack->frameLength + 5;
            BT_I410ES_Operation.BT_I410ES_Write((uint8_t*)pFeedBack, m_sendFrameLenTemp);
            BerthPo_WriteParamToFlash();
            break;
        case 0x0A:    //ATָ��
            int8_t m_ATCmdFromBt[30];
            memset(m_ATCmdFromBt, 0, 30);
            memcpy(m_ATCmdFromBt, (const void *)&pBT_rcvData->commandData[0],
                   pBT_rcvData->frameLength - 2);
            m_ATCmdFromBt[pBT_rcvData->frameLength - 2] = '\r';
            m_ATCmdFromBt[pBT_rcvData->frameLength - 1] = '\n';
            BT_I410ES_Operation.BT_I410ES_PowerReset(&BTI410ES_GPIO);
	    DelayMs(2000);
            BT_I410ES_Operation.BT_I410ES_Write(m_ATCmdFromBt, strlen(m_ATCmdFromBt));
            BT_I410ES_Operation.BT_I410ES_PowerReset(&BTI410ES_GPIO);
            //BT_I410ES_Operation.BT_I410ES_Write((uint8_t*)pFeedBack, m_ATLen);
            nop();
            break;
        case 0x0B:
            BerthPo_GetRM3100Data();            //��ȡ�ش�
            BerthPo_EMDealGeomagneticValue();   //����ش�����
            BerthPo_BerthStateSwithProcess();
            pFeedBack->commandData[0] = sensorRm3100.rm3100CurrentData.EMDataCurrent_x;
            pFeedBack->commandData[1] = sensorRm3100.rm3100CurrentData.EMDataCurrent_y;
            pFeedBack->commandData[2] = sensorRm3100.rm3100CurrentData.EMDataCurrent_z;
            if(parkStatus.currentParkState == 0x01) //�г�
            {
                pFeedBack->commandData[3] = 0x01;
            }
            else
            {
                pFeedBack->commandData[3] = 0x00;
            }
            pFeedBack->commandData[4] = sensorRm3100.diffOfRM;
            pFeedBack->frameLength = 5 + 2;
            m_frameLenTemp = pFeedBack->frameLength + 3;
            Calculate_Crc16((uint8_t *)&pFeedBack->framePrefix0, m_frameLenTemp);
            m_sendFrameLenTemp = pFeedBack->frameLength + 5;
            BT_I410ES_Operation.BT_I410ES_Write((uint8_t*)pFeedBack, m_sendFrameLenTemp);
            nop();
            break;
        /*case 0x0C:  //��������֡������
        break;
        case 0x0D:    //��ȡ�豸����״̬
        pFeedBack->commandData[0] = TagCng_symple.Config.Tag_initNB;
        pFeedBack->frameLength = 1 + 2;
        Calculate_Crc16((uint8_t *)&pFeedBack->framePrefix0, pFeedBack->frameLength + 5 - 2);
        sendUsartData(USART_FOR_BT, &MyBT->BT_TX_BUFF_TMP[0], pFeedBack->frameLength + 5);
        break;*/
        /*case 0x0E:    //�����豸����״̬
            if(pBT_rcvData->commandData[0] == 0x01)
            {
                if(netMutualInfo.initNB != 0x01)
                {
                    netMutualInfo.initNB = 0x01;
                    NBiotPowerSet;
                }
            }
            else if(pBT_rcvData->commandData[0] == 0x00)
            {
                if(TagCng_symple.Config.Tag_initNB != 0x00)
                {
                    TagCng_symple.Config.Tag_initNB = 0x00;
                    NBiotPowerClr;
                    NBAtCommand.NetworkStatus = 0;
                }
            }
            else
            {}
            pFeedBack->commandData[0] = 0x01;
            pFeedBack->frameLength = 1 + 2;
            Calculate_Crc16((uint8_t *)&pFeedBack->framePrefix0,
                            pFeedBack->frameLength + 5 - 2);
            sendUsartData(USART_FOR_BT, &MyBT->BT_TX_BUFF_TMP[0],
                          pFeedBack->frameLength + 5);
            break;
            */
        case 0x0F:    //��ȡ����ʱ��
            pFeedBack->commandData[0] = netMutualInfo.heartbeatInterval % 256;
            pFeedBack->commandData[1] = netMutualInfo.heartbeatInterval / 256;
            pFeedBack->frameLength = 2 + 2;
            m_frameLenTemp = pFeedBack->frameLength + 3;
            Calculate_Crc16((uint8_t *)&pFeedBack->framePrefix0, m_frameLenTemp);
            m_sendFrameLenTemp = pFeedBack->frameLength + 5;
            BT_I410ES_Operation.BT_I410ES_Write((uint8_t*)pFeedBack, m_sendFrameLenTemp);
            break;
        case 0x10:    //��������ʱ��
            controlConfig.paramConfig.heartbeatInterval = pBT_rcvData->commandData[0] +
                    pBT_rcvData->commandData[1] * 256;
            pFeedBack->commandData[0] = 0x01;
            pFeedBack->frameLength = 1 + 2;
            m_frameLenTemp = pFeedBack->frameLength + 3;
            Calculate_Crc16((uint8_t *)&pFeedBack->framePrefix0, m_frameLenTemp);
            m_sendFrameLenTemp = pFeedBack->frameLength + 5;
            BT_I410ES_Operation.BT_I410ES_Write((uint8_t*)pFeedBack, m_sendFrameLenTemp);
            BerthPo_WriteParamToFlash();
            tagConfigSymple.sendPackCount = (controlConfig.paramConfig.heartbeatInterval *
                                             60) / controlConfig.paramConfig.wdtInterval;
            break;
        default:
            break;
    }
    /*memset(MyBT->BT_RX_BUF_TMP, 0, BT_BUFF_SIZE);
    MyBT->BT_RX_CNT = 0;*/
    return ERROR;
}