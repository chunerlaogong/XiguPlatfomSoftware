#include "StringOperation.h"


/*******************************************************************************
* Function Name :MemCopy
* Description   :�ַ�����������
* Input         :pFrom:Դ��ַ
			     pTo:Ŀ�ĵ�ַ
			     cLen:��������
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
* Description   :�ַ����ȽϺ���
* Input         :p1:�ַ���1
			     p2:�ַ���2
			     cLen:�Ƚϳ���
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