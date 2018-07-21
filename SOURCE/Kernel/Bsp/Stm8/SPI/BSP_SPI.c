#include "BSP_SPI.h"
#include "BSP_Common.h"


void BSP_SpiInit(SPI_TypeDef* SPIx )
{	
	CLK->PCKENR1 |= BITs(4); //SYSCLK to peripheral enabled SPI
	SPIx_CS_set;		// = 1;//PB4=1,CSÖÃ¸ß
	SPIx ->CR2 = 3;
	SPIx ->CR1 = BITs(6) + BITs(2) + BITs(4)  ; // fSYSCLK/4     BITs(X) (1<<(X))  0100 1100
}
uint8_t BSP_SpiSendAndGetByte(SPI_TypeDef* SPIx, uint8_t cD)
{
	SPIx->DR = cD;
	while(!(SPIx->SR & SPI_SR_RXNE));	// == 0);
	return SPIx->DR;
}

void BSP_SpiWriteByte(SPI_TypeDef  * SPIx, uint8_t cAddr, uint8_t cc)
{
	SPIx_CS_clr;		 //= 0;
	BSP_SpiSendAndGetByte(SPIx, cAddr | 0x80);
	BSP_SpiSendAndGetByte(SPIx, cc);
	SPIx_CS_set;		// = 1;
}
void BSP_SpiWirteByteBuff(SPI_TypeDef* SPIx, uint8_t cAddr, uint8_t* pBuff, uint8_t cLen)
{
	uint8_t i;
	SPIx_CS_clr;		// = 0;
	BSP_SpiSendAndGetByte(SPIx, cAddr | 0x80);
	for (i = 0; i<cLen; i++)
	{
		BSP_SpiSendAndGetByte(SPIx, pBuff[i]);
	}
	SPIx_CS_set;		// = 1;
}
void BSP_SpiWirteByteConst(SPI_TypeDef* SPIx, uint8_t cAddr,const uint8_t* pBuff,uint8_t cLen)
{
	uint8_t i;
	SPIx_CS_clr;		// = 0;
	BSP_SpiSendAndGetByte(SPIx, cAddr | 0x80);
	for (i = 0; i<cLen; i++)
	{
		BSP_SpiSendAndGetByte(SPIx, pBuff[i]);
	}
	SPIx_CS_set;		// = 1;
}
void BSP_SpiReadByteBuff(SPI_TypeDef* SPIx, uint8_t cAddr,uint8_t* pBuff,uint8_t cLen)
{
	uint8_t i;
	SPIx_CS_clr;		// = 0;
	BSP_SpiSendAndGetByte(SPIx, cAddr & 0x7F);
	for (i = 0; i<cLen; i++)
	{
		pBuff[i] = BSP_SpiSendAndGetByte(SPIx, 0xFF);
	}
	SPIx_CS_set;		// = 1;
}
uint8_t BSP_SpiReadByte(SPI_TypeDef* SPIx, uint8_t cAddr)
{
	uint8_t cTemp;
	SPIx_CS_clr;		// = 0;PB4
	BSP_SpiSendAndGetByte(SPIx, cAddr & 0x7F);
	cTemp = BSP_SpiSendAndGetByte(SPIx, 0xff);
	SPIx_CS_set;		// = 1;
	return cTemp;
}