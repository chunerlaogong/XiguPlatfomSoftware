#include "Drivers_Battry.h"
#include "Delay.h"
#include "stm8l15x_conf.h"
//#include "arm_math.h"

uint16_t Drivers_GetBattryVCC()
{
    uint16_t m_VADC = 0;
    uint32_t m_VCC = 0;
    m_VADC = BSP_GetVrefValue();    //先测参考电压
    /*ADC 模块使用完必须关闭，否则会有1mA 电流消耗*/
    ADC_VrefintCmd(DISABLE);   //关闭内部参考电压
    ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint,
                   DISABLE);   /* disable ADC1 Channel used for Vref measurement */
    ADC_Cmd(ADC1, DISABLE);    /* disable ADC1 */
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE);
    m_VCC = 487305 / m_VADC;
    m_VCC = (m_VCC > 251) ? (m_VCC - 251) / 2 : 0; //以2.5v为基准，输出值步进0.02v
    return m_VCC;
}

