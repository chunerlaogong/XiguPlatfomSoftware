#ifndef _DRIVERS_RM3100_H_
#define _DRIVERS_RM3100_H_

#include <stm8l15x.h>
#include "BSP_GPIO_I2C.h"
#include "Drivers_Common.h"

#define MAGNETIC_FACTOR   44         //�ų�ǿ��ת��ϵ��
                                     //��΢��Ϊ��׼��λ����ϵ����Ӧ����              //44--1.7//47--1.6//50--1.5//56--1.33//75--1


#define RM3100_SW_I2C 1   //ʹ�����ģ���I2C


typedef struct
{
    GPIO_TypeDef *MEG_PRW1_GPIO_BASE;   //PRWn��Դ�ܽ�GPIO
    GPIO_TypeDef *MEG_PRW2_GPIO_BASE;
    GPIO_TypeDef *MEG_PRW3_GPIO_BASE;
    uint8_t  MEG_PRW1_GPIO_Pin;         //PRWn��Դ�ܽ�Pin
    uint8_t  MEG_PRW2_GPIO_Pin;
    uint8_t  MEG_PRW3_GPIO_Pin;
    GPIO_TypeDef *MCU_DRDY_GPIO_BASE;   //�ж����ݹܽ�GPIO
    uint8_t       MCU_DRDY_GPIO_Pin;    //�ж����ݹܽ�Pin
#if RM3100_SW_I2C
    SGPIO_I2C_TypeDef gpioI2cStructer;   //I2C�豸
#endif
    GPIO_TypeDef *I2C_EN_GPIO_BASE;     //RM3100 EN�ܽ�GPIO
    uint8_t I2C_EN_GPIO_Pin;            //RM3100 EN�ܽ�Pin
} *PRM3100_GPIO_TypeDef, SRM3100_GPIO_TypeDef ;

typedef struct
{
    void (*RM3100_PowerOn)(PRM3100_GPIO_TypeDef pRM3100_GPIO);      //RM3100 ʹ��
    void (*RM3100_PowerOff)(PRM3100_GPIO_TypeDef pRM3100_GPIO);     //RM3100 �ر�
    void (*RM3100_I2CEnable)(PRM3100_GPIO_TypeDef pRM3100_GPIO);    //RM3100 I2Cʹ��
    void (*RM3100_I2CDisable)(PRM3100_GPIO_TypeDef pRM3100_GPIO);   //RM3100 I2C�ر�
    void (*RM3100_McuRdRyOn)(PRM3100_GPIO_TypeDef pRM3100_GPIO);    //ʹ���ж�����
    void (*RM3100_McuRdRyOff)(PRM3100_GPIO_TypeDef pRM3100_GPIO);   //�ر��ж�����
    void (*RM3100_Wake)(PRM3100_GPIO_TypeDef pRM3100_GPIO);                             //RM3100 ����,ʹ�����IO��
    void (*RM3100_I2CInit)(PGPIO_I2C_TypeDef pGpioI2cStructer);
    uint8_t (*RM3100_ReadBytes)(uint8_t regAddr, PGPIO_I2C_TypeDef pGpioI2cStructer,
            uint8_t* readData, uint8_t readLen);
    uint8_t (*RM3100_WriteBytes)(uint8_t regAddr, PGPIO_I2C_TypeDef pGpioI2cStructer,
                                 uint8_t* writeData, uint8_t writeLen);
    void (*RM3100_SetMeaureMode)(PRM3100_GPIO_TypeDef pRM3100_GPIO , uint8_t mode);   //1280��������
	uint8_t (*RM3100_TestMode)(PRM3100_GPIO_TypeDef pRM3100_GPIO);   //����ģʽ
} RM3100_OPERATION_TypeDef;


//I2CEN��(PA7)Ϊ���,����
#define RM3100_I2C_EN_ON(pRM3100_GPIO)\
                     pRM3100_GPIO->I2C_EN_GPIO_BASE->DDR |=  pRM3100_GPIO->I2C_EN_GPIO_Pin;  \
                     pRM3100_GPIO->I2C_EN_GPIO_BASE->CR1 |=  pRM3100_GPIO->I2C_EN_GPIO_Pin;  \
                     pRM3100_GPIO->I2C_EN_GPIO_BASE->ODR |=  pRM3100_GPIO->I2C_EN_GPIO_Pin
//I2CEN��(PA7)Ϊ����
#define RM3100_I2C_EN_OFF(pRM3100_GPIO)\
                    pRM3100_GPIO->I2C_EN_GPIO_BASE->DDR |=  pRM3100_GPIO->I2C_EN_GPIO_Pin;  \
                    pRM3100_GPIO->I2C_EN_GPIO_BASE->ODR &=  ~pRM3100_GPIO->I2C_EN_GPIO_Pin
#if RM3100_SW_I2C
//SDA-0(PC0)Ϊ��� SCL-0��PC1��Ϊ�����
#define RM3100_I2C_SDA_SCL_ON(gpioI2cStructer)\
                      gpioI2cStructer.GPIO_SCL_BASE->DDR |=  gpioI2cStructer.GPIO_SCL_PIN; \
                      gpioI2cStructer.GPIO_SDA_BASE->DDR |=  gpioI2cStructer.GPIO_SDA_PIN
