// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : tsi_drv.h
// Version      : V0.1
// date 				:20190425
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef _TSI_DRV_H_
#define _TSI_DRV_H_
#include "tsi_reg.h"
#include "type.h"
#include "memmap.h"

#define EN_CHANNEL_NUM  		12
//可根据实际配置修改TSI通道
#define TSI_EN_CHANNEL  		0xc7ef            //bit[15:0] ch0-ch3 ch5-ch10  ch14-ch15
#define TSI_WAKEUP_CHANNEL  0xc7ef            //bit[15:0] ch0-ch3 ch5-ch10  ch14-ch15

#define KEY_SENS 0
//按键灵敏度  0x08-0x3f  注意：值越小，灵敏度越高，每个通道可以单独调整  
#define TSI_CH0_SENS		10+KEY_SENS	  //灵敏度中  //key7
#define TSI_CH1_SENS		14+KEY_SENS								//key10		
#define TSI_CH2_SENS		16+KEY_SENS								//key11	
#define TSI_CH3_SENS		16+KEY_SENS								//key1	
																					
#define  TSI_CH4_SENS		10+KEY_SENS 									//		
#define  TSI_CH5_SENS		10+KEY_SENS								//key8	
#define  TSI_CH6_SENS		15+KEY_SENS								//key0
#define  TSI_CH7_SENS		14+KEY_SENS								//key6
																			
#define  TSI_CH8_SENS		18+KEY_SENS								//key3   	
#define  TSI_CH9_SENS 	12+KEY_SENS								//key9	
#define  TSI_CH10_SENS 	12+KEY_SENS								//key4		
#define  TSI_CH11_SENS 	10+KEY_SENS									//
																				
#define  TSI_CH12_SENS  10+KEY_SENS									//
#define  TSI_CH13_SENS  10+KEY_SENS									//
#define  TSI_CH14_SENS  12+KEY_SENS								//key2
#define  TSI_CH15_SENS  12+KEY_SENS								//key5

#define WAKEUP_SENS 0x8
//按键唤醒灵敏度  0x08-0x3f  注意：值越小，灵敏度越高，每个通道可以单独调整
#define TSI_CH0_WAKEUP_SENS    WAKEUP_SENS
#define TSI_CH1_WAKEUP_SENS    WAKEUP_SENS
#define TSI_CH2_WAKEUP_SENS    WAKEUP_SENS
#define TSI_CH3_WAKEUP_SENS    WAKEUP_SENS

#define  TSI_CH4_WAKEUP_SENS    WAKEUP_SENS
#define  TSI_CH5_WAKEUP_SENS    WAKEUP_SENS
#define  TSI_CH6_WAKEUP_SENS    WAKEUP_SENS
#define  TSI_CH7_WAKEUP_SENS    WAKEUP_SENS

#define  TSI_CH8_WAKEUP_SENS    WAKEUP_SENS
#define  TSI_CH9_WAKEUP_SENS    WAKEUP_SENS
#define  TSI_CH10_WAKEUP_SENS   WAKEUP_SENS
#define  TSI_CH11_WAKEUP_SENS   WAKEUP_SENS

#define  TSI_CH12_WAKEUP_SENS   WAKEUP_SENS
#define  TSI_CH13_WAKEUP_SENS   WAKEUP_SENS
#define  TSI_CH14_WAKEUP_SENS   WAKEUP_SENS
#define  TSI_CH15_WAKEUP_SENS   WAKEUP_SENS

extern unsigned char tsi_sens[16];
extern unsigned char tsi_wakeup_sens[16];

