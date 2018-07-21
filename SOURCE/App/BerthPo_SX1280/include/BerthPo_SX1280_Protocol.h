#ifndef _BERTHPO_PROTOCOL_H_
#define _BERTHPO_PROTOCOL_H_

#define RF_TX_BUFFER_SIZE    40
#define RF_RX_BUFFER_SIZE    40

#define    TagCmd_RES       0x01 //��λ
#define    TagCmd_ID        0x02 //����
#define    TagCmd_POW       0x03 //����
#define    TagCmd_CCO       0x04 //����
#define    TagCmd_CHA       0x05 // ����
#define    TagCmd_PN        0x06 //����
#define    TagCmd_TX        0x07 //����
#define    TagCmd_TIM       0x08 //����             	
#define    TagCmd_RX        0x09 //��
#define    TagCmd_MOD       0x0a //����
#define    TagCmd_DAT       0x0d //д
#define    TagCmd_ALM       0x0e //��ȡ
#define    TagCmd_POS       0x0F //
#define    TagCmd_WCM       0x10 //���ñ�ǩ
#define    TagCmd_CPC       0x11 //
#define    TagCmd_TPC       0x12 //����
#define    TagCmd_MDC       0x13 //�޸�
#define    TagCmd_BTM       0x14 //����
#define    TagCmd_BNK       0x15 //����
#define    TagCmd_SMS       0x17 //����
#define    TagCmd_SBT       0x18 //����
#define    TagCmd_SEN       0x19 //������ʹ��
#define    TagCmd_SDT       0x1a //��ȡ������
#define    TagCmd_CAS       0x14 //
#define    TagCmd_RPM       0x1d //��ǩ
#define    TagCmd_TRT       0x1e //д��ǩ���ݴ�������
#define    TagCmd_IOS       0x1F //������ǩ�ⲿIO
#define    TagCmd_THS       0x20
#define    TagCmd_SSD       0x21//��ȡ��������ֵ
#define    TagCmd_BRD_pose  0x22 //
#define    TagCmd_HMC_GetB  0x26 //�شű��ײ���
#define    TagCmd_INF       0x0b //
#define    TagCmd_GSB       0x30 //�ش�

#define    TagCmd_PanDianSucessSleepTime		0x22		//�̵�ɹ�˯��ʱ��
#define    TagCmd_RecFinishLedOn	         0x24	//���ý������ݳɹ���LED��˸
#define    TagCmd_CollisionSleepTimeOffSet	         0x25	//��ͻ˯��ʱ��ƫ������

#define    memeryMapSize   1024                     

#ifndef TRUE
#define TRUE    0x01
#endif

#ifndef FALSE
#define FALSE   0x00
#endif

#define   ReadAttr   0
#define   WriteAttr  1

//֡����1byte=(bit7~bit6)��Ϣ֡����+(bit5~bit3)�豸����
//�豸����
#define  Tag_SubFame        (0x00<<3)	
#define  ReadSubFame        (0x01<<3) 	
#define  CoodSubFame        (0x02<<3)	
#define  R_WSubFame         (0x03<<3)	
//��Ϣ֡����
#define  CtrlOfFrame        (0x00<<6)		
#define  DataOfFrame        (0x01<<6)	
#define  PoseOfFrame        (0x02<<6)	
#define  RespOfFrame        (0x03<<6)	


#define  StyleOfFrame      0x00 
//����֡
#define  AckOfFrame        0x00
//Ӧ��
#define  Coordinate        0x10
//��վ��
#define  LoadPrg           0x20 
//�м���
#define  ReaderOfFrame     0x18 
//��д��
#define  CallerOfFrame     0x20 
//������
#define  PositOfFrame      0x28 
//��λ��
#define  TgaeOfFrame       0x00 
//��ǩ
#define  ReserveOfFrame    0x38 
//Ԥ��

#define  YueduOfFrame     0x08
/***********************************************/

#define  StyleOfFrameData      0x40
//����֡

//����֡
#define  AckOfFrame        0x00 
//Ӧ��
#define  StationOfFrame    0x08 
//��վ��
#define  RelayOfFrame      0x10
//�м���
#define  ReaderOfFrame     0x18
//��д��
#define  LoadPrg           0x20 
//������
#define  PositOfFrame      0x28 
//��λ��
#define  TgaeOfFrame       0x00 
//��ǩ
#define  ReserveOfFrame    0x38
//Ԥ��


#define  StyleOfFrameRsp        0xc0
//��λ֡
#define  BDPlaceAskOfFramePlace   0x0000 
//�㲥��λ����
#define  NDPlaceAskOfFramePlace   0x0800 
//�㲥��λ����
#define  PlaceEndOfFramePlace     0x1000 
//��λ��ֹ
#define  BDpacksendOfFramePlace   0x1800 
//�㲥��λ������
#define  NDpacksendOfFramePlace   0x2000 
//�㲥��λ������
#define  AckOfFramePlace          0x2800 
//��λӦ��
#define  AlarmOfFramePlace      0x3000 
//����
#define  ReserveOfFramePlace    0x3800 
//Ԥ��


/************************************************/

#define  SubStyleOfFrame    0x3800
#define  CastSetplace  		0x00
#define  NodeSetplace  		0x01
#define  DataCard0			0x02
#define  DataCard1			0x02
#define  AttrOfRead         0x00
#define  AttrOfWrite        0x01
#define  BatteryVolt        0x07

typedef struct Frame_ctrlArea		//֡����5byte
{
	unsigned int   userCode;		//
	unsigned char  frameCtrl;	//
	unsigned char  frameLens;			//
	unsigned char  pasMima;			//���ܿ���1byte
	unsigned char  text[100];			//֡��(��233 byte)=֡����+������+��������
}*Pframe_ctrlArea, Sframe_ctrlArea;

typedef struct
{
	uint8_t frameHeader[2];         //֡ͷ
	uint8_t  controlType;       //��������
	uint8_t  frameType;         //֡����
	uint8_t  deviceType;        //�豸����
	uint8_t  deviceNumber[6];   //�豸��
	uint8_t  frameLens;          //֡����
}*PFRAME_HEAD_TypeDef, SFRAME_HEAD_TypeDef;
typedef struct
{
	SFRAME_HEAD_TypeDef packageHeader;            //֡ͷ
	uint8_t  rxPayLoad[RF_TX_BUFFER_SIZE];     //������Ч���ݲ���
	uint8_t  txPayLoad[RF_RX_BUFFER_SIZE];     //������Ч���ݲ���
}*PFRAME_DATA_TypeDef, SFRAME_DATA_TypeDef;
#endif    //_BERTHPO_PROTOCOL_H_
