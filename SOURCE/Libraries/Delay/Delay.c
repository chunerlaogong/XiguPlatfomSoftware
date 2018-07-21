#include "Delay.h"

void DelayUs(uint8_t us)   //延时us微妙，最小值2us
{
    uint16_t i;
    for(i = 0 ; i < us; i++)
    {
        asm("nop");
        asm("nop");
    }
}
void DelayMs(uint32_t ms)   //延时ms毫秒
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

