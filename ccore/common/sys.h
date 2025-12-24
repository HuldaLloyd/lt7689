#ifndef __SYS_H__
#define __SYS_H__	 

#include "LT7689_C.h" 
#include "type.h"
#include "common.h"

//0,不支持OS
//1,支持OS
#define SYSTEM_SUPPORT_OS		0		//定义系统文件夹是否支持OS
///////////////////////////////////////////////////////////////////////////////////

//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 


extern UINT32 g_sys_clk;	//系统时钟全局变量
extern UINT32 g_ips_clk;
extern UINT32 g_trim_clk;

////////////////////////////////////////////////////////////////////////////////// 

extern void Sys_Soft_Reset(void);      							//系统软复位
extern void Sys_Standby(void);         							//待机模式 	
extern void NVIC_SetVectorTable(UINT32 NVIC_VectTab, UINT32 Offset);	//设置偏移地址
extern void NVIC_PriorityGroupConfig(UINT8 NVIC_Group);			//设置NVIC分组
extern void NVIC_Init(UINT8 NVIC_PreemptionPriority,UINT8 NVIC_SubPriority,UINT8 NVIC_Channel,UINT8 NVIC_Group);//设置中断
extern void Ex_NVIC_Config(UINT8 GPIOx,UINT8 BITx,UINT8 TRIM);				//外部中断配置函数(只对GPIOA~I)
extern void Sys_Init(void);

//以下为汇编函数
extern void WFI_SET(void);		//执行WFI指令
extern void INTX_DISABLE(void);//关闭所有中断
extern void INTX_ENABLE(void);	//开启所有中断
extern void MSR_MSP(UINT32 addr);	//设置堆栈地址 
#endif		/*__SYS_H__*/











