#ifndef _BERTHPO_BT_H_
#define _BERTHPO_BT_H_
#include <stm8l15x.h>
#include "Queue.h"
#define BT_COMMAND_DATA_SIZE    150
#define BT_SEND_DATA_SIZE       150 
typedef enum 
{
        BT_NO_BACK = 0, 
	BT_BACK_OK = 1, 
	BT_BACK_ERROR = 2, 
	BT_BACK_TIMEOUT = 3, 
	BT_BACK_TRYOUT = 4, 
	BT_NEW_MESSAGE = 5
}RETURN_STATUS;
typedef enum   //������������ģʽ������/ָ��
{
	AT_MODE = 0,
	DATA_MODE = 1
}BT_RX_MODE;
typedef enum 
{
	INIT_READY = 0,
	ADVERTISING = 1,
	CONNECTED = 2,
	DISCONNECTED = 3,
	STANDBY = 4,
	CENTRAL_SCAN = 5,
	CENTRAL_CON = 6,
	CENTRAL_DISCON = 7
} BT_MODE_STATUS;
	
typedef struct         //�յ��������������
{
	uint8_t framePrefix0;
	uint8_t framePrefix1;
	uint8_t commandCode;
	uint8_t frameLength;
	uint8_t protocolVersion;
	uint8_t commandData[BT_COMMAND_DATA_SIZE];
}*PBT_RCV_DATA_TypeDef, SBT_RCV_DATA_TypeDef;

typedef struct{         //AT���ָ��
	char *pCmdData;				    // ָ������
	uint16_t maxTry;				// ����Դ���
	uint16_t timeOut;				// unit: second
	char *pTrueBack;				//ָ�������ȷ����
	FunctionalState isReturn;		// �Ƿ񷵻�
	RETURN_STATUS BT_Return;
	char *pReturn;
}*PBT_CMD_TypeDef, SBT_CMD_TypeDef;	
typedef struct {
	char BT_Version[6];	    //BT version default:V1.0
	char BT_Address[12];	//BT address MAC ��ַ
	char BT_Name[20];	    //BT name default:BRT DATA
	uint8_t BT_RX_flag;
	BT_RX_MODE BT_Rx_mode;
	uint8_t	BT_RX_count;
	uint8_t BT_TX_count;
	uint8_t BT_enableFlag;
	BT_MODE_STATUS BT_modeStatus;
	uint8_t currentCmdNum;
	uint32_t currentCmdTime;
	uint8_t txBuf[BT_SEND_DATA_SIZE];
//	int8_t BT_TX_BUFF_TMP[BT_BUFF_SIZE];
//	int8_t BT_TX_BUFF[BT_BUFF_SIZE];
//	int8_t BT_RX_BUF_TMP[BT_BUFF_SIZE];
//	int8_t BT_RX_BUF[BT_BUFF_SIZE];
}*PBT_INFO_TypeDef,SBT_INFO_TypeDef;

/********************��������********************/
void BerthPo_BTInit(void);
void BerthPo_JudgeBTRxData(void);
ErrorStatus BerthPo_DealBTData(uint8_t *btRxbuf);
#endif   //_BERTHPO_BT_H_
