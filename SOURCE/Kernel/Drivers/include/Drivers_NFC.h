#ifndef _DRIVERS_NFC_H_
#define _DRIVERS_NFC_H_

#define NFC_INT_SET       GPIOB->CR2|=GPIO_Pin_0;					//开外部唤醒中断输入
#define NFC_INT_CLR       GPIOB->CR2&=(~GPIO_Pin_0);				//关外部唤醒中断输入

#endif   //_DRIVERS_NFC_H_