#define INIT_SENS_PARA  tsi_sens[0]=TSI_CH0_SENS;tsi_sens[1]=TSI_CH1_SENS;\
												tsi_sens[2]=TSI_CH2_SENS;tsi_sens[3]=TSI_CH3_SENS;\
												tsi_sens[4]=TSI_CH4_SENS;tsi_sens[5]=TSI_CH5_SENS;\
												tsi_sens[6]=TSI_CH6_SENS;tsi_sens[7]=TSI_CH7_SENS;\
												tsi_sens[8]=TSI_CH8_SENS;tsi_sens[9]=TSI_CH9_SENS;\
												tsi_sens[10]=TSI_CH10_SENS;tsi_sens[11]=TSI_CH11_SENS;\
												tsi_sens[12]=TSI_CH12_SENS;tsi_sens[13]=TSI_CH13_SENS;\
												tsi_sens[14]=TSI_CH14_SENS;tsi_sens[15]=TSI_CH15_SENS;\
												tsi_wakeup_sens[0]=TSI_CH0_WAKEUP_SENS;tsi_wakeup_sens[1]=TSI_CH1_WAKEUP_SENS;\
												tsi_wakeup_sens[2]=TSI_CH2_WAKEUP_SENS;tsi_wakeup_sens[3]=TSI_CH3_WAKEUP_SENS;\
												tsi_wakeup_sens[4]=TSI_CH4_WAKEUP_SENS;tsi_wakeup_sens[5]=TSI_CH5_WAKEUP_SENS;\
												tsi_wakeup_sens[6]=TSI_CH6_WAKEUP_SENS;tsi_wakeup_sens[7]=TSI_CH7_WAKEUP_SENS;\
												tsi_wakeup_sens[8]=TSI_CH8_WAKEUP_SENS;tsi_wakeup_sens[9]=TSI_CH9_WAKEUP_SENS;\
												tsi_wakeup_sens[10]=TSI_CH10_WAKEUP_SENS;tsi_wakeup_sens[11]=TSI_CH11_WAKEUP_SENS;\
												tsi_wakeup_sens[12]=TSI_CH12_WAKEUP_SENS;tsi_wakeup_sens[13]=TSI_CH13_WAKEUP_SENS;\
												tsi_wakeup_sens[14]=TSI_CH14_WAKEUP_SENS;tsi_wakeup_sens[15]=TSI_CH15_WAKEUP_SENS;

/*
注意:
1.在调用TSI_Init()、TSI_Wakeup_Re_init()、Get_Channel_Data()函数前，初始化PCI模块，调用PCI_Init(UINT8 clk_sel);
2.在调用TSI_Init()、TSI_Wakeup_Re_init()前，调用宏INIT_SENS_PARA;保证灵敏度变量正常
*/

/*******************************************************************************
* Function Name  : TSI_Init
* Description    : TSI初始化
* Input          :  en_channel  使能的channel
*
* Output         : None
* Return         : None
******************************************************************************/
extern void TSI_Init(unsigned int en_channel);

/*******************************************************************************
* Function Name  : TSI_Wakeup_Re_init
* Description    : 低功耗唤醒后TSI初始化
* Input          :  en_channel  使能的channel
*
* Output         : None
* Return         : None
******************************************************************************/
extern void TSI_Wakeup_Re_init(unsigned int en_channel);

/*******************************************************************************
* Function Name  : TSI_Key_Data
* Description    : 按键扫描
* Input          : None
*
* Output         : None
* Return         : 按键通道号
******************************************************************************/
extern unsigned char TSI_Key_Data(void);

/*******************************************************************************
* Function Name  : Config_TSI_Lowpower_Wakeup
* Description    : 配置低功耗唤醒
* Input          : en_channel  使能的channel
*
* Output         : None
* Return         : None
******************************************************************************/
extern void Config_TSI_Lowpower_Wakeup(u32 en_channel);

/*******************************************************************************
* Function Name  : Get_Channel_Data
* Description    : 判断唤醒后当前TSI_data是否存在按键动作
* Input          : en_channel  使能的channel
*
* Output         : None
* Return         : 1=存在 ，0=不存在
******************************************************************************/
extern unsigned char Get_Channel_Data(unsigned int en_channel);
#endif //_TSI_H_
