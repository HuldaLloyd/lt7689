/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     rtc.c
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief
 *************************************************************************/

#include "rtc.h"
#include "rtc_drv.h"
#include "cpm_reg.h"
#include "sys.h"

_calendar_obj calendar;		// Clock structure

struct DS1302DATA gRtcTime = {18, 11, 25, 9, 42, 5, 7};
const uint8_t Clock_Time[7] = {19, 8, 8, 2, 5, 50, 4};

uint8_t RTC_One_Sec_Update = 0;

static uint8_t LT_AsciiToBcd(uint8_t asciiData)
{
	uint8_t bcdData = 0;
	bcdData = (((asciiData / 10) << 4) | ((asciiData % 10)));
	return bcdData;
}

uint8_t LT_BcdToAscii(uint8_t bcdData)
{
	uint8_t asciiData = 0;
	asciiData = (((bcdData & 0xf0) >> 4) * 10 + (bcdData & 0x0f));
	return asciiData;
}

uint8_t LT_RtcInit(void)
{
	UINT32 rtc_wclk_div = 0;

	rtc_wclk_div = g_sys_clk / 10000000;

	if (rtc_wclk_div >= 1)
		rtc_wclk_div--;

	/*For debug
	 Update RTC Counter(function case Dir=0)
	 NVIC_Init(2, 1, PMU_RTC_IRQn, 2);
	*/
	RTC->RTC_PRKEYR = 0x5aa55aa5;			// input key

	RTC->RTC_PRC1R &= (~WCLK_DIV_MASK);
	RTC->RTC_PRC1R |= (rtc_wclk_div << 3);	// 8 frequency division

	RTC->RTC_PRC1R &= ~Dir; 				// set dir = 0;
	RTC->RTC_PRC1R |= Sec_IEN;

	RTC_Get();
	if (calendar.w_year == 0 && calendar.w_month == 0 && calendar.w_date == 0)
	{
		RTC_Set(2023, 4, 7, 17, 3, 0);
	}
	else
	{
		RTC_Set(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);
	}

	DelayMS(50);
	NVIC_Init(2, 1, PMU_RTC_IRQn, 2); 		// Open RTC interrupt
	RTC_Get();
}

