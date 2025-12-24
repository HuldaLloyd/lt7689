// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : rtc_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "rtc_drv.h"
#include "cpm_reg.h"
#include "uart.h"
#include "pci_drv.h"


#define RTC_WCLK_MAX    10000000//10MHz

UINT32 g_rtc_int_sta = 0;

/*******************************************************************************
* Function Name  : RTC_ISR
* Description    : RTC中断处理
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
//void PMU_RTC_IRQHandler(void)
//{
//	UINT32 prc1r;
//	UINT8  alarm_wakeup_flg = 1;

//	prc1r = RTC->RTC_PRC1R;

//	//printf("\r\nRTC INT[0x%08x]\r\n", prc1r);
//	if (((prc1r&Ala_intf) == Ala_intf) && ((prc1r&Ala_IEN) == Ala_IEN))//alarm interrupt
//	{
//		RTC->RTC_PRC1R |= Ala_intf;
//		g_rtc_int_sta |= Ala_intf;
//		alarm_wakeup_flg = 0;
//		printf("RTC Alarm\r\n");
//	}

//	if(((prc1r&Day_intf) == Day_intf) && ((prc1r&Day_IEN) == Day_IEN))//day interrupt
//	{
//		RTC->RTC_PRC1R |= Day_intf;
//		g_rtc_int_sta |= Day_intf;
//		alarm_wakeup_flg = 0;
//	}

//	if(((prc1r&Hou_intf) == Hou_intf) && ((prc1r&Hou_IEN) == Hou_IEN))//hour interrupt
//	{
//		RTC->RTC_PRC1R |= Hou_intf;
//		g_rtc_int_sta |= Hou_intf;
//		alarm_wakeup_flg = 0;
//	}

//	if(((prc1r&Min_intf) == Min_intf) && ((prc1r&Min_IEN) == Min_IEN))//minute interrupt
//	{
//		RTC->RTC_PRC1R |= Min_intf;
//		g_rtc_int_sta |= Min_intf;
//		alarm_wakeup_flg = 0;
//	}

//	if(((prc1r&Sec_intf) == Sec_intf) && ((prc1r&Sec_IEN) == Sec_IEN))//second interrupt
//	{
//		//tm tim;

//		RTC->RTC_PRC1R |= Sec_intf;
//		g_rtc_int_sta |= Sec_intf;
//		//printf("RTC Second\r\n");
//		//RTC_GetTime(&tim);
//		//printf("RTC Second INT[%d]\r\n", tim.second);
//		alarm_wakeup_flg = 0;
//	}

//	if (alarm_wakeup_flg)
//	{
//		printf("\r\nRTC alarm wakeup from sleep mode\r\n");
//		RTC->RTC_PRKEYR = 0x5aa55aa5;//输入key
//		RTC->RTC_PRC1R &= ~Ala_IEN;
//		RTC->RTC_PRC1R &= ~Dir;//set dir = 0;
//	}
//}

/*******************************************************************************
* Function Name  : RTC_Init
* Description    : RTC初始化
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void RTC_Init(UINT8 clk_sel)
{
	UINT32 rtc_wclk_div = 0;
	
	if(clk_sel == INTERNAL_CLK_SEL)
	{
		//write enable key
		PCI->PCI_WRITEEN = 0x40000000;
		PCI->PCI_WRITEEN = 0x80000000;
		PCI->PCI_WRITEEN = 0xC0000000;
		PCI->PCI_WRITEEN = 0xC000003F;//write enable
		
		PCI->PCI_RTCIER  = 0x00040001;  //RTC Interface Enable
	
		while((PCI->PCI_RTCIER&0x80000001)!=0x80000001);  //RTC Interface Enable Delay Match

		PCI->PCI_DETPCR |= 0x80770000;	//RTC32K Clock Enable
		
		PCI->PCI_IRCCR = 0x84;

		RTC->RTC_PRENR = 0x0100;	//disable PMU_RTC 5
		RTC->RTC_PRENR = 0x0300;
	}
	
	rtc_wclk_div = g_sys_clk/RTC_WCLK_MAX;

	if (rtc_wclk_div >= 1)
		rtc_wclk_div--;
	
	//Update RTC Counter(function case Dir=0)
	NVIC_Init(3, 3, PMU_RTC_IRQn, 2);//开RTC中断
	RTC->RTC_PRKEYR = 0x5aa55aa5;//输入key
	RTC->RTC_PRC1R &= (~WCLK_DIV_MASK);
	RTC->RTC_PRC1R |= (rtc_wclk_div<<3);//8分频
	RTC->RTC_PRC1R &= ~Dir;//set dir = 0;
	
}
/*******************************************************************************
* Function Name  : RTC_SetTime
* Description    : 设置RTC初值
* Input          :  - t：tm结构体
*                        t->day:设置RTC的day值
*                        t->hour:设置RTC的hour值
*                        t->minute:设置RTC的minute值
*                        t->second:设置RTC的second值
*
* Output         : None
* Return         : None
******************************************************************************/
void RTC_SetTime(tm t)
{

	RTC->RTC_PRT1R = t.day;
	RTC->RTC_PRT2R = (t.hour<<16)|(t.minute<<8)|t.second;

	RTC->RTC_PRC1R = (RTC->RTC_PRC1R & (~RTC_SEL_MASK)) | (RTC_SEL_MASK & RTC_RISING_EDGE_INT);
	//RTC->RTC_PRC1R |= Sec_IEN;

//	printf("t.day = %d\r\n", t.day);
//	printf("t.hour = %d\r\n", t.hour);
//	printf("t.minute = %d\r\n", t.minute);
//	printf("t.second = %d\r\n", t.second);
}

