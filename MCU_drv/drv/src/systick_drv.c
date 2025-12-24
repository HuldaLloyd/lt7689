
#include "systick_drv.h"
//#include "test_demo.h"

#ifdef DAC_DEMO
#include "dac_drv.h"
#endif

#ifdef DAC_AUDIO_DEMO
#include "dac_audio.h"
#endif

volatile UINT32 g_systickCounter;

void SysTick_Handler(void)
{
#ifdef DAC_SYSTICK_INT
	DAC_Systick_ISR();
#endif

#ifdef DAC_SYSTICK_DMA_INT
	DMA_Systick_ISR();
#endif
	
#ifdef SYSTICK_DEMO
	g_systickCounter++;
#endif
}

/*******************************************************************************
* Function Name  : SysTick_Enable
* Description    : 
* Input          : flag  : TRUE :Open systick  FALSE :Close systick
*
* Output         : 
* Return         : None
******************************************************************************/
void SysTick_Enable(bool flag)
{
	if(flag == TRUE)
	{
		SysTick->CTRL  |= SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick */
	}
	else
	{
		SysTick->CTRL  &= ~SysTick_CTRL_ENABLE_Msk;                         /* Disable SysTick */
	}
}


