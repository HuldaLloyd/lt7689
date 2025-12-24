// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : rtc_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef RTC_DRV_H_
#define RTC_DRV_H_

#include "memmap.h"
#include "rtc_reg.h"

#define RTC		                      ((RTC_TypeDef *)(PMURTC_BASE_ADDR))

#define RTC_SEL_MASK                (0x03 << 8)
#define RTC_LOW_LEVEL_INT           (0x00 << 8)
#define RTC_RISING_EDGE_INT         (0x01 << 8)
#define RTC_FALLING_EDGE_INT        (0x02 << 8)
#define RTC_RISING_FALLING_EDGE_INT (0x03 << 8)

#define DAY_ALARM                    0x01
#define HOUR_ALARM                   0x02
#define MINUTE_ALARM                 0x03
#define SECOND_ALARM                 0x04
#define ALL_ALARM                    0x05

#define IS_RTC_ALARM_MOD(MOD) (((MOD) == DAY_ALARM) ||    \
                               ((MOD) == HOUR_ALARM) ||   \
                               ((MOD) == MINUTE_ALARM) || \
                               ((MOD) == SECOND_ALARM) || \
                               ((MOD) == ALL_ALARM))


#define	Day_intf	    0x00400000
#define	Hou_intf	    0x00200000
#define	Min_intf	    0x00100000
#define	Sec_intf	    0x00080000
#define	Ala_intf	    0x00040000
#define	CLK_1KHz_intf	0x00020000
#define	CLK_32KHz_intf	0x00010000


typedef struct _tm{
	// date and time components
	UINT16 day;
	UINT8  hour;
	UINT8  minute;
	UINT8  second;
}tm;


extern UINT32 g_rtc_int_sta;

/*******************************************************************************
* Function Name  : RTC_ISR
* Description    : RTC中断处理
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void RTC_ISR(void);

/*******************************************************************************
* Function Name  : RTC_Init
* Description    : RTC初始化
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void RTC_Init(UINT8 clk_sel);

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
extern void RTC_SetTime(tm t);

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
extern void RTC_SetAlarm(tm t, UINT8 AlarmMod);

/*******************************************************************************
* Function Name  : RTC_GetTime
* Description    : 读取RTC计数器值
* Input          : None
*
* Output         : - t：tm结构体，
*                       t->day:读取的RTC的day值
*                       t->hour:读取的RTC的hour值
*                       t->minute:读取的RTC的minute值
*                       t->second:读取的RTC的second值
* Return         : None
******************************************************************************/
extern void RTC_GetTime(tm *t);




#endif /* RTC_DRV_H_ */
