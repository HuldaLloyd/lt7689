// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : adc_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef __ADC_DRV_H__
#define __ADC_DRV_H__

#include "type.h"
#include "adc_reg.h"
#include "memmap.h"


#define ADCCH_0	    0
#define ADCCH_1	    8
#define ADCCH_2	    1
#define ADCCH_9	    9 		//通道9直接连接内部1.1V基准点压

#define ADC_DIS     0
#define ADC_EN      1


#define ADC		((ADC_TypeDef *)(ADC_BASE_ADDR))


/*******************************************************************************
* Function Name  : ADC_Init
* Description    : ADC 初始化
* Input          :  - cfgr1：ADC configuration register 1
*                   - cfgr2：ADC configuration register 2
*                   - chselr1： ADC channel selection register 1
*                   - chselr2: ADC channel selection register 2
*                   - smpr：    ADC sampling time register
*
* Output         : None
* Return         : None
******************************************************************************/
extern void ADC_Init(UINT32 cfgr1, UINT32 cfgr2, UINT32 chselr1, UINT32 chselr2, UINT32 smpr);



/*******************************************************************************
* Function Name  : ADC_GetConversionValue
* Description    : ADC 采样处理函数
* Input          :  - ADC_CHx：ADC采用通道  对应芯片引脚 ADC_INx (其中x取值0~7)
*
* Output         : None
* Return         : 对应引脚AD采集的值
******************************************************************************/
extern UINT32 ADC_GetConversionValue(UINT8 ADC_CHx);


 /*******************************************************************************
 * Function Name  : ADC_Cmd
 * Description    : Enable or Disable ADC
 * Input          :  - en：ADC_DIS----disable ADC   ADC_EN----enable ADC
 *
 * Output         : None
 * Return         : None
 ******************************************************************************/
 extern void ADC_Cmd(BOOL en);


 /*******************************************************************************
 * Function Name  : ADC_StartConv
 * Description    : ADC 转换启动
 * Input          : None
 *
 * Output         : None
 * Return         : None
 ******************************************************************************/

 extern void ADC_StartConv(void);

 /*******************************************************************************
 * Function Name  : ADC_StopConv
 * Description    : ADC 转换停止
 * Input          : None
 *
 * Output         : None
 * Return         : None
 ******************************************************************************/
 extern void ADC_StopConv(void);
 
/*******************************************************************************
* Function Name  : ADC_Get_Battery
* Description    : 采集锂电池电量
* Input          : - ADC_CHx：ADC采用通道  对应芯片引脚 ADC_INx (其中x取值0~7)
*
* Output         : None
* Return         : None
******************************************************************************/
extern unsigned int ADC_Get_Battery(unsigned char ADC_CHx);


#endif /* __ADC_DRV_H__ */
