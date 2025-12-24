/*
 * pit32_drv.h
 *
 *  Created on: 2017年11月29日
 *      Author: nltao
 */

#ifndef PIT32_DRV_H_
#define PIT32_DRV_H_

#include "type.h"
#include "pit32_reg.h"

#define PIT32_ADDR        (PIT1_BASE_ADDR)
//#define PIT32_ADDR        (PIT2_BASE_ADDR)

#define PIT32             ((PIT32_TypeDef *)PIT32_ADDR)     

#define PIT1             ((PIT32_TypeDef *)PIT1_BASE_ADDR)    
#define PIT2             ((PIT32_TypeDef *)PIT2_BASE_ADDR)

typedef enum
{
	PIT32_CLK_DIV_1,//1分频
	PIT32_CLK_DIV_2,//2分频
	PIT32_CLK_DIV_4,//4分频
	PIT32_CLK_DIV_8,//8分频
	PIT32_CLK_DIV_16,//16分频
	PIT32_CLK_DIV_32,//32分频
	PIT32_CLK_DIV_64,//64分频
	PIT32_CLK_DIV_128,//128分频
	PIT32_CLK_DIV_256,//256分频
	PIT32_CLK_DIV_512,//512分频
	PIT32_CLK_DIV_1024,//1024分频
	PIT32_CLK_DIV_2048,//2048分频
	PIT32_CLK_DIV_4096,//4096分频
	PIT32_CLK_DIV_8192,//8192分频
	PIT32_CLK_DIV_16384,//16384分频
	PIT32_CLK_DIV_32768,//32768分频
}PIT32_CLKDIV;


/*******************************************************************************
* Function Name  : PIT32_Init
* Description    : PIT初始化
* Input          : - Prescaler：PIT时钟分频值
*                  - PMRReloadCounterVal:PIT计数器的起始值
*                  - pitINT :使用PIT中断方式
*
* Output         : None
* Return         : None
******************************************************************************/
extern void PIT32_Init(PIT32_CLKDIV Prescaler, UINT32 PMRReloadCounterVal,BOOL pitINT);

/*******************************************************************************
* Function Name  : PIT32_Start
* Description    : PIT启动
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void PIT32_Start(void);


/*******************************************************************************
* Function Name  : PIT32_Stop
* Description    : PIT关闭
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void PIT32_Stop(void);



/*******************************************************************************
* Function Name  : PIT32_Cmd
* Description    : PIT使能控制
* Input          : - NewState：new state of the specified PIT.
*                              This parameter can be: ENABLE or DISABLE.
*
* Output         : None
* Return         : None
******************************************************************************/
extern void PIT32_Cmd(FunctionalState NewState);



/*******************************************************************************
* Function Name  : PIT32_CmdPIT_ITConfig
* Description    : PIT中断使能控制
* Input          : - NewState：new state of the specified PIT interrupts.
*                              This parameter can be: ENABLE or DISABLE.
*
* Output         : None
* Return         : None
******************************************************************************/
extern void PIT32_ITConfig(FunctionalState NewState);


/*******************************************************************************
* Function Name  : PIT32_ReadCNTR
* Description    : 读取PIT计数值
* Input          : -None
*
* Output         : 计数值
* Return         : 计数值
******************************************************************************/
extern UINT32 PIT32_ReadCNTR(void);

/*******************************************************************************
* Function Name  : PIT32_ISR
* Description    : PIT32中断处理
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void PIT_dis(void);

#endif /* PIT32_DRV_H_ */
