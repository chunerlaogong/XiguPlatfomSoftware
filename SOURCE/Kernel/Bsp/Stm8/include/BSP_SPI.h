#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_
#include <stm8l15x.h>

#define SPIx_CS_clr		GPIOB->ODR&=(~GPIO_Pin_4)	    //RF片选清除
#define SPIx_CS_set		GPIOB->ODR|=GPIO_Pin_4          //RF片选选中

uint8_t BSP_SpiReadByte(SPI_TypeDef* SPIx, uint8_t cAddr);
void BSP_SpiReadByteBuff(SPI_TypeDef* SPIx, uint8_t cAddr,uint8_t* pBuff,uint8_t cLen);
void BSP_SpiWirteByteConst(SPI_TypeDef* SPIx, uint8_t cAddr,const uint8_t* pBuff,uint8_t cLen);
void BSP_SpiWirteByteBuff(SPI_TypeDef* SPIx, uint8_t cAddr, uint8_t* pBuff, uint8_t cLen);
void BSP_SpiWriteByte(SPI_TypeDef* SPIx, uint8_t cAddr,uint8_t cc);
uint8_t BSP_SpiSendAndGetByte(SPI_TypeDef* SPIx, uint8_t cD);
void BSP_SpiInit(SPI_TypeDef* SPIx );



#endif
