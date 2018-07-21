#include "Drivers_LED.h"

void Drivers_LedOn(LED_GPIO_TypeDef LED_GPIO, uint8_t ledNum)
{
	switch(ledNum)
	{
		case 1:
			LED1_SET(LED_GPIO);
			break;
		case 2:
			LED2_SET(LED_GPIO);
			break;
		default:
			break;
		
	}
}
void Drivers_LedOff(LED_GPIO_TypeDef LED_GPIO, uint8_t ledNum)
{
	switch(ledNum)
	{
		case 1:
			LED1_CLR(LED_GPIO);
			break;
		case 2:
			LED2_CLR(LED_GPIO);
			break;
		default:
			break;
		
	}
}


