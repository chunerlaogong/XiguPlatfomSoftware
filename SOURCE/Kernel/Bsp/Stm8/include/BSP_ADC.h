/*
********************************************************************************
*
*                                 BSP_ADC.h
*
* File          : BSP_ADC.h
* Version       : V1.0
* Author        : yjd
* Mode          : Thumb2
* Toolchain     : 
* Description   : ADC 采集驱动程序
*                
* History       :
* Date          : 2013.10.16
*******************************************************************************/


#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_

#include <stm8l15x.h>



void BSP_ADCOpen(void);
void BSP_ADCClose(void);
void BSP_ADInit(void);
uint16_t BSP_GetVrefValue(void);

#endif


