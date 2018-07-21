#include "StringOperation.h"


/*******************************************************************************
* Function Name :MemCopy
* Description   :字符串拷贝函数
* Input         :pFrom:源地址
			     pTo:目的地址
			     cLen:拷贝长度
* Output        :
* Author		: yjd
* Other         :
* Date          :2018.05.08
*******************************************************************************/


void MemCopy(uint8_t* pFrom, uint8_t* pTo, uint8_t cLen)
{
	for (uint8_t i=0; i<cLen; i++)
	{
		pTo[i] = pFrom[i];
	}
}

/*******************************************************************************
* Function Name :MemCmp
* Description   :字符串比较函数
* Input         :p1:字符串1
			     p2:字符串2
			     cLen:比较长度
* Output        :
* Author		: yjd
* Other         :
* Date          :2018.05.08
*******************************************************************************/

uint8_t MemCmp(const int8_t* p1,const int8_t* p2,uint16_t nLen)
{
	for (uint16_t i=0; i<nLen; i++)
	{
		if(p1[i] != p2[i])
			return 0;
	}
	return 1;
}
uint8_t CompareString(uint8_t *sorc, uint8_t *dist)
{
   if(    (sorc[0]==dist[0])
        &&(sorc[1]==dist[1])
        &&(sorc[2]==dist[2])
      )
      {
      return 1;
      }
      else
      {
       return 0;
      }


}