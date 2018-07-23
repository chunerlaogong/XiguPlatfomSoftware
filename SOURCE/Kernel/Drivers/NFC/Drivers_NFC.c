#include "Drivers_NFC.h"

NFC_OPERATION_TypeDef NFC_Operation =
{
	.NFC_NFCInit = Drivers_NFCInit,
    .NFC_PowerOn = Drivers_NFCPowerOn,
    .NFC_PowerOff = Drivers_NFCPowerOff,
};

SNFC_GPIO_TypeDef G_NFC_GPIO_CONFIG;

void Drivers_NFCInit(void)
{
	G_NFC_GPIO_CONFIG.NFC_GPIO_BASE = GPIOE;
	G_NFC_GPIO_CONFIG.NFC_GPIO_Pin = GPIO_Pin_4;
	G_NFC_GPIO_CONFIG.NFC_EXIT_Pin = EXTI_Pin_4;
	G_NFC_GPIO_CONFIG.NFC_Mode_type = GPIO_Mode_In_FL_IT;
	G_NFC_GPIO_CONFIG.NFC_Trigger_Type = EXTI_Trigger_Rising;
}
void Drivers_NFCPowerOn(void)
{
	G_NFC_GPIO_CONFIG.NFC_GPIO_BASE->DDR &= (~G_NFC_GPIO_CONFIG.NFC_GPIO_Pin);   //���ѿڣ�PB0)Ϊ���ˣ��ⲿ�����ж�	
        GPIO_Init(G_NFC_GPIO_CONFIG.NFC_GPIO_BASE, G_NFC_GPIO_CONFIG.NFC_GPIO_Pin, G_NFC_GPIO_CONFIG.NFC_Mode_type);   //GPIOB0����Ϊ�����ж�ʹ��
	EXTI_ClearITPendingBit(EXTI_IT_Pin4);
	EXTI_SetPinSensitivity(G_NFC_GPIO_CONFIG.NFC_EXIT_Pin, G_NFC_GPIO_CONFIG.NFC_Trigger_Type);   //�ж�4Ϊ�����ص͵�ƽ����

}
void Drivers_NFCPowerOff(void)
{
	

}
/**
  * @brief External IT PIN4 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI4_IRQHandler, 12)
{  
    EXTI_ClearITPendingBit(EXTI_IT_Pin4); 
    NFC_Operation.NFC_CallBack();
    
}



