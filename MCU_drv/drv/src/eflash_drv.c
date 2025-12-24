// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : eflash_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "common.h"
#include "libEflash.h"
#include "eflash_drv.h"
#include <string.h>

UINT32  eflash_buffer[EFLASH_PAGE_SIZE_BYTE/4];

/*******************************************************************************
* Function Name  : EFLASH_Set_RWSC
* Description    : 设置EFLASH RWSC值
* Input          : -rwsc: rwsc值，默认为0x07
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_Set_RWSC(UINT8 rwsc)
{
	UINT32 efcr_reg = readl(EFCR);
	
	efcr_reg &= (~0x0000000F);
	efcr_reg |= (rwsc & 0x0F);
	
	writel(EFCR,efcr_reg);
}

/*******************************************************************************
* Function Name  : EFLASH_Init
* Description    : EFLASH初始化
* Input          : - f_sys_khz: 系统时钟（单位khz）
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_Init(UINT32 sys_clk_khz)
{
	EFM_Init(sys_clk_khz);
}

/*******************************************************************************
* Function Name  : EFLASH_SetWritePermission
* Description    : 开启EFLASH首块操作
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_SetWritePermission(void)
{
	EFlash_Set_Main_Permission(EFLASH_WRITEABLE, EFLASH_READABLE);
}

/*******************************************************************************
* Function Name  : EFLASH_ClrWritePermission
* Description    : 关闭EFLASH首块操作
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_ClrWritePermission(void)
{
	EFlash_Set_Main_Permission(EFLASH_DISWRITEABLE, EFLASH_READABLE);
}

/*******************************************************************************
* Function Name  : EFLASH_ByteRead
* Description    : 从EFLASH目标地址读取一个byte数据
* Input          : - addr: EFLASH destination address
*
* Output         : None
* Return         : 读取的byte数据
******************************************************************************/
UINT8 EFLASH_ByteRead(UINT32 addr)
{
	return (*(volatile UINT8 *)(addr));
}

/*******************************************************************************
* Function Name  : EFLASH_HalfWordRead
* Description    : 从EFLASH目标地址读取一个half word数据
* Input          : - addr: EFLASH destination address
*
* Output         : None
* Return         : 读取的half word数据
******************************************************************************/
UINT16 EFLASH_HalfWordRead(UINT32 addr)
{
	return (*(volatile UINT16 *)(addr));
}

/*******************************************************************************
* Function Name  : EFLASH_WordRead
* Description    : 从EFLASH目标地址读取一个word数据
* Input          : - addr: EFLASH destination address
*
* Output         : None
* Return         : 读取的word数据
******************************************************************************/
UINT32 EFLASH_WordRead(UINT32 addr)
{
	return (*(volatile UINT32 *)(addr));
}

/*******************************************************************************
* Function Name  : EFLASH_WordsRead
* Description    :  从EFLASH目标地址读取长度为len的word数据
* Input          : - addr: EFLASH destination address
*                  - len:  需要读取word数据的长度
*
* Output         : - des:  读取的word数据缓冲
* Return         : None
******************************************************************************/
void EFLASH_WordsRead(UINT32 addr, UINT32 *des, UINT32 len)
{
	UINT32 i;

	for(i = 0; i < len; i ++)
	{
		des[i] = (*(volatile UINT32 *)(addr+i*4));
	}
}

void EFLASH_WordsRead_8(UINT32 addr, UINT8 *des, UINT32 len)
{
	UINT32 i;

	for(i = 0; i < len; i ++)
	{
		des[i] = (*(volatile UINT8 *)(addr+i));
	}
}
/*******************************************************************************
* Function Name  : EFLASH_PageErase
* Description    : EFLASH页擦除
* Input          : - addr: 要擦除的EFLASH页地址
*
* Output         : None
* Return         :  TRUE：擦除成功   FALSE：擦除失败
******************************************************************************/
UINT8 EFLASH_PageErase(UINT32 addr)
{
	if(EFlash_Page_Erase(addr))
	{
		return FALSE;
	}
	return TRUE;
}

/*******************************************************************************
* Function Name  : EFLASH_WordProg
* Description    : EFLASH的单word编程
* Input          : - addr: 要编程的EFLASH地址
*                  - dat:  编程的word数据
*
* Output         : None
* Return         :  TRUE：编程成功   FALSE：编程失败
******************************************************************************/
UINT8 EFLASH_WordProg(UINT32 addr, UINT32 dat)
{
	if(EFlash_Bulk_Program((addr), 1, &dat))
	{
		return FALSE;
	}
	return TRUE;
}

