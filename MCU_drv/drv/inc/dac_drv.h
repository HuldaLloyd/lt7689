// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : dac_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __DAC_DRV_H__
#define __DAC_DRV_H__

#include "dac_reg.h"
//#include "test_demo.h"

#ifdef DAC_DEMO
#define DAC_SYSTICK_INT
#define DAC_DMA_INT
#endif

typedef enum _DAC_DATA_FORMAT_
{
	LEFTALIGNED_8BITS = 0,
	LEFTALIGNED_12BITS,
	RIGHTALIGNED_8BITS,
	RIGHTALIGNED_12BITS
}DAC_DATA_FORMAT;

typedef enum _DAC_TRIGGER_SEL_
{
	TRIGGER_SOFTWARE = 0,
	TRIGGER_EXTERNAL,
	TRIGGER_PIT
}DAC_TRIGGER_SEL;

typedef enum _DAC_EXT_TMOD_
{
	MOD_RESERVED = 0,
	DET_ON_RISING = 1,
	DET_ON_FALLING,
	DET_ON_BOTH,
	DET_ON_HIGH,
	DET_ON_LOW
}DAC_EXT_TMOD;

/*******************************************************************************
* Function Name  : DAC_Init
* Description    : DAC初始化
* Input          : -data_format:
*                      LEFTALIGNED_8BITS：8bits左对齐
*                      LEFTALIGNED_12BITS：12bits左对齐
*                      RIGHTALIGNED_8BITS：8bits右对齐
*                      RIGHTALIGNED_12BITS：12bits右对齐
*                  -trigger_select:
*                      TRIGGER_SOFTWARE：软件触发
*                      TRIGGER_EXTERNAL：外部触发(SS_SPI6)
*                      TRIGGER_PIT：定时器触发(PIT2)
*                  -external_mode:
*                      DET_ON_RISING：外部触发方式为上升沿
*                      DET_ON_FALLING：外部触发方式为下降沿触发
*                      DET_ON_BOTH：外部触发方式为双沿触发
*                      DET_ON_HIGH：外部触发方式为高电平触发
*                      DET_ON_LOW：外部触发方式为低电平触发
* Output         : None
* Return         : None
******************************************************************************/
extern void DAC_Init(DAC_DATA_FORMAT data_format, DAC_TRIGGER_SEL trigger_select, DAC_EXT_TMOD external_mode);
/*******************************************************************************
* Function Name  : DAC_Start
* Description    : DAC启动
* Input          : -trigger_select:
*                      TRIGGER_SOFTWARE：软件触发
*                      TRIGGER_EXTERNAL：外部触发(SS_SPI6)
*                      TRIGGER_PIT：定时器触发(PIT2)
* Output         : None
* Return         : None
******************************************************************************/
extern void DAC_Start(DAC_TRIGGER_SEL trigger_select,unsigned int rate);
/*******************************************************************************
* Function Name  : DAC_Wait_Load_Done
* Description    : DAC等待数据完成
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void DAC_Wait_Load_Done(void);

extern void DAC_Wait_Load_Cear(void);

extern void DAC_SW_Trig(void);

extern void Send_DAC_data(unsigned short data);
extern void DAC_Systick_ISR(void);
extern void DAC_close(void);
extern void DAC_Write_Data(UINT16 data);
#endif /* __DAC_DRV_H__ */
