// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : tc_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "uart.h"
#include "tc_drv.h"


/*******************************************************************************
* Function Name  : TC_ISR
* Description    : TC 中断处理函数
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void TC_ISR(void)
{
	TC->TCCR |= TC_IF;
	while(TC->TCCR&TC_IS);
//	printf("tc int\r\n");
}

void TC_IRQHandler(void)
{
	TC_ISR();
}

/*******************************************************************************
* Function Name  : TC_Init
* Description    : TC初始化
* Input          : - tc_wdp: TC预分频值
*                  - Counterval：time起始值
*
* Output         : None
* Return         : None
******************************************************************************/
void TC_Init(TC_TIMER_PRESCALER tc_wdp, UINT16 Counterval)
{
	TC->TCMR = Counterval;
	TC->TCCR = (tc_wdp<<4) | TC_IE;
	TC->TCCR |= TC_CU;
}

/*******************************************************************************
* Function Name  : TC_Close
* Description    : 关闭TC
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void TC_Close(void)
{
	TC->TCCR &= ~TC_RN;
	TC->TCCR &= ~TC_IE;
}	

/*******************************************************************************
* Function Name  : TC_FeedWatchDog
* Description    : TC喂狗，清除TC watchdog，防止tc time out导致系统中断
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void TC_FeedWatchDog(void)
{
	TC->TCSR = 0x5555;
	TC->TCSR = 0xAAAA;
}

/*******************************************************************************
* Function Name  : TC_GetCount
* Description    : 获取TC counter值
* Input          : None
*
* Output         : TC counter
* Return         : None
******************************************************************************/
UINT32 TC_GetCount(void)
{
	return (TC->TCCNTR);
}

/*******************************************************************************
* Function Name  : TC_ResetInit
* Description    : TC reset功能初始化
* Input          : -wdp：TC分频系数
*                  -tcmr：TC计数初始值
*                  -reset_en：是否产生TC reset
* Output         : None
* Return         : None
******************************************************************************/
void TC_ResetInit(TC_TIMER_PRESCALER wdp, UINT16 tcmr, BOOL reset_en)
{
	UINT16 temp;
	
	if (FALSE == reset_en) //close reset
	{
		TC->TCCR &= ~TC_RN;
        TC->TCMR = tcmr;
        temp = TC->TCCR;
        temp &= (~0x7f);
        NVIC_Init(3, 3, TC_IRQn, 2);
        TC->TCCR = temp|TC_IE|(wdp<<TC_WDP_SHIFT);
	}
    else
    {
        TC->TCMR = tcmr;
        temp = TC->TCCR;
        temp &= (~0x7f);
        TC->TCCR = temp|TC_RN|(wdp<<TC_WDP_SHIFT);
    }
    TC->TCCR |= TC_CU; // update WDP & WMR
}

/*******************************************************************************
* Function Name  : TC_IntInit
* Description    : TC 中断功能初始化
* Input          : -wdp：TC分频系数
*                  -tcmr：TC计数初始值
*                  -int_en：是否产生TC int
* Output         : None
* Return         : None
******************************************************************************/
void TC_IntInit(TC_TIMER_PRESCALER wdp, UINT16 tcmr, BOOL int_en)
{
	UINT16 temp;

	TC->TCMR = tcmr;
	temp = TC->TCCR;
	temp &= (~0x7f);
	
	if (TRUE == int_en)
	{
		NVIC_Init(3, 3, TC_IRQn, 2);
		TC->TCCR = temp|TC_IE|(wdp<<TC_WDP_SHIFT);
	}
	else
		TC->TCCR = temp|(wdp<<TC_WDP_SHIFT);
	
	TC->TCCR |= TC_CU;
}
