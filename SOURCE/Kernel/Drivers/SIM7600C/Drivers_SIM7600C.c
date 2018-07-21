#include "Drives_SIM7600C.h"
#include <stdarg.h>
#include "Delay.h"

/*****************��������****************/
SSIM7600C_INFO_DEF g_sim7600cInfo;


/*����ATָ��Ϸ�װ������ʹ��ʱ����*/
volatile SCMD_INFO_DEF  g_cmdArray[21] =
{
    //cmd/timeOut(s)/returnValOk/retryTimesMax
    {"AT+CGDCONT=1,\"IP\",\"CMNET\"", 5, "OK", 3},
    {"AT+CIPMODE=1", 5, "OK", 3},
    {"AT+NETOPEN", 5, "OK", 3},
    {"AT+CIPOPEN=0,\"TCP\", \"211.149.158.237\",8011", 10, "OK", 3},
};


/*******************************************************************************
* Function Name : void Drivers_SimParamSet(PNET_INFO_DEF pNetInfo, uint8_t netType, uint8_t* serverIp, uint8_t* serverPort)
* Description   : �����������
* Author        : YJD
* Input         : pNetInfo:������Ϣ�ṹ��          netType:��������
                  serverIp:������IP        serverPort:�������˿�
* Output        : NULL
* Other         : NULL
* Date          :2018.06.09
*******************************************************************************/
void Drivers_Sim7600cSetNetParam(PNET_INFO_DEF pNetInfo, uint8_t netType,
                                 uint8_t* serverIp, uint8_t* serverPort)
{
    pNetInfo->netType = netType;
    strcpy(pNetInfo->serverIp, serverIp);
    strcpy(pNetInfo->serverPort, serverPort);
}

/*******************************************************************************
* Function Name : void Drivers_SimParamSet(PNET_INFO_DEF pNetInfo, uint8_t netType, uint8_t* serverIp, uint8_t* serverPort)
* Description   : ����͸��ģʽ����Tcp����
* Author        : YJD
* Input         :
* Output        : NULL
* Other         : NULL
* Date          :2018.06.09
*******************************************************************************/

uint8_t Drivers_Sim7600cSerialPenetrate(PSIM7600C_INFO_DEF pSim7600cInfo,
                                        uint8_t cmdNum, int8_t *fmt, ...)
{
    uint8_t m_backResult;
    static int m_len = 0;
    int8_t *m_ptr = pSim7600cInfo->TxBuffer;
    va_list m_ap;
    g_cmdArray[cmdNum]->backResult = NO_BACK;
    pSim7600cInfo->cmdExecCnt =  g_cmdArray[cmdNum]->timeOut * 600;
    memset(pSim7600cInfo->txBuffer, 0, sizeof(pSim7600cInfo->txBuffer));
    pSim7600cInfo->currentCmdNum = cmdNum;
    va_start(m_ap, fmt);
    len = (int8_t)vsprintf(m_ptr, fmt, m_ap);
    va_end(m_ap);
    BSP_UartSendStr(pSim7600cInfo->txBuffer);   //��������
    DelayMs(1000);   //��ʱ1S
    while(g_cmdArray[cmdNum].backResult ==
          NO_BACK)
    {
        DelayMs(1000);
        pSim7600cInfo->cmdExecCnt--;
        if(!pSim7600cInfo->cmdExecCnt--)
        {
            g_cmdArray[cmdNum]->backResult = BACK_TIMEOUT;
        }
    }
    pSim7600cInfo->cmdExecCnt = 0;
    m_backResult = g_cmdArray[cmdNum]->backResult;
    return m_backResult;
}
/*******************************************************************************
* Function Name : void Drivers_Sim7600ParaseData(PSIM7600C_INFO_DEF pSim7600cInfo)
* Description   : ����4Gģ�鷵�ص�����
* Author        : YJD
* Input         :
* Output        : NULL
* Other         : NULL
* Date          :2018.06.11
*******************************************************************************/
void Drivers_Sim7600ParaseData()
{


}

