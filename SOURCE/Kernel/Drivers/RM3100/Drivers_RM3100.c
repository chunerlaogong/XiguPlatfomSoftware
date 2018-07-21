#include "Drivers_RM3100.h"
#include "BSP_I2C.h"
#include "Sleep.h"


/*******************************************************************************
* Function Name :void Drivers_RM3100_PowerOn(PRM3100_GPIO_TypeDef pRM3100_GPIO)
* Description   :��RM3100��Դ
* Author        : YJD
* Input         : RM3100_GPIO_TypeDef RM3100_GPIO
* Output        :
* Other         :
* Date          :2018.06.27
*******************************************************************************/
void Drivers_RM3100_PowerOn(PRM3100_GPIO_TypeDef pRM3100_GPIO)
{
    RM3100_POWER_ON(pRM3100_GPIO);
    Drivers_RM3100_I2CEnable(pRM3100_GPIO);
    RM3100_I2C_SDA_SCL_ON(pRM3100_GPIO->gpioI2cStructer);
	Simulation_I2C.I2cInit(&(pRM3100_GPIO->gpioI2cStructer));
	Drivers_RM3100_I2CEnable(pRM3100_GPIO);
}
/*******************************************************************************
* Function Name :void Drivers_RM3100PowerOff(RM3100_GPIO_TypeDef RM3100_GPIO)
* Description   :�ر�RM3100��Դ
* Author        : YJD
* Input         : RM3100_GPIO_TypeDef RM3100_GPIO
* Output        :
* Other         :
* Date          :2018.06.27
*******************************************************************************/
void Drivers_RM3100_PowerOff(PRM3100_GPIO_TypeDef pRM3100_GPIO)
{
    RM3100_POWER_OFF(pRM3100_GPIO);
	RM3100_I2C_EN_OFF(pRM3100_GPIO);
	RM3100_I2C_SDA_SCL_OFF(pRM3100_GPIO->gpioI2cStructer);
    RM3100_MCURDRY_OFF(pRM3100_GPIO);
}
/*******************************************************************************
* Function Name :void Drivers_RM3100I2CEnable(RM3100_GPIO_TypeDef RM3100_GPIO)
* Description   :RM3100 I2Cʹ��
* Author        : YJD
* Input         : RM3100_GPIO_TypeDef RM3100_GPIO
* Output        :
* Other         :
* Date          :2018.06.27
*******************************************************************************/
void Drivers_RM3100_I2CEnable(PRM3100_GPIO_TypeDef pRM3100_GPIO)
{
    RM3100_I2C_EN_ON(pRM3100_GPIO);
	
}

/*******************************************************************************
* Function Name :void Drivers_RM3100I2CDisable(RM3100_GPIO_TypeDef RM3100_GPIO)
* Description   :RM3100 I2C��ֹ
* Author        : YJD
* Input         : RM3100_GPIO_TypeDef RM3100_GPIO
* Output        :
* Other         :
* Date          :2018.06.27
*******************************************************************************/
void Drivers_RM3100_I2CDisable(PRM3100_GPIO_TypeDef pRM3100_GPIO)
{
    RM3100_I2C_EN_OFF(pRM3100_GPIO);
}


/*******************************************************************************
* Function Name :void Drivers_RM3100SetMeaureMode(int mode)
* Description   :RM3100 ���ò���ģʽ
* Author        : YJD
* Input         : mode  ��Ҫ���õ�ģʽ1-n
* Output        :
* Other         :
* Date          :2018.06.29
*******************************************************************************/
void Drivers_RM3100_SetMeaureMode(PRM3100_GPIO_TypeDef pRM3100_GPIO, uint8_t mode)
{
    uint8_t m_writeData[2] = {0};
    switch(mode)
    {
        case 1:
            m_writeData[0] = 0x70;
            BSP_I2cWriteBytes(0, &(pRM3100_GPIO->gpioI2cStructer), m_writeData, 1);
        	break;
		default:
			break;
    }
}
/*******************************************************************************
* Function Name :void Drivers_RM3100_McuRdRyOn(RM3100_GPIO_TypeDef RM3100_GPIO)
* Description   :RM3100 MUC_RDRY�ж�ʹ��
* Author        : YJD
* Input         : mode  ��Ҫ���õ�ģʽ1-n
* Output        :
* Other         :
* Date          :2018.06.29
*******************************************************************************/
void Drivers_RM3100_McuRdRyOn(PRM3100_GPIO_TypeDef pRM3100_GPIO)
{
	RM3100_MCURDRY_ON(pRM3100_GPIO);
}
/*******************************************************************************
* Function Name :void Drivers_RM3100_McuRdRyOff(RM3100_GPIO_TypeDef RM3100_GPIO)
* Description   :RM3100 MUC_RDRY�жϽ�ֹ
* Author        : YJD
* Input         : mode  ��Ҫ���õ�ģʽ1-n
* Output        :
* Other         :
* Date          :2018.06.29
*******************************************************************************/
void Drivers_RM3100_McuRdRyOff(PRM3100_GPIO_TypeDef pRM3100_GPIO)
{
	RM3100_MCURDRY_ON(pRM3100_GPIO);
}



