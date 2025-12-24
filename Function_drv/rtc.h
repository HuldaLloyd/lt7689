/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     rtc.h
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief
 *************************************************************************/

#ifndef _rtc_h
#define _rtc_h

#include "delay.h"
#include "data.h"
#include "bsp.h"

typedef struct
{
	volatile uint16_t w_year;
	volatile uint8_t w_month;
	volatile uint8_t w_date;
	volatile uint8_t hour;
	volatile uint8_t min;
	volatile uint8_t sec;
	volatile uint8_t week;
} _calendar_obj;
extern _calendar_obj calendar; // the Gregorian calendar

typedef struct DS1302DATA
{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t week;
} DS1302DATA;
extern struct DS1302DATA gRtcTime;

extern uint8_t RTC_One_Sec_Update;

#define DS1302_RST PCout(3)
#define DS1302_CLK PCout(1)
#define DS1302_DATIN PCin(2)
#define DS1302_DATOUT PCout(2)

#define DS1302_DAT_INPUT()        \
	{                             \
		GPIOC->CRL &= 0XFFFFF0FF; \
		GPIOC->CRL |= 8 << 8;     \
	}
#define DS1302_DAT_OUTPUT()       \
	{                             \
		GPIOC->CRL &= 0XFFFFF0FF; \
		GPIOC->CRL |= 3 << 8;     \
	}

#define CLOCKSTOP 0X80
#define CLOCKSTART 0X00

uint8_t LT_RtcInit(void);
uint8_t RTC_Set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);
uint8_t RTC_Get(void);
uint8_t RTC_Get_Week(uint16_t year, uint8_t month, uint8_t day);
uint8_t Is_Leap_Year(uint16_t year);
uint8_t LT_BcdToAscii(uint8_t bcdData);
	
/*For debug(DS1302)
	// 芯片寄存器地址定义 定义的写地址，读需要+1 
	#define DS1302_SEC_REG 0x80			// 秒数据地址
	#define DS1302_MIN_REG 0x82			// 分数据地址
	#define DS1302_HR_REG 0x84			// 时数据地址
	#define DS1302_DATE_REG 0x86		// 日数据地址
	#define DS1302_MONTH_REG 0x88		// 月数据地址
	#define DS1302_DAY_REG 0x8a			// 周数据地址
	#define DS1302_YEAR_REG 0x8c		// 年数据地址
	#define DS1302_CONTROL_REG 0x8e		// 写保护寄存器地址
	#define DS1302_CHARGER_REG 0x90		// 涓流充电寄存器 
	#define DS1302_CLKBURST_REG 0xbe	// 脉冲串寄存器  
	#define DS1302_RAM_BASE 0X30		// RAM基础地址
	
	void LT_Ds1302Write(uint8_t address,uint8_t dat);
	void LT_Ds1302WriteTimeAll(uint8_t start);
	void LT_Ds1302Readtime(void);
	uint8_t LT_Ds1302Read(uint8_t address);
*/
#endif
