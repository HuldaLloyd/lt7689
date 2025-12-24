// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : cache_reg.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __CACHE_REG_H__
#define __CACHE_REG_H__

#include "type.h"
#include "memmap.h"

typedef struct
{
	__IO UINT32 CACHE_CCR;//0x00
	__IO UINT32 CACHE_CLCR;//0x04
	__IO UINT32 CACHE_CSAR;//0x08
	__IO UINT32 CACHE_CCVR;//0x0c
	__IO UINT32 RESERVED1[4];//0x10~0x1C
	__IO UINT32 CACHE_ACRG;//0x20

}CACHE_TypeDef;

#define IPCCCR  *(volatile UINT32*)(CACHE_BASE_ADDR +0x0)
#define IPCCLCR *(volatile UINT32*)(CACHE_BASE_ADDR +0x4)
#define IPCCSAR *(volatile UINT32*)(CACHE_BASE_ADDR +0x8)
#define IPCCCVR *(volatile UINT32*)(CACHE_BASE_ADDR +0xc)
#define IPCCSRR *(volatile UINT32*)(CACHE_BASE_ADDR +0x10)

#define IPCCRGS *(volatile UINT32*)(CACHE_BASE_ADDR +0x20)
#define IPCCRGS_H *(volatile UINT32*)(CACHE_BASE_ADDR +0x24)
	
#define IR2HIGHADDR		*(volatile UINT32 *)(CACHE_BASE_ADDR + 0x80)

#define IPCRINVPAGEADDR	*(volatile UINT32 *)(CACHE_BASE_ADDR + 0x180)
#define IPCRINVPAGESIZE	*(volatile UINT32 *)(CACHE_BASE_ADDR + 0x184)

#define IPSCCR  *(volatile UINT32*)(CACHE_BASE_ADDR +0x800)
#define IPSCLCR *(volatile UINT32*)(CACHE_BASE_ADDR +0x804)
#define IPSCSAR *(volatile UINT32*)(CACHE_BASE_ADDR +0x808)
#define IPSCCVR *(volatile UINT32*)(CACHE_BASE_ADDR +0x80c)
#define IPSCSRR *(volatile UINT32*)(CACHE_BASE_ADDR +0x810)

#define IPSCRGS *(volatile UINT32*)(CACHE_BASE_ADDR +0x820)
	
#define DPCCCR  *(volatile UINT32*)(CACHE2_BASE_ADDR +0x0)
#define DPCCLCR *(volatile UINT32*)(CACHE2_BASE_ADDR +0x4)
#define DPCCSAR *(volatile UINT32*)(CACHE2_BASE_ADDR +0x8)
#define DPCCCVR *(volatile UINT32*)(CACHE2_BASE_ADDR +0xc)
#define DPCCSRR *(volatile UINT32*)(CACHE2_BASE_ADDR +0x10)

#define DPCCRGS *(volatile UINT32*)(CACHE2_BASE_ADDR +0x20)
#define DPCCRGS_H *(volatile UINT32*)(CACHE2_BASE_ADDR +0x24)
	
#define DR2HIGHADDR		*(volatile UINT32 *)(CACHE2_BASE_ADDR + 0x80)

#define DPCRINVPAGEADDR	*(volatile UINT32 *)(CACHE2_BASE_ADDR + 0x180)
#define DPCRINVPAGESIZE	*(volatile UINT32 *)(CACHE2_BASE_ADDR + 0x184)

#define DPSCCR  *(volatile UINT32*)(CACHE2_BASE_ADDR +0x800)
#define DPSCLCR *(volatile UINT32*)(CACHE2_BASE_ADDR +0x804)
#define DPSCSAR *(volatile UINT32*)(CACHE2_BASE_ADDR +0x808)
#define DPSCCVR *(volatile UINT32*)(CACHE2_BASE_ADDR +0x80c)
#define DPSCSRR *(volatile UINT32*)(CACHE2_BASE_ADDR +0x810)

#define DPSCRGS *(volatile UINT32*)(CACHE2_BASE_ADDR +0x820)


#define PAGE_CACHE_CLEAN_GO		0x00000001



#define INVW1   ((1)<<26)
#define INVW0   ((1)<<24)
#define PUSHW0  ((1)<<25)
#define PUSHW1  ((1)<<27)
#define GO      (((UINT32)1)<<31)
#define ENWRBUF ((1)<<1)
#define ENCACHE ((1)<<0)

#define LGO     (0x01)

#define CACHE_LINE_SIZE  0x10
#define CACHE_LINE_MASK  0x0f

#define R0_WT_WB				((1)<<0)
#define R0_ENCACHE				((1)<<1)
#define R2_WT_WB				((1)<<4)
#define R2_ENCACHE				((1)<<5)
#define RAM0_WT_WB				((1)<<12)
#define RAM0_ENCACHE			((1)<<13)

#define WRITE_BACK				(0xFF)
#define WRITE_THROUGH			(0xaa)

#define EFLASH_WRITE_BACK			(0xff)
#define EFLASH_WRITE_THROUGH	(0xaa)

#define BOOT_CACHEOFF			0xff00ffff
#define ROM_CACHEOFF			0xfffcffff
//#define SPIM1_CACHEOFF		0xffffff3f
#define SPIM1_CACHEOFF		0xffff00ff
#define EFLASH_CACHEOFF		0xffffff00

#define BOOT_CACHE_SHIFT	(16)
#define ROM_CACHE_SHIFT		(16)
#define SPIM1_CACHE_SHIFT	(8)

#endif /* __CACHE_REG_H__ */
