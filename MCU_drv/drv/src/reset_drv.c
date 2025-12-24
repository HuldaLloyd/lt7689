// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : reset_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "stdio.h"
#include "reset_drv.h"
#include "cpm_drv.h"

/*******************************************************************************
* Function Name  : soft_reset
* Description    : 芯片软件复位
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void Soft_Reset(void)
{
	CHIP_RESET->RCR |= SOFTRST;
}

/*******************************************************************************
* Function Name  : get_reset_status
* Description    : 获取芯片复位状态
* 				   bit7 bit6 bit5  bit4  bit3  bit2  bit1  bit0
*					   TCR  VD   SOFT  WDR   POR    FD   CR     NU
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
RST_STATUS Get_Reset_Status(void)
{
	return (RST_STATUS)(CHIP_RESET->RSR);
}

/*******************************************************************************
* Function Name  : Set_POR_Reset
* Description    : 产生一个Power on reset
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void Set_POR_Reset(void)
{
	CPM_Write_CoreTestKey();
	CPM->CPM_VCCCTMR |= (1<<3);
}


/*******************************************************************************
* Function Name  : Disable_SmartCardReset
* Description    : 禁止smart card 复位芯片复位
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void Disable_SmartCardReset(void)
{
	CHIP_RESET->RCR &= ~(CRE);
}
