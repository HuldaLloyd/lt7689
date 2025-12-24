// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : cache_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "cache_drv.h"


/*******************************************************************************
* Function Name  : ICACHE_Init
* Description    : CACHE初始化
* Input          :  - rom: The attributes of rom memory regions；
*                          取值cacheThrough, cacheBack or cacheOff
*                   - boot：The attributes of boot memory regions
*                          取值cacheThrough, cacheBack or cacheOff
*                   - spim1： The attributes of spim1 memory regions
*                          取值cacheThrough, cacheBack or cacheOff
*                   - eflash： The attributes of eflash memory regions
*                          取值cacheThrough, cacheBack or cacheOff
* Output         : None
*
* Return         : None
* Note			  ：上电后执行一次，请勿重复执行。
******************************************************************************/
void ICACHE_Init(cache_com spim1, cache_com eflash, cache_com rom, cache_com boot)
{
	/*spim1 cache configuration*/
	if (cacheOff == spim1)
	{
		IPCCRGS_H &= SPIM1_CACHEOFF;
	}
	else if (cacheThrough == spim1)
	{
		IPCCRGS_H &= SPIM1_CACHEOFF;
		IPCCRGS_H |= (WRITE_THROUGH << SPIM1_CACHE_SHIFT);
	}
	else if (cacheBack == spim1)
	{
		IPCCRGS_H &= SPIM1_CACHEOFF;
		IPCCRGS_H |= (WRITE_BACK << SPIM1_CACHE_SHIFT);
	}
	
	/*rom cache configuration*/
	if (cacheOff == rom)
	{
		IPCCRGS &= ROM_CACHEOFF;
	}
	else if (cacheThrough == rom)
	{
		IPCCRGS &= ROM_CACHEOFF;
		IPCCRGS |= (0x02 << ROM_CACHE_SHIFT);
	}
	else if (cacheBack == rom)
	{
		IPCCRGS &= ROM_CACHEOFF;
		IPCCRGS |= (0x03 << ROM_CACHE_SHIFT);
	}
		
	/*boot cache configuration*/
	if (cacheOff == boot)
	{
		IPCCRGS_H &= BOOT_CACHEOFF;
	}
	else if (cacheThrough == boot)
	{
		IPCCRGS_H &= BOOT_CACHEOFF;
		IPCCRGS_H |= (WRITE_THROUGH << BOOT_CACHE_SHIFT);
	}
	else if (cacheBack == boot)
	{
		IPCCRGS_H &= BOOT_CACHEOFF;
		IPCCRGS_H |= (WRITE_BACK << BOOT_CACHE_SHIFT);
	}
	
	/*eflash cache configuration*/
	if (cacheOff == eflash)
	{
		IPCCRGS_H &= EFLASH_CACHEOFF;
	}
	else if (cacheThrough == eflash)
	{
		IR2HIGHADDR = 0x1fffff;
		IPCCRGS_H &= EFLASH_CACHEOFF;
		IPCCRGS_H |= (EFLASH_WRITE_THROUGH);
	}
	else if (cacheBack == eflash)
	{
		IR2HIGHADDR = 0x1fffff;
		IPCCRGS_H &= EFLASH_CACHEOFF;
		IPCCRGS_H |= (EFLASH_WRITE_BACK);
	}
	
	ICACHE->CACHE_CCR |= (GO|INVW1|INVW0);
	while( ((ICACHE->CACHE_CCR)&(GO)) == GO );
	ICACHE->CACHE_CCR |= ENCACHE;
}

/*******************************************************************************
* Function Name  : DCACHE_Init
* Description    : CACHE初始化
* Input          :  - rom: The attributes of rom memory regions；
*                          取值cacheThrough, cacheBack or cacheOff
*                   - boot：The attributes of boot memory regions
*                          取值cacheThrough, cacheBack or cacheOff
*                   - spim1： The attributes of spim1 memory regions
*                          取值cacheThrough, cacheBack or cacheOff
*                   - eflash： The attributes of eflash memory regions
*                          取值cacheThrough, cacheBack or cacheOff
* Output         : None
*
* Return         : None
* Note			  ：上电后执行一次，请勿重复执行。
******************************************************************************/
void DCACHE_Init(cache_com spim1, cache_com eflash, cache_com rom, cache_com boot)
{
	/*spim1 cache configuration*/
	if (cacheOff == spim1)
	{
		DPCCRGS_H &= SPIM1_CACHEOFF;
	}
	else if (cacheThrough == spim1)
	{
		DPCCRGS_H &= SPIM1_CACHEOFF;
		DPCCRGS_H |= (WRITE_THROUGH << SPIM1_CACHE_SHIFT);
	}
	else if (cacheBack == spim1)
	{
		DPCCRGS_H &= SPIM1_CACHEOFF;
		DPCCRGS_H |= (WRITE_BACK << SPIM1_CACHE_SHIFT);
	}
	
	/*rom cache configuration*/
	if (cacheOff == rom)
	{
		DPCCRGS &= ROM_CACHEOFF;
	}
	else if (cacheThrough == rom)
	{
		DPCCRGS &= ROM_CACHEOFF;
		DPCCRGS |= (0x02 << ROM_CACHE_SHIFT);
	}
	else if (cacheBack == rom)
	{
		DPCCRGS &= ROM_CACHEOFF;
		DPCCRGS |= (0x03 << ROM_CACHE_SHIFT);
	}
		
	/*boot cache configuration*/
	if (cacheOff == boot)
	{
		DPCCRGS_H &= BOOT_CACHEOFF;
	}
	else if (cacheThrough == boot)
	{
		DPCCRGS_H &= BOOT_CACHEOFF;
		DPCCRGS_H |= (WRITE_THROUGH << BOOT_CACHE_SHIFT);
	}
	else if (cacheBack == boot)
	{
		DPCCRGS_H &= BOOT_CACHEOFF;
		DPCCRGS_H |= (WRITE_BACK << BOOT_CACHE_SHIFT);
	}
	
	/*eflash cache configuration*/
	if (cacheOff == eflash)
	{
		DPCCRGS_H &= EFLASH_CACHEOFF;
	}
	else if (cacheThrough == eflash)
	{
		DR2HIGHADDR = 0x1fffff;
		DPCCRGS_H &= EFLASH_CACHEOFF;
		DPCCRGS_H |= (EFLASH_WRITE_THROUGH);
	}
	else if (cacheBack == eflash)
	{
		DR2HIGHADDR = 0x1fffff;
		DPCCRGS_H &= EFLASH_CACHEOFF;
		DPCCRGS_H |= (EFLASH_WRITE_BACK);
	}
	
	DCACHE->CACHE_CCR |= (GO|INVW1|INVW0);
	while( ((DCACHE->CACHE_CCR)&(GO)) == GO );
	DCACHE->CACHE_CCR |= ENCACHE;	
}

