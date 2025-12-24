// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : common.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef __COMMON_H__
#define __COMMON_H__
#include "type.h"
#include "uart.h"

#define writel(addr,value) (*(volatile unsigned long *)(addr))=((unsigned long)(value))
#define writew(addr,value) (*(volatile unsigned short *)(addr))=((unsigned short)(value))
#define writeb(addr,value) (*(volatile unsigned char *)(addr))=((unsigned char)(value))
#define readl(addr)		(*(volatile unsigned long *)(addr))
#define readw(addr)		(*(volatile unsigned short *)(addr))
#define readb(addr)		(*(volatile unsigned char *)(addr))

#define Disable_Interrupts		__asm("CPSID I")
#define Enable_Interrupts		__asm("CPSIE I")

extern UINT32 enter_critical_sr_save(void);
extern void exit_critical_sr_restore(UINT32 primask);
extern UINT32 Read_VEC(void);

void assert_failed(UINT8* file, UINT32 line); 
#define assert_param(expr) ((expr) ? (void)0 : assert_failed((UINT8 *)__FILE__, __LINE__))


#endif /* __COMMON_H__ */
