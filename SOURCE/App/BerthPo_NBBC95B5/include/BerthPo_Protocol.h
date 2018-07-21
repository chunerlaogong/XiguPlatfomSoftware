#ifndef _BERTHPO_PROTOCOL_H_
#define _BERTHPO_PROTOCOL_H_

#ifndef TRUE
#define TRUE    0x01
#endif

#ifndef FALSE
#define FALSE   0x00
#endif

#define NB_TX_BUFFER_SIZE 128
#define NB_RX_BUFFER_SIZE 128
typedef struct
{
	uint8_t frameHeader[2];       //֡ͷ
	uint8_t  controlType;       //��������
	uint8_t  frameType;         //֡����
	uint8_t  deviceType;        //�豸����
	uint8_t  deviceNumber[6];   //�豸��
	uint8_t  frameLens;          //֡����
}*PFRAME_HEAD_TypeDef, SFRAME_HEAD_TypeDef;
typedef struct
{
	SFRAME_HEAD_TypeDef packageHeader;         //��ͷ
	uint8_t  rxPayLoad[NB_TX_BUFFER_SIZE];     //������Ч���ݲ���
	uint8_t  txPayLoad[NB_RX_BUFFER_SIZE];     //����Ч���ݲ���
}*PFRAME_DATA_TypeDef, SFRAME_DATA_TypeDef;
#endif    //_BERTHPO_PROTOCOL_H_