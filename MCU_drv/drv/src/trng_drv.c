// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : trng_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "cpm_drv.h"
#include "trng_drv.h"

/*******************************************************************************
* Function Name  : Init_Trng
* Description    : 随机数功能初始化函数。
*                  开启随机数模块时钟，并设置随机数模块时钟频率为系统时钟频率的1/8
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void Init_Trng(void)
{
	CPM->CPM_IPSCGTCR |= TRNG_ENABLE;

	//系统时钟的60分频
	rTRNGIF_CTRL = 59;
	rTRNGIF_CTRL |= TRNGIF_CTRL_EN_MASK;

	rTRNGIF_CTRL |= (0xFF << 16);
}

/*******************************************************************************
* Function Name  : Trng_Disable
* Description    : 关闭随机数模块时钟
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void Trng_Disable(void)
{
	CPM->CPM_IPSCGTCR &= ~(TRNG_ENABLE);
}

/*******************************************************************************
* Function Name  : GetRandomWord
* Description    : 随机数生成函数
* Input          : None
* Output         : None
* Return         : 1个UINT32型随机数
******************************************************************************/
UINT32 GetRandomWord(void)
{
	UINT32 random_value, tmp;

	tmp = rTRNGIF_CTRL;

	while(!(tmp & TRNGIF_CTRL_STA_MASK))
	{
		tmp = rTRNGIF_CTRL;
	}

	random_value = rTRNGIF_DATA;

	rTRNGIF_CTRL |= TRNGIF_CTRL_CLR_MASK;

	return random_value;
}

