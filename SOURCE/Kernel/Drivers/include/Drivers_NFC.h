#ifndef _DRIVERS_NFC_H_
#define _DRIVERS_NFC_H_

#define NFC_INT_SET       GPIOB->CR2|=GPIO_Pin_0;					//���ⲿ�����ж�����
#define NFC_INT_CLR       GPIOB->CR2&=(~GPIO_Pin_0);				//���ⲿ�����ж�����

#endif   //_DRIVERS_NFC_H_