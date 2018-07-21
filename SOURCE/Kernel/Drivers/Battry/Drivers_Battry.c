#include "Drivers_Battry.h"
#include "Delay.h"
#include "stm8l15x_conf.h"
//#include "arm_math.h"

uint16_t Drivers_GetBattryVCC()
{
    uint16_t m_VADC = 0;
    uint32_t m_VCC = 0;
    m_VADC = BSP_GetVrefValue();    //�Ȳ�ο���ѹ
    /*ADC ģ��ʹ�������رգ��������1mA ��������*/
    ADC_VrefintCmd(DISABLE);   //�ر��ڲ��ο���ѹ
    ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint,
                   DISABLE);   /* disable ADC1 Channel used for Vref measurement */
    ADC_Cmd(ADC1, DISABLE);    /* disable ADC1 */
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE);
    m_VCC = 487305 / m_VADC;
    m_VCC = (m_VCC > 251) ? (m_VCC - 251) / 2 : 0; //��2.5vΪ��׼�����ֵ����0.02v
    return m_VCC;
}
