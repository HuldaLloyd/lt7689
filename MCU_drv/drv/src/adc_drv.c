// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : adc_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "type.h"
#include "adc_drv.h"
#include "cpm_drv.h"
#include "uart.h"
#include "delay.h"

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
void ADC_Init(UINT32 cfgr1, UINT32 cfgr2, UINT32 chselr1, UINT32 chselr2, UINT32 smpr)
{
	ADC->CFGR1 = cfgr1;
	ADC->CFGR2 = cfgr2;
	ADC->CHSELR1 = chselr1;
	ADC->CHSELR2 = chselr2;
	ADC->SMPR = smpr;
	ADC->CHSELR3 = 0xffff;
}
/*******************************************************************************
* Function Name  : ADC_GetConversionValue
* Description    : ADC 采样处理函数
* Input          :  - ADC_CHx：ADC采用通道  对应芯片引脚 ADC_INx (其中x取值0~7)
*
* Output         : None
* Return         : 对应引脚AD采集的值
******************************************************************************/
UINT32 ADC_GetConversionValue(UINT8 ADC_CHx)
{
	UINT32 adcisr = 0,i;

	ADC_Init((ADC_SEQ_LEN_1|ADC_OVRMOD_LAST_CONV|ADC_CONTINUOUS_CONV_MODE|ADC_EXTERNAL_VREF|ADC_RIGHT_ALIGN|ADC_CONV_RESOLUTION_12BIT),
		     ((3<<8)|0x20),//1    0x20
				 CCW0(ADC_CHx),
				 0,
		     0x20);//0x20
	ADC_Cmd(ADC_EN);
	
	for (i = 0; i < 9; i++)
	{
		ADC_StartConv();
		while (EOSEQ !=(ADC->ISR & EOSEQ));
		ADC->ISR |= EOSEQ;
		ADC_StopConv();

		if(i == 0)
		{// 第一个数丢掉
			adcisr = (ADC->uFIFO_DAT.FIFO)&0xFFFF;
			adcisr = 0;
			continue;
		}
		adcisr += (ADC->uFIFO_DAT.FIFO)&0xFFFF;

	}

	ADC_Cmd(ADC_DIS);

	return ((adcisr>>3)&0x0fff);
}

/*******************************************************************************
* Function Name  : adc_module_dis
* Description    : ADC模块禁用
* 				         注意：
* 				    None
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
static void adc_module_dis(void)
{
	ADC->CR = ADDIS;
	while (0 != ADC->CR);

}
/*******************************************************************************
* Function Name  : adc_module_en
* Description    : ADC模块使能
* 				         注意：
* 				    None
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
static void adc_module_en(void)
{
	ADC->CR = ADEN;
	while (ADRDY != (ADC->ISR& ADRDY));
}


/*******************************************************************************
* Function Name  : ADC_Cmd
* Description    : Enable or Disable ADC
* Input          :  - en：ADC_DIS----disable ADC   ADC_EN----enable ADC
*
* Output         : None
* Return         : None
******************************************************************************/
void ADC_Cmd(BOOL en)
{
	if (en == ADC_DIS)
	{
		adc_module_dis();
	}
	else
	{
		adc_module_en();
	}
}



/*******************************************************************************
* Function Name  : ADC_StartConv
* Description    : ADC 转换启动
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void ADC_StartConv(void)
{
	UINT32 adccr;
	adccr = ADC->CR;
	adccr |= ADSTART;
	ADC->CR = adccr;
}

/*******************************************************************************
* Function Name  : ADC_StopConv
* Description    : ADC 转换停止
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void ADC_StopConv(void)
{
	ADC->CR |= ADSTP;
	while (ADEN !=ADC->CR);
}

/*******************************************************************************
* Function Name  : ADC_Get_Battery
* Description    : 采集锂电池电量
* Input          : - ADC_CHx：ADC采用通道  对应芯片引脚 ADC_INx (其中x取值0~7)
*
* Output         : None
* Return         : None
******************************************************************************/
unsigned int ADC_Get_Battery(unsigned char ADC_CHx)
{
	unsigned int resVC = 0;
	unsigned int res_battery = 0;
	unsigned int res_standard = 0;
	unsigned int reg_tmp;
	int i;

	for(i = 0; i < 10; i ++)
	{
		res_battery += ADC_GetConversionValue(ADC_CHx);
	}
	
	//input core test key
	CPM_Write_CoreTestKey();
	CPM->CPM_VCCCTMR |= (1<<21);
	

	reg_tmp = CPM->CPM_VCCVTRIMR;
	//CPM->CPM_VCCCTMR |= (1<<21);//OVERWR_VCC_TRIM位置上
	CPM->CPM_VCCVTRIMR |=  (1<<8);//关v_store reg
	CPM->CPM_VCCVTRIMR |=  (1<<10);//开trim en reg
	CPM->CPM_VCCVTRIMR |=  (1<<9);
	CPM->CPM_VCCCTMR &= ~(1<<21);
	
//	ADC_GetConversionValue(ADCCH_9);
//	for(i = 0; i < 10; i ++)
	{
		res_standard += ADC_GetConversionValue(ADCCH_9);
	}
	

	CPM->CPM_VCCVTRIMR &=  ~(1<<8);
	CPM->CPM_VCCVTRIMR &=  ~(1<<10);
	
	CPM->CPM_VCCCTMR |= (1<<21);
	CPM->CPM_VCCVTRIMR = reg_tmp;
  CPM->CPM_VCCCTMR &= ~(1<<21);

	resVC = 9*res_battery/res_standard;

	return resVC;
}
