#ifndef _BSP_EEPROM_H
#define _BSP_EEPROM_H
#include <stm8l15x.h>

//flash������ر�������
#define MASS_KEY1   0XAE            //д������Կ,ע��FLASH��EEPROM����Կ�෴
#define MASS_KEY2   0X56
#define TAG_PARAM_START_ADDRESS  (0x50)
#define BOOTLOADER_START_ADDRESS  (0x8000)
#define FLASH_START_ADDRESS			(0xD000)
#define EEPROM_START_ADDRESS 		(0x1000)    //main����eem��ʼ��ַ

//flash������ر�������
#define MASS_KEY1   0XAE            //д������Կ,ע��FLASH��EEPROM����Կ�෴
#define MASS_KEY2   0X56
#define TAG_PARAM_START_ADDRESS  (0x50)
#define BOOTLOADER_START_ADDRESS  (0x8000)
void WriteBuffToFlashEepRom(FLASH_MemType_TypeDef FLASH_MemType, uint32_t cAddr, uint8_t *pFromBuff, uint32_t cLen);   
void ReadFlashEepRomToBuff(FLASH_MemType_TypeDef FLASH_MemType, uint32_t cAddr, uint8_t *pToBuff, uint32_t cLen);
void ReadParamFromEEPROM(uint8_t* buff, uint8_t* cLen, uint8_t* cAddr);//len:207  cAddr:48
void WriteParamToEEPROM(uint8_t* buff,uint8_t* cLen, uint8_t* cAddr);
#endif   //_BSP_EEPROM_H