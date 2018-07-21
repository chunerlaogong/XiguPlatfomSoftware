#ifndef _BERTHPO_PROTOCOL_H_
#define _BERTHPO_PROTOCOL_H_

#define RF_TX_BUFFER_SIZE    40
#define RF_RX_BUFFER_SIZE    40

#define    TagCmd_RES       0x01 //复位
#define    TagCmd_ID        0x02 //设置
#define    TagCmd_POW       0x03 //设置
#define    TagCmd_CCO       0x04 //设置
#define    TagCmd_CHA       0x05 // 设置
#define    TagCmd_PN        0x06 //设置
#define    TagCmd_TX        0x07 //设置
#define    TagCmd_TIM       0x08 //设置             	
#define    TagCmd_RX        0x09 //设
#define    TagCmd_MOD       0x0a //设置
#define    TagCmd_DAT       0x0d //写
#define    TagCmd_ALM       0x0e //读取
#define    TagCmd_POS       0x0F //
#define    TagCmd_WCM       0x10 //设置标签
#define    TagCmd_CPC       0x11 //
#define    TagCmd_TPC       0x12 //设置
#define    TagCmd_MDC       0x13 //修改
#define    TagCmd_BTM       0x14 //设置
#define    TagCmd_BNK       0x15 //设置
#define    TagCmd_SMS       0x17 //设置
#define    TagCmd_SBT       0x18 //设置
#define    TagCmd_SEN       0x19 //传感器使能
#define    TagCmd_SDT       0x1a //读取传感器
#define    TagCmd_CAS       0x14 //
#define    TagCmd_RPM       0x1d //标签
#define    TagCmd_TRT       0x1e //写标签数据传输速率
#define    TagCmd_IOS       0x1F //操作标签外部IO
#define    TagCmd_THS       0x20
#define    TagCmd_SSD       0x21//读取传感器数值
#define    TagCmd_BRD_pose  0x22 //
#define    TagCmd_HMC_GetB  0x26 //地磁本底操作
#define    TagCmd_INF       0x0b //
#define    TagCmd_GSB       0x30 //地磁

#define    TagCmd_PanDianSucessSleepTime		0x22		//盘点成功睡眠时间
#define    TagCmd_RecFinishLedOn	         0x24	//设置接收数据成功后LED闪烁
#define    TagCmd_CollisionSleepTimeOffSet	         0x25	//冲突睡眠时间偏移设置

#define    memeryMapSize   1024                     

#ifndef TRUE
#define TRUE    0x01
#endif

#ifndef FALSE
#define FALSE   0x00
#endif

#define   ReadAttr   0
#define   WriteAttr  1

//帧类型1byte=(bit7~bit6)信息帧类型+(bit5~bit3)设备类型
//设备类型
#define  Tag_SubFame        (0x00<<3)	
#define  ReadSubFame        (0x01<<3) 	
#define  CoodSubFame        (0x02<<3)	
#define  R_WSubFame         (0x03<<3)	
//信息帧类型
#define  CtrlOfFrame        (0x00<<6)		
#define  DataOfFrame        (0x01<<6)	
#define  PoseOfFrame        (0x02<<6)	
#define  RespOfFrame        (0x03<<6)	


#define  StyleOfFrame      0x00 
//控制帧
#define  AckOfFrame        0x00
//应答
#define  Coordinate        0x10
//基站器
#define  LoadPrg           0x20 
//中继器
#define  ReaderOfFrame     0x18 
//读写器
#define  CallerOfFrame     0x20 
//唤醒器
#define  PositOfFrame      0x28 
//定位器
#define  TgaeOfFrame       0x00 
//标签
#define  ReserveOfFrame    0x38 
//预留

#define  YueduOfFrame     0x08
/***********************************************/

#define  StyleOfFrameData      0x40
//数据帧

//控制帧
#define  AckOfFrame        0x00 
//应答
#define  StationOfFrame    0x08 
//基站器
#define  RelayOfFrame      0x10
//中继器
#define  ReaderOfFrame     0x18
//读写器
#define  LoadPrg           0x20 
//唤醒器
#define  PositOfFrame      0x28 
//定位器
#define  TgaeOfFrame       0x00 
//标签
#define  ReserveOfFrame    0x38
//预留


#define  StyleOfFrameRsp        0xc0
//定位帧
#define  BDPlaceAskOfFramePlace   0x0000 
//广播定位请求
#define  NDPlaceAskOfFramePlace   0x0800 
//点播定位请求
#define  PlaceEndOfFramePlace     0x1000 
//定位终止
#define  BDpacksendOfFramePlace   0x1800 
//广播定位包发送
#define  NDpacksendOfFramePlace   0x2000 
//点播定位包发送
#define  AckOfFramePlace          0x2800 
//定位应答
#define  AlarmOfFramePlace      0x3000 
//报警
#define  ReserveOfFramePlace    0x3800 
//预留


/************************************************/

#define  SubStyleOfFrame    0x3800
#define  CastSetplace  		0x00
#define  NodeSetplace  		0x01
#define  DataCard0			0x02
#define  DataCard1			0x02
#define  AttrOfRead         0x00
#define  AttrOfWrite        0x01
#define  BatteryVolt        0x07

typedef struct Frame_ctrlArea		//帧控制5byte
{
	unsigned int   userCode;		//
	unsigned char  frameCtrl;	//
	unsigned char  frameLens;			//
	unsigned char  pasMima;			//加密控制1byte
	unsigned char  text[100];			//帧体(≤233 byte)=帧属性+命令码+后续参数
}*Pframe_ctrlArea, Sframe_ctrlArea;

typedef struct
{
	uint8_t frameHeader[2];         //帧头
	uint8_t  controlType;       //控制类型
	uint8_t  frameType;         //帧类型
	uint8_t  deviceType;        //设备类型
	uint8_t  deviceNumber[6];   //设备号
	uint8_t  frameLens;          //帧长度
}*PFRAME_HEAD_TypeDef, SFRAME_HEAD_TypeDef;
typedef struct
{
	SFRAME_HEAD_TypeDef packageHeader;            //帧头
	uint8_t  rxPayLoad[RF_TX_BUFFER_SIZE];     //发送有效数据部分
	uint8_t  txPayLoad[RF_RX_BUFFER_SIZE];     //接收有效数据部分
}*PFRAME_DATA_TypeDef, SFRAME_DATA_TypeDef;
#endif    //_BERTHPO_PROTOCOL_H_
