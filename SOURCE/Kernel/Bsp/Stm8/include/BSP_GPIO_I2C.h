/*
********************************************************************************
*
*                                 BSP_GPIO_I2C.h
*
* File          : BSP_GPIO_I2C.h
* Version       : V1.0
* Author        : Lhl
* Mode          : Thumb2
* Toolchain     :
* Description   : IIC驱动头文件
*
* History       :
* Date          : 2013.08.12
*******************************************************************************/
#ifndef _BSP_GPIO_I2C_H_
#define _BSP_GPIO_I2C_H_

#include "stm8l15x.h"
#include "stdio.h"
typedef struct GPIO_I2C_struct
{
    GPIO_TypeDef* GPIO_SCL_BASE;        //模拟I2C使用的GPIO分组
    GPIO_TypeDef* GPIO_SDA_BASE;        //模拟I2C使用的GPIO分组
    uint8_t       GPIO_SCL_PIN;         //模拟I2C所有SCL管脚的GPIO Pin
    uint8_t       GPIO_SDA_PIN;         //模拟I2C所有SDA管脚的GPIO Pin
    uint16_t      SLAVE_ADDRESS;
}*PGPIO_I2C_TypeDef,SGPIO_I2C_TypeDef;


struct BSP_I2C_SIMULATION
{
    void (*I2cInit) (PGPIO_I2C_TypeDef pGpioI2cStructer);
    uint8_t (*I2cWriteBytes) (uint8_t Address, PGPIO_I2C_TypeDef pGpioI2cStructer,
                              uint8_t* pData, uint8_t len);
    uint8_t (*I2cReadBytes) (uint8_t Address, PGPIO_I2C_TypeDef pGpioI2cStructer,
                             uint8_t* pData, uint8_t Len);
};

#define I2C_WRITE   0                  //读寄存器
#define I2C_READ    1                  //写寄存器



#define I2C_SCL_SET(GPIO_SCL_BASE, GPIO_SCL_PIN)     (GPIO_SCL_BASE->DDR &=(~GPIO_SCL_PIN))  //配置为输入
#define I2C_SDA_SET(GPIO_SDA_BASE, GPIO_SDA_PIN)     (GPIO_SDA_BASE->DDR &= (~GPIO_SDA_PIN)) //配置为输入
#define I2C_SCL_CLR(GPIO_SCL_BASE, GPIO_SCL_PIN)     (GPIO_SCL_BASE->DDR |= GPIO_SCL_PIN)    //配置为输出
#define I2C_SDA_CLR(GPIO_SDA_BASE, GPIO_SDA_PIN)     (GPIO_SDA_BASE->DDR |= GPIO_SDA_PIN)    //配置为输出
#define I2C_SDA_READ(GPIO_SDA_BASE, GPIO_SDA_PIN)    (GPIO_SDA_BASE->IDR & GPIO_SDA_PIN)     //读数据线输出值    //读数据线输出值
#define I2C_INIT(GPIO_SCL_BASE, GPIO_SCL_PIN,    \
                    GPIO_SDA_BASE, GPIO_SDA_PIN)  \
                                                     (GPIO_SCL_BASE->ODR &= ~GPIO_SCL_PIN); \
                                                     (GPIO_SDA_BASE->ODR &= ~GPIO_SDA_PIN);


void BSP_I2cInit(PGPIO_I2C_TypeDef pGpioI2cStructer);
uint8_t BSP_I2cStart(PGPIO_I2C_TypeDef pGpioI2cStructer);
void BSP_I2cStop(PGPIO_I2C_TypeDef pGpioI2cStructer);
uint8_t BSP_I2cReadByte(uint8_t m_uACK, PGPIO_I2C_TypeDef pGpioI2cStructer);
uint8_t BSP_I2cWriteByte(uint8_t data, PGPIO_I2C_TypeDef pGpioI2cStructer);
void BSP_I2cWrite(uint8_t Address, uint8_t data,
                  PGPIO_I2C_TypeDef pGpioI2cStructer);
uint8_t BSP_I2cRead(uint8_t Address, PGPIO_I2C_TypeDef pGpioI2cStructer);
uint8_t BSP_I2cWriteBytes(uint8_t Address, PGPIO_I2C_TypeDef pGpioI2cStructer,
                          uint8_t* pData, uint8_t len);
uint8_t BSP_I2cReadBytes(uint8_t Address, PGPIO_I2C_TypeDef pGpioI2cStructer,
                         uint8_t* pData, uint8_t Len);
static struct BSP_I2C_SIMULATION Simulation_I2C =
{
    .I2cInit = BSP_I2cInit,
    .I2cWriteBytes = BSP_I2cWriteBytes,
    .I2cReadBytes = BSP_I2cReadBytes,
};

#endif
/******************************************ENDLINE*****************************/
