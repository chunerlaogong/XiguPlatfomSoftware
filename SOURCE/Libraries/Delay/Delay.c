#include "Delay.h"

void DelayUs(uint8_t us)   //��ʱus΢���Сֵ2us
{
    uint16_t i;
    for(i = 0 ; i < us; i++)
    {
        asm("nop");
        asm("nop");
    }
}
void DelayMs(uint32_t ms)   //��ʱms����
{
    uint32_t i;
    int j = 0;
    for(i = 0; i < ms; i++ )
    {
        for (j = 0; j < 70; j++)
        {
            DelayUs(1);
        }
    }
}

