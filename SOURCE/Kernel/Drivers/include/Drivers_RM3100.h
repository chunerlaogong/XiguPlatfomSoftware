#ifndef _DRIVERS_RM3100_H_
#define _DRIVERS_RM3100_H_

#include <stm8l15x.h>
#include "BSP_GPIO_I2C.h"
#include "Drivers_Common.h"

#define MAGNETIC_FACTOR   44         //磁场强度转换系数
                                     //以微特为标准单位，该系数对应倍数              //44--1.7//47--1.6//50--1.5//56--1.33//75--1


#define RM3100_SW_I2C 1   //使用软件模拟的I2C


typedef struct
{
    GPIO_TypeDef *MEG_PRW1_GPIO_BASE;   //PRWn电源管脚GPIO
    GPIO_TypeDef *MEG_PRW2_GPIO_BASE;
    GPIO_TypeDef *MEG_PRW3_GPIO_BASE;
    uint8_t  MEG_PRW1_GPIO_Pin;         //PRWn电源管脚Pin
    uint8_t  MEG_PRW2_GPIO_Pin;
    uint8_t  MEG_PRW3_GPIO_Pin;
    GPIO_TypeDef *MCU_DRDY_GPIO_BASE;   //中断数据管脚GPIO
    uint8_t       MCU_DRDY_GPIO_Pin;    //中断数据管脚Pin
#if RM3100_SW_I2C
    SGPIO_I2C_TypeDef gpioI2cStructer;   //I2C设备
#endif
    GPIO_TypeDef *I2C_EN_GPIO_BASE;     //RM3100 EN管脚GPIO
    uint8_t I2C_EN_GPIO_Pin;            //RM3100 EN管脚Pin
} *PRM3100_GPIO_TypeDef, SRM3100_GPIO_TypeDef ;

typedef struct
{
    void (*RM3100_PowerOn)(PRM3100_GPIO_TypeDef pRM3100_GPIO);      //RM3100 使能
    void (*RM3100_PowerOff)(PRM3100_GPIO_TypeDef pRM3100_GPIO);     //RM3100 关闭
    void (*RM3100_I2CEnable)(PRM3100_GPIO_TypeDef pRM3100_GPIO);    //RM3100 I2C使能
    void (*RM3100_I2CDisable)(PRM3100_GPIO_TypeDef pRM3100_GPIO);   //RM3100 I2C关闭
    void (*RM3100_McuRdRyOn)(PRM3100_GPIO_TypeDef pRM3100_GPIO);    //使能中断引脚
    void (*RM3100_McuRdRyOff)(PRM3100_GPIO_TypeDef pRM3100_GPIO);   //关闭中断引脚
    void (*RM3100_Wake)(PRM3100_GPIO_TypeDef pRM3100_GPIO);                             //RM3100 唤醒,使能相关IO口
    void (*RM3100_I2CInit)(PGPIO_I2C_TypeDef pGpioI2cStructer);
    uint8_t (*RM3100_ReadBytes)(uint8_t regAddr, PGPIO_I2C_TypeDef pGpioI2cStructer,
            uint8_t* readData, uint8_t readLen);
    uint8_t (*RM3100_WriteBytes)(uint8_t regAddr, PGPIO_I2C_TypeDef pGpioI2cStructer,
                                 uint8_t* writeData, uint8_t writeLen);
    void (*RM3100_SetMeaureMode)(PRM3100_GPIO_TypeDef pRM3100_GPIO , uint8_t mode);   //1280参数设置
	uint8_t (*RM3100_TestMode)(PRM3100_GPIO_TypeDef pRM3100_GPIO);   //测试模式
} RM3100_OPERATION_TypeDef;


//I2CEN脚(PA7)为输出,拉高
#define RM3100_I2C_EN_ON(pRM3100_GPIO)\
                     pRM3100_GPIO->I2C_EN_GPIO_BASE->DDR |=  pRM3100_GPIO->I2C_EN_GPIO_Pin;  \
                     pRM3100_GPIO->I2C_EN_GPIO_BASE->CR1 |=  pRM3100_GPIO->I2C_EN_GPIO_Pin;  \
                     pRM3100_GPIO->I2C_EN_GPIO_BASE->ODR |=  pRM3100_GPIO->I2C_EN_GPIO_Pin
//I2CEN脚(PA7)为输入
#define RM3100_I2C_EN_OFF(pRM3100_GPIO)\
                    pRM3100_GPIO->I2C_EN_GPIO_BASE->DDR |=  pRM3100_GPIO->I2C_EN_GPIO_Pin;  \
                    pRM3100_GPIO->I2C_EN_GPIO_BASE->ODR &=  ~pRM3100_GPIO->I2C_EN_GPIO_Pin
#if RM3100_SW_I2C
//SDA-0(PC0)为输出 SCL-0（PC1）为输出高
#define RM3100_I2C_SDA_SCL_ON(gpioI2cStructer)\
                      gpioI2cStructer.GPIO_SCL_BASE->DDR |=  gpioI2cStructer.GPIO_SCL_PIN; \
                      gpioI2cStructer.GPIO_SDA_BASE->DDR |=  gpioI2cStructer.GPIO_SDA_PIN
//SDA-0(PC0)为输入 SCL-0（PC1）推挽输出低
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

//MEG_PRWn,RM3100供电口线地磁芯片供电口,输出  ; MCU给RM3100供电口线，设为推免输出
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
//MEG_PRWn,RM3100供电口线地磁芯片供电口,输入 ; MCU给RM3100供电口线，设为输入
#define RM3100_POWER_OFF(pRM3100_GPIO) \
                        pRM3100_GPIO->MEG_PRW1_GPIO_BASE->DDR |=  pRM3100_GPIO->MEG_PRW1_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW2_GPIO_BASE->DDR |=  pRM3100_GPIO->MEG_PRW2_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW3_GPIO_BASE->DDR |=  pRM3100_GPIO->MEG_PRW3_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW1_GPIO_BASE->ODR &=  ~pRM3100_GPIO->MEG_PRW1_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW2_GPIO_BASE->ODR &=  ~pRM3100_GPIO->MEG_PRW2_GPIO_Pin; \
                        pRM3100_GPIO->MEG_PRW3_GPIO_BASE->ODR &=  ~pRM3100_GPIO->MEG_PRW3_GPIO_Pin; \


/***********函数声明*****************/
void Drivers_RM3100_PowerOn(PRM3100_GPIO_TypeDef pRM3100_GPIO);
void Drivers_RM3100_PowerOff(PRM3100_GPIO_TypeDef pRM3100_GPIO);
void Drivers_RM3100_I2CEnable(PRM3100_GPIO_TypeDef pRM3100_GPIO);
void Drivers_RM3100_I2CDisable(PRM3100_GPIO_TypeDef pRM3100_GPIO);
void Drivers_RM3100_SetMeaureMode(PRM3100_GPIO_TypeDef pRM3100_GPIO, uint8_t mode);
void Drivers_RM3100_McuRdRyOn(PRM3100_GPIO_TypeDef pRM3100_GPIO);
void Drivers_RM3100_McuRdRyOff(PRM3100_GPIO_TypeDef pRM3100_GPIO);
uint8_t Drivers_RM3100_TestMode(PRM3100_GPIO_TypeDef pRM3100_GPIO);

//提供RM3100对外的接口
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