// RTC clock interrupt
void PMU_RTC_IRQHandler(void)
{
	UINT32 prc1r;
	uint8_t i = 0, j = 0;
	uint8_t rebuf[40];
	prc1r = RTC->RTC_PRC1R;

	if (((prc1r & Sec_intf) == Sec_intf) && ((prc1r & Sec_IEN) == Sec_IEN)) // second interrupt
	{
		RTC->RTC_PRC1R |= Sec_intf;

		RTC_Get();

		RTC_flag = 1;
		Clock_flag = 1;
		Count_flag = 1;
		for (i = 0; i < Count_sum; i++)
		{
			if (((var[gCount_Info[i].c_add * 2] << 8) + var[gCount_Info[i].c_add * 2 + 1]) == 1)
			{
				if (gCount_Info[i].dir == 0) // Count backwards
				{
					if (var[gCount_Info[i].t_add * 2 + 1] > 0)
							var[gCount_Info[i].t_add * 2 + 1] = var[gCount_Info[i].t_add * 2 + 1] - 1;
						
					else if (var[gCount_Info[i].t_add * 2 + 1] == 0 && var[gCount_Info[i].t_add * 2] > 0)
					{
						var[gCount_Info[i].t_add * 2] = var[gCount_Info[i].t_add * 2] - 1;
						var[gCount_Info[i].t_add * 2 + 1] = 0xFF;
					}
					else if (((var[gCount_Info[i].t_add * 2] << 8) + var[gCount_Info[i].t_add * 2 + 1]) == 0)
					{
						var[gCount_Info[i].c_add * 2] = 0;
						var[gCount_Info[i].c_add * 2 + 1] = 0;
						
						for (j = 0; j < 8; j++)
						{
							if (gCount_Info[i].var_addr[j] != 0xFFFF && gCount_Info[i].var_addr[j] < 0x7200)
							{
								var[gCount_Info[i].var_addr[j] * 2] = (gCount_Info[i].var[j] >> 8) & 0xFF;
								var[gCount_Info[i].var_addr[j] * 2 + 1] = gCount_Info[i].var[j] & 0xFF;

								if (gCount_Info[i].var_addr[j] >= 0x7000)
								{
									//reg_operation(gCount_Info[i].var_addr[j]);
									Count_reg[Count_reg_num] = gCount_Info[i].var_addr[j];
									Count_reg_num++;
								}
							}
						}

						if (gCount_Info[i].end_feedback == 1) // Feedback
						{
							rebuf[3] = 0x41;
							for (j = 0; j < 8; j++)
							{
								rebuf[4 * j + 4] = gCount_Info[i].var_addr[j] >> 8;
								rebuf[4 * j + 5] = gCount_Info[i].var_addr[j] & 0xFF;
								rebuf[4 * j + 6] = gCount_Info[i].var[j] >> 8;
								rebuf[4 * j + 7] = gCount_Info[i].var[j] & 0xFF;
							}
							LT_SendData_CRC_Frame(rebuf, 33);
						}
					}
				}
				else if (gCount_Info[i].dir == 1) // Positive number
				{
					if (((var[gCount_Info[i].t_add * 2] << 8) + var[gCount_Info[i].t_add * 2 + 1]) >=
						((var[gCount_Info[i].v_add * 2] << 8) + var[gCount_Info[i].v_add * 2 + 1]))
					{
						var[gCount_Info[i].c_add * 2] = 0;
						var[gCount_Info[i].c_add * 2 + 1] = 0;
						for (j = 0; j < 8; j++)
						{
							if (gCount_Info[i].var_addr[j] != 0xFFFF && gCount_Info[i].var_addr[j] < 0x7200)
							{
								var[gCount_Info[i].var_addr[j] * 2] = (gCount_Info[i].var[j] >> 8) & 0xFF;
								var[gCount_Info[i].var_addr[j] * 2 + 1] = gCount_Info[i].var[j] & 0xFF;

								if (gCount_Info[i].var_addr[j] >= 0x7000)
								{
									//reg_operation(gCount_Info[i].var_addr[j]);
									Count_reg[Count_reg_num] = gCount_Info[i].var_addr[j];
									Count_reg_num++;
									
								}
							}
						}
	
						if (gCount_Info[i].end_feedback == 1) // Feedback
						{
							rebuf[3] = 0x41;
							for (j = 0; j < 8; j++)
							{  
								rebuf[4 * j + 4] = gCount_Info[i].var_addr[j] >> 8;
								rebuf[4 * j + 5] = gCount_Info[i].var_addr[j] & 0xFF;
								rebuf[4 * j + 6] = gCount_Info[i].var[j] >> 8;
								rebuf[4 * j + 7] = gCount_Info[i].var[j] & 0xFF;
							}
							LT_SendData_CRC_Frame(rebuf, 33);
						}
					}

					if (((var[gCount_Info[i].c_add * 2] << 8) + var[gCount_Info[i].c_add * 2 + 1]) != 0x00)
					{
						if (var[gCount_Info[i].t_add * 2 + 1] < 0xFF)
							var[gCount_Info[i].t_add * 2 + 1] = var[gCount_Info[i].t_add * 2 + 1] + 1;
						else if (var[gCount_Info[i].t_add * 2 + 1] == 0xFF)
						{
							var[gCount_Info[i].t_add * 2] = var[gCount_Info[i].t_add * 2] + 1;
							var[gCount_Info[i].t_add * 2 + 1] = 0;
						}
					}
				}
			}
		}

		for (i = 0; i < Record_count; i++)
		{
			if (((var[gRecord_count_Info[i].c_add * 2] << 8) + var[gRecord_count_Info[i].c_add * 2 + 1]) == 1)
			{
				if (gRecord_count_Info[i].dir == 0) // Count backwards
				{
					if (var[gRecord_count_Info[i].t_add * 2 + 1] > 0)
						var[gRecord_count_Info[i].t_add * 2 + 1] = var[gRecord_count_Info[i].t_add * 2 + 1] - 1;
					else if (var[gRecord_count_Info[i].t_add * 2 + 1] == 0 && var[gRecord_count_Info[i].t_add * 2] > 0)
					{
						var[gRecord_count_Info[i].t_add * 2] = var[gRecord_count_Info[i].t_add * 2] - 1;
						var[gRecord_count_Info[i].t_add * 2 + 1] = 0xFF;
					}
					if (((var[gRecord_count_Info[i].t_add * 2] << 8) + var[gRecord_count_Info[i].t_add * 2 + 1]) == 0)
					{
						for (j = 0; j < 8; j++)
						{
							if (gRecord_count_Info[i].var_addr[j] != 0xfFFF && gRecord_count_Info[i].var_addr[j] < 0x7200)
							{
								var[gRecord_count_Info[i].var_addr[j] * 2] = (gRecord_count_Info[i].var[j] >> 8) & 0xFF;
								var[gRecord_count_Info[i].var_addr[j] * 2 + 1] = gRecord_count_Info[i].var[j] & 0xFF;

								if (gRecord_count_Info[i].var_addr[j] >= 0x7000)
								{
									//reg_operation(gRecord_count_Info[i].var_addr[j]);
									Count_reg[Count_reg_num] = gCount_Info[i].var_addr[j];
									Count_reg_num++;
								}
							}
						}

						if (gRecord_count_Info[i].end_feedback == 1) // Feedback
						{
							rebuf[3] = 0x41;
							for (j = 0; j < 8; j++)
							{
								rebuf[4 * j + 4] = gRecord_count_Info[i].var_addr[j] >> 8;
								rebuf[4 * j + 5] = gRecord_count_Info[i].var_addr[j] & 0xFF;
								rebuf[4 * j + 6] = gRecord_count_Info[i].var[j] >> 8;
								rebuf[4 * j + 7] = gRecord_count_Info[i].var[j] & 0xFF;
							}
							LT_SendData_CRC_Frame(rebuf, 33);
						}
						gRecord_count_Info[i].v_add = gRecord_count_Info[Record_count - 1].v_add;
						gRecord_count_Info[i].t_add = gRecord_count_Info[Record_count - 1].t_add;
						gRecord_count_Info[i].c_add = gRecord_count_Info[Record_count - 1].c_add;
						gRecord_count_Info[i].dir = gRecord_count_Info[Record_count - 1].dir;
						gRecord_count_Info[i].end_feedback = gRecord_count_Info[Record_count - 1].end_feedback;
						gRecord_count_Info[i].var[0] = gRecord_count_Info[Record_count - 1].var[0];
						gRecord_count_Info[i].var_addr[0] = gRecord_count_Info[Record_count - 1].var_addr[0];
						gRecord_count_Info[i].var[1] = gRecord_count_Info[Record_count - 1].var[1];
						gRecord_count_Info[i].var_addr[1] = gRecord_count_Info[Record_count - 1].var_addr[1];
						gRecord_count_Info[i].var[2] = gRecord_count_Info[Record_count - 1].var[2];
						gRecord_count_Info[i].var_addr[2] = gRecord_count_Info[Record_count - 1].var_addr[2];
						gRecord_count_Info[i].var[3] = gRecord_count_Info[Record_count - 1].var[3];
						gRecord_count_Info[i].var_addr[3] = gRecord_count_Info[Record_count - 1].var_addr[3];
						gRecord_count_Info[i].var[4] = gRecord_count_Info[Record_count - 1].var[4];
						gRecord_count_Info[i].var_addr[4] = gRecord_count_Info[Record_count - 1].var_addr[4];
						gRecord_count_Info[i].var[5] = gRecord_count_Info[Record_count - 1].var[5];
						gRecord_count_Info[i].var_addr[5] = gRecord_count_Info[Record_count - 1].var_addr[5];
						gRecord_count_Info[i].var[6] = gRecord_count_Info[Record_count - 1].var[6];
						gRecord_count_Info[i].var_addr[6] = gRecord_count_Info[Record_count - 1].var_addr[6];
						gRecord_count_Info[i].var[7] = gRecord_count_Info[Record_count - 1].var[7];
						gRecord_count_Info[i].var_addr[7] = gRecord_count_Info[Record_count - 1].var_addr[7];
						i--;
						Record_count--;
					}
				}
				else if (gRecord_count_Info[i].dir == 1) // Positive number
				{
					if (var[gRecord_count_Info[i].t_add * 2 + 1] < 0xFF)
						var[gRecord_count_Info[i].t_add * 2 + 1] = var[gRecord_count_Info[i].t_add * 2 + 1] + 1;
					else if (var[gRecord_count_Info[i].t_add * 2 + 1] == 0xFF)
					{
						var[gRecord_count_Info[i].t_add * 2] = var[gRecord_count_Info[i].t_add * 2] + 1;
						var[gRecord_count_Info[i].t_add * 2 + 1] = 0;
					}

					if (((var[gRecord_count_Info[i].t_add * 2] << 8) + var[gRecord_count_Info[i].t_add * 2 + 1]) >=
						((var[gRecord_count_Info[i].v_add * 2] << 8) + var[gRecord_count_Info[i].v_add * 2 + 1]))
					{
						for (j = 0; j < 8; j++)
						{
							if (gRecord_count_Info[i].var_addr[j] != 0xFFFF && gRecord_count_Info[i].var_addr[j] < 0x7200)
							{
								var[gRecord_count_Info[i].var_addr[j] * 2] = (gRecord_count_Info[i].var[j] >> 8) & 0xFF;
								var[gRecord_count_Info[i].var_addr[j] * 2 + 1] = gRecord_count_Info[i].var[j] & 0xFF;

								if (gRecord_count_Info[i].var_addr[j] >= 0x7000)
								{
									//reg_operation(gRecord_count_Info[i].var_addr[j]);
									Count_reg[Count_reg_num] = gCount_Info[i].var_addr[j];
									Count_reg_num++;
								}
							}
						}

						if (gRecord_count_Info[i].end_feedback == 1) // Feedback
						{
							rebuf[3] = 0x41;
							for (j = 0; j < 8; j++)
							{
								rebuf[4 * j + 4] = gRecord_count_Info[i].var_addr[j] >> 8;
								rebuf[4 * j + 5] = gRecord_count_Info[i].var_addr[j] & 0xFF;
								rebuf[4 * j + 6] = gRecord_count_Info[i].var[j] >> 8;
								rebuf[4 * j + 7] = gRecord_count_Info[i].var[j] & 0xFF;
							}
							LT_SendData_CRC_Frame(rebuf, 33);
						}

						gRecord_count_Info[i].v_add = gRecord_count_Info[Record_count - 1].v_add;
						gRecord_count_Info[i].t_add = gRecord_count_Info[Record_count - 1].t_add;
						gRecord_count_Info[i].c_add = gRecord_count_Info[Record_count - 1].c_add;
						gRecord_count_Info[i].dir = gRecord_count_Info[Record_count - 1].dir;
						gRecord_count_Info[i].end_feedback = gRecord_count_Info[Record_count - 1].end_feedback;
						gRecord_count_Info[i].var[0] = gRecord_count_Info[Record_count - 1].var[0];
						gRecord_count_Info[i].var_addr[0] = gRecord_count_Info[Record_count - 1].var_addr[0];
						gRecord_count_Info[i].var[1] = gRecord_count_Info[Record_count - 1].var[1];
						gRecord_count_Info[i].var_addr[1] = gRecord_count_Info[Record_count - 1].var_addr[1];
						gRecord_count_Info[i].var[2] = gRecord_count_Info[Record_count - 1].var[2];
						gRecord_count_Info[i].var_addr[2] = gRecord_count_Info[Record_count - 1].var_addr[2];
						gRecord_count_Info[i].var[3] = gRecord_count_Info[Record_count - 1].var[3];
						gRecord_count_Info[i].var_addr[3] = gRecord_count_Info[Record_count - 1].var_addr[3];
						gRecord_count_Info[i].var[4] = gRecord_count_Info[Record_count - 1].var[4];
						gRecord_count_Info[i].var_addr[4] = gRecord_count_Info[Record_count - 1].var_addr[4];
						gRecord_count_Info[i].var[5] = gRecord_count_Info[Record_count - 1].var[5];
						gRecord_count_Info[i].var_addr[5] = gRecord_count_Info[Record_count - 1].var_addr[5];
						gRecord_count_Info[i].var[6] = gRecord_count_Info[Record_count - 1].var[6];
						gRecord_count_Info[i].var_addr[6] = gRecord_count_Info[Record_count - 1].var_addr[6];
						gRecord_count_Info[i].var[7] = gRecord_count_Info[Record_count - 1].var[7];
						gRecord_count_Info[i].var_addr[7] = gRecord_count_Info[Record_count - 1].var_addr[7];
						i--;
						Record_count--;
					}
				}
			}
		}
	}
}

