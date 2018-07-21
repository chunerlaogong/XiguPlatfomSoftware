#include "BSP_ADC.h"
#include "stm8l15x_conf.h"


void BSP_AacInit(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1,
                              ENABLE);    /* Enable ADC1 clock */
    ADC_DeInit(ADC1);//初始化ADC1
    ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit,
             ADC_Prescaler_2);/* Initialize and configure ADC1 */
    ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels,
                           ADC_SamplingTime_384Cycles);/* ADC channel used for vdd measurement */
    ADC_VrefintCmd(ENABLE);//启用内部参考电压
    ADC_Cmd(ADC1, ENABLE);/* Enable ADC1 */
    ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint,
                   ENABLE); /* Enable ADC1 Channel used for Vref measurement */
}
void BSP_ADInit(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1,
                              ENABLE);    /* Enable ADC1 clock */
    ADC_DeInit(ADC1);//初始化ADC1
    ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit,
             ADC_Prescaler_2);/* Initialize and configure ADC1 */
    ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels,
                           ADC_SamplingTime_384Cycles);/* ADC channel used for AD measurement */
    ADC_Cmd(ADC1, ENABLE);/* Enable ADC1 */
}
uint16_t BSP_GetVrefValue(void)
{
    uint8_t i = 0;
    uint16_t res = 0;
    BSP_AacInit();
    /*8次平均*/
    for(i = 8; i > 0; i--)
    {
        ADC_SoftwareStartConv(ADC1);                       //开始转换
        while(!(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)));   //是否转换结束
        res += ADC_GetConversionValue(ADC1);               //读ADC转换值
    }
    return (res >> 3);
}
