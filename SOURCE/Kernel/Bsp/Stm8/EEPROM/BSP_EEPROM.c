#include "BSP_EEPROM.h"
#include "stm8l15x_flash.h"
#include "StringOperation.h"
/************************************************************************************************
Function: void BSP_ReadEepromToBuff(uint8_t* buff,uint8_t cLen, uint8_t cAddr)
Description: 读E2rom测试程序
note：
Calls:          
Called By:      
Table Accessed: 
Table Updated:  
Input:               
Output:         
Return:       
Timers:  2014.02.25    
author:  ME 

************************************************************************************************/
void BerthPo_ReadParamFromEEPROM(uint8_t* buff, uint8_t cLen, uint8_t cAddr)//len:207  cAddr:48
{
	MemCopy((uint8_t*) (EEPROM_START_ADDRESS + cAddr), buff, cLen);
}




/************************************************************************************************
Function:void BSP_WriteBuffToEeprom(uint8_t* buff,uint8_t cLen, uint8_t cAddr)
Description: 读E2rom测试程序
note：
Calls:          
Called By:      
Table Accessed: 
Table Updated:  
Input:               
Output:         
Return:       
Timers:  2015.12.28    
author:  ME 

************************************************************************************************/
void BerthPo_WriteParamToEEPROM(uint8_t* buff,uint8_t cLen, uint8_t cAddr)
{ 
	FLASH->DUKR = MASS_KEY1;	
	FLASH->DUKR = MASS_KEY2;
  
	while(!(FLASH->IAPSR & FLASH_IAPSR_DUL))    asm("nop");	//等待硬件置位	
  
	MemCopy(buff,(uint8_t*) (EEPROM_START_ADDRESS + cAddr), cLen);
  
	FLASH->IAPSR &= ~FLASH_IAPSR_DUL;
}

/*******************************************************************************
* Function Name :void WriteBuffToFlashEepRom(FLASH_MemType_TypeDef FLASH_MemType, uint32_t cAddr, uint8_t *pFromBuff, uint32_t cLen)
* Description   :写数据到EEPROM
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          :2018.05.08
*******************************************************************************/


/*void WriteBuffToFlashEepRom(FLASH_MemType_TypeDef FLASH_MemType, uint32_t cAddr, uint8_t *pFromBuff, uint32_t cLen)
{
	FLASH_Unlock(FLASH_MemType);
	if(FLASH_MemType == FLASH_MemType_Data)
	{
		while(RESET == FLASH_GetFlagStatus(FLASH_FLAG_DUL));	// unlock eeprom
		for(uint32_t i = 0; i < cLen; i++)
		{
			FLASH_ProgramByte(EEPROM_START_ADDRESS + cAddr + i, pFromBuff[i]);
		}
		
	}
	else
	{
		while(RESET == FLASH_GetFlagStatus(FLASH_FLAG_PUL));	// unlock flash
		for(uint32_t i = 0; i < cLen; i++)
		{
			FLASH_ProgramByte(FLASH_START_ADDRESS + cAddr + i, pFromBuff[i]);
		}
	}
	FLASH_Lock(FLASH_MemType);
}*/
/*******************************************************************************
* Function Name :void ReadFlashEepRomToBuff(FLASH_MemType_TypeDef FLASH_MemType, uint32_t cAddr, uint8_t *pToBuff, uint32_t cLen)
* Description   :写数据到EEPROM
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          :2018.05.08
*******************************************************************************/

/*void ReadFlashEepRomToBuff(FLASH_MemType_TypeDef FLASH_MemType, uint32_t cAddr, uint8_t *pToBuff, uint32_t cLen)
{
	if(FLASH_MemType == FLASH_MemType_Program)
	{
		for(uint32_t i = 0; i < cLen; i++)
		{
			pToBuff[i] = FLASH_ReadByte(FLASH_START_ADDRESS + cAddr + i);
		}
	}
	else
	{
		for(uint32_t i = 0; i < cLen; i++)
		{
			pToBuff[i] = FLASH_ReadByte(EEPROM_START_ADDRESS + cAddr + i);
		}
	}
}*/