// month           1  2  3  4  5  6  7  8  9  10 11 12
// leap year       31 29 31 30 31 30 31 31 30 31 30 31
// non-leap years  31 28 31 30 31 30 31 31 30 31 30 31
/********************************************************************************
 * Function Name	: Is_Leap_Year
 * Description  	: Judge whether it is a leap year.
 * Input        	: - year:
 * Output       	: None
 * Return       	: Is this year a leap year. 1:yes, 0:no
 *********************************************************************************/

uint8_t Is_Leap_Year(uint16_t year)
{
	if (year % 4 == 0) // Must be divisible by 4
	{
		if (year % 100 == 0)
		{
			if (year % 400 == 0)
				return 1; // If it ends with 00, it should be divisible by 400
			else
				return 0;
		}
		else
			return 1;
	}
	else
		return 0;
}

// Monthly data table
uint8_t const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5}; // Monthly correction data sheet
// Date table of the month in the ordinary year
const uint8_t mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/********************************************************************************
* Function Name	: RTC_Set
* Description  	: set clock.
* Input        	: - syear: Only gregorian 1970-2099 allowed
				  - smon:
				  - sday:
				  - hour:
				  - min:
				  - sec:
* Output       	: None
* Return       	: week
*********************************************************************************/
uint8_t RTC_Set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
	uint16_t t;
	tm timer;
	UINT16 days = 0;

	if (syear < 2010 || syear > 2099)
		return 1;
	for (t = 2010; t < syear; t++)	// Add the seconds of all years
	{
		if (Is_Leap_Year(t))
			days += 366;			// Days in leap year
		else
			days += 365;			// Days in non-leap years
	}
	smon -= 1;
	for (t = 0; t < smon; t++)		// Add the number of seconds in the previous month
	{
		days += (uint32_t)mon_table[t]; // Add months and seconds
		if (Is_Leap_Year(syear) && t == 1)
			days++; 				// Increase the number of seconds of one day in February of leap year
	}
	days += (sday - 1);				// Add the seconds of the previous date

	timer.day = days;
	if (hour >= 24)
		hour = 23;
	timer.hour = hour;
	if (min >= 60)
		min = 59;
	timer.minute = min;
	if (sec >= 60)
		sec = 59;
	timer.second = sec;

	RTC_SetTime(timer);

	return 0;
}

