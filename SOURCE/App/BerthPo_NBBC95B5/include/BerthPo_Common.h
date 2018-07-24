#ifndef _BERTHPO_COMMON_H_
#define _BERTHPO_COMMON_H_

#include "stm8l15x.h"
#include "Drivers_Common.h"
#include "BerthPo_RM3100.h"
#include "Drivers_I410ES.h"
#include "Drivers_LED.h"
#include "Drivers_RM3100.h"
#include "BerthPo_Protocol.h"
#include "Delay.h"
#include <math.h>

#define MAX_(bit1, bit2)  ((bit1) >= (bit2) ? (bit1) : (bit2))// ���ֵ
#define MIN_(bit1, bit2)  ((bit1) <= (bit2) ? (bit1) : (bit2))// ��Сֵ
#define TYPE_S8(data) (data) > 127 ? 127 : ((data) < -127 ? -127 : (data));//��data>127ʱ��data=127����-127=<data<=127ʱ��data=data����data<-127ʱ��data=-127
#define TYPE_U8(data) (data) > 255 ? 255 : ((data) < 0 ? 0 : (data));//��data>255ʱ��data=255����0=<data<=255ʱ��data=data����data<0ʱ��data=0 




//ö�ٳ�λ״̬
typedef enum
{
    BERTHPO_PARK_STATE_NULL,    //�޳�
    BERTHPO_PARK_STATE_HAVE    //�г�
} BERTH_STATE;
//ö�ٹ���ģʽ
typedef enum
{
    BERTHPO_MODE_FACTORY = 0xC5,       //����ģʽ
	BERTHPO_MODE_DEEP_SLEEP = 0xB5,    //�������ģʽ
	BERTHPO_MODE_SELF_TEST = 0xD5,     //�����Լ�ģʽ
    BERTHPO_MODE_ACTIVE = 0xA5,        //������������ģ   
} BERTHPO_MODE;

typedef struct
{
    uint8_t workMode;              //����ģʽ
    uint8_t initNB;
    uint16_t userCode;
    uint8_t  idNumber[6];     //UID
    uint8_t  keyNumber[6];    //KEY
    uint8_t updateFlag;
    uint8_t version;
    uint16_t wdtInterval;          //WDT˯��ʱ��
    uint16_t heartbeatInterval;    //����ʱ��
    uint16_t alarmValid;
    uint8_t paraIntFlag[3];        //����ͬ����־
    uint8_t ledFlag;
    uint8_t getEMBottom_RFFlag;    //�شſ���
    int16_t getEMBottom_RFModThreshold;
    int16_t getEMBottom_RFAngleThreshold;
    int16_t EMDataBottom_x;
    int16_t EMDataBottom_y;
    int16_t EMDataBottom_z;
    uint8_t fastRouse;                //���ٻ��ѱ�־
    uint8_t fastRouseAlarm;
    uint8_t serverConfig;
    uint8_t defaultCoapIpAddr[4];     //COAPЭ��IP
    uint16_t defaultCoapPort;          //COAPЭ��˿�
    uint8_t defaultUdpIpAddr[4];   //ԭʼЭ��IP
    uint16_t defaultUdpPort[4];     //ԭʼЭ��˿�
}*PNET_MUTUAL_INFO, SNET_MUTUAL_INFO;

typedef struct
{
    uint8_t paraIntFlag;                            //������ʼ����־
    uint8_t wdtInterval;                            //wdt�̶�˯��ʱ�䣬
    uint8_t fastRouseAlarm;                         //��ο��ٻ��ѱ�����־
    uint8_t fastRouse;                              //wdt���ٻ��ѱ�־
    uint8_t heartbeatInterval;                      //�������ʱ��
    uint32_t alarmValid;
    uint32_t alarmStatus;
    uint8_t HMCOutFlag;                             //�ȴ��شŲ����˳���־��0--�˳���1--�����ȴ�
    uint8_t getEMBottomFlag;                        //ȡ���׳ɹ���־
    uint8_t getEMBottom_RFFlag;                     //�شŹ�����־
    int32_t getEMBottom_RFModThreshold;             //�ų�ǿ��������
    int32_t getEMBottom_RFAngleThreshold;           //�ų�ƫת�Ƕ�������
    int16_t EMDataBottom_x;                         //x��ǿ�ȱ���
    int16_t EMDataBottom_y;                         //y��ǿ�ȱ���
    int16_t EMDataBottom_z;                         //z��ǿ�ȱ���
    int32_t EMData_xyz_AbsValue_Module_Bottom;      //xyz��仯ģֵ����
    int32_t EMData_RMS;
    int32_t EMData_xRMS;
    int32_t EMData_zRMS;
} *PPARAM_CONFIG, SPARAM_CONFIG;
typedef struct
{
    uint8_t  idNumber[6];     //UID
    uint8_t  keyNumber[6];    //KEY
    uint8_t  ledFlag;    //led���ر�־��ȡϵͳ������
    uint8_t  mcuSleepFlag;
}*PNODE_CONFIG, SNODE_CONFIG;
typedef struct
{
    uint8_t initNB;
    uint8_t workMode;                   //����ģʽ
    SNODE_CONFIG   nodeConfig;          //��7�ֽ�
    SPARAM_CONFIG  paramConfig;         //��32�ֽ�
} *PCONTROL_CONFIG, SCONTROL_CONFIG;
typedef struct
{
    uint8_t currentParkState;    //��ǰͣ��״̬�����ڱȽ�ͣ��״̬�Ƿ�ı�
    uint8_t fastGetParkCount;    //���ٻ�ȡ����״̬�������������Ϊ��ʱ��Ϊǰ�泵���ո��뿪����ʱ������������ֹ��������λ����
}*PBERTHPO_PARK_STATUS, SBERTHPO_PARK_STATUS;

typedef struct
{


    uint8_t batteryCheckCount;
    int32_t batteryCount;              //��ؼ�������
    uint32_t sendPackCount;
    uint32_t sendNodeCount;            //�Ƿ�ﵽ��������������
    uint8_t  autoLockCount;            //�Զ���s������������Ӧ��
    uint8_t  sendToServerCount;        //���ͼ���,�����������ݰ�˳�����֣�Ӧ���������������ӳٵ��µ�����˳�����
    uint8_t  sendDataRandomDelay;      //�������������ʱ
    uint8_t  debugInfoFlag;            //������Ϣ��־
    uint8_t  outBottomFlag;            //������ױ�־,0--�ֶ�У׼���ף�1--��̬����
    uint8_t  startUpGetBottom;         //ϵͳ������ȡ���ر�־
	uint16_t randomDelay;              //�����ʱʱ��
} *PCONTROL_SYMPLE, SCONTROL_SYMPLE;

/************************��������*********************/
void BerthPo_WriteParamToFlash();
void BerthPo_ReadParamFromFlash();
void BerthPo_ProductTestMode();
void BerthPo_MakeHead();
void Hex_to_ASCII(char *pHex, char *pASCII, uint8_t len);
void BerthPo_GetRM3100Data();
uint8_t BerthPo_SetRm3100Base();
void BerthPo_EMDealGeomagneticValue();   //����ش�����
uint8_t BerthPo_FixedVCheck();           //�̶�ֵ�жϳ�λ״̬

#endif   //_BERTHPO_COMMON_H_
