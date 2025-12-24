// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : pit32_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "uart.h"
#include "pit32_drv.h"
//#include "test_demo.h"

#ifdef BCTC_DEMO
#include "iccemv.h"
#endif

#if PIT32_ADDR == PIT1_BASE_ADDR
	#define PIT_IRQn   PIT1_IRQn
#else
	#define PIT_IRQn   PIT2_IRQn
#endif

/*******************************************************************************
* Function Name  : PIT32_ISR
* Description    : PIT32中断处理
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
static void PIT32_ISR(void)
{
	UINT32 tmp1 = PIT32->PCSR;

	if (tmp1 & PCSR_PIF)//pit
	{
		PIT32->PCSR |= PIT_PIF;
/*************************************************************/
		//以下为7816主模式下需要的时间中断处理代码
#ifdef USI_PIT
		if((k_IcMax_TotalTimeCount>0)&&(k_total_timeover==0))
		{
			  k_Ic_TotalTimeCount++;
			 if(k_Ic_TotalTimeCount>k_IcMax_TotalTimeCount)
			 {
				k_total_timeover=1;
			 }
			 if(k_total_timeover==1)
			 {
			   k_Ic_TotalTimeCount=0;
			   PIT32_Stop();
			 }
			 else
			 {

			 }
		 }	
#else 
		printf("enter pit interrupt.\r\n");
#endif
	/***************************************************************/
	}
}

//void PIT1_IRQHandler(void)
//{
//	PIT32_ISR();
//}

//void PIT2_IRQHandler(void)
//{
//	PIT32_ISR();
//}

void PIT_en(void)
{
	PIT32->PCSR |= PCSR_EN;
}
void PIT_dis(void)
{
	PIT32->PCSR &=~PCSR_EN;
}
/*******************************************************************************
* Function Name  : PIT32_Init
* Description    : PIT初始化
* Input          : - Prescaler：PIT时钟分频值
*                  - PMRReloadCounterVal:PIT计数器的起始值
*
* Output         : None
* Return         : None
******************************************************************************/
void PIT32_Init(PIT32_CLKDIV Prescaler, UINT32 PMRReloadCounterVal,BOOL pitINT)
{
	//ModuleClk_On(MODULE_CLK_PIT1_IPG_CLK);
	if(pitINT == TRUE)
	{
			NVIC_Init(3, 3, PIT_IRQn, 2);
	}
	PIT32->PCSR &= (~PCSR_EN);
	if(pitINT == TRUE)
	{
		PIT32->PCSR = (Prescaler<<8)|PCSR_OVW|PCSR_PIE|PCSR_RLD|PCSR_PDBG; //中断模式
	}
	else
	{
		PIT32->PCSR = (Prescaler<<8)|PCSR_OVW|PCSR_RLD;    //普通模式
	}


	PIT32->PMR = PMRReloadCounterVal;
	PIT32->PCSR |= PCSR_EN;
}

/*******************************************************************************
* Function Name  : PIT32_Start
* Description    : PIT启动
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void PIT32_Start(void)
{
	PIT32->PCSR |= PCSR_EN;
}

/*******************************************************************************
* Function Name  : PIT32_Stop
* Description    : PIT关闭
* Input          : -None
*
* Output         : None
* Return         : None
******************************************************************************/
void PIT32_Stop(void)
{
	PIT32->PCSR &= ~PCSR_EN;
}


/*******************************************************************************
* Function Name  : PIT32_Cmd
* Description    : PIT使能控制
* Input          : - NewState：new state of the specified PIT.
*                              This parameter can be: ENABLE or DISABLE.
*
* Output         : None
* Return         : None
******************************************************************************/
void PIT32_Cmd(FunctionalState NewState)
{
	if (NewState != DISABLE)
	{
		/* Enable the PIT */
		PIT32->PCSR |= PCSR_EN;
	}
	else
	{
		/* Disable the PIT */
		PIT32->PCSR &= ~PCSR_EN;
	}
}

/*******************************************************************************
* Function Name  : PIT32_CmdPIT_ITConfig
* Description    : PIT中断使能控制
* Input          : - NewState：new state of the specified PIT interrupts.
*                              This parameter can be: ENABLE or DISABLE.
*
* Output         : None
* Return         : None
******************************************************************************/
void PIT32_ITConfig(FunctionalState NewState)
{
	if (NewState != DISABLE)
	{
		/* Enable the Interrupt source		s */
		PIT32->PCSR |= PCSR_PIE;
	}
	else
	{
		/* Disable the Interrupt sources */
		PIT32->PCSR &= (UINT16)~PCSR_PIE;
	}
}

/*******************************************************************************
* Function Name  : PIT32_ReadCNTR
* Description    : 读取PIT计数值
* Input          : None
*
* Output         : 计数值
* Return         : 计数值
******************************************************************************/
UINT32 PIT32_ReadCNTR(void)
{
	return PIT32->PCNTR;
}