/********************************************************************************
* Function Name	: RTC_Get
* Description  	: Get calendar date and time.
* Input        	: - year: Only gregorian 1901-2099 allowed
				  - month: Gregorian month
				  - day: Gregorian day
* Output       	: None
* Return       	: week
*********************************************************************************/
uint8_t RTC_Get(void)
{
	static uint16_t daycnt = 0;

	uint32_t temp = 0;
	uint16_t temp1 = 0;
	UINT16 days = 0;
	tm timer;

	RTC_GetTime(&timer);
	days = timer.day;

	temp = days;					// Get Days
	if (daycnt != temp)				// More than a day
	{
		daycnt = temp;
		temp1 = 2010; 				// From 2010
		while (temp >= 365)
		{
			if (Is_Leap_Year(temp1))// leap year
			{
				if (temp >= 366)
					temp -= 366;	// leap year seconds
				else
				{
					temp1++;
					break;
				}
			}
			else
				temp -= 365;		// non-leap year
			temp1++;
		}
		calendar.w_year = temp1;	// Get year
		temp1 = 0;
		while (temp >= 28)			// More than a month
		{
			if (Is_Leap_Year(calendar.w_year) && temp1 == 1)	// Is that leap year in February
			{
				if (temp >= 29)
					temp -= 29;		// leap year seconds
				else
					break;
			}
			else
			{
				if (temp >= mon_table[temp1])
					temp -= mon_table[temp1];	// non-leap year
				else
					break;
			}
			temp1++;
		}
		calendar.w_month = temp1 + 1;			// Get month
		calendar.w_date = temp + 1;				// Get date
	}
	calendar.hour = timer.hour;
	calendar.min = timer.minute;
	calendar.sec = timer.second;
	calendar.week = RTC_Get_Week(calendar.w_year, calendar.w_month, calendar.w_date);	// Get week

	return 0;
}

/********************************************************************************
* Function Name	: RTC_Get_Week
* Description  	: Get the week according to the gregorian calendar date.
* Input        	: - year: Only gregorian 1901-2099 allowed
				  - month: Gregorian month
				  - day: Gregorian day
* Output       	: None
* Return       	: week
*********************************************************************************/
uint8_t RTC_Get_Week(uint16_t year, uint8_t month, uint8_t day)
{
	uint16_t temp2;
	uint8_t yearH, yearL;

	yearH = year / 100;
	yearL = year % 100;
	// If it is the 21st century, add 100 years
	if (yearH > 19)
		yearL += 100;
	// The number of leap years passed is only counted after 1900
	temp2 = yearL + yearL / 4;
	temp2 = temp2 % 7;
	temp2 = temp2 + day + table_week[month - 1];
	if (yearL % 4 == 0 && month < 3)
		temp2--;
	return (temp2 % 7);
}
