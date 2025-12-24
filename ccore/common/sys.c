#include "sys.h"  
#include "type.h"
#include "cpm_drv.h"
#include "tc_drv.h"
#include "delay.h"
#include "libRegOpt.h"
#include "clk_switch_drv.h"
#include "cache_drv.h"

UINT32 g_sys_clk;//系统时钟全局变量
UINT32 g_ips_clk;
UINT32 g_trim_clk = OSC_160M_HZ;

//设置向量表偏移地址
//NVIC_VectTab:基址
//Offset:偏移量		 
void NVIC_SetVectorTable(UINT32 NVIC_VectTab,UINT32 Offset)	 
{ 	   	  
	SCB->VTOR=NVIC_VectTab|(Offset&(UINT32)0xFFFFFE00);//设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留。
}
//设置NVIC分组
//NVIC_Group:NVIC分组 0~4 总共5组 		   
void NVIC_PriorityGroupConfig(UINT8 NVIC_Group)	 
{ 
	UINT32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//取后三位
	temp1<<=8;
	temp=SCB->AIRCR;  //读取先前的设置
	temp&=0X0000F8FF; //清空先前分组
	temp|=0X05FA0000; //写入钥匙
	temp|=temp1;	   
	SCB->AIRCR=temp;  //设置分组	    	  				   
}
//设置NVIC 
//NVIC_PreemptionPriority:抢占优先级
//NVIC_SubPriority       :响应优先级
//NVIC_Channel           :中断编号
//NVIC_Group             :中断分组 0~3
//注意优先级不能超过设定的组的范围!否则会有意想不到的错误
//组划分:
//组0:0位抢占优先级,3位响应优先级
//组1:1位抢占优先级,2位响应优先级
//组2:2位抢占优先级,1位响应优先级
//组3:3位抢占优先级,0位响应优先级

//NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先	   
void NVIC_Init(UINT8 NVIC_PreemptionPriority,UINT8 NVIC_SubPriority,UINT8 NVIC_Channel,UINT8 NVIC_Group)	 
{ 
	UINT32 temp;	  
	NVIC_PriorityGroupConfig(NVIC_Group);//设置分组
	temp=NVIC_PreemptionPriority<<(3-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x07>>NVIC_Group);
	temp&=0x7;								//取低三位
	NVIC->ISER[NVIC_Channel/32]|=1<<NVIC_Channel%32;//使能中断位(要清除的话,设置ICER对应位为1即可)
	NVIC->IP[NVIC_Channel]|=temp<<5;				//设置响应优先级和抢断优先级   	    	  				   
} 
////外部中断配置函数
////只针对GPIOA~I;不包括PVD,RTC,USB_OTG,USB_HS,以太网唤醒等
////参数:
////GPIOx:0~8,代表GPIOA~I
////BITx:需要使能的位;
////TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
////该函数一次只能配置1个IO口,多个IO口,需多次调用
////该函数会自动开启对应中断,以及屏蔽线   	    
//void Ex_NVIC_Config(UINT8 GPIOx,UINT8 BITx,UINT8 TRIM) 
//{ 
//	UINT8 EXTOFFSET=(BITx%4)*4;  
//	RCC->APB2ENR|=1<<14;  						//使能SYSCFG时钟  
//	SYSCFG->EXTICR[BITx/4]&=~(0x000F<<EXTOFFSET);//清除原来设置！！！
//	SYSCFG->EXTICR[BITx/4]|=GPIOx<<EXTOFFSET;	//EXTI.BITx映射到GPIOx.BITx 
//	//自动设置
//	EXTI->IMR|=1<<BITx;					//开启line BITx上的中断(如果要禁止中断，则反操作即可)
//	if(TRIM&0x01)EXTI->FTSR|=1<<BITx;	//line BITx上事件下降沿触发
//	if(TRIM&0x02)EXTI->RTSR|=1<<BITx;	//line BITx上事件上升降沿触发
//} 	

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  

//进入待机模式	  
void Sys_Standby(void)
{ 
	SCB->SCR|=1<<2;		//使能SLEEPDEEP位 (SYS->CTRL)	   
//	RCC->APB1ENR|=1<<28;//使能电源时钟 
//	PWR->CSR|=1<<8;     //设置WKUP用于唤醒
//	PWR->CR|=1<<2;      //清除Wake-up 标志
//	PWR->CR|=1<<1;      //PDDS置位
	if((CPM->CPM_SLPCFGR & 0x80000000) == 0)
	{
//		CPM->CPM_PWRCR &=~ 0x0c;	//disable LVD if enter lp/poff1/poff1.5 mode.
		CPM_PWRCR_OptBits(0x0c,RESET);
	}
	else
	{
//		CPM->CPM_PWRCR |= 0x0c;		//enable LVD if enter poff2 mode.
		CPM_PWRCR_OptBits(0x0c,SET);
	}
	WFI_SET();			//执行WFI指令,进入待机模式
//	CPM->CPM_PWRCR |= 0x0c;		//enable LVD after wake-up from lowpower.
	CPM_PWRCR_OptBits(0x0c,SET);
}	     
//系统软复位   
void Sys_Soft_Reset(void)
{   
	SCB->AIRCR =0X05FA0000|(UINT32)0x04;	  
} 		 

void Sys_Init(void)
{
	/*TC has been closed in startup_ARMCM4.s*/
//	TC_Reset_Init(TC_WDP_0_5, 0xffff, FALSE); // disable TC reset
	
	delay(250000);	//调试时钟建议打开，预防修改时钟失败导致无法仿真
	
	Sys_Clk_Init(OSC_150M_HZ, SYS_CLK_DIV_1,IPS_CLK_DIV_2);
	
	//开启EFLASH加速模块
	DCACHE_Init(cacheOff, cacheThrough, cacheOff, cacheOff);
	ICACHE_Init(cacheOff, cacheThrough, cacheOff, cacheOff);
	
	//	CPM_VCC5V_Bypass(); //芯片3.3V或1.8V供电时，对功耗要求高，建议打开此接口，对功耗要求不高可以不打开; 芯片5V供电时必须屏蔽此接口,否则可能坏芯片！！！
}

void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//关闭所有中断(但是不包括fault和NMI中断)
void INTX_DISABLE(void)
{
	__ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(UINT32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}





