//SDA-0(PC0)Ϊ���� SCL-0��PC1�����������
#define RM3100_I2C_SDA_SCL_OFF(gpioI2cStructer)\
                       gpioI2cStructer.GPIO_SCL_BASE->DDR |=  gpioI2cStructer.GPIO_SCL_PIN; \
                       gpioI2cStructer.GPIO_SDA_BASE->DDR |=  gpioI2cStructer.GPIO_SDA_PIN; \
                       gpioI2cStructer.GPIO_SCL_BASE->ODR |=  ~gpioI2cStructer.GPIO_SCL_PIN; \
                       gpioI2cStructer.GPIO_SDA_BASE->ODR |=  ~gpioI2cStructer.GPIO_SDA_PIN; 

                       
                       
#endif

#define RM3100_MCURDRY_ON(pRM3100_GPIO)\
                        pRM3100_GPIO->MCU_DRDY_GPIO_BASE->DDR |= pRM3100_GPIO->MCU_DRDY_GPIO_Pin;\
                        pRM3100_GPIO->MCU_DRDY_GPIO_BASE->ODR |= pRM3100_GPIO->MCU_DRDY_GPIO_Pin
#define RM3100_MCURDRY_OFF(pRM3100_GPIO)\
                        pRM3100_GPIO->MCU_DRDY_GPIO_BASE->DDR |= pRM3100_GPIO->MCU_DRDY_GPIO_Pin;\
                        pRM3100_GPIO->MCU_DRDY_GPIO_BASE->ODR &= ~pRM3100_GPIO->MCU_DRDY_GPIO_Pin

//MEG_PRWn,RM3100������ߵش�оƬ�����,���  ; MCU��RM3100������ߣ���Ϊ�������
#define RM3100_POWER_ON(pRM3100_GPIO) \
                        pRM3100_GPIO->MEG_PRW1_GPIO_BASE->DDR |=  pRM3100_GPIO->MEG_PRW1_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW2_GPIO_BASE->DDR |=  pRM3100_GPIO->MEG_PRW2_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW3_GPIO_BASE->DDR |=  pRM3100_GPIO->MEG_PRW3_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW1_GPIO_BASE->ODR |=  pRM3100_GPIO->MEG_PRW1_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW2_GPIO_BASE->ODR |=  pRM3100_GPIO->MEG_PRW2_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW3_GPIO_BASE->ODR |=  pRM3100_GPIO->MEG_PRW3_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW1_GPIO_BASE->CR1 |=  pRM3100_GPIO->MEG_PRW1_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW2_GPIO_BASE->CR1 |=  pRM3100_GPIO->MEG_PRW2_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW3_GPIO_BASE->CR1 |=  pRM3100_GPIO->MEG_PRW3_GPIO_Pin
//MEG_PRWn,RM3100������ߵش�оƬ�����,���� ; MCU��RM3100������ߣ���Ϊ����
#define RM3100_POWER_OFF(pRM3100_GPIO) \
                        pRM3100_GPIO->MEG_PRW1_GPIO_BASE->DDR |=  pRM3100_GPIO->MEG_PRW1_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW2_GPIO_BASE->DDR |=  pRM3100_GPIO->MEG_PRW2_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW3_GPIO_BASE->DDR |=  pRM3100_GPIO->MEG_PRW3_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW1_GPIO_BASE->ODR &=  ~pRM3100_GPIO->MEG_PRW1_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW2_GPIO_BASE->ODR &=  ~pRM3100_GPIO->MEG_PRW2_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW3_GPIO_BASE->ODR &=  ~pRM3100_GPIO->MEG_PRW3_GPIO_Pin; \


/***********��������*****************/
void Drivers_RM3100_PowerOn(PRM3100_GPIO_TypeDef pRM3100_GPIO);
void Drivers_RM3100_PowerOff(PRM3100_GPIO_TypeDef pRM3100_GPIO);
void Drivers_RM3100_I2CEnable(PRM3100_GPIO_TypeDef pRM3100_GPIO);
void Drivers_RM3100_I2CDisable(PRM3100_GPIO_TypeDef pRM3100_GPIO);
void Drivers_RM3100_SetMeaureMode(PRM3100_GPIO_TypeDef pRM3100_GPIO, uint8_t mode);
void Drivers_RM3100_McuRdRyOn(PRM3100_GPIO_TypeDef pRM3100_GPIO);
void Drivers_RM3100_McuRdRyOff(PRM3100_GPIO_TypeDef pRM3100_GPIO);
uint8_t Drivers_RM3100_TestMode(PRM3100_GPIO_TypeDef pRM3100_GPIO);

//�ṩRM3100����Ľӿ�
static const RM3100_OPERATION_TypeDef RM3100_Operation =
{
    .RM3100_PowerOn = Drivers_RM3100_PowerOn,
    .RM3100_PowerOff = Drivers_RM3100_PowerOff,
    .RM3100_McuRdRyOn = Drivers_RM3100_McuRdRyOn,
    .RM3100_McuRdRyOff = Drivers_RM3100_McuRdRyOff,
    .RM3100_I2CInit =  BSP_I2cInit,
    .RM3100_ReadBytes = BSP_I2cReadBytes,
    .RM3100_WriteBytes = BSP_I2cWriteBytes,
    .RM3100_I2CEnable = Drivers_RM3100_I2CEnable,
    .RM3100_I2CDisable = Drivers_RM3100_I2CDisable,
    .RM3100_SetMeaureMode = Drivers_RM3100_SetMeaureMode,
    .RM3100_TestMode = Drivers_RM3100_TestMode,
};
#endif
