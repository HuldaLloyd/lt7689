
#ifndef __SYSTICK_DRV_H__
#define __SYSTICK_DRV_H__

#include "type.h"
#include "core_cm4.h"

extern volatile UINT32 g_systickCounter;

/*******************************************************************************
* Function Name  : SysTick_Enable
* Description    : 
* Input          : flag  : TRUE :Open systick  FALSE :Close systick
*
* Output         : 
* Return         : None
******************************************************************************/
extern void SysTick_Enable(bool flag);
#endif

