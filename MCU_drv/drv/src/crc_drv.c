// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : crc_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "crc_drv.h"


#define CRC           CRC1

/*******************************************************************************
* Function Name  : CRC_Config
* Description    : CRC配置设置.
* Input          :  - CRC_InitTypeDef : CRC_InitTypeDef结构体指针，解析如下：
*                    CRC_InitStruct->CRC_Mode:CRC数据模式，取值为CRC_MODE_CRC_x，x取值8,16,32
*                    CRC_InitStruct->CRC_SourceSel: CRC数据源选择，取值CRC_SOURCE_SEL_FROM_DMA、CRC_SOURCE_SEL_FROM_CPU
*                    CRC_InitStruct->CRC_DataFormat：CRC数据格式，大端模式或小端模式；CRC_MODE_CRC_32下，才有效；
*
*
* Output         : None
* Return         : None
*******************************************************************************/
void CRC_Config(CRC_InitTypeDef *CRC_InitStruct)
{
	if(CRC_InitStruct->CRC_Mode == CRC_MODE_CRC_32)
	{
		CRC->CRC_CR = ((CRC_InitStruct->CRC_Mode<<29) \
					  |(CRC_InitStruct->CRC_DataFormat<<26) \
					  |(CRC_InitStruct->CRC_SourceSel<<27));
	}
	else
	{
		CRC->CRC_CR = ((CRC_InitStruct->CRC_Mode<<29) \
					  |(CRC_InitStruct->CRC_SourceSel<<27));
	}

}

/*******************************************************************************
* Function Name  : CRC_SetInitValue
* Description    : 设置CRC初始值
* Input          : - InitValue : 设置的CRC初始值
*
* Output         : None
* Return         : None
*******************************************************************************/
void CRC_SetInitValue(UINT32 InitValue)
{
	CRC->CRC_IDR = InitValue;
	CRC->CRC_CR |= CRC_UPDATA_MASK;
}

/*******************************************************************************
* Function Name  : CRC_CalcCRC32
* Description    : CRC 32 计算
* Input          : - pBuffer : 需要计算CRC的数据缓冲
*                  - BufferLen : 需要计算的CRC数据缓冲的长度
*
* Output         : CRC计算结果
* Return         : None
*******************************************************************************/
UINT32 CRC_CalcCRC32(UINT8 *pBuffer, UINT32 BufferLen)
{
	UINT32 i;
	UINT32 aSize = BufferLen;
	UINT8 *point = pBuffer;

	if(aSize%4 == 0)
	{
		for(i = 0; i < aSize; i = i+4)
		{
			CRC->CRC_DR =(*(point+i)<<24)|(*(point+i+1)<<16)|(*(point+i+2)<<8)|(*(point+i+3));  //大端数据的内存组织方式决定高位数据在低地址
		}
	}
	else if(aSize%4 == 1)
	{
		for(i = 0; i < aSize-1; i = i+4)
		{
			CRC->CRC_DR =(*(point+i)<<24)|(*(point+i+1)<<16)|(*(point+i+2)<<8)|(*(point+i+3));
		}
		CRC->CRC_DR =(*(point+i)<<24);

	}
	else if(aSize%4 == 2)
	{
		for(i=0;i<aSize-2;i=i+4)
		{
			CRC->CRC_DR =(*(point+i)<<24)|(*(point+i+1)<<16)|(*(point+i+2)<<8)|(*(point+i+3));
		}
		CRC->CRC_DR =(*(point+i)<<24)|(*(point+i+1)<<16);
	}
	else
	{
		for(i = 0; i < aSize-3; i = i+4)
		{
			CRC->CRC_DR =(*(point+i)<<24)|(*(point+i+1)<<16)|(*(point+i+2)<<8)|(*(point+i+3));
		}
		CRC->CRC_DR =(*(point+i)<<24)|(*(point+i+1)<<16)|(*(point+i+2)<<8);
	}

	return (CRC->CRC_RR);
}


/*******************************************************************************
* Function Name  : CRC_CalcCRC16
* Description    : CRC 16 计算
* Input          : - pBuffer : 需要计算CRC的数据缓冲
*                  - BufferLen : 需要计算的CRC数据缓冲的长度
*
* Output         : CRC计算结果
* Return         : None
*******************************************************************************/
UINT16 CRC_CalcCRC16(UINT8 *pBuffer, UINT32 BufferLen)
{
	UINT32 i;

	if(BufferLen%2 == 0)
	{
		for(i = 0; i < BufferLen; i = i+2)
		{
			CRC->CRC_DR = (*(pBuffer+i)<<8)|(*(pBuffer+i+1));
		}
	}
	else
	{
		for(i = 0; i < BufferLen-1; i = i+2)
		{
			CRC->CRC_DR =(*(pBuffer+i)<<24)|(*(pBuffer+i+1)<<16);
		}
		CRC->CRC_DR =(*(pBuffer+i)<<24);
	}
	return ((UINT16)(CRC->CRC_RR));
}




/*******************************************************************************
* Function Name  : CRC_CalcCRC8
* Description    : CRC 8 计算
* Input          : - pBuffer : 需要计算CRC的数据缓冲
*                  - BufferLen : 需要计算的CRC数据缓冲的长度
*
* Output         : CRC计算结果
* Return         : None
*******************************************************************************/
UINT8 CRC_CalcCRC8(UINT8 *pBuffer, UINT32 BufferLen)
{
	UINT32 i;

	for(i = 0; i < BufferLen; i++)
	{
		CRC->CRC_DR = *(pBuffer+i);
	}
	return ((UINT8)(CRC->CRC_RR));
}

