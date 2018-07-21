#ifndef _STRING_OPERATION_H_
#define _STRING_OPERATION_H_
#include <stm8l15x.h>
uint8_t MemCmp(const int8_t* p1,const int8_t* p2,uint16_t nLen);
void MemCopy(uint8_t* pFrom, uint8_t* pTo, uint8_t cLen);
#endif   //_STRING_OPERATION_H_