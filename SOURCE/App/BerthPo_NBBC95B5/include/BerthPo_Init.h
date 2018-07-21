#ifndef _BERTHPO_INIT_H_
#define _BERTHPO_INIT_H_

#include "Drivers_Common.h"
#include "BerthPo_Common.h"
#include "BSP_GPIO.h"
#include "Drivers_I410ES.h"
#include "Drivers_RM3100.h"
/*****************宏定义***********************/
#define NB_BC95 1
#define BT_ENABLE 1
#define NB_ENABLE 1

#define COAP_MODE 1     //使用COAP模式上传数据到IOT平台
#define UDP_MODE 0   //使用普通模式上 数据到服务器
#if  COAP_MODE
#define DEFAULT_COAP_SERVER_ADDRESS "180.101.147.115:5683"
#endif
#if UDP_MODE
#define DEFAULT_ORIGIN_SERVER_ADDRESS "118.122.93.190:7788"
#endif


#if NB_ENABLE
#define NB_UART_RX_BUF_SIZE  128
#endif

/****************函数声明********************/
void BerthPo_InitMcu();
void BerthPo_InitSysParam();



//end
#endif

