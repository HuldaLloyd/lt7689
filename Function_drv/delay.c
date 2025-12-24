/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     delay.c
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief
 *************************************************************************/
 
#include "sys.h"
#include "cpm_drv.h"
#include "delay.h"

/*******************************************************************************
 * Function Name  : DelayMS
 * Description    : Delay n MS
 * Input          : - delaymS:Delay Parame
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DelayMS(vu32 delaymS)
{
	//	UINT32 counter=0;
	UINT32 k = 0;
	vu32 counter_ms = g_sys_clk / 6000;

	while (delaymS--)
	{
		while (k < counter_ms)
			k++;
		k = 0;
	}
}

/*******************************************************************************
 * Function Name  : DelayuS
 * Description    : Delay n uS
 * Input          : - delayuS:Delay Parame
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DelayUS(vu32 delayuS)
{
	//	UINT32 counter=0;
	UINT32 k = 0;
	vu32 counter_us = g_sys_clk / 6000000;

	while (delayuS--)
	{
		while (k < counter_us)
			k++;
		k = 0;
	}
}

/*******************************************************************************
 * Function Name  : delay
 * Description    : Delay n nop
 * Input          : time: n nop
 * Output         : None
 * Return         : None
 *******************************************************************************/
void delay(vu32 time)
{
	while (time--);
}
