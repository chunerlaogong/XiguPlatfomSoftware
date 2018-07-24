#ifndef _BERTHPO_SLEEP_H_
#define _BERTHPO_SLEEP_H_
#include "BerthPo_RM3100.h"
#include "BerthPo_Common.h"
void BerthPo_Sleep(void);
void BerthPo_DeepSleep(void);
void BerthPo_WakeFromSleep(void);
void BerthPo_NFCCallBack();   //NFC中断回调函数
#endif   //_BERTHPO_SLEEP_H_