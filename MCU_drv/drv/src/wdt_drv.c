// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : wdt_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "wdt_drv.h"


/*******************************************************************************
* Function Name  : WDT_Init
* Description    : WDT初始化
* Input          : - WMRCounterVal: 看门狗计数初值
*
* Output         : None
* Return         : None
******************************************************************************/
void WDT_Init(UINT16 WMRCounterVal)
{
	WDT->WDT_WMR = WMRCounterVal;
	WDT->WDT_WCR = WDT_EN;
}

/*******************************************************************************
* Function Name  : WDT_FeedDog
* Description    : WDT喂狗
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void WDT_FeedDog(void)
{
	WDT->WDT_WSR = 0x5555;
	WDT->WDT_WSR = 0xAAAA;
}

/*******************************************************************************
* Function Name  : WDT_Open
* Description    : 打开WDT
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void WDT_Open(void)
{
	WDT->WDT_WCR |= WDT_EN;
}
/*******************************************************************************
* Function Name  : WDT_Close
* Description    : 关闭WDT
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void WDT_Close(void)
{
	WDT->WDT_WCR &= ~WDT_EN;
}

/*******************************************************************************
* Function Name  : Get_WDTCount
* Description    : 获取Watch-dog counter值
* Input          : None
*
* Output         : Watch-dog counter
* Return         : None
******************************************************************************/
UINT32 Get_WDTCount(void)
{
	return (WDT->WDT_WCNTR);
}

