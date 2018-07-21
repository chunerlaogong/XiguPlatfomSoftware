#ifndef _DRIVERS_LED_H_
#define _DRIVERS_LED_H_

#include <stm8l15x.h>

 //LED1开启
#define LED1_SET(LED_GPIO) \
	             LED_GPIO.LED1_GPIO_BASE->DDR |= (LED_GPIO.LED1_GPIO_Pin); \
	             LED_GPIO.LED1_GPIO_BASE->ODR &= (~LED_GPIO.LED1_GPIO_Pin)                                                       
//LED1关闭	                           
#define LED1_CLR(LED_GPIO) \
	             LED_GPIO.LED1_GPIO_BASE->DDR |= (LED_GPIO.LED1_GPIO_Pin); \
	             LED_GPIO.LED1_GPIO_BASE->ODR |= (LED_GPIO.LED1_GPIO_Pin)                                                  
//LED2开启
#define LED2_SET(LED_GPIO) \
	             LED_GPIO.LED2_GPIO_BASE->DDR |= (LED_GPIO.LED2_GPIO_Pin); \
	             LED_GPIO.LED2_GPIO_BASE->ODR &= (~LED_GPIO.LED2_GPIO_Pin)                                                        
//LED2关闭	                           
#define LED2_CLR(LED_GPIO) \
	            LED_GPIO.LED2_GPIO_BASE->DDR |= (LED_GPIO.LED2_GPIO_Pin); \
	            LED_GPIO.LED2_GPIO_BASE->ODR |= (LED_GPIO.LED2_GPIO_Pin)                        
                           

typedef struct
{
    GPIO_TypeDef *LED1_GPIO_BASE;   //LED1电源管脚GPIO
    GPIO_TypeDef *LED2_GPIO_BASE;   //LED2电源管脚GPIO
    uint8_t  LED1_GPIO_Pin;         //LED1管脚Pin
    uint8_t  LED2_GPIO_Pin;
} LED_GPIO_TypeDef;

typedef struct
{
    void (*LedOn)(LED_GPIO_TypeDef LED_GPIO, uint8_t ledNum);      //LED 打开
    void (*LedOff)(LED_GPIO_TypeDef LED_GPIO, uint8_t ledNum);     //LED 关闭
} LED_OPERATION_TypeDef;
void Drivers_LedOn(LED_GPIO_TypeDef LED_GPIO, uint8_t ledNum);
void Drivers_LedOff(LED_GPIO_TypeDef LED_GPIO, uint8_t ledNum);
//提供RM3100对外的接口
static const LED_OPERATION_TypeDef Led_Operation =
{
    .LedOn = Drivers_LedOn,
    .LedOff = Drivers_LedOff,
};
#endif   //_DRIVERS_LED_H_