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
	uint8_t frameHeader[2];       //帧头
	uint8_t  controlType;       //控制类型
	uint8_t  frameType;         //帧类型
	uint8_t  deviceType;        //设备类型
	uint8_t  deviceNumber[6];   //设备号
	uint8_t  frameLens;          //帧长度
}*PFRAME_HEAD_TypeDef, SFRAME_HEAD_TypeDef;
typedef struct
{
	SFRAME_HEAD_TypeDef packageHeader;         //包头
	uint8_t  rxPayLoad[NB_TX_BUFFER_SIZE];     //发送有效数据部分
	uint8_t  txPayLoad[NB_RX_BUFFER_SIZE];     //接收效数据部分
}*PFRAME_DATA_TypeDef, SFRAME_DATA_TypeDef;
#endif    //_BERTHPO_PROTOCOL_H_