/*******************************************************************************
* Function Name  : RTC_SetAlarm
* Description    : 设置RTC初值
* Input          :  - t：tm结构体，设置alarm计数器的值
*                   - AlarmMod：alarm的模式
*                               DAY_ALARM：day报警
*                               HOUR_ALARM：hour报警
*                               MINUTE_ALARM：minute报警
*                               SECOND_ALARM：second报警
*                               ALL_ALARM：day、hour、minute、second报警
*
* Output         : None
* Return         : None
******************************************************************************/
void RTC_SetAlarm(tm t, UINT8 AlarmMod)
{
	switch(AlarmMod)
	{
		case DAY_ALARM:
			RTC->RTC_PRA1R = AlaD_en|t.day;
			break;
		case HOUR_ALARM:
			RTC->RTC_PRA2R = AlaH_en|(t.hour<<16);
			break;
		case MINUTE_ALARM:
			RTC->RTC_PRA2R = AlaM_en|(t.minute<<8);
			break;
		case SECOND_ALARM:
			RTC->RTC_PRA2R = AlaS_en|t.second;
			break;
		case ALL_ALARM:
		{
			RTC->RTC_PRA1R = AlaD_en|t.day;
			RTC->RTC_PRA2R = AlaS_en|AlaM_en|AlaH_en|(t.hour<<16)|(t.minute<<8)|t.second;
//			printf("t.day = %d\r\n", t.day);
//			printf("t.hour = %d\r\n", t.hour);
//			printf("t.minute = %d\r\n", t.minute);
//			printf("t.second = %d\r\n", t.second);
			break;
		}
		default:
			break;
	}

	RTC->RTC_PRC1R = (RTC->RTC_PRC1R & (~RTC_SEL_MASK)) | (RTC_SEL_MASK & RTC_FALLING_EDGE_INT);
	//RTC->RTC_PRC1R = (RTC->RTC_PRC1R & (~RTC_SEL_MASK)) | (RTC_SEL_MASK & RTC_LOW_LEVEL_INT);
	RTC->RTC_PRC1R |= Ala_IEN;
//	printf("RTC->RTC_PRC1R = %08x\r\n", RTC->RTC_PRC1R);
}

/*******************************************************************************
* Function Name  : RTC_GetTime
* Description    : 读取RTC计数器值
* Input          : None
*
* Output         : - t：tm结构体指针
*                       t->day:读取的RTC的day值
*                       t->hour:读取的RTC的hour值
*                       t->minute:读取的RTC的minute值
*                       t->second:读取的RTC的second值
* Return         : None
******************************************************************************/
void RTC_GetTime(tm *t)
{
	UINT32 tim;
#if 0
	/*
	tim = RTC->RTC_PRT1R;//RTC_PRTCR
	t->day = (UINT16)(tim&(0x7FFF));

	tim = RTC->RTC_PRT2R;

	t->second = (UINT8)(tim&0xff);
	t->minute = (UINT8)(tim>>8);
	t->hour = (UINT8)(tim>>16);
*/
#else
	tim = RTC->RTC_PRTCR;
	t->day = (UINT16)((tim>>17)&(0x7FFF));
	t->second = (UINT8)((tim)&0x3f);
	t->minute = (UINT8)((tim>>6)&0x3f);
	t->hour = (UINT8)((tim>>12)&0x1f);
#endif
//	printf("t->day = %d\r\n", t->day);
//	printf("t->hour = %d\r\n", t->hour);
//	printf("t->minute = %d\r\n", t->minute);
//	printf("t->second = %d\r\n", t->second);
}
