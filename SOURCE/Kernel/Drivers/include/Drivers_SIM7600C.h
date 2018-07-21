#ifndef _SIM7600C_H_
#define _SIM7600C_H_

#define NO_BACK               0
#define BACK_OK               1
#define BACK_ERROR            2
#define BACK_TIMEOUT          3

typedef struct    //ATָ�������Ϣ����
{
	int8_t *cmd;            //���͵�ATָ��
	int8_t timeOut;         //��ʱʱ��
	int8_t *backOkVal;      //ָ����ȷ����ֵ
	int8_t *backResult;     //ָ��ִ�з���ֵ
	int8_t retryTimesMax;   //��󷢳����ʹ���
}SCMD_INFO_DEF, PCMD_INFO_DEF;
typedef struct   //�����������Ϣ����
{
  int8_t  netType;         //��������:udp/tcp
  int8_t  serverIp[15];    //������IP
  int8_t  serverPort[5];   //�������˿�
}SNET_INFO_DEF, *PNET_INFO_DEF;
typedef struct  //SIM7600C��Ϣ��ض���
{
  int8_t signalStrength;   //�ź�ǿ��
  int8_t currentCmdNum;    //��ǰָ�����
  int16_t cmdExecCnt;      //ָ��ִ�м���
  int8_t rxTemp[64];       //����������ʱ������
  int8_t rxBuffer[64];     //�������ݻ�����
  int8_t rcvPointer;       //�������ݼ���ָ��
  int8_t txTemp[64];       //����������ʱ������
  int8_t txBuffer[64];     //�������ݻ�����
}SSIM7600C_INFO_DEF, *PSIM7600C_INFO_DEF;
#endif   //_SIM7600C_H_