/*******************************************************************************
* Function Name  : EFLASH_WordsProg
* Description    :  EFLASH的多word编程
* Input          : - addr: 要编程的EFLASH地址
*                  - des:  编程的word数据指针
*                  - len:  要编程的word数据长度
*
* Output         : None
* Return         :  TRUE：编程成功   FALSE：编程失败
******************************************************************************/
UINT8 EFLASH_WordsProg(UINT32 addr, UINT32 *des, UINT32 len)
{
	if(EFlash_Bulk_Program((addr), len, des))
	{
		return FALSE;
	}

	return TRUE;
}

/*******************************************************************************
* Function Name  : FLASH_checkAddr
* Description    :  EFLASH地址检测
* Input          : - flash_addr: 要编程的EFLASH地址
*                  - len:  要编程的byte数据长度
*
* Output         : None
* Return         :  TRUE：编程成功   FALSE：编程失败
******************************************************************************/
static UINT8 FLASH_checkAddr(UINT32 flash_addr,UINT32 len)
{
	if((flash_addr%4) != 0)
	{
		return FALSE;
	}
	if((flash_addr < EFLASH_BASE) || (flash_addr+len) > (EFLASH_BASE + EFLASH_BYTE_SIZE - 0x1000))
	{
		return FALSE;
	}

	return TRUE;
}

/*******************************************************************************
* Function Name  : EFLASH_Write
* Description    :  EFLASH任意地址写数据
* Input          : - flash_addr: 目的地址
*                  - ram_addr:  源地址指针
*                  - len:  数据长度
*
* Output         : None
* Return         :  TRUE：编程成功   FALSE：编程失败
******************************************************************************/
UINT8 EFLASH_Write(UINT32 flash_addr, const UINT8* ram_addr, UINT32 len)
{
	UINT16 offset;
	UINT16 pageleft;
	UINT16 pageright;
	UINT32 dest = flash_addr;
	UINT8* src  = (UINT8*)ram_addr;
	UINT8 result;
	
	pageright = pageright;

	if (FLASH_checkAddr(flash_addr,len) == FALSE)
	{
		return FALSE;
	}

	offset  = ((UINT32)dest & PAGE_MASK);
	pageleft  = EFLASH_PAGE_SIZE_BYTE - offset;
	pageright = ((UINT16)(dest + len) & PAGE_MASK);
	
	/*如果页没有对齐，则先对第一页操作*/
	if (offset != 0)
	{
		dest -= offset;/*dest指针跳转到当前页头*/
		memcpy((UINT8*)eflash_buffer, (UINT8*)dest, EFLASH_PAGE_SIZE_BYTE);
		if (len>pageleft)
		{
			memcpy((UINT8*)eflash_buffer + offset, src, pageleft);
			len  -= pageleft;
			src  += pageleft;
		}
		else
		{
			memcpy((UINT8*)eflash_buffer + offset, src, len);
			len=0;
		}
		result = EFLASH_PageErase(dest);
		result = EFLASH_WordsProg(dest, eflash_buffer, EFLASH_PAGE_SIZE_BYTE/4);
		dest += EFLASH_PAGE_SIZE_BYTE;
		//offset = 0;
	}
	while (len > 0)
	{
		/*如果存在中间页*/
		if (len >= EFLASH_PAGE_SIZE_BYTE)
		{
			memcpy((UINT8*)eflash_buffer, src, EFLASH_PAGE_SIZE_BYTE);
			result = EFLASH_PageErase(dest);
			result = EFLASH_WordsProg(dest, eflash_buffer, EFLASH_PAGE_SIZE_BYTE/4);

			dest += EFLASH_PAGE_SIZE_BYTE;
			src  += EFLASH_PAGE_SIZE_BYTE;
			len  -= EFLASH_PAGE_SIZE_BYTE;
		}
		else
		{   /*做最后页操作*/
			memcpy((UINT8*)eflash_buffer, (UINT8*)dest, EFLASH_PAGE_SIZE_BYTE);
			memcpy((UINT8*)eflash_buffer, src, len);
			result = EFLASH_PageErase(dest);
			result = EFLASH_WordsProg(dest, eflash_buffer, EFLASH_PAGE_SIZE_BYTE/4);
			len = 0;
		}
	}

	if (result == 0)
		return TRUE;
	else
		return FALSE;
}
