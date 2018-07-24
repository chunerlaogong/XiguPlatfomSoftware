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

#define MAX_(bit1, bit2)  ((bit1) >= (bit2) ? (bit1) : (bit2))// 最大值
#define MIN_(bit1, bit2)  ((bit1) <= (bit2) ? (bit1) : (bit2))// 最小值
#define TYPE_S8(data) (data) > 127 ? 127 : ((data) < -127 ? -127 : (data));//当data>127时，data=127；当-127=<data<=127时，data=data；当data<-127时，data=-127
#define TYPE_U8(data) (data) > 255 ? 255 : ((data) < 0 ? 0 : (data));//当data>255时，data=255；当0=<data<=255时，data=data；当data<0时，data=0 




//枚举车位状态
typedef enum
{
    BERTHPO_PARK_STATE_NULL,    //无车
    BERTHPO_PARK_STATE_HAVE    //有车
} BERTH_STATE;
//枚举工作模式
typedef enum
{
    BERTHPO_MODE_FACTORY = 0xC5,       //产测模式
	BERTHPO_MODE_DEEP_SLEEP = 0xB5,    //深度休眠模式
	BERTHPO_MODE_SELF_TEST = 0xD5,     //开机自检模式
    BERTHPO_MODE_ACTIVE = 0xA5,        //激活正常工作模   
} BERTHPO_MODE;

typedef struct
{
    uint8_t workMode;              //工作模式
    uint8_t initNB;
    uint16_t userCode;
    uint8_t  idNumber[6];     //UID
    uint8_t  keyNumber[6];    //KEY
    uint8_t updateFlag;
    uint8_t version;
    uint16_t wdtInterval;          //WDT睡眠时间
    uint16_t heartbeatInterval;    //心跳时间
    uint16_t alarmValid;
    uint8_t paraIntFlag[3];        //参数同步标志
    uint8_t ledFlag;
    uint8_t getEMBottom_RFFlag;    //地磁开关
    int16_t getEMBottom_RFModThreshold;
    int16_t getEMBottom_RFAngleThreshold;
    int16_t EMDataBottom_x;
    int16_t EMDataBottom_y;
    int16_t EMDataBottom_z;
    uint8_t fastRouse;                //快速唤醒标志
    uint8_t fastRouseAlarm;
    uint8_t serverConfig;
    uint8_t defaultCoapIpAddr[4];     //COAP协议IP
    uint16_t defaultCoapPort;          //COAP协议端口
    uint8_t defaultUdpIpAddr[4];   //原始协议IP
    uint16_t defaultUdpPort[4];     //原始协议端口
}*PNET_MUTUAL_INFO, SNET_MUTUAL_INFO;

typedef struct
{
    uint8_t paraIntFlag;                            //参数初始化标志
    uint8_t wdtInterval;                            //wdt固定睡眠时间，
    uint8_t fastRouseAlarm;                         //多次快速唤醒报警标志
    uint8_t fastRouse;                              //wdt快速唤醒标志
    uint8_t heartbeatInterval;                      //心跳间隔时间
    uint32_t alarmValid;
    uint32_t alarmStatus;
    uint8_t HMCOutFlag;                             //等待地磁操作退出标志，0--退出，1--继续等待
    uint8_t getEMBottomFlag;                        //取本底成功标志
    uint8_t getEMBottom_RFFlag;                     //地磁工作标志
    int32_t getEMBottom_RFModThreshold;             //磁场强度灵敏度
    int32_t getEMBottom_RFAngleThreshold;           //磁场偏转角度灵敏度
    int16_t EMDataBottom_x;                         //x轴强度本底
    int16_t EMDataBottom_y;                         //y轴强度本底
    int16_t EMDataBottom_z;                         //z轴强度本底
    int32_t EMData_xyz_AbsValue_Module_Bottom;      //xyz轴变化模值本底
    int32_t EMData_RMS;
    int32_t EMData_xRMS;
    int32_t EMData_zRMS;
} *PPARAM_CONFIG, SPARAM_CONFIG;
typedef struct
{
    uint8_t  idNumber[6];     //UID
    uint8_t  keyNumber[6];    //KEY
    uint8_t  ledFlag;    //led开关标志，取系统灯配置
    uint8_t  mcuSleepFlag;
}*PNODE_CONFIG, SNODE_CONFIG;
typedef struct
{
    uint8_t initNB;
    uint8_t workMode;                   //工作模式
    SNODE_CONFIG   nodeConfig;          //共7字节
    SPARAM_CONFIG  paramConfig;         //共32字节
} *PCONTROL_CONFIG, SCONTROL_CONFIG;
typedef struct
{
    uint8_t currentParkState;    //当前停车状态，用于比较停车状态是否改变
    uint8_t fastGetParkCount;    //快速获取车辆状态，当这个变量不为零时，为前面车辆刚刚离开，此时快速启动检查防止跟车进车位过快
}*PBERTHPO_PARK_STATUS, SBERTHPO_PARK_STATUS;

typedef struct
{


    uint8_t batteryCheckCount;
    int32_t batteryCount;              //电池计量周期
    uint32_t sendPackCount;
    uint32_t sendNodeCount;            //是否达到发送心跳包计数
    uint8_t  autoLockCount;            //自动锁s定计数，特殊应用
    uint8_t  sendToServerCount;        //发送计数,用做发送数据包顺序区分，应对因网络堵塞造成延迟导致的数据顺序混乱
    uint8_t  sendDataRandomDelay;      //发送数据随机延时
    uint8_t  debugInfoFlag;            //调试信息标志
    uint8_t  outBottomFlag;            //输出本底标志,0--手动校准本底，1--动态本底
    uint8_t  startUpGetBottom;         //系统开机获取本地标志
	uint16_t randomDelay;              //随机延时时间
} *PCONTROL_SYMPLE, SCONTROL_SYMPLE;

/************************函数声明*********************/
void BerthPo_WriteParamToFlash();
void BerthPo_ReadParamFromFlash();
void BerthPo_ProductTestMode();
void BerthPo_MakeHead();
void Hex_to_ASCII(char *pHex, char *pASCII, uint8_t len);
void BerthPo_GetRM3100Data();
uint8_t BerthPo_SetRm3100Base();
void BerthPo_EMDealGeomagneticValue();   //处理地磁数据
uint8_t BerthPo_FixedVCheck();           //固定值判断车位状态

#endif   //_BERTHPO_COMMON_H_
