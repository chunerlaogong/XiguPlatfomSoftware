#ifndef _SIM7600C_H_
#define _SIM7600C_H_

#define NO_BACK               0
#define BACK_OK               1
#define BACK_ERROR            2
#define BACK_TIMEOUT          3

typedef struct    //AT指令相关信息定义
{
	int8_t *cmd;            //发送的AT指令
	int8_t timeOut;         //超时时间
	int8_t *backOkVal;      //指令正确返回值
	int8_t *backResult;     //指令执行返回值
	int8_t retryTimesMax;   //最大发尝试送次数
}SCMD_INFO_DEF, PCMD_INFO_DEF;
typedef struct   //服务器相关信息定义
{
  int8_t  netType;         //网络类型:udp/tcp
  int8_t  serverIp[15];    //服务器IP
  int8_t  serverPort[5];   //服务器端口
}SNET_INFO_DEF, *PNET_INFO_DEF;
typedef struct  //SIM7600C信息相关定义
{
  int8_t signalStrength;   //信号强度
  int8_t currentCmdNum;    //当前指令序号
  int16_t cmdExecCnt;      //指令执行计数
  int8_t rxTemp[64];       //接收数据临时缓冲区
  int8_t rxBuffer[64];     //接收数据缓冲区
  int8_t rcvPointer;       //接收数据计数指针
  int8_t txTemp[64];       //发送数据临时缓冲区
  int8_t txBuffer[64];     //发送数据缓冲区
}SSIM7600C_INFO_DEF, *PSIM7600C_INFO_DEF;
#endif   //_SIM7600C_H_