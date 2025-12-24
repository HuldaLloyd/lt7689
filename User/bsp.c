/*******************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     bsp.c
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief    This file provides UartTFT Application display function
 ********************************************************************************/

#include "bsp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pwm.h"
#include "pit.h"
#include "ctp.h"
#include "iic.h"
#include "data.h"
#include "flash.h"
#include "math.h"
#include "dac.h"
#include "levetop.h"
#include "eflash_drv.h"

uint32_t Display_Layer = 0;
uint8_t Flash_type = 1;

/****configuration parameter variabl****/
uint8_t buzzer = 0;
uint16_t Start_page = 0;
uint8_t SCI_C0 = 0x5A;
uint8_t SCI_C1 = 0xA5;
uint8_t CRC_Enable_Flag = 1;
uint8_t CRC_Feedback_Flag = 1;
uint8_t Screen_DIR = 0;
uint8_t TFT_bitcolor = 0;
uint8_t UI_rotate = 0;
uint32_t UI_rotate_addr = 0;
uint8_t parity_flag = 0;	
uint8_t multi_language = 0;
uint8_t multi_language_count = 0;
/***********************/

/****ModBus operating variables*****/
uint16_t Cnt_ModbusTX = 0; // Record the offset position of Modbus operation parameters
uint16_t Sum_ModbusTX = 0; // Total length of Modbus operation parameters

/****picture information*****/
uint16_t pic_id = 0;
uint16_t pic_id_temp = 0;
uint32_t pic_add = 0;
uint16_t pic_w = 0;
uint16_t pic_h = 0;
uint32_t pic_len = 0;
uint8_t pic_flag = 0;

/****Backlight information*****/
uint16_t gDutyBuf[64] = {0, 5, 30, 47, 49, 53, 57, 61, 65, 69, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 137, 144, 151, 158, 165, 172, 180,
						 188, 196, 204, 212, 220, 228, 236, 244, 254, 264, 274, 284, 294, 304, 314, 324, 334, 344, 354, 364, 374, 384, 400, 430, 460, 490, 530,
						 570, 610, 650, 690, 730, 760, 799};
uint8_t gDutyflag = 0;

uint32_t Backlight_count = 0;
uint8_t Backlight_flag = 1;
uint8_t Backlight_first_touch = 0;
uint8_t Backlight_touch = 0;

/**********/
volatile uint8_t var[58368] = {0};			// variable buffer
volatile uint8_t var_temp[58368] = {0};		// variable buffer
uint8_t config_buff[PageCFG_Size] = {0};	// Display Information Profile Variables
uint8_t Display_buff[StrLen_Size];			// display buffer
uint16_t Curve_buff[Curve_Size][1024] = {0};// urve data buffer: 8 channels, 16KB in total
uint16_t Curve_count[Curve_Size] = {0};		// Curve length
uint8_t curve_update_flag[Curve_Size];

/****data display*****/
uint8_t data_D_num = 0;						// Width used to recorddata display
uint16_t data_D_w[Data_wLen_Size] = {0};	// Width used to recorddata display
uint16_t data_w = 0;
/*************/

/*picture data display*/
uint8_t pdata_D_num;						// width used to record picture data display
uint16_t pdata_D_w[pData_wLen_Size] = {0};	// width used to record picture data display
/*************/

/****encoder****/
uint16_t gEncoder_addr[40] = {0};
uint16_t Check_box_add = 0xFFFF;
uint8_t Check_box_flag = 0;
uint16_t gEncoder_count = 0;
uint16_t gEncoder_num = 0;
uint8_t knob_flag = 0;
uint8_t gEncoderFlag = 0;
/**************/

/****Touch public****/
uint8_t button_Press_flag = 0;				// Other touch cannot act after the touch is pressed
uint8_t Button_Key_Code_Flag = 0;
uint16_t Button_Key_Code = 0;
/**********/

/****Vasic touch****/
uint8_t Basci_count = 0;
uint8_t Basci_flag = 0;
uint8_t Basci_num = 0;
Basci_Info gBasci_Info[Basci_Size];
/******************/

/****Variable adjustment****/
uint8_t Adj_count = 0;
uint8_t Adj_flag = 0;
uint8_t Adj_num = 0;
uint8_t Adj_time = 0;
uint8_t Adj_loop = 0;
Adj_Info gAdj_Info[Adjust_Size];
/***************/

/****sliding progress bar****/
uint8_t Slide_count = 0;
uint8_t Slide_flag = 0;
uint8_t slide_first_touch = 0;
uint8_t slide_num = 0;
Slide_Info gSlide_Info[Slide_Size];
/**************************/

/****data keyboard****/
Dio_Info gDio_Info[Dio_Size];
uint8_t Dio_count = 0;
uint8_t Dio_num = 0;
uint8_t Dio_flag = 0;
/*******************/

/****keyboard keys****/
Kb_Info gKb_Info[KeyBoard_Size];
uint8_t Kb_count = 0;
uint8_t Kb_num = 0;
uint8_t Kb_flag = 0;
uint8_t DioKb_flag = 0;		// data input flag
uint8_t dot_flag;
uint8_t i_len = 0;
uint8_t d_len = 0;
uint16_t input_len = 0;
uint8_t neg_flag = 0;
uint16_t input_w = 0;
uint8_t cur_w = 0;			// cursor width
uint16_t cur_x = 0;
uint16_t cur_y = 0;
uint16_t Cusor_time = 0;	// cursor blink time
uint8_t Cusor_sw = 0;
/****************/

/****sliding gesture****/
Gesture_Info gGesture_Info;
/***************/

/****Slide screen****/
Sld_sc_Info gSld_sc_Info;
/**********/

/****ASCII keyboard****/
Aio_Info gAio_Info[20];
uint8_t Aio_count = 0;
uint8_t Aio_num = 0;
uint8_t Aio_flag = 0;
uint8_t AioKb_flag = 0;
uint8_t CapsLock = 0; 		// Caps lock
/***************/

/****GBK keyboard****/
GBKio_Info gGBKio_Info[GBKio_Size];
uint8_t GBKio_count = 0;
uint8_t GBKio_num = 0;
uint8_t GBKio_flag = 0;
uint8_t GBKioKb_flag = 0;
uint8_t GBK_PY_length = 0;
uint8_t GBK_CN_EN = 0;
uint8_t GBK_PY_buff[8] = {0};
uint16_t GBK_PY_num = 0;
uint8_t GBK_len = 0;
uint32_t GBK_PY_addr = 0;
uint8_t GBK_linenum = 0;
/***************/

/****Multivariable control****/
uint8_t VarKey_count = 0;
uint8_t VarKey_flag = 0;
uint8_t VarKey_num = 0;
VarKey_Info gVarKey_Info[Batch_Size];
/******************/

/****Oop-up window****/
uint8_t Pop_up_count = 0;
uint8_t Pop_up_flag = 0;
uint8_t Pop_up_num = 0;
uint8_t Pop_up_press = 0;
uint8_t Pop_bgflag = 0;
uint16_t Pop_up_x = 0;
uint16_t Pop_up_y = 0;
uint16_t Pop_up_id = 0;
uint16_t Pop_UI_rotate_id = 0;
//uint16_t 
Pop_up_Info gPop_up_Info[PopUp_Size];
/***********/

uint32_t CRC32_Table[256];

uint32_t GetCrc32(uint8_t *InStr,uint32_t len,uint32_t value)
{
    uint32_t i;
    uint32_t Crc;
    uint8_t *pch;

    Crc=value;
    pch = InStr;

    for(i=0; i<len; i++)
    {
        Crc = (Crc >> 8) ^ CRC32_Table[(Crc&0xFF)^(*pch)];
        pch++;
    }
    return Crc;
}

void Make_CRC32_Table(void)
{
	uint32_t c;
	uint32_t i = 0;
	uint32_t bit = 0;

	for(i = 0; i < 256; i++)
	{
		c  = (uint32_t)i;

		for(bit = 0; bit < 8; bit++)
		{
			if(c&1)
			{
				c = (c >> 1)^(0xEDB88320);
			}
			else
			{
				c =  c >> 1;
			}
		}
		CRC32_Table[i] = c;
	}
}

void LT_ReceiveCmd(uint8_t *buf)
{
	uint16_t i = 0;
	uint16_t j = 0;
	uint8_t cmd = 0;
	uint8_t rebuf[2100];
	uint16_t rxCrc, mcuCrc;
	uint16_t addr = 0xFFFF;
	uint8_t num = 0, cnt = 0;
	uint16_t temp = 0,count1 = 0, count2 = 0;
	uint32_t eflash_addr = 0, len = 0;
	uint32_t File_CRC = 0;
	gUsartRx.Flag = 0;

	if (CRC_Enable_Flag)
	{
		rxCrc = buf[gUsartRx.Count - 1];
		rxCrc = (rxCrc << 8) + buf[gUsartRx.Count - 2];
		//mcuCrc = CRC16(&buf[1], gUsartRx.Count - 3);
		if (buf[0] != 0)
		{
			mcuCrc = CRC16(&buf[1], gUsartRx.Count - 3);
		}
		else if (buf[0] == 0)
		{
			mcuCrc = CRC16(&buf[3], gUsartRx.Count - 5);
		}
	}
	else
		rxCrc = mcuCrc;

	if (buf[0] != 0)
	{
		cmd = buf[1];
		if (rxCrc == mcuCrc) // CRC OK
		{
			if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
			{
				rebuf[3] = cmd;
				rebuf[4] = 0xFF;
				LT_SendData_CRC_Frame(rebuf, 2);
			}

			/***************************Read instruction***************************/
			if (cmd == 0x03)
			{
				addr = (buf[2] << 8) | buf[3];
				len = (buf[4] << 8) | buf[5];

				for (i = 0; i < 5; i++)
				{
					rebuf[3 + i] = buf[1 + i];
				}
				if (addr <= 0x6FFF) // Variable area
				{
					for (i = 0; i < len; i++)
					{
						rebuf[8 + 2 * i] = var[2 * (addr + i)];
						rebuf[9 + 2 * i] = var[2 * (addr + i) + 1];
					}
				}
				else if (addr >= 0x7000 && addr <= 0x71FF) // Register region
				{
					if (addr <= 0x7007 || addr + len >= 0x7002)
					{
						var[0x7002 * 2 + 1] = calendar.w_year - 2000;
						var[0x7003 * 2 + 1] = calendar.w_month;
						var[0x7004 * 2 + 1] = calendar.w_date;
						var[0x7005 * 2 + 1] = calendar.hour;
						var[0x7006 * 2 + 1] = calendar.min;
						var[0x7007 * 2 + 1] = calendar.sec;
					}
					for (i = 0; i < len; i++)
					{
						rebuf[8 + 2 * i] = var[2 * (addr + i)];
						rebuf[9 + 2 * i] = var[2 * (addr + i) + 1];
					}
				}
				LT_SendData_CRC_Frame(rebuf, 5 + len * 2);
			}
			/***************************Write instructions***************************/
			else if (cmd == 0x10)
			{
				addr = (buf[2] << 8) | buf[3];
				if (CRC_Enable_Flag)
					len = (buf[0] - 2 - 3) / 2;
				else
					len = (buf[0] - 3) / 2;

				if (addr <= 0x6FFF) // Variable area
				{
					// Save data
					for (i = 0; i < len; i++)
					{
						var[2 * (addr + i)] = buf[4 + 2 * i];
						var[2 * (addr + i) + 1] = buf[5 + 2 * i];
					}
				}
				else if (addr >= 0x7000 && addr <= 0x71FF) // Register address
				{
					for (i = 0; i < len; i++)
					{
						var[2 * (addr + i)] = buf[4 + 2 * i];
						var[2 * (addr + i) + 1] = buf[5 + 2 * i];
					}

					for (i = 0; i < len;)
					{
						if ((addr + i) == 0x7000) // Page
						{
							if (pic_id != (var[0x7000 * 2] << 8 | var[0x7000 * 2 + 1]))
							{
								Display_page(var[0x7000 * 2] << 8 | var[0x7000 * 2 + 1]);
							}
							i++;
						}
						else if ((addr + i) == 0x7001) // Backlight
						{
							if (var[0x7001 * 2 + 1] > 63)
								var[0x7001 * 2 + 1] = 63;
							if (var[0x7001 * 2 + 1] == 0x00)
							{
								Stop_PWM1();
								gDutyflag = 1;
							}
							else
							{
								if (gDutyflag == 1)
								{
									gDutyflag = 0;
									Start_PWM1();
								}
								if(var[0x7001 * 2 + 1] >= 64) var[0x7001 * 2 + 1] = 63;
								Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
							}
							Backlight_flag = 1;

							i++;
						}
						else if ((addr + i) == 0x7002) // Time
						{
							if (var[0x7002 * 2 + 1] > 99)
								var[0x7002 * 2 + 1] = 99;
							if (var[0x7003 * 2 + 1] > 12)
								var[0x7003 * 2 + 1] = 12;
							if (var[0x7004 * 2 + 1] > 31)
								var[0x7004 * 2 + 1] = 31;
							if (var[0x7005 * 2 + 1] > 23)
								var[0x7005 * 2 + 1] = 23;
							if (var[0x7006 * 2 + 1] > 59)
								var[0x7006 * 2 + 1] = 59;
							if (var[0x7007 * 2 + 1] > 59)
								var[0x7007 * 2 + 1] = 59;

							RTC_Set(2000 + var[0x7002 * 2 + 1], var[0x7003 * 2 + 1], var[0x7004 * 2 + 1], var[0x7005 * 2 + 1], var[0x7006 * 2 + 1], var[0x7007 * 2 + 1]);
							i += 6;
						}
						else if ((addr + i) == 0x7008) // Time setting
						{
							i++;
						}
						else if ((addr + i) == 0x7009) // NULL
						{
							i++;
						}
						else if ((addr + i) == 0x700a) // WAV
						{
							Wav_DAC_Init((var[0x700a * 2] << 8) | var[0x700a * 2 + 1]);
							i++;
						}
						else if ((addr + i) == 0x700b) // Sound volume
						{
							if (var[2 * 0x700b + 1] > 16)
								wav_reduce_para = 16;
							else
								wav_reduce_para = var[2 * 0x700b + 1];

							i++;
						}
						else if ((addr + i) == 0x700c) // RTP calibration
						{
							if (var[0x700c * 2 + 1] == 0x5A)
							{
	#if Touch_selection
								LT_TpAdjust();
	#endif
								var[0x700c * 2 + 1] = 0x00;
							}
							i++;
						}
						else if ((addr + i) == 0x700d) // Key code trigger
						{
							Button_Key_Code = (var[0x700d * 2] << 8) | var[0x700d * 2 + 1];
							Button_Key_Code_Flag = 1;
							i++;
						}
						else if ((addr + i) == 0x700e)
						{
							if(var[2 * 0x700e + 1] == 1)
							{
								if (Backlight_flag == 0)
								{	
									Backlight_count = 0;								
									Backlight_flag = 1;
									Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
									if (var[0x700f * 2 + 1] == 0)
										Start_PWM1();
								}
							}
							else if (var[2 * 0x700e + 1] == 0)
							{
								Backlight_count = 0;								
								Backlight_flag = 0;
								Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
								if (var[0x700f * 2 + 1] == 0)
									Start_PWM1();
							}
							i++;
						}
						else if ((addr + i) == 0x700f)
						{
							if (Backlight_flag == 0 && var[2 * 0x700e + 1] == 1)
							{
								Set_Timer1_Compare_Buffer(gDutyBuf[var[0x700f * 2 + 1]]);
								if (var[0x700f * 2 + 1] == 0)
									Stop_PWM1();
							}
							i++;
						}
						else if ((addr + i) == 0x7011)
						{
							if (var[2 * 0x7011] == 0xAA && var[2 * 0x7011 + 1] == 0x55)
							{
								var[2 * 0x7011] = 0;
								var[2 * 0x7011 + 1] = 0;
								SCI1->SCICR2 &=~(1<<2);
								SCI1->SCICR2 &=~(1<<3);
								SCI1->SCICR2 &=~(1<<5);
								
								PIT1->PCSR&=~PCSR_PIE;
								PIT1->PCSR&=~PCSR_EN;	
								APP(0x8000000);
							}
							i++;
						}
						else if ((addr + i) == 0x703f)
						{
							if (var[2 * 0x703f + 1] < multi_language_count && var[2 * 0x703f + 1] != multi_language)
							{

								uint32_t language_sum = 0,language_count = 0,L_count = 0;
								uint32_t language_addr = 0,L_addr,L_len = 0;
								uint16_t L_num;
								multi_language = var[2 * 0x703f + 1];
								
								if (addr_index[19] > 0)
								{
									Flash_Read_UI(rebuf,addr_index[18] + 8 * multi_language,8);
									language_addr = (rebuf[0]) + (rebuf[1] << 8) + (rebuf[2] << 16) + (rebuf[3] << 24);
									language_sum = (rebuf[4]) + (rebuf[5] << 8) + (rebuf[6] << 16) + (rebuf[7] << 24);
								}
								
								if (language_sum > 0)
								{
									if (language_sum - language_count >=2048)
											Flash_Read_UI(rebuf,language_addr,2048);
									else 
										Flash_Read_UI(rebuf,language_addr,language_sum);
									
									while(1)
									{
										WDT_FeedDog(); //Clear watchdog
										if (L_count + 4 > 2048)
										{
											if (language_sum - language_count >= 2048)
												Flash_Read_UI(rebuf,language_addr + language_count,2048);
											else 
												Flash_Read_UI(rebuf,language_addr + language_count,language_sum - language_count);
											L_count = 0;
												
										}
										L_addr = rebuf[L_count] + (rebuf[L_count + 1] << 8);
										L_len = rebuf[L_count + 2] + (rebuf[L_count + 3] << 8);
										L_count = L_count + 4;
										
										if(L_count + L_len > 2048)
										{
											if (language_sum - (language_count + 4) >= 2048)
												Flash_Read_UI(rebuf,language_addr + language_count + 4,2048);
											else 
												Flash_Read_UI(rebuf,language_addr + language_count + 4 ,language_sum - (language_count + 4));
											L_count = 0;
										}
										
										for (uint16_t ll = 0; ll < L_len; ll++)
										{
											var[L_addr * 2 + ll] = rebuf[L_count + ll];
										}
										language_count += L_len + 4;
										L_count += L_len;
										
										if(language_count >= language_sum)
											break;
									}
									
								}
								if(Pop_up_flag == 1)
								{
									Display_pop_up(Pop_up_id,Pop_up_x,Pop_up_y);
								}
								else Display_page(pic_id);
							}
							else
								var[2 * 0x703f + 1] = multi_language;

							i++;
						}
						else if ((addr + i) == 0x7040)
						{
							if (UI_rotate != var[2 * 0x7040 + 1])
							{ 
								UI_rotate = var[2 * 0x7040 + 1];
								if (UI_rotate == 0) 
								{
									UI_rotate_addr = 0;
									Screen_DIR = 0;
									Address_Index(); 		// Get address index
									
									if(GBKioKb_flag == 1 || DioKb_flag == 1 || AioKb_flag == 1)
									{
										if (Pop_up_flag == 0)
											Display_page(pic_id_temp);
										else if (Pop_up_flag == 1)
										{
											Display_Layer = LAY_BUFF7;
											Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
										}	
									}
									else
									{
										if (Pop_up_flag == 1)
										{
											Display_page(Pop_UI_rotate_id); 
										}
										else
										{
											Display_page(pic_id); 
										}
									}
									
								}
								else if (UI_rotate == 1)
								{  
									uint8_t check[8] = {0};
									uint32_t encryption1, encryption2;
									UI_rotate_addr = second_UI_add;
									Screen_DIR = 1;
									
									Flash_Read_UI(check, 0x80 + 0xEC, 8); 	
									encryption1 = check[0x4] | (check[0x5] << 8) | (check[0x6] << 16) | (check[0x7] << 24);
									encryption2 = check[0x0] | (check[0x1] << 8) | (check[0x2] << 16) | (check[0x3] << 24);
									
									if (encryption1 == ~encryption2)
									{
										Address_Index(); 		// Get address index
										if(GBKioKb_flag == 1 || DioKb_flag == 1 || AioKb_flag == 1)
										{
											if (Pop_up_flag == 0)
												Display_page(pic_id_temp);
											else if (Pop_up_flag == 1)
											{
												Display_Layer = LAY_BUFF7;
												Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
											}	
										}
										else
										{
											if (Pop_up_flag == 1)
											{
												Display_page(Pop_UI_rotate_id); 
											}
											else
											{
												Display_page(pic_id); 
											}
										}
									}
									else
									{
										UI_rotate_addr = 0;
										Screen_DIR = 0;
									}
									
								}
								else if (UI_rotate == 2)
								{
									
								}
								else if (UI_rotate == 3)
								{
									
								}
							}
							i++;
						}
						else
						{
							i++;
						}
					}
				}
				else if (addr >= 0xC001 && addr <= 0xCFFF) // Curve channel address for write
				{
					temp = addr & 0xFFF;
					for (i = 0; i < Curve_Size; i++)
					{
						if (temp & (1 << i))
							num++; // Record the number of channel groups
					}

					i = 0;
					j = 0;
					while (j < len) // Store data to corresponding channels respectively
					{
						if (temp & (1 << i))
						{
							Curve_buff[i][Curve_count[i]] = buf[4 + 2 * j] << 8 | buf[5 + 2 * j];
							Curve_count[i]++;
							if (Curve_count[i] >= 1024)
							{
								Curve_count[i] = 0;
							}
							curve_update_flag[i] = 1;
							j++;
						}
						i++;
						if (i >= Curve_Size)
							i = 0;
					}

					if (len % num) // When multi-channel buffer data is triggered, if some channel data is missing, the missing part will be supplemented
					{
						for (i = 0; i < Curve_Size; i++)
						{
							if (temp & (1 << i))
								cnt++;
							if (cnt > len % num && (temp & (1 << i)))
							{
								if (Curve_count[i] != 0)
									Curve_buff[i][Curve_count[i]] = Curve_buff[i][Curve_count[i] - 1];
								else
									Curve_buff[i][0] = Curve_buff[i][1023];

								Curve_count[i]++;
								if (Curve_count[i] >= 1024)
								{
									Curve_count[i] = 0;
								}
							}
						}
					}
				}
				else if (addr >= 0xE001 && addr <= 0xEFFF) // Curve channel address for clear
				{
					temp = addr & 0xFFF;

					i = 0;
					j = 0;
					for (i = 0; i < Curve_Size; i++)
					{
						if (temp & (1 << i))
						{
							for (j = 0; j < 1024; j++)
							{
								Curve_buff[i][j] = 0x00;
							}

							Curve_count[i] = 0;
							curve_update_flag[i] = 1;
						}
					}
				}
			}
		}
		else // CRC NG
		{
			if (CRC_Feedback_Flag) // CRC feedback is the default
			{
				rebuf[3] = cmd;
				rebuf[4] = 0x00;
				LT_SendData_CRC_Frame(rebuf, 2);
			}
		}
	}
	else if (buf[0] == 0)
	{
		cmd = buf[3];
		if (rxCrc == mcuCrc) // CRC OK
		{
			if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
			{
				rebuf[3] = cmd;
				rebuf[4] = 0xFF;
				LT_SendData_CRC_Frame(rebuf, 2);
			}
			
			if (cmd == 0x61)
			{
				len = (buf[1] << 8) | buf[2];
				eflash_addr = 0x804B000 + ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]);
				EFLASH_Init(g_sys_clk / 1000);
				EFLASH_SetWritePermission();

				temp = EFLASH_Write(eflash_addr,&buf[8],len-7);

				EFLASH_ClrWritePermission();
				
				if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
				{
					rebuf[5] = cmd;
					rebuf[6] = buf[4];
					rebuf[7] = buf[5];
					rebuf[8] = buf[6];
					rebuf[9] = buf[7];
					rebuf[10] = 0x4F;
					rebuf[11] = 0x4B;
			
					LT_SendFlashData_CRC_Frame(rebuf, 7);
				}
			}
			else if (cmd == 0x62)
			{
				len = (buf[8] << 8) | buf[9];
				if (len > 2048) len = 2048;
				eflash_addr = 0x804B000 + ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]);
				EFLASH_Init(g_sys_clk / 1000);
				EFLASH_SetWritePermission();
				EFLASH_WordsRead_8(eflash_addr,&rebuf[10],len);
				EFLASH_ClrWritePermission();
				
				if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
				{
					rebuf[5] = cmd;
					rebuf[6] = buf[4];
					rebuf[7] = buf[5];
					rebuf[8] = buf[6];
					rebuf[9] = buf[7];
					LT_SendFlashData_CRC_Frame(rebuf, len + 5);
				}
			}
			else if (cmd == 0x63)
			{
				Make_CRC32_Table();
				len = (buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | buf[11];
				count1 = len / 2048;
				count2 = len % 2048;
				eflash_addr = 0x804B000 + ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]);
				EFLASH_Init(g_sys_clk / 1000);
				EFLASH_SetWritePermission();
				for(i = 0; i < count1; i++)
				{
					WDT_FeedDog();
					EFLASH_WordsRead_8(eflash_addr + 2048 * i,rebuf,2048);
					File_CRC = GetCrc32(rebuf,2048,File_CRC);
				}
				if(count2 > 0)
				{
					EFLASH_WordsRead_8(eflash_addr + 2048 * i,rebuf,count2);
					File_CRC = GetCrc32(rebuf,count2,File_CRC);
				}
				EFLASH_ClrWritePermission();
				
				if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
				{
					rebuf[5] = cmd;
					rebuf[6] = buf[4];
					rebuf[7] = buf[5];
					rebuf[8] = buf[6];
					rebuf[9] = buf[7];
					rebuf[10] = (File_CRC >> 24) & 0xFF;
					rebuf[11] = (File_CRC >> 16) & 0xFF;
					rebuf[12] = (File_CRC >> 8) & 0xFF;
					rebuf[13] = File_CRC & 0xFF;
					LT_SendFlashData_CRC_Frame(rebuf, 9);
				}
			}
			else if (cmd == 0x64)
			{
				len = (buf[1] << 8) | buf[2];
				eflash_addr = ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]);

				Flash_Write_NoCheck(&buf[8],eflash_addr,len-7);
			
				if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
				{
					rebuf[5] = cmd;
					rebuf[6] = buf[4];
					rebuf[7] = buf[5];
					rebuf[8] = buf[6];
					rebuf[9] = buf[7];
					rebuf[10] = 0x4F;
					rebuf[11] = 0x4B;
			
					LT_SendFlashData_CRC_Frame(rebuf, 7);
				}
			}
			else if (cmd == 0x65)
			{
				len = (buf[8] << 8) | buf[9];
				if (len > 2048) len = 2048;
				eflash_addr = ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]);

				Flash_Read(&rebuf[10],eflash_addr,len);
		
				if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
				{
					rebuf[5] = cmd;
					rebuf[6] = buf[4];
					rebuf[7] = buf[5];
					rebuf[8] = buf[6];
					rebuf[9] = buf[7];
					LT_SendFlashData_CRC_Frame(rebuf, len + 5);
				}
			}
			else if (cmd == 0x66)
			{
				Make_CRC32_Table();
				len = (buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | buf[11];
				count1 = len / 2048;
				count2 = len % 2048;
				eflash_addr = ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]);
				
				for(i = 0; i < count1; i++)
				{
					WDT_FeedDog();
					Flash_Read(rebuf,eflash_addr + 2048 * i,2048);
					File_CRC = GetCrc32(rebuf,2048,File_CRC);
				}
				
				if(count2 > 0)
				{
					Flash_Read(rebuf,eflash_addr + 2048 * i,count2);
					File_CRC = GetCrc32(rebuf,count2,File_CRC);
				}

				if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
				{
					rebuf[5] = cmd;
					rebuf[6] = buf[4];
					rebuf[7] = buf[5];
					rebuf[8] = buf[6];
					rebuf[9] = buf[7];
					rebuf[10] = (File_CRC >> 24) & 0xFF;
					rebuf[11] = (File_CRC >> 16) & 0xFF;
					rebuf[12] = (File_CRC >> 8) & 0xFF;
					rebuf[13] = File_CRC & 0xFF;
					LT_SendFlashData_CRC_Frame(rebuf, 9);
				}
			}
		}
		else // CRC NG
		{
			if (CRC_Feedback_Flag) // CRC feedback is the default
			{
				rebuf[3] = cmd;
				rebuf[4] = 0x00;
				LT_SendData_CRC_Frame(rebuf, 2);
			}
		}
	}
	
	gUsartRx.Flag = 0;
	gUsartRx.Count = 0; // Clear to receive the next data
	LT_ClearRxBuf();	// Clear receive buffer
}

#if (UARTBUS_OPTION == 3 || UARTBUS_OPTION == 4)
void LT_Hid_ReceiveCmd(uint8_t *buf)
{
	uint16_t i = 0;
	uint16_t j = 0;
	uint8_t cmd = 0;
	uint8_t rebuf[2100];
	uint16_t rxCrc, mcuCrc;
	uint16_t addr = 0xFFFF;
	uint8_t num = 0, cnt = 0;
	uint16_t temp = 0,count1 = 0, count2 = 0;
	uint32_t eflash_addr = 0, len = 0;
	uint32_t File_CRC = 0;
	
	uint32_t language_sum = 0,language_count = 0,L_count = 0;
	uint32_t language_addr = 0,L_addr,L_len = 0;
	uint16_t L_num;
	gHidRx.Flag = 0;
	

	if (CRC_Enable_Flag)
	{
		rxCrc = buf[gHidRx.Count - 1];
		rxCrc = (rxCrc << 8) + buf[gHidRx.Count - 2];
		if (buf[0] != 0)
		{
			mcuCrc = CRC16(&buf[1], gHidRx.Count - 3);
		}
		else if (buf[0] == 0)
		{
			mcuCrc = CRC16(&buf[3], gHidRx.Count - 5);
		}
	}
	else
		rxCrc = mcuCrc;

	if (buf[0] != 0)
	{
		cmd = buf[1];

		if (rxCrc == mcuCrc) // CRC OK
		{
			if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
			{
				rebuf[3] = cmd;
				rebuf[4] = 0xFF;
				LT_SendData_CRC_Frame(rebuf, 2);
				DelayUS(1200);
			}

			/***************************Read instruction***************************/
			if (cmd == 0x03)
			{
				addr = (buf[2] << 8) | buf[3];
				len = (buf[4] << 8) | buf[5];

				for (i = 0; i < 5; i++)
				{
					rebuf[3 + i] = buf[1 + i];
				}
				if (addr <= 0x6FFF) // Variable area
				{
					for (i = 0; i < len; i++)
					{
						rebuf[8 + 2 * i] = var[2 * (addr + i)];
						rebuf[9 + 2 * i] = var[2 * (addr + i) + 1];
					}
				}
				else if (addr >= 0x7000 && addr <= 0x71FF) // Register region
				{
					if (addr <= 0x7007 || addr + len >= 0x7002)
					{
						var[0x7002 * 2 + 1] = calendar.w_year - 2000;
						var[0x7003 * 2 + 1] = calendar.w_month;
						var[0x7004 * 2 + 1] = calendar.w_date;
						var[0x7005 * 2 + 1] = calendar.hour;
						var[0x7006 * 2 + 1] = calendar.min;
						var[0x7007 * 2 + 1] = calendar.sec;
					}
					for (i = 0; i < len; i++)
					{
						rebuf[8 + 2 * i] = var[2 * (addr + i)];
						rebuf[9 + 2 * i] = var[2 * (addr + i) + 1];
					}
				}
				LT_SendData_CRC_Frame(rebuf, 5 + len * 2);
			}
			/***************************Write instructions***************************/
			else if (cmd == 0x10)
			{
				addr = (buf[2] << 8) | buf[3];
				if (CRC_Enable_Flag)
					len = (buf[0] - 2 - 3) / 2;
				else
					len = (buf[0] - 3) / 2;

				if (addr <= 0x6FFF) // Variable area
				{
					// Save data
					for (i = 0; i < len; i++)
					{
						var[2 * (addr + i)] = buf[4 + 2 * i];
						var[2 * (addr + i) + 1] = buf[5 + 2 * i];
					}
				}
				else if (addr >= 0x7000 && addr <= 0x71FF) // Register address
				{
					for (i = 0; i < len; i++)
					{
						var[2 * (addr + i)] = buf[4 + 2 * i];
						var[2 * (addr + i) + 1] = buf[5 + 2 * i];
					}

					for (i = 0; i < len;)
					{
						if ((addr + i) == 0x7000) // Page
						{
							if (pic_id != (var[0x7000 * 2] << 8 | var[0x7000 * 2 + 1]))
							{
								Display_page(var[0x7000 * 2] << 8 | var[0x7000 * 2 + 1]);
							}
							i++;
						}
						else if ((addr + i) == 0x7001) // Backlight
						{
							if (var[0x7001 * 2 + 1] > 63)
								var[0x7001 * 2 + 1] = 63;
							if (var[0x7001 * 2 + 1] == 0x00)
							{
								Stop_PWM1();
								gDutyflag = 1;
							}
							else
							{
								if (gDutyflag == 1)
								{
									gDutyflag = 0;
									Start_PWM1();
								}
								if(var[0x7001 * 2 + 1] >= 64) var[0x7001 * 2 + 1] = 63;
								Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
							}
							Backlight_flag = 1;

							i++;
						}
						else if ((addr + i) == 0x7002) // Time
						{
							if (var[0x7002 * 2 + 1] > 99)
								var[0x7002 * 2 + 1] = 99;
							if (var[0x7003 * 2 + 1] > 12)
								var[0x7003 * 2 + 1] = 12;
							if (var[0x7004 * 2 + 1] > 31)
								var[0x7004 * 2 + 1] = 31;
							if (var[0x7005 * 2 + 1] > 23)
								var[0x7005 * 2 + 1] = 23;
							if (var[0x7006 * 2 + 1] > 59)
								var[0x7006 * 2 + 1] = 59;
							if (var[0x7007 * 2 + 1] > 59)
								var[0x7007 * 2 + 1] = 59;

							RTC_Set(2000 + var[0x7002 * 2 + 1], var[0x7003 * 2 + 1], var[0x7004 * 2 + 1], var[0x7005 * 2 + 1], var[0x7006 * 2 + 1], var[0x7007 * 2 + 1]);
							i += 6;
						}
						else if ((addr + i) == 0x7008) // Time setting
						{
							i++;
						}
						else if ((addr + i) == 0x7009) // NULL
						{
							i++;
						}
						else if ((addr + i) == 0x700a) // WAV
						{
							Wav_DAC_Init((var[0x700a * 2] << 8) | var[0x700a * 2 + 1]);
							i++;
						}
						else if ((addr + i) == 0x700b) // Sound volume
						{
							if (var[2 * 0x700b + 1] > 16)
								wav_reduce_para = 16;
							else
								wav_reduce_para = var[2 * 0x700b + 1];

							i++;
						}
						else if ((addr + i) == 0x700c) // RTP calibration
						{
							if (var[0x700c * 2 + 1] == 0x5A)
							{
	#if Touch_selection
								LT_TpAdjust();
	#endif
								var[0x700c * 2 + 1] = 0x00;
							}
							i++;
						}
						else if ((addr + i) == 0x700d) // Key code trigger
						{
							Button_Key_Code = (var[0x700d * 2] << 8) | var[0x700d * 2 + 1];
							Button_Key_Code_Flag = 1;
							i++;
						}
						else if ((addr + i) == 0x700e)
						{
							if(var[2 * 0x700e + 1] == 1)
							{
								if (Backlight_flag == 0)
								{	
									Backlight_count = 0;								
									Backlight_flag = 1;
									Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
									if (var[0x700f * 2 + 1] == 0)
										Start_PWM1();
								}
							}
							else if (var[2 * 0x700e + 1] == 0)
							{
								Backlight_count = 0;								
								Backlight_flag = 0;
								Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
								if (var[0x700f * 2 + 1] == 0)
									Start_PWM1();
							}
							i++;
						}
						else if ((addr + i) == 0x700f)
						{
							if (Backlight_flag == 0 && var[2 * 0x700e + 1] == 1)
							{
								Set_Timer1_Compare_Buffer(gDutyBuf[var[0x700f * 2 + 1]]);
								if (var[0x700f * 2 + 1] == 0)
									Stop_PWM1();
							}
							i++;
						}
						else if ((addr + i) == 0x7011)
						{
							if (var[2 * 0x7011] == 0xAA && var[2 * 0x7011 + 1] == 0x55)
							{
								var[2 * 0x7011] = 0;
								var[2 * 0x7011 + 1] = 0;
								SCI1->SCICR2 &=~(1<<2);
								SCI1->SCICR2 &=~(1<<3);
								SCI1->SCICR2 &=~(1<<5);
								
								PIT1->PCSR&=~PCSR_PIE;
								PIT1->PCSR&=~PCSR_EN;	
								APP(0x8000000);
							}
							i++;
						}
						else if ((addr + i) == 0x703f)
						{
							if (var[2 * 0x703f + 1] < multi_language_count && var[2 * 0x703f + 1] != multi_language)
							{
								multi_language = var[2 * 0x703f + 1];
								
								if (addr_index[19] > 0)
								{
									Flash_Read_UI(rebuf,addr_index[18] + 8 * multi_language,8);
									language_addr = (rebuf[0]) + (rebuf[1] << 8) + (rebuf[2] << 16) + (rebuf[3] << 24);
									language_sum = (rebuf[4]) + (rebuf[5] << 8) + (rebuf[6] << 16) + (rebuf[7] << 24);
								}
								if (language_sum > 0)
								{
									if (language_sum - language_count >=2048)
											Flash_Read_UI(rebuf,language_addr,2048);
									else 
										Flash_Read_UI(rebuf,language_addr,language_sum);

									while(1)
									{
										WDT_FeedDog(); //Clear watchdog
										if (L_count + 4 > 2048)
										{
		
											if (language_sum - language_count >= 2048)
												Flash_Read_UI(rebuf,language_addr + language_count,2048);
											else 
												Flash_Read_UI(rebuf,language_addr + language_count,language_sum - language_count);
											L_count = 0;
												
										}
										L_addr = rebuf[L_count] + (rebuf[L_count + 1] << 8);
										L_len = rebuf[L_count + 2] + (rebuf[L_count + 3] << 8);
										L_count = L_count + 4;
										
										if(L_count + L_len > 2048)
										{

											if (language_sum - (language_count + 4) >= 2048)
												Flash_Read_UI(rebuf,language_addr + language_count + 4,2048);
											else 
												Flash_Read_UI(rebuf,language_addr + language_count + 4 ,language_sum - (language_count + 4));
											L_count = 0;
										}

										for (uint16_t ll = 0; ll < L_len; ll++)
										{
											var[L_addr * 2 + ll] = rebuf[L_count + ll];
										}
										language_count += L_len + 4;
										L_count += L_len;
										
										if(language_count >= language_sum)
											break;
									}
								}
								if(Pop_up_flag == 1)
								{
									Display_pop_up(Pop_up_id,Pop_up_x,Pop_up_y);
								}
								else Display_page(pic_id);
							}
							else
								var[2 * 0x703f + 1] = multi_language;

							i++;
						}
						else if ((addr + i) == 0x7040)
						{
							if (UI_rotate != var[2 * 0x7040 + 1])
							{ 
								UI_rotate = var[2 * 0x7040 + 1];
								if (UI_rotate == 0) 
								{
									UI_rotate_addr = 0;
									Screen_DIR = 0;
									Address_Index(); 		// Get address index
									
									if(GBKioKb_flag == 1 || DioKb_flag == 1 || AioKb_flag == 1)
									{
										if (Pop_up_flag == 0)
											Display_page(pic_id_temp);
										else if (Pop_up_flag == 1)
										{
											Display_Layer = LAY_BUFF7;
											Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
										}	
									}
									else
									{
										if (Pop_up_flag == 1)
										{
											Display_page(Pop_UI_rotate_id); 
										}
										else
										{
											Display_page(pic_id); 
										}
									}
									
								}
								else if (UI_rotate == 1)
								{  
				
									uint8_t check[8] = {0};
									uint32_t encryption1, encryption2;
									UI_rotate_addr = second_UI_add;
									Screen_DIR = 1;
									
									Flash_Read_UI(check, 0x80 + 0xEC, 8); 
									encryption1 = check[0x4] | (check[0x5] << 8) | (check[0x6] << 16) | (check[0x7] << 24);
									encryption2 = check[0x0] | (check[0x1] << 8) | (check[0x2] << 16) | (check[0x3] << 24);
									
									if (encryption1 == ~encryption2)
									{
										Address_Index(); 		// Get address index
										if(GBKioKb_flag == 1 || DioKb_flag == 1 || AioKb_flag == 1)
										{
											if (Pop_up_flag == 0)
												Display_page(pic_id_temp);
											else if (Pop_up_flag == 1)
											{
												Display_Layer = LAY_BUFF7;
												Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
											}	
										}
										else
										{
											if (Pop_up_flag == 1)
											{
												Display_page(Pop_UI_rotate_id); 
											}
											else
											{
												Display_page(pic_id); 
											}
										}
									}
									else
									{
										UI_rotate_addr = 0;
										Screen_DIR = 0;
									}
									
								}
								else if (UI_rotate == 2)
								{
									
								}
								else if (UI_rotate == 3)
								{
									
								}
							}
							i++;
						}
						else
						{
							i++;
						}
					}
				}
				else if (addr >= 0xC001 && addr <= 0xCFFF) // Curve channel address for write
				{
					temp = addr & 0xFFF;
					for (i = 0; i < Curve_Size; i++)
					{
						if (temp & (1 << i))
							num++; // Record the number of channel groups
					}

					i = 0;
					j = 0;
					while (j < len) // Store data to corresponding channels respectively
					{
						if (temp & (1 << i))
						{
							Curve_buff[i][Curve_count[i]] = buf[4 + 2 * j] << 8 | buf[5 + 2 * j];
							Curve_count[i]++;
							if (Curve_count[i] >= 1024)
							{
								Curve_count[i] = 0;
							}
							curve_update_flag[i] = 1;
							j++;
						}
						i++;
						if (i >= Curve_Size)
							i = 0;
					}

					if (len % num) // When multi-channel buffer data is triggered, if some channel data is missing, the missing part will be supplemented
					{
						for (i = 0; i < Curve_Size; i++)
						{
							if (temp & (1 << i))
								cnt++;
							if (cnt > len % num && (temp & (1 << i)))
							{
								if (Curve_count[i] != 0)
									Curve_buff[i][Curve_count[i]] = Curve_buff[i][Curve_count[i] - 1];
								else
									Curve_buff[i][0] = Curve_buff[i][1023];

								Curve_count[i]++;
								if (Curve_count[i] >= 1024)
								{
									Curve_count[i] = 0;
								}
							}
						}
					}
				}
				else if (addr >= 0xE001 && addr <= 0xEFFF) // Curve channel address for clear
				{
					temp = addr & 0xFFF;

					i = 0;
					j = 0;
					for (i = 0; i < Curve_Size; i++)
					{
						if (temp & (1 << i))
						{
							for (j = 0; j < 1024; j++)
							{
								Curve_buff[i][j] = 0x00;
							}

							Curve_count[i] = 0;
							curve_update_flag[i] = 1;
						}
					}
				}
			}
		}
		else // CRC NG
		{
			if (CRC_Feedback_Flag) // CRC feedback is the default
			{
				rebuf[3] = cmd;
				rebuf[4] = 0x00;
				LT_SendData_CRC_Frame(rebuf, 2);
			}
		}
	}
	else if (buf[0] == 0)
	{
		cmd = buf[3];

		if (rxCrc == mcuCrc) // CRC OK
		{
			if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
			{
				rebuf[3] = cmd;
				rebuf[4] = 0xFF;
				LT_SendData_CRC_Frame(rebuf, 2);
				DelayUS(1200);
			}
			
			if (cmd == 0x61)
			{
				len = (buf[1] << 8) | buf[2];
				eflash_addr = 0x804B000 + ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]);
				EFLASH_Init(g_sys_clk / 1000);
				EFLASH_SetWritePermission();

				temp = EFLASH_Write(eflash_addr,&buf[8],len-7);

				EFLASH_ClrWritePermission();
				
				if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
				{
					rebuf[5] = cmd;
					rebuf[6] = buf[4];
					rebuf[7] = buf[5];
					rebuf[8] = buf[6];
					rebuf[9] = buf[7];
					rebuf[10] = 0x4F;
					rebuf[11] = 0x4B;
			
					LT_SendFlashData_CRC_Frame(rebuf, 7);
				}
			}
			else if (cmd == 0x62)
			{
				
				len = (buf[8] << 8) | buf[9];
				if (len > 2048) len = 2048;
				eflash_addr = 0x804B000 + ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]);
				EFLASH_Init(g_sys_clk / 1000);
				EFLASH_SetWritePermission();
				EFLASH_WordsRead_8(eflash_addr,&rebuf[10],len);
				EFLASH_ClrWritePermission();
				
				if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
				{
					rebuf[5] = cmd;
					rebuf[6] = buf[4];
					rebuf[7] = buf[5];
					rebuf[8] = buf[6];
					rebuf[9] = buf[7];
					LT_SendFlashData_CRC_Frame(rebuf, len + 5);
				}
			}
			else if (cmd == 0x63)
			{
				Make_CRC32_Table();
				len = (buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | buf[11];
				count1 = len / 2048;
				count2 = len % 2048;
				eflash_addr = 0x804B000 + ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]);
				EFLASH_Init(g_sys_clk / 1000);
				EFLASH_SetWritePermission();
				for(i = 0; i < count1; i++)
				{
					WDT_FeedDog();
					EFLASH_WordsRead_8(eflash_addr + 2048 * i,rebuf,2048);
					File_CRC = GetCrc32(rebuf,2048,File_CRC);
				}
				if(count2 > 0)
				{
					EFLASH_WordsRead_8(eflash_addr + 2048 * i,rebuf,count2);
					File_CRC = GetCrc32(rebuf,count2,File_CRC);
				}
				EFLASH_ClrWritePermission();
				
				if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
				{
					rebuf[5] = cmd;
					rebuf[6] = buf[4];
					rebuf[7] = buf[5];
					rebuf[8] = buf[6];
					rebuf[9] = buf[7];
					rebuf[10] = (File_CRC >> 24) & 0xFF;
					rebuf[11] = (File_CRC >> 16) & 0xFF;
					rebuf[12] = (File_CRC >> 8) & 0xFF;
					rebuf[13] = File_CRC & 0xFF;
					LT_SendFlashData_CRC_Frame(rebuf, 9);
				}
			}
			else if (cmd == 0x64)
			{
				len = (buf[1] << 8) | buf[2];
				eflash_addr = ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]);

				Flash_Write_NoCheck(&buf[8],eflash_addr,len-7);
			
				if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
				{
					rebuf[5] = cmd;
					rebuf[6] = buf[4];
					rebuf[7] = buf[5];
					rebuf[8] = buf[6];
					rebuf[9] = buf[7];
					rebuf[10] = 0x4F;
					rebuf[11] = 0x4B;
			
					LT_SendFlashData_CRC_Frame(rebuf, 7);
				}
			}
			else if (cmd == 0x65)
			{
				len = (buf[8] << 8) | buf[9];
				if (len > 2048) len = 2048;
				eflash_addr = ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]);

				Flash_Read(&rebuf[10],eflash_addr,len);
		
				if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
				{
					rebuf[5] = cmd;
					rebuf[6] = buf[4];
					rebuf[7] = buf[5];
					rebuf[8] = buf[6];
					rebuf[9] = buf[7];
					LT_SendFlashData_CRC_Frame(rebuf, len + 5);
				}
			}
			else if (cmd == 0x66)
			{
				Make_CRC32_Table();
				len = (buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | buf[11];
				count1 = len / 2048;
				count2 = len % 2048;
				eflash_addr = ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]);
				
				for(i = 0; i < count1; i++)
				{
					WDT_FeedDog();
					Flash_Read(rebuf,eflash_addr + 2048 * i,2048);
					File_CRC = GetCrc32(rebuf,2048,File_CRC);
				}
				
				if(count2 > 0)
				{
					Flash_Read(rebuf,eflash_addr + 2048 * i,count2);
					File_CRC = GetCrc32(rebuf,count2,File_CRC);
				}

				if (CRC_Enable_Flag && CRC_Feedback_Flag) // CRC feedback is the default
				{
					rebuf[5] = cmd;
					rebuf[6] = buf[4];
					rebuf[7] = buf[5];
					rebuf[8] = buf[6];
					rebuf[9] = buf[7];
					rebuf[10] = (File_CRC >> 24) & 0xFF;
					rebuf[11] = (File_CRC >> 16) & 0xFF;
					rebuf[12] = (File_CRC >> 8) & 0xFF;
					rebuf[13] = File_CRC & 0xFF;
					
					LT_SendFlashData_CRC_Frame(rebuf, 9);
				}
			}
		}
		else // CRC NG
		{
			if (CRC_Feedback_Flag) // CRC feedback is the default
			{
				rebuf[3] = cmd;
				rebuf[4] = 0x00;
				LT_SendData_CRC_Frame(rebuf, 2);
			}
		}
	}
	
	gHidRx.Flag = 0;
	gHidRx.Count = 0; // Clear to receive the next data
	//LT_ClearRxBuf();	// Clear receive buffer
}

#endif

#if (UARTBUS_OPTION == 1) || (UARTBUS_OPTION == 2)
void LT_ModBus_REG_Cmd(void)
{
	uint16_t i = 0;
	for (i = 0; i < 0x41;)
	{
		if (i == 0 && flag_MB_REG[i])
		{
			if (pic_id != (var[2 * 0x7000] << 8 | var[2 * 0x7000 + 1]))
			{
				Display_page(var[2 * 0x7000] << 8 | var[2 * 0x7000 + 1]);
			}
			flag_MB_REG[i] = 0;
			i++;
		}
		else if (i == 1 && flag_MB_REG[i])
		{
			if (var[0x7001 * 2 + 1] > 63)
				var[0x7001 * 2 + 1] = 63;
			if (var[0x7001 * 2 + 1] == 0x00)
			{
				Stop_PWM1();
				gDutyflag = 1;
			}
			else
			{
				if (gDutyflag == 1)
				{
					gDutyflag = 0;
					Start_PWM1();
				}
				Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
			}
			Backlight_flag = 1;
			flag_MB_REG[i] = 0;
			i++;
		}
		else if (i == 2 && flag_MB_REG[i]) // Time
		{
			if (var[0x7002 * 2 + 1] > 99)
				var[0x7002 * 2 + 1] = 99;
			if (var[0x7003 * 2 + 1] > 12)
				var[0x7003 * 2 + 1] = 12;
			if (var[0x7004 * 2 + 1] > 31)
				var[0x7004 * 2 + 1] = 31;
			if (var[0x7005 * 2 + 1] > 23)
				var[0x7005 * 2 + 1] = 23;
			if (var[0x7006 * 2 + 1] > 59)
				var[0x7006 * 2 + 1] = 59;
			if (var[0x7007 * 2 + 1] > 59)
				var[0x7007 * 2 + 1] = 59;

			RTC_Set(2000 + var[0x7002 * 2 + 1], var[0x7003 * 2 + 1], var[0x7004 * 2 + 1], var[0x7005 * 2 + 1], var[0x7006 * 2 + 1], var[0x7007 * 2 + 1]);
			flag_MB_REG[i] = 0;
			i += 6;
		}
		else if (i == 8 && flag_MB_REG[i] == 1) //	Time setting
		{
			flag_MB_REG[i] = 0;
			i++;
		}
		else if (i == 9 && flag_MB_REG[i] == 1) // NULL
		{
			flag_MB_REG[i] = 0;
			i++;
		}
		else if (i == 10 && flag_MB_REG[i] == 1) // WAV
		{
			Wav_DAC_Init((var[0x700a * 2] << 8) | var[0x700a * 2 + 1]);
			flag_MB_REG[i] = 0;
			i++;
		}
		else if (i == 11 && flag_MB_REG[i] == 1) // Sound volume
		{
			if (var[2 * 0x700b + 1] > 16)
				wav_reduce_para = 16;
			else
				wav_reduce_para = var[2 * 0x700b + 1];
			flag_MB_REG[i] = 0;
			i++;
		}
		else if (i == 12 && flag_MB_REG[i] == 1) // RTP calibration
		{
			if (var[0x700c * 2 + 1] == 0x5A)
			{
#if Touch_selection
				LT_TpAdjust();
#endif
				var[0x700c * 2 + 1] = 0x00;
			}
			flag_MB_REG[i] = 0;
			i++;
		}
		else if (i == 13 && flag_MB_REG[i] == 1) // Key code trigger
		{
			Button_Key_Code = (var[0x700d * 2] << 8) | var[0x700d * 2 + 1];
			Button_Key_Code_Flag = 1;
			flag_MB_REG[i] = 0;
			i++;
		}
		else if (i == 14 && flag_MB_REG[i] == 1)
		{
			if(var[2 * 0x700e + 1] == 1)
			{
				if (Backlight_flag == 0)
				{	
					Backlight_count = 0;								
					Backlight_flag = 1;
					Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
					if (var[0x700f * 2 + 1] == 0)
						Start_PWM1();
				}
			}
			else if (var[2 * 0x700e + 1] == 0)
			{
				Backlight_count = 0;								
				Backlight_flag = 0;
				Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
				if (var[0x700f * 2 + 1] == 0)
					Start_PWM1();
			}
			flag_MB_REG[i] = 0;
			i++;
		}
		else if (i == 15 && flag_MB_REG[i] == 1)
		{
			if (Backlight_flag == 0 && var[2 * 0x700e + 1] == 1)
			{
				Set_Timer1_Compare_Buffer(gDutyBuf[var[0x700f * 2 + 1]]);
				if (var[0x700f * 2 + 1] == 0)
					Stop_PWM1();
			}
			flag_MB_REG[i] = 0;
			i++;
		}
		else if (i == 17 && flag_MB_REG[i] == 1)
		{
			if (var[2 * 0x7011] == 0xAA && var[2 * 0x7011 + 1] == 0x55)
			{
				var[2 * 0x7011] = 0;
				var[2 * 0x7011 + 1] = 0;
				SCI1->SCICR2 &=~(1<<2);
				SCI1->SCICR2 &=~(1<<3);
				SCI1->SCICR2 &=~(1<<5);
				
				PIT1->PCSR&=~PCSR_PIE;
				PIT1->PCSR&=~PCSR_EN;	
				APP(0x8000000);
			}
			flag_MB_REG[i] = 0;
			i++;
		}
		else if (i == 0x3F && flag_MB_REG[i] == 1)
		{
			if (var[2 * 0x703f + 1] < multi_language_count && var[2 * 0x703f + 1] != multi_language)
			{
				
				uint32_t language_sum = 0,language_count = 0,L_count = 0;
				uint32_t language_addr = 0,L_addr,L_len = 0;
				uint16_t L_num;
				uint8_t rebuf[2100];
				multi_language = var[2 * 0x703f + 1];
				
				if (addr_index[19] > 0)
				{
					Flash_Read_UI(rebuf,addr_index[18] + 8 * multi_language,8);
					language_addr = (rebuf[0]) + (rebuf[1] << 8) + (rebuf[2] << 16) + (rebuf[3] << 24);
					language_sum = (rebuf[4]) + (rebuf[5] << 8) + (rebuf[6] << 16) + (rebuf[7] << 24);
				}
				if (language_sum > 0)
				{
				
					if (language_sum - language_count >=2048)
							Flash_Read_UI(rebuf,language_addr,2048);
					else 
						Flash_Read_UI(rebuf,language_addr,language_sum);
					
					while(1)
					{
						WDT_FeedDog(); //Clear watchdog
						if (L_count + 4 > 2048)
						{
							if (language_sum - language_count >= 2048)
								Flash_Read_UI(rebuf,language_addr + language_count,2048);
							else 
								Flash_Read_UI(rebuf,language_addr + language_count,language_sum- language_count);
							L_count = 0;
								
						}
						L_addr = rebuf[L_count] + (rebuf[L_count + 1] << 8);
						L_len = rebuf[L_count + 2] + (rebuf[L_count + 3] << 8);
						L_count = L_count + 4;
						
						if(L_count + L_len > 2048)
						{
							if (language_sum - (language_count + 4) >= 2048)
								Flash_Read_UI(rebuf,language_addr + language_count + 4,2048);
							else 
								Flash_Read_UI(rebuf,language_addr + language_count + 4 ,language_sum - (language_count + 4));
							L_count = 0;
						}
						
						for (uint16_t ll = 0; ll < L_len; ll++)
						{
							var[L_addr * 2 + ll] = rebuf[L_count + ll];
						}
						language_count += L_len + 4;
						L_count += L_len;
						
						if(language_count >= language_sum)
							break;
					}
					
				}
				if(Pop_up_flag == 1)
				{
					Display_pop_up(Pop_up_id,Pop_up_x,Pop_up_y);
				}
				else Display_page(pic_id);
			}
			else
				var[2 * 0x703f + 1] = multi_language;
			flag_MB_REG[i] = 0;
			i++;
		}
		else if (i == 0x40 && flag_MB_REG[i] == 1)
		{
			if (UI_rotate != var[2 * 0x7040 + 1])
			{ 
				UI_rotate = var[2 * 0x7040 + 1];
				if (UI_rotate == 0) 
				{
					UI_rotate_addr = 0;
					Screen_DIR = 0;
					Address_Index(); 		// Get address index
					
					if(GBKioKb_flag == 1 || DioKb_flag == 1 || AioKb_flag == 1)
					{
						if (Pop_up_flag == 0)
							Display_page(pic_id_temp);
						else if (Pop_up_flag == 1)
						{
							Display_Layer = LAY_BUFF7;
							Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
						}	
					}
					else
					{
						if (Pop_up_flag == 1)
						{
							Display_page(Pop_UI_rotate_id); 
						}
						else
						{
							Display_page(pic_id); 
						}
					}
					
				}
				else if (UI_rotate == 1)
				{  

					uint8_t check[8] = {0};
					uint32_t encryption1, encryption2;
					UI_rotate_addr = second_UI_add;
					Screen_DIR = 1;
					
					Flash_Read_UI(check, 0x80 + 0xEC, 8); 	
					encryption1 = check[0x4] | (check[0x5] << 8) | (check[0x6] << 16) | (check[0x7] << 24);
					encryption2 = check[0x0] | (check[0x1] << 8) | (check[0x2] << 16) | (check[0x3] << 24);
					
					if (encryption1 == ~encryption2)
					{
						Address_Index(); 		// Get address index
						if(GBKioKb_flag == 1 || DioKb_flag == 1 || AioKb_flag == 1)
						{
							if (Pop_up_flag == 0)
								Display_page(pic_id_temp);
							else if (Pop_up_flag == 1)
							{
								Display_Layer = LAY_BUFF7;
								Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
							}	
						}
						else
						{
							if (Pop_up_flag == 1)
							{
								Display_page(Pop_UI_rotate_id); 
							}
							else
							{
								Display_page(pic_id); 
							}
						}
					}
					else
					{
						UI_rotate_addr = 0;
						Screen_DIR = 0;
					}
					
				}
				else if (UI_rotate == 2)
				{
					
				}
				else if (UI_rotate == 3)
				{
					
				}
			}
			flag_MB_REG[i] = 0;
			i++;
		}

		else
		{
			i++;
		}
	}
}

#endif

#if IIC_BUS
void LT_IIC_REG_Cmd(void)
{
	uint16_t i = 0;
	for (i = 0; i < 0x41;)
	{
		if (i == 0 && flag_IIC_REG[i] == 1)
		{
			Display_page(var[0x7000 * 2] << 8 | var[0x7000 * 2 + 1]);
			flag_IIC_REG[i] = 0;
			i++;
		}
		else if (i == 1 && flag_IIC_REG[i] == 1)
		{
			if (var[0x7001 * 2 + 1] == 0x00)
			{
				Stop_PWM1();
				gDutyflag = 1;
			}
			else
			{
				if (gDutyflag == 1)
				{
					gDutyflag = 0;
					Start_PWM1();
				}
				Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
			}
			flag_IIC_REG[i] = 0;
			i++;
		}
		else if (i == 2 && flag_IIC_REG[i] == 1)
		{
			if (var[0x7002 * 2 + 1] > 99)
				var[0x7002 * 2 + 1] = 99;
			if (var[0x7003 * 2 + 1] > 12)
				var[0x7003 * 2 + 1] = 12;
			if (var[0x7004 * 2 + 1] > 31)
				var[0x7004 * 2 + 1] = 31;
			if (var[0x7005 * 2 + 1] > 23)
				var[0x7005 * 2 + 1] = 23;
			if (var[0x7006 * 2 + 1] > 59)
				var[0x7006 * 2 + 1] = 59;
			if (var[0x7007 * 2 + 1] > 59)
				var[0x7007 * 2 + 1] = 59;

			RTC_Set(2000 + var[0x7002 * 2 + 1], var[0x7003 * 2 + 1], var[0x7004 * 2 + 1], var[0x7005 * 2 + 1], var[0x7006 * 2 + 1], var[0x7007 * 2 + 1]);
			flag_IIC_REG[i] = 0;
			i += 6;
		}
		else if (i == 8 && flag_IIC_REG[i] == 1)
		{
			flag_IIC_REG[i] = 0;
			i++;
		}
		else if (i == 9 && flag_IIC_REG[i] == 1)
		{
			flag_IIC_REG[i] = 0;
			i++;
		}
		else if (i == 10 && flag_IIC_REG[i] == 1)
		{
			var[0x700a * 2] = ID_IIC_WAV >> 8;
			var[0x700a * 2 + 1] = ID_IIC_WAV & 0xFF;

			Wav_DAC_Init((var[0x700a * 2] << 8) | var[0x700a * 2 + 1]);
			flag_IIC_REG[i] = 0;
			i++;
		}
		else if (i == 11 && flag_IIC_REG[i] == 1)
		{
			if (var[2 * 0x700b + 1] > 16)
				wav_reduce_para = 16;
			else
				wav_reduce_para = var[2 * 0x700b + 1];

			flag_IIC_REG[i] = 0;
			i++;
		}
		else if (i == 12 && flag_IIC_REG[i] == 1)
		{
			if (var[0x700c * 2 + 1] == 0x5A)
			{
#if Touch_selection
				LT_TpAdjust();
#endif
				var[0x700c * 2 + 1] = 0x00;
			}
			flag_IIC_REG[i] = 0;
			i++;
		}
		else if (i == 13 && flag_IIC_REG[i] == 1)
		{
			Button_Key_Code = (var[0x700d * 2] << 8) | var[0x700d * 2 + 1];
			Button_Key_Code_Flag = 1;
			flag_IIC_REG[i] = 0;
			i++;
		}
		else if (i == 14 && flag_IIC_REG[i] == 1)
		{
			if(var[2 * 0x700e + 1] == 1)
			{
				if (Backlight_flag == 0)
				{	
					Backlight_count = 0;								
					Backlight_flag = 1;
					Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
					if (var[0x700f * 2 + 1] == 0)
						Start_PWM1();
				}
			}
			else if (var[2 * 0x700e + 1] == 0)
			{
				Backlight_count = 0;								
				Backlight_flag = 0;
				Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
				if (var[0x700f * 2 + 1] == 0)
					Start_PWM1();
			}
			flag_IIC_REG[i] = 0;
			i++;
		}
		else if (i == 15 && flag_IIC_REG[i] == 1)
		{
			if (Backlight_flag == 0 && var[2 * 0x700e + 1] == 1)
			{
				Set_Timer1_Compare_Buffer(gDutyBuf[var[0x700f * 2 + 1]]);
				if (var[0x700f * 2 + 1] == 0)
					Stop_PWM1();
			}
			flag_IIC_REG[i] = 0;
			i++;
		}
		else if (i == 17 && flag_IIC_REG[i] == 1)
		{
			if (var[2 * 0x7011] == 0xAA && var[2 * 0x7011 + 1] == 0x55)
			{
				var[2 * 0x7011] = 0;
				var[2 * 0x7011 + 1] = 0;
				SCI1->SCICR2 &=~(1<<2);
				SCI1->SCICR2 &=~(1<<3);
				SCI1->SCICR2 &=~(1<<5);
				
				PIT1->PCSR&=~PCSR_PIE;
				PIT1->PCSR&=~PCSR_EN;	
				APP(0x8000000);
			}
			flag_IIC_REG[i] = 0;
			i++;
		}
		else if (i == 0x3F && flag_IIC_REG[i] == 1)
		{
			if (var[2 * 0x703f + 1] < multi_language_count && var[2 * 0x703f + 1] != multi_language)
			{
				uint32_t language_sum = 0,language_count = 0,L_count = 0;
				uint32_t language_addr = 0,L_addr,L_len = 0;
				uint16_t L_num;
				uint8_t rebuf[2100];
				
				multi_language = var[2 * 0x703f + 1];
				if (addr_index[19] > 0)
				{
					Flash_Read_UI(rebuf,addr_index[18] + 8 * multi_language,8);
					language_addr = (rebuf[0]) + (rebuf[1] << 8) + (rebuf[2] << 16) + (rebuf[3] << 24);
					language_sum = (rebuf[4]) + (rebuf[5] << 8) + (rebuf[6] << 16) + (rebuf[7] << 24);
				}
				
				if (language_sum > 0)
				{
					if (language_sum - language_count >=2048)
							Flash_Read_UI(rebuf,language_addr,2048);
					else 
						Flash_Read_UI(rebuf,language_addr,language_sum - language_count);
					
					while(1)
					{
						WDT_FeedDog(); //Clear watchdog
						if (L_count + 4 > 2048)
						{
							if (language_sum - language_count >= 2048)
								Flash_Read_UI(rebuf,language_addr + language_count,2048);
							else 
								Flash_Read_UI(rebuf,language_addr + language_count,language_sum)- language_count;
							L_count = 0;
								
						}
						L_addr = rebuf[L_count] + (rebuf[L_count + 1] << 8);
						L_len = rebuf[L_count + 2] + (rebuf[L_count + 3] << 8);
						L_count = L_count + 4;
						
						if(L_count + L_len > 2048)
						{
							if (language_sum - (language_count + 4) >= 2048)
								Flash_Read_UI(rebuf,language_addr + language_count + 4,2048);
							else 
								Flash_Read_UI(rebuf,language_addr + language_count + 4 ,language_sum - (language_count + 4));
							L_count = 0;
						}
						
						for (uint16_t ll = 0; ll < L_len; ll++)
						{
							var[L_addr * 2 + ll] = rebuf[L_count + ll];
						}
						language_count += L_len + 4;
						L_count += L_len;
						
						if(language_count >= language_sum)
							break;
					}
				}
				if(Pop_up_flag == 1)
				{
					Display_pop_up(Pop_up_id,Pop_up_x,Pop_up_y);
				}
				else Display_page(pic_id);
			}
			else
				var[2 * 0x703f + 1] = multi_language;
			flag_IIC_REG[i] = 0;
			i++;
		}
		else if (i == 0x40 && flag_IIC_REG[i] == 1)
		{
			if (UI_rotate != var[2 * 0x7040 + 1])
			{ 
				UI_rotate = var[2 * 0x7040 + 1];
				if (UI_rotate == 0) 
				{
					UI_rotate_addr = 0;
					Screen_DIR = 0;
					Address_Index(); 		// Get address index
					
					if(GBKioKb_flag == 1 || DioKb_flag == 1 || AioKb_flag == 1)
					{
						if (Pop_up_flag == 0)
							Display_page(pic_id_temp);
						else if (Pop_up_flag == 1)
						{
							Display_Layer = LAY_BUFF7;
							Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
						}	
					}
					else
					{
						if (Pop_up_flag == 1)
						{
							Display_page(Pop_UI_rotate_id); 
						}
						else
						{
							Display_page(pic_id); 
						}
					}
					
				}
				else if (UI_rotate == 1)
				{  

					uint8_t check[8] = {0};
					uint32_t encryption1, encryption2;
					UI_rotate_addr = second_UI_add£»
					Screen_DIR = 1;

					Flash_Read_UI(check, 0x80 + 0xEC, 8); 	
					encryption1 = check[0x4] | (check[0x5] << 8) | (check[0x6] << 16) | (check[0x7] << 24);
					encryption2 = check[0x0] | (check[0x1] << 8) | (check[0x2] << 16) | (check[0x3] << 24);
					
					if (encryption1 == ~encryption2)
					{
						Address_Index(); 		// Get address index
						if(GBKioKb_flag == 1 || DioKb_flag == 1 || AioKb_flag == 1)
						{
							if (Pop_up_flag == 0)
								Display_page(pic_id_temp);
							else if (Pop_up_flag == 1)
							{
								Display_Layer = LAY_BUFF7;
								Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
							}	
						}
						else
						{
							if (Pop_up_flag == 1)
							{
								Display_page(Pop_UI_rotate_id); 
							}
							else
							{
								Display_page(pic_id); 
							}
						}
					}
					else
					{
						UI_rotate_addr = 0;
						Screen_DIR = 0;
					}
					
				}
				else if (UI_rotate == 2)
				{
					
				}
				else if (UI_rotate == 3)
				{
					
				}
			}
			flag_IIC_REG[i] = 0;
			i++;
		}
		
		else
		{
			i++;
		}
	}
}
#endif

/********************************************************************************
* Function Name	: LT_ReadFlash
* Description  	: Read data from external FLASH.
* Input        	: - *pBuffer: buffer for read data
				  - ReadAddr: Address to start reading
				  - NumByteToRead: Number of bytes to read
* Output       	: None
* Return       	: None
*********************************************************************************/

void LT_ReadFlash(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	SPI_Clock_Period(1);
	if (Flash_type == 0)
	{
		LT_W25qxx_Read(pBuffer, ReadAddr, NumByteToRead);
	}
	else
	{
		LT_W25N01_Read(pBuffer, ReadAddr, NumByteToRead);
	}
}

/********************************************************************************
 * Function Name	: reg_operation
 * Description  	: Operation register.
 * Input        	: - addr: register address
 * Output       	: None
 * Return       	: None
 *********************************************************************************/
void reg_operation(uint16_t addr)
{
	if (addr == 0x7000)
	{
		if (pic_id != (var[0x7000 * 2] << 8) + var[0x7000 * 2 + 1])
		{
			Display_page((var[0x7000 * 2] << 8) + var[0x7000 * 2 + 1]);
		}
	}
	else if (addr == 0x7001)
	{
		if (var[0x7001 * 2 + 1] == 0x00)
		{
			Stop_PWM1();
			gDutyflag = 1;
		}
		else
		{
			if (gDutyflag == 1)
			{
				Start_PWM1();
				gDutyflag = 0;
			}
			if(var[0x7001 * 2 + 1] >= 64) var[0x7001 * 2 + 1] = 63;
			Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
		}
	}
	else if (addr == 0x700a)
		Wav_DAC_Init((var[0x700a * 2] << 8) + var[0x700a * 2 + 1]);
	else if (addr == 0x700b)
	{
		if (var[2 * 0x700b + 1] > 16)
			wav_reduce_para = 16;
		else
			wav_reduce_para = var[2 * 0x700b + 1];
	}
	else if (addr == 0x700d)
	{
		Button_Key_Code = (var[0x700d * 2] << 8) | var[0x700d * 2 + 1];
		Button_Key_Code_Flag = 1;
	}
	else if (addr == 0x700e)
	{
		if(var[2 * 0x700e + 1] == 1)
		{
			if (Backlight_flag == 0)
			{	
				Backlight_count = 0;								
				Backlight_flag = 1;
				Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
				if (var[0x700f * 2 + 1] == 0)
					Start_PWM1();
			}
		}
		else if (var[2 * 0x700e + 1] == 0)
		{
			Backlight_count = 0;								
			Backlight_flag = 0;
			Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
			if (var[0x700f * 2 + 1] == 0)
				Start_PWM1();
		}
	}
	else if (addr == 0x7008) // Time setting
	{
		if (var[0x7008 * 2 + 1] == 0)
		{
			RTC_Set(2000 + var[0x7002 * 2 + 1], var[0x7003 * 2 + 1], var[0x7004 * 2 + 1],
					var[0x7005 * 2 + 1], var[0x7006 * 2 + 1], var[0x7007 * 2 + 1]);
		}
		else if (var[0x7008 * 2 + 1] == 1)
		{
			RTC_Get();
			var[0x7005 * 2 + 1] = calendar.hour;
			var[0x7006 * 2 + 1] = calendar.min;
			var[0x7007 * 2 + 1] = calendar.sec;
			RTC_Set(2000 + var[0x7002 * 2 + 1], var[0x7003 * 2 + 1], var[0x7004 * 2 + 1],
					var[0x7005 * 2 + 1], var[0x7006 * 2 + 1], var[0x7007 * 2 + 1]);
		}
		else if (var[0x7008 * 2 + 1] == 2)
		{
			RTC_Get();
			var[0x7004 * 2 + 1] = calendar.w_date;
			var[0x7005 * 2 + 1] = calendar.hour;
			var[0x7006 * 2 + 1] = calendar.min;
			var[0x7007 * 2 + 1] = calendar.sec;
			RTC_Set(2000 + var[0x7002 * 2 + 1], var[0x7003 * 2 + 1], var[0x7004 * 2 + 1],
					var[0x7005 * 2 + 1], var[0x7006 * 2 + 1], var[0x7007 * 2 + 1]);
		}
		else if (var[0x7008 * 2 + 1] == 3)
		{
			RTC_Get();
			var[0x7002 * 2 + 1] = calendar.w_year - 2000;
			var[0x7005 * 2 + 1] = calendar.hour;
			var[0x7006 * 2 + 1] = calendar.min;
			var[0x7007 * 2 + 1] = calendar.sec;
			RTC_Set(2000 + var[0x7002 * 2 + 1], var[0x7003 * 2 + 1], var[0x7004 * 2 + 1],
					var[0x7005 * 2 + 1], var[0x7006 * 2 + 1], var[0x7007 * 2 + 1]);
		}
		else if (var[0x7008 * 2 + 1] == 4)
		{
			RTC_Get();
			var[0x7002 * 2 + 1] = calendar.w_year - 2000;
			var[0x7003 * 2 + 1] = calendar.w_month;
			var[0x7004 * 2 + 1] = calendar.w_date;
			RTC_Set(2000 + var[0x7002 * 2 + 1], var[0x7003 * 2 + 1], var[0x7004 * 2 + 1],
					var[0x7005 * 2 + 1], var[0x7006 * 2 + 1], var[0x7007 * 2 + 1]);
		}
		else if (var[0x7008 * 2 + 1] == 5)
		{
			RTC_Get();
			var[0x7002 * 2 + 1] = calendar.w_year - 2000;
			var[0x7003 * 2 + 1] = calendar.w_month;
			var[0x7004 * 2 + 1] = calendar.w_date;
			var[0x7007 * 2 + 1] = calendar.sec;
			RTC_Set(2000 + var[0x7002 * 2 + 1], var[0x7003 * 2 + 1], var[0x7004 * 2 + 1],
					var[0x7005 * 2 + 1], var[0x7006 * 2 + 1], var[0x7007 * 2 + 1]);
		}
		else if (var[0x7008 * 2 + 1] == 6)
		{
			RTC_Get();
			var[0x7002 * 2 + 1] = calendar.w_year - 2000;
			var[0x7003 * 2 + 1] = calendar.w_month;
			var[0x7004 * 2 + 1] = calendar.w_date;
			var[0x7005 * 2 + 1] = calendar.hour;
			RTC_Set(2000 + var[0x7002 * 2 + 1], var[0x7003 * 2 + 1], var[0x7004 * 2 + 1],
					var[0x7005 * 2 + 1], var[0x7006 * 2 + 1], var[0x7007 * 2 + 1]);
		}
	}
	else if (addr == 0x703f)
	{
		if (var[2 * 0x703f + 1] < multi_language_count && var[2 * 0x703f + 1] != multi_language)
		{
			uint8_t language_buff[2048];
			uint32_t language_sum = 0,language_count = 0,L_count = 0;
			uint32_t language_addr = 0,L_addr,L_len = 0;
			uint16_t L_num;
			multi_language = var[2 * 0x703f + 1];
			
		
			if (addr_index[19] > 0)
			{
				Flash_Read_UI(language_buff, addr_index[18] + 8 * multi_language, 8);
				language_addr = (language_buff[0]) + (language_buff[1] << 8) + (language_buff[2] << 16) + (language_buff[3] << 24);
				language_sum = (language_buff[4]) + (language_buff[5] << 8) + (language_buff[6] << 16) + (language_buff[7] << 24);
			}
			if (language_sum > 0)
			{
				if (language_sum - language_count >=2048)
						Flash_Read_UI(language_buff,language_addr,2048);
				else 
					Flash_Read_UI(language_buff,language_addr,language_sum);
				
				while(1)
				{
					if (L_count + 4 > 2048)
					{
						if (language_sum - language_count >= 2048)
							Flash_Read_UI(language_buff,language_addr + language_count,2048);
						else 
							Flash_Read_UI(language_buff,language_addr + language_count,language_sum - language_count);
						L_count = 0;
							
					}
					L_addr = language_buff[L_count] + (language_buff[L_count + 1] << 8);
					L_len = language_buff[L_count + 2] + (language_buff[L_count + 3] << 8);
					L_count = L_count + 4;
					
					if(L_count + L_len > 2048)
					{
						if (language_sum - (language_count + 4) >= 2048)
							Flash_Read_UI(language_buff,language_addr + language_count + 4,2048);
						else 
							Flash_Read_UI(language_buff,language_addr + language_count + 4 ,language_sum - (language_count + 4));
						L_count = 0;
					}
					
					for (uint16_t ll = 0; ll < L_len; ll++)
					{
						var[L_addr * 2 + ll] = language_buff[L_count + ll];
					}
					language_count += L_len + 4;
					L_count += L_len;
					
					if(language_count >= language_sum)
						break;
				}

			}
			if(Pop_up_flag == 1)
			{
				Display_pop_up(Pop_up_id,Pop_up_x,Pop_up_y);
			}
			else Display_page(pic_id);
		}
		else
			var[2 * 0x703f + 1] = multi_language;
	}
	else if (addr == 0x7040)
	{
		if (UI_rotate != var[2 * 0x7040 + 1])
		{ 
			UI_rotate = var[2 * 0x7040 + 1];
			if (UI_rotate == 0) 
			{
				UI_rotate_addr = 0;
				Screen_DIR = 0;
				Address_Index(); 		// Get address index
				
				if(GBKioKb_flag == 1 || DioKb_flag == 1 || AioKb_flag == 1)
				{
					if (Pop_up_flag == 0)
						Display_page(pic_id_temp);
					else if (Pop_up_flag == 1)
					{
						Display_Layer = LAY_BUFF7;
						Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
					}	
				}
				else
				{
					if (Pop_up_flag == 1)
					{
						Display_page(Pop_UI_rotate_id); 
					}
					else
					{
						Display_page(pic_id); 
					}
				}
				
			}
			else if (UI_rotate == 1)
			{  
				
				uint8_t check[8] = {0};
				uint32_t encryption1, encryption2;
				UI_rotate_addr = second_UI_add;
				Screen_DIR = 1;
				

				
				Flash_Read_UI(check, 0x80 + 0xEC, 8); 	
				encryption1 = check[0x4] | (check[0x5] << 8) | (check[0x6] << 16) | (check[0x7] << 24);
				encryption2 = check[0x0] | (check[0x1] << 8) | (check[0x2] << 16) | (check[0x3] << 24);
				
				if (encryption1 == ~encryption2)
				{
					Address_Index(); 		// Get address index
					if(GBKioKb_flag == 1 || DioKb_flag == 1 || AioKb_flag == 1)
					{
						if (Pop_up_flag == 0)
							Display_page(pic_id_temp);
						else if (Pop_up_flag == 1)
						{
							Display_Layer = LAY_BUFF7;
							Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
						}	
					}
					else
					{
						if (Pop_up_flag == 1)
						{
							Display_page(Pop_UI_rotate_id); 
						}
						else
						{
							Display_page(pic_id); 
						}
					}
				}
				else
				{
					UI_rotate_addr = 0;
					Screen_DIR = 0;
				}
				
			}
			else if (UI_rotate == 2)
			{
				
			}
			else if (UI_rotate == 3)
			{
				
			}
		}

	}

}
/***********/

/***********************************************************************************
 * Function Name	: Display_page
 * Description		: Display the interface of a page.
 * Input			: - id: Page No
 * Output			: None
 * Return			: None
************************************************************************************/
void Display_page(uint16_t id)
{

	if (id < addr_index[7])
	{
		uint8_t buff[12] = {0};
		uint16_t canvas_w = 0;

		var[0x7000 * 2 + 0] = id >> 8;
		var[0x7000 * 2 + 1] = id & 0xFF;

		Flash_Read_UI(buff, addr_index[6] + 12 * id, 12);
		pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
		pic_w = buff[4] + (buff[5] << 8);
		pic_h = buff[6] + (buff[7] << 8);
		pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
		pic_flag = buff[11];
		pic_id = id;
	

		Display_Layer = LAY_BUFF7; // Change the display layer window

		if (pic_flag == 0x10) // Pure color
		{
			Canvas_Image_Start_address(MAIN_BUFF);
			Canvas_image_width(LCD_XSIZE_TFT);
			LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT - 1, LCD_YSIZE_TFT - 1, pic_len);
			Canvas_Image_Start_address(Display_Layer);
			Canvas_image_width(LCD_XSIZE_TFT);
			LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT - 1, LCD_YSIZE_TFT - 1, pic_len);
			
		}
		else
		{
			Canvas_Image_Start_address(MAIN_BUFF);
			Canvas_image_width(LCD_XSIZE_TFT);

			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												pic_w, pic_h,
												pic_w, pic_add, MAIN_BUFF, LCD_XSIZE_TFT);

			if (pic_flag == 0)
			{
				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, 0, 0,
									  MAIN_BUFF, LCD_XSIZE_TFT, 0, 0,
									  Display_Layer, LCD_XSIZE_TFT, 0, 0,
									  0x0c, pic_w, pic_h);
			}

		
			if (pic_flag == 2)
			{
				BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, 0, 0,
											   MAIN_BUFF, LCD_XSIZE_TFT, 0, 0,
											   Display_Layer, LCD_XSIZE_TFT, 0, 0,
											   pic_w, pic_h);
			}
		}

		/*Clear previous page key function*/
//		Button_Key_Code_Flag = 0;
//		Button_Key_Code = 0;

		Gesture_press = 0;
		Basci_count = 0;
		Basci_flag = 0;
		Ges_sc_flag = 0;
		Adj_count = 0;
		Adj_flag = 0;

		Slide_count = 0;
		Slide_flag = 0;

		Dio_count = 0;
		Dio_flag = 0;
		DioKb_flag = 0;
		Aio_count = 0;
		AioKb_flag = 0;
		Aio_flag = 0;
		GBKio_count = 0;
		GBKioKb_flag = 0;
		GBKio_flag = 0;
		Kb_count = 0;
		Kb_flag = 0;

		sldmenu_count = 0;
		sldmenu_flag = 0;

		VarKey_count = 0;
		VarKey_flag = 0;

		Pop_up_count = 0;
		Pop_up_flag = 0;
		Pop_bgflag = 0;
		button_Press_flag = 0;
		/**************/

		/*Clear the dynamic display function on the previous page*/
		data_D_num = 0;
		memset(data_D_w, 0, Data_wLen_Size);

		pdata_D_num = 0;
		memset(pdata_D_w, 0, pData_wLen_Size);
		Var_Count_num = 0;
		Gif_num = 0;
		RTC_num = 0;
		Clock_num = 0;
		Scroll_num = 0;
		Gesture_flag = 0;
		RingSld_count = 0;
		curve_num = 0;
		gEncoderFlag = 0;
		Pointer_Count_flag = 0;
		Pointer_Count_num = 0;
		
		Saving_counting_parameters(); // Save data for time counter
		Count_sum = 0;
#if encoder_on
		knob_flag = 0;
//		SW_flag = 0;
//		SW_flag4 = 0;
#endif
#if (UARTBUS_OPTION == 2)
		if (gUsartTx.Mode == 0x02)
			tx_repeat_flag = 0;
#endif
		if (gWavFlag == 1 || gWavType == 1)
		{
			gWavType = 0;
			close_wav_dac();
		}

		RTC_Get();
		var[0x7002 * 2 + 1] = calendar.w_year - 2000;
		var[0x7003 * 2 + 1] = calendar.w_month;
		var[0x7004 * 2 + 1] = calendar.w_date;
		var[0x7005 * 2 + 1] = calendar.hour;
		var[0x7006 * 2 + 1] = calendar.min;
		var[0x7007 * 2 + 1] = calendar.sec;
		var[0x7008 * 2 + 1] = calendar.week;-

		Get_Touch_info(id);		  // Get touch information
		Get_date_variable_Id(id); // Get display information
		Display_Layer = 0;		  // The window changes back to the display window
		
		LT768_BTE_Memory_Copy(LAY_BUFF7, LCD_XSIZE_TFT, 0, 0,
							  LAY_BUFF7, LCD_XSIZE_TFT, 0, 0,
							  Display_Layer, LCD_XSIZE_TFT, 0, 0,
							  0x0c, LCD_XSIZE_TFT, LCD_YSIZE_TFT);
		
		Disable_PIP1();			  // Picture-in-Picture Off
		

	}
}

/***********************************************************************************
* Function Name	: download_page
* Description	: Buffer page information to sdram.
* Input			: - id: page id.
				  - Layer_width : Buffer layer width
				  - sdram_x,sdram_y: Buffer layer coordinates
* Output		: None
* Return		: None
************************************************************************************/

void download_page(uint16_t id, uint16_t Layer_width, uint16_t sdram_x, uint16_t sdram_y)
{
	if (id < addr_index[7])
	{
		uint8_t buff[12] = {0};
		uint16_t canvas_w = 0;

		Flash_Read_UI(buff, addr_index[6] + 12 * id, 12);
		pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
		pic_w = buff[4] + (buff[5] << 8);
		pic_h = buff[6] + (buff[7] << 8);
		pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
		pic_flag = buff[11];

		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, 0, 0,
							  MAIN_BUFF, LCD_XSIZE_TFT, 0, 0,
							  LAY_BUFF8, LCD_XSIZE_TFT, 0, 0,
							  0x0c, LCD_XSIZE_TFT, LCD_YSIZE_TFT);

		Display_Layer = LAY_BUFF5; // Change the display layer window
		if (pic_flag == 0x10)	   // Pure color
		{
			Canvas_Image_Start_address(MAIN_BUFF);
			Canvas_image_width(LCD_XSIZE_TFT);
			LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT - 1, LCD_YSIZE_TFT - 1, pic_len);
			Canvas_Image_Start_address(Display_Layer);
			Canvas_image_width(LCD_XSIZE_TFT);
			LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT - 1, LCD_YSIZE_TFT - 1, pic_len);
		}
		else
		{
			Canvas_Image_Start_address(MAIN_BUFF);
			Canvas_image_width(LCD_XSIZE_TFT);

			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												pic_w, pic_h,
												pic_w, pic_add,
												MAIN_BUFF, LCD_XSIZE_TFT);

			if (pic_flag == 0)
			{
				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, 0, 0,
									  MAIN_BUFF, LCD_XSIZE_TFT, 0, 0,
									  Display_Layer, LCD_XSIZE_TFT, 0, 0,
									  0x0c, pic_w, pic_h);
			}
			if (pic_flag == 2)
			{
				BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, 0, 0,
											   MAIN_BUFF, LCD_XSIZE_TFT, 0, 0,
											   Display_Layer, LCD_XSIZE_TFT, 0, 0,
											   pic_w, pic_h);
			}
		}

		/*Clear previous page key function*/
//		Button_Key_Code_Flag = 0;
//		Button_Key_Code = 0;
		Gesture_press = 0;
		Basci_count = 0;
		Basci_flag = 0;

		Adj_count = 0;
		Adj_flag = 0;

		Slide_count = 0;
		Slide_flag = 0;

		Dio_count = 0;
		Dio_flag = 0;
		DioKb_flag = 0;
		Aio_count = 0;
		AioKb_flag = 0;
		Aio_flag = 0;
		GBKio_count = 0;
		GBKioKb_flag = 0;
		GBKio_flag = 0;
		Kb_count = 0;
		Kb_flag = 0;

		Kb_count = 0;
		Kb_flag = 0;

		sldmenu_count = 0;
		sldmenu_flag = 0;

		VarKey_count = 0;
		VarKey_flag = 0;

		Pop_up_count = 0;
		Pop_up_flag = 0;
		button_Press_flag = 0;
		/**************/

		/*Clear the dynamic display function on the previous page*/
		data_D_num = 0;
		memset(data_D_w, 0, Data_wLen_Size);
		pdata_D_num = 0;
		memset(pdata_D_w, 0, pData_wLen_Size);
		Gif_num = 0;
		Var_Count_num = 0;
		RTC_num = 0;
		Clock_num = 0;
		Scroll_num = 0;
		Gesture_flag = 0;
		RingSld_count = 0;
		curve_num = 0;
		gEncoderFlag = 0;
		Pointer_Count_flag = 0;
		Pointer_Count_num = 0;
#if (UARTBUS_OPTION == 2)
		if (gUsartTx.Mode == 0x02)
			tx_repeat_flag = 0;
#endif

		Get_date_variable_Id_download(id);
		Get_Touch_info_download(id);

		LT768_BTE_Memory_Copy(LAY_BUFF5, LCD_XSIZE_TFT, 0, 0,
							  LAY_BUFF5, LCD_XSIZE_TFT, 0, 0,
							  LAY_BUFF12, Layer_width, sdram_x, sdram_y,
							  0x0c, LCD_XSIZE_TFT, LCD_YSIZE_TFT);

		Display_Layer = LAY_BUFF7;

		LT768_BTE_Memory_Copy(LAY_BUFF8, LCD_XSIZE_TFT, 0, 0,
							  LAY_BUFF8, LCD_XSIZE_TFT, 0, 0,
							  MAIN_BUFF, LCD_XSIZE_TFT, 0, 0,
							  0x0c, LCD_XSIZE_TFT, LCD_YSIZE_TFT);
	}
}

/***********************************************************************************
* Function Name	: Display_pop_up
* Description	: Show pop-up page.
* Input			: - id: page id
				  - x,y: Screen coordinates
* Output		: None
* Return		: None
************************************************************************************/

void Display_pop_up(uint16_t id, uint16_t x, uint16_t y)
{
	if (id < addr_index[7])
	{
		uint8_t buff[12] = {0};
		uint16_t canvas_w = 0;
		uint16_t w, h;
		uint8_t bgflag = 0;
		Flash_Read_UI(buff, addr_index[6] + 12 * id, 12);
		pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
		pic_w = buff[4] + (buff[5] << 8);
		pic_h = buff[6] + (buff[7] << 8);
		pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
		pic_flag = buff[11];
		pic_id = id;
		w = pic_w;
		h = pic_h;
		
		var[0x7000 * 2 + 0] = id >> 8;
		var[0x7000 * 2 + 1] = id & 0xFF;
		
		Display_Layer = LAY_BUFF7;
		if (pic_len != 0 && pic_flag != 0xFF)
		{

			if ((gPop_up_Info[Pop_up_num].function & 0x01) == 1 && Pop_bgflag == 0)
			{
				Pop_bgflag = 1;
				bgflag = 1;
				Canvas_Image_Start_address(LAY_BUFF1);
				Canvas_image_width(LCD_XSIZE_TFT);
				LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT - 1, LCD_YSIZE_TFT - 1, Black);

				BTE_Alpha_Blending(0, LCD_XSIZE_TFT, 0, 0,
								   LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
								   Display_Layer, LCD_XSIZE_TFT, 0, 0,
								   LCD_XSIZE_TFT, LCD_YSIZE_TFT, 25);
			}

			Canvas_Image_Start_address(LAY_BUFF1);
			Canvas_image_width(LCD_XSIZE_TFT);

			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, x, y,
												pic_w, pic_h,
												pic_w, pic_add, LAY_BUFF1, LCD_XSIZE_TFT);

			if (pic_flag == 0)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, x, y,
									  LAY_BUFF1, LCD_XSIZE_TFT, x, y,
									  Display_Layer, LCD_XSIZE_TFT, x, y,
									  0x0c, pic_w, pic_h);
			}
			else if (pic_flag == 1)
			{
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, LCD_XSIZE_TFT, x, y,
												 Display_Layer, LCD_XSIZE_TFT, x, y,
												 Black, pic_w, pic_h);
			}
			else if (pic_flag == 2)
			{

				BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, x, y,
											   LAY_BUFF1, LCD_XSIZE_TFT, x, y,
											   Display_Layer, LCD_XSIZE_TFT, x, y,
											   pic_w, pic_h);
			}

			LT768_BTE_Memory_Copy(Display_Layer, LCD_XSIZE_TFT, x, y,
								  Display_Layer, LCD_XSIZE_TFT, x, y,
								  MAIN_BUFF, LCD_XSIZE_TFT, x, y,
								  0x0c, pic_w, pic_h);
		}

		/*Clear previous page key function*/
//		Button_Key_Code_Flag = 0;
//		Button_Key_Code = 0;
		Gesture_press = 0;
		Basci_count = 0;
		Basci_flag = 0;
		Ges_sc_flag = 0;
		Adj_count = 0;
		Adj_flag = 0;

		Slide_count = 0;
		Slide_flag = 0;

		Dio_count = 0;
		Dio_flag = 0;
		DioKb_flag = 0;
		Aio_count = 0;
		AioKb_flag = 0;
		Aio_flag = 0;
		GBKio_count = 0;
		GBKioKb_flag = 0;
		GBKio_flag = 0;
		Kb_count = 0;
		Kb_flag = 0;

		Kb_count = 0;
		Kb_flag = 0;

		sldmenu_count = 0;
		sldmenu_flag = 0;

		VarKey_count = 0;
		VarKey_flag = 0;

		button_Press_flag = 0;
		/**************/
		
		Pop_up_count = 0;

		/*Clear the dynamic display function on the previous page*/
		data_D_num = 0;
		memset(data_D_w, 0, Data_wLen_Size);

		pdata_D_num = 0;
		memset(pdata_D_w, 0, pData_wLen_Size);

		Var_Count_num = 0; 
		Gif_num = 0;
		RTC_num = 0;
		Clock_num = 0;
		Scroll_num = 0;
		Gesture_flag = 0;
		RingSld_count = 0;
		Saving_counting_parameters();
		Count_sum = 0;
		gWavType = 0;
		Pointer_Count_flag = 0;
		Pointer_Count_num = 0;
#if (UARTBUS_OPTION == 2)
		if (gUsartTx.Mode == 0x02)
			tx_repeat_flag = 0;
#endif

		RTC_Get();
		var[0x7002 * 2 + 1] = calendar.w_year - 2000;
		var[0x7003 * 2 + 1] = calendar.w_month;
		var[0x7004 * 2 + 1] = calendar.w_date;
		var[0x7005 * 2 + 1] = calendar.hour;
		var[0x7006 * 2 + 1] = calendar.min;
		var[0x7007 * 2 + 1] = calendar.sec;
		var[0x7008 * 2 + 1] = calendar.week;

		Get_date_variable_Id(id);
		Get_Touch_info(id);
		Display_Layer = 0;
		//Wav_DAC_Init((var[0x700a * 2] << 8) + var[0x700a * 2 + 1]);

		if (bgflag == 1)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF7, LCD_XSIZE_TFT, 0, 0,
								  LAY_BUFF7, LCD_XSIZE_TFT, 0, 0,
								  Display_Layer, LCD_XSIZE_TFT, 0, 0,
								  0x0c, LCD_XSIZE_TFT, LCD_YSIZE_TFT);
		}
		else
		{
			LT768_BTE_Memory_Copy(LAY_BUFF7, LCD_XSIZE_TFT, x, y,
								  LAY_BUFF7, LCD_XSIZE_TFT, x, y,
								  Display_Layer, LCD_XSIZE_TFT, x, y,
								  0x0c, w, h);
		}
	}
}

/**************/

/***********************************************************************************
 * Function Name	: Get_Canvas_Width
 * Description		: Convert to multiple of 4
 * Input			: - width : Width value
 * Output			: None
 * Return			: The width is added as a multiple of 4
 ************************************************************************************/

uint16_t Get_Canvas_Width(uint16_t width)
{
	uint16_t w = 0;
	if ((width % 4) == 0)
		w = width;
	else
		w = (width / 4 + 1) * 4;
	return w;
}

/***********************************************************************************
* Function Name	: Display_Icon
* Description	: Show a icon.
* Input			: - id: icon id
				  - x,y: Screen coordinates
				  - Display_mode: How to combine with layers
* Output		: None
* Return		: None
************************************************************************************/

void Display_Icon(uint16_t id, uint16_t x, uint16_t y, uint8_t Display_mode)
{
	uint8_t buff[12] = {0};
	uint16_t canvas_w = 0;
	uint8_t on_gif = 0;

	Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * id, 12);
	pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
	pic_w = buff[4] + (buff[5] << 8);
	pic_h = buff[6] + (buff[7] << 8);
	pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
	pic_flag = buff[11];


	if (pic_len != 0 && pic_flag != 0xFF)
	{
		canvas_w = Get_Canvas_Width(pic_w);
		Canvas_Image_Start_address(LAY_BUFF1);
		Canvas_image_width(canvas_w);

		if (pic_flag == 0)
		{
			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												pic_w, pic_h,
												pic_w, pic_add,
												LAY_BUFF1, canvas_w);

			LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
								  LAY_BUFF1, canvas_w, 0, 0,
								  Display_Layer, LCD_XSIZE_TFT, x, y,
								  0x0c, pic_w, pic_h);

#if encoder_on
			if (gEncoderFlag == 1 && Check_box_flag == 0)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF8, LCD_XSIZE_TFT, x, y,
									  0x0c, pic_w, pic_h);
			}
#endif
		}
		else if (pic_flag == 1)
		{
			if (Display_mode == 0)
			{
				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x, y,
									  MAIN_BUFF, LCD_XSIZE_TFT, x, y,
									  LAY_BUFF2, canvas_w, 0, 0,
									  0x0c, pic_w, pic_h);
			}
			else
			{
				LT768_BTE_Memory_Copy(Display_Layer, LCD_XSIZE_TFT, x, y,
									  Display_Layer, LCD_XSIZE_TFT, x, y,
									  LAY_BUFF2, canvas_w, 0, 0,
									  0x0c, pic_w, pic_h);
			}
			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												pic_w, pic_h,
												pic_w, pic_add, LAY_BUFF1, canvas_w);

			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
											 LAY_BUFF2, canvas_w, 0, 0,
											 Black, pic_w, pic_h);
			
			LT768_BTE_Memory_Copy(LAY_BUFF2, canvas_w, 0, 0,
								  LAY_BUFF2, canvas_w, 0, 0,
								  Display_Layer, LCD_XSIZE_TFT, x, y,
								  0x0c, pic_w, pic_h);
			

#if encoder_on
			if (gEncoderFlag == 1)
			{
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
												 LAY_BUFF8, LCD_XSIZE_TFT, x, y,
												 Black, pic_w, pic_h);
			}
#endif
		}
		else if (pic_flag == 2)
		{
			/*test*/
			for (uint8_t i = 0; i < Gif_num; i++)
			{
				if (x >= gGif_Info[i].x && x <= gGif_Info[i].x + gGif_Info[i].w &&
					y >= gGif_Info[i].y && y <= gGif_Info[i].y + gGif_Info[i].h && gGif_Info[i].on_gif == 1)
				{
					on_gif = 1;
					break;
				}
			}
			
			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												pic_w, pic_h,
												pic_w, pic_add,
												LAY_BUFF1, canvas_w);
#if encoder_on
			if (Check_box_flag == 0)
			{
				if (Display_mode == 0)
				{
					if (on_gif == 0)
					{
						BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, x, y,
													   LAY_BUFF1, canvas_w, 0, 0,
													   Display_Layer, LCD_XSIZE_TFT, x, y,
													   pic_w, pic_h);
					}
					else if (on_gif == 1)
					{
						BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF6, LCD_XSIZE_TFT, x, y,
													   LAY_BUFF1, canvas_w, 0, 0,
													   Display_Layer, LCD_XSIZE_TFT, x, y,
													   pic_w, pic_h);
						
						LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
											  LAY_BUFF1, canvas_w, 0, 0,
											  LAY_BUFF5, LCD_XSIZE_TFT, x, y,
											  0x0c, pic_w, pic_h);
					}
				}
				else
				{
					if (on_gif == 0)
					{
						BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, x, y,
													   LAY_BUFF1, canvas_w, 0, 0,
													   Display_Layer, LCD_XSIZE_TFT, x, y,
													   pic_w, pic_h);
					}
					else if (on_gif == 1)
					{
						BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF6, LCD_XSIZE_TFT, x, y,
													   LAY_BUFF1, canvas_w, 0, 0,
													   Display_Layer, LCD_XSIZE_TFT, x, y,
													   pic_w, pic_h);
						
						LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
											  LAY_BUFF1, canvas_w, 0, 0,
											  LAY_BUFF5, LCD_XSIZE_TFT, x, y,
											  0x0c, pic_w, pic_h);
					}
				}
			}
			else if (Check_box_flag == 1)
			{
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF8, LCD_XSIZE_TFT, x, y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   Display_Layer, LCD_XSIZE_TFT, x, y,
											   pic_w, pic_h);
			}
			if (gEncoderFlag == 1 && Check_box_flag == 0)
			{
				if (on_gif == 0)
				{
					if (Display_mode == 0)
					{
						BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, x, y,
													   LAY_BUFF1, canvas_w, 0, 0,
													   LAY_BUFF8, LCD_XSIZE_TFT, x, y,
													   pic_w, pic_h);
					}
					else
					{
						BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, x, y,
													   LAY_BUFF1, canvas_w, 0, 0,
													   LAY_BUFF8, LCD_XSIZE_TFT, x, y,
													   pic_w, pic_h);
					}
				}
				else if (on_gif == 1)
				{
					BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF6, LCD_XSIZE_TFT, x, y,
												   LAY_BUFF1, canvas_w, 0, 0,
												   Display_Layer, LCD_XSIZE_TFT, x, y,
												   pic_w, pic_h);
					
					LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
										  LAY_BUFF1, canvas_w, 0, 0,
										  LAY_BUFF5, LCD_XSIZE_TFT, x, y,
										  0x0c, pic_w, pic_h);
				}
			}
#endif

#if !encoder_on
			if (on_gif == 0)
			{
				if (Display_mode == 0)
				{
					BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, x, y,
												   LAY_BUFF1, canvas_w, 0, 0,
												   Display_Layer, LCD_XSIZE_TFT, x, y,
												   pic_w, pic_h);
				}
				else
				{
					BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, x, y,
												   LAY_BUFF1, canvas_w, 0, 0,
												   Display_Layer, LCD_XSIZE_TFT, x, y,
												   pic_w, pic_h);
				}
			}
			else if (on_gif == 1)
			{
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF6, LCD_XSIZE_TFT, x, y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   Display_Layer, LCD_XSIZE_TFT, x, y,
											   pic_w, pic_h);
				
				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF5, LCD_XSIZE_TFT, x, y,
									  0x0c, pic_w, pic_h);
			}
#endif
		}
	}
}
/****************/

uint8_t Ascii_w[95] = {0};

/*******************************************************************************
 * Function Name	: LT_Print_zk_Font
 * Description		: Display a string.
 * Input			:
 * Output       	: None
 * Return       	: None
 ********************************************************************/

void LT_Print_zk_Font
(
	uint16_t encode		// Encoding type, 0:GB2312  1:GBK  2:BIG5  3:UNICODE  4:ASCII 6:UNICODE
	,
	uint32_t FlashAddr	// Font source address(exSpiFlash)
	,
	uint32_t ShowAddr	// Displays the address of the layer
	,
	uint16_t width		// Display the width of the layer
	,
	uint8_t W_Size		// Font width
	,
	uint8_t H_Size		// Font height
	,
	uint8_t Alignment	// Alignment 0:Left	1:Mid	2:Right
	,
	uint32_t FontColor	// The foreground color of the font
	,
	uint8_t star		// Text display mode: 0:txt  1:encryption use '*' (0x2A)
	,
	uint16_t Xs			// Text box start x position
	,
	uint16_t Ys			// Text box start Y position
	,
	uint16_t Xe			// Text box end x position
	,
	uint16_t Ye			// Text box end y position
	,
	uint8_t dis_x		// The interval between two adjacent fonts on the x-axis
	,
	uint8_t dis_y		// The interval between two adjacent fonts on the y-axis
	,
	uint8_t *c			// Address of text
	,
	uint16_t len_max	// max lenght of text
)
{
	uint16_t temp_H = 0, temp_L = 0, temp = 0;
	uint32_t i = 0;
	uint16_t width_Hor = 0;
	uint8_t pixel_format = 0, temp_pixel = 0, row = 0, Font_W;
	uint16_t row_w[20] = {0};
	uint16_t x_src = 0, y_src = 0, x_des = 0, y_des = 0, w_real = 0, h_real = 0;
	uint16_t len_disp = 0, offset_disp = 0, W_Len = 0, H_Len = 0;

	uint16_t unicode = 0;
	uint16_t start_code = 0, end_code = 0;
	uint8_t buff_code[4];
	uint8_t on_gif = 0,star_flag = 0;
	
	Flash_Read_UI(Ascii_w, FlashAddr + 9, 95);
	Flash_Read_UI(&pixel_format, FlashAddr + 8, 1);

	if (pixel_format == 0)
		temp_pixel = 8;
	else if (pixel_format == 1)
		temp_pixel = 4;
	else if (pixel_format == 2)
		temp_pixel = 2;
	
	if (W_Size % temp_pixel != 0)
		width_Hor = W_Size / temp_pixel + 1;
	else
		width_Hor = W_Size / temp_pixel;

	if (TFT_bitcolor == 1 && pixel_format != 0)
		Memory_16bpp_Mode();

	if (Screen_DIR == 0 || Screen_DIR == 2)
	{
		W_Len = Xe - Xs + 1;
		H_Len = Ye - Ys + 1;
	}
	else if (Screen_DIR == 1 || Screen_DIR == 3)
	{
		W_Len = Ye - Ys + 1;
		H_Len = Xe - Xs + 1;
	}

	if (encode == 4) // ASCII
	{
		while ((c[i] != '\0')) // Automatic exit when encountering non-Chinese character code
		{
			WDT_FeedDog(); //Clear watchdog
			if (c[i] < 0x80 && c[i] > 0x00)
			{
				if (star == 1)
					temp = '*';
				else
					temp = c[i];
				if (((len_disp + Ascii_w[temp - 0x20]) > W_Len) || c[i] == 0x0A) // Line feed
				{
					row_w[row] = len_disp;
					row++;
					offset_disp += H_Size;
					len_disp = 0;
					if ((offset_disp + H_Size) > H_Len)
						break;
				}
				if (c[i] != 0x0A)
				{
					if (Screen_DIR == 0)
					{
						x_src = len_disp;
						y_src = offset_disp;
					}
					else if (Screen_DIR == 1)
					{
						x_src = LCD_XSIZE_TFT - H_Size - offset_disp;
						y_src = len_disp;
					}
					else if (Screen_DIR == 2)
					{
						x_src = LCD_XSIZE_TFT - width_Hor * temp_pixel - len_disp;
						y_src = LCD_YSIZE_TFT - H_Size - offset_disp;
					}
					else if (Screen_DIR == 3)
					{
						x_src = offset_disp;
						y_src = LCD_YSIZE_TFT - width_Hor * temp_pixel - len_disp;
					}
					LT768_Draw_16bit(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, FlashAddr + 104 + (temp - 0x20) * width_Hor * H_Size, LAY_BUFF1, LCD_XSIZE_TFT);
					len_disp = len_disp + Ascii_w[temp - 0x20] + 2;
				}

				i++;
				if (i >= len_max)
					break;
			}
			else
			{
				break;
			}
		}
	}
	else if (encode == 0) // GB2312
	{
		
		while ((c[i] != '\0')) // Automatic exit when encountering non-Chinese character code
		{
			WDT_FeedDog(); //Clear watchdog
			
			if (star == 1 && c[i] >= 0xa1 && c[i] != 0xFF)
			{
				//c[i + 1] = '*';
				star_flag = 1;
				i++;
			}
			
			if ((c[i] < 128 && c[i] > 0x00) || star_flag == 1)
			{
				if (star == 1)
				{
					temp = '*';
					star_flag = 0;
				}
				else
					temp = c[i];
				if (((len_disp + Ascii_w[temp - 0x20]) > W_Len) || c[i] == 0x0A)
				{
					row_w[row] = len_disp;
					row++;
					offset_disp += H_Size;
					len_disp = 0;
					if ((offset_disp + H_Size) > H_Len)
						break;
				}
				if (c[i] != 0x0A)
				{
					if (Screen_DIR == 0)
					{
						x_src = len_disp;
						y_src = offset_disp;
					}
					else if (Screen_DIR == 1)
					{
						x_src = LCD_XSIZE_TFT - H_Size - offset_disp;
						y_src = len_disp;
					}
					else if (Screen_DIR == 2)
					{
						x_src = LCD_XSIZE_TFT - width_Hor * temp_pixel - len_disp;
						y_src = LCD_YSIZE_TFT - H_Size - offset_disp;
					}
					else if (Screen_DIR == 3)
					{
						x_src = offset_disp;
						y_src = LCD_YSIZE_TFT - width_Hor * temp_pixel - len_disp;
					}
					LT768_Draw_16bit(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, FlashAddr + 104 + (temp - 0x20) * width_Hor * H_Size, LAY_BUFF1, LCD_XSIZE_TFT);
					len_disp = len_disp + Ascii_w[temp - 0x20] + 2;
				}

				i++;
				if (i >= len_max)
					break;
			}
			else if (c[i] >= 0xa1 && c[i] != 0xFF)
			{
				temp_H = ((c[i] - 0xa1) & 0x00ff) * 94;
				temp_L = c[i + 1] - 0xa1;
				temp = temp_H + temp_L;

				if ((len_disp + W_Size) > W_Len)
				{
					row_w[row] = len_disp;
					row++;
					offset_disp += H_Size;
					len_disp = 0;
					if ((offset_disp + H_Size) > H_Len)
						break;
				}
				if (c[i] != 0x0A)
				{
					if (Screen_DIR == 0)
					{
						x_src = len_disp;
						y_src = offset_disp;
					}
					else if (Screen_DIR == 1)
					{
						x_src = LCD_XSIZE_TFT - H_Size - offset_disp;
						y_src = len_disp;
					}
					else if (Screen_DIR == 2)
					{
						x_src = LCD_XSIZE_TFT - width_Hor * temp_pixel - len_disp;
						y_src = LCD_YSIZE_TFT - H_Size - offset_disp;
					}
					else if (Screen_DIR == 3)
					{
						x_src = offset_disp;
						y_src = LCD_YSIZE_TFT - width_Hor * temp_pixel - len_disp;
					}
					LT768_Draw_16bit(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, FlashAddr + 104 + (95 + temp) * width_Hor * H_Size, LAY_BUFF1, LCD_XSIZE_TFT);
					len_disp = len_disp + W_Size + dis_x;
				}

				i += 2;
				if (i >= len_max)
					break;
			}
			else
			{
				break;
			}
		}
	}
	else if (encode == 1) // GBK
	{
		while ((c[i] != '\0'))
		{
			WDT_FeedDog(); //Clear watchdog

			if (star == 1 && c[i] >= 0x81 && c[i + 1] >= 0x40 && c[i] != 0xFF)
			{
				star_flag = 1;
	
				i++;
			}

			if ((c[i] < 128 && c[i] > 0x00) || star_flag == 1)
			{

				if (star == 1)
				{
					star_flag = 0;
					temp = '*';
				}
				else
					temp = c[i];
				if (((len_disp + Ascii_w[temp - 0x20]) > W_Len) || c[i] == 0x0A)
				{
					row_w[row] = len_disp;
					row++;
					offset_disp += H_Size;
					len_disp = 0;

					if ((offset_disp + H_Size) > H_Len)
						break;
				}
				if (c[i] != 0x0A) // New line
				{
					if (Screen_DIR == 0)
					{
						x_src = len_disp;
						y_src = offset_disp;
					}
					else if (Screen_DIR == 1)
					{
						x_src = LCD_XSIZE_TFT - H_Size - offset_disp;
						y_src = len_disp;
					}
					else if (Screen_DIR == 2)
					{
						x_src = LCD_XSIZE_TFT - width_Hor * temp_pixel - len_disp;
						y_src = LCD_YSIZE_TFT - H_Size - offset_disp;
					}
					else if (Screen_DIR == 3)
					{
						x_src = offset_disp;
						y_src = LCD_YSIZE_TFT - width_Hor * temp_pixel - len_disp;
					}
					LT768_Draw_16bit(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, FlashAddr + 104 + (temp - 0x20) * width_Hor * H_Size, LAY_BUFF1, LCD_XSIZE_TFT);
					len_disp = len_disp + Ascii_w[temp - 0x20] + 2;
				}

				i++;
				if (i >= len_max)
					break;
			}
			else if (c[i] >= 0x81 && c[i + 1] >= 0x40 && c[i] != 0xFF)
			{
				temp_H = (c[i] - 0x81) * 190;
				if (c[i + 1] <= 0x7F)
				{
					temp_L = c[i + 1] - 0x40;
				}
				else if (c[i + 1] > 0x7F)
				{
					temp_L = c[i + 1] - 0x40 - 1;
				}
				temp = temp_H + temp_L;

				if ((len_disp + W_Size + dis_x) > W_Len)
				{
					row_w[row] = len_disp;
					row++;
					offset_disp += H_Size;
					len_disp = 0;
					if ((offset_disp + H_Size) > H_Len)
						break;
				}

				if (Screen_DIR == 0)
				{
					x_src = len_disp;
					y_src = offset_disp;
				}
				else if (Screen_DIR == 1)
				{
					x_src = LCD_XSIZE_TFT - H_Size - offset_disp;
					y_src = len_disp;
				}
				else if (Screen_DIR == 2)
				{
					x_src = LCD_XSIZE_TFT - width_Hor * temp_pixel - len_disp;
					y_src = LCD_YSIZE_TFT - H_Size - offset_disp;
				}
				else if (Screen_DIR == 3)
				{
					x_src = offset_disp;
					y_src = LCD_YSIZE_TFT - width_Hor * temp_pixel - len_disp;
				}

				LT768_Draw_16bit(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, FlashAddr + 104 + (95 + temp) * width_Hor * H_Size, LAY_BUFF1, LCD_XSIZE_TFT);

				if (dis_x > 0)
				{
					Active_Window_XY(0, 0);
					Active_Window_WH(LCD_XSIZE_TFT, LCD_YSIZE_TFT);
					if (Screen_DIR == 0)
						LT768_DrawSquare_Fill(x_src + W_Size, y_src, x_src + W_Size + dis_x, y_src + H_Size, Black);
					else if (Screen_DIR == 1)
						LT768_DrawSquare_Fill(x_src, y_src + W_Size, x_src + H_Size, y_src + W_Size + dis_x, Black);
					else if (Screen_DIR == 2)
						LT768_DrawSquare_Fill(x_src, y_src, x_src - dis_x, y_src + H_Size, Black);
					else if (Screen_DIR == 3)
						LT768_DrawSquare_Fill(x_src, y_src, x_src + H_Size, y_src - dis_x, Black);
				}

				len_disp = len_disp + W_Size + dis_x;

				i += 2;
				if (i >= len_max)
					break;
			}
			else
			{
				break;
			}
		}
	}
	else if (encode == 2) // BIG5
	{
		while ((c[i] != '\0'))
		{
			WDT_FeedDog(); //Clear watchdog
			if (star == 1 && c[i] >= 0xa1)
			{
				star_flag  = 1;
				i++;
			}
			if ((c[i] < 128 && c[i] > 0x00) || star_flag == 1)
			{
				if (star == 1)
				{
					star_flag = 0;
					temp = '*';
				}
				else
					temp = c[i];
				if (((len_disp + Ascii_w[temp - 0x20]) > W_Len) || c[i] == 0x0A)
				{
					row_w[row] = len_disp;
					row++;
					offset_disp += H_Size;
					len_disp = 0;
					if ((offset_disp + H_Size) > H_Len)
						break;
				}

				if (c[i] != 0x0A)
				{
					if (Screen_DIR == 0)
					{
						x_src = len_disp;
						y_src = offset_disp;
					}
					else if (Screen_DIR == 1)
					{
						x_src = LCD_XSIZE_TFT - H_Size - offset_disp;
						y_src = len_disp;
					}
					else if (Screen_DIR == 2)
					{
						x_src = LCD_XSIZE_TFT - width_Hor * temp_pixel - len_disp;
						y_src = LCD_YSIZE_TFT - H_Size - offset_disp;
					}
					else if (Screen_DIR == 3)
					{
						x_src = offset_disp;
						y_src = LCD_YSIZE_TFT - width_Hor * temp_pixel - len_disp;
					}
					LT768_Draw_16bit(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, FlashAddr + 104 + (temp - 0x20) * width_Hor * H_Size, LAY_BUFF1, LCD_XSIZE_TFT);
					len_disp = len_disp + Ascii_w[temp - 0x20] + 2;
				}

				i++;
				if (i >= len_max)
					break;
			}
			else if (c[i] >= 0xa1)
			{
				temp_H = ((c[i] - 0xa1) & 0x00ff) * 160;
				if (c[i + 1] >= 0x40 && c[i + 1] <= 0x7F)
					temp_L = c[i + 1] - 0x40;
				else if (c[i + 1] >= 0xA0 && c[i + 1] <= 0xFF)
					temp_L = c[i + 1] - 0xA0 + 64;

				temp = temp_H + temp_L;

				if ((len_disp + W_Size) > W_Len)
				{
					row_w[row] = len_disp;
					row++;
					offset_disp += H_Size;
					len_disp = 0;
					if ((offset_disp + H_Size) > H_Len)
						break;
				}

				if (Screen_DIR == 0)
				{
					x_src = len_disp;
					y_src = offset_disp;
				}
				else if (Screen_DIR == 1)
				{
					x_src = LCD_XSIZE_TFT - H_Size - offset_disp;
					y_src = len_disp;
				}
				else if (Screen_DIR == 2)
				{
					x_src = LCD_XSIZE_TFT - width_Hor * temp_pixel - len_disp;
					y_src = LCD_YSIZE_TFT - H_Size - offset_disp;
				}
				else if (Screen_DIR == 3)
				{
					x_src = offset_disp;
					y_src = LCD_YSIZE_TFT - width_Hor * temp_pixel - len_disp;
				}
				LT768_Draw_16bit(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, FlashAddr + 104 + (95 + temp) * width_Hor * H_Size, LAY_BUFF1, LCD_XSIZE_TFT);

				if (dis_x > 0)
				{
					Active_Window_XY(0, 0);
					Active_Window_WH(LCD_XSIZE_TFT, LCD_YSIZE_TFT);
					if (Screen_DIR == 0)
						LT768_DrawSquare_Fill(x_src + W_Size, y_src, x_src + W_Size + dis_x, y_src + H_Size, Black);
					else if (Screen_DIR == 1)
						LT768_DrawSquare_Fill(x_src, y_src + W_Size, x_src + H_Size, y_src + W_Size + dis_x, Black);
					else if (Screen_DIR == 3)
						LT768_DrawSquare_Fill(x_src, y_src, x_src + H_Size, y_src - dis_x, Black);
				}

				len_disp += W_Size;

				i += 2;
				if (i >= len_max)
					break;
			}
			else
				break;
		}
	}
	else if (encode == 3) // Unicode
	{
		Flash_Read_UI(buff_code, FlashAddr + 4, 4);
		start_code = buff_code[0] + (buff_code[1] << 8);
		end_code = buff_code[2] + (buff_code[3] << 8);

		while (((c[i] << 8) + c[i + 1]) > 0)
		{
			WDT_FeedDog(); //Clear watchdog
			unicode = (c[i] << 8) + c[i + 1];
			if (star == 1 && unicode >= start_code && unicode <= end_code)
			{
				unicode = '*';
			}
			if (unicode < 128 && unicode > 0x00)
			{
				if (star == 1)
					unicode = '*';
				if (((len_disp + Ascii_w[unicode - 0x20]) > W_Len) || unicode == 0x0A)
				{
					row_w[row] = len_disp;
					row++;
					offset_disp += H_Size;
					len_disp = 0;
					if ((offset_disp + H_Size) > H_Len)
						break;
				}

				if (unicode != 0x0A)
				{
					if (Screen_DIR == 0)
					{
						x_src = len_disp;
						y_src = offset_disp;
					}
					else if (Screen_DIR == 1)
					{
						x_src = LCD_XSIZE_TFT - H_Size - offset_disp;
						y_src = len_disp;
					}
					else if (Screen_DIR == 2)
					{
						x_src = LCD_XSIZE_TFT - width_Hor * temp_pixel - len_disp;
						y_src = LCD_YSIZE_TFT - H_Size - offset_disp;
					}
					else if (Screen_DIR == 3)
					{
						x_src = offset_disp;
						y_src = LCD_YSIZE_TFT - width_Hor * temp_pixel - len_disp;
					}
					LT768_Draw_16bit(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, FlashAddr + 104 + (unicode - 0x20) * width_Hor * H_Size, LAY_BUFF1, LCD_XSIZE_TFT);
					len_disp = len_disp + Ascii_w[unicode - 0x20] + 2;
				}

				i += 2;
				if (i >= len_max)
					break;
			}
			else if (unicode >= start_code && unicode <= end_code)
			{
				if ((len_disp + W_Size) > W_Len)
				{
					row_w[row] = len_disp;
					row++;
					offset_disp += H_Size;
					len_disp = 0;
					if ((offset_disp + H_Size) > H_Len)
						break;
				}
				temp = unicode - start_code;

				if (Screen_DIR == 0)
				{
					x_src = len_disp;
					y_src = offset_disp;
				}
				else if (Screen_DIR == 1)
				{
					x_src = LCD_XSIZE_TFT - H_Size - offset_disp;
					y_src = len_disp;
				}
				else if (Screen_DIR == 2)
				{
					x_src = LCD_XSIZE_TFT - width_Hor * temp_pixel - len_disp;
					y_src = LCD_YSIZE_TFT - H_Size - offset_disp;
				}
				else if (Screen_DIR == 3)
				{
					x_src = offset_disp;
					y_src = LCD_YSIZE_TFT - width_Hor * temp_pixel - len_disp;
				}
				LT768_Draw_16bit(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, FlashAddr + 104 + (95 + temp) * width_Hor * H_Size, LAY_BUFF1, LCD_XSIZE_TFT);
				len_disp = len_disp + W_Size;

				i += 2;
				if (i >= len_max)
					break;
			}
			else
				break;
		}
	}
	else if (encode == 6) // Unicode
	{
		Flash_Read_UI(buff_code, FlashAddr + 4, 4);
		start_code = buff_code[0] + (buff_code[1] << 8);
		end_code = buff_code[2] + (buff_code[3] << 8);

		while (((c[i] << 8) + c[i + 1]) > 0)
		{
			WDT_FeedDog(); //Clear watchdog
			unicode = (c[i] << 8) + c[i + 1];
			//printf("unicode[%d]=0x%x \r\n",i,unicode);
			if (star == 1 && unicode >= start_code && unicode <= end_code)
			{
				unicode = '*';
			}
			if (unicode < 128 && unicode > 0x00)
			{
				//printf("unicode=%x \r\n",unicode);
				if (star == 1)
					unicode = '*';
				if (((len_disp + Ascii_w[unicode - 0x20]) > W_Len) || unicode == 0x0A)
				{

					row_w[row] = len_disp;
					row++;
					offset_disp += H_Size;
					len_disp = 0;
					if ((offset_disp + H_Size) > H_Len)
						break;
				}

				if (unicode != 0x0A)
				{
					if (Screen_DIR == 0)
					{
						x_src = len_disp;
						y_src = offset_disp;
					}
					else if (Screen_DIR == 1)
					{
						x_src = LCD_XSIZE_TFT - H_Size - offset_disp;
						y_src = len_disp;
					}
					else if (Screen_DIR == 2)
					{
						x_src = LCD_XSIZE_TFT - width_Hor * temp_pixel - len_disp;
						y_src = LCD_YSIZE_TFT - H_Size - offset_disp;
					}
					else if (Screen_DIR == 3)
					{
						x_src = offset_disp;
						y_src = LCD_YSIZE_TFT - width_Hor * temp_pixel - len_disp;
					}
					
					LT768_Draw_16bit(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, FlashAddr + 104 + (unicode - 0x20) * width_Hor * H_Size, LAY_BUFF1, LCD_XSIZE_TFT);
					len_disp = len_disp + Ascii_w[unicode - 0x20] + 2;
					
				}

				i += 2;
				if (i >= len_max)
					break;
			}
			else if (unicode >= start_code && unicode <= end_code)
			{
				Flash_Read_UI(&Font_W, FlashAddr + 104 + 95 * width_Hor * H_Size + (unicode - start_code) * (width_Hor * H_Size + 1), 1);
				
				if ((len_disp + Font_W) > W_Len)
				{
					row_w[row] = len_disp;
					row++;
					offset_disp += H_Size;
					len_disp = 0;
					if ((offset_disp + H_Size) > H_Len)
						break;
				}
				temp = unicode - start_code;

				if (Screen_DIR == 0)
				{
					x_src = len_disp;
					y_src = offset_disp;
				}
				else if (Screen_DIR == 1)
				{
					x_src = LCD_XSIZE_TFT - H_Size - offset_disp;
					y_src = len_disp;
				}
				else if (Screen_DIR == 2)
				{
					x_src = LCD_XSIZE_TFT - width_Hor * temp_pixel - len_disp;
					y_src = LCD_YSIZE_TFT - H_Size - offset_disp;
				}
				else if (Screen_DIR == 3)
				{
					x_src = offset_disp;
					y_src = LCD_YSIZE_TFT - width_Hor * temp_pixel - len_disp;
				}
				LT768_Draw_16bit(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, FlashAddr + 104 + 95 * width_Hor * H_Size + temp * (width_Hor * H_Size + 1) + 1, LAY_BUFF1, LCD_XSIZE_TFT);
				len_disp = len_disp + Font_W + 2;

				if (TFT_bitcolor == 1 && pixel_format != 0)
				{
					Canvas_Image_Start_address(LAY_BUFF1);
					Canvas_image_width(LCD_XSIZE_TFT);
					Active_Window_XY(0, 0);
					Active_Window_WH(LCD_XSIZE_TFT, LCD_YSIZE_TFT);
					Goto_Pixel_XY(0, 0);
					Foreground_color_65k(0);
					Square_Start_XY(x_src + Font_W, y_src);
					Square_End_XY(x_src + Font_W + 2, y_src + H_Size);
					Start_Square_Fill();
					Check_2D_Busy();
				}
				else
				{
					Canvas_Image_Start_address(LAY_BUFF1);
					Canvas_image_width(LCD_XSIZE_TFT);
					Active_Window_XY(0, 0);
					Active_Window_WH(LCD_XSIZE_TFT, LCD_YSIZE_TFT);
					Goto_Pixel_XY(0, 0);
					LT768_DrawSquare_Fill(x_src + Font_W, y_src, x_src + Font_W + 1,y_src + H_Size, Black);
				}
				i += 2;
				if (i >= len_max)
					break;
			}
			else
				break;
		}
	}

	if (TFT_bitcolor == 1 && pixel_format != 0)
		Memory_24bpp_Mode();

	Active_Window_XY(0, 0);
	Active_Window_WH(LCD_XSIZE_TFT, LCD_YSIZE_TFT);
	Goto_Pixel_XY(0, 0);

	data_w = len_disp; // For input method

	if (len_disp != 0)
	{
		row_w[row] = len_disp -2;

		row++;
	}

	// Alignment
	if (Alignment == 0 || Alignment == 1 || Alignment == 2) // Mid
		offset_disp = (H_Len - row * H_Size) / 2;
	else if (Alignment == 3 || Alignment == 4 || Alignment == 5) // Upper
		offset_disp = 0;
	else if (Alignment == 6 || Alignment == 7 || Alignment == 8) // Below
		offset_disp = H_Len - row * H_Size;

	for (i = 0; i < row; i++)
	{
		if (Alignment == 0 || Alignment == 3 || Alignment == 6) // Left alignment 
		{
			if (Screen_DIR == 0)
			{
				x_src = 0;
				y_src = i * H_Size;
				x_des = Xs;
				y_des = Ys + offset_disp + i * H_Size;
				w_real = row_w[i];
				h_real = H_Size;
			}
			else if (Screen_DIR == 1)
			{
				x_src = LCD_XSIZE_TFT - (1 + i) * H_Size;
				y_src = 0;
				x_des = Xe - offset_disp - (1 + i) * H_Size + 1;
				y_des = Ys;
				w_real = H_Size;
				h_real = row_w[i];
			}
			else if (Screen_DIR == 2)
			{
				x_src = LCD_XSIZE_TFT - row_w[i];
				y_src = LCD_YSIZE_TFT - (1 + i) * H_Size;
				x_des = Xe - row_w[i] + 1;
				y_des = Ye - offset_disp - (1 + i) * H_Size + 1;
				w_real = row_w[i];
				h_real = H_Size;
			}
			else if (Screen_DIR == 3)
			{
				x_src = i * H_Size;
				y_src = LCD_YSIZE_TFT - row_w[i];
				x_des = Xs + offset_disp + i * H_Size;
				y_des = Ye - row_w[i] + 1;
				w_real = H_Size;
				h_real = row_w[i];
			}
		}
		else if (Alignment == 1 || Alignment == 4 || Alignment == 7) // Mid alignment 
		{
			if (Screen_DIR == 0)
			{
				x_src = 0;
				y_src = i * H_Size;
				x_des = Xs + (W_Len - row_w[i]) / 2;
				y_des = Ys + offset_disp + i * H_Size;
				w_real = row_w[i];
				h_real = H_Size;
			}
			else if (Screen_DIR == 1)
			{
				x_src = LCD_XSIZE_TFT - (1 + i) * H_Size;
				y_src = 0;
				x_des = Xe - offset_disp - (1 + i) * H_Size + 1;
				y_des = Ys + (W_Len - row_w[i]) / 2;
				w_real = H_Size;
				h_real = row_w[i];
			}
			else if (Screen_DIR == 2)
			{
				x_src = LCD_XSIZE_TFT - row_w[i];
				y_src = LCD_YSIZE_TFT - (1 + i) * H_Size;
				x_des = Xs + (W_Len - row_w[i]) / 2;
				;
				y_des = Ye - offset_disp - (1 + i) * H_Size + 1;
				w_real = row_w[i];
				h_real = H_Size;
			}
			else if (Screen_DIR == 3)
			{
				x_src = i * H_Size;
				y_src = LCD_YSIZE_TFT - row_w[i];
				x_des = Xs + offset_disp + i * H_Size;
				y_des = Ys + (W_Len - row_w[i]) / 2;
				w_real = H_Size;
				h_real = row_w[i];
			}
		}
		else if (Alignment == 2 || Alignment == 5 || Alignment == 8) // Right alignment 
		{
			if (Screen_DIR == 0)
			{
				x_src = 0;
				y_src = i * H_Size;
				x_des = Xe - row_w[i] + 1;
				y_des = Ys + offset_disp + i * H_Size;
				w_real = row_w[i];
				h_real = H_Size;
			}
			else if (Screen_DIR == 1)
			{
				x_src = LCD_XSIZE_TFT - (1 + i) * H_Size;
				y_src = 0;
				x_des = Xe - offset_disp - (1 + i) * H_Size + 1;
				y_des = Ye - row_w[i] + 1;
				w_real = H_Size;
				h_real = row_w[i];
			}
			else if (Screen_DIR == 2)
			{
				x_src = LCD_XSIZE_TFT - row_w[i];
				y_src = LCD_YSIZE_TFT - (1 + i) * H_Size;
				x_des = Xs;
				y_des = Ye - offset_disp - (1 + i) * H_Size + 1;
				w_real = row_w[i];
				h_real = H_Size;
			}
			else if (Screen_DIR == 3)
			{
				x_src = i * H_Size;
				y_src = LCD_YSIZE_TFT - row_w[i];
				x_des = Xs + offset_disp + i * H_Size;
				y_des = Ys;
				w_real = H_Size;
				h_real = row_w[i];
			}
		}
		
		/*test*/
		for (uint8_t k = 0; k < Gif_num; k++)
		{
			if (Xs >= gGif_Info[k].x && Xs <= gGif_Info[k].x + gGif_Info[k].w &&
				Ys >= gGif_Info[k].y && Ys <= gGif_Info[k].y + gGif_Info[k].h && gGif_Info[k].on_gif == 1)
			{
				on_gif = 1;
				break;
			}
		}
		

		if (pixel_format == 0)
		{
			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
											 ShowAddr, width, x_des, y_des,
											 Black, w_real, h_real);
		}
		else if (pixel_format == 1 || pixel_format == 2)
		{
			
			if (on_gif == 0)
			{
				BTE_Pixel_16bpp_Alpha_Blending(ShowAddr, LCD_XSIZE_TFT, x_des, y_des,
											   LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
											   ShowAddr, LCD_XSIZE_TFT, x_des, y_des,
											   w_real, h_real);
			}
			else if (on_gif == 1)
			{
				BTE_Pixel_16bpp_Alpha_Blending(ShowAddr, LCD_XSIZE_TFT, x_des, y_des,
											   LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
											   ShowAddr, LCD_XSIZE_TFT, x_des, y_des,
											   w_real, h_real);
				
				Canvas_Image_Start_address(LAY_BUFF5);
				Canvas_image_width(LCD_XSIZE_TFT);
				LT768_DrawSquare_Fill(Xs, Ys, Xe, Ye, Black);
						
				LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
									  LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
									  LAY_BUFF5, LCD_XSIZE_TFT, x_des, y_des,
									  0x0c, w_real, h_real);
			}
		}
	}
}

/***********************************************************************************
* Function Name	: text_display
* Description	: Show text.
* Input			: - type: text info sources
				  - *buff: text info
* Output		: None
* Return		: None
************************************************************************************/

void text_display(uint8_t type, uint8_t *buff)
{
	uint16_t Zk_length;
	uint16_t xs, ys, xe, ye; // Display box
	uint8_t Zk_id, w, h;
	uint8_t encode; // encoding type
	uint8_t Alignment, transparent, star;
	uint32_t fcolor, bcolor;
	uint16_t p_add, p_len, pointer;
	uint32_t zk_addr;
	uint8_t buff1[4] = {0};
	uint8_t on_gif = 0;
	if (type == 0) // Instruction information source is configuration file
	{
		p_add = (buff[5] + (buff[6] << 8)) * 2;
		p_len = buff[7] + (buff[8] << 8);
		xs = buff[9] + (buff[10] << 8);
		ys = buff[11] + (buff[12] << 8);
		xe = buff[13] + (buff[14] << 8);
		ye = buff[15] + (buff[16] << 8);
		w = buff[17];
		h = buff[18];
		Zk_id = buff[19];
		encode = buff[20];
		Alignment = buff[21];
		bcolor = buff[22] + (buff[23] << 8) + (buff[24] << 16);
		fcolor = buff[25] + (buff[26] << 8) + (buff[27] << 16);
		transparent = buff[28] & 0x01;
		star = (buff[28] & 0x02) >> 1;
	}
	else if (type == 1) // Instruction information ource is variable space
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		p_add = (var[pointer + 1] + (var[pointer + 2] << 8)) * 2;
		p_len = var[pointer + 3] + (var[pointer + 4] << 8);
		xs = var[pointer + 5] + (var[pointer + 6] << 8);
		ys = var[pointer + 7] + (var[pointer + 8] << 8);
		xe = var[pointer + 9] + (var[pointer + 10] << 8);
		ye = var[pointer + 11] + (var[pointer + 12] << 8);
		w = var[pointer + 13];
		h = var[pointer + 14];
		Zk_id = var[pointer + 15];
		encode = var[pointer + 16];
		Alignment = var[pointer + 17];
		bcolor = var[pointer + 18] + (var[pointer + 19] << 8) + (var[pointer + 20] << 16);
		fcolor = var[pointer + 21] + (var[pointer + 22] << 8) + (var[pointer + 23] << 16);
		transparent = var[pointer + 24] & 0x01 ;
		star = (var[pointer + 24] & 0x02) >> 1;	
	}
	if (Pop_up_flag == 1) // Popup offset
	{
		xs += Pop_up_x;
		ys += Pop_up_y;
		xe += Pop_up_x;
		ye += Pop_up_y;
	}

	Flash_Read_UI(buff1, addr_index[2] + Zk_id * 8, 4);
	zk_addr = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);

	/*test*/
	for (uint8_t i = 0; i < Gif_num; i++)
	{
		if (xs >= gGif_Info[i].x && xs <= gGif_Info[i].x + gGif_Info[i].w &&
			ys >= gGif_Info[i].y && ys <= gGif_Info[i].y + gGif_Info[i].h && gGif_Info[i].on_gif == 1)
		{
			on_gif = 1;
			break;
		}
	}

	if (transparent == 0) 		// Transparent background
	{
		if (on_gif == 0)
		{
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, xs, ys,
								  MAIN_BUFF, LCD_XSIZE_TFT, xs, ys,
								  LAY_BUFF2, LCD_XSIZE_TFT, xs, ys,
								  0x0c, xe - xs + 1, ye - ys + 1);
		}
		else if (on_gif == 1)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF6, LCD_XSIZE_TFT, xs, ys,
								  LAY_BUFF6, LCD_XSIZE_TFT, xs, ys,
								  LAY_BUFF2, LCD_XSIZE_TFT, xs, ys,
								  0x0c, xe - xs + 1, ye - ys + 1);
		}
	}
	else if (transparent == 1) // Background color
	{
		Canvas_Image_Start_address(LAY_BUFF2);
		Canvas_image_width(LCD_XSIZE_TFT);
		LT768_DrawSquare_Fill(xs, ys, xe, ye, bcolor);
	}

	LT_Print_zk_Font(encode, zk_addr, LAY_BUFF2, LCD_XSIZE_TFT,
					 w, h, Alignment, fcolor, star,
					 xs, ys, xe, ye, 0, 0, &var[p_add], p_len * 2);

	LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, xs, ys,
						  LAY_BUFF2, LCD_XSIZE_TFT, xs, ys,
						  Display_Layer, LCD_XSIZE_TFT, xs, ys,
						  0x0c, xe - xs + 1, ye - ys + 1);

}

/************************************************/

/*******************************************************************************
 * Function Name	: LT_Print_zk_ASCII
 * Description		: Display a ASCII.
 * Input			:
 * Output       	: None
 * Return       	: None
 ********************************************************************/
void LT_Print_zk_ASCII
(
	uint32_t FlashAddr	// Font source address(exSpiFlash)
	,
	uint32_t ShowAddr	// Displays the address of the layer
	,
	uint16_t width		// Display the width of the layer
	,
	uint8_t W_Size		// Font width
	,
	uint8_t H_Size		// Font height
	,
	uint8_t Alignment	// Alignment 0:Left	1:Mid	2:Right
	,
	uint32_t FontColor	// The foreground color of the font
	,
	uint16_t Xs			// Text box start x position
	,
	uint16_t Ys			// Text box start Y position
	,
	uint8_t *c			// Address of text
)
{
	uint32_t i = 0;
	uint16_t width_Hor = 0;
	uint8_t pixel_format = 1;
	uint8_t temp_pixel;

	uint16_t x_src = 0, y_src = 0, x_des = 0, y_des = 0, w_real = 0, h_real = 0;
	uint16_t len_disp = 0;

	Flash_Read_UI(Ascii_w, FlashAddr + 9, 95);
	Flash_Read_UI(&pixel_format, FlashAddr + 8, 1);
	if (pixel_format == 0)
		temp_pixel = 8;
	else if (pixel_format == 1)
		temp_pixel = 4;
	else if (pixel_format == 2)
		temp_pixel = 2;

	if (W_Size % temp_pixel != 0)
		width_Hor = W_Size / temp_pixel + 1;
	else
		width_Hor = W_Size / temp_pixel;

	if (TFT_bitcolor == 1 && pixel_format != 0)
		Memory_16bpp_Mode();
	while ((c[i] != '\0'))
	{
		WDT_FeedDog(); //Clear watchdog
		if (c[i] < 128 && c[i] > 0x00)
		{
			if (Screen_DIR == 0)
			{
				x_src = len_disp;
				y_src = 0;
			}
			else if (Screen_DIR == 1)
			{
				x_src = LCD_XSIZE_TFT - H_Size;
				y_src = len_disp;
			}
			else if (Screen_DIR == 2)
			{
				x_src = LCD_XSIZE_TFT - width_Hor * temp_pixel - len_disp;
				y_src = LCD_YSIZE_TFT - H_Size;
			}
			else if (Screen_DIR == 3)
			{
				x_src = 0;
				y_src = LCD_YSIZE_TFT - width_Hor * temp_pixel - len_disp;
			}
			LT768_Draw_16bit(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor,
							 FlashAddr + 104 + (c[i] - 0x20) * width_Hor * H_Size, LAY_BUFF1, LCD_XSIZE_TFT);

			len_disp = len_disp + Ascii_w[c[i] - 0x20] + 2;
			i++;
		}
		else
			break;
	}
	if (TFT_bitcolor == 1 && pixel_format != 0)
		Memory_24bpp_Mode();

	Active_Window_XY(0, 0);
	Active_Window_WH(LCD_XSIZE_TFT, LCD_YSIZE_TFT);
	Goto_Pixel_XY(0, 0);

	data_w = len_disp;

	if (Alignment == 0)
	{
		if (Screen_DIR == 0)
		{
			x_src = 0;
			y_src = 0;
			x_des = Xs;
			y_des = Ys;
			w_real = len_disp;
			h_real = H_Size;
		}
		else if (Screen_DIR == 1)
		{
			x_src = LCD_XSIZE_TFT - H_Size;
			y_src = 0;
			x_des = Xs - H_Size + 1;
			y_des = Ys;
			w_real = H_Size;
			h_real = len_disp;
		}
		else if (Screen_DIR == 2)
		{
			x_src = LCD_XSIZE_TFT - len_disp;
			y_src = LCD_YSIZE_TFT - H_Size;
			x_des = Xs - len_disp + 1;
			y_des = Ys - H_Size + 1;
			w_real = len_disp;
			h_real = H_Size;
		}
		else if (Screen_DIR == 3)
		{
			x_src = 0;
			y_src = LCD_YSIZE_TFT - len_disp;
			x_des = Xs;
			y_des = Ys - len_disp + 1;
			w_real = H_Size;
			h_real = len_disp;
		}
	}
	else if (Alignment == 2)
	{
		if (Screen_DIR == 0)
		{
			x_src = 0;
			y_src = 0;
			x_des = Xs - len_disp + 1;
			y_des = Ys;
			w_real = len_disp;
			h_real = H_Size;
		}
		else if (Screen_DIR == 1)
		{
			x_src = LCD_XSIZE_TFT - H_Size;
			y_src = 0;
			x_des = Xs - H_Size;
			y_des = Ys - len_disp + 1;
			w_real = H_Size;
			h_real = len_disp;
		}
		else if (Screen_DIR == 2)
		{
			x_src = LCD_XSIZE_TFT - len_disp;
			y_src = LCD_YSIZE_TFT - H_Size;
			x_des = Xs;
			y_des = Ys - H_Size + 1;
			w_real = len_disp;
			h_real = H_Size;
		}
		else if (Screen_DIR == 3)
		{
			x_src = 0;
			y_src = LCD_YSIZE_TFT - len_disp;
			x_des = Xs;
			y_des = Ys;
			w_real = H_Size;
			h_real = len_disp;
		}
	}
	else if (Alignment == 1)
	{
		if (Screen_DIR == 0)
		{
			x_src = 0;
			y_src = 0;
			x_des = Xs - len_disp / 2 + 1;
			y_des = Ys;
			w_real = len_disp;
			h_real = H_Size;
		}
		else if (Screen_DIR == 1)
		{
			x_src = LCD_XSIZE_TFT - H_Size;
			y_src = 0;
			x_des = Xs - H_Size;
			y_des = Ys - len_disp / 2 + 1;
			w_real = H_Size;
			h_real = len_disp;
		}
		else if (Screen_DIR == 2)
		{
			x_src = LCD_XSIZE_TFT - len_disp;
			y_src = LCD_YSIZE_TFT - H_Size;
			x_des = Xs - len_disp / 2 + 1;
			y_des = Ys - H_Size + 1;
			w_real = len_disp;
			h_real = H_Size;
		}
		else if (Screen_DIR == 3)
		{
			x_src = 0;
			y_src = LCD_YSIZE_TFT - len_disp;
			x_des = Xs;
			y_des = Ys - len_disp / 2 + 1;
			w_real = H_Size;
			h_real = len_disp;
		}
	}
	if (pixel_format == 0)
	{
		LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
										 ShowAddr, width, x_des, y_des,
										 Black, w_real, h_real);
	}
	else if (pixel_format == 1 || pixel_format == 2)
	{
		BTE_Pixel_16bpp_Alpha_Blending(ShowAddr, width, x_des, y_des,
									   LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
									   ShowAddr, width, x_des, y_des,
									   w_real, h_real);
	}
}

/***********************************************************************************
* Function Name	: data_display
* Description	: Show numbers in font.
* Input			: - type: data info sources
				  - buff: data info
* Output		: None
* Return		: None
************************************************************************************/
void data_display(uint8_t type, uint8_t *buff)
{
	uint8_t font_w, font_h, zk_id, Alignment;
	uint8_t N_int, D_int;
	uint8_t unit_len, unit_buff[11];
	uint8_t buff1[4] = {0};
	uint8_t data_type;
	uint16_t p_add, pointer;
	uint16_t x, y;
	uint32_t fcolor, zk_addr;
	
	uint8_t i, Symbol = 0, n = 0, k = 0,m = 0;
	uint8_t value_1 = 0;
	uint16_t value_2 = 0;
	uint32_t value_4 = 0;
	uint64_t value_8 = 0, ll;
	uint16_t dx, dy, dw, dh, w_temp = 0, h_temp = 0;
	
	uint8_t leading_zero = 0;
	memset(Display_buff, 0, 100);

	if (type == 0)		// Instruction information source is configuration file
	{
		p_add = (buff[5] + (buff[6] << 8)) * 2;
		x = buff[7] + (buff[8] << 8);
		y = buff[9] + (buff[10] << 8);
		fcolor = buff[11] + (buff[12] << 8) + (buff[13] << 16);
		zk_id = buff[14];
		font_w = buff[15];
		Alignment = buff[16] & 0x7F;
		leading_zero = (buff[16] & 0x80) >> 7;
		N_int = buff[17];
		D_int = buff[18];
		data_type = buff[19];
		unit_len = buff[20];
	}
	else if (type == 1)	// Instruction information ource is variable space
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		p_add = (var[pointer + 1] + (var[pointer + 2] << 8)) * 2;
		x = var[pointer + 3] + (var[pointer + 4] << 8);
		y = var[pointer + 5] + (var[pointer + 6] << 8);
		fcolor = var[pointer + 7] + (var[pointer + 8] << 8) + (var[pointer + 9] << 16);
		zk_id = var[pointer + 10];
		font_w = var[pointer + 11];
		Alignment = var[pointer + 12] & 0x7F;
		leading_zero = (var[pointer + 12] & 0x80) >> 7;
		N_int = var[pointer + 13];
		D_int = var[pointer + 14];
		data_type = var[pointer + 15];
		unit_len = var[pointer + 16];
	}

	if (Pop_up_flag == 1) // Popup offset
	{
		x += Pop_up_x;
		y += Pop_up_y;
	}
	Flash_Read_UI(buff1, addr_index[2] + zk_id * 8, 4);
	zk_addr = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
	Flash_Read_UI(&font_h, zk_addr + 3, 1);

	if (data_type == 0x00)
	{
		value_8 = var[p_add + 1];
	}
	else if (data_type == 0x01)
	{
		value_8 = (var[p_add] << 8) + var[p_add + 1];
	}
	else if (data_type == 0x02)
	{
		value_8 = (0xFF000000 & (var[p_add] << 24)) | (var[p_add + 1] << 16) | (var[p_add + 2] << 8) | var[p_add + 3];
	}
	else if (data_type == 0x03)
	{
		value_8 = (0xFF000000 & (var[p_add] << 24)) + (var[p_add + 1] << 16) + (var[p_add + 2] << 8) + var[p_add + 3];
		value_8 = ((value_8 << 24) << 8) + (0xFF000000 & (var[p_add + 4] << 24)) + (var[p_add + 5] << 16) + (var[p_add + 6] << 8) + var[p_add + 7];
		if (value_8 >= 0x8000000000000000)
		{
			value_8 = ~value_8 + 1;
			Symbol = 1;
		}
	}
	else if (data_type == 0x04)
	{
		value_8 = var[p_add];
	}
	else if (data_type == 0x80)
	{
		value_1 = var[p_add + 1];
		if (value_1 >= 0x80)
		{
			value_1 = ~value_1 + 1;
			Symbol = 1;
		}
		value_8 = value_1;
	}
	else if (data_type == 0x81)
	{
		value_2 = (var[p_add] << 8) + var[p_add + 1];
		if (value_2 >= 0x8000)
		{
			value_2 = ~value_2 + 1;
			Symbol = 1;
		}
		value_8 = value_2;
	}
	else if (data_type == 0x82)
	{
		value_4 = (var[p_add] << 24) + (var[p_add + 1] << 16) + (var[p_add + 2] << 8) + var[p_add + 3];
		if (value_4 >= 0x80000000)
		{
			value_4 = ~value_4 + 1;
			Symbol = 1;
		}
		value_8 = value_4;
	}
	else if (data_type == 0x83)
	{
		value_8 = (0xFF000000 & (var[p_add] << 24)) + (var[p_add + 1] << 16) + (var[p_add + 2] << 8) + var[p_add + 3];
		value_8 = ((value_8 << 24) << 8) + (0xFF000000 & (var[p_add + 4] << 24)) + (var[p_add + 5] << 16) + (var[p_add + 6] << 8) + var[p_add + 7];
		if (value_8 >= 0x8000000000000000)
		{
			value_8 = ~value_8 + 1;
			Symbol = 1;
		}
	}
	else if(data_type == 0x84)
	{
		value_1 = var[p_add];
		if (value_1 >= 0x80)
		{
			value_1 = ~value_1 + 1;
			Symbol = 1;
		}
		value_8 = value_1;
	}

	ll = pow(10, N_int + D_int);
	value_8 = value_8 % ll;
	if (Symbol == 1)
		Display_buff[0] = '-';

	ll = value_8;
	do
	{ // Get the number of digits
		n++;
		ll /= 10;
	} while (ll > 0);
	if (D_int > 0) // With decimal point
	{
		if (D_int >= n)
		{
			if (leading_zero == 0)
			{
				Display_buff[Symbol] = '0';
				Display_buff[Symbol + 1] = '.';
			}
			else if (leading_zero == 1)
			{
				for(m = 0; m < N_int; m++)
				{
					Display_buff[Symbol + m] = '0';
				}
				m--;
				Display_buff[Symbol + m + 1] = '.';
			}
			for (i = n; D_int > i; i++)
			{
				Display_buff[Symbol + m + 2 + i - n] = '0';
			}
	
			sprintf(&Display_buff[Symbol + m + 2 + D_int - n], "%lld", value_8);

			for (k = 0; k < unit_len; k++)
			{
				if (type == 0)
					Display_buff[Symbol + m + 2 + D_int + k] = buff[21 + k];
				else if (type == 1)
					Display_buff[Symbol + m + 2 + D_int + k] = var[pointer + 17 + k];
			}
		}
		else
		{
			if (leading_zero == 1)
			{
				for(m = 0; m < D_int + N_int - n; m++)
				{
					Display_buff[Symbol + m] = '0';
				}
			}
			
			sprintf(&Display_buff[Symbol + m], "%lld", value_8);

			for (i = 0; i < D_int; i++)
			{
				Display_buff[n + Symbol + m - i] = Display_buff[n - 1 + Symbol + m - i];
			}

			Display_buff[n + Symbol + m - D_int] = '.';

			for (k = 0; k < unit_len; k++)
			{
				if (type == 0)
					Display_buff[Symbol + m + n + 1 + k] = buff[21 + k];
				else if (type == 1)
					Display_buff[Symbol + m + n + 1 + k] = var[pointer + 17 + k];
			}
		}
	}
	else
	{
		if (leading_zero == 1)
		{
			for(m = 0; m < D_int + N_int - n; m++)
			{
				Display_buff[Symbol + m] = '0';
			}
		}
		sprintf(&Display_buff[Symbol + m], "%lld", value_8);

		for (k = 0; k < unit_len; k++)
		{
			if (type == 0)
				Display_buff[Symbol + m + n + k] = buff[21 + k];
			else if (type == 1)
				Display_buff[Symbol + m + n + k] = var[pointer + 17 + k];
		}
		Display_buff[Symbol + m + n + k] = 0xFF;
	}

	dh = font_h;
	if (D_int > 0)
	{
		if ((data_type & 0x80) > 0)
			dw = font_w * (N_int + D_int + 2);
		else
			dw = font_w * (N_int + D_int + 1);
	}
	else
	{
		if ((data_type & 0x80) > 0)
			dw = font_w * (N_int + 1);
		else
			dw = font_w * N_int;
	}

	if (Alignment == 0) // Left alignment
	{
		if (Screen_DIR == 0)
		{
			dx = x;
			dy = y;
			w_temp = dw;
			h_temp = dh;
		}
		else if (Screen_DIR == 1)
		{
			dx = x - dh + 1;
			dy = y;
			w_temp = dh;
			h_temp = dw;
		}
		else if (Screen_DIR == 2)
		{
			if(x < dw) dx = 0;
			else dx = x - dw + 1;
			dy = y - dh + 1;
			if(dx + dw > LCD_XSIZE_TFT) 
				dw = LCD_XSIZE_TFT - dx;
			w_temp = dw;
			h_temp = dh;
		}
		else if (Screen_DIR == 3)
		{
			dx = x;
			if(y < dw) dy = 0;
			else dy = y - dw + 1;
			if(dy + dw > LCD_YSIZE_TFT) 
				dw = LCD_YSIZE_TFT - dy;
			w_temp = dh;
			h_temp = dw;
		}
	}
	else if (Alignment == 2) // Right alignment
	{
		if (Screen_DIR == 0)
		{
			dx = x - dw + 1;
			dy = y;
			w_temp = dw;
			h_temp = dh;
		}
		else if (Screen_DIR == 1)
		{
			dx = x - dh + 1;
			dy = y - dw + 1;
			w_temp = dh;
			h_temp = dw;
		}
		else if (Screen_DIR == 2)
		{
			dx = x;
			dy = y - dh + 1;
			if(dx + dw > LCD_XSIZE_TFT)
				dw = LCD_XSIZE_TFT - dx;
			w_temp = dw;
			h_temp = dh;
		}
		else if (Screen_DIR == 3)
		{
			dx = x;
			dy = y;
			if(dy + dw > LCD_YSIZE_TFT) 
				dw = LCD_YSIZE_TFT - dy;
			w_temp = dh;
			h_temp = dw;
		}
	}
	else if (Alignment == 1) // Mid alignment
	{
		if (Screen_DIR == 0)
		{
			dx = x - dw / 2 + 1;
			dy = y;
			w_temp = dw;
			h_temp = dh;
		}
		else if (Screen_DIR == 1)
		{
			dx = x - dh + 1;
			dy = y - dw / 2 + 1;
			w_temp = dh;
			h_temp = dw;
		}
		else if (Screen_DIR == 2)
		{
			if(x < dw) dx = 0;
			else dx = x - dw / 2 + 1;
			dy = y - dh + 1;
			if(dx + dw > LCD_XSIZE_TFT) 
				dw = LCD_XSIZE_TFT - dx;
			w_temp = dw;
			h_temp = dh;
		}
		else if (Screen_DIR == 3)
		{
			dx = x;
			if(y < dw / 2) dy = 0;
			else dy = y - dw / 2 + 1;
			if(dy + dw > LCD_YSIZE_TFT) 
				dw = LCD_YSIZE_TFT - dy;
			w_temp = dh;
			h_temp = dw;
		}
	}

	LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
						  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
						  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
						  0x0c, w_temp, h_temp);

	LT_Print_zk_ASCII(zk_addr, LAY_BUFF1, LCD_XSIZE_TFT,
					  font_w, font_h, Alignment, fcolor,
					  x, y, Display_buff);

	if (data_w > data_D_w[data_D_num])
		dw = data_w;
	else
		dw = data_D_w[data_D_num];

	if (Alignment == 0) // Left alignment
	{
		if (Screen_DIR == 0)
		{
			dx = x;
			dy = y;
			w_temp = dw;
			h_temp = dh;
		}
		else if (Screen_DIR == 1)
		{
			dx = x - dh + 1;
			dy = y;
			w_temp = dh;
			h_temp = dw;
		}
		else if (Screen_DIR == 2)
		{
			if(x < dw) dx = 0;
			else dx = x - dw + 1;
			dy = y - dh + 1;
			if(dx + dw > LCD_XSIZE_TFT) 
				dw = LCD_XSIZE_TFT - dx;
			w_temp = dw;
			h_temp = dh;
		}
		else if (Screen_DIR == 3)
		{
			dx = x;
			if(y < dw) dy = 0;
			else dy = y - dw + 1;
			if(dy + dw > LCD_YSIZE_TFT) 
				dw = LCD_YSIZE_TFT - dy;
			w_temp = dh;
			h_temp = dw;
		}
	}
	else if (Alignment == 2) // Right alignment
	{
		if (Screen_DIR == 0)
		{
			dx = x - dw + 1;
			dy = y;
			w_temp = dw;
			h_temp = dh;
		}
		else if (Screen_DIR == 1)
		{
			dx = x - dh + 1;
			dy = y - dw + 1;
			w_temp = dh;
			h_temp = dw;
		}
		else if (Screen_DIR == 2)
		{
			dx = x;
			dy = y - dh + 1;
			if(dx + dw > LCD_XSIZE_TFT)
				dw = LCD_XSIZE_TFT - dx;
			w_temp = dw;
			h_temp = dh;
		}
		else if (Screen_DIR == 3)
		{
			dx = x;
			dy = y;
			if(dy + dw > LCD_YSIZE_TFT) 
				dw = LCD_YSIZE_TFT - dy;
			w_temp = dh;
			h_temp = dw;
		}
	}
	else if (Alignment == 1) // Mid alignment
	{
		if (Screen_DIR == 0)
		{
			dx = x - dw / 2 + 1;
			dy = y;
			w_temp = dw;
			h_temp = dh;
		}
		else if (Screen_DIR == 1)
		{
			dx = x - dh + 1;
			dy = y - dw / 2 + 1;
			w_temp = dh;
			h_temp = dw;
		}
		else if (Screen_DIR == 2)
		{
			if(x < dw) dx = 0;
			else dx = x - dw / 2 + 1;
			dy = y - dh + 1;
			if(dx + dw > LCD_XSIZE_TFT) 
				dw = LCD_XSIZE_TFT - dx;
			w_temp = dw;
			h_temp = dh;
		}
		else if (Screen_DIR == 3)
		{
			dx = x;
			if(y < dw / 2) dy = 0;
			else dy = y - dw / 2 + 1;
			if(dy + dw > LCD_YSIZE_TFT) 
				dw = LCD_YSIZE_TFT - dy;
			w_temp = dh;
			h_temp = dw;
		}
	}
	data_D_w[data_D_num] = data_w;

	LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
						  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
						  Display_Layer, LCD_XSIZE_TFT, dx, dy,
						  0x0c, w_temp, h_temp);
}

/***********************************************************************************
* Function Name	: icon_display
* Description	: Show a icon.
* Input			: - type: icon info sources
				  - buff: icon info
* Output		: None
* Return		: None 
************************************************************************************/
void icon_display(uint8_t type, uint8_t *buff)
{
	uint16_t p_add, p_len, pointer;
	uint16_t x, y, w, h;
	uint16_t simage, eimage, nimage;
	uint16_t s_num, e_num, d_num;
	uint8_t buff1[12] = {0};
	uint8_t Display_Mode = 0;
	uint8_t on_gif = 0;
	if (type == 0)		// Instruction information source is configuration file
	{
		Display_Mode = (buff[4] & 0x80) >> 7;
		p_add = (buff[5] + (buff[6] << 8));
		x = buff[7] + (buff[8] << 8);
		y = buff[9] + (buff[10] << 8);
		simage = buff[11] + (buff[12] << 8);
		eimage = buff[13] + (buff[14] << 8);
		s_num = buff[15] + (buff[16] << 8);
		e_num = buff[17] + (buff[18] << 8);
	}
	else if (type == 1)	// Instruction information ource is variable space
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		Display_Mode = (var[pointer]& 0x80) >> 7;
		p_add = (var[pointer + 1] + (var[pointer + 2] << 8));
		x = var[pointer + 3] + (var[pointer + 4] << 8);
		y = var[pointer + 5] + (var[pointer + 6] << 8);
		simage = var[pointer + 7] + (var[pointer + 8] << 8);
		eimage = var[pointer + 9] + (var[pointer + 10] << 8);
		s_num = var[pointer + 11] + (var[pointer + 12] << 8);
		e_num = var[pointer + 13] + (var[pointer + 14] << 8);

	}

	if (Pop_up_flag == 1) // Popup offset
	{
		x += Pop_up_x;
		y += Pop_up_y;
	}

#if !encoder_on
	nimage = (var[p_add * 2] << 8) + var[p_add * 2 + 1];
	if (nimage >= s_num && nimage <= e_num)
	{
		d_num = simage + nimage - s_num;
		Display_Icon(d_num, x, y, Display_Mode);
	}
	else
	{
		Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * simage, 12);
		pic_w = buff1[4] + (buff1[5] << 8);
		pic_h = buff1[6] + (buff1[7] << 8);
		for (uint8_t i = 0; i < Gif_num; i++)
		{
			if (x >= gGif_Info[i].x && x <= gGif_Info[i].x + gGif_Info[i].w &&
				y >= gGif_Info[i].y && y <= gGif_Info[i].y + gGif_Info[i].h && gGif_Info[i].on_gif == 1)
			{
				on_gif = 1;
				break;
			}
		}
		if (on_gif == 0)
		{
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x, y,
								  MAIN_BUFF, LCD_XSIZE_TFT, x, y,
								  Display_Layer, LCD_XSIZE_TFT, x, y,
								  0x0c, pic_w, pic_h);
		}
		else if (on_gif == 1)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF6, LCD_XSIZE_TFT, x, y,
								  LAY_BUFF6, LCD_XSIZE_TFT, x, y,
								  Display_Layer, LCD_XSIZE_TFT, x, y,
								  0x0c, pic_w, pic_h);
			
			Canvas_Image_Start_address(LAY_BUFF5);
			Canvas_image_width(LCD_XSIZE_TFT);
			LT768_DrawSquare_Fill(x, y, x + pic_w, y + pic_h, Black);
		}
	}
#endif
#if encoder_on
	if ((p_add != Check_box_add && Check_box_flag == 0) || (Check_box_flag == 1 && p_add == Check_box_add))
	{
		
		nimage = (var[p_add * 2] << 8) + var[p_add * 2 + 1];
		
		if (nimage >= s_num && nimage <= e_num)
		{
			d_num = simage + nimage - s_num;
			Display_Icon(d_num, x, y, Display_Mode);
		}
		else
		{
			Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * simage, 12);
			pic_w = buff1[4] + (buff1[5] << 8);
			pic_h = buff1[6] + (buff1[7] << 8);

			if (gEncoderFlag == 1 && Check_box_flag == 1)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF8, LCD_XSIZE_TFT, x, y,
									  LAY_BUFF8, LCD_XSIZE_TFT, x, y,
									  Display_Layer, LCD_XSIZE_TFT, x, y,
									  0x0c, pic_w, pic_h);
			}
			else
			{	
				/*test*/
				for (uint8_t i = 0; i < Gif_num; i++)
				{
					if (x >= gGif_Info[i].x && x <= gGif_Info[i].x + gGif_Info[i].w &&
						y >= gGif_Info[i].y && y <= gGif_Info[i].y + gGif_Info[i].h && gGif_Info[i].on_gif == 1)
					{
						on_gif = 1;
						break;
					}
				}
				if (on_gif == 0)
				{
					LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x, y,
										  MAIN_BUFF, LCD_XSIZE_TFT, x, y,
										  Display_Layer, LCD_XSIZE_TFT, x, y,
										  0x0c, pic_w, pic_h);
				}
				else if (on_gif == 1)
				{
					LT768_BTE_Memory_Copy(LAY_BUFF6, LCD_XSIZE_TFT, x, y,
										  LAY_BUFF6, LCD_XSIZE_TFT, x, y,
										  Display_Layer, LCD_XSIZE_TFT, x, y,
										  0x0c, pic_w, pic_h);
					
					Canvas_Image_Start_address(LAY_BUFF5);
					Canvas_image_width(LCD_XSIZE_TFT);
					LT768_DrawSquare_Fill(x, y, x + pic_w, y + pic_h, Black);
				}
			}
			if (gEncoderFlag == 1 && Check_box_flag == 0)
			{
				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x, y,
									  MAIN_BUFF, LCD_XSIZE_TFT, x, y,
									  LAY_BUFF8, LCD_XSIZE_TFT, x, y,
									  0x0c, pic_w, pic_h);
			}
		}
	}
	Check_box_flag = 0;
#endif
}
/**************************************************************/

/***********************************************************************************
* Function Name	: pic_date_display
* Description	: Display data in picture mode.
* Input			: - type: data info sources
				  - buff: data info
* Output		: None
* Return		: None
************************************************************************************/

void pic_date_display(uint8_t type, uint8_t *buff)
{
	uint16_t p_add, p_len, pointer;
	uint16_t x, y, xd, w, x1;
	uint16_t simage, eimage;
	uint8_t data_type, I_num, D_num, Alig, i = 0, num = 0;
	uint8_t flag = 0, nflag = 0;
	uint16_t num_w, dot_w, neg_w, h, canvas_numw, canvas_dotw, canvas_negw;
	int8_t value0 = 0;
	int16_t value1 = 0;
	int32_t value2 = 0;
	uint32_t value3 = 0;
	int64_t value = 0;

	uint16_t x_src = 0, y_src = 0, x_des = 0, y_des = 0, w_real = 0, h_real = 0;
	uint16_t x_max = 0, y_max = 0, w_max = 0, h_max = 0, w0 = 0, h0 = 0;
	uint32_t Lay_Addr = 0;

	uint8_t buff1[12] = {0};
	uint16_t canvas_w = 0;
	
	uint8_t on_gif = 0;
	uint8_t leading_zero = 0;

	if (type == 0)		// Instruction information source is configuration file
	{
		p_add = (buff[5] + (buff[6] << 8)) * 2;
		data_type = buff[7];
		x = buff[8] + (buff[9] << 8);
		y = buff[10] + (buff[11] << 8);
		I_num = buff[12];
		D_num = buff[13];
		Alig = buff[14] & 0x7F;
		leading_zero = (buff[14] & 0x80) >> 7;
		simage = buff[15] + (buff[16] << 8);
		eimage = buff[17] + (buff[18] << 8);
	}
	else if (type == 1) // Instruction information ource is variable space
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		p_add = (var[pointer + 1] + (var[pointer + 2] << 8)) * 2;
		data_type = var[pointer + 3];
		x = var[pointer + 4] + (var[pointer + 5] << 8);
		y = var[pointer + 6] + (var[pointer + 7] << 8);
		I_num = var[pointer + 8];
		D_num = var[pointer + 9];
		Alig = var[pointer + 10] & 0x7F;
		leading_zero = (var[pointer + 10] & 0x80) >> 7;
		simage = var[pointer + 11] + (var[pointer + 12] << 8);
		eimage = var[pointer + 13] + (var[pointer + 14] << 8);
	}

	if (Pop_up_flag == 1) // Popup offset
	{
		x += Pop_up_x;
		y += Pop_up_y;
	}
	if (data_type == 0x00)
	{
		value3 = var[p_add + 1];
		value = value3;
	}
	else if (data_type == 0x01)
	{
		value3 = (var[p_add] << 8) + var[p_add + 1];
		value = value3;
	}
	else if (data_type == 0x02)
	{
		value3 = (0xFF000000 & (var[p_add] << 24)) | (var[p_add + 1] << 16) | (var[p_add + 2] << 8) | var[p_add + 3];
		value = value3;
	}
	else if (data_type == 0x04)
	{
		value3 = var[p_add];
		value = value3;
	}
	else if (data_type == 0x80)
	{
		value0 = var[p_add + 1];
		value = value0;
	}
	else if (data_type == 0x81)
	{
		value1 = (var[p_add] << 8) + var[p_add + 1];
		value = value1;
	}
	else if (data_type == 0x82)
	{
		value2 = (var[p_add] << 24) | (var[p_add + 1] << 16) | (var[p_add + 2] << 8) | var[p_add + 3];
		value = value2;
	}
	else if (data_type == 0x83)
	{
		value = (0xFF000000 & (var[p_add] << 24)) + (var[p_add + 1] << 16) + (var[p_add + 2] << 8) + var[p_add + 3];
		value = ((value << 24) << 8) + (0xFF000000 & (var[p_add + 4] << 24)) + (var[p_add + 5] << 16) + (var[p_add + 6] << 8) + var[p_add + 7];
	}
	else if (data_type == 0x84)
	{
		value0 = var[p_add];
		value = value0;
	}

	memset(Display_buff, 0, 50);

	if (value < 0)
	{
		Display_buff[num] = 11; // negative number
		value = value * (-1);	// Become an integer
		nflag = 1;
		num++;
	}

	if (value < 1000000000000000000)
		value = value % ((int64_t)pow(10, I_num + D_num));

	for (i = 0; i < I_num; i++) // Integer part 
	{
		Display_buff[num] = value / (int64_t)pow(10, I_num - i - 1 + D_num);
		if (leading_zero == 0)
		{
			if (Display_buff[num] != 0 || flag == 1)
			{
				num++;
				flag = 1; // The first digit is not 0
			}
		}
		else
		{
			num++;
			flag = 1; // The first digit is not 0
		}
		value = value % (int64_t)pow(10, I_num - i - 1 + D_num);
	}

	if (flag == 0)
	{
		Display_buff[num] = 0;
		num++;
	}
	if (D_num > 0)
	{
		Display_buff[num] = 10; //"."
		num++;
	}

	for (i = 0; i < D_num; i++) // Fractional part
	{
		Display_buff[num] = value / (int32_t)pow(10, D_num - i - 1);
		num++;
		value = value % (int64_t)pow(10, D_num - i - 1);
	}

	Canvas_Image_Start_address(LAY_BUFF2);
	Canvas_image_width(LCD_XSIZE_TFT);

	xd = 0;
	w = 0;
	flag = 0;
	for (i = 0; i < num; i++) // First in LAY_ BUFF2 cache-related numbers or symbols
	{
		if (simage + Display_buff[i] >= addr_index[11])
			continue; // Filter out the case that the current ID number is greater than the total number of ICON

		Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * (simage + Display_buff[i]), 12);
		pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
		num_w = buff1[4] + (buff1[5] << 8);
		h = buff1[6] + (buff1[7] << 8);
		pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
		pic_flag = buff1[11];

		if (pic_len != 0 && pic_flag != 0xFF)
		{
			if (Screen_DIR == 0)
			{
				x_src = xd;
				y_src = 0;
				w_real = num_w;
				if (i == 0)
				{
					flag = pic_flag;
					h0 = h;
				}
			}
			else if (Screen_DIR == 1)
			{
				x_src = LCD_XSIZE_TFT - num_w;
				y_src = xd;
				w_real = h;
				if (i == 0)
				{
					flag = pic_flag;
					w0 = num_w;
				}
			}
			else if (Screen_DIR == 2)
			{
				x_src = LCD_XSIZE_TFT - num_w - xd;
				y_src = LCD_YSIZE_TFT - h;
				w_real = num_w;
				if (i == 0)
				{
					flag = pic_flag;
					h0 = h;
				}
			}
			else if (Screen_DIR == 3)
			{
				x_src = 0;
				y_src = LCD_YSIZE_TFT - h - xd;
				w_real = h;
				if (i == 0)
				{
					flag = pic_flag;
					w0 = num_w;
				}
			}

			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, x_src, y_src,
												num_w, h,
												num_w, pic_add,
												LAY_BUFF2, LCD_XSIZE_TFT);

			xd += w_real;
		}
	}

	/* When the length of the last displayed content is longer,
	 you need to copy MAIN_ BUFF to the cache area, otherwise it will be directly superimposed*/
	if (pdata_D_w[pdata_D_num] > xd)
		Lay_Addr = LAY_BUFF1;
	else
		Lay_Addr = Display_Layer;

	if (Screen_DIR == 0)
	{
		x_src = 0;
		y_src = 0;
		w_real = xd;
		h_real = h0;
		w_max = pdata_D_w[pdata_D_num];
		h_max = h0;
		if (Alig == 0)		// Left alignment
		{
			x_des = x;
			y_des = y;
			x_max = x;
			y_max = y;
		}
		else if (Alig == 1) // Mid alignment
		{
			x_des = x - w_real / 2 + 1;
			y_des = y;
			x_max = x - w_max / 2 + 1;
			y_max = y;
		}
		else if (Alig == 2) // Right alignment
		{
			x_des = x - w_real + 1;
			y_des = y;
			x_max = x - w_max + 1;
			y_max = y;
		}
	}
	else if (Screen_DIR == 1)
	{
		x_src = LCD_XSIZE_TFT - w0;
		y_src = 0;
		w_real = w0;
		h_real = xd;
		w_max = w0;
		h_max = pdata_D_w[pdata_D_num];
		if (Alig == 0) 		// Left alignment
		{
			x_des = x - w_real + 1;
			y_des = y;
			x_max = x - w_max + 1;
			y_max = y;
		}
		else if (Alig == 1) // Mid alignment
		{
			x_des = x - w_real + 1;
			y_des = y - h_real / 2 + 1;
			x_max = x - w_max + 1;
			y_max = y - h_max / 2 + 1;
		}
		else if (Alig == 2) // Right alignment
		{
			x_des = x - w_real + 1;
			y_des = y - h_real + 1;
			x_max = x - w_max + 1;
			y_max = y - h_max + 1;
		}
	}
	else if (Screen_DIR == 2)
	{
		x_src = LCD_XSIZE_TFT - xd;
		y_src = LCD_YSIZE_TFT - h0;
		w_real = xd;
		h_real = h0;
		w_max = pdata_D_w[pdata_D_num];
		h_max = h0;
		if (Alig == 0)		// Left alignment
		{
			x_des = x - w_real + 1;
			y_des = y - h_real + 1;
			x_max = x - w_max + 1;
			y_max = y - h_max + 1;
		}
		else if (Alig == 1) // Mid alignment
		{
			x_des = x - w_real / 2 + 1;
			y_des = y - h_real + 1;
			x_max = x - w_max / 2 + 1;
			y_max = y - h_max + 1;
		}
		else if (Alig == 2) // Right alignment
		{
			x_des = x;
			y_des = y - h_real + 1;
			x_max = x;
			y_max = y - h_max + 1;
		}
	}
	else if (Screen_DIR == 3)
	{
		x_src = 0;
		y_src = LCD_YSIZE_TFT - xd;
		w_real = w0;
		h_real = xd;
		w_max = w0;
		h_max = pdata_D_w[pdata_D_num];
		if (Alig == 0) 		// Left alignment
		{
			x_des = x;
			y_des = y - h_real + 1;
			x_max = x;
			y_max = y - h_max + 1;
		}
		else if (Alig == 1) // Mid alignment
		{
			x_des = x;
			y_des = y - h_real / 2 + 1;
			x_max = x;
			y_max = y - h_max / 2 + 1;
		}
		else if (Alig == 2) // Right alignment
		{
			x_des = x;
			y_des = y;
			x_max = x;
			y_max = y;
		}
	}
	
	/*test*/
	for (i = 0; i < Gif_num; i++)
	{
		if (x >= gGif_Info[i].x && x <= gGif_Info[i].x + gGif_Info[i].w &&
			y >= gGif_Info[i].y && y <= gGif_Info[i].y + gGif_Info[i].h && gGif_Info[i].on_gif == 1)
		{
			on_gif = 1;
			break;
		}
	}
	
	if (pdata_D_w[pdata_D_num] > xd || flag == 1)
	{
		if (on_gif == 0)
		{
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x_max, y_max,
								  MAIN_BUFF, LCD_XSIZE_TFT, x_max, y_max,
								  LAY_BUFF1, LCD_XSIZE_TFT, x_max, y_max,
								  0x0c, w_max, h_max);
		}
		else if (on_gif == 1)
		{
			Canvas_Image_Start_address(LAY_BUFF5);
			Canvas_image_width(LCD_XSIZE_TFT);
			LT768_DrawSquare_Fill(x_max, y_max, x_max + w_max, y_max + h_max,Black);
			
			LT768_BTE_Memory_Copy(LAY_BUFF6, LCD_XSIZE_TFT, x_max, y_max,
								  LAY_BUFF6, LCD_XSIZE_TFT, x_max, y_max,
								  LAY_BUFF1, LCD_XSIZE_TFT, x_max, y_max,
								  0x0c, w_max, h_max);
		}
	}

	if (flag == 0)
	{
		LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
							  LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
							  Lay_Addr, LCD_XSIZE_TFT, x_des, y_des,
							  0x0c, w_real, h_real);
	}
	else if (flag == 1)
	{
		LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
										 LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
										 Black, w_real, h_real);
	}
	else if (flag == 2)
	{
		if (on_gif == 0)
		{
			BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
										   LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
										   Lay_Addr, LCD_XSIZE_TFT, x_des, y_des,
										   w_real, h_real);
		}
		else if (on_gif == 1)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
								  LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
								  LAY_BUFF5, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_real, h_real);
			
			BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF6, LCD_XSIZE_TFT, x_des, y_des,
										   LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
										   Lay_Addr, LCD_XSIZE_TFT, x_des, y_des,
										   w_real, h_real);
		}
	}

	/* The main needs to be copied only when the last displayed content is longer_ BUFF to the buffer area,
	otherwise it will be displayed directly */
	if (pdata_D_w[pdata_D_num] > xd || flag == 1)
	{
		LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, x_max, y_max,
							  LAY_BUFF1, LCD_XSIZE_TFT, x_max, y_max,
							  Display_Layer, LCD_XSIZE_TFT, x_max, y_max,
							  0x0c, w_max, h_max);
	}
	pdata_D_w[pdata_D_num] = xd; // Record the pixel length occupied by each display
}
/****************************************************/

uint8_t Gif_num = 0;
uint8_t Gif_flag = 0;
Gif_Info gGif_Info[Gif_Size];
/***********************************************************************************
* Function Name	: gif_display
* Description	: Show picture of gif.
* Input			: - type: gif info sources
				  - buff: gif info
* Output		: None
* Return		: None
************************************************************************************/

void gif_display(uint8_t type, uint8_t *buff)
{
	uint8_t rbuff[16], i;
	uint8_t reset_En, mode, t_d;
	uint16_t p_add, p_len, pointer;
	uint16_t x, y, w, h;
	uint16_t V_stop, V_start, gif_id, V_once;
	uint16_t canvas_w;
	uint32_t pic_addr;
	uint8_t on_gif = 0;
	if (type == 0) // Instruction information source is configuration file
	{
		on_gif = (buff[4] & 0x80) >> 7;
		p_add = buff[5] + (buff[6] << 8);
		x = buff[7] + (buff[8] << 8);
		y = buff[9] + (buff[10] << 8);
		w = buff[11] + (buff[12] << 8);
		h = buff[13] + (buff[14] << 8);
		gif_id = buff[15] + (buff[16] << 8);
		reset_En = buff[17];
		t_d = buff[18];
		V_start = buff[19] + (buff[20] << 8);
		V_stop = buff[21] + (buff[22] << 8);
		mode = buff[23];
		V_once = buff[24] + (buff[25] << 8);
		for (i = 0; i < 32; i += 4)
		{
			gGif_Info[Gif_num].var_addr[i / 4] = buff[26 + i] + (buff[26 + i + 1] << 8);
			gGif_Info[Gif_num].var[i / 4] = buff[26 + i + 2] + (buff[26 + i + 3] << 8);
		}
	}
	else if (type == 1) // Instruction information ource is variable space
	{
		pointer = buff[2] + (buff[3] << 8) * 2;
		on_gif = (var[pointer] & 0x80) >> 7;
		p_add = var[pointer + 1] + (var[pointer + 2] << 8);
		x = var[pointer + 3] + (var[pointer + 4] << 8);
		y = var[pointer + 5] + (var[pointer + 6] << 8);
		w = var[pointer + 7] + (var[pointer + 8] << 8);
		h = var[pointer + 9] + (var[pointer + 10] << 8);
		gif_id = var[pointer + 11] + (var[pointer + 12] << 8);
		reset_En = var[pointer + 13];
		t_d = var[pointer + 14];
		V_start = var[pointer + 15] + (var[pointer + 16] << 8);
		V_stop = var[pointer + 17] + (var[pointer + 18] << 8);
		mode = var[pointer + 19];
		V_once = var[pointer + 20] + (var[pointer + 21] << 8);
		for (i = 0; i < 32; i += 4)
		{
			gGif_Info[Gif_num].var_addr[i / 4] = var[pointer + 22 + i] + (var[pointer + 22 + i + 1] << 8);
			gGif_Info[Gif_num].var[i / 4] = var[pointer + 22 + i + 2] + (var[pointer + 22 + i + 3] << 8);
		}
	}

	if (Pop_up_flag == 1) // Popup offset
	{
		x += Pop_up_x;
		y += Pop_up_y;
	}

	gGif_Info[Gif_num].p_add = p_add;
	gGif_Info[Gif_num].x = x;
	gGif_Info[Gif_num].y = y;
	gGif_Info[Gif_num].w = w;
	gGif_Info[Gif_num].h = h;
	gGif_Info[Gif_num].gif_id = gif_id;
	gGif_Info[Gif_num].reset_En = reset_En;
	gGif_Info[Gif_num].t_d = t_d;
	gGif_Info[Gif_num].V_stop = V_stop;
	gGif_Info[Gif_num].V_start = V_start;
	gGif_Info[Gif_num].V_once = V_once;
	gGif_Info[Gif_num].mode = mode;
	gGif_Info[Gif_num].p_c = 0;
	gGif_Info[Gif_num].flag = 1;
	Flash_Read_UI(rbuff, addr_index[8] + gif_id * 8, 8);
	gGif_Info[Gif_num].addr = rbuff[0] + (rbuff[1] << 8) + (rbuff[2] << 16) + (rbuff[3] << 24);
	gGif_Info[Gif_num].total = rbuff[4] + (rbuff[5] << 8) + (rbuff[6] << 16);

	if(on_gif == 1)
	{
		gGif_Info[Gif_num].on_gif = 1;
		Canvas_Image_Start_address(LAY_BUFF5);
		Canvas_image_width(LCD_XSIZE_TFT);
		LT768_DrawSquare_Fill(gGif_Info[Gif_num].x, gGif_Info[Gif_num].y, gGif_Info[Gif_num].x + gGif_Info[Gif_num].w - 1, gGif_Info[Gif_num].y + gGif_Info[Gif_num].h - 1,Black);
		
		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[Gif_num].x, gGif_Info[Gif_num].y,
							  MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[Gif_num].x, gGif_Info[Gif_num].y,
							  LAY_BUFF6, LCD_XSIZE_TFT, gGif_Info[Gif_num].x, gGif_Info[Gif_num].y,
							  0x0c, gGif_Info[Gif_num].w, gGif_Info[Gif_num].h);
		
	
	}
	else gGif_Info[Gif_num].on_gif = 0;
	
	/*Display the first frame when stopping*/
	if (((var[gGif_Info[Gif_num].p_add * 2] << 8) + var[gGif_Info[Gif_num].p_add * 2 + 1]) == gGif_Info[Gif_num].V_stop)
	{
		Flash_Read_UI(rbuff, gGif_Info[Gif_num].addr, 16);
		pic_addr = rbuff[0] + (rbuff[1] << 8) + (rbuff[2] << 16) + (rbuff[3] << 24);
		pic_flag = rbuff[15];

		canvas_w = Get_Canvas_Width(gGif_Info[Gif_num].w);
		Canvas_Image_Start_address(LAY_BUFF1);
		Canvas_image_width(canvas_w);

		LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
											gGif_Info[Gif_num].w, gGif_Info[Gif_num].h,
											gGif_Info[Gif_num].w, pic_addr, LAY_BUFF1, canvas_w);

		if (pic_flag == 0)
		{
			if (gGif_Info[Gif_num].on_gif == 1)
			{
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF5, LCD_XSIZE_TFT, gGif_Info[Gif_num].x, gGif_Info[Gif_num].y,
											   LAY_BUFF1, canvas_w, 0, 0,
											gGif_Info[Gif_num].w, gGif_Info[Gif_num].h);
			}
			
			LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
								  LAY_BUFF1, canvas_w, 0, 0,
								  Display_Layer, LCD_XSIZE_TFT, gGif_Info[Gif_num].x, gGif_Info[Gif_num].y,
								  0x0c, gGif_Info[Gif_num].w, gGif_Info[Gif_num].h);
		}
		else if (pic_flag == 1)
		{
			if (gGif_Info[Gif_num].on_gif == 1)
			{
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF5, LCD_XSIZE_TFT, gGif_Info[Gif_num].x, gGif_Info[Gif_num].y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   gGif_Info[Gif_num].w, gGif_Info[Gif_num].h);
			}
			
			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
											 Display_Layer, LCD_XSIZE_TFT, gGif_Info[Gif_num].x, gGif_Info[Gif_num].y,
											 Black, gGif_Info[Gif_num].w, gGif_Info[Gif_num].h);
		}
		else if (pic_flag == 2)
		{
			if (gGif_Info[Gif_num].on_gif == 1)
			{
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF5, LCD_XSIZE_TFT, gGif_Info[Gif_num].x, gGif_Info[Gif_num].y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   gGif_Info[Gif_num].w, gGif_Info[Gif_num].h);
			}
			
			BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[Gif_num].x, gGif_Info[Gif_num].y,
										   LAY_BUFF1, canvas_w, 0, 0,
										   Display_Layer, LCD_XSIZE_TFT, gGif_Info[Gif_num].x, gGif_Info[Gif_num].y,
										   gGif_Info[Gif_num].w, gGif_Info[Gif_num].h);
		}
	}

	LT_ShowGifPicture(Gif_num);
	Gif_num++;
}

uint8_t LT_ShowGifPicture(uint8_t num)
{
	uint8_t rbuff[16], i;
	uint16_t canvas_w;
	uint16_t page_flag = 0;
	uint32_t pic_addr;

	if (((var[gGif_Info[num].p_add * 2] << 8) + var[gGif_Info[num].p_add * 2 + 1]) == gGif_Info[num].V_start)
	{
		gGif_Info[num].flag = 1;
		if (gGif_Info[num].p_c >= gGif_Info[num].total)
			gGif_Info[num].p_c = 0;

		Flash_Read_UI(rbuff, gGif_Info[num].addr + gGif_Info[num].p_c * 16, 16);

		pic_addr = rbuff[0] + (rbuff[1] << 8) + (rbuff[2] << 16) + (rbuff[3] << 24);
		pic_flag = rbuff[15];

		if (DioKb_flag == 1) // In the keyboard input interface
		{
			if (!(gGif_Info[num].x > gDio_Info[Dio_num].Xe || (gGif_Info[num].x + gGif_Info[num].w) < gDio_Info[Dio_num].Xs || gGif_Info[num].y > gDio_Info[Dio_num].Ye || (gGif_Info[num].y + gGif_Info[num].h) < gDio_Info[Dio_num].Ys))
			{
				return 0;
			}
		}
		else if (AioKb_flag == 1) // In the keyboard input interface
		{
			if (!(gGif_Info[num].x > gAio_Info[Aio_num].Xe || (gGif_Info[num].x + gGif_Info[num].w) < gAio_Info[Aio_num].Xs || gGif_Info[num].y > gAio_Info[Aio_num].Ye || (gGif_Info[num].y + gGif_Info[num].h) < gAio_Info[Aio_num].Ys))
			{
				return 0;
			}
		}
		else if (GBKioKb_flag == 1) // In the keyboard input interface
		{
			if (!(gGif_Info[num].x > gGBKio_Info[GBKio_num].Xe || (gGif_Info[num].x + gGif_Info[num].w) < gGBKio_Info[GBKio_num].Xs || gGif_Info[num].y > gGBKio_Info[GBKio_num].Ye || (gGif_Info[num].y + gGif_Info[num].h) < gGBKio_Info[GBKio_num].Ys))
			{
				return 0;
			}
		}

		canvas_w = Get_Canvas_Width(gGif_Info[num].w);
		Canvas_Image_Start_address(LAY_BUFF1);
		Canvas_image_width(canvas_w);
	
		if (pic_flag == 0)
		{
			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												gGif_Info[num].w, gGif_Info[num].h,
												gGif_Info[num].w, pic_addr, LAY_BUFF1, canvas_w);
			
			if (gGif_Info[num].on_gif == 1)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF6, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
									  0x0c, gGif_Info[num].w, gGif_Info[num].h);
				
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF5, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   gGif_Info[num].w, gGif_Info[num].h);
			
			}
			
			LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
								  LAY_BUFF1, canvas_w, 0, 0,
								  Display_Layer, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
								  0x0c, gGif_Info[num].w, gGif_Info[num].h);
			

		}
		else if (pic_flag == 1)
		{
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
								  MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
								  LAY_BUFF2, canvas_w, 0, 0,
								  0x0c, gGif_Info[num].w, gGif_Info[num].h);

			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												gGif_Info[num].w, gGif_Info[num].h,
												gGif_Info[num].w, pic_addr, LAY_BUFF1, canvas_w);

			if (gGif_Info[num].on_gif == 1)
			{
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
												 LAY_BUFF2, canvas_w, 0, 0,
												 Black, gGif_Info[num].w, gGif_Info[num].h);
				LT768_BTE_Memory_Copy(LAY_BUFF2, canvas_w, 0, 0,
									  LAY_BUFF2, canvas_w, 0, 0,
									  LAY_BUFF6, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
									  0x0c, gGif_Info[num].w, gGif_Info[num].h);
				
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF2, canvas_w, 0, 0,
											   LAY_BUFF5, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
											   LAY_BUFF2, canvas_w, 0, 0,
											   gGif_Info[num].w, gGif_Info[num].h);
			}
			
			LT768_BTE_Memory_Copy(LAY_BUFF2, canvas_w, 0, 0,
								  LAY_BUFF2, canvas_w, 0, 0,
								  Display_Layer, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
								  0x0c, gGif_Info[num].w, gGif_Info[num].h);								 
											 
		}
		else if (pic_flag == 2)
		{
			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												gGif_Info[num].w, gGif_Info[num].h,
												gGif_Info[num].w, pic_addr, LAY_BUFF1, canvas_w);

			if (gGif_Info[num].on_gif == 1)
			{
				BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF6, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
											   gGif_Info[num].w, gGif_Info[num].h);
				
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF5, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   gGif_Info[num].w, gGif_Info[num].h);
			}
			
			BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
										   LAY_BUFF1, canvas_w, 0, 0,
										   Display_Layer, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
										   gGif_Info[num].w, gGif_Info[num].h);
		}
		gGif_Info[num].p_c++;
	}
	else if (((var[gGif_Info[num].p_add * 2] << 8) + var[gGif_Info[num].p_add * 2 + 1]) == gGif_Info[num].V_once)
	{
		gGif_Info[num].flag = 1;
		if (gGif_Info[num].p_c >= gGif_Info[num].total)
		{
			var[gGif_Info[num].p_add * 2] = (gGif_Info[num].V_stop >> 8) & 0xFF;
			var[gGif_Info[num].p_add * 2 + 1] = (gGif_Info[num].V_stop) & 0xFF;
			for (i = 0; i < 8; i++)
			{
				if (gGif_Info[num].var_addr[i] != 0xFFFF && gGif_Info[num].var_addr[i] < 0x7200)
				{
					var[gGif_Info[num].var_addr[i] * 2] = (gGif_Info[num].var[i] >> 8) & 0xFF;
					var[gGif_Info[num].var_addr[i] * 2 + 1] = gGif_Info[num].var[i] & 0xFF;

					if (gGif_Info[num].var_addr[i] > 0x7000)
						reg_operation(gGif_Info[num].var_addr[i]);
					else if (gGif_Info[num].var_addr[i] == 0x7000)
					{
						page_flag = 1;
					}
				}
			}
			
			if (page_flag == 1)
				reg_operation(0x7000);
			return 0;
		}

		Flash_Read_UI(rbuff, gGif_Info[num].addr + gGif_Info[num].p_c * 16, 16);
		pic_addr = rbuff[0] + (rbuff[1] << 8) + (rbuff[2] << 16) + (rbuff[3] << 24);
		pic_flag = rbuff[15];

		if (DioKb_flag == 1) // In the keyboard input interface
		{
			if (!(gGif_Info[num].x > gDio_Info[Dio_num].Xe || (gGif_Info[num].x + gGif_Info[num].w) < gDio_Info[Dio_num].Xs || gGif_Info[num].y > gDio_Info[Dio_num].Ye || (gGif_Info[num].y + gGif_Info[num].h) < gDio_Info[Dio_num].Ys))
			{
				return 0;
			}
		}
		else if (AioKb_flag == 1) // In the keyboard input interface
		{
			if (!(gGif_Info[num].x > gAio_Info[Aio_num].Xe || (gGif_Info[num].x + gGif_Info[num].w) < gAio_Info[Aio_num].Xs || gGif_Info[num].y > gAio_Info[Aio_num].Ye || (gGif_Info[num].y + gGif_Info[num].h) < gAio_Info[Aio_num].Ys))
			{
				return 0;
			}
		}
		else if (GBKioKb_flag == 1) // In the keyboard input interface
		{
			if (!(gGif_Info[num].x > gGBKio_Info[GBKio_num].Xe || (gGif_Info[num].x + gGif_Info[num].w) < gGBKio_Info[GBKio_num].Xs || gGif_Info[num].y > gGBKio_Info[GBKio_num].Ye || (gGif_Info[num].y + gGif_Info[num].h) < gGBKio_Info[GBKio_num].Ys))
			{
				return 0;
			}
		}

		canvas_w = Get_Canvas_Width(gGif_Info[num].w);
		Canvas_Image_Start_address(LAY_BUFF1);
		Canvas_image_width(canvas_w);

		if (pic_flag == 0)
		{
			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												gGif_Info[num].w, gGif_Info[num].h,
												gGif_Info[num].w, pic_addr, LAY_BUFF1, canvas_w);
			
			if (gGif_Info[num].on_gif == 1)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF6, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
									  0x0c, gGif_Info[num].w, gGif_Info[num].h);
				
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF5, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   gGif_Info[num].w, gGif_Info[num].h);
			}
				
			
			LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
								  LAY_BUFF1, canvas_w, 0, 0,
								  Display_Layer, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
								  0x0c, gGif_Info[num].w, gGif_Info[num].h);
		}
		else if (pic_flag == 1)
		{
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
								  MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
								  LAY_BUFF2, canvas_w, 0, 0,
								  0x0c, gGif_Info[num].w, gGif_Info[num].h);

			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												gGif_Info[num].w, gGif_Info[num].h,
												gGif_Info[num].w, pic_addr, LAY_BUFF1, canvas_w);

			if (gGif_Info[num].on_gif == 1)
			{
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
												 LAY_BUFF2, canvas_w, 0, 0,
												 Black, gGif_Info[num].w, gGif_Info[num].h);
				LT768_BTE_Memory_Copy(LAY_BUFF2, canvas_w, 0, 0,
									  LAY_BUFF2, canvas_w, 0, 0,
									  LAY_BUFF6, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
									  0x0c, gGif_Info[num].w, gGif_Info[num].h);
				
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF2, canvas_w, 0, 0,
											   LAY_BUFF5, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
											   LAY_BUFF2, canvas_w, 0, 0,
										   gGif_Info[num].w, gGif_Info[num].h);
			}
			
			LT768_BTE_Memory_Copy(LAY_BUFF2, canvas_w, 0, 0,
								  LAY_BUFF2, canvas_w, 0, 0,
								  Display_Layer, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
								  0x0c, gGif_Info[num].w, gGif_Info[num].h);								 
											 
		}
		else if (pic_flag == 2)
		{
			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												gGif_Info[num].w, gGif_Info[num].h,
												gGif_Info[num].w, pic_addr, LAY_BUFF1, canvas_w);

			if (gGif_Info[num].on_gif == 1)
			{
				BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF6, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
											   gGif_Info[num].w, gGif_Info[num].h);
				
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF5, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   gGif_Info[num].w, gGif_Info[num].h);
			}
			
			BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
										   LAY_BUFF1, canvas_w, 0, 0,
										   Display_Layer, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
										   gGif_Info[num].w, gGif_Info[num].h);
		}
		gGif_Info[num].p_c++;
	}
	else if (((var[gGif_Info[num].p_add * 2] << 8) + var[gGif_Info[num].p_add * 2 + 1]) == gGif_Info[num].V_stop)
	{
		gGif_Info[num].flag = 1;
		if (gGif_Info[num].reset_En == 1)
			gGif_Info[num].p_c = 0; // Equal to 1 reset
	}
	else if (((var[gGif_Info[num].p_add * 2] << 8) + var[gGif_Info[num].p_add * 2 + 1]) == 0xFFFF)
	{
		if (DioKb_flag == 1) // In the keyboard input interface
		{
			if (!(gGif_Info[num].x > gDio_Info[Dio_num].Xe || (gGif_Info[num].x + gGif_Info[num].w) < gDio_Info[Dio_num].Xs || gGif_Info[num].y > gDio_Info[Dio_num].Ye || (gGif_Info[num].y + gGif_Info[num].h) < gDio_Info[Dio_num].Ys))
			{
				return 0;
			}
		}
		else if (AioKb_flag == 1) // In the keyboard input interface
		{
			if (!(gGif_Info[num].x > gAio_Info[Aio_num].Xe || (gGif_Info[num].x + gGif_Info[num].w) < gAio_Info[Aio_num].Xs || gGif_Info[num].y > gAio_Info[Aio_num].Ye || (gGif_Info[num].y + gGif_Info[num].h) < gAio_Info[Aio_num].Ys))
			{
				return 0;
			}
		}
		else if (GBKioKb_flag == 1) // In the keyboard input interface
		{
			if (!(gGif_Info[num].x > gGBKio_Info[GBKio_num].Xe || (gGif_Info[num].x + gGif_Info[num].w) < gGBKio_Info[GBKio_num].Xs || gGif_Info[num].y > gGBKio_Info[GBKio_num].Ye || (gGif_Info[num].y + gGif_Info[num].h) < gGBKio_Info[GBKio_num].Ys))
			{
				return 0;
			}
		}
		if (gGif_Info[num].flag != 0)
		{
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
								  MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
								  LAY_BUFF1, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
								  0x0c, gGif_Info[num].w, gGif_Info[num].h);
			
			if (gGif_Info[num].on_gif == 1)
			{
				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
									  MAIN_BUFF, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
									  LAY_BUFF6, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
									  0x0c, gGif_Info[num].w, gGif_Info[num].h);
				
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
											   LAY_BUFF5, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
											   LAY_BUFF1, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
											   gGif_Info[num].w, gGif_Info[num].h);
			}
			
			LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
								  LAY_BUFF1, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
								  Display_Layer, LCD_XSIZE_TFT, gGif_Info[num].x, gGif_Info[num].y,
								  0x0c, gGif_Info[num].w, gGif_Info[num].h);
			
			gGif_Info[num].flag = 0;
		}
		gGif_Info[num].p_c = 0;
	}
}

void show_gif(void)
{
	if (Gif_num > 0 && Gif_flag == 1)
	{
		Gif_flag = 0;
		for (uint8_t i = 0; i < Gif_num; i++)
			gGif_Info[i].t_c++;
		for (uint8_t i = 0; i < Gif_num; i++)
		{
			if (gGif_Info[i].t_c >= gGif_Info[i].t_d)
			{
				LT_ShowGifPicture(i); // When a window timing time expires, switch pictures
				gGif_Info[i].t_c = 0;
			}
		}
	}
}
/*****************************************/

/***********************************************************************************
* Function Name	: qr_code
* Description	: Show QRcode.
* Input			: - type: QRcode info sources
				  - buff: QRcode info
* Output		: None
* Return		: None
************************************************************************************/
void qr_code(uint8_t type, uint8_t *buff)
{
	uint8_t dxy = 50;
	uint8_t unit_pixle;
	int8_t buf[10];
	uint16_t p_add, pointer, length;
	uint16_t i = 0, j = 0;
	uint16_t x1 = 0, y1 = 0, x, y;
	int32_t size = 50;
	if (type == 0)		// Instruction information source is configuration file
	{
		p_add = (buff[5] + (buff[6] << 8)) * 2;
		length = buff[7] + (buff[8] << 8);
		x = buff[9] + (buff[10] << 8);
		y = buff[11] + (buff[12] << 8);
		unit_pixle = buff[13];
	}
	else if (type == 1) // Instruction information ource is variable space
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		p_add = (var[pointer + 1] + (var[pointer + 2] << 8)) * 2;
		length = var[pointer + 3] + (var[pointer + 4] << 8);
		x = var[pointer + 5] + (var[pointer + 6] << 8);
		y = var[pointer + 7] + (var[pointer + 8] << 8);
		unit_pixle = var[pointer + 9];
	}
	if (Pop_up_flag == 1) // Popup offset
	{
		x += Pop_up_x;
		y += Pop_up_y;
	}
	EncodeData(&var[p_add]);

	size = size * unit_pixle / m_nSymbleSize;
	dxy = ((dxy * unit_pixle) % m_nSymbleSize) / 2;
	y1 = dxy;

	Canvas_Image_Start_address(LAY_BUFF1);
	Canvas_image_width(LCD_XSIZE_TFT);
	LT768_DrawSquare_Fill(0, 0, 50 * unit_pixle - 1, 50 * unit_pixle - 1, White);

	if (Screen_DIR == 0)
	{
		for (i = 0; i < m_nSymbleSize; i++)
		{
			x1 = dxy;
			for (j = 0; j < m_nSymbleSize; j++)
			{
				if (m_byModuleData[i][j])
				{
					LT768_DrawSquare_Fill(x1, y1, x1 + size, y1 + size, Black);
				}
				x1 += size;
			}
			y1 += size;
		}
	}
	else if (Screen_DIR == 1)
	{
		for (j = 0; j < m_nSymbleSize; j++)
		{
			x1 = dxy;
			for (i = 0; i < m_nSymbleSize; i++)
			{
				if (m_byModuleData[m_nSymbleSize - 1 - i][j])
				{
					LT768_DrawSquare_Fill(x1, y1, x1 + size, y1 + size, Black);
				}
				x1 += size;
			}
			y1 += size;
		}
	}
	else if (Screen_DIR == 2)
	{
		for (i = 0; i < m_nSymbleSize; i++)
		{
			x1 = dxy;
			for (j = 0; j < m_nSymbleSize; j++)
			{
				if (m_byModuleData[m_nSymbleSize - 1 - i][m_nSymbleSize - 1 - j])
				{
					LT768_DrawSquare_Fill(x1, y1, x1 + size, y1 + size, Black);
				}
				x1 += size;
			}
			y1 += size;
		}
	}
	else if (Screen_DIR == 3)
	{
		for (j = 0; j < m_nSymbleSize; j++)
		{
			x1 = dxy;
			for (i = 0; i < m_nSymbleSize; i++)
			{
				if (m_byModuleData[i][m_nSymbleSize - 1 - j])
				{
					LT768_DrawSquare_Fill(x1, y1, x1 + size, y1 + size, Black);
				}
				x1 += size;
			}
			y1 += size;
		}
	}
	LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
						  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
						  Display_Layer, LCD_XSIZE_TFT, x, y,
						  0x0c, 50 * unit_pixle, 50 * unit_pixle);
}

/**************************************/

#define RTC_Blank 20
RTC_Info gRTC_Info[RTC_Size];
uint8_t RTC_num = 0;
uint8_t RTC_flag = 0;

/***********************************************************************************
* Function Name	: Display_RTC
* Description	: Show digital clock.
* Input			: - gRTC: digital clock info sources
				  - status: digital clock status
* Output		: None
* Return		: None
************************************************************************************/
uint8_t Display_RTC(RTC_Info *gRTC, uint8_t status)
{
	uint8_t buff[12] = {0};
	uint8_t flag = 0;
	uint8_t mode = gRTC->mode;
	int8_t num = 0;
	uint16_t canvas_w = 0, i = 0, ID[11];
	uint16_t x_date = 0, x_time = 0, width = 0, high = 0;
	uint16_t id_start = gRTC->ID_start;
	uint16_t x = gRTC->x;
	uint16_t y = gRTC->y;
	uint16_t x_src, y_src, x_des, y_des, w_real, h_real, cnt;
	
	uint8_t state = 0;	//0: Aligned 1: Unaligned
	Canvas_Image_Start_address(LAY_BUFF1);
	Canvas_image_width(LCD_XSIZE_TFT);

	if (mode == 0 || mode == 8) // Year/month/day/hour/minute/second
	{
		/* Display date*/
		num = 0;
		if (calendar.w_year / 1000)
			num = 0; // The number of digits is different
		else
		{
			num--;
			if ((calendar.w_year / 100 % 10) == 0)
			{
				num--;
				if ((calendar.w_year / 10 % 10) == 0)
					num--;
			}
		}

		if (num >= 0)
			ID[num] = (calendar.w_year) / 1000;
		if (num + 1 >= 0)
			ID[num + 1] = (calendar.w_year) / 100 % 10;
		if (num + 2 >= 0)
			ID[num + 2] = (calendar.w_year) / 10 % 10;
		if (num + 3 >= 0)
			ID[num + 3] = (calendar.w_year) % 10;
		ID[num + 4] = 11;
		if (state == 0)
		{
			ID[num + 5] = (calendar.w_month) / 10 % 10;
		}
		else 
		{
			if (((calendar.w_month) / 10 % 10) == 0)
				num--;
			else
				ID[num + 5] = (calendar.w_month) / 10 % 10;
		}
		ID[num + 6] = (calendar.w_month) % 10;
		ID[num + 7] = 12;

		if (state == 0)
		{
			ID[num + 8] = (calendar.w_date) / 10 % 10;
		}
		else
		{
			if (((calendar.w_date) / 10 % 10) == 0)
				num--;
			else
				ID[num + 8] = (calendar.w_date) / 10 % 10;
		}

		ID[num + 9] = (calendar.w_date) % 10;
		if (mode == 0)
			cnt = num + 9;
		else
		{
			ID[num + 10] = 13;
			cnt = num + 10;
		}

		for (i = 0; i <= cnt; i++)
		{
			if (id_start + ID[i] >= addr_index[11])
				continue; // ICON is available

			Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * (id_start + ID[i]), 12);

			pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
			pic_w = buff[4] + (buff[5] << 8);
			pic_h = buff[6] + (buff[7] << 8);
			pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
			pic_flag = buff[11];

			if (pic_len != 0 && pic_flag != 0xFF) // ICON is not empty
			{
				if (Screen_DIR == 0)
				{
					// Record the height and flag of the first valid ICON, and the following ICON arrangement shall be subject to this
					if (i == 0)
					{
						high = pic_h;
						flag = pic_flag;
					}

					x_src = x_date;
					y_src = 0;
					w_real = pic_w;
				}
				else if (Screen_DIR == 1)
				{
					// Record the height and flag of the first valid ICON, and the following ICON arrangement shall be subject to this
					if (i == 0)
					{
						width = pic_w;
						flag = pic_flag;
					}

					x_src = LCD_XSIZE_TFT - pic_w;
					y_src = x_date;
					w_real = pic_h;
				}
				else if (Screen_DIR == 2)
				{
					// Record the height and flag of the first valid ICON, and the following ICON arrangement shall be subject to this
					if (i == 0)
					{
						high = pic_h;
						flag = pic_flag;
					}

					x_src = LCD_XSIZE_TFT - pic_w - x_date;
					y_src = LCD_YSIZE_TFT - pic_h;
					w_real = pic_w;
				}
				else if (Screen_DIR == 3)
				{
					// Record the height and flag of the first valid ICON, and the following ICON arrangement shall be subject to this
					if (i == 0)
					{
						width = pic_w;
						flag = pic_flag;
					}

					x_src = 0;
					y_src = LCD_YSIZE_TFT - pic_h - x_date;
					w_real = pic_h;
				}
				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, x_src, y_src,
													pic_w, pic_h,
													pic_w, pic_add, LAY_BUFF1, LCD_XSIZE_TFT);
				x_date += w_real;
			}
		}
		/*On the keyboard input page*/
		if (DioKb_flag == 1)
		{
			if (!(x > gDio_Info[Dio_num].Xe || (x + x_date) < gDio_Info[Dio_num].Xs || y > gDio_Info[Dio_num].Ye || (y + high) < gDio_Info[Dio_num].Ys))
			{
				return 0;
			}
		}
		else if (AioKb_flag == 1)
		{
			if (!(x > gAio_Info[Aio_num].Xe || (x + x_date) < gAio_Info[Aio_num].Xs || y > gAio_Info[Aio_num].Ye || (y + high) < gAio_Info[Aio_num].Ys))
			{
				return 0;
			}
		}
		else if (GBKioKb_flag == 1)
		{
			if (!(x > gGBKio_Info[GBKio_num].Xe || (x + x_date) < gGBKio_Info[GBKio_num].Xs || y > gGBKio_Info[GBKio_num].Ye || (y + high) < gGBKio_Info[GBKio_num].Ys))
			{
				return 0;
			}
		}

		if (Screen_DIR == 0)
		{
			x_src = 0;
			y_src = 0;
			x_des = x;
			y_des = y;
			w_real = x_date;
			h_real = high;
		}
		else if (Screen_DIR == 1)
		{
			x_src = LCD_XSIZE_TFT - width;
			y_src = 0;
			x_des = x - width + 1;
			y_des = y;
			w_real = width;
			h_real = x_date;
		}
		else if (Screen_DIR == 2)
		{
			x_src = LCD_XSIZE_TFT - x_date;
			y_src = LCD_YSIZE_TFT - high;
			x_des = x - x_date + 1;
			y_des = y - high + 1;
			w_real = x_date;
			h_real = high;
		}
		else if (Screen_DIR == 3)
		{
			x_src = 0;
			y_src = LCD_YSIZE_TFT - x_date;
			x_des = x;
			y_des = y - x_date + 1;
			w_real = width;
			h_real = x_date;
		}

		if (flag == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
								  LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
								  Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_real, h_real);
		}
		else if (flag == 1)
		{
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
								  MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
								  LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_real, h_real);

			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
											 LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
											 Black, w_real, h_real);

			LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
								  LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
								  Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_real, h_real);
		}
		else if (flag == 2)
		{
			BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
										   LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
										   Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
										   w_real, h_real);
		}

		if (status) // To remove the tail residue
		{
			if (Screen_DIR == 0)
			{
				x_des = x + x_date;
				y_des = y;
				w_real = RTC_Blank;
				h_real = high;
			}
			else if (Screen_DIR == 1)
			{
				x_des = x - width + 1;
				y_des = y + x_date;
				w_real = width;
				h_real = RTC_Blank;
			}
			else if (Screen_DIR == 2)
			{
				x_des = x - (x_date + RTC_Blank) + 1;
				y_des = y - high + 1;
				w_real = RTC_Blank;
				h_real = high;
			}
			else if (Screen_DIR == 3)
			{
				x_des = x;
				y_des = y - (x_date + RTC_Blank) + 1;
				w_real = width;
				h_real = RTC_Blank;
			}
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
								  MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
								  Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_real, h_real);
		}

		/* Display time*/
		num = 0;
		if(state == 0)
		{
			ID[num] = (calendar.hour) / 10 % 10;
		}
		else
		{
			if ((calendar.hour / 10 % 10) == 0)
				num--; // The number of digits is different
			else
				ID[num] = (calendar.hour) / 10 % 10;
		}
		ID[num + 1] = (calendar.hour) % 10;
		ID[num + 2] = 10;
		ID[num + 3] = (calendar.min) / 10 % 10;
		ID[num + 4] = (calendar.min) % 10;
		ID[num + 5] = 10;
		ID[num + 6] = (calendar.sec) / 10 % 10;
		ID[num + 7] = (calendar.sec) % 10;
		cnt = num + 7;

		if (Screen_DIR == 0)
			x = x + x_date + RTC_Blank; // Pixel interval between time and date
		else if (Screen_DIR == 1)
			y = y + x_date + RTC_Blank;
		else if (Screen_DIR == 2)
			x = x - x_date - RTC_Blank;
		else if (Screen_DIR == 3)
			y = y - x_date - RTC_Blank;

		for (i = 0; i <= cnt; i++)
		{
			if (id_start + ID[i] >= addr_index[11])
				continue; // ICON is available
			Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * (id_start + ID[i]), 12);
			pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
			pic_w = buff[4] + (buff[5] << 8);
			pic_h = buff[6] + (buff[7] << 8);
			pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
			pic_flag = buff[11];

			if (pic_len != 0 && pic_flag != 0xFF) // ICON is not empty
			{
				if (Screen_DIR == 0)
				{
					if (i == 0) // // Record the height and flag of the first valid ICON, and the following ICON arrangement shall be subject to this
					{
						high = pic_h;
						flag = pic_flag;
					}
					x_src = x_time;
					y_src = 0;
					w_real = pic_w;
				}
				else if (Screen_DIR == 1)
				{
					if (i == 0) // // Record the height and flag of the first valid ICON, and the following ICON arrangement shall be subject to this
					{
						width = pic_w;
						flag = pic_flag;
					}
					x_src = LCD_XSIZE_TFT - pic_w;
					y_src = x_time;
					w_real = pic_h;
				}
				else if (Screen_DIR == 2)
				{
					if (i == 0) // // Record the height and flag of the first valid ICON, and the following ICON arrangement shall be subject to this
					{
						high = pic_h;
						flag = pic_flag;
					}

					x_src = LCD_XSIZE_TFT - pic_w - x_time;
					y_src = LCD_YSIZE_TFT - pic_h;
					w_real = pic_w;
				}
				else if (Screen_DIR == 3)
				{
					if (i == 0) // // Record the height and flag of the first valid ICON, and the following ICON arrangement shall be subject to this
					{
						width = pic_w;
						flag = pic_flag;
					}
					x_src = 0;
					y_src = LCD_YSIZE_TFT - pic_h - x_time;
					w_real = pic_h;
				}
				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, x_src, y_src,
													pic_w, pic_h,
													pic_w, pic_add, LAY_BUFF1, LCD_XSIZE_TFT);
				x_time += w_real;
			}
		}

		if (Screen_DIR == 0)
		{
			x_src = 0;
			y_src = 0;
			x_des = x;
			y_des = y;
			w_real = x_time;
			h_real = high;
		}
		else if (Screen_DIR == 1)
		{
			x_src = LCD_XSIZE_TFT - width;
			y_src = 0;
			x_des = x - width + 1;
			y_des = y;
			w_real = width;
			h_real = x_time;
		}
		else if (Screen_DIR == 2)
		{
			x_src = LCD_XSIZE_TFT - x_time;
			y_src = LCD_YSIZE_TFT - high;
			x_des = x - x_time + 1;
			y_des = y - high + 1;
			w_real = x_time;
			h_real = high;
		}
		else if (Screen_DIR == 3)
		{
			x_src = 0;
			y_src = LCD_YSIZE_TFT - x_time;
			x_des = x;
			y_des = y - x_time + 1;
			w_real = width;
			h_real = x_time;
		}

		if (flag == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
								  LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
								  Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_real, h_real);
		}
		else if (flag == 1)
		{
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
								  MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
								  LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_real, h_real);

			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
											 LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
											 Black, w_real, h_real);

			LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
								  LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
								  Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_real, h_real);
		}
		else if (flag == 2)
		{
			BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
										   LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
										   Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
										   w_real, h_real);
		}

		if (gRTC->Len_last > (x_date + RTC_Blank + x_time) && status)
		{
			if (Screen_DIR == 0)
			{
				x_des = x + x_time;
				y_des = y;
				w_real = gRTC->Len_last - (x_date + RTC_Blank + x_time);
				h_real = high;
			}
			else if (Screen_DIR == 1)
			{
				x_des = x - width + 1;
				y_des = y + x_time;
				w_real = width;
				h_real = gRTC->Len_last - (x_date + RTC_Blank + x_time);
			}
			else if (Screen_DIR == 2)
			{
				x_des = x - (gRTC->Len_last - x_date - RTC_Blank) + 1;
				y_des = y - high + 1;
				w_real = gRTC->Len_last - (x_date + RTC_Blank + x_time);
				h_real = high;
			}
			else if (Screen_DIR == 3)
			{
				x_des = x;
				y_des = y - (gRTC->Len_last - x_date - RTC_Blank) + 1;
				w_real = width;
				h_real = gRTC->Len_last - (x_date + RTC_Blank + x_time);
			}
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
								  MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
								  Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_real, h_real);
		}
		gRTC->Len_last = x_date + RTC_Blank + x_time; // Record the time sequence length of the last second
	}

	else if ((mode >= 1 && mode <= 6) || (mode >= 9 && mode <= 11))
	{
		if (mode == 1 || mode == 9)
		{
			num = 0;
			
			if (num >= 0)
				ID[num] = (calendar.w_year) / 1000;
			if (num + 1 >= 0)
				ID[num + 1] = (calendar.w_year) / 100 % 10;
			if (num + 2 >= 0)
				ID[num + 2] = (calendar.w_year) / 10 % 10;
			if (num + 3 >= 0)
				ID[num + 3] = (calendar.w_year) % 10;

			ID[num + 4] = 11;

			if(state == 0)
			{
				ID[num + 5] = calendar.w_month / 10 % 10;
			}
			else
			{
				if ((calendar.w_month / 10 % 10) == 0)
					num--;
				else
					ID[num + 5] = calendar.w_month / 10 % 10;
			}

			ID[num + 6] = (calendar.w_month) % 10;
			ID[num + 7] = 12;

			if(state == 0)
			{
				ID[num + 8] = calendar.w_date / 10 % 10;
			}
			else
			{
				if ((calendar.w_date / 10 % 10) == 0)
					num--;
				else
					ID[num + 8] = calendar.w_date / 10 % 10;
			}

			ID[num + 9] = calendar.w_date % 10;

			if (mode == 1)
				cnt = num + 9;
			else
			{
				ID[num + 10] = 13;
				cnt = num + 10;
			}
		}
		else if (mode == 2 || mode == 10)
		{
			num = 0;
			if (num >= 0)
				ID[num] = calendar.w_year / 1000;
			if (num + 1 >= 0)
				ID[num + 1] = calendar.w_year / 100 % 10;
			if (num + 2 >= 0)
				ID[num + 2] = calendar.w_year / 10 % 10;
			if (num + 3 >= 0)
				ID[num + 3] = calendar.w_year % 10;

			ID[num + 4] = 11;

			if(state == 0)
			{
				ID[num + 5] = calendar.w_month / 10 % 10;
			}
			else
			{
				if ((calendar.w_month / 10 % 10) == 0)
					num--;
				else
					ID[num + 5] = calendar.w_month / 10 % 10;
			}

			ID[num + 6] = calendar.w_month % 10;

			if (mode == 2)
				cnt = num + 6;
			else
			{
				ID[num + 7] = 12;
				cnt = num + 7;
			}
		}
		else if (mode == 3 || mode == 11)
		{
			num = 0;
			if (state == 0)
			{
				ID[num] = calendar.w_month / 10 % 10;
			}
			else
			{
				if ((calendar.w_month / 10 % 10) == 0)
					num--;
				else
					ID[num] = calendar.w_month / 10 % 10;
			}

			ID[num + 1] = calendar.w_month % 10;
			ID[num + 2] = 12;
	
			if (state == 0)
			{
				ID[num + 3] = calendar.w_date / 10 % 10;
			}
			else 
			{
				if ((calendar.w_date / 10 % 10) == 0)
					num--;
				else
					ID[num + 3] = calendar.w_date / 10 % 10;
			}

			ID[num + 4] = calendar.w_date % 10;

			if (mode == 3)
				cnt = num + 4;
			else
			{
				ID[num + 5] = 13;
				cnt = num + 5;
			}
		}
		else if (mode == 4)
		{
			num = 0;
			if (state == 0)
			{
				ID[num] = calendar.hour / 10 % 10;
			}
			else
			{
				if ((calendar.hour / 10 % 10) == 0)
					num--;
				else
					ID[num] = calendar.hour / 10 % 10;
			}
			ID[num + 1] = calendar.hour % 10;
			ID[num + 2] = 10;
			ID[num + 3] = calendar.min / 10 % 10;
			ID[num + 4] = calendar.min % 10;
			ID[num + 5] = 10;
			ID[num + 6] = calendar.sec / 10 % 10;
			ID[num + 7] = calendar.sec % 10;
			cnt = num + 7;
		}
		else if (mode == 5)
		{
			num = 0;
			if (state == 0)
			{
				ID[num] = calendar.hour / 10 % 10;
			}
			else
			{
				if ((calendar.hour / 10 % 10) == 0)
					num--;
				else
					ID[num] = calendar.hour / 10 % 10;
			}
			ID[num + 1] = calendar.hour % 10;
			ID[num + 2] = 10;
			ID[num + 3] = calendar.min / 10 % 10;
			ID[num + 4] = calendar.min % 10;
			cnt = num + 4;
		}
		else if (mode == 6)
		{
			num = 0;
			ID[num] = calendar.min / 10 % 10;
			ID[num + 1] = calendar.min % 10;
			ID[num + 2] = 10;
			ID[num + 3] = calendar.sec / 10 % 10;
			ID[num + 4] = calendar.sec % 10;
			cnt = num + 4;
		}

		for (i = 0; i <= cnt; i++)
		{
			if (id_start + ID[i] >= addr_index[11])
				continue; // ICON is available

			Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * (id_start + ID[i]), 12);

			pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
			pic_w = buff[4] + (buff[5] << 8);
			pic_h = buff[6] + (buff[7] << 8);
			pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
			pic_flag = buff[11];

			if (pic_len != 0 && pic_flag != 0xFF) // ICON is not empty
			{
				if (Screen_DIR == 0)
				{
					if (i == 0)
					{
						high = pic_h;
						flag = pic_flag;
					}

					x_src = x_time;
					y_src = 0;
					w_real = pic_w;
				}
				else if (Screen_DIR == 1)
				{
					if (i == 0)
					{
						width = pic_w;
						flag = pic_flag;
					}

					x_src = LCD_XSIZE_TFT - pic_w;
					y_src = x_time;
					w_real = pic_h;
				}
				else if (Screen_DIR == 2)
				{
					if (i == 0)
					{
						high = pic_h;
						flag = pic_flag;
					}

					x_src = LCD_XSIZE_TFT - pic_w - x_time;
					y_src = LCD_YSIZE_TFT - pic_h;
					w_real = pic_w;
				}
				else if (Screen_DIR == 3)
				{
					if (i == 0)
					{
						width = pic_w;
						flag = pic_flag;
					}

					x_src = 0;
					y_src = LCD_YSIZE_TFT - pic_h - x_time;
					w_real = pic_h;
				}
				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, x_src, y_src,
													pic_w, pic_h,
													pic_w, pic_add,
													LAY_BUFF1, LCD_XSIZE_TFT);
				x_time += w_real;
			}
		}
		/*On the keyboard input page*/
		if (DioKb_flag == 1)
		{
			if (!(x > gDio_Info[Dio_num].Xe || (x + x_time) < gDio_Info[Dio_num].Xs || y > gDio_Info[Dio_num].Ye || (y + pic_h) < gDio_Info[Dio_num].Ys))
			{
				return 0;
			}
		}
		else if (AioKb_flag == 1)
		{
			if (!(x > gAio_Info[Aio_num].Xe || (x + x_time) < gAio_Info[Aio_num].Xs || y > gAio_Info[Aio_num].Ye || (y + pic_h) < gAio_Info[Aio_num].Ys))
			{
				return 0;
			}
		}
		else if (GBKioKb_flag == 1)
		{
			if (!(x > gGBKio_Info[GBKio_num].Xe || (x + x_time) < gGBKio_Info[GBKio_num].Xs || y > gGBKio_Info[GBKio_num].Ye || (y + pic_h) < gGBKio_Info[GBKio_num].Ys))
			{
				return 0;
			}
		}

		if (Screen_DIR == 0)
		{
			x_src = 0;
			y_src = 0;
			x_des = x;
			y_des = y;
			w_real = x_time;
			h_real = high;
		}
		else if (Screen_DIR == 1)
		{
			x_src = LCD_XSIZE_TFT - width;
			y_src = 0;
			x_des = x - width + 1;
			y_des = y;
			w_real = width;
			h_real = x_time;
		}
		else if (Screen_DIR == 2)
		{
			x_src = LCD_XSIZE_TFT - x_time;
			y_src = LCD_YSIZE_TFT - high;
			x_des = x - x_time + 1;
			y_des = y - high + 1;
			w_real = x_time;
			h_real = high;
		}
		else if (Screen_DIR == 3)
		{
			x_src = 0;
			y_src = LCD_YSIZE_TFT - x_time;
			x_des = x;
			y_des = y - x_time + 1;
			w_real = width;
			h_real = x_time;
		}

		if (flag == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
								  LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
								  Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_real, h_real);
		}
		else if (flag == 1)
		{
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
								  MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
								  LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_real, h_real);

			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
											 LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
											 Black, w_real, h_real);

			LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
								  LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
								  Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_real, h_real);
		}
		else if (flag == 2)
		{
			BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
										   LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
										   Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
										   w_real, h_real);
		}

		if (gRTC->Len_last > x_time && status) // Remove the tail residue
		{
			if (Screen_DIR == 0)
			{
				x_des = x + x_time;
				y_des = y;
				w_real = gRTC->Len_last - x_time;
				h_real = high;
			}
			else if (Screen_DIR == 1)
			{
				x_des = x - width + 1;
				y_des = y + x_time;
				w_real = width;
				h_real = gRTC->Len_last - x_time;
			}
			else if (Screen_DIR == 2)
			{
				x_des = x - gRTC->Len_last + 1;
				y_des = y - high + 1;
				w_real = gRTC->Len_last - x_time;
				h_real = high;
			}
			else if (Screen_DIR == 3)
			{
				x_des = x;
				y_des = y - gRTC->Len_last + 1;
				w_real = width;
				h_real = gRTC->Len_last - x_time;
			}
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
								  MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
								  Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_real, h_real);
		}
		gRTC->Len_last = x_time; // Record the time sequence length of the last second
	}
	else if (mode == 7) // Week
	{
		ID[0] = 14 + calendar.week; // Offset value of icon in RTC material of week material

		if (id_start + ID[0] < addr_index[11]) // ICON is available
		{
			Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * (id_start + ID[0]), 12);

			pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
			pic_w = buff[4] + (buff[5] << 8);
			pic_h = buff[6] + (buff[7] << 8);
			pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
			pic_flag = buff[11];

			/*On the keyboard input page*/
			if (DioKb_flag == 1)
			{
				if (!(x > gDio_Info[Dio_num].Xe || (x + pic_w) < gDio_Info[Dio_num].Xs || y > gDio_Info[Dio_num].Ye || (y + pic_h) < gDio_Info[Dio_num].Ys))
				{
					return 0;
				}
			}
			else if (AioKb_flag == 1)
			{
				if (!(x > gAio_Info[Aio_num].Xe || (x + pic_w) < gAio_Info[Aio_num].Xs || y > gAio_Info[Aio_num].Ye || (y + pic_h) < gAio_Info[Aio_num].Ys))
				{
					return 0;
				}
			}
			else if (GBKioKb_flag == 1)
			{
				if (!(x > gGBKio_Info[GBKio_num].Xe || (x + pic_w) < gGBKio_Info[GBKio_num].Xs || y > gGBKio_Info[GBKio_num].Ye || (y + pic_h) < gGBKio_Info[GBKio_num].Ys))
				{
					return 0;
				}
			}

			if (pic_len != 0 && pic_flag != 0xFF) // ICON is not empty
			{
				if (Screen_DIR == 0)
				{
					x_des = x;
					y_des = y;
				}
				else if (Screen_DIR == 1)
				{
					x_des = x - pic_w + 1;
					y_des = y;
				}
				else if (Screen_DIR == 2)
				{
					x_des = x - pic_w + 1;
					y_des = y - pic_h + 1;
				}
				else if (Screen_DIR == 3)
				{
					x_des = x;
					y_des = y - pic_h + 1;
				}
				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, x_des, y_des,
													pic_w, pic_h,
													pic_w, pic_add, LAY_BUFF1, LCD_XSIZE_TFT);

				if (pic_flag == 0)
				{

					LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
										  LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
										  Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
										  0x0c, pic_w, pic_h);
				}
				else if (pic_flag == 1)
				{
					LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
										  MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
										  LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
										  0x0c, pic_w, pic_h);

					LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
													 LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
													 Black, pic_w, pic_h);

					LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
										  LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
										  Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
										  0x0c, pic_w, pic_h);
				}
				else if (pic_flag == 2)
				{
					BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
												   LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
												   Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
												   pic_w, pic_h);
				}
			}
		}
	}
}

/***********************************************************************************
* Function Name	: RTC_display
* Description	: Get digital clock info.
* Input			: - type: digital clock info sources
				  - buff: digital clock info
* Output		: None
* Return		: None
************************************************************************************/

void RTC_display(uint8_t type, uint8_t *buff)
{
	uint8_t mode = 0;
	uint16_t p_add, p_len, pointer;
	uint16_t x, y, w, h;
	uint16_t simage, eimage;

	if (type == 0) // Instruction information source is configuration file
	{
		x = buff[5] + (buff[6] << 8);
		y = buff[7] + (buff[8] << 8);
		simage = buff[9] + (buff[10] << 8);
		eimage = buff[11] + (buff[12] << 8);
		mode = buff[13];
	}
	else if (type == 1) // Instruction information ource is variable space
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		x = var[pointer + 1] + (var[pointer + 2] << 8);
		y = var[pointer + 3] + (var[pointer + 4] << 8);
		simage = var[pointer + 5] + (var[pointer + 6] << 8);
		eimage = var[pointer + 7] + (var[pointer + 8] << 8);
		mode = var[pointer + 9];
	}

	gRTC_Info[RTC_num].x = x;
	gRTC_Info[RTC_num].y = y;
	gRTC_Info[RTC_num].ID_start = simage;
	gRTC_Info[RTC_num].mode = mode;

	if (Pop_up_flag == 1)
	{
		gRTC_Info[RTC_num].x += Pop_up_x;
		gRTC_Info[RTC_num].y += Pop_up_y;
	}
	Display_RTC(&gRTC_Info[RTC_num], 0);
	RTC_num++;
}

void show_RTC(void)
{
	if (RTC_num > 0 && RTC_flag == 1)
	{
		RTC_flag = 0;
		for (uint8_t i = 0; i < RTC_num; i++)
		{
			Display_RTC(&gRTC_Info[i], 1);
		}
	}
}
/**************************************************/

Clock_Info gClock_Info[Clock_Size];
uint8_t Clock_num = 0;
uint8_t Clock_flag = 0;
uint32_t Lay_temp = 0;

void DrawPoint_Buffer_alphablend(int32_t buff_x, int32_t buff_y, float alpha, uint32_t color, uint16_t canvas)
{
	if (alpha == 1)
	{
		Goto_Pixel_XY(buff_x, buff_y);
		LCD_CmdWrite(0x04);
		SS_RESET;
		SPI2_ReadWriteByte(0x80);
		SPI2_WriteByte(color);
		SPI2_WriteByte(color >> 8);
		if (TFT_bitcolor == 1)
			SPI2_WriteByte(color >> 16);
		SS_SET;
	}
	else if (alpha > 0)
	{
		uint16_t R0 = 0, G0 = 0, B0 = 0;
		uint16_t R1 = 0, G1 = 0, B1 = 0;
		uint16_t R2 = 0, G2 = 0, B2 = 0;
		uint32_t new_color;
		if (TFT_bitcolor == 0)
		{
			R0 = (color >> 11) & 0x1F;
			G0 = (color >> 5) & 0x3F;
			B0 = color & 0x1F;
		}
		else if (TFT_bitcolor == 1)
		{
			R0 = (color >> 16) & 0xFF;
			G0 = (color >> 8) & 0xFF;
			B0 = color & 0xFF;
		}
		uint8_t color1, color2, color3;
		uint32_t color4;
		Canvas_Image_Start_address(Lay_temp);
		Canvas_image_width(LCD_XSIZE_TFT);
		Goto_Pixel_XY(buff_x, buff_y);
		LCD_CmdWrite(0x04);
		LCD_DataRead();
		color1 = LCD_DataRead();
		color2 = LCD_DataRead();
		if (TFT_bitcolor == 1)
			color3 = LCD_DataRead();

		if (TFT_bitcolor == 0)
		{
			color4 = color1 + (color2 << 8);
			R1 = (color4 >> 11) & 0x1F;
			G1 = (color4 >> 5) & 0x3F;
			B1 = color4 & 0x1F;
		}
		else if (TFT_bitcolor == 1)
		{
			color4 = color1 + (color2 << 8) + (color3 << 16);
			R1 = (color4 >> 16) & 0xFF;
			G1 = (color4 >> 8) & 0xFF;
			B1 = color4 & 0xFF;
		}
		R2 = R1 * (1 - alpha) + R0 * alpha;
		G2 = G1 * (1 - alpha) + G0 * alpha;
		B2 = B1 * (1 - alpha) + B0 * alpha;
		if (TFT_bitcolor == 0)
			new_color = (R2 << 11) | (G2 << 5) | B2;
		else
			new_color = (R2 << 16) | (G2 << 8) | B2;
		Goto_Pixel_XY(buff_x, buff_y);
		LCD_CmdWrite(0x04);
		SS_RESET;
		SPI2_ReadWriteByte(0x80);
		SPI2_WriteByte(new_color);
		SPI2_WriteByte(new_color >> 8);
		if (TFT_bitcolor == 1)
			SPI2_WriteByte(new_color >> 16);
		SS_SET;
	}
}

/***********************************************************************************
* Function Name	: Display_Clock
* Description	: Show analog clock.
* Input			: - num: analog clock number
				  - status: analog clock status
* Output		: None
* Return		: None
************************************************************************************/

uint8_t Display_Clock(uint8_t num, uint8_t status)
{
	uint8_t hour, min, sec;
	uint8_t hour_temp;
	uint16_t canvas_w;

	hour = calendar.hour;
	min = calendar.min;
	sec = calendar.sec;

	hour = hour % 12;

	if (gClock_Info[num].Image_BG < addr_index[11]) // ICON is available
	{
		if (gClock_Info[num].len_pic != 0 && gClock_Info[num].flag_pic != 0xFF) // ICON is not empty
		{
			// Cache underlay
			Canvas_Image_Start_address(LAY_BUFF1);
			Canvas_image_width(LCD_XSIZE_TFT);
			pic_flag = gClock_Info[num].flag_pic;
			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												gClock_Info[num].w_pic, gClock_Info[num].h_pic,
												gClock_Info[num].w_pic, gClock_Info[num].addr_pic, LAY_BUFF1, LCD_XSIZE_TFT);

			/*On the keyboard input page*/
			if (DioKb_flag == 1)
			{
				if (!(gClock_Info[num].x_pic > gDio_Info[Dio_num].Xe || (gClock_Info[num].x_pic + gClock_Info[num].w_pic) < gDio_Info[Dio_num].Xs || gClock_Info[num].y_pic > gDio_Info[Dio_num].Ye || (gClock_Info[num].y_pic + gClock_Info[num].h_pic) < gDio_Info[Dio_num].Ys))
				{
					return 0;
				}
			}
			else if (AioKb_flag == 1)
			{
				if (!(gClock_Info[num].x_pic > gAio_Info[Aio_num].Xe || (gClock_Info[num].x_pic + gClock_Info[num].w_pic) < gAio_Info[Aio_num].Xs || gClock_Info[num].y_pic > gAio_Info[Aio_num].Ye || (gClock_Info[num].y_pic + gClock_Info[num].h_pic) < gAio_Info[Aio_num].Ys))
				{
					return 0;
				}
			}
			else if (GBKioKb_flag == 1)
			{
				if (!(gClock_Info[num].x_pic > gGBKio_Info[GBKio_num].Xe || (gClock_Info[num].x_pic + gClock_Info[num].w_pic) < gGBKio_Info[GBKio_num].Xs || gClock_Info[num].y_pic > gGBKio_Info[GBKio_num].Ye || (gClock_Info[num].y_pic + gClock_Info[num].h_pic) < gGBKio_Info[GBKio_num].Ys))
				{
					return 0;
				}
			}
			if (status == 0)
				Lay_temp = Display_Layer;
			else
				Lay_temp = LAY_BUFF2;

			if (gClock_Info[num].flag_pic == 0)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  Lay_temp, LCD_XSIZE_TFT, gClock_Info[num].x_pic, gClock_Info[num].y_pic,
									  0x0c, gClock_Info[num].w_pic, gClock_Info[num].h_pic);
			}
			else if (gClock_Info[num].flag_pic == 1)
			{
				if (Lay_temp == LAY_BUFF2)
				{
					LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gClock_Info[num].x_pic, gClock_Info[num].y_pic,
										  MAIN_BUFF, LCD_XSIZE_TFT, gClock_Info[num].x_pic, gClock_Info[num].y_pic,
										  Lay_temp, LCD_XSIZE_TFT, gClock_Info[num].x_pic, gClock_Info[num].y_pic,
										  0x0c, gClock_Info[num].w_pic, gClock_Info[num].h_pic);
				}
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
												 Lay_temp, LCD_XSIZE_TFT, gClock_Info[num].x_pic, gClock_Info[num].y_pic,
												 Black, gClock_Info[num].w_pic, gClock_Info[num].h_pic);
			}
			else if (gClock_Info[num].flag_pic == 2)
			{
				BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, gClock_Info[num].x_pic, gClock_Info[num].y_pic,
											   LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
											   Lay_temp, LCD_XSIZE_TFT, gClock_Info[num].x_pic, gClock_Info[num].y_pic,
											   gClock_Info[num].w_pic, gClock_Info[num].h_pic);
			}

			DrawClockPointer(gClock_Info[num].x, gClock_Info[num].y, hour * 30 + min * 30 / 60, gClock_Info[num].L_hour, gClock_Info[num].S_hour, gClock_Info[num].Color_hour, gClock_Info[num].W_hour,
							 gClock_Info[num].x, gClock_Info[num].y, min * 6 + sec * 6 / 60, gClock_Info[num].L_min, gClock_Info[num].S_min, gClock_Info[num].Color_min, gClock_Info[num].W_min,
							 gClock_Info[num].x, gClock_Info[num].y, sec * 6, gClock_Info[num].L_sec, gClock_Info[num].S_sec, gClock_Info[num].Color_sec, gClock_Info[num].W_sec,
							 gClock_Info[num].x_pic, gClock_Info[num].y_pic, gClock_Info[num].w_pic, gClock_Info[num].h_pic, num);

			// Cache display center ICON
			if (gClock_Info[num].len_cen != 0 && gClock_Info[num].flag_cen != 0xFF) // ICON is not empty
			{
				Canvas_Image_Start_address(LAY_BUFF1);
				Canvas_image_width(LCD_XSIZE_TFT);
				pic_flag = gClock_Info[num].flag_cen;
				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													gClock_Info[num].w_cen, gClock_Info[num].h_cen,
													gClock_Info[num].w_cen, gClock_Info[num].addr_cen,
													LAY_BUFF1, LCD_XSIZE_TFT);

				if (status == 0)
					Lay_temp = Display_Layer;
				else
					Lay_temp = LAY_BUFF2;

				if (gClock_Info[num].flag_cen == 0)
				{
					LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
										  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
										  Lay_temp, LCD_XSIZE_TFT, gClock_Info[num].x_cen, gClock_Info[num].y_cen,
										  0x0c, gClock_Info[num].w_cen, gClock_Info[num].h_cen);
				}
				else if (gClock_Info[num].flag_cen == 1)
				{
					LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
													 Lay_temp, LCD_XSIZE_TFT, gClock_Info[num].x_cen, gClock_Info[num].y_cen,
													 Black, gClock_Info[num].w_cen, gClock_Info[num].h_cen);
				}
				else if (gClock_Info[num].flag_cen == 2)
				{
					BTE_Pixel_16bpp_Alpha_Blending(Lay_temp, LCD_XSIZE_TFT, gClock_Info[num].x_cen, gClock_Info[num].y_cen,
												   LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
												   Lay_temp, LCD_XSIZE_TFT, gClock_Info[num].x_cen, gClock_Info[num].y_cen,
												   gClock_Info[num].w_cen, gClock_Info[num].h_cen);
				}
			}

			if (status)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, gClock_Info[num].x_pic, gClock_Info[num].y_pic,
									  LAY_BUFF2, LCD_XSIZE_TFT, gClock_Info[num].x_pic, gClock_Info[num].y_pic,
									  Display_Layer, LCD_XSIZE_TFT, gClock_Info[num].x_pic, gClock_Info[num].y_pic,
									  0x0c, gClock_Info[num].w_pic, gClock_Info[num].h_pic);
			}
		}
	}
}

/***********************************************************************************
* Function Name	: Clock_display
* Description	: Get analog clock info.
* Input			: - type: analog clock info sources
				  - buff: analog clock info
* Output		: None
* Return		: None
************************************************************************************/

void Clock_display(uint8_t type, uint8_t *buff)
{
	uint8_t buff1[12] = {0};
	uint8_t L_hour, S_hour, W_hour, L_min, S_min, W_min, L_sec, S_sec, W_sec;
	uint16_t p_add, p_len, pointer;
	uint16_t x, y, image, image_cen;
	uint16_t r, g, b;
	uint32_t Color_hour = 0, Color_min = 0, Color_sec = 0;
	if (type == 0) // Instruction information source is configuration file
	{
		x = buff[5] + (buff[6] << 8);
		y = buff[7] + (buff[8] << 8);
		image = buff[9] + (buff[10] << 8);
		L_hour = buff[11];
		S_hour = buff[12];
		W_hour = buff[13];
		Color_hour = buff[14] + (buff[15] << 8) + (buff[16] << 16);
		L_min = buff[17];
		S_min = buff[18];
		W_min = buff[19];
		Color_min = buff[20] + (buff[21] << 8) + (buff[22] << 16);
		L_sec = buff[23];
		S_sec = buff[24];
		W_sec = buff[25];
		Color_sec = buff[26] + (buff[27] << 8) + (buff[28] << 16);
		image_cen = buff[29] + (buff[30] << 8);
	}
	else if (type == 1) // Instruction information ource is variable space
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		x = var[pointer + 1] + (var[pointer + 2] << 8);
		y = var[pointer + 3] + (var[pointer + 4] << 8);
		image = var[pointer + 5] + (var[pointer + 6] << 8);
		L_hour = var[pointer + 7];
		S_hour = var[pointer + 8];
		W_hour = var[pointer + 9];
		Color_hour = var[pointer + 10] + (var[pointer + 11] << 8) + (var[pointer + 12] << 16);
		L_min = var[pointer + 13];
		S_min = var[pointer + 14];
		W_min = var[pointer + 15];
		Color_min = var[pointer + 16] + (var[pointer + 17] << 8) + (var[pointer + 18] << 16);
		L_sec = var[pointer + 19];
		S_sec = var[pointer + 20];
		W_sec = var[pointer + 21];
		Color_sec = var[pointer + 22] + (var[pointer + 23] << 8) + (var[pointer + 24] << 16);
		image_cen = var[pointer + 25] + (var[pointer + 26] << 8);
	}

	if (Pop_up_flag == 1) // Popup offset
	{
		x += Pop_up_x;
		y += Pop_up_y;
	}
	gClock_Info[Clock_num].x_pic = x;
	gClock_Info[Clock_num].y_pic = y;
	gClock_Info[Clock_num].Image_BG = image;
	gClock_Info[Clock_num].L_hour = L_hour;
	gClock_Info[Clock_num].S_hour = S_hour;
	gClock_Info[Clock_num].W_hour = W_hour;
	gClock_Info[Clock_num].Color_hour = Color_hour;
	gClock_Info[Clock_num].L_min = L_min;
	gClock_Info[Clock_num].S_min = S_min;
	gClock_Info[Clock_num].W_min = W_min;
	gClock_Info[Clock_num].Color_min = Color_min;
	gClock_Info[Clock_num].L_sec = L_sec;
	gClock_Info[Clock_num].S_sec = S_sec;
	gClock_Info[Clock_num].W_sec = W_sec;
	gClock_Info[Clock_num].Color_sec = Color_sec;

	if (image < addr_index[11]) // ICON is not empty
	{
		Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * image, 12);

		gClock_Info[Clock_num].addr_pic = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
		gClock_Info[Clock_num].w_pic = buff1[4] + (buff1[5] << 8);
		gClock_Info[Clock_num].h_pic = buff1[6] + (buff1[7] << 8);
		gClock_Info[Clock_num].len_pic = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
		gClock_Info[Clock_num].flag_pic = buff1[11];
		gClock_Info[Clock_num].x = x + gClock_Info[Clock_num].w_pic / 2;
		gClock_Info[Clock_num].y = y + gClock_Info[Clock_num].h_pic / 2;

		if (image_cen < addr_index[11]) // ICON is not empty
		{
			Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * image_cen, 12);

			gClock_Info[Clock_num].addr_cen = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
			gClock_Info[Clock_num].w_cen = buff1[4] + (buff1[5] << 8);
			gClock_Info[Clock_num].h_cen = buff1[6] + (buff1[7] << 8);
			gClock_Info[Clock_num].len_cen = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
			gClock_Info[Clock_num].flag_cen = buff1[11];
			gClock_Info[Clock_num].x_cen = gClock_Info[Clock_num].x - gClock_Info[Clock_num].w_cen / 2;
			gClock_Info[Clock_num].y_cen = gClock_Info[Clock_num].y - gClock_Info[Clock_num].h_cen / 2;
		}
	}

	Display_Clock(Clock_num, 0);
	Clock_num++;
}

void show_Clock(void)
{
	if (Clock_num > 0 && Clock_flag == 1)
	{
		Clock_flag = 0;
		for (uint8_t i = 0; i < Clock_num; i++)
		{
			Display_Clock(i, 1);
		}
	}
}

Scroll_Info gScroll_Info[Scroll_Size];
uint8_t Scroll_num = 0;
uint8_t Scroll_flag = 0;

/*******************************************************************************
 * Function Name	: LT_Print_zk_Font_Scroll
 * Description		: txt scroll
 * Input			:
 * Output			: None
 * Return			: None
 ********************************************************************/

void LT_Print_zk_Scroll
(
	uint16_t encode		// encoding type  0:GB2312  1:GBK  2:BIG5  3:UNICODE  4:ASCII 6:UNICODE
	,
	uint32_t FlashAddr	// Font source address(exSpiFlash)
	,
	uint32_t ShowAddr	// Displays the address of the layer
	,
	uint16_t width		// Display the width of the layer
	,
	uint8_t W_Size		// Font width
	,
	uint8_t H_Size		// Font height
	,
	uint8_t Alignment	// Alignment 0:Left 1:Mid 2:Right
	,
	uint32_t FontColor	// The foreground color of the font
	,
	uint32_t BGColor	// The background color of the font
	,
	uint16_t Xs			// Text box start x position
	,
	uint16_t Ys			// Text box start Y position
	,
	uint16_t Xe			// Text box end x position
	,
	uint16_t Ye			// Text box end y position
	,
	uint8_t dis_x		// The interval between two adjacent fonts on the x-axis
	,
	uint8_t dis_y		// The interval between two adjacent fonts on the y-axis
	,
	uint8_t *c,
	uint16_t len_max	// Max lenght of text
	,
	uint8_t num			// Scroll_num
	,
	uint8_t transparent //	Background color enable, 0:Disable  1:Enable
)
{
	uint16_t temp_H = 0, temp_L = 0, temp = 0;
	uint32_t i = 0;
	uint16_t width_Hor = 0;
	uint8_t pixel_format = 1;
	uint8_t temp_pixel, Font_W;
	uint8_t Full_flag = 0;
	uint16_t Lay_len = 0;
	uint16_t x_src = 0, y_src = 0, len_disp = 0, offset_disp = 0, len_real = 0, w_real = 0, h_real = 0;
	uint16_t x_des = 0, y_des = 0, H_Maximum = 0;

	uint8_t buff_code[4];
	uint16_t unicode = 0, start_code = 0, end_code = 0;

	Flash_Read_UI(Ascii_w, FlashAddr + 9, 95);
	Flash_Read_UI(&pixel_format, FlashAddr + 8, 1);

	gScroll_Info[num].pixel_format = pixel_format;

	if (pixel_format == 0)
		temp_pixel = 8;
	else if (pixel_format == 1)
		temp_pixel = 4;
	else if (pixel_format == 2)
		temp_pixel = 2;

	if (W_Size % temp_pixel != 0)
		width_Hor = W_Size / temp_pixel + 1;
	else
		width_Hor = W_Size / temp_pixel;

	/* If there are multiple text scroll controls on the same page,
		you need to set the_ The contents to be scrolled are arranged in the BUFF12 area from top to bottom,
		and the displayed contents will be cached only once when they are updated */
	if (num == 0)
		gScroll_Info[num].y_offset = 0;
	else
	{
		if (Screen_DIR == 0 || Screen_DIR == 2)
		{
			if (TFT_bitcolor == 1)
			{
				if (pixel_format == 0)
					gScroll_Info[num].y_offset = gScroll_Info[num - 1].y_offset + gScroll_Info[num - 1].h; // Buffer Y coordinate offset value
				else
					gScroll_Info[num].y_offset = (gScroll_Info[num - 1].y_offset + gScroll_Info[num - 1].h) * 3 / 2;
				/* In order to match the 16bpp and 24bpp cache data with the upper array height,
				the height needs to be * 1.5 times*/
			}
			else
				gScroll_Info[num].y_offset = gScroll_Info[num - 1].y_offset + gScroll_Info[num - 1].h;
		}
		else if (Screen_DIR == 1 || Screen_DIR == 3)
		{
			if (TFT_bitcolor == 1)
			{
				if (pixel_format == 0)
					gScroll_Info[num].y_offset = gScroll_Info[num - 1].y_offset + gScroll_Info[num - 1].w;
				else
					gScroll_Info[num].y_offset = (gScroll_Info[num - 1].y_offset + gScroll_Info[num - 1].w) * 3 / 2;
			}
			else
				gScroll_Info[num].y_offset = gScroll_Info[num - 1].y_offset + gScroll_Info[num - 1].w;
		}
	}

	if (Screen_DIR == 0)
		Lay_len = LCD_XSIZE_TFT * 2;
	else if (Screen_DIR == 1)
		Lay_len = LCD_XSIZE_TFT;
	else if (Screen_DIR == 2)
	{
		Lay_len = LCD_XSIZE_TFT * 2;
		if (TFT_bitcolor == 0)
			H_Maximum = 2 * LCD_YSIZE_TFT;
		else if (TFT_bitcolor == 1)
			H_Maximum = 3 * LCD_YSIZE_TFT / 4;
	}
	else if (Screen_DIR == 3)
	{
		Lay_len = LCD_XSIZE_TFT;
		if (TFT_bitcolor == 0)
			H_Maximum = 2 * LCD_YSIZE_TFT;
		else if (TFT_bitcolor == 1)
			H_Maximum = 3 * LCD_YSIZE_TFT / 2;
	}

	len_disp = 0;
	offset_disp = gScroll_Info[num].y_offset;

	/*	When RGB888 is displayed, the 2 gray scale and 4 gray scale fonts are still in the ARGB4444 format,
		so when caching such data, it is necessary to first convert to 16bpp mod */
	if (TFT_bitcolor == 1 && pixel_format != 0)
		Memory_16bpp_Mode();

	if (encode == 4) // ASCII
	{
		while ((c[i] != '\0'))
		{
			if (Screen_DIR == 0)
			{
				x_src = len_disp;
				y_src = offset_disp;
			}
			else if (Screen_DIR == 1)
			{
				x_src = Lay_len - H_Size - offset_disp;
				y_src = len_disp;
			}
			else if (Screen_DIR == 2)
			{
				x_src = Lay_len - width_Hor * temp_pixel - len_disp;
				y_src = H_Maximum - H_Size - offset_disp;
			}
			else if (Screen_DIR == 3)
			{
				x_src = offset_disp;
				y_src = H_Maximum - width_Hor * temp_pixel - len_disp;
			}
			if (c[i] < 0x80 && c[i] >= 0x20)
			{
				// In LAY_ BUFF12 cache monochrome data content
				LT768_Draw_16bit_2(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, BGColor, transparent, FlashAddr + 104 + (c[i] - 0x20) * width_Hor * H_Size, LAY_BUFF15, Lay_len);
				len_disp = len_disp + Ascii_w[c[i] - 0x20] + 2;

				i++;
				if (i >= len_max)
					break;
			}
			else
				break;
		}
	}
	else if (encode == 0) // GB2312
	{
		while ((c[i] != '\0'))
		{
			if (Screen_DIR == 0)
			{
				x_src = len_disp;
				y_src = offset_disp;
			}
			else if (Screen_DIR == 1)
			{
				x_src = Lay_len - H_Size - offset_disp;
				y_src = len_disp;
			}
			else if (Screen_DIR == 2)
			{
				x_src = Lay_len - width_Hor * temp_pixel - len_disp;
				y_src = H_Maximum - H_Size - offset_disp;
			}
			else if (Screen_DIR == 3)
			{
				x_src = offset_disp;
				y_src = H_Maximum - width_Hor * temp_pixel - len_disp;
			}
			if (c[i] < 0x80 && c[i] >= 0x20)
			{
				// In LAY_ BUFF12 cache monochrome data content
				LT768_Draw_16bit_2(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, BGColor, transparent, FlashAddr + 104 + (c[i] - 0x20) * width_Hor * H_Size, LAY_BUFF15, Lay_len);

				len_disp = len_disp + Ascii_w[c[i] - 0x20] + 2;
				i++;
				if (i >= len_max)
					break;
			}
			else if (c[i] >= 0xa1 && c[i] != 0xFF)
			{
				temp_H = ((c[i] - 0xa1) & 0x00ff) * 94;
				temp_L = c[i + 1] - 0xa1;
				temp = temp_H + temp_L;

				// In LAY_ BUFF12 cache monochrome data content
				LT768_Draw_16bit_2(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, BGColor, transparent, FlashAddr + 104 + (95 + temp) * width_Hor * H_Size, LAY_BUFF15, Lay_len);

				len_disp += W_Size;

				i += 2;
				if (i >= len_max)
					break;
			}
			else
				break;
		}
	}
	else if (encode == 1) // GBK
	{
		while ((c[i] != '\0'))
		{
			if (Screen_DIR == 0)
			{
				x_src = len_disp;
				y_src = offset_disp;
			}
			else if (Screen_DIR == 1)
			{
				x_src = Lay_len - H_Size - offset_disp;
				y_src = len_disp;
			}
			else if (Screen_DIR == 2)
			{
				x_src = Lay_len - width_Hor * temp_pixel - len_disp;
				y_src = H_Maximum - H_Size - offset_disp;
			}
			else if (Screen_DIR == 3)
			{
				x_src = offset_disp;
				y_src = H_Maximum - width_Hor * temp_pixel - len_disp;
			}
			if (c[i] < 0x80 && c[i] >= 0x20)
			{
				// In LAY_ BUFF12 cache monochrome data content
				LT768_Draw_16bit_2(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, BGColor, transparent, FlashAddr + 104 + (c[i] - 0x20) * width_Hor * H_Size, LAY_BUFF15, Lay_len);
				len_disp = len_disp + Ascii_w[c[i] - 0x20] + 2;

				i++;
				if (i >= len_max)
					break;
			}
			else if (c[i] >= 0x81 && c[i + 1] >= 0x40 && c[i] != 0xFF)
			{
				temp_H = (c[i] - 0x81) * 190;
				if (c[i + 1] <= 0x7F)
				{
					temp_L = c[i + 1] - 0x40;
				}
				else if (c[i + 1] > 0x7F)
				{
					temp_L = c[i + 1] - 0x40 - 1;
				}
				temp = temp_H + temp_L;

				// In LAY_ BUFF12 cache monochrome data content
				LT768_Draw_16bit_2(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, BGColor, transparent, FlashAddr + 104 + (95 + temp) * width_Hor * H_Size, LAY_BUFF15, Lay_len);
				len_disp += W_Size;
				i += 2;
				if (i >= len_max)
					break;
			}
			else
				break;
		}
	}
	else if (encode == 2) // BIG5
	{
		while ((c[i] != '\0'))
		{
			if (Screen_DIR == 0)
			{
				x_src = len_disp;
				y_src = offset_disp;
			}
			else if (Screen_DIR == 1)
			{
				x_src = Lay_len - H_Size - offset_disp;
				y_src = len_disp;
			}
			else if (Screen_DIR == 2)
			{
				x_src = Lay_len - width_Hor * temp_pixel - len_disp;
				y_src = H_Maximum - H_Size - offset_disp;
			}
			else if (Screen_DIR == 3)
			{
				x_src = offset_disp;
				y_src = H_Maximum - width_Hor * temp_pixel - len_disp;
			}
			if (c[i] < 128 && c[i] >= 0x20)
			{
				LT768_Draw_16bit_2(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, BGColor, transparent, FlashAddr + 104 + (c[i] - 0x20) * width_Hor * H_Size, LAY_BUFF15, Lay_len);
				len_disp = len_disp + Ascii_w[c[i] - 0x20] + 2;

				i++;
				if (i >= len_max)
					break;
			}
			else if (c[i] >= 0xa1)
			{
				temp_H = ((c[i] - 0xa1) & 0x00ff) * 160;
				if (c[i + 1] >= 0x40 && c[i + 1] <= 0x7F)
					temp_L = c[i + 1] - 0x40;
				else if (c[i + 1] >= 0xA0 && c[i + 1] <= 0xFF)
					temp_L = c[i + 1] - 0xA0 + 64;

				temp = temp_H + temp_L;

				LT768_Draw_16bit_2(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, BGColor, transparent, FlashAddr + 104 + (95 + temp) * width_Hor * H_Size, LAY_BUFF15, Lay_len);

				len_disp += W_Size;

				i += 2;
				if (i >= len_max)
					break;
			}
			else
				break;
		}
	}
	else if (encode == 3) // Unicode
	{
		Flash_Read_UI(buff_code, FlashAddr + 4, 4);
		start_code = buff_code[0] + (buff_code[1] << 8);
		end_code = buff_code[2] + (buff_code[3] << 8);

		while (((c[i] << 8) + c[i + 1]) > 0)
		{
			if (Screen_DIR == 0)
			{
				x_src = len_disp;
				y_src = offset_disp;
			}
			else if (Screen_DIR == 1)
			{
				x_src = Lay_len - H_Size - offset_disp;
				y_src = len_disp;
			}
			else if (Screen_DIR == 2)
			{
				x_src = Lay_len - width_Hor * temp_pixel - len_disp;
				y_src = H_Maximum - H_Size - offset_disp;
			}
			else if (Screen_DIR == 3)
			{
				x_src = offset_disp;
				y_src = H_Maximum - width_Hor * temp_pixel - len_disp;
			}

			unicode = (c[i] << 8) + c[i + 1];
			if (unicode < 128 && unicode > 0x00)
			{
				LT768_Draw_16bit_2(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, BGColor, transparent, FlashAddr + 104 + (unicode - 0x20) * width_Hor * H_Size, LAY_BUFF15, Lay_len);
				len_disp = len_disp + Ascii_w[unicode - 0x20] + 2;
				i += 2;
				if (i >= len_max)
					break;
			}
			else if (unicode >= start_code && unicode <= end_code)
			{
				temp = unicode - start_code;
				LT768_Draw_16bit_2(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, BGColor, transparent, FlashAddr + 104 + (95 + temp) * width_Hor * H_Size, LAY_BUFF15, Lay_len);

				len_disp += W_Size;
				i += 2;
				if (i >= len_max)
					break;
			}
			else
				break;
		}
	}
	else if (encode == 6) // Unicode
	{
		Flash_Read_UI(buff_code, FlashAddr + 4, 4);
		start_code = buff_code[0] + (buff_code[1] << 8);
		end_code = buff_code[2] + (buff_code[3] << 8);

		while (((c[i] << 8) + c[i + 1]) > 0)
		{
			if (Screen_DIR == 0)
			{
				x_src = len_disp;
				y_src = offset_disp;
			}
			else if (Screen_DIR == 1)
			{
				x_src = Lay_len - H_Size - offset_disp;
				y_src = len_disp;
			}
			else if (Screen_DIR == 2)
			{
				x_src = Lay_len - width_Hor * temp_pixel - len_disp;
				y_src = H_Maximum - H_Size - offset_disp;
			}
			else if (Screen_DIR == 3)
			{
				x_src = offset_disp;
				y_src = H_Maximum - width_Hor * temp_pixel - len_disp;
			}

			unicode = (c[i] << 8) + c[i + 1];
			if (unicode < 128 && unicode > 0x00)
			{
				LT768_Draw_16bit_2(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel, pixel_format, FontColor, BGColor, transparent, FlashAddr + 104 + (unicode - 0x20) * width_Hor * H_Size, LAY_BUFF15, Lay_len);
				len_disp = len_disp + Ascii_w[unicode - 0x20] + 2;
				i += 2;
				if (i >= len_max)
					break;
			}
			else if (unicode >= start_code && unicode <= end_code)
			{
				temp = unicode - start_code;
				Flash_Read_UI(&Font_W, FlashAddr + 104 + 95 * width_Hor * H_Size + temp * (width_Hor * H_Size + 1), 1);

				LT768_Draw_16bit_2(1, 0, x_src, y_src, width_Hor, H_Size, width_Hor * temp_pixel,
								   pixel_format, FontColor, BGColor, transparent,
								   FlashAddr + 104 + 95 * width_Hor * H_Size + temp * (width_Hor * H_Size + 1) + 1,
								   LAY_BUFF15, Lay_len);

				len_disp = len_disp + Font_W + 2;
				i += 2;
				if (i >= len_max)
					break;
			}
			else
				break;
		}
	}

	// After the 2 grayscale and 4 grayscale fonts are cached, they are restored to 24bpp mode
	if (TFT_bitcolor == 1 && pixel_format != 0)
		Memory_24bpp_Mode();

	// Restore the working window to full-screen area
	Active_Window_XY(0, 0);
	Active_Window_WH(LCD_XSIZE_TFT, LCD_YSIZE_TFT);
	Goto_Pixel_XY(0, 0);

	if (Screen_DIR == 0 || Screen_DIR == 2)
	{
		if (len_disp > Xe - Xs + 1)
			Full_flag = 1; // If the display content is larger than the width of the display box, scrolling must be triggered
		else
			Full_flag = 0; // If the display content is smaller than the display box width, whether to scroll according to the flag bit set by the customer
	}
	else if (Screen_DIR == 1 || Screen_DIR == 3)
	{
		if (len_disp > Ye - Ys + 1)
			Full_flag = 1;
		else
			Full_flag = 0;
	}

	if (Full_flag)
		gScroll_Info[num].flag = 1;
	else
	{
		if (gScroll_Info[num].scroll_en)
			gScroll_Info[num].flag = 1;
		else
			gScroll_Info[num].flag = 0;
	}

	gScroll_Info[num].len = len_disp;	// Total length of record cache content

	if (gScroll_Info[num].flag == 1)	// Left alignment:The first content display that triggers scrolling.
	{
		if (Screen_DIR == 0)
		{
			len_real = len_disp < (Xe - Xs + 1) ? len_disp : (Xe - Xs + 1);
			x_src = 0;
			y_src = offset_disp;
			w_real = len_real;
			h_real = H_Size;
			x_des = Xs;
			y_des = Ys + (Ye - Ys + 1 - H_Size) / 2;
		}
		else if (Screen_DIR == 1)
		{
			len_real = len_disp < (Ye - Ys + 1) ? len_disp : (Ye - Ys + 1);
			x_src = Lay_len - H_Size - offset_disp;
			y_src = 0;
			w_real = H_Size;
			h_real = len_real;
			x_des = Xe - H_Size + 1 - (Xe - Xs + 1 - H_Size) / 2;
			y_des = Ys;
		}
		else if (Screen_DIR == 2)
		{
			len_real = len_disp < (Xe - Xs + 1) ? len_disp : (Xe - Xs + 1);
			x_src = Lay_len - len_real;
			y_src = H_Maximum - H_Size - offset_disp;
			w_real = len_real;
			h_real = H_Size;
			x_des = Xe - len_real + 1;
			y_des = Ye - H_Size + 1 - (Ye - Ys + 1 - H_Size) / 2;
		}
		else if (Screen_DIR == 3)
		{
			len_real = len_disp < (Ye - Ys + 1) ? len_disp : (Ye - Ys + 1);
			x_src = offset_disp;
			y_src = H_Maximum - len_real;
			w_real = H_Size;
			h_real = len_real;
			x_des = Xs + (Xe - Xs + 1 - H_Size) / 2;
			y_des = Ye - len_real + 1;
		}
	}
	else if (gScroll_Info[num].flag == 0 && Alignment == 0) // Left alignment:The displayed content does not exceed
	{
		if (Screen_DIR == 0)
		{
			x_src = 0;
			y_src = offset_disp;
			w_real = len_disp;
			h_real = H_Size;
			x_des = Xs;
			y_des = Ys + (Ye - Ys + 1 - H_Size) / 2;
		}
		else if (Screen_DIR == 1)
		{
			x_src = Lay_len - H_Size - offset_disp;
			y_src = 0;
			w_real = H_Size;
			h_real = len_disp;
			x_des = Xe - H_Size + 1 - (Xe - Xs + 1 - H_Size) / 2;
			y_des = Ys;
		}
		else if (Screen_DIR == 2)
		{
			x_src = Lay_len - len_disp;
			y_src = H_Maximum - H_Size - offset_disp;
			w_real = len_disp;
			h_real = H_Size;
			x_des = Xe - len_disp + 1;
			y_des = Ye - H_Size + 1 - (Ye - Ys + 1 - H_Size) / 2;
		}
		else if (Screen_DIR == 3)
		{
			x_src = offset_disp;
			y_src = H_Maximum - len_disp;
			w_real = H_Size;
			h_real = len_disp;
			x_des = Xs + (Xe - Xs + 1 - H_Size) / 2;
			y_des = Ye - len_disp + 1;
		}
	}
	else if (gScroll_Info[num].flag == 0 && Alignment == 1) // Mid alignment:The displayed content does not exceed
	{
		len_real = Xe - Xs + 1;
		if (Screen_DIR == 0)
		{
			x_src = 0;
			y_src = offset_disp;
			w_real = len_disp;
			h_real = H_Size;
			x_des = Xs + ((Xe - Xs + 1) - len_disp) / 2;
			y_des = Ys + (Ye - Ys + 1 - H_Size) / 2;
		}
		else if (Screen_DIR == 1)
		{
			x_src = Lay_len - H_Size - offset_disp;
			y_src = 0;
			w_real = H_Size;
			h_real = len_disp;
			x_des = Xe - H_Size + 1 - (Xe - Xs + 1 - H_Size) / 2;
			y_des = Ys + ((Ye - Ys + 1) - len_disp) / 2;
		}
		else if (Screen_DIR == 2)
		{
			x_src = Lay_len - len_disp;
			y_src = H_Maximum - H_Size - offset_disp;
			w_real = len_disp;
			h_real = H_Size;
			x_des = Xs + ((Xe - Xs + 1) - len_disp) / 2;
			y_des = Ye - H_Size + 1 - (Ye - Ys + 1 - H_Size) / 2;
		}
		else if (Screen_DIR == 3)
		{
			x_src = offset_disp;
			y_src = H_Maximum - len_disp;
			w_real = H_Size;
			h_real = len_disp;
			x_des = Xs + (Xe - Xs + 1 - H_Size) / 2;
			y_des = Ys + ((Ye - Ys + 1) - len_disp) / 2;
		}
	}
	else if (gScroll_Info[num].flag == 0 && Alignment == 2) // Right alignment:The displayed content does not exceed
	{
		if (Screen_DIR == 0)
		{
			x_src = 0;
			y_src = offset_disp;
			w_real = len_disp;
			h_real = H_Size;
			x_des = Xe - len_disp + 1;
			y_des = Ys + (Ye - Ys + 1 - H_Size) / 2;
		}
		else if (Screen_DIR == 1)
		{
			x_src = Lay_len - H_Size - offset_disp;
			y_src = 0;
			w_real = H_Size;
			h_real = len_disp;
			x_des = Xe - H_Size + 1 - (Xe - Xs + 1 - H_Size) / 2;
			y_des = Ye - len_disp + 1;
		}
		else if (Screen_DIR == 2)
		{
			x_src = Lay_len - len_disp;
			y_src = H_Maximum - H_Size - offset_disp;
			w_real = len_disp;
			h_real = H_Size;
			x_des = Xs;
			y_des = Ye - H_Size + 1 - (Ye - Ys + 1 - H_Size) / 2;
		}
		else if (Screen_DIR == 3)
		{
			x_src = offset_disp;
			y_src = H_Maximum - len_disp;
			w_real = H_Size;
			h_real = len_disp;
			x_des = Xs + (Xe - Xs + 1 - H_Size) / 2; 
			y_des = Ys;
		}
	}

	if (pixel_format == 0)
	{
		LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF15, Lay_len, x_src, y_src,
										 ShowAddr, width, x_des, y_des,
										 Black, w_real, h_real);
	}
	else if (pixel_format == 1 || pixel_format == 2)
	{
		if (transparent == 1)
		{
			if (w_real >0 && h_real > 0)
			{
				Canvas_Image_Start_address(ShowAddr);
				Canvas_image_width(width);
				LT768_DrawSquare_Fill(x_des, y_des, x_des + w_real - 1, y_des + h_real - 1, BGColor);
			}
		}

		BTE_Pixel_16bpp_Alpha_Blending(ShowAddr, width, x_des, y_des,
									   LAY_BUFF15, Lay_len, x_src, y_src,
									   ShowAddr, width, x_des, y_des,
									   w_real, h_real);
	}
}

/***********************************************************************************
* Function Name	: text_scroll
* Description	: Get text_scroll info.
* Input			: - type: text_scroll info sources
				  - buff: text_scroll info
* Output		: None
* Return		: None
************************************************************************************/

void text_scroll(uint8_t type, uint8_t *buff)
{
	uint8_t Zk_id, w, h;
	uint8_t encode;
	uint8_t Alignment;
	uint8_t buff1[4] = {0};
	uint8_t transparent = 0, l = 0, t_d = 0, scroll_en = 0;
	uint16_t Len_window = 0, gap = 0;
	uint16_t p_add, p_len, pointer;
	uint16_t Zk_length;
	uint16_t xs, ys, xe, ye;
	uint32_t fcolor, bcolor;
	uint32_t zk_addr;

	if (type == 0)		// Instruction information source is configuration file
	{
		p_add = (buff[5] + (buff[6] << 8)) * 2;
		p_len = buff[15] + (buff[16] << 8);
		xs = buff[7] + (buff[8] << 8);
		ys = buff[9] + (buff[10] << 8);
		xe = buff[11] + (buff[12] << 8);
		ye = buff[13] + (buff[14] << 8);
		Zk_id = buff[17];
		w = buff[18];
		h = buff[19];
		encode = buff[20];
		Alignment = buff[21];
		scroll_en = buff[22];
		fcolor = buff[23] + (buff[24] << 8) + (buff[25] << 16);
		t_d = buff[26];
		bcolor = buff[27] + (buff[28] << 8) + (buff[29] << 16);
		gap = buff[30] + (buff[31] << 8);
		transparent = buff[32];
	}
	else if (type == 1) // Instruction information ource is variable spac
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		p_add = (var[pointer + 1] + (var[pointer + 2] << 8)) * 2;
		p_len = var[pointer + 11] + (var[pointer + 12] << 8);
		xs = var[pointer + 3] + (var[pointer + 4] << 8);
		ys = var[pointer + 5] + (var[pointer + 6] << 8);
		xe = var[pointer + 7] + (var[pointer + 8] << 8);
		ye = var[pointer + 9] + (var[pointer + 10] << 8);
		Zk_id = var[pointer + 13];
		w = var[pointer + 14];
		h = var[pointer + 15];
		encode = var[pointer + 16];
		Alignment = var[pointer + 17];
		scroll_en = var[pointer + 18];
		fcolor = var[pointer + 19] + (var[pointer + 20] << 8) + (var[pointer + 21] << 16);
		t_d = var[pointer + 22];
		bcolor = var[pointer + 23] + (var[pointer + 24] << 8) + (var[pointer + 25] << 16);
		gap = var[pointer + 26] + (var[pointer + 27] << 8);
		transparent = var[pointer + 28];
	}

	Flash_Read_UI(buff1, addr_index[2] + Zk_id * 8, 4);
	zk_addr = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);

	if (Pop_up_flag == 1) // Popup offset
	{
		xs += Pop_up_x;
		ys += Pop_up_y;
		xe += Pop_up_x;
		ye += Pop_up_y;
	}
	
	gScroll_Info[Scroll_num].x  = xs;
	gScroll_Info[Scroll_num].y  = ys;
	gScroll_Info[Scroll_num].xe = xe;
	gScroll_Info[Scroll_num].ye = ye;

	if (Screen_DIR == 0 || Screen_DIR == 2)
	{
		Len_window = xe - xs + 1;
		gScroll_Info[Scroll_num].w = Len_window;
		gScroll_Info[Scroll_num].h = h;
	}
	else if (Screen_DIR == 1 || Screen_DIR == 3)
	{
		Len_window = ye - ys + 1;
		gScroll_Info[Scroll_num].h = Len_window;
		gScroll_Info[Scroll_num].w = h;
	}
	gScroll_Info[Scroll_num].cnt = Len_window;
	gScroll_Info[Scroll_num].t_d = t_d;
	gScroll_Info[Scroll_num].t_c = 0;
	gScroll_Info[Scroll_num].gap = gap;
	gScroll_Info[Scroll_num].transparent = transparent;
	gScroll_Info[Scroll_num].scroll_en = scroll_en;
	gScroll_Info[Scroll_num].bcolor = bcolor;

	if (transparent == 0)
	{
		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, xs, ys,
							  MAIN_BUFF, LCD_XSIZE_TFT, xs, ys,
							  LAY_BUFF1, LCD_XSIZE_TFT, xs, ys,
							  0x0c, xe - xs + 1, ye - ys + 1);
	}
	else if (transparent == 1)
	{
		Canvas_Image_Start_address(LAY_BUFF1);
		Canvas_image_width(LCD_XSIZE_TFT);
		LT768_DrawSquare_Fill(xs, ys, xe, ye, bcolor);
	}

	// Caching or displaying text content
	LT_Print_zk_Scroll(encode, zk_addr, LAY_BUFF1, LCD_XSIZE_TFT,
					   w, h, Alignment, fcolor, bcolor, xs, ys, xe, ye,
					   0, 0, &var[p_add], p_len * 2, Scroll_num, transparent);

	// When the (display content+segment interval) width must be greater than the display box width
	if (gScroll_Info[Scroll_num].len + gScroll_Info[Scroll_num].gap < Len_window)
		gScroll_Info[Scroll_num].gap = Len_window - gScroll_Info[Scroll_num].len;

	LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, xs, ys,
						  LAY_BUFF1, LCD_XSIZE_TFT, xs, ys,
						  Display_Layer, LCD_XSIZE_TFT, xs, ys,
						  0x0c, xe - xs + 1, ye - ys + 1);
}

uint8_t text_scroll_Once(uint8_t num)
{
	uint16_t Lay_len = 0;
	uint16_t x_src = 0, y_src = 0, len_disp = 0, offset_disp = 0, len_real = 0, w_real = 0, h_real = 0;
	uint16_t x_des = 0, y_des = 0;
	uint16_t Len_window = 0, H_Maximum = 0;
	
	if (gScroll_Info[num].transparent == 0)
	{
		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gScroll_Info[num].x, gScroll_Info[num].y,
							  MAIN_BUFF, LCD_XSIZE_TFT, gScroll_Info[num].x, gScroll_Info[num].y,
							  LAY_BUFF1, LCD_XSIZE_TFT, gScroll_Info[num].x, gScroll_Info[num].y,
							  0x0c, gScroll_Info[num].xe - gScroll_Info[num].x + 1, gScroll_Info[num].ye - gScroll_Info[num].y + 1);
	}
	else if (gScroll_Info[num].transparent == 1)
	{
		Canvas_Image_Start_address(LAY_BUFF1);
		Canvas_image_width(LCD_XSIZE_TFT);
		LT768_DrawSquare_Fill(gScroll_Info[num].x, gScroll_Info[num].y, gScroll_Info[num].xe, gScroll_Info[num].ye, gScroll_Info[num].bcolor);
	}
	/* In the keyboard input interface*/
	if (DioKb_flag == 1)		
	{
		if (!(gScroll_Info[num].x > gDio_Info[Dio_num].Xe || gScroll_Info[num].xe < gDio_Info[Dio_num].Xs || gScroll_Info[num].y > gDio_Info[Dio_num].Ye || gScroll_Info[num].ye  < gDio_Info[Dio_num].Ys))
		{
			return 0;
		}
	}
	else if (AioKb_flag == 1)
	{
		if (!(gScroll_Info[num].x > gAio_Info[Aio_num].Xe || gScroll_Info[num].xe < gAio_Info[Aio_num].Xs || gScroll_Info[num].y > gAio_Info[Aio_num].Ye || gScroll_Info[num].ye  < gAio_Info[Aio_num].Ys))
		{
			return 0;
		}
	}
	else if (GBKioKb_flag == 1)
	{
		if (!(gScroll_Info[num].x > gGBKio_Info[GBKio_num].Xe || gScroll_Info[num].xe < gGBKio_Info[GBKio_num].Xs || gScroll_Info[num].y > gGBKio_Info[GBKio_num].Ye || gScroll_Info[num].ye  < gGBKio_Info[GBKio_num].Ys))
		{
			return 0;
		}
	}

	if (Screen_DIR == 0)
	{
		Lay_len = LCD_XSIZE_TFT * 2;
		Len_window = gScroll_Info[num].w;
	}
	else if (Screen_DIR == 1)
	{
		Lay_len = LCD_XSIZE_TFT;
		Len_window = gScroll_Info[num].h;
	}
	else if (Screen_DIR == 2)
	{
		Lay_len = LCD_XSIZE_TFT * 2;
		Len_window = gScroll_Info[num].w;
		if (TFT_bitcolor == 0)
			H_Maximum = 2 * LCD_YSIZE_TFT;
		else if (TFT_bitcolor == 1)
			H_Maximum = 3 * LCD_YSIZE_TFT / 4;
	}
	else if (Screen_DIR == 3)
	{
		Lay_len = LCD_XSIZE_TFT;
		Len_window = gScroll_Info[num].h;
		if (TFT_bitcolor == 0)
			H_Maximum = 2 * LCD_YSIZE_TFT;
		else if (TFT_bitcolor == 1)
			H_Maximum = 3 * LCD_YSIZE_TFT / 2;
	}

	if (gScroll_Info[num].len > Len_window) // The displayed content is larger than the display box width
	{
		// The cnt is the relative pixel offset value of the text relative to the display box from right to left
		if (gScroll_Info[num].cnt <= Len_window)
		{
			if (Screen_DIR == 0)
			{
				x_src = 0;
				y_src = gScroll_Info[num].y_offset;
				w_real = gScroll_Info[num].cnt;
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x + (Len_window - gScroll_Info[num].cnt);
				y_des = gScroll_Info[num].y + (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 1)
			{
				x_src = Lay_len - gScroll_Info[num].w - gScroll_Info[num].y_offset;
				y_src = 0;
				w_real = gScroll_Info[num].w;
				h_real = gScroll_Info[num].cnt;
				x_des = gScroll_Info[num].xe - w_real + 1 - (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].ye - h_real + 1;
			}
			else if (Screen_DIR == 2)
			{
				x_src = Lay_len - gScroll_Info[num].cnt;
				y_src = H_Maximum - gScroll_Info[num].h - gScroll_Info[num].y_offset;
				w_real = gScroll_Info[num].cnt;
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x;
				y_des = gScroll_Info[num].ye - h_real + 1 - (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 3)
			{
				x_src = gScroll_Info[num].y_offset;
				y_src = H_Maximum - gScroll_Info[num].cnt;
				w_real = gScroll_Info[num].w;
				h_real = gScroll_Info[num].cnt;
				x_des = gScroll_Info[num].x + (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].y;
			}
		}
		else if (gScroll_Info[num].cnt <= gScroll_Info[num].len)
		{
			if (Screen_DIR == 0)
			{
				x_src = gScroll_Info[num].cnt - Len_window;
				y_src = gScroll_Info[num].y_offset;
				w_real = Len_window;
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x;
				y_des = gScroll_Info[num].y + (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 1)
			{
				x_src = Lay_len - gScroll_Info[num].w - gScroll_Info[num].y_offset;
				y_src = gScroll_Info[num].cnt - Len_window;
				w_real = gScroll_Info[num].w;
				h_real = Len_window;
				x_des = gScroll_Info[num].xe - w_real + 1 - (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].y;
			}
			else if (Screen_DIR == 2)
			{
				x_src = Lay_len - gScroll_Info[num].cnt;
				y_src = H_Maximum - gScroll_Info[num].h - gScroll_Info[num].y_offset;
				w_real = Len_window;
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x;
				y_des = gScroll_Info[num].ye - h_real + 1 - (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 3)
			{
				x_src = gScroll_Info[num].y_offset;
				y_src = H_Maximum - gScroll_Info[num].cnt;
				w_real = gScroll_Info[num].w;
				h_real = Len_window;
				x_des = gScroll_Info[num].x + (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].y;
			}
		}
		else if (gScroll_Info[num].cnt > gScroll_Info[num].len)
		{
			if (Screen_DIR == 0)
			{
				x_src = gScroll_Info[num].cnt - Len_window;
				y_src = gScroll_Info[num].y_offset;
				w_real = Len_window - (gScroll_Info[num].cnt - gScroll_Info[num].len);
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x;
				y_des = gScroll_Info[num].y + (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 1)
			{
				x_src = Lay_len - gScroll_Info[num].w - gScroll_Info[num].y_offset;
				y_src = gScroll_Info[num].cnt - Len_window;
				w_real = gScroll_Info[num].w;
				h_real = Len_window - (gScroll_Info[num].cnt - gScroll_Info[num].len);
				x_des = gScroll_Info[num].xe - w_real + 1 - (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].y;
			}
			else if (Screen_DIR == 2)
			{
				x_src = Lay_len - gScroll_Info[num].len;
				y_src = H_Maximum - gScroll_Info[num].h - gScroll_Info[num].y_offset;
				w_real = Len_window - (gScroll_Info[num].cnt - gScroll_Info[num].len);
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x + (gScroll_Info[num].cnt - gScroll_Info[num].len);
				y_des = gScroll_Info[num].ye - h_real + 1 - (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 3)
			{
				x_src = gScroll_Info[num].y_offset;
				y_src = H_Maximum - gScroll_Info[num].len;
				w_real = gScroll_Info[num].w;
				h_real = Len_window - (gScroll_Info[num].cnt - gScroll_Info[num].len);
				x_des = gScroll_Info[num].x + (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].y + (gScroll_Info[num].cnt - gScroll_Info[num].len) - 1;
			}
		}

		// First paragraph copy
		if (gScroll_Info[num].pixel_format == 0)
		{
			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF15, Lay_len, x_src, y_src,
											 LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
											 Black, w_real, h_real);
		}
		else if (gScroll_Info[num].pixel_format == 1 || gScroll_Info[num].pixel_format == 2)
		{
//			if (gScroll_Info[num].transparent == 1)
//			{
//				Canvas_Image_Start_address(LAY_BUFF1);
//				Canvas_image_width(LCD_XSIZE_TFT);
//				LT768_DrawSquare_Fill(x_des, y_des, x_des + w_real - 1, y_des + h_real - 1, gScroll_Info[num].bcolor);
//			}
			BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
										   LAY_BUFF15, Lay_len, x_src, y_src,
										   LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
										   w_real, h_real);
		}

		/* Second paragraph copy: here is the tail of the next paragraph of text to connect to the current paragraph of text,
			and the gap value is the pixel interval between */
		if (gScroll_Info[num].cnt > gScroll_Info[num].len + gScroll_Info[num].gap)
		{
			if (Screen_DIR == 0)
			{
				x_src = 0;
				y_src = gScroll_Info[num].y_offset;
				w_real = gScroll_Info[num].cnt - (gScroll_Info[num].len + gScroll_Info[num].gap);
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x + (Len_window + gScroll_Info[num].len + gScroll_Info[num].gap - gScroll_Info[num].cnt);
				y_des = gScroll_Info[num].y + (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 1)
			{
				x_src = Lay_len - gScroll_Info[num].w - gScroll_Info[num].y_offset;
				y_src = 0;
				w_real = gScroll_Info[num].w;
				h_real = gScroll_Info[num].cnt - (gScroll_Info[num].len + gScroll_Info[num].gap);
				x_des = gScroll_Info[num].xe - w_real + 1 - (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].ye - h_real + 1;
			}
			else if (Screen_DIR == 2)
			{
				x_src = Lay_len - (gScroll_Info[num].cnt - (gScroll_Info[num].len + gScroll_Info[num].gap));
				y_src = H_Maximum - gScroll_Info[num].h - gScroll_Info[num].y_offset;
				w_real = gScroll_Info[num].cnt - (gScroll_Info[num].len + gScroll_Info[num].gap);
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x;
				y_des = gScroll_Info[num].ye - h_real + 1 - (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 3)
			{
				x_src = gScroll_Info[num].y_offset;
				y_src = H_Maximum - (gScroll_Info[num].cnt - (gScroll_Info[num].len + gScroll_Info[num].gap));
				w_real = gScroll_Info[num].w;
				h_real = gScroll_Info[num].cnt - (gScroll_Info[num].len + gScroll_Info[num].gap);
				x_des = gScroll_Info[num].x + (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].y;
			}
			if (gScroll_Info[num].pixel_format == 0)
			{
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF15, Lay_len, x_src, y_src,
												 LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
												 Black, w_real, h_real);
			}
			else if (gScroll_Info[num].pixel_format == 1 || gScroll_Info[num].pixel_format == 2)
			{
				if (gScroll_Info[num].transparent == 1)
				{
					Canvas_Image_Start_address(LAY_BUFF1);
					Canvas_image_width(LCD_XSIZE_TFT);
					LT768_DrawSquare_Fill(x_des, y_des, x_des + w_real - 1, y_des + h_real - 1, gScroll_Info[num].bcolor);
				}
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
											   LAY_BUFF15, Lay_len, x_src, y_src,
											   LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
											   w_real, h_real);
			}
		}
	}
	else // The display content is smaller than the display box width
	{
		// The cnt is the relative pixel offset value of the text relative to the display box from right to left
		if (gScroll_Info[num].cnt <= gScroll_Info[num].len)
		{
			if (Screen_DIR == 0)
			{
				x_src = 0;
				y_src = gScroll_Info[num].y_offset;
				w_real = gScroll_Info[num].cnt;
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x + (Len_window - gScroll_Info[num].cnt);
				y_des = gScroll_Info[num].y + (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 1)
			{
				x_src = Lay_len - gScroll_Info[num].w - gScroll_Info[num].y_offset;
				y_src = 0;
				w_real = gScroll_Info[num].w;
				h_real = gScroll_Info[num].cnt;
				x_des = gScroll_Info[num].xe - w_real + 1 - (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].ye - h_real + 1;
			}
			else if (Screen_DIR == 2)
			{
				x_src = Lay_len - gScroll_Info[num].cnt;
				y_src = H_Maximum - gScroll_Info[num].h - gScroll_Info[num].y_offset;
				w_real = gScroll_Info[num].cnt;
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x;
				y_des = gScroll_Info[num].ye - h_real + 1 - (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 3)
			{
				x_src = gScroll_Info[num].y_offset;
				y_src = H_Maximum - gScroll_Info[num].cnt;
				w_real = gScroll_Info[num].w;
				h_real = gScroll_Info[num].cnt;
				x_des = gScroll_Info[num].x + (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].y;
			}
		}
		else if (gScroll_Info[num].cnt <= Len_window)
		{
			if (Screen_DIR == 0)
			{
				x_src = 0;
				y_src = gScroll_Info[num].y_offset;
				w_real = gScroll_Info[num].len;
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x + (Len_window - gScroll_Info[num].cnt);
				y_des = gScroll_Info[num].y + (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 1)
			{
				x_src = Lay_len - gScroll_Info[num].w - gScroll_Info[num].y_offset;
				y_src = 0;
				w_real = gScroll_Info[num].w;
				h_real = gScroll_Info[num].len;
				x_des = gScroll_Info[num].xe - w_real + 1 - (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].ye - gScroll_Info[num].cnt + 1;
			}
			else if (Screen_DIR == 2)
			{
				x_src = Lay_len - gScroll_Info[num].len;
				y_src = H_Maximum - gScroll_Info[num].h - gScroll_Info[num].y_offset;
				w_real = gScroll_Info[num].len;
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x + (gScroll_Info[num].cnt - gScroll_Info[num].len);
				y_des = gScroll_Info[num].ye - h_real + 1 - (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 3)
			{
				x_src = gScroll_Info[num].y_offset;
				y_src = H_Maximum - gScroll_Info[num].len;
				w_real = gScroll_Info[num].w;
				h_real = gScroll_Info[num].len;
				x_des = gScroll_Info[num].x + (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].y + (gScroll_Info[num].cnt - gScroll_Info[num].len) - 1;
			}
		}
		else if (gScroll_Info[num].cnt > Len_window)
		{
			if (Screen_DIR == 0)
			{
				x_src = gScroll_Info[num].cnt - Len_window;
				y_src = gScroll_Info[num].y_offset;
				w_real = gScroll_Info[num].len - (gScroll_Info[num].cnt - Len_window);
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x;
				y_des = gScroll_Info[num].y + (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 1)
			{
				x_src = Lay_len - gScroll_Info[num].w - gScroll_Info[num].y_offset;
				y_src = gScroll_Info[num].cnt - Len_window;
				w_real = gScroll_Info[num].w;
				h_real = gScroll_Info[num].len - (gScroll_Info[num].cnt - Len_window);
				x_des = gScroll_Info[num].xe - w_real + 1 - (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].y;
			}
			else if (Screen_DIR == 2)
			{
				x_src = Lay_len - gScroll_Info[num].len;
				y_src = H_Maximum - gScroll_Info[num].h - gScroll_Info[num].y_offset;
				w_real = gScroll_Info[num].len - (gScroll_Info[num].cnt - Len_window);
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x + (gScroll_Info[num].cnt - gScroll_Info[num].len);
				y_des = gScroll_Info[num].ye - h_real + 1 - (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 3)
			{
				x_src = gScroll_Info[num].y_offset;
				y_src = H_Maximum - gScroll_Info[num].len;
				w_real = gScroll_Info[num].w;
				h_real = gScroll_Info[num].len - (gScroll_Info[num].cnt - Len_window);
				x_des = gScroll_Info[num].x + (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].y + (gScroll_Info[num].cnt - gScroll_Info[num].len) - 1;
			}
		}
		// First paragraph copy
		if (gScroll_Info[num].pixel_format == 0)
		{
			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF15, Lay_len, x_src, y_src,
											 LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
											 Black, w_real, h_real);
		}
		else if (gScroll_Info[num].pixel_format == 1 || gScroll_Info[num].pixel_format == 2)
		{
			if (gScroll_Info[num].transparent == 1)
			{
//				if (w_real > 0 && h_real > 0)
//				{
//					Canvas_Image_Start_address(LAY_BUFF1);
//					Canvas_image_width(LCD_XSIZE_TFT);
//					LT768_DrawSquare_Fill(x_des, y_des, x_des + w_real, y_des + h_real - 1, gScroll_Info[num].bcolor);
//				}
			}
			BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
										   LAY_BUFF15, Lay_len, x_src, y_src,
										   LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
										   w_real, h_real);

		}

		/* Second paragraph copy: here is the tail of the next paragraph of text to connect to the current paragraph of text,
			and the gap value is the pixel interval between */
		if (gScroll_Info[num].cnt > gScroll_Info[num].len + gScroll_Info[num].gap)
		{
			if (Screen_DIR == 0)
			{
				x_src = 0;
				y_src = gScroll_Info[num].y_offset;
				w_real = gScroll_Info[num].cnt - (gScroll_Info[num].len + gScroll_Info[num].gap);
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x + ((gScroll_Info[num].w + gScroll_Info[num].len + gScroll_Info[num].gap) - gScroll_Info[num].cnt);
				y_des = gScroll_Info[num].y + (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 1)
			{
				x_src = Lay_len - gScroll_Info[num].w - gScroll_Info[num].y_offset;
				y_src = 0;
				w_real = gScroll_Info[num].w;
				h_real = gScroll_Info[num].cnt - (gScroll_Info[num].len + gScroll_Info[num].gap);
				x_des = gScroll_Info[num].xe - w_real + 1 - (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].ye - h_real + 1;
			}
			else if (Screen_DIR == 2)
			{
				x_src = Lay_len - (gScroll_Info[num].cnt - (gScroll_Info[num].len + gScroll_Info[num].gap));
				y_src = H_Maximum - gScroll_Info[num].h - gScroll_Info[num].y_offset;
				w_real = gScroll_Info[num].cnt - (gScroll_Info[num].len + gScroll_Info[num].gap);
				h_real = gScroll_Info[num].h;
				x_des = gScroll_Info[num].x;
				y_des = gScroll_Info[num].ye - h_real + 1 - (gScroll_Info[num].ye - gScroll_Info[num].y + 1 - gScroll_Info[num].h) / 2;
			}
			else if (Screen_DIR == 3)
			{
				x_src = gScroll_Info[num].y_offset;
				y_src = H_Maximum - (gScroll_Info[num].cnt - (gScroll_Info[num].len + gScroll_Info[num].gap));
				w_real = gScroll_Info[num].w;
				h_real = gScroll_Info[num].cnt - (gScroll_Info[num].len + gScroll_Info[num].gap);
				x_des = gScroll_Info[num].x + (gScroll_Info[num].xe - gScroll_Info[num].x + 1 - gScroll_Info[num].w) / 2;
				y_des = gScroll_Info[num].y;
			}
			if (gScroll_Info[num].pixel_format == 0)
			{
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF15, Lay_len, x_src, y_src,
												 LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
												 Black, w_real, h_real);
			}
			else if (gScroll_Info[num].pixel_format == 1 || gScroll_Info[num].pixel_format == 2)
			{
//				if (gScroll_Info[num].transparent == 1)
//				{
//					Canvas_Image_Start_address(LAY_BUFF1);
//					Canvas_image_width(LCD_XSIZE_TFT);
//					LT768_DrawSquare_Fill(x_des, y_des, x_des + w_real - 1, y_des + h_real - 1, gScroll_Info[num].bcolor);
//				}
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
											   LAY_BUFF15, Lay_len, x_src, y_src,
											   LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
											   w_real, h_real);
			}
		}
	}

	LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, gScroll_Info[num].x, gScroll_Info[num].y,
						  LAY_BUFF1, LCD_XSIZE_TFT, gScroll_Info[num].x, gScroll_Info[num].y,
						  Display_Layer, LCD_XSIZE_TFT, gScroll_Info[num].x, gScroll_Info[num].y,
						  0x0c, gScroll_Info[num].xe - gScroll_Info[num].x + 1, gScroll_Info[num].ye - gScroll_Info[num].y + 1);


	
	gScroll_Info[num].cnt += 1;
	if (gScroll_Info[num].cnt >= (gScroll_Info[num].len + Len_window))
		gScroll_Info[num].cnt = Len_window - gScroll_Info[num].gap;
}

void scroll_text(void)
{
	if (Scroll_num && Scroll_flag)
	{
		Scroll_flag = 0;
		for (uint8_t i = 0; i < Scroll_num; i++)
			gScroll_Info[i].t_c++;
		for (uint8_t i = 0; i < Scroll_num; i++)
		{
			if ((gScroll_Info[i].t_c >= gScroll_Info[i].t_d) && gScroll_Info[i].flag)
			{
				text_scroll_Once(i);
				gScroll_Info[i].t_c = 0;
			}
		}
	}
}
/**********************************/

/***********************************************************************************
* Function Name	: progress_bar
* Description	: Display progress_bar.
* Input			: - type: progress_bar info sources
				  - buff: progress_bar info
* Output		: None
* Return		: None
************************************************************************************/
void progress_bar(uint8_t type, uint8_t *buff)
{
	uint8_t buff1[12] = {0}, dir;
	int16_t sum = 0;
	uint16_t canvas_w = 0;
	uint16_t w, h, dw1 = 0, dh1 = 0, bgw, bgh;
	uint16_t pointer, p_add, x2, y2, x1, y1, bar_id, bg_id;
	int16_t Vmax, Vmin;

	if (type == 0) // Instruction information source is configuration file
	{
		p_add = (buff[5] + (buff[6] << 8));
		x2 = buff[7] + (buff[8] << 8);
		y2 = buff[9] + (buff[10] << 8);
		bar_id = buff[11] + (buff[12] << 8);
		dir = buff[13];
		Vmax = buff[14] + (buff[15] << 8);
		Vmin = buff[16] + (buff[17] << 8);
		x1 = buff[18] + (buff[19] << 8);
		y1 = buff[20] + (buff[21] << 8);
		bg_id = buff[22] + (buff[23] << 8);
	}
	else if (type == 1) // Instruction information ource is variable space
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		p_add = (var[pointer + 1] + (var[pointer + 2] << 8));
		x2 = var[pointer + 3] + (var[pointer + 4] << 8);
		y2 = var[pointer + 5] + (var[pointer + 6] << 8);
		bar_id = var[pointer + 7] + (var[pointer + 8] << 8);
		dir = var[pointer + 9];
		Vmax = var[pointer + 10] + (var[pointer + 11] << 8);
		Vmin = var[pointer + 12] + (var[pointer + 13] << 8);
		x1 = var[pointer + 14] + (var[pointer + 15] << 8);
		y1 = var[pointer + 16] + (var[pointer + 17] << 8);
		bg_id = var[pointer + 18] + (var[pointer + 19] << 8);
	}

	if (Pop_up_flag == 1) // Popup offset
	{
		x1 += Pop_up_x;
		y1 += Pop_up_y;
		x2 += Pop_up_x;
		y2 += Pop_up_y;
	}

	if (bg_id != 0xFFFF)
	{
		Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * bg_id, 12);
		pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
		bgw = buff1[4] + (buff1[5] << 8);
		bgh = buff1[6] + (buff1[7] << 8);
		pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
		pic_flag = buff1[11];
		canvas_w = Get_Canvas_Width(bgw);
		Canvas_Image_Start_address(LAY_BUFF1);
		Canvas_image_width(canvas_w);

		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x1, y1,
							  MAIN_BUFF, LCD_XSIZE_TFT, x1, y1,
							  LAY_BUFF2, LCD_XSIZE_TFT, x1, y1,
							  0x0c, bgw, bgh);

		LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
											bgw, bgh,
											bgw, pic_add,
											LAY_BUFF1, canvas_w);

		if (pic_flag == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
								  LAY_BUFF1, canvas_w, 0, 0,
								  LAY_BUFF2, LCD_XSIZE_TFT, x1, y1,
								  0x0c, bgw, bgh);
		}
		else if (pic_flag == 1)
		{
			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
											 LAY_BUFF2, LCD_XSIZE_TFT, x1, y1,
											 Black, bgw, bgh);
		}
		else if (pic_flag == 2)
		{
			BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, x1, y1,
										   LAY_BUFF1, canvas_w, 0, 0,
										   LAY_BUFF2, LCD_XSIZE_TFT, x1, y1,
										   bgw, bgh);
		}
	}

	if (bar_id != 0xFFFF)
	{
		Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * bar_id, 12);
		pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
		pic_w = buff1[4] + (buff1[5] << 8);
		pic_h = buff1[6] + (buff1[7] << 8);
		pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
		pic_flag = buff1[11];
		canvas_w = Get_Canvas_Width(pic_w);
		Canvas_Image_Start_address(LAY_BUFF1);
		Canvas_image_width(canvas_w);

		dw1 = pic_w;
		dh1 = pic_h;
		
		if (bg_id == 0xFFFF)
		{
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x2, y2,
								  MAIN_BUFF, LCD_XSIZE_TFT, x2, y2,
								  LAY_BUFF2, LCD_XSIZE_TFT, x2, y2,
								  0x0c, dw1, dh1);
		}
		LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
											pic_w, pic_h,
											pic_w, pic_add,
											LAY_BUFF1, canvas_w);
	}

	switch (dir)
	{
	case 0: // Left to right
		sum = (var[p_add * 2] << 8) + var[p_add * 2 + 1];
		if (sum > Vmax)
			sum = Vmax;
		else if (sum < Vmin)
			sum = Vmin;

		w = dw1 * (sum - Vmin) / (Vmax - Vmin);

		if (pic_flag == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
								  LAY_BUFF1, canvas_w, 0, 0,
								  LAY_BUFF2, LCD_XSIZE_TFT, x2, y2,
								  0x0c, w, pic_h);
		}
		else if (pic_flag == 1)
		{
			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
											 LAY_BUFF2, LCD_XSIZE_TFT, x2, y2,
											 Black, w, pic_h);
		}
		else if (pic_flag == 2)
		{
			BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF2, LCD_XSIZE_TFT, x2, y2,
										   LAY_BUFF1, canvas_w, 0, 0,
										   LAY_BUFF2, LCD_XSIZE_TFT, x2, y2,
										   w, pic_h);
		}
		break;
	case 1: // Right to left

		sum = (var[p_add * 2] << 8) + var[p_add * 2 + 1];
		if (sum > Vmax)
			sum = Vmax;
		else if (sum < Vmin)
			sum = Vmin;

		w = dw1 * (sum - Vmin) / (Vmax - Vmin);

		if (pic_flag == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, dw1 - w, 0,
								  LAY_BUFF1, canvas_w, dw1 - w, 0,
								  LAY_BUFF2, LCD_XSIZE_TFT, x2 + dw1 - w, y2,
								  0x0c, w, pic_h);
		}
		else if (pic_flag == 1)
		{
			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, dw1 - w, 0,
											 LAY_BUFF2, LCD_XSIZE_TFT, x2 + dw1 - w, y2,
											 Black, w, pic_h);
		}
		else if (pic_flag == 2)
		{
			BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF2, LCD_XSIZE_TFT, x2 + dw1 - w, y2,
										   LAY_BUFF1, canvas_w, dw1 - w, 0,
										   LAY_BUFF2, LCD_XSIZE_TFT, x2 + dw1 - w, y2,
										   w, pic_h);
		}

		break;
	case 2: // top to bottom

		sum = (var[p_add * 2] << 8) + var[p_add * 2 + 1];
		if (sum > Vmax)
			sum = Vmax;
		else if (sum < Vmin)
			sum = Vmin;

		h = dh1 * (sum - Vmin) / (Vmax - Vmin);

		if (pic_flag == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
								  LAY_BUFF1, canvas_w, 0, 0,
								  LAY_BUFF2, LCD_XSIZE_TFT, x2, y2,
								  0x0c, pic_w, h);
		}
		else if (pic_flag == 1)
		{
			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
											 LAY_BUFF2, LCD_XSIZE_TFT, x2, y2,
											 Black, pic_w, h);
		}
		else if (pic_flag == 2)
		{

			BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF2, LCD_XSIZE_TFT, x2, y2,
										   LAY_BUFF1, canvas_w, 0, 0,
										   LAY_BUFF2, LCD_XSIZE_TFT, x2, y2,
										   pic_w, h);
		}

		break;
	case 3: // Down to up

		sum = (var[p_add * 2] << 8) + var[p_add * 2 + 1];
		if (sum > Vmax)
			sum = Vmax;
		else if (sum < Vmin)
			sum = Vmin;

		h = dh1 * (sum - Vmin) / (Vmax - Vmin);

		if (pic_flag == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, dh1 - h,
								  LAY_BUFF1, canvas_w, 0, dh1 - h,
								  LAY_BUFF2, LCD_XSIZE_TFT, x2, y2 + dh1 - h,
								  0x0c, pic_w, h);
		}
		else if (pic_flag == 1)
		{
			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, dh1 - h,
											 LAY_BUFF2, LCD_XSIZE_TFT, x2, y2 + dh1 - h,
											 Black, pic_w, h);
		}
		else if (pic_flag == 2)
		{

			BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF2, LCD_XSIZE_TFT, x2, y2 + dh1 - h,
										   LAY_BUFF1, canvas_w, 0, dh1 - h,
										   LAY_BUFF2, LCD_XSIZE_TFT, x2, y2 + dh1 - h,
										   pic_w, h);
		}
		break;
	default:
		break;
	}
	if (bg_id != 0xFFFF)
	{
		LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, x1, y1,
							  LAY_BUFF2, LCD_XSIZE_TFT, x1, y1,
							  Display_Layer, LCD_XSIZE_TFT, x1, y1,
							  0x0c, bgw, bgh);
	}
	else if (bar_id != 0xFFFF)
	{
		LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, x2, y2,
							  LAY_BUFF2, LCD_XSIZE_TFT, x2, y2,
							  Display_Layer, LCD_XSIZE_TFT, x2, y2,
							  0x0c, dw1, dh1);
	}
}
/************************************/

/***********************************************************************************
* Function Name	: Circular_progress_bar
* Description	: Display ring progress bar.
* Input			: - type: ring info sources
				  - buff: ring info
* Output		: None
* Return		: None
************************************************************************************/
void Circular_progress_bar(uint8_t type, uint8_t *buff)
{
	uint8_t I_num, D_num, Alig, zk_id, flag, n = 0, i = 0, kk = 0, flag1 = 0;
	uint8_t buff1[16],Symbol = 0;
	uint16_t temp, xd = 0;
	uint16_t pointer, p_add, x, y, dx, dy, fornt_id, bg_id, Sangle, Eangle;
	uint16_t S_id, E_id;
	int16_t Vmax, Vmin,angle,tt,value1;
	int32_t value, ll;
	uint16_t xR, yR;
	uint16_t num_w, dot_w, h, canvas_numw, canvas_dotw, canvas_w1;
	uint16_t x_src = 0, y_src = 0;
	uint16_t w_des, h_des, x_des, y_des, w0, h0;
	uint32_t color, zk_addr;

	if (type == 0)		// Instruction information source is configuration file
	{
		p_add = (buff[5] + (buff[6] << 8));
		x = buff[7] + (buff[8] << 8);
		y = buff[9] + (buff[10] << 8);
		fornt_id = buff[11] + (buff[12] << 8);
		bg_id = buff[13] + (buff[14] << 8);
		Vmin = buff[15] + (buff[16] << 8);
		Vmax = buff[17] + (buff[18] << 8);
		Sangle = buff[19] + (buff[20] << 8);
		Eangle = buff[21] + (buff[22] << 8);
		dx = buff[23] + (buff[24] << 8);
		dy = buff[25] + (buff[26] << 8);
		I_num = buff[27];
		D_num = buff[28];
		Alig = buff[29];
		zk_id = buff[30];
		color = buff[31] + (buff[32] << 8) + (buff[33] << 16);
		S_id = buff[34] + (buff[35] << 8);
		E_id = buff[36] + (buff[37] << 8);
		flag = buff[38];
	}
	else if (type == 1) // Instruction information ource is variable space
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		p_add = (var[pointer + 1] + (var[pointer + 2] << 8));
		x = var[pointer + 3] + (var[pointer + 4] << 8);
		y = var[pointer + 5] + (var[pointer + 6] << 8);
		fornt_id = var[pointer + 7] + (var[pointer + 8] << 8);
		bg_id = var[pointer + 9] + (var[pointer + 10] << 8);
		Vmin = var[pointer + 11] + (var[pointer + 12] << 8);
		Vmax = var[pointer + 13] + (var[pointer + 14] << 8);
		Sangle = var[pointer + 15] + (var[pointer + 16] << 8);
		Eangle = var[pointer + 17] + (var[pointer + 18] << 8);
		dx = var[pointer + 19] + (var[pointer + 20] << 8);
		dy = var[pointer + 21] + (var[pointer + 22] << 8);
		I_num = var[pointer + 23];
		D_num = var[pointer + 24];
		Alig = var[pointer + 25];
		zk_id = var[pointer + 26];
		color = var[pointer + 27] + (var[pointer + 28] << 8) + (var[pointer + 29] << 16);
		S_id = var[pointer + 30] + (var[pointer + 31] << 8);
		E_id = var[pointer + 32] + (var[pointer + 33] << 8);
		flag = var[pointer + 34];
	}
	if (Pop_up_flag == 1) // Popup offset
	{
		x += Pop_up_x;
		y += Pop_up_y;
		dx += Pop_up_x;
		dy += Pop_up_y;
	}

	if (fornt_id + 1 == bg_id) // Picture continuous
	{
		if (fornt_id <= addr_index[11])
		{
			Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * fornt_id, 12);

			pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
			pic_w = buff1[4] + (buff1[5] << 8);
			pic_h = buff1[6] + (buff1[7] << 8);
			pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
			pic_flag = buff1[11];

			if (pic_len != 0 && pic_flag != 0xFF)
			{
				canvas_w1 = Get_Canvas_Width(pic_w);
				Canvas_Image_Start_address(LAY_BUFF3);
				Canvas_image_width(canvas_w1);

				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													pic_w, pic_h * 2,
													pic_w, pic_add,
													LAY_BUFF3, canvas_w1);
			}
		}
	}
	else // Picture discontinuity
	{
		if (fornt_id <= addr_index[11])
		{
			Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * fornt_id, 12);
			pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
			pic_w = buff1[4] + (buff1[5] << 8);
			pic_h = buff1[6] + (buff1[7] << 8);
			pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
			pic_flag = buff1[11];

			if (pic_len != 0 && pic_flag != 0xFF)
			{
				canvas_w1 = Get_Canvas_Width(pic_w);
				Canvas_Image_Start_address(LAY_BUFF3);
				Canvas_image_width(canvas_w1);

				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													pic_w, pic_h,
													pic_w, pic_add,
													LAY_BUFF3, canvas_w1);
			}
		}
		if (bg_id <= addr_index[11])
		{
			Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * bg_id, 12);
			pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
			pic_w = buff1[4] + (buff1[5] << 8);
			pic_h = buff1[6] + (buff1[7] << 8);
			pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
			pic_flag = buff1[11];

			if (pic_len != 0 && pic_flag != 0xFF)
			{
				canvas_w1 = Get_Canvas_Width(pic_w);
				Canvas_Image_Start_address(LAY_BUFF3);
				Canvas_image_width(canvas_w1);

				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, pic_h,
													pic_w, pic_h,
													pic_w, pic_add,
													LAY_BUFF3, canvas_w1);
			}
		}
	}
	xR = x + pic_w / 2;
	yR = y + pic_h / 2;

	if (pic_flag == 0)
	{
		LT768_BTE_Memory_Copy(LAY_BUFF3, canvas_w1, 0, pic_h,
							  LAY_BUFF3, canvas_w1, 0, pic_h,
							  LAY_BUFF1, LCD_XSIZE_TFT, x, y,
							  0x0c, pic_w, pic_h);

		LT768_BTE_Memory_Copy(LAY_BUFF3, canvas_w1, 0, 0,
							  LAY_BUFF3, canvas_w1, 0, 0,
							  LAY_BUFF2, LCD_XSIZE_TFT, x, y,
							  0x0c, pic_w, pic_h);
	}
	else if (pic_flag == 1)
	{
		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x, y,
							  MAIN_BUFF, LCD_XSIZE_TFT, x, y,
							  LAY_BUFF1, LCD_XSIZE_TFT, x, y,
							  0x0c, pic_w, pic_h);

		LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF3, canvas_w1, 0, pic_h,
										 LAY_BUFF1, LCD_XSIZE_TFT, x, y,
										 Black, pic_w, pic_h);

		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x, y,
							  MAIN_BUFF, LCD_XSIZE_TFT, x, y,
							  LAY_BUFF2, LCD_XSIZE_TFT, x, y,
							  0x0c, pic_w, pic_h);

		LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF3, canvas_w1, 0, 0,
										 LAY_BUFF2, LCD_XSIZE_TFT, x, y,
										 Black, pic_w, pic_h);
	}
	else if (pic_flag == 2)
	{
		BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, x, y,
									   LAY_BUFF3, canvas_w1, 0, pic_h,
									   LAY_BUFF1, LCD_XSIZE_TFT, x, y,
									   pic_w, pic_h);

		BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, x, y,
									   LAY_BUFF3, canvas_w1, 0, 0,
									   LAY_BUFF2, LCD_XSIZE_TFT, x, y,
									   pic_w, pic_h);
	}

	
	tt = (var[p_add * 2] << 8) + var[p_add * 2 + 1];
	
//	if ((var[p_add * 2] << 8) + var[p_add * 2 + 1] <= Vmin)
//		angle = Sangle;
//	else if ((var[p_add * 2] << 8) + var[p_add * 2 + 1] >= Vmax)
//		angle = Eangle;
//	else
//		angle = (Eangle - Sangle) * ((int16_t)((var[p_add * 2] << 8) + var[p_add * 2 + 1]) - Vmin) / (Vmax - Vmin) + Sangle;
	if (tt <= Vmin)
		angle = Sangle;
	else if (tt >= Vmax)
		angle = Eangle;
	else
		angle = (Eangle - Sangle) * (tt - Vmin) / (Vmax - Vmin) + Sangle;

	Canvas_Image_Start_address(LAY_BUFF2);
	Canvas_image_width(LCD_XSIZE_TFT);

	if (angle == 0)
	{
		LT768_DrawSquare_Fill(xR - pic_w / 2, yR - pic_h / 2, xR + pic_w / 2, yR + pic_h / 2, Black);
		angle = -1;
	}
	else if (angle == 360)
		angle = -1;
	else
	{
		if (Screen_DIR == 1)
		{
			angle = angle + 90;
			angle = angle % 360;
		}
		else if (Screen_DIR == 2)
		{
			angle = angle + 180;
			angle = angle % 360;
		}
		if (Screen_DIR == 3)
		{
			angle = angle + 270;
			angle = angle % 360;
		}
	}
	if (angle >= 0 && angle < 45)
	{
		temp = pic_h / 2 * fast_tan(angle);
		if (Screen_DIR == 0)
		{
			LT768_DrawSquare_Fill(xR, yR - pic_h / 2, xR + pic_w / 2, yR + pic_h / 2, Black);
			LT768_DrawPentagon_Fill(xR, yR, xR - temp, yR + pic_h / 2, xR - pic_w / 2, yR + pic_h / 2, xR - pic_w / 2, yR - pic_h / 2, xR, yR - pic_h / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			LT768_DrawQuadrilateral_Fill(xR, yR, xR - temp, yR + pic_h / 2, xR - pic_w / 2, yR + pic_h / 2, xR - pic_w / 2, yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			LT768_DrawPentagon_Fill(xR, yR, xR - temp, yR + pic_h / 2, xR - pic_w / 2, yR + pic_h / 2, xR - pic_w / 2, yR - pic_h / 2, xR, yR - pic_h / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			LT768_DrawSquare_Fill(xR, yR - pic_h / 2, xR + pic_w / 2, yR, Black);
			LT768_DrawPentagon_Fill(xR, yR, xR - temp, yR + pic_h / 2, xR - pic_w / 2, yR + pic_h / 2, xR - pic_w / 2, yR - pic_h / 2, xR, yR - pic_h / 2, Black);
		}
	}
	else if (angle >= 45 && angle <= 90)
	{
		temp = pic_w / 2 * fast_tan(90 - angle);
		if (Screen_DIR == 0)
		{
			LT768_DrawSquare_Fill(xR, yR - pic_h / 2, xR + pic_w / 2, yR + pic_h / 2, Black);
			LT768_DrawQuadrilateral_Fill(xR, yR, xR - pic_w / 2, yR + temp, xR - pic_w / 2, yR - pic_h / 2, xR, yR - pic_h / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			LT768_DrawTriangle_Fill(xR, yR, xR - pic_w / 2, yR + temp, xR - pic_w / 2, yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			LT768_DrawQuadrilateral_Fill(xR, yR, xR - pic_w / 2, yR + temp, xR - pic_w / 2, yR - pic_h / 2, xR, yR - pic_h / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			LT768_DrawSquare_Fill(xR, yR - pic_h / 2, xR + pic_w / 2, yR, Black);
			LT768_DrawQuadrilateral_Fill(xR, yR, xR - pic_w / 2, yR + temp, xR - pic_w / 2, yR - pic_h / 2, xR, yR - pic_h / 2, Black);
		}
	}
	else if (angle > 90 && angle < 135)
	{
		temp = pic_w / 2 * fast_tan(angle - 90);
		if (Screen_DIR == 0)
		{
			LT768_DrawSquare_Fill(xR, yR - pic_h / 2, xR + pic_w / 2, yR + pic_h / 2, Black);
			LT768_DrawQuadrilateral_Fill(xR, yR, xR - pic_w / 2, yR - temp, xR - pic_w / 2, yR - pic_h / 2, xR, yR - pic_h / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			LT768_DrawSquare_Fill(xR - pic_w / 2, yR, xR + pic_w / 2, yR + pic_h / 2, Black);
			LT768_DrawPentagon_Fill(xR, yR, xR - pic_w / 2, yR - temp, xR - pic_w / 2, yR - pic_h / 2, xR + pic_w / 2, yR - pic_h / 2, xR + pic_w / 2, yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			LT768_DrawQuadrilateral_Fill(xR, yR, xR - pic_w / 2, yR - temp, xR - pic_w / 2, yR - pic_h / 2, xR, yR - pic_h / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			LT768_DrawSquare_Fill(xR, yR - pic_h / 2, xR + pic_w / 2, yR, Black);
			LT768_DrawQuadrilateral_Fill(xR, yR, xR - pic_w / 2, yR - temp, xR - pic_w / 2, yR - pic_h / 2, xR, yR - pic_h / 2, Black);
		}
	}
	else if (angle >= 135 && angle <= 180)
	{
		temp = pic_h / 2 * fast_tan(180 - angle);
		if (Screen_DIR == 0)
		{
			LT768_DrawSquare_Fill(xR, yR - pic_h / 2, xR + pic_w / 2, yR + pic_h / 2, Black);
			LT768_DrawTriangle_Fill(xR, yR, xR - temp, yR - pic_h / 2, xR, yR - pic_h / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			LT768_DrawSquare_Fill(xR - pic_w / 2, yR, xR + pic_w / 2, yR + pic_h / 2, Black);
			LT768_DrawQuadrilateral_Fill(xR, yR, xR - temp, yR - pic_h / 2, xR + pic_w / 2, yR - pic_h / 2, xR + pic_w / 2, yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			LT768_DrawTriangle_Fill(xR, yR, xR - temp, yR - pic_h / 2, xR, yR - pic_h / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			LT768_DrawSquare_Fill(xR, yR - pic_h / 2, xR + pic_w / 2, yR, Black);
			LT768_DrawTriangle_Fill(xR, yR, xR - temp, yR - pic_h / 2, xR, yR - pic_h / 2, Black);
		}
	}
	else if (angle > 180 && angle < 225)
	{
		temp = pic_h / 2 * fast_tan(angle - 180);
		if (Screen_DIR == 0)
		{
			LT768_DrawPentagon_Fill(xR, yR, xR + temp, yR - pic_h / 2, xR + pic_w / 2, yR - pic_h / 2, xR + pic_w / 2, yR + pic_h / 2, xR, yR + pic_h / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			LT768_DrawSquare_Fill(xR - pic_w / 2, yR, xR + pic_w / 2, yR + pic_h / 2, Black);
			LT768_DrawQuadrilateral_Fill(xR, yR, xR + temp, yR - pic_h / 2, xR + pic_w / 2, yR - pic_h / 2, xR + pic_w / 2, yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			LT768_DrawSquare_Fill(xR - pic_w / 2, yR - pic_h / 2, xR, yR + pic_h / 2, Black);
			LT768_DrawPentagon_Fill(xR, yR, xR + temp, yR - pic_h / 2, xR + pic_w / 2, yR - pic_h / 2, xR + pic_w / 2, yR + pic_h / 2, xR, yR + pic_h / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			LT768_DrawQuadrilateral_Fill(xR, yR, xR + temp, yR - pic_h / 2, xR + pic_w / 2, yR - pic_h / 2, xR + pic_w / 2, yR, Black);
		}
	}
	else if (angle >= 225 && angle <= 270)
	{
		temp = pic_w / 2 * fast_tan(270 - angle);
		if (Screen_DIR == 0)
		{
			LT768_DrawQuadrilateral_Fill(xR, yR, xR + pic_w / 2, yR - temp, xR + pic_w / 2, yR + pic_h / 2, xR, yR + pic_h / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			LT768_DrawSquare_Fill(xR - pic_w / 2, yR, xR + pic_w / 2, yR + pic_h / 2, Black);
			LT768_DrawTriangle_Fill(xR, yR, xR + pic_w / 2, yR - temp, xR + pic_w / 2, yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			LT768_DrawSquare_Fill(xR - pic_w / 2, yR - pic_h / 2, xR, yR + pic_h / 2, Black);
			LT768_DrawQuadrilateral_Fill(xR, yR, xR + pic_w / 2, yR - temp, xR + pic_w / 2, yR + pic_h / 2, xR, yR + pic_h / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			LT768_DrawTriangle_Fill(xR, yR, xR + pic_w / 2, yR - temp, xR + pic_w / 2, yR, Black);
		}
	}
	else if (angle > 270 && angle < 315)
	{
		temp = pic_w / 2 * fast_tan(angle - 270);
		if (Screen_DIR == 0)
		{
			LT768_DrawQuadrilateral_Fill(xR, yR, xR + pic_w / 2, yR + temp, xR + pic_w / 2, yR + pic_h / 2, xR, yR + pic_h / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			LT768_DrawPentagon_Fill(xR, yR, xR + pic_w / 2, yR + temp, xR + pic_w / 2, yR + pic_h / 2, xR - pic_w / 2, yR + pic_h / 2, xR - pic_w / 2, yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			LT768_DrawSquare_Fill(xR - pic_w / 2, yR - pic_h / 2, xR, yR + pic_h / 2, Black);
			LT768_DrawQuadrilateral_Fill(xR, yR, xR + pic_w / 2, yR + temp, xR + pic_w / 2, yR + pic_h / 2, xR, yR + pic_h / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			LT768_DrawPentagon_Fill(xR, yR, xR + pic_w / 2, yR + temp, xR + pic_w / 2, yR + pic_h / 2, xR - pic_w / 2, yR + pic_h / 2, xR - pic_w / 2, yR, Black);

			LT768_DrawSquare_Fill(xR - pic_w / 2, yR - pic_h / 2, xR + pic_w / 2, yR, Black);
		}
	}
	else if (angle >= 315 && angle < 360)
	{
		temp = pic_h / 2 * fast_tan(360 - angle);
		if (Screen_DIR == 0)
		{
			LT768_DrawTriangle_Fill(xR, yR, xR + temp, yR + pic_h / 2, xR, yR + pic_h / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			LT768_DrawQuadrilateral_Fill(xR, yR, xR + temp, yR + pic_h / 2, xR - pic_w / 2, yR + pic_h / 2, xR - pic_w / 2, yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			LT768_DrawSquare_Fill(xR - pic_w / 2, yR - pic_h / 2, xR, yR + pic_h / 2, Black);
			LT768_DrawTriangle_Fill(xR, yR, xR + temp, yR + pic_h / 2, xR, yR + pic_h / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			LT768_DrawQuadrilateral_Fill(xR, yR, xR + temp, yR + pic_h / 2, xR - pic_w / 2, yR + pic_h / 2, xR - pic_w / 2, yR - pic_h / 2, Black); // Question
			LT768_DrawSquare_Fill(xR - pic_w / 2, yR - pic_h / 2, xR + pic_w / 2, yR, Black);
		}
	}

	LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF2, LCD_XSIZE_TFT, x, y,
									 LAY_BUFF1, LCD_XSIZE_TFT, x, y,
									 Black, pic_w, pic_h);

	if (flag == 1)
	{
		memset(Display_buff, 0, 20);
		Flash_Read_UI(buff1, addr_index[2] + zk_id * 8, 4);
		zk_addr = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
		value1 = (var[p_add * 2] << 8) + var[p_add * 2 + 1];
		ll = pow(10, I_num + D_num);
		value = value1 % ll;
		if (value1 < 0)
        {
            Display_buff[Symbol] = '-';
            Symbol = 1;
            value = ~value1 + 1;
        }
		ll = value;
		do
		{ // Get the number of digits
			n++;
			ll /= 10;
		} while (ll > 0);
		if (D_num > 0) // With decimal point
		{
			if (D_num >= n)
			{
				Display_buff[Symbol] = '0';
				Display_buff[Symbol + 1] = '.';
				for (i = n; D_num > i; i++)
				{
					Display_buff[Symbol + 2 + i - n] = '0';
				}

				sprintf(&Display_buff[Symbol + 2 + D_num - n], "%d", value);


			}
			else
			{
				sprintf(&Display_buff[Symbol], "%d", value);

				for (i = 0; i < D_num; i++)
				{
					Display_buff[n + Symbol - i] = Display_buff[n - 1 + Symbol - i];
				}

				Display_buff[n + Symbol - D_num] = '.';

			}
		}
		else
		{
			sprintf(&Display_buff[Symbol], "%d", value);
		}
		Flash_Read_UI(buff1, zk_addr, 4);
		LT_Print_zk_ASCII(zk_addr, LAY_BUFF1, LCD_XSIZE_TFT, buff1[2], buff1[3], Alig, color,
						  dx, dy, Display_buff);
	}
	else if (flag == 2)
	{
		memset(Display_buff, 0, 20);
		value1 = (var[p_add * 2] << 8) + var[p_add * 2 + 1];
		
		if (value1 < 0)
        {
            Display_buff[kk] = 11;
            kk++;
            value = ~value1 + 1;
        }
        else
        {
            value = value1;
        }
		value = value % ((int)pow(10, I_num + D_num));

		for (i = 0; i < I_num; i++) // Integer part
		{
			Display_buff[kk] = value / (int)pow(10, I_num - i - 1 + D_num);

			if (Display_buff[kk] != 0 || flag1 == 1)
			{
				kk++;
				flag1 = 1;
			}
			value = value % (int)pow(10, I_num - i - 1 + D_num);
		}
		if (D_num > 0)
		{
			if (flag1 == 0)
			{
				Display_buff[kk] = 0; //"."
				kk++;
			}
			Display_buff[kk] = 10; //"."
			kk++;
		}
		else if (flag1 == 0)
		{
			Display_buff[kk] = 0; //"."
			kk++;
		}

		for (i = 0; i < D_num; i++) // Fractional part
		{
			Display_buff[kk] = value / (int)pow(10, D_num - i - 1);
			kk++;
			value = value % (int)pow(10, i - 1 + D_num);
		}

		xd = 0;
		for (i = 0; i < kk; i++) // Display section
		{
			if (S_id + Display_buff[i] >= addr_index[11])
				continue; // ICON is available
			Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * (S_id + Display_buff[i]), 12);
			pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
			num_w = buff1[4] + (buff1[5] << 8);
			h = buff1[6] + (buff1[7] << 8);
			pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
			pic_flag = buff1[11];

			if (pic_len != 0 && pic_flag != 0xFF)
			{
				if (Screen_DIR == 0)
				{
					x_src = xd;
					y_src = 0;
					w_des = num_w;
					if (i == 0)
						h0 = h;
				}
				else if (Screen_DIR == 1)
				{
					x_src = LCD_XSIZE_TFT - num_w;
					y_src = xd;
					w_des = h;
					if (i == 0)
						w0 = num_w;
				}
				else if (Screen_DIR == 2)
				{
					x_src = LCD_XSIZE_TFT - num_w - xd;
					y_src = LCD_YSIZE_TFT - h;
					w_des = num_w;
					if (i == 0)
						h0 = h;
				}
				else if (Screen_DIR == 3)
				{
					x_src = 0;
					y_src = LCD_YSIZE_TFT - h - xd;
					w_des = h;
					if (i == 0)
						w0 = num_w;
				}

				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, x_src, y_src,
													num_w, h,
													num_w, pic_add,
													LAY_BUFF2, LCD_XSIZE_TFT);
				xd += w_des;
			}
		}

		if (Screen_DIR == 0)
		{
			x_src = 0;
			y_src = 0;
			w_des = xd;
			h_des = h;
			if (Alig == 0) 		// Left alignment
			{
				x_des = dx;
				y_des = dy;
			}
			else if (Alig == 1) // Mid alignment
			{
				x_des = dx - w_des / 2 + 1;
				y_des = dy;
			}
			else if (Alig == 2) // Right alignment
			{
				x_des = dx - w_des + 1;
				y_des = dy;
			}
		}
		else if (Screen_DIR == 1)
		{
			x_src = LCD_XSIZE_TFT - w0;
			y_src = 0;
			w_des = w0;
			h_des = xd;
			if (Alig == 0) 		// Left alignment
			{
				x_des = dx - w_des + 1;
				y_des = dy;
			}
			else if (Alig == 1) // Mid alignment
			{
				x_des = dx - w_des + 1;
				y_des = dy - h_des / 2 + 1;
			}
			else if (Alig == 2) // Right alignment
			{
				x_des = dx - w_des + 1;
				y_des = dy - h_des + 1;
			}
		}
		else if (Screen_DIR == 2)
		{
			x_src = LCD_XSIZE_TFT - xd;
			y_src = LCD_YSIZE_TFT - h0;
			w_des = xd;
			h_des = h0;
			if (Alig == 0) 		// Left alignment
			{
				x_des = dx - w_des + 1;
				y_des = dy - h_des + 1;
			}
			else if (Alig == 1) // Mid alignment
			{
				x_des = dx - w_des / 2 + 1;
				y_des = dy - h_des + 1;
			}
			else if (Alig == 2) // Right alignment
			{
				x_des = dx;
				y_des = dy - h_des + 1;
			}
		}
		else if (Screen_DIR == 3)
		{
			x_src = 0;
			y_src = LCD_YSIZE_TFT - xd;
			w_des = w0;
			h_des = xd;
			if (Alig == 0) 		// Left alignment
			{
				x_des = dx;
				y_des = dy - xd + 1;
			}
			else if (Alig == 1) // Mid alignment
			{
				x_des = dx;
				y_des = dy - xd / 2 + 1;
			}
			else if (Alig == 2) // Right alignment
			{
				x_des = dx;
				y_des = dy;
			}
		}

		if (pic_flag == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
								  LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
								  LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_des, h_des);
		}
		else if (pic_flag == 1)
		{
			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
											 LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
											 Black, w_des, h_des);
		}
		else if (pic_flag == 2)
		{
			BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
										   LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
										   LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
										   w_des, h_des);
		}
	}

	LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, x, y,
						  LAY_BUFF1, LCD_XSIZE_TFT, x, y,
						  Display_Layer, LCD_XSIZE_TFT, x, y,
						  0x0c, pic_w, pic_h);

}
/**************************************/

Curve_Info gCurve_Info[Curve_Size];
uint8_t curve_num = 0;		 // Number of curve controls
uint8_t curve_flag = 0;		 // Curve timing refresh flag bit
uint8_t curve_group_sum = 1; // Total number of groups in curve area

/***********************************************************************************
 * Function Name	: Real_time_curve_display_static
 * Description		: Show static curves.
 * Input			: - curve: curve info
 * Output			: None
 * Return			: None
 ************************************************************************************/

void Real_time_curve_display_static(Curve_Info curve)
{
	uint16_t points = 0, num = 0, yCentral = 0, x, y;
	int16_t X0 = 0, X1 = 0, Y0 = 0, Y1 = 0;

	Canvas_Image_Start_address(Display_Layer);
	Canvas_image_width(LCD_XSIZE_TFT);

	Active_Window_XY(curve.xs, curve.ys);
	Active_Window_WH(curve.xe - curve.xs + 1, curve.ye - curve.ys + 1);

	if (Screen_DIR == 0)
	{
		if (curve.dir == 0)
		{
			x = curve.xe - (curve.Width - 1);
			points = (x - curve.xs + 1) / curve.Dis_HOR;
		}
		else
		{
			x = curve.xs;
			points = (curve.xe - x + 1) / curve.Dis_HOR;
		}
		yCentral = (curve.ys + curve.Y_Central) + curve.VD_Central * curve.MUL_Y / 256;
	}
	else if (Screen_DIR == 1)
	{
		if (curve.dir == 0)
		{
			y = curve.ye - (curve.Width - 1);
			points = (y - curve.ys + 1) / curve.Dis_HOR;
		}
		else
		{
			y = curve.ys;
			points = (curve.ye - y + 1) / curve.Dis_HOR;
		}
	}
	else if (Screen_DIR == 2)
	{
		if (curve.dir == 0)
		{
			x = curve.xs;
			points = (curve.xe - x - 1) / curve.Dis_HOR;
		}
		else
		{
			x = curve.xe - (curve.Width - 1);
			points = (x - curve.xs + 1) / curve.Dis_HOR;
		}
	}
	else if (Screen_DIR == 3)
	{
		if (curve.dir == 0)
		{
			y = curve.ys;
			points = (curve.ye - y + 1) / curve.Dis_HOR;
		}
		else
		{
			y = curve.ye - (curve.Width - 1);
			points = (y - curve.ys + 1) / curve.Dis_HOR;
		}
		yCentral = (curve.xs + curve.Y_Central) + curve.VD_Central * curve.MUL_Y / 256;
	}

	num = Curve_count[curve.channel] - 1 + 1024;

	for (uint16_t i = 0; i < points; i++)
	{
		if (Screen_DIR == 0)
		{
			X0 = x;
			if (curve.dir == 0)
			{
				X1 = x - curve.Dis_HOR;
				if (X1 < curve.xs)
					break;
			}
			else
			{
				X1 = x + curve.Dis_HOR;
				if (X1 + (curve.Width - 1) > curve.xe)
					break;
			}

			Y0 = yCentral - Curve_buff[curve.channel][num % 1024] * curve.MUL_Y / 256;
			Y1 = yCentral - Curve_buff[curve.channel][(num - 1) % 1024] * curve.MUL_Y / 256;

			if (Y0 < curve.ys)
				Y0 = curve.ys; // When the Y coordinate exceeds the upper or lower limit, it defaults to the upper or lower limit coordinate value
			else if (Y0 > curve.ye - curve.Width + 1)
				Y0 = curve.ye - curve.Width + 1; // Keep the part with lineweight greater than 1, so the Y coordinate value should be offset (lineweight - 1) pixels
			if (Y1 < curve.ys)
				Y1 = curve.ys;
			else if (Y1 > curve.ye - curve.Width + 1)
				Y1 = curve.ye - curve.Width + 1;
		}
		else if (Screen_DIR == 1)
		{
			Y0 = y;
			if (curve.dir == 0)
			{
				Y1 = y - curve.Dis_HOR;
				if (Y1 < curve.ys)
					break;
			}
			else
			{
				Y1 = y + curve.Dis_HOR;
				if (Y1 + (curve.Width - 1) > curve.ye)
					break;
			}

			X0 = (curve.xe - curve.Y_Central) + (Curve_buff[curve.channel][num % 1024] - curve.VD_Central) * curve.MUL_Y / 256;
			X1 = (curve.xe - curve.Y_Central) + (Curve_buff[curve.channel][(num - 1) % 1024] - curve.VD_Central) * curve.MUL_Y / 256;

			if (X0 < curve.xs)
				X0 = curve.xs;
			else if (X0 > curve.xe - curve.Width + 1)
				X0 = curve.xe - curve.Width + 1;
			if (X1 < curve.xs)
				X1 = curve.xs;
			else if (X1 > curve.xe - curve.Width + 1)
				X1 = curve.xe - curve.Width + 1;
		}
		else if (Screen_DIR == 2)
		{
			X0 = x;
			if (curve.dir == 0)
			{
				X1 = x + curve.Dis_HOR;
				if (X1 + (curve.Width - 1) > curve.xe)
					break;
			}
			else
			{
				X1 = x - curve.Dis_HOR;
				if (X1 < curve.xs)
					break;
			}
			Y0 = (curve.ye - curve.Y_Central) + (Curve_buff[curve.channel][num % 1024] - curve.VD_Central) * curve.MUL_Y / 256;
			Y1 = (curve.ye - curve.Y_Central) + (Curve_buff[curve.channel][(num - 1) % 1024] - curve.VD_Central) * curve.MUL_Y / 256;

			if (Y0 < curve.ys)
				Y0 = curve.ys; // When the Y coordinate exceeds the upper or lower limit, it defaults to the upper or lower limit coordinate value
			else if (Y0 > curve.ye - curve.Width + 1)
				Y0 = curve.ye - curve.Width + 1; // Keep the part with lineweight greater than 1, so the Y coordinate value should be offset (lineweight - 1) pixels
			if (Y1 < curve.ys)
				Y1 = curve.ys;
			else if (Y1 > curve.ye - curve.Width + 1)
				Y1 = curve.ye - curve.Width + 1;
		}
		else if (Screen_DIR == 3)
		{
			Y0 = y;
			if (curve.dir == 0)
			{
				Y1 = y + curve.Dis_HOR;
				if (Y1 + (curve.Width - 1) > curve.ye)
					break;
			}
			else
			{
				Y1 = y - curve.Dis_HOR;
				if (Y1 < curve.ys)
					break;
			}

			X0 = yCentral - Curve_buff[curve.channel][num % 1024] * curve.MUL_Y / 256;
			X1 = yCentral - Curve_buff[curve.channel][(num - 1) % 1024] * curve.MUL_Y / 256;

			if (X0 < curve.xs)
				X0 = curve.xs;
			else if (X0 > curve.xe - curve.Width + 1)
				X0 = curve.xe - curve.Width + 1;
			if (X1 < curve.xs)
				X1 = curve.xs;
			else if (X1 > curve.xe - curve.Width + 1)
				X1 = curve.xe - curve.Width + 1;
		}

		LT768_DrawLine_Width(X0, Y0, X1, Y1, curve.color, curve.Width);

		if (Screen_DIR == 0)
		{
			if (curve.dir == 0)
				x = x - curve.Dis_HOR;
			else
				x = x + curve.Dis_HOR;
		}
		else if (Screen_DIR == 1)
		{
			if (curve.dir == 0)
				y = y - curve.Dis_HOR;
			else
				y = y + curve.Dis_HOR;
		}
		else if (Screen_DIR == 2)
		{
			if (curve.dir == 0)
				x = x + curve.Dis_HOR;
			else
				x = x - curve.Dis_HOR;
		}
		else if (Screen_DIR == 3)
		{
			if (curve.dir == 0)
				y = y + curve.Dis_HOR;
			else
				y = y - curve.Dis_HOR;
		}

		num--;
	}

	Active_Window_XY(0, 0); // Restore the working serial port to the full-screen area
	Active_Window_WH(LCD_XSIZE_TFT, LCD_YSIZE_TFT);
}

/***********************************************************************************
 * Function Name	: Real_time_curve_display_dynamic
 * Description		: Show Dynamic Curves.
 * Input			: - group: which group of curve
 * Output			: None
 * Return			: None
 ************************************************************************************/

void Real_time_curve_display_dynamic(uint8_t group)
{
	uint8_t temp = 0, once_flag = 0;
	uint16_t points = 0, num = 0, yCentral = 0, x, y, i = 0, j = 0;
	int16_t X0 = 0, X1 = 0, Y0 = 0, Y1 = 0;

	for (i = 0; i < curve_num; i++)
	{
		if (gCurve_Info[i].group == group)
		{
			// Only load the background once under the same group
			if (once_flag == 0)
			{
				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gCurve_Info[i].xs, gCurve_Info[i].ys, // Curve underlay loading
									  MAIN_BUFF, LCD_XSIZE_TFT, gCurve_Info[i].xs, gCurve_Info[i].ys,
									  LAY_BUFF1, LCD_XSIZE_TFT, gCurve_Info[i].xs, gCurve_Info[i].ys,
									  0x0c, gCurve_Info[i].xe - gCurve_Info[i].xs + 1, gCurve_Info[i].ye - gCurve_Info[i].ys + 1);
				temp = i;
				once_flag = 1;
			}

			Canvas_Image_Start_address(LAY_BUFF1);
			Canvas_image_width(LCD_XSIZE_TFT);

			if (Screen_DIR == 0)
			{
				if (gCurve_Info[i].dir == 0)
				{
					x = gCurve_Info[i].xe - (gCurve_Info[i].Width - 1);
					points = (x - gCurve_Info[i].xs + 1) / gCurve_Info[i].Dis_HOR;
				}
				else
				{
					x = gCurve_Info[i].xs;
					points = (gCurve_Info[i].xe - x + 1) / gCurve_Info[i].Dis_HOR;
				}
				yCentral = (gCurve_Info[i].ys + gCurve_Info[i].Y_Central) + gCurve_Info[i].VD_Central * gCurve_Info[i].MUL_Y / 256;
			}
			else if (Screen_DIR == 1)
			{
				if (gCurve_Info[i].dir == 0)
				{
					y = gCurve_Info[i].ye - (gCurve_Info[i].Width - 1);
					points = (y - gCurve_Info[i].ys + 1) / gCurve_Info[i].Dis_HOR;
				}
				else
				{
					y = gCurve_Info[i].ys;
					points = (gCurve_Info[i].ye - y + 1) / gCurve_Info[i].Dis_HOR;
				}
			}
			else if (Screen_DIR == 2)
			{
				if (gCurve_Info[i].dir == 0)
				{
					x = gCurve_Info[i].xs;
					points = (gCurve_Info[i].xe - x + 1) / gCurve_Info[i].Dis_HOR;
				}
				else
				{
					x = gCurve_Info[i].xe - (gCurve_Info[i].Width - 1);
					points = (x - gCurve_Info[i].xs + 1) / gCurve_Info[i].Dis_HOR;
				}
			}
			else if (Screen_DIR == 3)
			{
				if (gCurve_Info[i].dir == 0)
				{
					y = gCurve_Info[i].ys;
					points = (gCurve_Info[i].ye - y + 1) / gCurve_Info[i].Dis_HOR;
				}
				else
				{
					y = gCurve_Info[i].ye - (gCurve_Info[i].Width - 1);
					points = (y - gCurve_Info[i].ys + 1) / gCurve_Info[i].Dis_HOR;
				}
				yCentral = (gCurve_Info[i].xs + gCurve_Info[i].Y_Central) + gCurve_Info[i].VD_Central * gCurve_Info[i].MUL_Y / 256;
			}

			num = Curve_count[gCurve_Info[i].channel] - 1 + 1024;

			for (j = 0; j < points; j++)
			{
				if (Screen_DIR == 0)
				{
					X0 = x;
					if (gCurve_Info[i].dir == 0)
					{
						X1 = x - gCurve_Info[i].Dis_HOR;
						if (X1 < gCurve_Info[i].xs)
							break;
					}
					else
					{
						X1 = x + gCurve_Info[i].Dis_HOR;
						if (X1 + (gCurve_Info[i].Width - 1) > gCurve_Info[i].xe)
							break;
					}
					Y0 = yCentral - Curve_buff[gCurve_Info[i].channel][num % 1024] * gCurve_Info[i].MUL_Y / 256;
					Y1 = yCentral - Curve_buff[gCurve_Info[i].channel][(num - 1) % 1024] * gCurve_Info[i].MUL_Y / 256;

					if (Y0 < gCurve_Info[i].ys)
						Y0 = gCurve_Info[i].ys;
					else if (Y0 > gCurve_Info[i].ye - gCurve_Info[i].Width + 1)
						Y0 = gCurve_Info[i].ye - gCurve_Info[i].Width + 1;
					if (Y1 < gCurve_Info[i].ys)
						Y1 = gCurve_Info[i].ys;
					else if (Y1 > gCurve_Info[i].ye - gCurve_Info[i].Width + 1)
						Y1 = gCurve_Info[i].ye - gCurve_Info[i].Width + 1;
				}
				else if (Screen_DIR == 1)
				{
					Y0 = y;

					if (gCurve_Info[i].dir == 0)
					{
						Y1 = y - gCurve_Info[i].Dis_HOR;
						if (Y1 < gCurve_Info[i].ys)
							break;
					}
					else
					{
						Y1 = y + gCurve_Info[i].Dis_HOR;
						if (Y1 + (gCurve_Info[i].Width - 1) > gCurve_Info[i].ye)
							break;
					}

					X0 = (gCurve_Info[i].xe - gCurve_Info[i].Y_Central) + (Curve_buff[gCurve_Info[i].channel][num % 1024] - gCurve_Info[i].VD_Central) * gCurve_Info[i].MUL_Y / 256;
					X1 = (gCurve_Info[i].xe - gCurve_Info[i].Y_Central) + (Curve_buff[gCurve_Info[i].channel][(num - 1) % 1024] - gCurve_Info[i].VD_Central) * gCurve_Info[i].MUL_Y / 256;

					if (X0 < gCurve_Info[i].xs)
						X0 = gCurve_Info[i].xs;
					else if (X0 > gCurve_Info[i].xe - gCurve_Info[i].Width + 1)
						X0 = gCurve_Info[i].xe - gCurve_Info[i].Width + 1;
					if (X1 < gCurve_Info[i].xs)
						X1 = gCurve_Info[i].xs;
					else if (X1 > gCurve_Info[i].xe - gCurve_Info[i].Width + 1)
						X1 = gCurve_Info[i].xe - gCurve_Info[i].Width + 1;
				}
				else if (Screen_DIR == 2)
				{
					X0 = x;
					if (gCurve_Info[i].dir == 0)
					{
						X1 = x + gCurve_Info[i].Dis_HOR;
						if (X1 + (gCurve_Info[i].Width - 1) > gCurve_Info[i].xe)
							break;
					}
					else
					{
						X1 = x - gCurve_Info[i].Dis_HOR;
						if (X1 < gCurve_Info[i].xs)
							break;
					}
					Y0 = (gCurve_Info[i].ye - gCurve_Info[i].Y_Central) + (Curve_buff[gCurve_Info[i].channel][num % 1024] - gCurve_Info[i].VD_Central) * gCurve_Info[i].MUL_Y / 256;
					Y1 = (gCurve_Info[i].ye - gCurve_Info[i].Y_Central) + (Curve_buff[gCurve_Info[i].channel][(num - 1) % 1024] - gCurve_Info[i].VD_Central) * gCurve_Info[i].MUL_Y / 256;

					if (Y0 < gCurve_Info[i].ys)
						Y0 = gCurve_Info[i].ys; // When the Y coordinate exceeds the upper or lower limit, it defaults to the upper or lower limit coordinate value
					else if (Y0 > gCurve_Info[i].ye - gCurve_Info[i].Width + 1)
						Y0 = gCurve_Info[i].ye - gCurve_Info[i].Width + 1; // Keep the part with lineweight greater than 1, so the Y coordinate value should be offset (lineweight - 1) pixels
					if (Y1 < gCurve_Info[i].ys)
						Y1 = gCurve_Info[i].ys;
					else if (Y1 > gCurve_Info[i].ye - gCurve_Info[i].Width + 1)
						Y1 = gCurve_Info[i].ye - gCurve_Info[i].Width + 1;
				}
				else if (Screen_DIR == 3)
				{
					Y0 = y;
					if (gCurve_Info[i].dir == 0)
					{
						Y1 = y + gCurve_Info[i].Dis_HOR;
						if (Y1 + (gCurve_Info[i].Width - 1) > gCurve_Info[i].ye)
							break;
					}
					else
					{
						Y1 = y - gCurve_Info[i].Dis_HOR;
						if (Y1 < gCurve_Info[i].ys)
							break;
					}

					X0 = yCentral - Curve_buff[gCurve_Info[i].channel][num % 1024] * gCurve_Info[i].MUL_Y / 256;
					X1 = yCentral - Curve_buff[gCurve_Info[i].channel][(num - 1) % 1024] * gCurve_Info[i].MUL_Y / 256;

					if (X0 < gCurve_Info[i].xs)
						X0 = gCurve_Info[i].xs;
					else if (X0 > gCurve_Info[i].xe - gCurve_Info[i].Width + 1)
						X0 = gCurve_Info[i].xe - gCurve_Info[i].Width + 1;
					if (X1 < gCurve_Info[i].xs)
						X1 = gCurve_Info[i].xs;
					else if (X1 > gCurve_Info[i].xe - gCurve_Info[i].Width + 1)
						X1 = gCurve_Info[i].xe - gCurve_Info[i].Width + 1;
				}

				LT768_DrawLine_Width(X0, Y0, X1, Y1, gCurve_Info[i].color, gCurve_Info[i].Width);

				if (Screen_DIR == 0)
				{
					if (gCurve_Info[i].dir == 0)
						x = x - gCurve_Info[i].Dis_HOR;
					else
						x = x + gCurve_Info[i].Dis_HOR;
				}
				else if (Screen_DIR == 1)
				{
					if (gCurve_Info[i].dir == 0)
						y = y - gCurve_Info[i].Dis_HOR;
					else
						y = y + gCurve_Info[i].Dis_HOR;
				}
				else if (Screen_DIR == 2)
				{
					if (gCurve_Info[i].dir == 0)
						x = x + gCurve_Info[i].Dis_HOR;
					else
						x = x - gCurve_Info[i].Dis_HOR;
				}
				else if (Screen_DIR == 3)
				{
					if (gCurve_Info[i].dir == 0)
						y = y + gCurve_Info[i].Dis_HOR;
					else
						y = y - gCurve_Info[i].Dis_HOR;
				}

				num--;
			}
		}
	}

	// After all the curves of the same group are drawn, update the corresponding display area
	LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, gCurve_Info[temp].xs, gCurve_Info[temp].ys, // Curve underlay loading
						  LAY_BUFF1, LCD_XSIZE_TFT, gCurve_Info[temp].xs, gCurve_Info[temp].ys,
						  Display_Layer, LCD_XSIZE_TFT, gCurve_Info[temp].xs, gCurve_Info[temp].ys,
						  0x0c, gCurve_Info[temp].xe - gCurve_Info[temp].xs + 1, gCurve_Info[temp].ye - gCurve_Info[temp].ys + 1);
}
/***********************************************************************************
* Function Name	: Curve_display
* Description	: Get curves info.
* Input			: - type: curves info sources
				  - buff: curves info
* Output		: None
* Return		: None
************************************************************************************/

void Curve_display(uint8_t type, uint8_t *buff)
{
	uint8_t channel = 0, Dis_HOR = 0, Width = 0, i = 0, count = 0, dir = 0;
	uint16_t p_add, p_len, pointer;
	uint16_t xs = 0, ys = 0, xe = 0, ye = 0, Y_Central = 0, VD_Central = 0, MUL_Y = 0;
	uint32_t color = 0;

	if (type == 0)		// Instruction information source is configuration file
	{
		xs = buff[5] + (buff[6] << 8);
		ys = buff[7] + (buff[8] << 8);
		xe = buff[9] + (buff[10] << 8);
		ye = buff[11] + (buff[12] << 8);
		Y_Central = buff[13] + (buff[14] << 8);
		VD_Central = buff[15] + (buff[16] << 8);
		MUL_Y = buff[17] + (buff[18] << 8);
		color = buff[19] + (buff[20] << 8) + (buff[21] << 16);
		channel = buff[22] & 0x7F;
		dir = (buff[22] & 0x80) >> 7;
		Dis_HOR = buff[23];
		Width = buff[24];
	}
	else if (type == 1) // Instruction information ource is variable space
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		xs = var[pointer + 1] + (var[pointer + 2] << 8);
		ys = var[pointer + 3] + (var[pointer + 4] << 8);
		xe = var[pointer + 5] + (var[pointer + 6] << 8);
		ye = var[pointer + 7] + (var[pointer + 8] << 8);
		Y_Central = var[pointer + 9] + (var[pointer + 10] << 8);
		VD_Central = var[pointer + 11] + (var[pointer + 12] << 8);
		MUL_Y = var[pointer + 13] + (var[pointer + 14] << 8);
		color = var[pointer + 15] + (var[pointer + 16] << 8) + (var[pointer + 17] << 16);
		channel = var[pointer + 18] & 0x7F;
		dir = (var[pointer + 18] & 0x80) >> 7;
		Dis_HOR = var[pointer + 19];
		Width = var[pointer + 20];
	}

	if (Pop_up_flag == 1) // Popup offset
	{
		xs += Pop_up_x;
		ys += Pop_up_y;
		xe += Pop_up_x;
		ye += Pop_up_y;
	}
	gCurve_Info[curve_num].xs = xs;
	gCurve_Info[curve_num].ys = ys;
	gCurve_Info[curve_num].xe = xe;
	gCurve_Info[curve_num].ye = ye;
	gCurve_Info[curve_num].Y_Central = Y_Central;
	gCurve_Info[curve_num].VD_Central = VD_Central;
	gCurve_Info[curve_num].MUL_Y = MUL_Y;
	gCurve_Info[curve_num].color = color;
	gCurve_Info[curve_num].channel = channel;
	gCurve_Info[curve_num].Dis_HOR = Dis_HOR;
	gCurve_Info[curve_num].Width = Width;
	gCurve_Info[curve_num].dir = dir;

	Real_time_curve_display_static(gCurve_Info[curve_num]);
	curve_update_flag[gCurve_Info[curve_num].channel] = 0;

	// Curve grouping
	if (curve_num)
	{
		for (i = 0; i < curve_num; i++)
		{
			if (gCurve_Info[curve_num].xs != gCurve_Info[i].xs ||
				gCurve_Info[curve_num].ys != gCurve_Info[i].ys ||
				gCurve_Info[curve_num].xe != gCurve_Info[i].xe ||
				gCurve_Info[curve_num].ye != gCurve_Info[i].ye)
			{
				// If x y w h is different, add a group of areas
				count++;
			}
			else
			{
				// If x y w h is the same, it will be classified into the same group of areas
				gCurve_Info[curve_num].group = gCurve_Info[i].group;
				break;
			}
		}
		if (count == curve_num) // Indicates that the latest control is compared with all previous controls. If the area is different, a new group of areas will be added
		{
			curve_group_sum++;
			gCurve_Info[curve_num].group = curve_group_sum;
		}
	}
	else // The first curve control defaults to group 0
	{
		curve_group_sum = 1;
		gCurve_Info[curve_num].group = curve_group_sum;
	}
	curve_num++;
}

void Display_Curve(void)
{
	uint8_t i = 0, j = 0;
	if (curve_num && curve_flag)
	{
		curve_flag = 0;
		for (i = 1; i <= curve_group_sum; i++)
		{
			for (j = 0; j < curve_num; j++)
			{
				// When a channel data is updated, multiple controls in the same group will be updated accordingly
				if (curve_update_flag[gCurve_Info[j].channel] && gCurve_Info[j].group == i)
				{
					Real_time_curve_display_dynamic(i);
					break;
				}
			}
		}
		for (i = 0; i < Curve_Size; i++) // After all the curves are updated, clear the flag
		{
			curve_update_flag[i] = 0;
		}
	}
}

/**************************************/






/***********************************************************************************
* Function Name	: bit_control
* Description	: control variable's bit.
* Input			: - type: icon info sources
				  - buff: icon info
* Output		: None
* Return		: None
************************************************************************************/
void bit_control(uint8_t type, uint8_t *buff)
{
	uint8_t control_bit, Display_mode;
	uint8_t Display_Mode = 0;
	uint8_t buff1[12] = {0};
	uint16_t p_add, pointer;
	uint16_t x, y;
	uint16_t simage, eimage, nimage;

	if (type == 0)		// Instruction information source is configuration file
	{
		Display_Mode = (buff[4] & 0x80) >> 7;
		p_add = (buff[5] + (buff[6] << 8));
		x = buff[7] + (buff[8] << 8);
		y = buff[9] + (buff[10] << 8);
		control_bit = buff[11];
		simage = buff[12] + (buff[13] << 8);
		eimage = buff[14] + (buff[15] << 8);
		Display_mode = buff[16];
	}
	else if (type == 1) // Instruction information ource is variable space
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		Display_Mode = (var[pointer] & 0x80) >> 7;
		p_add = (var[pointer + 1] + (var[pointer + 2] << 8));
		x = var[pointer + 3] + (var[pointer + 4] << 8);
		y = var[pointer + 5] + (var[pointer + 6] << 8);
		control_bit = var[pointer + 7];
		simage = var[pointer + 8] + (var[pointer + 9] << 8);
		eimage = var[pointer + 10] + (var[pointer + 11] << 8);
		Display_mode = var[pointer + 12];
	}

	if (Pop_up_flag == 1) // Popup offset
	{
		x += Pop_up_x;
		y += Pop_up_y;
	}
	nimage = (var[p_add * 2] << 8) + var[p_add * 2 + 1];

	if ((nimage & (0x01 << control_bit)) == 0)
		Display_Icon(simage, x, y, Display_Mode);
	else
		Display_Icon(eimage, x, y, Display_Mode);
}

/*****************/

Count_Info gCount_Info[Count_size];
Record_count_Info gRecord_count_Info[20];
uint8_t Count_sum = 0;
uint8_t Count_flag = 0;
uint8_t Record_count = 0;
uint16_t Count_reg[8];
uint8_t Count_reg_num;
/***********************************************************************************
* Function Name	: count_display
* Description	: Get time counter info.
* Input			: - type: time counter info sources
				  - buff: time counter info
* Output		: None
* Return		: None
************************************************************************************/

void count_display(uint8_t type, uint8_t *buff)
{
	uint8_t i = 0;
	uint16_t pointer = 0;
	if (type == 0)
	{
		gCount_Info[Count_sum].v_add = (buff[5] + (buff[6] << 8));
		gCount_Info[Count_sum].t_add = buff[7] + (buff[8] << 8);
		gCount_Info[Count_sum].c_add = buff[9] + (buff[10] << 8);
		gCount_Info[Count_sum].x = buff[11] + (buff[12] << 8);
		gCount_Info[Count_sum].y = buff[13] + (buff[14] << 8);
		gCount_Info[Count_sum].ID_start = buff[15] + (buff[16] << 8);
		gCount_Info[Count_sum].dis_mode = buff[17];
		gCount_Info[Count_sum].dir = buff[18];
		gCount_Info[Count_sum].end_mode = buff[19];
		gCount_Info[Count_sum].end_feedback = buff[20];
		gCount_Info[Count_sum].var_addr[0] = buff[21] + (buff[22] << 8);
		gCount_Info[Count_sum].var[0] = buff[23] + (buff[24] << 8);
		gCount_Info[Count_sum].var_addr[1] = buff[25] + (buff[26] << 8);
		gCount_Info[Count_sum].var[1] = buff[27] + (buff[28] << 8);
		gCount_Info[Count_sum].var_addr[2] = buff[29] + (buff[30] << 8);
		gCount_Info[Count_sum].var[2] = buff[31] + (buff[32] << 8);
		gCount_Info[Count_sum].var_addr[3] = buff[33] + (buff[34] << 8);
		gCount_Info[Count_sum].var[3] = buff[35] + (buff[36] << 8);
		gCount_Info[Count_sum].var_addr[4] = buff[37] + (buff[38] << 8);
		gCount_Info[Count_sum].var[4] = buff[39] + (buff[40] << 8);
		gCount_Info[Count_sum].var_addr[5] = buff[41] + (buff[42] << 8);
		gCount_Info[Count_sum].var[5] = buff[43] + (buff[44] << 8);
		gCount_Info[Count_sum].var_addr[6] = buff[45] + (buff[46] << 8);
		gCount_Info[Count_sum].var[6] = buff[47] + (buff[48] << 8);
		gCount_Info[Count_sum].var_addr[7] = buff[49] + (buff[50] << 8);
		gCount_Info[Count_sum].var[7] = buff[51] + (buff[52] << 8);
	}
	else if (type == 1)
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		gCount_Info[Count_sum].v_add = (var[pointer + 1] + (var[pointer + 2] << 8));
		gCount_Info[Count_sum].t_add = var[pointer + 3] + (var[pointer + 4] << 8);
		gCount_Info[Count_sum].c_add = var[pointer + 5] + (var[pointer + 6] << 8);
		gCount_Info[Count_sum].x = var[pointer + 7] + (var[pointer + 8] << 8);
		gCount_Info[Count_sum].y = var[pointer + 9] + (var[pointer + 10] << 8);
		gCount_Info[Count_sum].ID_start = var[pointer + 11] + (var[pointer + 12] << 8);
		gCount_Info[Count_sum].dis_mode = var[pointer + 13];
		gCount_Info[Count_sum].dir = var[pointer + 14];
		gCount_Info[Count_sum].end_mode = var[pointer + 15];
		gCount_Info[Count_sum].end_feedback = var[pointer + 16];
		gCount_Info[Count_sum].var_addr[0] = var[pointer + 17] + (var[pointer + 18] << 8);
		gCount_Info[Count_sum].var[0] = var[pointer + 19] + (var[pointer + 20] << 8);
		gCount_Info[Count_sum].var_addr[1] = var[pointer + 21] + (var[pointer + 22] << 8);
		gCount_Info[Count_sum].var[1] = var[pointer + 23] + (var[pointer + 24] << 8);
		gCount_Info[Count_sum].var_addr[2] = var[pointer + 25] + (var[pointer + 26] << 8);
		gCount_Info[Count_sum].var[2] = var[pointer + 27] + (var[pointer + 28] << 8);
		gCount_Info[Count_sum].var_addr[3] = var[pointer + 29] + (var[pointer + 30] << 8);
		gCount_Info[Count_sum].var[3] = var[pointer + 31] + (var[pointer + 32] << 8);
		gCount_Info[Count_sum].var_addr[4] = var[pointer + 33] + (var[pointer + 34] << 8);
		gCount_Info[Count_sum].var[4] = var[pointer + 35] + (var[pointer + 36] << 8);
		gCount_Info[Count_sum].var_addr[5] = var[pointer + 37] + (var[pointer + 38] << 8);
		gCount_Info[Count_sum].var[5] = var[pointer + 39] + (var[pointer + 40] << 8);
		gCount_Info[Count_sum].var_addr[6] = var[pointer + 41] + (var[pointer + 42] << 8);
		gCount_Info[Count_sum].var[6] = var[pointer + 43] + (var[pointer + 44] << 8);
		gCount_Info[Count_sum].var_addr[7] = var[pointer + 45] + (var[pointer + 46] << 8);
		gCount_Info[Count_sum].var[7] = var[pointer + 47] + (var[pointer + 48] << 8);
	}

	if (Pop_up_flag == 1) // Popup offset
	{
		gCount_Info[Count_sum].x += Pop_up_x;
		gCount_Info[Count_sum].y += Pop_up_y;
	}
	for (i = 0; i < Record_count; i++)
	{
		if (gCount_Info[Count_sum].v_add == gRecord_count_Info[i].v_add)
		{
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
			Record_count--;
		}
	}
	gCount_Info[Count_sum].last_w = 0;
	Display_Count(Count_sum);

	Count_sum++;
}

uint8_t Display_Count(uint8_t i)
{
	uint8_t flag = 0, cnt = 0, j = 0;
	uint8_t buff[12] = {0};
	uint16_t ID[11];
	uint16_t temp, x_time = 0;
	uint16_t x_src, y_src, x_des, y_des, w_real, h_real;
	uint16_t width = 0, high = 0;
	uint8_t on_gif = 0;
	Canvas_Image_Start_address(LAY_BUFF1);
	Canvas_image_width(LCD_XSIZE_TFT);
	if (((var[gCount_Info[i].c_add * 2] << 8) + var[gCount_Info[i].c_add * 2 + 1]) == 2)
	{
		if (gCount_Info[i].last_w > 0)
		{
			Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * (gCount_Info[i].ID_start), 12);
			pic_w = buff[4] + (buff[5] << 8);
			pic_h = buff[6] + (buff[7] << 8);

			if (Screen_DIR == 0)
			{
				x_des = gCount_Info[i].x;
				y_des = gCount_Info[i].y;
				w_real = gCount_Info[i].last_w;
				h_real = pic_h;
			}
			else if (Screen_DIR == 1)
			{
				x_des = gCount_Info[i].x - pic_w + 1;
				y_des = gCount_Info[i].y;
				w_real = pic_w;
				h_real = gCount_Info[i].last_w;
			}
			else if (Screen_DIR == 2)
			{
				x_des = gCount_Info[i].x - gCount_Info[i].last_w + 1;
				y_des = gCount_Info[i].y - pic_h + 1;
				w_real = gCount_Info[i].last_w;
				h_real = pic_h;
			}
			else if (Screen_DIR == 3)
			{
				x_des = gCount_Info[i].x;
				y_des = gCount_Info[i].y - gCount_Info[i].last_w + 1;
				w_real = pic_w;
				h_real = gCount_Info[i].last_w;
			}
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
								  MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
								  Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_real, h_real);
			gCount_Info[i].last_w = 0;
		}
		return 0;
	}
	if (gCount_Info[i].dis_mode == 0)
	{
		return 0;
	}
	else if (gCount_Info[i].dis_mode == 1)
	{
		temp = (var[gCount_Info[i].t_add * 2] << 8) + var[gCount_Info[i].t_add * 2 + 1];
		ID[0] = temp / 36000 % 10;
		ID[1] = temp / 3600 % 10;
		ID[2] = 10;
		temp = temp % 3600;
		ID[3] = temp / 600 % 10;
		ID[4] = temp / 60 % 10;
		ID[5] = 10;
		temp = temp % 60;
		ID[6] = temp / 10 % 10;
		ID[7] = temp % 10;
		cnt = 7;
	}
	else if (gCount_Info[i].dis_mode == 2)
	{
		temp = (var[gCount_Info[i].t_add * 2] << 8) + var[gCount_Info[i].t_add * 2 + 1];

		ID[0] = temp / 36000 % 10;
		ID[1] = temp / 3600 % 10;
		ID[2] = 10;
		temp = temp % 3600;
		ID[3] = temp / 600 % 10;
		ID[4] = temp / 60 % 10;
		cnt = 4;
	}
	else if (gCount_Info[i].dis_mode == 3)
	{
		temp = (var[gCount_Info[i].t_add * 2] << 8) + var[gCount_Info[i].t_add * 2 + 1];
		temp = temp % 3600;
		ID[0] = temp / 600 % 10;
		ID[1] = temp / 60 % 10;
		ID[2] = 10;
		temp = temp % 60;
		ID[3] = temp / 10 % 10;
		ID[4] = temp % 10;
		cnt = 4;
	}
	else if (gCount_Info[i].dis_mode == 4)
	{
		if (((var[gCount_Info[i].v_add * 2] << 8) + var[gCount_Info[i].v_add * 2 + 1]) > 10000)
		{
			temp = (var[gCount_Info[i].t_add * 2] << 8) + var[gCount_Info[i].t_add * 2 + 1];
			ID[0] = temp / 10000 % 10;
			ID[1] = temp / 1000 % 10;
			ID[2] = temp / 100 % 10;
			ID[3] = temp / 10 % 10;
			ID[4] = temp % 10;
			cnt = 4;
		}
		else if (((var[gCount_Info[i].v_add * 2] << 8) + var[gCount_Info[i].v_add * 2 + 1]) > 1000)
		{
			temp = (var[gCount_Info[i].t_add * 2] << 8) + var[gCount_Info[i].t_add * 2 + 1];
			ID[0] = temp / 1000 % 10;
			ID[1] = temp / 100 % 10;
			ID[2] = temp / 10 % 10;
			ID[3] = temp % 10;
			cnt = 3;
		}
		else if (((var[gCount_Info[i].v_add * 2] << 8) + var[gCount_Info[i].v_add * 2 + 1]) > 100)
		{
			temp = (var[gCount_Info[i].t_add * 2] << 8) + var[gCount_Info[i].t_add * 2 + 1];
			ID[0] = temp / 100 % 10;
			ID[1] = temp / 10 % 10;
			ID[2] = temp % 10;
			cnt = 2;
		}
		else if (((var[gCount_Info[i].v_add * 2] << 8) + var[gCount_Info[i].v_add * 2 + 1]) > 10)
		{
			temp = (var[gCount_Info[i].t_add * 2] << 8) + var[gCount_Info[i].t_add * 2 + 1];
			ID[0] = temp / 10 % 10;
			ID[1] = temp % 10;
			cnt = 1;
		}
		else
		{
			temp = (var[gCount_Info[i].t_add * 2] << 8) + var[gCount_Info[i].t_add * 2 + 1];
			ID[0] = temp % 10;
			cnt = 0;
		}
	}
//	var_temp[gCount_Info[i].t_add * 2] = var[gCount_Info[i].t_add * 2];
//	var_temp[gCount_Info[i].t_add * 2 + 1] = var[gCount_Info[i].t_add * 2 + 1];
	for (j = 0; j <= cnt; j++)
	{
		if (gCount_Info[i].ID_start + ID[j] >= addr_index[11])
			continue;
		Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * (gCount_Info[i].ID_start + ID[j]), 12);

		pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
		pic_w = buff[4] + (buff[5] << 8);
		pic_h = buff[6] + (buff[7] << 8);
		pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
		pic_flag = buff[11];

		if (pic_len != 0 && pic_flag != 0xFF)
		{
			if (Screen_DIR == 0)
			{
				// Record the height and flag of the first valid ICON, and the following ICON arrangement shall be subject to this
				if (j == 0)
				{
					high = pic_h;
					flag = pic_flag;
				}
				x_src = x_time;
				y_src = 0;
				w_real = pic_w;
			}
			else if (Screen_DIR == 1)
			{
				if (j == 0)
				{
					width = pic_w;
					flag = pic_flag;
				}
				x_src = LCD_XSIZE_TFT - pic_w;
				y_src = x_time;
				w_real = pic_h;
			}
			else if (Screen_DIR == 2)
			{
				if (j == 0)
				{
					high = pic_h;
					flag = pic_flag;
				}
				x_src = LCD_XSIZE_TFT - pic_w - x_time;
				y_src = LCD_YSIZE_TFT - pic_h;
				w_real = pic_w;
			}
			else if (Screen_DIR == 3)
			{
				if (j == 0)
				{
					width = pic_w;
					flag = pic_flag;
				}
				x_src = 0;
				y_src = LCD_YSIZE_TFT - pic_h - x_time;
				w_real = pic_h;
			}
			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, x_src, y_src,
												pic_w, pic_h,
												pic_w, pic_add,
												LAY_BUFF1, LCD_XSIZE_TFT);
			x_time += w_real;
		}
	}

	/*On the keyboard input page*/
	if (DioKb_flag == 1)
	{
		if (!(gCount_Info[i].x > gDio_Info[Dio_num].Xe || (gCount_Info[i].x + x_time) < gDio_Info[Dio_num].Xs || gCount_Info[i].y > gDio_Info[Dio_num].Ye || (gCount_Info[i].y + pic_h) < gDio_Info[Dio_num].Ys))
		{
			return 0;
		}
	}
	else if (AioKb_flag == 1)
	{
		if (!(gCount_Info[i].x > gAio_Info[Aio_num].Xe || (gCount_Info[i].x + x_time) < gAio_Info[Aio_num].Xs || gCount_Info[i].y > gAio_Info[Aio_num].Ye || (gCount_Info[i].y + pic_h) < gAio_Info[Aio_num].Ys))
		{
			return 0;
		}
	}
	else if (GBKioKb_flag == 1)
	{
		if (!(gCount_Info[i].x > gGBKio_Info[GBKio_num].Xe || (gCount_Info[i].x + x_time) < gGBKio_Info[GBKio_num].Xs || gCount_Info[i].y > gGBKio_Info[GBKio_num].Ye || (gCount_Info[i].y + pic_h) < gGBKio_Info[GBKio_num].Ys))
		{
			return 0;
		}
	}

	if (Screen_DIR == 0)
	{
		x_src = 0;
		y_src = 0;
		x_des = gCount_Info[i].x;
		y_des = gCount_Info[i].y;
		w_real = x_time;
		h_real = high;
	}
	else if (Screen_DIR == 1)
	{
		x_src = LCD_XSIZE_TFT - width;
		y_src = 0;
		x_des = gCount_Info[i].x - width + 1;
		y_des = gCount_Info[i].y;
		w_real = width;
		h_real = x_time;
	}
	else if (Screen_DIR == 2)
	{
		x_src = LCD_XSIZE_TFT - x_time;
		y_src = LCD_YSIZE_TFT - high;
		x_des = gCount_Info[i].x - x_time + 1;
		y_des = gCount_Info[i].y - high + 1;
		w_real = x_time;
		h_real = high;
	}
	else if (Screen_DIR == 3)
	{
		x_src = 0;
		y_src = LCD_YSIZE_TFT - x_time;
		x_des = gCount_Info[i].x;
		y_des = gCount_Info[i].y - x_time + 1;
		w_real = width;
		h_real = x_time;
	}

	if (flag == 0)
	{
		LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
							  LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
							  Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
							  0x0c, w_real, h_real);
	}
	else if (flag == 1)
	{
		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
							  MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
							  LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
							  0x0c, w_real, h_real);

		LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
										 LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
										 Black, w_real, h_real);

		LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, x_des, y_des,
							  LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
							  Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
							  0x0c, w_real, h_real);
	}
	else if (flag == 2)
	{
		for (uint8_t i = 0; i < Gif_num; i++)
		{
			if (x_des >= gGif_Info[i].x && x_des <= gGif_Info[i].x + gGif_Info[i].w &&
				y_des >= gGif_Info[i].y && y_des <= gGif_Info[i].y + gGif_Info[i].h && gGif_Info[i].on_gif == 1)
			{
				on_gif = 1;
				break;
			}
		}

		if (on_gif == 0)
		{
			BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, x_des, y_des,
										   LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
										   Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
										   w_real, h_real);
		}
		else if (on_gif == 1)
		{
			BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF6, LCD_XSIZE_TFT, x_des, y_des,
										   LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
										   Display_Layer, LCD_XSIZE_TFT, x_des, y_des,
										    w_real, h_real);
			
			LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
								  LAY_BUFF1, LCD_XSIZE_TFT, x_src, y_src,
								  LAY_BUFF5, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c,  w_real, h_real);
		}
	}

	gCount_Info[i].last_w = x_time;
}

void show_Count(void)
{
	if (Count_sum > 0 && Count_flag == 1)
	{
		Count_flag = 0;
		for (uint8_t i = 0; i < Count_sum; i++)
		{
			if (((var[gCount_Info[i].c_add * 2] << 8) + var[gCount_Info[i].c_add * 2 + 1]) != 0)
				Display_Count(i);
			else if (((var[gCount_Info[i].t_add * 2] << 8) + var[gCount_Info[i].t_add * 2 + 1]) !=
					 (var_temp[gCount_Info[i].t_add * 2] << 8) + var_temp[gCount_Info[i].t_add * 2 + 1])
				Display_Count(i);

		}
	}
}

void Saving_counting_parameters(void)
{
	uint8_t i, j;
	for (i = 0; i < Count_sum; i++)
	{
		if (Record_count > 0 && gCount_Info[i].end_mode == 1)
		{
			for (j = 0; j < Record_count; j++)
			{
				if (gCount_Info[i].v_add == gRecord_count_Info[j].v_add)
					break;
				if (j == Record_count - 1)
				{
					gRecord_count_Info[Record_count].v_add = gCount_Info[i].v_add;
					gRecord_count_Info[Record_count].t_add = gCount_Info[i].t_add;
					gRecord_count_Info[Record_count].c_add = gCount_Info[i].c_add;
					gRecord_count_Info[Record_count].dir = gCount_Info[i].dir;
					gRecord_count_Info[Record_count].end_feedback = gCount_Info[i].end_feedback;
					gRecord_count_Info[Record_count].var[0] = gCount_Info[i].var[0];
					gRecord_count_Info[Record_count].var_addr[0] = gCount_Info[i].var_addr[0];
					gRecord_count_Info[Record_count].var[1] = gCount_Info[i].var[1];
					gRecord_count_Info[Record_count].var_addr[1] = gCount_Info[i].var_addr[1];
					gRecord_count_Info[Record_count].var[2] = gCount_Info[i].var[2];
					gRecord_count_Info[Record_count].var_addr[2] = gCount_Info[i].var_addr[2];
					gRecord_count_Info[Record_count].var[3] = gCount_Info[i].var[3];
					gRecord_count_Info[Record_count].var_addr[3] = gCount_Info[i].var_addr[3];
					gRecord_count_Info[Record_count].var[4] = gCount_Info[i].var[4];
					gRecord_count_Info[Record_count].var_addr[4] = gCount_Info[i].var_addr[4];
					gRecord_count_Info[Record_count].var[5] = gCount_Info[i].var[5];
					gRecord_count_Info[Record_count].var_addr[5] = gCount_Info[i].var_addr[5];
					gRecord_count_Info[Record_count].var[6] = gCount_Info[i].var[6];
					gRecord_count_Info[Record_count].var_addr[6] = gCount_Info[i].var_addr[6];
					gRecord_count_Info[Record_count].var[7] = gCount_Info[i].var[7];
					gRecord_count_Info[Record_count].var_addr[7] = gCount_Info[i].var_addr[7];
					Record_count++;
				}
			}
		}
		else if (gCount_Info[i].end_mode == 1)
		{
			gRecord_count_Info[Record_count].v_add = gCount_Info[i].v_add;
			gRecord_count_Info[Record_count].t_add = gCount_Info[i].t_add;
			gRecord_count_Info[Record_count].c_add = gCount_Info[i].c_add;
			gRecord_count_Info[Record_count].dir = gCount_Info[i].dir;
			gRecord_count_Info[Record_count].end_feedback = gCount_Info[i].end_feedback;
			gRecord_count_Info[Record_count].var[0] = gCount_Info[i].var[0];
			gRecord_count_Info[Record_count].var_addr[0] = gCount_Info[i].var_addr[0];
			gRecord_count_Info[Record_count].var[1] = gCount_Info[i].var[1];
			gRecord_count_Info[Record_count].var_addr[1] = gCount_Info[i].var_addr[1];
			gRecord_count_Info[Record_count].var[2] = gCount_Info[i].var[2];
			gRecord_count_Info[Record_count].var_addr[2] = gCount_Info[i].var_addr[2];
			gRecord_count_Info[Record_count].var[3] = gCount_Info[i].var[3];
			gRecord_count_Info[Record_count].var_addr[3] = gCount_Info[i].var_addr[3];
			gRecord_count_Info[Record_count].var[4] = gCount_Info[i].var[4];
			gRecord_count_Info[Record_count].var_addr[4] = gCount_Info[i].var_addr[4];
			gRecord_count_Info[Record_count].var[5] = gCount_Info[i].var[5];
			gRecord_count_Info[Record_count].var_addr[5] = gCount_Info[i].var_addr[5];
			gRecord_count_Info[Record_count].var[6] = gCount_Info[i].var[6];
			gRecord_count_Info[Record_count].var_addr[6] = gCount_Info[i].var_addr[6];
			gRecord_count_Info[Record_count].var[7] = gCount_Info[i].var[7];
			gRecord_count_Info[Record_count].var_addr[7] = gCount_Info[i].var_addr[7];
			Record_count++;
		}
	}
}

/***********************************/
Var_Count_Info gVar_Count_Info[Var_count_size];
uint8_t Var_Count_num = 0;
uint8_t Var_Count_flag = 0;
/***********************************************************************************
* Function Name	: Variable Count
* Description	: Get Variable Count info.
* Input			: - type: time counter info sources
				  - buff: time counter info
* Output		: None
* Return		: None
************************************************************************************/
void Variable_Count(uint8_t type, uint8_t *buff)
{
	uint16_t pointer = 0;
	
	if (type == 0)
	{
		gVar_Count_Info[Var_Count_num].v_add = (buff[5] + (buff[6] << 8));
		gVar_Count_Info[Var_Count_num].v_type = buff[7];
		gVar_Count_Info[Var_Count_num].c_add = buff[8] + (buff[9] << 8);
		gVar_Count_Info[Var_Count_num].V_loop = buff[10] + (buff[11] << 8);
		gVar_Count_Info[Var_Count_num].V_once = buff[12] + (buff[13] << 8);
		gVar_Count_Info[Var_Count_num].V_stop = buff[14] + (buff[15] << 8);
		gVar_Count_Info[Var_Count_num].Vmin = ((long long)((buff[16] << 24) + (buff[17] << 16) + (buff[18] << 8) + buff[19]) << 32) 
												| (long long)((buff[20] << 24) + (buff[21] << 16) + (buff[22] << 8) + buff[23]);
		gVar_Count_Info[Var_Count_num].Vmax = ((long long)((buff[24] << 24) + (buff[25] << 16) + (buff[26] << 8) + buff[27]) << 32) 
											| (long long)((buff[28] << 24) + (buff[29] << 16) + (buff[30] << 8) + buff[31]);
		gVar_Count_Info[Var_Count_num].step = buff[32] + (buff[33] << 8);
		gVar_Count_Info[Var_Count_num].t_d = buff[34] + (buff[35] << 8);
		gVar_Count_Info[Var_Count_num].t_c = 0;
		gVar_Count_Info[Var_Count_num].var_addr[0] = buff[36] + (buff[37] << 8);
		gVar_Count_Info[Var_Count_num].var[0] = buff[38] + (buff[39] << 8);
		gVar_Count_Info[Var_Count_num].var_addr[1] = buff[40] + (buff[41] << 8);
		gVar_Count_Info[Var_Count_num].var[1] = buff[42] + (buff[43] << 8);
		gVar_Count_Info[Var_Count_num].var_addr[2] = buff[44] + (buff[45] << 8);
		gVar_Count_Info[Var_Count_num].var[2] = buff[46] + (buff[47] << 8);
		gVar_Count_Info[Var_Count_num].var_addr[3] = buff[48] + (buff[49] << 8);
		gVar_Count_Info[Var_Count_num].var[3] = buff[50] + (buff[51] << 8);
		gVar_Count_Info[Var_Count_num].var_addr[4] = buff[52] + (buff[53] << 8);
		gVar_Count_Info[Var_Count_num].var[4] = buff[54] + (buff[55] << 8);
		gVar_Count_Info[Var_Count_num].var_addr[5] = buff[56] + (buff[57] << 8);
		gVar_Count_Info[Var_Count_num].var[5] = buff[58] + (buff[59] << 8);
		gVar_Count_Info[Var_Count_num].var_addr[6] = buff[60] + (buff[61] << 8);
		gVar_Count_Info[Var_Count_num].var[6] = buff[62] + (buff[63] << 8);
		gVar_Count_Info[Var_Count_num].var_addr[7] = buff[64] + (buff[65] << 8);
		gVar_Count_Info[Var_Count_num].var[7] = buff[66] + (buff[67] << 8);
		gVar_Count_Info[Var_Count_num].mode = buff[68];
		gVar_Count_Info[Var_Count_num].feedback = buff[69];
	}
	else if (type == 1)
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		gVar_Count_Info[Var_Count_num].v_add = var[pointer + 1] + (var[pointer + 2] << 8);
		gVar_Count_Info[Var_Count_num].v_type = var[pointer + 3];
		gVar_Count_Info[Var_Count_num].c_add = var[pointer + 4] + (var[pointer + 5] << 8);
		gVar_Count_Info[Var_Count_num].V_loop = var[pointer + 6] + (var[pointer + 7] << 8);
		gVar_Count_Info[Var_Count_num].V_once = var[pointer + 8] + (var[pointer + 9] << 8);
		gVar_Count_Info[Var_Count_num].V_stop = var[pointer + 10] + (var[pointer + 11] << 8);
		gVar_Count_Info[Var_Count_num].Vmin = ((long long)((var[pointer + 12] << 24) + (var[pointer + 13] << 16) + (var[pointer + 14] << 8) + var[pointer + 15]) << 32)
											| (long long)((var[pointer + 16] << 24) + (var[pointer + 17] << 16) + (var[pointer + 18] << 8) + var[pointer + 19]);
		gVar_Count_Info[Var_Count_num].Vmax = ((long long)((var[pointer + 20] << 24) + (var[pointer + 21] << 16) + (var[pointer + 22] << 8) + var[pointer + 23]) << 32) 
											| (long long)((var[pointer + 24] << 24) + (var[pointer + 25] << 16) + (var[pointer + 26] << 8) + var[pointer + 27]);
		gVar_Count_Info[Var_Count_num].step = var[pointer + 28] + (var[pointer + 29] << 8);
		gVar_Count_Info[Var_Count_num].t_d = var[pointer + 30] + (var[pointer + 31] << 8);
		gVar_Count_Info[Var_Count_num].t_c = 0;
		gVar_Count_Info[Var_Count_num].var_addr[0] = var[pointer + 32] + (var[pointer + 33] << 8);
		gVar_Count_Info[Var_Count_num].var[0] = var[pointer + 34] + (var[pointer + 35] << 8);
		gVar_Count_Info[Var_Count_num].var_addr[1] = var[pointer + 36] + (var[pointer + 37] << 8);
		gVar_Count_Info[Var_Count_num].var[1] = var[pointer + 38] + (var[pointer + 39] << 8);
		gVar_Count_Info[Var_Count_num].var_addr[2] = var[pointer + 40] + (var[pointer + 41] << 8);
		gVar_Count_Info[Var_Count_num].var[2] = var[pointer + 42] + (var[pointer + 43] << 8);
		gVar_Count_Info[Var_Count_num].var_addr[3] = var[pointer + 44] + (var[pointer + 45] << 8);
		gVar_Count_Info[Var_Count_num].var[3] = var[pointer + 46] + (var[pointer + 47] << 8);
		gVar_Count_Info[Var_Count_num].var_addr[4] = var[pointer + 48] + (var[pointer + 49] << 8);
		gVar_Count_Info[Var_Count_num].var[4] = var[pointer + 50] + (var[pointer + 51] << 8);
		gVar_Count_Info[Var_Count_num].var_addr[5] = var[pointer + 52] + (var[pointer + 53] << 8);
		gVar_Count_Info[Var_Count_num].var[5] = var[pointer + 54] + (var[pointer + 55] << 8);
		gVar_Count_Info[Var_Count_num].var_addr[6] = var[pointer + 56] + (var[pointer + 57] << 8);
		gVar_Count_Info[Var_Count_num].var[6] = var[pointer + 58] + (var[pointer + 59] << 8);
		gVar_Count_Info[Var_Count_num].var_addr[7] = var[pointer + 60] + (var[pointer + 61] << 8);
		gVar_Count_Info[Var_Count_num].var[7] = var[pointer + 62] + (var[pointer + 63] << 8);
		gVar_Count_Info[Var_Count_num].mode = var[pointer + 64];
		gVar_Count_Info[Var_Count_num].feedback = var[pointer + 65];
	}
	Var_Count_num ++;

}

void Run_Var_Count(uint8_t num)
{
	uint8_t finish = 0;
	uint8_t rebuf[20];
	uint8_t page_flag = 0;
	int8_t sum1 = 0;
	int16_t sum2 = 0;
	int32_t sum4 = 0;
	long long temp = 0;

	if (gVar_Count_Info[num].v_type == 0 )
	{
		temp = var[gVar_Count_Info[num].v_add * 2 + 1];
	}
	else if(gVar_Count_Info[num].v_type == 0x80 )
	{
		sum1 = var[gVar_Count_Info[num].v_add * 2 + 1];
		temp = sum1;
	}
	else if (gVar_Count_Info[num].v_type == 1) 
	{
		temp = (var[gVar_Count_Info[num].v_add * 2] << 8) + var[gVar_Count_Info[num].v_add * 2 + 1];
	}
	else if (gVar_Count_Info[num].v_type == 0x81) 
	{
		sum2 = (var[gVar_Count_Info[num].v_add * 2] << 8) + var[gVar_Count_Info[num].v_add * 2 + 1];
		temp = sum2;
	}
	else if (gVar_Count_Info[num].v_type == 2)
	{
		temp = (var[gVar_Count_Info[num].v_add * 2] << 24) + (var[gVar_Count_Info[num].v_add * 2 + 1] << 16) + (var[gVar_Count_Info[num].v_add * 2 + 2] << 8) + var[gVar_Count_Info[num].v_add * 2 + 3];
	}
	else if (gVar_Count_Info[num].v_type == 0x82)
	{
		sum4 = (var[gVar_Count_Info[num].v_add * 2] << 24) + (var[gVar_Count_Info[num].v_add * 2 + 1] << 16) + (var[gVar_Count_Info[num].v_add * 2 + 2] << 8) + var[gVar_Count_Info[num].v_add * 2 + 3];
		temp = sum4;
	}
	else if (gVar_Count_Info[num].v_type == 3 || gVar_Count_Info[num].v_type == 0x83)
	{
		temp = ((long long)((var[gVar_Count_Info[num].v_add * 2] << 24) + (var[gVar_Count_Info[num].v_add * 2 + 1] << 16) + (var[gVar_Count_Info[num].v_add * 2 + 2] << 8) + var[gVar_Count_Info[num].v_add * 2 + 3]) << 32);
		temp = temp | (long long)((var[gVar_Count_Info[num].v_add * 2 + 4] << 24) + (var[gVar_Count_Info[num].v_add * 2 + 5] << 16) + (var[gVar_Count_Info[num].v_add * 2 + 6] << 8) + var[gVar_Count_Info[num].v_add * 2 + 7]);
	}
	if ((var[gVar_Count_Info[num].c_add * 2] << 8) + var[gVar_Count_Info[num].c_add * 2 + 1] == gVar_Count_Info[num].V_loop)
	{
		if (gVar_Count_Info[num].mode == 1)	//+
		{
			if (temp + gVar_Count_Info[num].step < gVar_Count_Info[num].Vmin || temp >= gVar_Count_Info[num].Vmax)
			{
				temp = gVar_Count_Info[num].Vmin;
			}
			else if (temp <  gVar_Count_Info[num].Vmax && temp + gVar_Count_Info[num].step > gVar_Count_Info[num].Vmax)
				temp = gVar_Count_Info[num].Vmax;
			else
				temp += gVar_Count_Info[num].step;
			
		}
		else if (gVar_Count_Info[num].mode == 0)
		{
			if (temp - gVar_Count_Info[num].step < gVar_Count_Info[num].Vmin && temp > gVar_Count_Info[num].Vmin)
				temp = gVar_Count_Info[num].Vmin;
			else if (temp - gVar_Count_Info[num].step >  gVar_Count_Info[num].Vmax || temp <= gVar_Count_Info[num].Vmin)
				temp = gVar_Count_Info[num].Vmax;
			else
				temp -= gVar_Count_Info[num].step;
		}
	}
	else if ((var[gVar_Count_Info[num].c_add * 2] << 8) + var[gVar_Count_Info[num].c_add * 2 + 1] == gVar_Count_Info[num].V_once)
	{

		if (gVar_Count_Info[num].mode == 1)	//+
		{
			if (temp + gVar_Count_Info[num].step < gVar_Count_Info[num].Vmin)
				temp = gVar_Count_Info[num].Vmin;
			else if (temp + gVar_Count_Info[num].step > gVar_Count_Info[num].Vmax)
				temp = gVar_Count_Info[num].Vmax;
			else
				temp += gVar_Count_Info[num].step;
			if (temp == gVar_Count_Info[num].Vmax)
				finish = 1;
			
		}
		else if (gVar_Count_Info[num].mode == 0)	//-
		{
			if (temp - gVar_Count_Info[num].step < gVar_Count_Info[num].Vmin)
				temp = gVar_Count_Info[num].Vmin;
			else if (temp - gVar_Count_Info[num].step > gVar_Count_Info[num].Vmax)
				temp = gVar_Count_Info[num].Vmax;
			else
				temp -= gVar_Count_Info[num].step;
			
			if (temp == gVar_Count_Info[num].Vmin)
				finish = 1;
		}
	}
	
	if (gVar_Count_Info[num].v_type == 0 || gVar_Count_Info[num].v_type == 0x80)
	{
		var[gVar_Count_Info[num].v_add * 2 + 1] = temp & 0xFF;
	}
	else if (gVar_Count_Info[num].v_type == 1 || gVar_Count_Info[num].v_type == 0x81)
	{
		var[gVar_Count_Info[num].v_add * 2] = (temp >> 8) & 0xFF;
		var[gVar_Count_Info[num].v_add * 2 + 1] = temp & 0xFF;
	}
	else if (gVar_Count_Info[num].v_type == 2 || gVar_Count_Info[num].v_type == 0x82)
	{
		var[gVar_Count_Info[num].v_add * 2] = (temp >> 24) & 0xFF;
		var[gVar_Count_Info[num].v_add * 2 + 1] = (temp >> 16) & 0xFF;
		var[gVar_Count_Info[num].v_add * 2 + 2] = (temp >> 8) & 0xFF;
		var[gVar_Count_Info[num].v_add * 2 + 3] = temp & 0xFF;
	}
	else if (gVar_Count_Info[num].v_type == 3 || gVar_Count_Info[num].v_type == 0x83)
	{
		var[gVar_Count_Info[num].v_add * 2] = temp >> 56 & 0xFF;
		var[gVar_Count_Info[num].v_add * 2 + 1] = temp >> 48 & 0xFF;
		var[gVar_Count_Info[num].v_add * 2 + 2] = temp >> 40 & 0xFF;
		var[gVar_Count_Info[num].v_add * 2 + 3] = temp >> 32 & 0xFF;
		var[gVar_Count_Info[num].v_add * 2 + 4] = temp >> 24 & 0xFF;
		var[gVar_Count_Info[num].v_add * 2 + 5] = temp >> 16 & 0xFF;
		var[gVar_Count_Info[num].v_add * 2 + 6] = temp >> 8 & 0xFF;
		var[gVar_Count_Info[num].v_add * 2 + 7] = temp & 0xFF;
	}
	
	if (gVar_Count_Info[num].v_add >= 0x7000)
		reg_operation(gVar_Count_Info[num].v_add);
	page_date_update(pic_id);
	
	if (finish == 1)
	{
		var[gVar_Count_Info[num].c_add * 2] = (gVar_Count_Info[num].V_stop >> 8) & 0xFF;
		var[gVar_Count_Info[num].c_add * 2 + 1] = gVar_Count_Info[num].V_stop  & 0xFF;
		
		
		if (gVar_Count_Info[num].feedback == 1)
		{
			if (gVar_Count_Info[num].v_type == 0x00 || 
				gVar_Count_Info[num].v_type == 0x80 || 
				gVar_Count_Info[num].v_type == 0x01 || 
				gVar_Count_Info[num].v_type == 0x81)
			{
				rebuf[3] = 0x41;
				rebuf[4] = gVar_Count_Info[num].v_add >> 8;;
				rebuf[5] = gVar_Count_Info[num].v_add & 0xFF;;
				rebuf[6] = var[gVar_Count_Info[num].v_add * 2];
				rebuf[7] = var[gVar_Count_Info[num].v_add * 2 + 1];
				LT_SendData_CRC_Frame(rebuf, 5);
			}
			else if (gVar_Count_Info[num].v_type == 0x02 || gVar_Count_Info[num].v_type == 0x82)
			{
				rebuf[3] = 0x41;
				rebuf[4] = gVar_Count_Info[num].v_add >> 8;;
				rebuf[5] = gVar_Count_Info[num].v_add & 0xFF;;
				rebuf[6] = var[gVar_Count_Info[num].v_add * 2];
				rebuf[7] = var[gVar_Count_Info[num].v_add * 2 + 1];
				rebuf[8] = var[gVar_Count_Info[num].v_add * 2 + 2];
				rebuf[9] = var[gVar_Count_Info[num].v_add * 2 + 3];
				LT_SendData_CRC_Frame(rebuf, 7);
			}
			else if (gVar_Count_Info[num].v_type == 0x03 || gVar_Count_Info[num].v_type == 0x83)
			{
				rebuf[3] = 0x41;
				rebuf[4] = gVar_Count_Info[num].v_add >> 8;;
				rebuf[5] = gVar_Count_Info[num].v_add & 0xFF;;
				rebuf[6] = var[gVar_Count_Info[num].v_add * 2];
				rebuf[7] = var[gVar_Count_Info[num].v_add * 2 + 1];
				rebuf[8] = var[gVar_Count_Info[num].v_add * 2 + 2];
				rebuf[9] = var[gVar_Count_Info[num].v_add * 2 + 3];
				rebuf[10] = var[gVar_Count_Info[num].v_add * 2 + 4];
				rebuf[11] = var[gVar_Count_Info[num].v_add * 2 + 5];
				rebuf[12] = var[gVar_Count_Info[num].v_add * 2 + 6];
				rebuf[13] = var[gVar_Count_Info[num].v_add * 2 + 7];
				LT_SendData_CRC_Frame(rebuf, 11);
			}			
		}
		
		for (uint8_t i = 0; i < 8; i++)
		{
			if (gVar_Count_Info[num].var_addr[i] != 0xFFFF && gVar_Count_Info[num].var_addr[i] < 0x7200)
			{
				var[gVar_Count_Info[num].var_addr[i] * 2] = (gVar_Count_Info[num].var[i] >> 8) & 0xFF;
				var[gVar_Count_Info[num].var_addr[i] * 2 + 1] = gVar_Count_Info[num].var[i] & 0xFF;

				if (gVar_Count_Info[num].var_addr[i] > 0x7000)
					reg_operation(gVar_Count_Info[num].var_addr[i]);
				else if (gVar_Count_Info[num].var_addr[i] == 0x7000)
					page_flag = 1;
			}
		}
		
		if (page_flag == 1) 
			reg_operation(0x7000);
		page_date_update(pic_id);
	}
}


void Check_Var_Count(void)
{
	
	if (Var_Count_num > 0 && Var_Count_flag == 1)
	{
		Var_Count_flag = 0;
		for (uint8_t i = 0; i < Var_Count_num; i++)
			gVar_Count_Info[i].t_c++;
		for (uint8_t i = 0; i < Var_Count_num; i++)
		{
			if (gVar_Count_Info[i].t_c >= gVar_Count_Info[i].t_d)
			{
				if ((var[gVar_Count_Info[i].c_add * 2] << 8) + var[gVar_Count_Info[i].c_add * 2 + 1] == gVar_Count_Info[i].V_loop
					|| (var[gVar_Count_Info[i].c_add * 2] << 8) + var[gVar_Count_Info[i].c_add * 2 + 1] == gVar_Count_Info[i].V_once)
				Run_Var_Count(i); // When a window timing time expires, switch pictures
				gVar_Count_Info[i].t_c = 0;
			}
		}
	}
}



/************************/

/***********************************************************************************
* Function Name	: Pointer_display
* Description	: Get Pointer_display info.
* Input			: - type: time counter info sources
				  - buff: time counter info
* Output		: None
* Return		: None
************************************************************************************/
Pointer_Info gPointer_Info[Pointer_count_size];
uint8_t Pointer_Count_num = 0;
uint8_t Pointer_Count_flag = 0;
void Pointer_display(uint8_t type, uint8_t *buff)
{
	uint16_t pointer = 0;
	uint8_t buff1[16] = {0};
	uint16_t canvas_w;
	if (type == 0)
	{
		gPointer_Info[Pointer_Count_num].v_add = (buff[5] + (buff[6] << 8));
		gPointer_Info[Pointer_Count_num].base_id = buff[7] + (buff[8] << 8);;
		gPointer_Info[Pointer_Count_num].x = buff[9] + (buff[10] << 8);
		gPointer_Info[Pointer_Count_num].y = buff[11] + (buff[12] << 8);
		gPointer_Info[Pointer_Count_num].w = buff[13] + (buff[14] << 8);
		gPointer_Info[Pointer_Count_num].h = buff[15] + (buff[16] << 8);
		gPointer_Info[Pointer_Count_num].Pointer_x = buff[17] + (buff[18] << 8);
		gPointer_Info[Pointer_Count_num].Pointer_y = buff[19] + (buff[20] << 8);
		gPointer_Info[Pointer_Count_num].Sangle = buff[21] + (buff[22] << 8);
		gPointer_Info[Pointer_Count_num].Eangle = buff[23] + (buff[24] << 8);
		gPointer_Info[Pointer_Count_num].once_angle = buff[25] + (buff[26] << 8);
		gPointer_Info[Pointer_Count_num].effect = buff[27];
		gPointer_Info[Pointer_Count_num].center_id = buff[28] + (buff[29] << 8);
		gPointer_Info[Pointer_Count_num].Show_Number = buff[30];
		gPointer_Info[Pointer_Count_num].Num_add = buff[31] + (buff[32] << 8);
		gPointer_Info[Pointer_Count_num].Num_type = buff[33];
		gPointer_Info[Pointer_Count_num].Num_x = buff[34] + (buff[35] << 8);
		gPointer_Info[Pointer_Count_num].Num_y = buff[36] + (buff[37] << 8);
		gPointer_Info[Pointer_Count_num].Num_sid = buff[38] + (buff[39] << 8);
		gPointer_Info[Pointer_Count_num].Num_eid = buff[40] + (buff[41] << 8);
		gPointer_Info[Pointer_Count_num].Num_Alig = buff[42] & 0x7F;
		gPointer_Info[Pointer_Count_num].leading_zero = (buff[42] & 0x80) >> 7;
		gPointer_Info[Pointer_Count_num].Num_Nint = buff[43];
		gPointer_Info[Pointer_Count_num].Num_Dint = buff[44];
		gPointer_Info[Pointer_Count_num].mode = buff[45];
		gPointer_Info[Pointer_Count_num].Pointer_w = (buff[46] + (buff[47] << 8)) / 2;
		gPointer_Info[Pointer_Count_num].L1 = buff[48] + (buff[49] << 8);
		gPointer_Info[Pointer_Count_num].L1_colour = buff[50] + (buff[51] << 8) + (buff[52] << 16);
		gPointer_Info[Pointer_Count_num].L2 = buff[53] + (buff[54] << 8);
		gPointer_Info[Pointer_Count_num].L2_colour = buff[55] + (buff[56] << 8) + (buff[57] << 16);
		gPointer_Info[Pointer_Count_num].Pointer_sid = buff[58] + (buff[59] << 8);
		gPointer_Info[Pointer_Count_num].Pointer_eid = buff[60] + (buff[61] << 8);
		gPointer_Info[Pointer_Count_num].Pointer_id = buff[62] + (buff[63] << 8);
	}
	else if (type == 1)
	{
		pointer = (buff[2] + (buff[3] << 8)) * 2;
		gPointer_Info[Pointer_Count_num].v_add = (var[pointer + 1] + (var[pointer + 2] << 8));
		gPointer_Info[Pointer_Count_num].base_id = var[pointer + 3] + (var[pointer + 4] << 8);;
		gPointer_Info[Pointer_Count_num].x = var[pointer + 5] + (var[pointer + 6] << 8);
		gPointer_Info[Pointer_Count_num].y = var[pointer + 7] + (var[pointer + 8] << 8);
		gPointer_Info[Pointer_Count_num].w = var[pointer + 9] + (var[pointer + 10] << 8);
		gPointer_Info[Pointer_Count_num].h = var[pointer + 11] + (var[pointer + 12] << 8);
		gPointer_Info[Pointer_Count_num].Pointer_x = var[pointer + 13] + (var[pointer + 14] << 8);
		gPointer_Info[Pointer_Count_num].Pointer_y = var[pointer + 15] + (var[pointer + 16] << 8);
		gPointer_Info[Pointer_Count_num].Sangle = var[pointer + 17] + (var[pointer + 18] << 8);
		gPointer_Info[Pointer_Count_num].Eangle = var[pointer + 19] + (var[pointer + 20] << 8);
		gPointer_Info[Pointer_Count_num].once_angle = var[pointer + 21] + (var[pointer + 22] << 8);
		gPointer_Info[Pointer_Count_num].effect = var[pointer + 23];
		gPointer_Info[Pointer_Count_num].center_id = var[pointer + 24] + (var[pointer + 25] << 8);
		gPointer_Info[Pointer_Count_num].Show_Number = var[pointer + 26];
		gPointer_Info[Pointer_Count_num].Num_add = var[pointer + 27] + (var[pointer + 28] << 8);
		gPointer_Info[Pointer_Count_num].Num_type = var[pointer + 29];
		gPointer_Info[Pointer_Count_num].Num_x = var[pointer + 30] + (var[pointer + 31] << 8);
		gPointer_Info[Pointer_Count_num].Num_y = var[pointer + 32] + (var[pointer + 33] << 8);
		gPointer_Info[Pointer_Count_num].Num_sid = var[pointer + 34] + (var[pointer + 35] << 8);
		gPointer_Info[Pointer_Count_num].Num_eid = var[pointer + 36] + (var[pointer + 37] << 8);
		gPointer_Info[Pointer_Count_num].Num_Alig = var[pointer + 38] & 0x7F;
		gPointer_Info[Pointer_Count_num].leading_zero = (var[pointer + 38] & 0x80) >> 7;
		gPointer_Info[Pointer_Count_num].Num_Nint = var[pointer + 39];
		gPointer_Info[Pointer_Count_num].Num_Dint = var[pointer + 40];
		gPointer_Info[Pointer_Count_num].mode = var[pointer + 41];
		gPointer_Info[Pointer_Count_num].Pointer_w = (var[pointer + 42] + (var[pointer + 43] << 8)) / 2;
		gPointer_Info[Pointer_Count_num].L1 = var[pointer + 44] + (var[pointer + 45] << 8);
		gPointer_Info[Pointer_Count_num].L1_colour = var[pointer + 46] + (var[pointer + 47] << 8) + (var[pointer + 48] << 16);
		gPointer_Info[Pointer_Count_num].L2 = var[pointer + 49] + (var[pointer + 50] << 8);
		gPointer_Info[Pointer_Count_num].L2_colour = var[pointer + 51] + (var[pointer + 52] << 8) + (var[pointer + 53] << 16);
		gPointer_Info[Pointer_Count_num].Pointer_sid = var[pointer + 54] + (var[pointer + 55] << 8);
		gPointer_Info[Pointer_Count_num].Pointer_eid = var[pointer + 56] + (var[pointer + 57] << 8);
		gPointer_Info[Pointer_Count_num].Pointer_id = var[pointer + 58] + (var[pointer + 59] << 8);
	}

	gPointer_Info[Pointer_Count_num].current_angle = (var[gPointer_Info[Pointer_Count_num].v_add * 2] << 8) + var[gPointer_Info[Pointer_Count_num].v_add * 2 + 1];
	if (gPointer_Info[Pointer_Count_num].base_id != 0xFFFF)
	{			
		Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gPointer_Info[Pointer_Count_num].base_id , 12);
		pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
		pic_w = buff1[4] + (buff1[5] << 8);
		pic_h = buff1[6] + (buff1[7] << 8);
		pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
		pic_flag = buff1[11];
	
		if (pic_len != 0 && pic_flag != 0xFF)
		{
			canvas_w = Get_Canvas_Width(pic_w);
			Canvas_Image_Start_address(LAY_BUFF1);
			Canvas_image_width(canvas_w);
			if (pic_flag == 0)
			{
				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													pic_w, pic_h,
													pic_w, pic_add,
													LAY_BUFF1, canvas_w);

				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF11, LCD_XSIZE_TFT, gPointer_Info[Pointer_Count_num].x, gPointer_Info[Pointer_Count_num].y,
									  0x0c, pic_w, pic_h);
			}
			else if (pic_flag == 1)
			{
				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													pic_w, pic_h,
													pic_w, pic_add,
													LAY_BUFF1, canvas_w);

				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
												 LAY_BUFF11, LCD_XSIZE_TFT, gPointer_Info[Pointer_Count_num].x, gPointer_Info[Pointer_Count_num].y,
												 Black, pic_w, pic_h);
			}
			else if (pic_flag  == 2)
			{
				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													pic_w, pic_h,
													pic_w, pic_add,
													LAY_BUFF1, canvas_w);
				
				BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, gPointer_Info[Pointer_Count_num].x, gPointer_Info[Pointer_Count_num].y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF11, LCD_XSIZE_TFT, gPointer_Info[Pointer_Count_num].x, gPointer_Info[Pointer_Count_num].y,
											   pic_w, pic_h);
				
			}
		}
	}
	else 
	{
		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gPointer_Info[Pointer_Count_num].x, gPointer_Info[Pointer_Count_num].y,
							MAIN_BUFF, LCD_XSIZE_TFT, gPointer_Info[Pointer_Count_num].x, gPointer_Info[Pointer_Count_num].y,
							LAY_BUFF11, LCD_XSIZE_TFT, gPointer_Info[Pointer_Count_num].x, gPointer_Info[Pointer_Count_num].y,
							0x0c, gPointer_Info[Pointer_Count_num].w, gPointer_Info[Pointer_Count_num].h);
	}
	

	Run_Pointer(Pointer_Count_num);
	Pointer_Count_num++;
}

uint8_t Run_Pointer(uint8_t num)
{
	uint8_t n = 0, flag = 0, i = 0, kk = 0, Symbol = 0;
	uint8_t buff1[16] = {0};
	uint16_t canvas_w;
	int16_t angle, temp_angle;
	uint16_t x_src = 0, y_src = 0, w_des, h_des, x_des, y_des;
	uint16_t canvas_w1 = 0, canvas_w2 = 0, num_w, dot_w, h, canvas_numw, canvas_dotw;
	uint16_t xd = 0, dx = 0;
	uint16_t x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0, x4 = 0, y4 = 0;
	int16_t Pointer_x,Pointer_y;
	int32_t value = 0;
	int8_t value1 = 0;
	int16_t value2 = 0;

	
	if (DioKb_flag == 1) // In the keyboard input interface
	{
		if (!(gPointer_Info[num].x > gDio_Info[Dio_num].Xe || (gPointer_Info[num].x + gPointer_Info[num].w) < gDio_Info[Dio_num].Xs || gPointer_Info[num].y > gDio_Info[Dio_num].Ye || (gPointer_Info[num].y + gPointer_Info[num].h) < gDio_Info[Dio_num].Ys))
		{
			return 0;
		}
	}
	else if (AioKb_flag == 1) // In the keyboard input interface
	{
		if (!(gPointer_Info[num].x > gAio_Info[Aio_num].Xe || (gPointer_Info[num].x + gPointer_Info[num].w) < gAio_Info[Aio_num].Xs || gPointer_Info[num].y > gAio_Info[Aio_num].Ye || (gPointer_Info[num].y + gPointer_Info[num].h) < gAio_Info[Aio_num].Ys))
		{
			return 0;
		}
	}
	else if (GBKioKb_flag == 1) // In the keyboard input interface
	{
		if (!(gPointer_Info[num].x > gGBKio_Info[GBKio_num].Xe || (gPointer_Info[num].x + gPointer_Info[num].w) < gGBKio_Info[GBKio_num].Xs || gPointer_Info[num].y > gGBKio_Info[GBKio_num].Ye || (gPointer_Info[num].y + gPointer_Info[num].h) < gGBKio_Info[GBKio_num].Ys))
		{
			return 0;
		}
	}
	
	LT768_BTE_Memory_Copy(LAY_BUFF11, LCD_XSIZE_TFT, gPointer_Info[num].x, gPointer_Info[num].y,
						LAY_BUFF11, LCD_XSIZE_TFT, gPointer_Info[num].x, gPointer_Info[num].y,
						LAY_BUFF3, LCD_XSIZE_TFT, gPointer_Info[num].x, gPointer_Info[num].y,
						0x0c, gPointer_Info[num].w, gPointer_Info[num].h);
	


	
	if (gPointer_Info[num].Show_Number == 1)
	{
		memset(Display_buff, 0, 20);
		if (gPointer_Info[num].Num_type == 0 )
		{
			value = var[gPointer_Info[num].Num_add * 2 + 1];
			var_temp[gPointer_Info[num].Num_add * 2 + 1] = var[gPointer_Info[num].Num_add * 2 + 1];
		}
		else if(gPointer_Info[num].Num_type == 0x80 )
		{
			value1 = var[gPointer_Info[num].Num_add * 2 + 1];
			value = value1;
		}
		else if (gPointer_Info[num].Num_type == 1) 
		{
			value = (var[gPointer_Info[num].Num_add * 2] << 8) + var[gPointer_Info[num].Num_add * 2 + 1];
			var_temp[gPointer_Info[num].Num_add * 2] = var[gPointer_Info[num].Num_add * 2];
			var_temp[gPointer_Info[num].Num_add * 2 + 1] = var[gPointer_Info[num].Num_add * 2 + 1];
		}
		else if (gPointer_Info[num].Num_type == 0x81) 
		{
			value2 = (var[gPointer_Info[num].Num_add * 2] << 8) + var[gPointer_Info[num].Num_add * 2 + 1];
			var_temp[gPointer_Info[num].Num_add * 2] = var[gPointer_Info[num].Num_add * 2];
			var_temp[gPointer_Info[num].Num_add * 2 + 1] = var[gPointer_Info[num].Num_add * 2 + 1];
			value = value2;
		}
		else if (gPointer_Info[num].Num_type == 2 || gPointer_Info[num].Num_type == 0x82)
		{
			value = (var[gPointer_Info[num].Num_add * 2] << 24) + (var[gPointer_Info[num].Num_add * 2 + 1] << 16) + (var[gPointer_Info[num].Num_add * 2 + 2] << 8) + var[gPointer_Info[num].Num_add * 2 + 3];
			var_temp[gPointer_Info[num].Num_add * 2] = var[gPointer_Info[num].Num_add * 2];
			var_temp[gPointer_Info[num].Num_add * 2 + 1] = var[gPointer_Info[num].Num_add * 2 + 1];
			var_temp[gPointer_Info[num].Num_add * 2 + 2] = var[gPointer_Info[num].Num_add * 2 + 2];
			var_temp[gPointer_Info[num].Num_add * 2 + 3] = var[gPointer_Info[num].Num_add * 2 + 3];
		}
		if (value < 0)
		{
			Display_buff[kk] = 11;
			kk++;
			value = ~value + 1;
		}
		
		value = value % ((int)pow(10, gPointer_Info[num].Num_Nint + gPointer_Info[num].Num_Dint));

		for (i = 0; i < gPointer_Info[num].Num_Nint; i++) // All integers
		{

			Display_buff[kk] = value / (int)pow(10, gPointer_Info[num].Num_Nint - i - 1 + gPointer_Info[num].Num_Dint);

			if (gPointer_Info[num].leading_zero == 0)
			{
				if (Display_buff[kk] != 0 || flag == 1)
				{
					kk++;
					flag = 1; // The first digit is not 0
				}
			}
			else
			{
				kk++;
				flag = 1; // The first digit is not 0
			}
			value = value % (int)pow(10, gPointer_Info[num].Num_Nint - i - 1 + gPointer_Info[num].Num_Dint);
		}

		if (gPointer_Info[num].Num_Dint > 0)
		{
			if (flag == 0)
			{
				Display_buff[kk] = 0; //"."
				kk++;
			}
			Display_buff[kk] = 10; //"."
			kk++;
		}
		else
		{
			if (flag == 0)
			{
				Display_buff[kk] = 0; //"."
				kk++;
			}
		}
		for (i = 0; i < gPointer_Info[num].Num_Dint; i++) // Decimal
		{
			Display_buff[kk] = value / (int)pow(10, gPointer_Info[num].Num_Nint- i - 1);
			kk++;
			value = value % (int)pow(10, i - 1 + gPointer_Info[num].Num_Dint);
		}
		
		xd = 0;
		for (i = 0; i < kk; i++) // show
		{
			
			if (gPointer_Info[num].Num_sid + Display_buff[i] >= addr_index[11])
				continue;
	
			Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * (gPointer_Info[num].Num_sid + Display_buff[i]), 12);
			pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
			num_w = buff1[4] + (buff1[5] << 8);
			h = buff1[6] + (buff1[7] << 8);
			pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
			pic_flag = buff1[11];

			if (pic_len != 0 && pic_flag != 0xFF)
			{
				if (Screen_DIR == 0)
				{
					x_src = xd;
					y_src = 0;
					w_des = num_w;
				}
				else if (Screen_DIR == 1)
				{
					x_src = LCD_XSIZE_TFT - num_w;
					y_src = xd;
					w_des = h;
				}
				else if (Screen_DIR == 2)
				{
					x_src = LCD_XSIZE_TFT - num_w - xd;
					y_src = LCD_YSIZE_TFT - h;
					w_des = num_w;
				}
				else if (Screen_DIR == 3)
				{
					x_src = 0;
					y_src = LCD_YSIZE_TFT - h - xd;
					w_des = h;
				}

				Canvas_Image_Start_address(LAY_BUFF2);
				Canvas_image_width(LCD_XSIZE_TFT);
				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, x_src, y_src,
													num_w, h,
													num_w, pic_add,
													LAY_BUFF2, LCD_XSIZE_TFT);

				xd += w_des;
			}
		}

		if (Screen_DIR == 0)
		{
			x_src = 0;
			y_src = 0;
			w_des = xd;
			h_des = h;
			if (gPointer_Info[num].Num_Alig == 0)		// Left alignment
			{
				x_des = gPointer_Info[num].Num_x;
				y_des = gPointer_Info[num].Num_y;
			}
			else if (gPointer_Info[num].Num_Alig == 1)	// Mid alignment
			{
				x_des = gPointer_Info[num].Num_x - w_des / 2;
				y_des = gPointer_Info[num].Num_y;
			}
			else if (gPointer_Info[num].Num_Alig == 2)	// Right alignment
			{
				x_des = gPointer_Info[num].Num_x - xd;
				y_des = gPointer_Info[num].Num_y;
			}
		}
		else if (Screen_DIR == 1)
		{
			x_src = LCD_XSIZE_TFT - num_w;
			y_src = 0;
			w_des = num_w;
			h_des = xd;
			if (gPointer_Info[num].Num_Alig == 0) // Left alignment
			{
				x_des = gPointer_Info[num].Num_x - w_des + 1;
				y_des = gPointer_Info[num].Num_y;
			}
			else if (gPointer_Info[num].Num_Alig == 1) // Mid alignment
			{
				x_des = gPointer_Info[num].Num_x - w_des + 1;
				y_des = gPointer_Info[num].Num_y - h_des / 2 + 1;
			}
			else if (gPointer_Info[num].Num_Alig == 2) // Right alignment
			{
				x_des = gPointer_Info[num].Num_x - w_des + 1;
				y_des = gPointer_Info[num].Num_y - h_des + 1;
			}
		}
		else if (Screen_DIR == 2)
		{
			x_src = LCD_XSIZE_TFT - xd;
			y_src = LCD_YSIZE_TFT - h;
			w_des = xd;
			h_des = h;
			if (gPointer_Info[num].Num_Alig == 0) // Left alignment
			{
				x_des = gPointer_Info[num].Num_x - w_des + 1;
				y_des = gPointer_Info[num].Num_y - h_des + 1;
			}
			else if (gPointer_Info[num].Num_Alig == 1) // Mid alignment
			{
				x_des = gPointer_Info[num].Num_x - w_des / 2 + 1;
				y_des = gPointer_Info[num].Num_y - h_des + 1;

			}
			else if (gPointer_Info[num].Num_Alig == 2) // Right alignment
			{
				x_des = gPointer_Info[num].Num_x;
				y_des = gPointer_Info[num].Num_y - h_des + 1;
			}
		}
		else if (Screen_DIR == 3)
		{
			x_src = 0;
			y_src = LCD_YSIZE_TFT - xd;
			w_des = num_w;
			h_des = xd;
			if (gPointer_Info[num].Num_Alig == 0) // Left alignment
			{
				x_des = gPointer_Info[num].Num_x;
				y_des = gPointer_Info[num].Num_y - xd;
			}
			else if (gPointer_Info[num].Num_Alig == 1) // Mid alignment
			{
				x_des = gPointer_Info[num].Num_x;
				y_des = gPointer_Info[num].Num_y - xd / 2;
			}
			else if (gPointer_Info[num].Num_Alig == 2) // Right alignment
			{
				x_des = gPointer_Info[num].Num_x;
				y_des = gPointer_Info[num].Num_y;
			}
		}

		if (pic_flag == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
								  LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
								  LAY_BUFF3, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_des, h_des);
		}
		else if (pic_flag == 1)
		{

			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
											 LAY_BUFF3, LCD_XSIZE_TFT, x_des, y_des,
											 Black, w_des, h_des);
		}
		else if (pic_flag == 2)
		{
			BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF3, LCD_XSIZE_TFT, x_des, y_des,
										   LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
										   LAY_BUFF3, LCD_XSIZE_TFT, x_des, y_des,
										   w_des, h_des);
		}
	}
	
	if (((var[gPointer_Info[num].v_add * 2] << 8) + var[gPointer_Info[num].v_add * 2 + 1]) > gPointer_Info[num].Eangle)
	{
		var[gPointer_Info[num].v_add * 2] = (gPointer_Info[num].Eangle >> 8) & 0xFF;
		var[gPointer_Info[num].v_add * 2 + 1] = gPointer_Info[num].Eangle & 0xFF;
	}
	else if (((var[gPointer_Info[num].v_add * 2] << 8) + var[gPointer_Info[num].v_add * 2 + 1]) < gPointer_Info[num].Sangle)
	{
		var[gPointer_Info[num].v_add * 2] = (gPointer_Info[num].Sangle >> 8) & 0xFF;
		var[gPointer_Info[num].v_add * 2 + 1] = gPointer_Info[num].Sangle & 0xFF;
	}
	if (gPointer_Info[num].effect == 1)
	{
		temp_angle = (var[gPointer_Info[num].v_add * 2] << 8) + var[gPointer_Info[num].v_add * 2 + 1];
		if (gPointer_Info[num].current_angle < temp_angle)
		{
			gPointer_Info[num].current_angle += gPointer_Info[num].once_angle;
			if(gPointer_Info[num].current_angle >= temp_angle)
			{
				gPointer_Info[num].current_angle = temp_angle; 
				var_temp[gPointer_Info[num].v_add * 2] = var[gPointer_Info[num].v_add * 2];
				var_temp[gPointer_Info[num].v_add * 2 + 1] = var[gPointer_Info[num].v_add * 2 + 1];
			}
		}
		else if (gPointer_Info[num].current_angle > temp_angle)
		{
			gPointer_Info[num].current_angle -= gPointer_Info[num].once_angle;
			if(gPointer_Info[num].current_angle <= temp_angle)
			{
				gPointer_Info[num].current_angle = temp_angle;
				var_temp[gPointer_Info[num].v_add * 2] = var[gPointer_Info[num].v_add * 2];
				var_temp[gPointer_Info[num].v_add * 2 + 1] = var[gPointer_Info[num].v_add * 2 + 1];
				
			}
		}
		else if (gPointer_Info[num].current_angle == temp_angle)
		{
			var_temp[gPointer_Info[num].v_add * 2] = var[gPointer_Info[num].v_add * 2];
			var_temp[gPointer_Info[num].v_add * 2 + 1] = var[gPointer_Info[num].v_add * 2 + 1];
		}
	}
	else 
	{
		gPointer_Info[num].current_angle = (var[gPointer_Info[num].v_add * 2] << 8) + var[gPointer_Info[num].v_add * 2 + 1];
		var_temp[gPointer_Info[num].v_add * 2] = var[gPointer_Info[num].v_add * 2];
		var_temp[gPointer_Info[num].v_add * 2 + 1] = var[gPointer_Info[num].v_add * 2 + 1];
	}
	if (gPointer_Info[num].mode == 0 || gPointer_Info[num].mode == 1)
	{
		if (Screen_DIR == 0)
		{
			angle = gPointer_Info[num].current_angle;
		}
		else if (Screen_DIR == 1)
		{
			angle = gPointer_Info[num].current_angle + 90;
			angle = angle % 360;
		}
		else if (Screen_DIR == 2)
		{
			angle = gPointer_Info[num].current_angle + 180;
			angle = angle % 360;
		}
		if (Screen_DIR == 3)
		{
			angle = gPointer_Info[num].current_angle + 270;
			angle = angle % 360;
		}
	
		x1 = gPointer_Info[num].Pointer_x + gPointer_Info[num].Pointer_w * fast_sin(angle + 270);
		y1 = gPointer_Info[num].Pointer_y - gPointer_Info[num].Pointer_w * fast_cos(angle + 270);

		x2 = gPointer_Info[num].Pointer_x + gPointer_Info[num].L2 * fast_sin(angle);
		y2 = gPointer_Info[num].Pointer_y - gPointer_Info[num].L2 * fast_cos(angle);

		x3 = gPointer_Info[num].Pointer_x + gPointer_Info[num].Pointer_w * fast_sin(angle + 90);
		y3 = gPointer_Info[num].Pointer_y - gPointer_Info[num].Pointer_w * fast_cos(angle + 90);

		x4 = gPointer_Info[num].Pointer_x + gPointer_Info[num].L1 * fast_sin(angle + 180);
		y4 = gPointer_Info[num].Pointer_y - gPointer_Info[num].L1 * fast_cos(angle + 180);
		Canvas_Image_Start_address(LAY_BUFF3);
		Canvas_image_width(LCD_XSIZE_TFT);
		
		LT768_DrawTriangle_Fill(x1, y1, x2, y2, x4, y4, gPointer_Info[num].L2_colour);
		LT768_DrawTriangle_Fill(x3, y3, x2, y2, x4, y4, gPointer_Info[num].L1_colour);
		
		if (gPointer_Info[num].mode == 1)
		{
			Lay_temp = LAY_BUFF3;
			lineSDFAABB_BUFF(x1,y1,x2,y2,1,gPointer_Info[num].L2_colour,LCD_XSIZE_TFT,0);
			lineSDFAABB_BUFF(x1,y1,x4,y4,1,gPointer_Info[num].L2_colour,LCD_XSIZE_TFT,0);
			lineSDFAABB_BUFF(x2,y2,x3,y3,1,gPointer_Info[num].L1_colour,LCD_XSIZE_TFT,0);
			lineSDFAABB_BUFF(x3,y3,x4,y4,1,gPointer_Info[num].L1_colour,LCD_XSIZE_TFT,0);
		}
	}
	else if (gPointer_Info[num].mode == 4)
	{
		if (Screen_DIR == 0)
		{
			angle = gPointer_Info[num].current_angle;
		}
		else if (Screen_DIR == 1)
		{
			angle = gPointer_Info[num].current_angle + 90;
			angle = angle % 360;
		}
		else if (Screen_DIR == 2)
		{
			angle = gPointer_Info[num].current_angle + 180;
			angle = angle % 360;
		}
		if (Screen_DIR == 3)
		{
			angle = gPointer_Info[num].current_angle + 270;
			angle = angle % 360;
		}
		x1 = gPointer_Info[num].Pointer_x + gPointer_Info[num].L2 * fast_sin(angle);
		x2 = gPointer_Info[num].Pointer_x - gPointer_Info[num].L1 * fast_sin(angle);
		y1 = gPointer_Info[num].Pointer_y - gPointer_Info[num].L2 * fast_cos(angle);
		y2 = gPointer_Info[num].Pointer_y + gPointer_Info[num].L1 * fast_cos(angle);
		Lay_temp = LAY_BUFF3;
		
		
		lineSDFAABB_BUFF(x1,y1,x2,y2,gPointer_Info[num].Pointer_w,gPointer_Info[num].L1_colour,LCD_XSIZE_TFT,0);
	}
	else if (gPointer_Info[num].mode == 3)
	{
		temp_angle = (gPointer_Info[num].current_angle - gPointer_Info[num].Sangle) / gPointer_Info[num].once_angle;
		
		Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * (gPointer_Info[num].Pointer_sid + temp_angle), 12);
		pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
		pic_w = buff1[4] + (buff1[5] << 8);
		pic_h = buff1[6] + (buff1[7] << 8);
		pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
		pic_flag = buff1[11];

		Flash_Read_UI(buff1, pic_add, 4);
		Pointer_x = buff1[0] + (buff1[1] << 8);
		Pointer_y = buff1[2] + (buff1[3] << 8);

		if (pic_len != 0 && pic_flag != 0xFF)
		{
			canvas_w = Get_Canvas_Width(pic_w);
			Canvas_Image_Start_address(LAY_BUFF1);
			Canvas_image_width(canvas_w);
			if (pic_flag  == 2)
			{
				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													pic_w, pic_h,
													pic_w, pic_add + 4,
													LAY_BUFF1, canvas_w);
				
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF3, LCD_XSIZE_TFT, gPointer_Info[num].Pointer_x - gPointer_Info[num].L1 + Pointer_x , gPointer_Info[num].Pointer_y - gPointer_Info[num].L1 + Pointer_y ,
											   LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF3, LCD_XSIZE_TFT, gPointer_Info[num].Pointer_x - gPointer_Info[num].L1 + Pointer_x, gPointer_Info[num].Pointer_y - gPointer_Info[num].L1 + Pointer_y ,
											   pic_w, pic_h);
				
			}
		}
	}		
	
	if (gPointer_Info[num].center_id != 0xFFFF )
	{			
		Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gPointer_Info[num].center_id , 12);
		pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
		pic_w = buff1[4] + (buff1[5] << 8);
		pic_h = buff1[6] + (buff1[7] << 8);
		pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
		pic_flag = buff1[11];
		

		if (pic_len != 0 && pic_flag != 0xFF)
		{
			canvas_w = Get_Canvas_Width(pic_w);
			Canvas_Image_Start_address(LAY_BUFF1);
			Canvas_image_width(canvas_w);
			if (pic_flag == 0)
			{
				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													pic_w, pic_h,
													pic_w, pic_add,
													LAY_BUFF1, canvas_w);

				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF3, LCD_XSIZE_TFT, gPointer_Info[num].Pointer_x - pic_w/2, gPointer_Info[num].Pointer_y - pic_h/2,
									  0x0c, pic_w, pic_h);
			}
			else if (pic_flag == 1)
			{
				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													pic_w, pic_h,
													pic_w, pic_add,
													LAY_BUFF1, canvas_w);

				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
												 LAY_BUFF3, LCD_XSIZE_TFT, gPointer_Info[num].Pointer_x - pic_w/2, gPointer_Info[num].Pointer_y - pic_h/2,
												 Black, pic_w, pic_h);
			}
			else if (pic_flag  == 2)
			{
				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													pic_w, pic_h,
													pic_w, pic_add,
													LAY_BUFF1, canvas_w);
				
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF3, LCD_XSIZE_TFT, gPointer_Info[num].Pointer_x - pic_w/2, gPointer_Info[num].Pointer_y - pic_h/2,
											   LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF3, LCD_XSIZE_TFT, gPointer_Info[num].Pointer_x - pic_w/2, gPointer_Info[num].Pointer_y - pic_h/2,
											   pic_w, pic_h);
				
			}
		}
	}
	
	LT768_BTE_Memory_Copy(LAY_BUFF3, LCD_XSIZE_TFT, gPointer_Info[num].x, gPointer_Info[num].y,
						  LAY_BUFF3, LCD_XSIZE_TFT, gPointer_Info[num].x, gPointer_Info[num].y,
						  Display_Layer, LCD_XSIZE_TFT, gPointer_Info[num].x, gPointer_Info[num].y,
						  0x0c, gPointer_Info[num].w, gPointer_Info[num].h);

}

void Check_Pointer(void)
{
	if (Pointer_Count_num > 0 && Pointer_Count_flag == 1)
	{
		Pointer_Count_flag = 0;
		for (uint8_t i = 0; i < Pointer_Count_num; i++)
		{
			if (var[gPointer_Info[i].v_add * 2] != var_temp[gPointer_Info[i].v_add * 2] ||
				var[gPointer_Info[i].v_add * 2 + 1] != var_temp[gPointer_Info[i].v_add * 2 + 1] ||
				gPointer_Info[i].current_angle != ((var[gPointer_Info[i].v_add * 2] << 8) + var[gPointer_Info[i].v_add * 2 + 1]))
			{
				Run_Pointer(i); 
			}
			else if ((gPointer_Info[i].Num_type == 0 || gPointer_Info[i].Num_type == 0x80) &&
				var[gPointer_Info[i].Num_add * 2 + 1] != var_temp[gPointer_Info[i].Num_add * 2 + 1])
			{
				Run_Pointer(i); 
			}
			else if ((gPointer_Info[i].Num_type == 1 || gPointer_Info[i].Num_type == 0x81) &&
				(var[gPointer_Info[i].Num_add * 2] != var_temp[gPointer_Info[i].Num_add * 2] ||
				var[gPointer_Info[i].Num_add * 2 + 1] != var_temp[gPointer_Info[i].Num_add * 2 + 1]))
			{
				Run_Pointer(i); 
			}
			else if ((gPointer_Info[i].Num_type == 2 || gPointer_Info[i].Num_type == 0x82) &&
				(var[gPointer_Info[i].Num_add * 2] != var_temp[gPointer_Info[i].Num_add * 2] ||
				var[gPointer_Info[i].Num_add * 2 + 1] != var_temp[gPointer_Info[i].Num_add * 2 + 1]||
				var[gPointer_Info[i].Num_add * 2 + 2] != var_temp[gPointer_Info[i].Num_add * 2 + 2]||
				var[gPointer_Info[i].Num_add * 2 + 3] != var_temp[gPointer_Info[i].Num_add * 2 + 3]))
			{
				Run_Pointer(i); 
			}
			
		}
	}
}

/***************************/
uint8_t buzzer_start = 0;
uint8_t buzzer_10ms_count = 0;
void touch_buzzer(void)
{
	buzzer_start = 1;
//	UART_WriteGpioData(SCI2, UART_TX, Bit_SET);
    SPI_WriteGpioData(SPI3, SPI_MOSI,Bit_SET);
}

/***********************************************************************************
 * Function Name	: Basic_touch
 * Description		: Touch detection of Basic_touch.
 * Input			: None
 * Output			: None
 * Return			: None
 ************************************************************************************/
void Basic_touch(void)
{
	uint8_t i = 0;
	uint8_t rebuf[20];
	
	if (gTpInfo.sta) // press down
	{
		for (i = 0; i < Basci_count; i++)
		{
			if (gTpInfo.x[0] >= gBasci_Info[i].Xs && gTpInfo.y[0] >= gBasci_Info[i].Ys &&
				gTpInfo.x[0] <= gBasci_Info[i].Xe && gTpInfo.y[0] <= gBasci_Info[i].Ye &&
				Basci_flag == 0 && button_Press_flag == 0)
			{
				if (gBasci_Info[i].press_id != 0xFFFF)
				{
					Display_Icon(gBasci_Info[i].press_id, gBasci_Info[i].Xs, gBasci_Info[i].Ys, 0);
				}
				Basci_num = i;
				Basci_flag = 1;
				button_Press_flag = 1;
				if (buzzer)
					touch_buzzer();
			}
		}
	}
	
	if (Button_Key_Code_Flag == 1)
	{
		for (i = 0; i < Basci_count; i++)
		{
			if (gBasci_Info[i].Xs == 0xFFFF && gBasci_Info[i].Ys ==  Button_Key_Code)
			{
				Button_Key_Code_Flag = 0;
				Button_Key_Code = 0;
				if (gBasci_Info[i].Code == 0xC001)
				{
					rebuf[3] = 0x41;
					rebuf[4] = 0xFF;
					rebuf[5] = 0xFF;
					rebuf[6] = gBasci_Info[i].Keyvalue >> 8;
					rebuf[7] = gBasci_Info[i].Keyvalue & 0xFF;
					LT_SendData_CRC_Frame(rebuf, 5);
				}

				if (gBasci_Info[i].Next_id != 0xFFFF)
				{
					Display_page(gBasci_Info[i].Next_id);
				}
			}
		}
	}
	if (gTpInfo.sta == 0 && Basci_flag == 1)
	{
		if (gBasci_Info[Basci_num].Code == 0xC001)
		{
			rebuf[3] = 0x41;
			rebuf[4] = 0xFF;
			rebuf[5] = 0xFF;
			rebuf[6] = gBasci_Info[Basci_num].Keyvalue >> 8;
			rebuf[7] = gBasci_Info[Basci_num].Keyvalue & 0xFF;
			LT_SendData_CRC_Frame(rebuf, 5);
		}
		if (gBasci_Info[Basci_num].id != 0xFFFF)
		{
			Display_Icon(gBasci_Info[Basci_num].id, gBasci_Info[Basci_num].Xs, gBasci_Info[Basci_num].Ys, 0);
		}
		Basci_flag = 0;
		button_Press_flag = 0;
		if (gBasci_Info[Basci_num].Next_id != 0xFFFF)
		{
			Display_page(gBasci_Info[Basci_num].Next_id);
		}
	}
	
}
/******************/

/***********************************************************************************
 * Function Name	: Adj_touch
 * Description		: Touch detection of variable adjustment.
 * Input			: None
 * Output			: None
 * Return			: None
 ************************************************************************************/

void Adj_touch(void)
{
	uint8_t i = 0, rebuf[20], num = 0;
	int8_t sum0 = 0;
	int16_t sum1 = 0;
	int32_t sum = 0;
	
	if (gTpInfo.sta) // press down
	{
		for (i = 0; i < Adj_count; i++)
		{
			if (gTpInfo.x[0] >= gAdj_Info[i].Xs && gTpInfo.y[0] >= gAdj_Info[i].Ys &&
				gTpInfo.x[0] <= gAdj_Info[i].Xe && gTpInfo.y[0] <= gAdj_Info[i].Ye &&
				Adj_flag == 0 && button_Press_flag == 0)
			{
				if (gAdj_Info[i].press_id != 0xFFFF)
				{
					Display_Icon(gAdj_Info[i].press_id, gAdj_Info[i].Xs, gAdj_Info[i].Ys, 0);
				}
				Adj_num = i;
				Adj_flag = 1;
				button_Press_flag = 1;
				if (gAdj_Info[i].key_mode == 1)
					Adj_loop = 1;

				if (buzzer)
					touch_buzzer();
			}
		}
	}

	if (((Adj_loop == 1 && Adj_time > 50)||(Adj_loop == 2 && Adj_time > 10)) && gTpInfo.sta == 1 && Adj_flag == 1)
	{
		Adj_loop = 2;
		Adj_time = 0;
		if (gAdj_Info[Adj_num].p_tpye == 0x00)
			sum = var[gAdj_Info[Adj_num].p_add * 2 + 1];
		else if (gAdj_Info[Adj_num].p_tpye == 0x80)
		{
			sum0 = var[gAdj_Info[Adj_num].p_add * 2 + 1];
			sum = sum0;
		}
		else if (gAdj_Info[Adj_num].p_tpye == 0x01)
			sum = (var[gAdj_Info[Adj_num].p_add * 2] << 8) + var[gAdj_Info[Adj_num].p_add * 2 + 1];
		else if (gAdj_Info[Adj_num].p_tpye == 0x81)
		{
			sum1 = (var[gAdj_Info[Adj_num].p_add * 2] << 8) + var[gAdj_Info[Adj_num].p_add * 2 + 1];
			sum = sum1;
		}
		else if (gAdj_Info[Adj_num].p_tpye == 0x02)
		{
			if (var[gAdj_Info[Adj_num].p_add * 2] >= 0x80)
				var[gAdj_Info[Adj_num].p_add * 2] = 0x40;
			sum = (var[gAdj_Info[Adj_num].p_add * 2] << 24) +
				  (var[gAdj_Info[Adj_num].p_add * 2 + 1] << 16) +
				  (var[gAdj_Info[Adj_num].p_add * 2 + 2] << 8) +
				  var[gAdj_Info[Adj_num].p_add * 2 + 3];
		}
		else if (gAdj_Info[Adj_num].p_tpye == 0x82)
			sum = (var[gAdj_Info[Adj_num].p_add * 2] << 24) +
				  (var[gAdj_Info[Adj_num].p_add * 2 + 1] << 16) +
				  (var[gAdj_Info[Adj_num].p_add * 2 + 2] << 8) +
				  var[gAdj_Info[Adj_num].p_add * 2 + 3];

		else if (gAdj_Info[Adj_num].p_tpye == 0x83)
		{
			sum = (var[gAdj_Info[Adj_num].p_add * 2 + 6] << 8) + var[gAdj_Info[Adj_num].p_add * 2 + 7];
			if (var[gAdj_Info[Adj_num].p_add * 2] >= 0x80)
				sum = sum + (0x80 << 24);
		}
		else if (gAdj_Info[Adj_num].p_tpye >= 0x40 && gAdj_Info[Adj_num].p_tpye <= 0x4F)
			sum = (var[gAdj_Info[Adj_num].p_add * 2] << 8) + var[gAdj_Info[Adj_num].p_add * 2 + 1];

		if (gAdj_Info[Adj_num].Adj_mode == 1) //+
		{
			if (gAdj_Info[Adj_num].p_tpye & 0x40)
			{
				num = gAdj_Info[Adj_num].p_tpye & 0x0F;
				if (sum & (1 << num))
					sum &= ~(1 << num);
				else
					sum |= (1 << num);
			}
			else
			{
				sum += gAdj_Info[Adj_num].Adj_step;
				if (sum > gAdj_Info[Adj_num].V_max)
				{
					if (gAdj_Info[Adj_num].Return_Mode == 0)
						sum = gAdj_Info[Adj_num].V_max;
					else
						sum = gAdj_Info[Adj_num].V_min;
				}
				else if (sum < gAdj_Info[Adj_num].V_min)
					sum = gAdj_Info[Adj_num].V_min;
			}
		}
		else // -
		{
			if (gAdj_Info[Adj_num].p_tpye & 0x40)
			{
				num = gAdj_Info[Adj_num].p_tpye & 0x0F;
				if (sum & (1 << num))
					sum &= ~(1 << num);
				else
					sum |= (1 << num);
			}
			else
			{
				sum -= gAdj_Info[Adj_num].Adj_step;

				if (sum < gAdj_Info[Adj_num].V_min)
				{
					if (gAdj_Info[Adj_num].Return_Mode == 0)
						sum = gAdj_Info[Adj_num].V_min;
					else
						sum = gAdj_Info[Adj_num].V_max;
				}
				else if (sum > gAdj_Info[Adj_num].V_max)
					sum = gAdj_Info[Adj_num].V_max;
			}
		}

		if (gAdj_Info[Adj_num].p_tpye == 0x00 || gAdj_Info[Adj_num].p_tpye == 0x80)
		{
			var[gAdj_Info[Adj_num].p_add * 2 + 1] = sum;
		}
		else if (gAdj_Info[Adj_num].p_tpye == 0x01 || gAdj_Info[Adj_num].p_tpye == 0x81)
		{
			var[gAdj_Info[Adj_num].p_add * 2] = sum >> 8 & 0xFF;
			var[gAdj_Info[Adj_num].p_add * 2 + 1] = sum & 0xFF;
		}
		else if (gAdj_Info[Adj_num].p_tpye == 0x02 || gAdj_Info[Adj_num].p_tpye == 0x82)
		{
			if (sum < 0)
			{
				var[gAdj_Info[Adj_num].p_add * 2] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 1] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 2] = sum >> 8 & 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 3] = sum & 0xFF;
			}
			else
			{
				var[gAdj_Info[Adj_num].p_add * 2] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 1] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 2] = sum >> 8 & 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 3] = sum & 0xFF;
			}
		}
		else if (gAdj_Info[Adj_num].p_tpye == 0x83)
		{
			if (sum < 0)
			{
				var[gAdj_Info[Adj_num].p_add * 2] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 1] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 2] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 3] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 4] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 5] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 6] = sum >> 8 & 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 7] = sum & 0xFF;
			}
			else
			{
				var[gAdj_Info[Adj_num].p_add * 2] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 1] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 2] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 3] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 4] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 5] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 6] = sum >> 8 & 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 7] = sum & 0xFF;
			}
		}
		else if (gAdj_Info[Adj_num].p_tpye >= 0x40 && gAdj_Info[Adj_num].p_tpye <= 0x4F)
		{
			var[gAdj_Info[Adj_num].p_add * 2] = sum >> 8 & 0xFF;
			var[gAdj_Info[Adj_num].p_add * 2 + 1] = sum & 0xFF;
		}

		if (gAdj_Info[Adj_num].p_add >= 0x7000)
			reg_operation(gAdj_Info[Adj_num].p_add);

		page_date_update(pic_id);
	}

	
	
	if (Button_Key_Code_Flag == 1)
	{
		for (i = 0; i < Adj_count; i++)
		{
			if (gAdj_Info[i].Xs == 0xFFFF && gAdj_Info[i].Ys ==  Button_Key_Code)
			{
				Button_Key_Code_Flag = 0;
				Button_Key_Code = 0;
				if (gAdj_Info[i].p_tpye == 0x00)
					sum = var[gAdj_Info[i].p_add * 2 + 1];
				else if (gAdj_Info[i].p_tpye == 0x80)
				{
					sum0 = var[gAdj_Info[i].p_add * 2 + 1];
					sum = sum0;
				}
				else if (gAdj_Info[i].p_tpye == 0x01)
					sum = (var[gAdj_Info[i].p_add * 2] << 8) + var[gAdj_Info[i].p_add * 2 + 1];
				else if (gAdj_Info[i].p_tpye == 0x81)
				{
					sum1 = (var[gAdj_Info[i].p_add * 2] << 8) + var[gAdj_Info[i].p_add * 2 + 1];
					sum = sum1;
				}
				else if (gAdj_Info[i].p_tpye == 0x02)
				{
					if (var[gAdj_Info[i].p_add * 2] >= 0x80)
						var[gAdj_Info[i].p_add * 2] = 0x40;
					sum = (var[gAdj_Info[i].p_add * 2] << 24) +
						  (var[gAdj_Info[i].p_add * 2 + 1] << 16) +
						  (var[gAdj_Info[i].p_add * 2 + 2] << 8) +
						  var[gAdj_Info[i].p_add * 2 + 3];
				}
				else if (gAdj_Info[i].p_tpye == 0x82)
					sum = (var[gAdj_Info[i].p_add * 2] << 24) +
						  (var[gAdj_Info[i].p_add * 2 + 1] << 16) +
						  (var[gAdj_Info[i].p_add * 2 + 2] << 8) +
						  var[gAdj_Info[i].p_add * 2 + 3];
				else if (gAdj_Info[i].p_tpye == 0x83)
				{
					sum = (var[gAdj_Info[i].p_add * 2 + 6] << 8) + var[gAdj_Info[i].p_add * 2 + 7];
					if (var[gAdj_Info[i].p_add * 2] >= 0x80)
						sum = sum + (0x80 << 24);
				}
				else if (gAdj_Info[i].p_tpye >= 0x40 && gAdj_Info[i].p_tpye <= 0x4F)
					sum = (var[gAdj_Info[i].p_add * 2] << 8) + var[gAdj_Info[i].p_add * 2 + 1];

				if (gAdj_Info[i].Adj_mode == 1) //+
				{
					if (gAdj_Info[i].p_tpye & 0x40)
					{
						num = gAdj_Info[i].p_tpye & 0x0F;
						if (sum & (1 << num))
							sum &= ~(1 << num);
						else
							sum |= (1 << num);
					}
					else
					{
						sum += gAdj_Info[i].Adj_step;
						if (sum > gAdj_Info[i].V_max)
						{
							if (gAdj_Info[i].Return_Mode == 0)
								sum = gAdj_Info[i].V_max;
							else
								sum = gAdj_Info[i].V_min;
						}
						else if (sum < gAdj_Info[i].V_min)
							sum = gAdj_Info[i].V_min;
					}
				}
				else // -
				{
					if (gAdj_Info[i].p_tpye & 0x40)
					{
						num = gAdj_Info[i].p_tpye & 0x0F;
						if (sum & (1 << num))
							sum &= ~(1 << num);
						else
							sum |= (1 << num);
					}
					else
					{
						sum -= gAdj_Info[i].Adj_step;

						if (sum < gAdj_Info[i].V_min)
						{
							if (gAdj_Info[i].Return_Mode == 0)
								sum = gAdj_Info[i].V_min;
							else
								sum = gAdj_Info[i].V_max;
						}
						else if (sum > gAdj_Info[i].V_max)
							sum = gAdj_Info[i].V_max;
					}
				}

				if (gAdj_Info[i].p_tpye == 0x00 || gAdj_Info[i].p_tpye == 0x80)
				{

					var[gAdj_Info[i].p_add * 2 + 1] = sum & 0xFF;
				}
				else if (gAdj_Info[i].p_tpye == 0x01 || gAdj_Info[i].p_tpye == 0x81)
				{
					var[gAdj_Info[i].p_add * 2] = sum >> 8 & 0xFF;
					var[gAdj_Info[i].p_add * 2 + 1] = sum & 0xFF;
				}
				else if (gAdj_Info[i].p_tpye == 0x02 || gAdj_Info[i].p_tpye == 0x82)
				{

					if (sum < 0)
					{
						var[gAdj_Info[i].p_add * 2] = 0xFF;
						var[gAdj_Info[i].p_add * 2 + 1] = 0xFF;
						var[gAdj_Info[i].p_add * 2 + 2] = sum >> 8 & 0xFF;
						var[gAdj_Info[i].p_add * 2 + 3] = sum & 0xFF;
					}
					else
					{
						var[gAdj_Info[i].p_add * 2] = 0;
						var[gAdj_Info[i].p_add * 2 + 1] = 0;
						var[gAdj_Info[i].p_add * 2 + 2] = sum >> 8 & 0xFF;
						var[gAdj_Info[i].p_add * 2 + 3] = sum & 0xFF;
					}
				}
				else if (gAdj_Info[i].p_tpye == 0x83)
				{
					if (sum < 0)
					{
						var[gAdj_Info[i].p_add * 2] = 0xFF;
						var[gAdj_Info[i].p_add * 2 + 1] = 0xFF;
						var[gAdj_Info[i].p_add * 2 + 2] = 0xFF;
						var[gAdj_Info[i].p_add * 2 + 3] = 0xFF;
						var[gAdj_Info[i].p_add * 2 + 4] = 0xFF;
						var[gAdj_Info[i].p_add * 2 + 5] = 0xFF;
						var[gAdj_Info[i].p_add * 2 + 6] = sum >> 8 & 0xFF;
						var[gAdj_Info[i].p_add * 2 + 7] = sum & 0xFF;
					}
					else
					{
						var[gAdj_Info[i].p_add * 2] = 0;
						var[gAdj_Info[i].p_add * 2 + 1] = 0;
						var[gAdj_Info[i].p_add * 2 + 2] = 0;
						var[gAdj_Info[i].p_add * 2 + 3] = 0;
						var[gAdj_Info[i].p_add * 2 + 4] = 0;
						var[gAdj_Info[i].p_add * 2 + 5] = 0;
						var[gAdj_Info[i].p_add * 2 + 6] = sum >> 8 & 0xFF;
						var[gAdj_Info[i].p_add * 2 + 7] = sum & 0xFF;
					}
				}
				else if (gAdj_Info[i].p_tpye >= 0x40 && gAdj_Info[i].p_tpye <= 0x4F)
				{
					var[gAdj_Info[i].p_add * 2] = sum >> 8 & 0xFF;
					var[gAdj_Info[i].p_add * 2 + 1] = sum & 0xFF;
				}

				if (gAdj_Info[i].Code == 0xC003) // Enable serial port return
				{
					rebuf[3] = 0x41;
					if (gAdj_Info[i].p_tpye == 0x00 ||
						gAdj_Info[i].p_tpye == 0x80 ||
						gAdj_Info[i].p_tpye == 0x01 ||
						gAdj_Info[i].p_tpye == 0x81 ||
						(gAdj_Info[i].p_tpye >= 0x40 && gAdj_Info[i].p_tpye <= 0x4F))
					{
						rebuf[4] = gAdj_Info[i].p_add >> 8;
						rebuf[5] = gAdj_Info[i].p_add & 0xFF;
						rebuf[6] = var[gAdj_Info[i].p_add * 2];
						rebuf[7] = var[gAdj_Info[i].p_add * 2 + 1];
						LT_SendData_CRC_Frame(rebuf, 5);
					}
					else if (gAdj_Info[i].p_tpye == 0x02 || gAdj_Info[i].p_tpye == 0x82)
					{
						rebuf[4] = gAdj_Info[i].p_add >> 8;
						rebuf[5] = gAdj_Info[i].p_add & 0xFF;
						rebuf[6] = var[gAdj_Info[i].p_add * 2];
						rebuf[7] = var[gAdj_Info[i].p_add * 2 + 1];
						rebuf[8] = var[gAdj_Info[i].p_add * 2 + 2];
						rebuf[9] = var[gAdj_Info[i].p_add * 2 + 3];
						LT_SendData_CRC_Frame(rebuf, 7);
					}
					else if (gAdj_Info[i].p_tpye == 0x03 || gAdj_Info[i].p_tpye == 0x83)
					{
						rebuf[4] = gAdj_Info[i].p_add >> 8;
						rebuf[5] = gAdj_Info[i].p_add & 0xFF;
						rebuf[6] = var[gAdj_Info[i].p_add * 2];
						rebuf[7] = var[gAdj_Info[i].p_add * 2 + 1];
						rebuf[8] = var[gAdj_Info[i].p_add * 2 + 2];
						rebuf[9] = var[gAdj_Info[i].p_add * 2 + 3];
						rebuf[10] = var[gAdj_Info[i].p_add * 2 + 4];
						rebuf[11] = var[gAdj_Info[i].p_add * 2 + 5];
						rebuf[12] = var[gAdj_Info[i].p_add * 2 + 6];
						rebuf[13] = var[gAdj_Info[i].p_add * 2 + 7];
						LT_SendData_CRC_Frame(rebuf, 11);
					}
				}

				if (gAdj_Info[i].id != 0xFFFF)
				{
					Display_Icon(gAdj_Info[i].id, gAdj_Info[i].Xs, gAdj_Info[i].Ys, 0);
				}

				if (gAdj_Info[i].p_add >= 0x7000)
					reg_operation(gAdj_Info[i].p_add);

				page_date_update(pic_id);
			}
		}
	}
	if (gTpInfo.sta == 0 && Adj_flag == 1)
	{
		if (gAdj_Info[Adj_num].p_tpye == 0x00)
			sum = var[gAdj_Info[Adj_num].p_add * 2 + 1];
		else if (gAdj_Info[Adj_num].p_tpye == 0x80)
		{
			sum0 = var[gAdj_Info[Adj_num].p_add * 2 + 1];
			sum = sum0;
		}
		else if (gAdj_Info[Adj_num].p_tpye == 0x01)
			sum = (var[gAdj_Info[Adj_num].p_add * 2] << 8) + var[gAdj_Info[Adj_num].p_add * 2 + 1];
		else if (gAdj_Info[Adj_num].p_tpye == 0x81)
		{
			sum1 = (var[gAdj_Info[Adj_num].p_add * 2] << 8) + var[gAdj_Info[Adj_num].p_add * 2 + 1];
			sum = sum1;
		}
		else if (gAdj_Info[Adj_num].p_tpye == 0x02)
		{
			if (var[gAdj_Info[Adj_num].p_add * 2] >= 0x80)
				var[gAdj_Info[Adj_num].p_add * 2] = 0x40;
			sum = (var[gAdj_Info[Adj_num].p_add * 2] << 24) +
				  (var[gAdj_Info[Adj_num].p_add * 2 + 1] << 16) +
				  (var[gAdj_Info[Adj_num].p_add * 2 + 2] << 8) +
				  var[gAdj_Info[Adj_num].p_add * 2 + 3];
		}
		else if (gAdj_Info[Adj_num].p_tpye == 0x82)
			sum = (var[gAdj_Info[Adj_num].p_add * 2] << 24) +
				  (var[gAdj_Info[Adj_num].p_add * 2 + 1] << 16) +
				  (var[gAdj_Info[Adj_num].p_add * 2 + 2] << 8) +
				  var[gAdj_Info[Adj_num].p_add * 2 + 3];
		else if (gAdj_Info[Adj_num].p_tpye == 0x83)
		{
			sum = (var[gAdj_Info[Adj_num].p_add * 2 + 6] << 8) + var[gAdj_Info[Adj_num].p_add * 2 + 7];
			if (var[gAdj_Info[Adj_num].p_add * 2] >= 0x80)
				sum = sum + (0x80 << 24);
		}
		else if (gAdj_Info[Adj_num].p_tpye >= 0x40 && gAdj_Info[Adj_num].p_tpye <= 0x4F)
			sum = (var[gAdj_Info[Adj_num].p_add * 2] << 8) + var[gAdj_Info[Adj_num].p_add * 2 + 1];

		if (gAdj_Info[Adj_num].Adj_mode == 1) //+
		{
			if (gAdj_Info[Adj_num].p_tpye & 0x40)
			{
				num = gAdj_Info[Adj_num].p_tpye & 0x0F;
				if (sum & (1 << num))
					sum &= ~(1 << num);
				else
					sum |= (1 << num);
			}
			else
			{
				sum += gAdj_Info[Adj_num].Adj_step;
				if (sum > gAdj_Info[Adj_num].V_max)
				{
					if (gAdj_Info[Adj_num].Return_Mode == 0)
						sum = gAdj_Info[Adj_num].V_max;
					else
						sum = gAdj_Info[Adj_num].V_min;
				}
				else if (sum < gAdj_Info[Adj_num].V_min)
					sum = gAdj_Info[Adj_num].V_min;
			}
		}
		else // -
		{
			if (gAdj_Info[Adj_num].p_tpye & 0x40)
			{
				num = gAdj_Info[Adj_num].p_tpye & 0x0F;
				if (sum & (1 << num))
					sum &= ~(1 << num);
				else
					sum |= (1 << num);
			}
			else
			{
				sum -= gAdj_Info[Adj_num].Adj_step;

				if (sum < gAdj_Info[Adj_num].V_min)
				{
					if (gAdj_Info[Adj_num].Return_Mode == 0)
						sum = gAdj_Info[Adj_num].V_min;
					else
						sum = gAdj_Info[Adj_num].V_max;
				}
				else if (sum > gAdj_Info[Adj_num].V_max)
					sum = gAdj_Info[Adj_num].V_max;
			}
		}

		if (gAdj_Info[Adj_num].p_tpye == 0x00 || gAdj_Info[Adj_num].p_tpye == 0x80)
		{

			var[gAdj_Info[Adj_num].p_add * 2 + 1] = sum & 0xFF;
		}
		else if (gAdj_Info[Adj_num].p_tpye == 0x01 || gAdj_Info[Adj_num].p_tpye == 0x81)
		{
			var[gAdj_Info[Adj_num].p_add * 2] = sum >> 8 & 0xFF;
			var[gAdj_Info[Adj_num].p_add * 2 + 1] = sum & 0xFF;
		}
		else if (gAdj_Info[Adj_num].p_tpye == 0x02 || gAdj_Info[Adj_num].p_tpye == 0x82)
		{

			if (sum < 0)
			{
				var[gAdj_Info[Adj_num].p_add * 2] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 1] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 2] = sum >> 8 & 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 3] = sum & 0xFF;
			}
			else
			{
				var[gAdj_Info[Adj_num].p_add * 2] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 1] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 2] = sum >> 8 & 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 3] = sum & 0xFF;
			}
		}
		else if (gAdj_Info[Adj_num].p_tpye == 0x83)
		{
			if (sum < 0)
			{
				var[gAdj_Info[Adj_num].p_add * 2] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 1] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 2] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 3] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 4] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 5] = 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 6] = sum >> 8 & 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 7] = sum & 0xFF;
			}
			else
			{
				var[gAdj_Info[Adj_num].p_add * 2] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 1] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 2] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 3] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 4] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 5] = 0;
				var[gAdj_Info[Adj_num].p_add * 2 + 6] = sum >> 8 & 0xFF;
				var[gAdj_Info[Adj_num].p_add * 2 + 7] = sum & 0xFF;
			}
		}
		else if (gAdj_Info[Adj_num].p_tpye >= 0x40 && gAdj_Info[Adj_num].p_tpye <= 0x4F)
		{
			var[gAdj_Info[Adj_num].p_add * 2] = sum >> 8 & 0xFF;
			var[gAdj_Info[Adj_num].p_add * 2 + 1] = sum & 0xFF;
		}

		if (gAdj_Info[Adj_num].Code == 0xC003) // Enable serial port return
		{
			rebuf[3] = 0x41;
			if (gAdj_Info[Adj_num].p_tpye == 0x00 ||
				gAdj_Info[Adj_num].p_tpye == 0x80 ||
				gAdj_Info[Adj_num].p_tpye == 0x01 ||
				gAdj_Info[Adj_num].p_tpye == 0x81 ||
				(gAdj_Info[Adj_num].p_tpye >= 0x40 && gAdj_Info[Adj_num].p_tpye <= 0x4F))
			{
				rebuf[4] = gAdj_Info[Adj_num].p_add >> 8;
				rebuf[5] = gAdj_Info[Adj_num].p_add & 0xFF;
				rebuf[6] = var[gAdj_Info[Adj_num].p_add * 2];
				rebuf[7] = var[gAdj_Info[Adj_num].p_add * 2 + 1];
				LT_SendData_CRC_Frame(rebuf, 5);
			}
			else if (gAdj_Info[Adj_num].p_tpye == 0x02 || gAdj_Info[Adj_num].p_tpye == 0x82)
			{
				rebuf[4] = gAdj_Info[Adj_num].p_add >> 8;
				rebuf[5] = gAdj_Info[Adj_num].p_add & 0xFF;
				rebuf[6] = var[gAdj_Info[Adj_num].p_add * 2];
				rebuf[7] = var[gAdj_Info[Adj_num].p_add * 2 + 1];
				rebuf[8] = var[gAdj_Info[Adj_num].p_add * 2 + 2];
				rebuf[9] = var[gAdj_Info[Adj_num].p_add * 2 + 3];
				LT_SendData_CRC_Frame(rebuf, 7);
			}
			else if (gAdj_Info[Adj_num].p_tpye == 0x03 || gAdj_Info[Adj_num].p_tpye == 0x83)
			{
				rebuf[4] = gAdj_Info[Adj_num].p_add >> 8;
				rebuf[5] = gAdj_Info[Adj_num].p_add & 0xFF;
				rebuf[6] = var[gAdj_Info[Adj_num].p_add * 2];
				rebuf[7] = var[gAdj_Info[Adj_num].p_add * 2 + 1];
				rebuf[8] = var[gAdj_Info[Adj_num].p_add * 2 + 2];
				rebuf[9] = var[gAdj_Info[Adj_num].p_add * 2 + 3];
				rebuf[10] = var[gAdj_Info[Adj_num].p_add * 2 + 4];
				rebuf[11] = var[gAdj_Info[Adj_num].p_add * 2 + 5];
				rebuf[12] = var[gAdj_Info[Adj_num].p_add * 2 + 6];
				rebuf[13] = var[gAdj_Info[Adj_num].p_add * 2 + 7];
				LT_SendData_CRC_Frame(rebuf, 11);
			}
		}

		if (gAdj_Info[Adj_num].id != 0xFFFF)
		{
			Display_Icon(gAdj_Info[Adj_num].id, gAdj_Info[Adj_num].Xs, gAdj_Info[Adj_num].Ys, 0);
		}
		button_Press_flag = 0;
		Adj_flag = 0;
		Adj_loop = 0;
		Adj_time = 0;
		if (gAdj_Info[Adj_num].p_add >= 0x7000)
			reg_operation(gAdj_Info[Adj_num].p_add);

		page_date_update(pic_id);
	}
	
}
/********************/

/***********************************************************************************
 * Function Name	: Disp_slide
 * Description		: Display sliding progress bar.
 * Input			: - i: which number of sliding progress bar
 * Output       	: None
 * Return       	: None
 ************************************************************************************/
void Disp_slide(uint8_t i)
{
	uint8_t buff[12] = {0};
	int16_t sum = 0;
	uint16_t w, h, canvas_w = 0;
	uint16_t dw = 0, dh = 0, dw1 = 0, dh1 = 0, bgw, bgh, Dsldw, Dsldh;
	uint16_t Offset;

	if (gSlide_Info[i].bgid != 0xFFFF)
	{
		Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gSlide_Info[i].bgid, 12);
		pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
		bgw = buff[4] + (buff[5] << 8);
		bgh = buff[6] + (buff[7] << 8);
		pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
		pic_flag = buff[11];
		canvas_w = Get_Canvas_Width(bgw);
		Canvas_Image_Start_address(LAY_BUFF1);
		Canvas_image_width(canvas_w);

		LT768_BTE_Memory_Copy(Display_Layer, LCD_XSIZE_TFT, gSlide_Info[i].x1, gSlide_Info[i].y1,
							  Display_Layer, LCD_XSIZE_TFT, gSlide_Info[i].x1, gSlide_Info[i].y1,
							  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x1, gSlide_Info[i].y1,
							  0x0c, bgw, bgh);

		LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
											bgw, bgh,
											bgw, pic_add,
											LAY_BUFF1, canvas_w);

		if (pic_flag == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
								  LAY_BUFF1, canvas_w, 0, 0,
								  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x1, gSlide_Info[i].y1,
								  0x0c, bgw, bgh);
		}
		else if (pic_flag == 1)
		{
			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
											 LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x1, gSlide_Info[i].y1,
											 Black, bgw, bgh);
		}
		else if (pic_flag == 2)
		{
			BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, gSlide_Info[i].x1, gSlide_Info[i].y1,
										   LAY_BUFF1, canvas_w, 0, 0,
										   LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x1, gSlide_Info[i].y1,
										   bgw, bgh);
		}
	}

	if (gSlide_Info[i].arrowid != 0xFFFF)
	{
		Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gSlide_Info[i].arrowid, 8);
		pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
		dw = buff[4] + (buff[5] << 8);
		dh = buff[6] + (buff[7] << 8);
	}

	if (gSlide_Info[i].slideid != 0xFFFF)
	{
		Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gSlide_Info[i].slideid, 12);
		pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
		pic_w = buff[4] + (buff[5] << 8);
		pic_h = buff[6] + (buff[7] << 8);
		pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
		pic_flag = buff[11];
		canvas_w = Get_Canvas_Width(pic_w);
		Canvas_Image_Start_address(LAY_BUFF1);
		Canvas_image_width(canvas_w);
		Dsldw = pic_w;
		Dsldh = pic_h;
		dw1 = pic_w;
		dh1 = pic_h;

		if (gSlide_Info[i].bgid == 0xFFFF) // Touch background image
		{
			if (gSlide_Info[i].direction == 0 || gSlide_Info[i].direction == 1)
			{
				if (dh > dh1)
				{
					Offset = (dh - dh1) / 2;
					dh1 = dh;
				}
				else
					Offset = 0;
				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gSlide_Info[i].x2 - dw / 2, gSlide_Info[i].y2 - Offset,
									  MAIN_BUFF, LCD_XSIZE_TFT, gSlide_Info[i].x2 - dw / 2, gSlide_Info[i].y2 - Offset,
									  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2 - dw / 2, gSlide_Info[i].y2 - Offset,
									  0x0c, pic_w + dw, dh1);
			}
			else if (gSlide_Info[i].direction == 2 || gSlide_Info[i].direction == 3)
			{
				if (dw > dw1)
				{
					Offset = (dw - dw1) / 2;
					dw1 = dw;
				}
				else
					Offset = 0;
				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gSlide_Info[i].x2 - Offset, gSlide_Info[i].y2 - dh / 2,
									  MAIN_BUFF, LCD_XSIZE_TFT, gSlide_Info[i].x2 - Offset, gSlide_Info[i].y2 - dh / 2,
									  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2 - Offset, gSlide_Info[i].y2 - dh / 2,
									  0x0c, dw1, pic_h + dh);
			}
		}

		LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
											pic_w, pic_h,
											pic_w, pic_add,
											LAY_BUFF1, canvas_w);

		switch (gSlide_Info[i].direction)
		{
		case 0: // From left to right
			if (Slide_flag == 0)
			{
				sum = (var[gSlide_Info[i].p_add * 2] << 8) + var[gSlide_Info[i].p_add * 2 + 1];
				if (sum > gSlide_Info[i].V_max)
					sum = gSlide_Info[i].V_max;
				else if (sum < gSlide_Info[i].V_min)
					sum = gSlide_Info[i].V_min;
				w = ((gSlide_Info[i].Xe - gSlide_Info[i].Xs + 1) * (sum - gSlide_Info[i].V_min)) / (gSlide_Info[i].V_max - gSlide_Info[i].V_min);
			}
			else
				w = gSlide_Info[i].dx - gSlide_Info[i].Xs;

			if (w > (dw1 + gSlide_Info[i].Xs - gSlide_Info[i].x2))
				w = dw1 + gSlide_Info[i].Xs - gSlide_Info[i].x2;
			if (w < (gSlide_Info[i].x2 - gSlide_Info[i].Xs))
				w = gSlide_Info[i].x2 - gSlide_Info[i].Xs;
			if (pic_flag == 0)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2, gSlide_Info[i].y2,
									  0x0c, w + gSlide_Info[i].Xs - gSlide_Info[i].x2, pic_h);
			}
			else if (pic_flag == 1)
			{
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
												 LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2, gSlide_Info[i].y2,
												 Black, w + gSlide_Info[i].Xs - gSlide_Info[i].x2, pic_h);
			}
			else if (pic_flag == 2)
			{
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2, gSlide_Info[i].y2,
											   LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2, gSlide_Info[i].y2,
											   w + gSlide_Info[i].Xs - gSlide_Info[i].x2, pic_h);
			}

			if (gSlide_Info[i].arrowid != 0xFFFF)
			{

				Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gSlide_Info[i].arrowid, 12);
				pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
				pic_w = buff[4] + (buff[5] << 8);
				pic_h = buff[6] + (buff[7] << 8);
				pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
				pic_flag = buff[11];
				canvas_w = Get_Canvas_Width(pic_w);
				Canvas_Image_Start_address(LAY_BUFF1);
				Canvas_image_width(canvas_w);

				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													pic_w, pic_h,
													pic_w, pic_add, LAY_BUFF1, canvas_w);

				if (pic_flag == 0)
				{
					LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
										  LAY_BUFF1, canvas_w, 0, 0,
										  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].Xs + w - pic_w / 2 + 1, gSlide_Info[i].y2 + Dsldh / 2 - pic_h / 2 + 1,
										  0x0c, pic_w, pic_h);
				}
				else if (pic_flag == 1)
				{
					LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
													 LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].Xs + w - pic_w / 2 + 1, gSlide_Info[i].y2 + Dsldh / 2 - pic_h / 2 + 1,
													 Black, pic_w, pic_h);
				}
				else if (pic_flag == 2)
				{

					BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].Xs + w - pic_w / 2 + 1, gSlide_Info[i].y2 + Dsldh / 2 - pic_h / 2 + 1,
												   LAY_BUFF1, canvas_w, 0, 0,
												   LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].Xs + w - pic_w / 2 + 1, gSlide_Info[i].y2 + Dsldh / 2 - pic_h / 2 + 1,
												   pic_w, pic_h);
				}
			}
			break;
		case 1: // From right to left
			if (Slide_flag == 0)
			{
				sum = (var[gSlide_Info[i].p_add * 2] << 8) + var[gSlide_Info[i].p_add * 2 + 1];
				if (sum > gSlide_Info[i].V_max)
					sum = gSlide_Info[i].V_max;
				else if (sum < gSlide_Info[i].V_min)
					sum = gSlide_Info[i].V_min;

				w = ((gSlide_Info[i].Xe - gSlide_Info[i].Xs + 1) * (sum - gSlide_Info[i].V_min)) / (gSlide_Info[i].V_max - gSlide_Info[i].V_min);
			}
			else
			{
				w = gSlide_Info[i].Xe - gSlide_Info[i].dx;
			}
			if (w > (dw1 + gSlide_Info[i].Xs - gSlide_Info[i].x2))
				w = dw1 + gSlide_Info[i].Xs - gSlide_Info[i].x2;
			if (w < (gSlide_Info[i].Xe - pic_w - gSlide_Info[i].x2))
				w = gSlide_Info[i].Xe - pic_w - gSlide_Info[i].x2;

			if (pic_flag == 0)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, gSlide_Info[i].Xe - w - gSlide_Info[i].x2, 0,
									  LAY_BUFF1, canvas_w, gSlide_Info[i].Xe - w - gSlide_Info[i].x2, 0,
									  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].Xe - w, gSlide_Info[i].y2,
									  0x0c, w + gSlide_Info[i].x2 + pic_w - gSlide_Info[i].Xe, pic_h);
			}
			else if (pic_flag == 1)
			{

				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, gSlide_Info[i].Xe - w - gSlide_Info[i].x2, 0,
												 LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].Xe - w, gSlide_Info[i].y2,
												 Black, w + gSlide_Info[i].x2 + pic_w - gSlide_Info[i].Xe, pic_h);
			}
			else if (pic_flag == 2)
			{
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].Xe - w, gSlide_Info[i].y2,
											   LAY_BUFF1, canvas_w, gSlide_Info[i].Xe - w - gSlide_Info[i].x2, 0,
											   LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].Xe - w, gSlide_Info[i].y2,
											   w + gSlide_Info[i].x2 + pic_w - gSlide_Info[i].Xe, pic_h);
			}

			if (gSlide_Info[i].arrowid != 0xFFFF)
			{
				Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gSlide_Info[i].arrowid, 12);
				pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
				pic_w = buff[4] + (buff[5] << 8);
				pic_h = buff[6] + (buff[7] << 8);
				pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
				pic_flag = buff[11];
				canvas_w = Get_Canvas_Width(pic_w);
				Canvas_Image_Start_address(LAY_BUFF1);
				Canvas_image_width(canvas_w);

				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													pic_w, pic_h,
													pic_w, pic_add,
													LAY_BUFF1, canvas_w);

				if (pic_flag == 0)
				{
					LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
										  LAY_BUFF1, canvas_w, 0, 0,
										  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].Xe - w + pic_w / 2, gSlide_Info[i].y2 + Dsldh / 2 - pic_h / 2 + 1,
										  0x0c, pic_w, pic_h);
				}
				else if (pic_flag == 1)
				{
					LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
													 LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].Xe - w + pic_w / 2, gSlide_Info[i].y2 + Dsldh / 2 - pic_h / 2 + 1,
													 Black, pic_w, pic_h);
				}
				else if (pic_flag == 2)
				{

					BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].Xe - w - pic_w / 2, gSlide_Info[i].y2 + Dsldh / 2 - pic_h / 2 + 1,
												   LAY_BUFF1, canvas_w, 0, 0,
												   LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].Xe - w - pic_w / 2, gSlide_Info[i].y2 + Dsldh / 2 - pic_h / 2 + 1,
												   pic_w, pic_h);
				}
			}
			break;
		case 2: // From top to bottom
			if (Slide_flag == 0)
			{
				sum = (var[gSlide_Info[i].p_add * 2] << 8) + var[gSlide_Info[i].p_add * 2 + 1];
				if (sum > gSlide_Info[i].V_max)
					sum = gSlide_Info[i].V_max;
				else if (sum < gSlide_Info[i].V_min)
					sum = gSlide_Info[i].V_min;

				h = ((gSlide_Info[i].Ye - gSlide_Info[i].Ys + 1) * (sum - gSlide_Info[i].V_min)) / (gSlide_Info[i].V_max - gSlide_Info[i].V_min);
			}
			else
				h = gSlide_Info[i].dy - gSlide_Info[i].Ys;
			if (h > (dh1 + gSlide_Info[i].Ys - gSlide_Info[i].y2))
				h = dh1 + gSlide_Info[i].Ys - gSlide_Info[i].y2;
			if (h < (gSlide_Info[i].y2 - gSlide_Info[i].Ys))
				h = gSlide_Info[i].y2 - gSlide_Info[i].Ys;
			if (pic_flag == 0)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2, gSlide_Info[i].y2,
									  0x0c, pic_w, h + gSlide_Info[i].Ys - gSlide_Info[i].y2);
			}
			else if (pic_flag == 1)
			{
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
												 LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2, gSlide_Info[i].y2,
												 Black, pic_w, h + gSlide_Info[i].Ys - gSlide_Info[i].y2);
			}
			else if (pic_flag == 2)
			{
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2, gSlide_Info[i].y2,
											   LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2, gSlide_Info[i].y2,
											   pic_w, h + gSlide_Info[i].Ys - gSlide_Info[i].y2);
			}

			if (gSlide_Info[i].arrowid != 0xFFFF)
			{
				Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gSlide_Info[i].arrowid, 12);
				pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
				pic_w = buff[4] + (buff[5] << 8);
				pic_h = buff[6] + (buff[7] << 8);
				pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
				pic_flag = buff[11];
				canvas_w = Get_Canvas_Width(pic_w);
				Canvas_Image_Start_address(LAY_BUFF1);
				Canvas_image_width(canvas_w);

				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													pic_w, pic_h,
													pic_w, pic_add,
													LAY_BUFF1, canvas_w);

				if (pic_flag == 0)
				{
					LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
										  LAY_BUFF1, canvas_w, 0, 0,
										  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2 + Dsldw / 2 - pic_w / 2 + 1, gSlide_Info[i].Ys + h - pic_h / 2 + 1,
										  0x0c, pic_w, pic_h);
				}
				else if (pic_flag == 1)
				{
					LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
													 LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2 + Dsldw / 2 - pic_w / 2 + 1, gSlide_Info[i].Ys + h - pic_h / 2 + 1,
													 Black, pic_w, pic_h);
				}
				else if (pic_flag == 2)
				{

					BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2 + Dsldw / 2 - pic_w / 2 + 1, gSlide_Info[i].Ys + h - pic_h / 2 + 1,
												   LAY_BUFF1, canvas_w, 0, 0,
												   LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2 + Dsldw / 2 - pic_w / 2 + 1, gSlide_Info[i].Ys + h - pic_h / 2 + 1,
												   pic_w, pic_h);
				}
			}
			break;
		case 3: // From bottom to top
			if (Slide_flag == 0)
			{
				sum = (var[gSlide_Info[i].p_add * 2] << 8) + var[gSlide_Info[i].p_add * 2 + 1];
				if (sum > gSlide_Info[i].V_max)
					sum = gSlide_Info[i].V_max;
				else if (sum < gSlide_Info[i].V_min)
					sum = gSlide_Info[i].V_min;

				h = ((gSlide_Info[i].Ye - gSlide_Info[i].Ys + 1) * (sum - gSlide_Info[i].V_min)) / (gSlide_Info[i].V_max - gSlide_Info[i].V_min);
			}
			else
				h = gSlide_Info[i].Ye - gSlide_Info[i].dy;
			if (h > (dh1 + gSlide_Info[i].Ys - gSlide_Info[i].y2))
				h = dh1 + gSlide_Info[i].Ys - gSlide_Info[i].y2;
			if (h < (gSlide_Info[i].Ye - pic_h - gSlide_Info[i].y2))
				h = gSlide_Info[i].Ye - pic_h - gSlide_Info[i].y2;
			if (pic_flag == 0)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, gSlide_Info[i].Ye - h - gSlide_Info[i].y2,
									  LAY_BUFF1, canvas_w, 0, gSlide_Info[i].Ye - h - gSlide_Info[i].y2,
									  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2, gSlide_Info[i].Ye - h,
									  0x0c, pic_w, gSlide_Info[i].y2 + pic_h - gSlide_Info[i].Ye + h);
			}
			else if (pic_flag == 1)
			{
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, gSlide_Info[i].Ye - h - gSlide_Info[i].y2,
												 LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2, gSlide_Info[i].Ye - h,
												 Black, pic_w, gSlide_Info[i].y2 + pic_h - gSlide_Info[i].Ye + h);
			}
			else if (pic_flag == 2)
			{
				BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2, gSlide_Info[i].Ye - h,
											   LAY_BUFF1, canvas_w, 0, gSlide_Info[i].Ye - h - gSlide_Info[i].y2,
											   LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2, gSlide_Info[i].Ye - h,
											   pic_w, gSlide_Info[i].y2 + pic_h - gSlide_Info[i].Ye + h);
			}

			if (gSlide_Info[i].arrowid != 0xFFFF)
			{
				Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gSlide_Info[i].arrowid, 12);
				pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
				pic_w = buff[4] + (buff[5] << 8);
				pic_h = buff[6] + (buff[7] << 8);
				pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
				pic_flag = buff[11];
				canvas_w = Get_Canvas_Width(pic_w);
				Canvas_Image_Start_address(LAY_BUFF1);
				Canvas_image_width(canvas_w);

				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													pic_w, pic_h,
													pic_w, pic_add, LAY_BUFF1, canvas_w);
				if (pic_flag == 0)
				{
					LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
										  LAY_BUFF1, canvas_w, 0, 0,
										  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2 + Dsldw / 2 - pic_w / 2, gSlide_Info[i].Ye - h - pic_h / 2,
										  0x0c, pic_w, pic_h);
				}
				else if (pic_flag == 1)
				{
					LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
													 LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2 + Dsldw / 2 - pic_w / 2, gSlide_Info[i].Ye - h - pic_h / 2,
													 Black, pic_w, pic_h);
				}
				else if (pic_flag == 2)
				{

					BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2 + Dsldw / 2 - pic_w / 2, gSlide_Info[i].Ye - h - pic_h / 2,
												   LAY_BUFF1, canvas_w, 0, 0,
												   LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2 + Dsldw / 2 - pic_w / 2, gSlide_Info[i].Ye - h - pic_h / 2,
												   pic_w, pic_h);
				}
			}
			break;
		default:
			break;
		}
	}

	if (gSlide_Info[i].bgid != 0xFFFF)
	{
		LT768_BTE_Memory_Copy(LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x1, gSlide_Info[i].y1,
							  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x1, gSlide_Info[i].y1,
							  Display_Layer, LCD_XSIZE_TFT, gSlide_Info[i].x1, gSlide_Info[i].y1,
							  0x0c, bgw, bgh);
	}
	else
	{
		if (gSlide_Info[i].direction == 0 || gSlide_Info[i].direction == 1)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2 - dw / 2, gSlide_Info[i].y2 - Offset,
								  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2 - dw / 2, gSlide_Info[i].y2 - Offset,
								  Display_Layer, LCD_XSIZE_TFT, gSlide_Info[i].x2 - dw / 2, gSlide_Info[i].y2 - Offset,
								  0x0c, Dsldw + dw, dh1);
		}
		else if (gSlide_Info[i].direction == 2 || gSlide_Info[i].direction == 3)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2 - Offset, gSlide_Info[i].y2 - dh / 2,
								  LAY_BUFF3, LCD_XSIZE_TFT, gSlide_Info[i].x2 - Offset, gSlide_Info[i].y2 - dh / 2,
								  Display_Layer, LCD_XSIZE_TFT, gSlide_Info[i].x2 - Offset, gSlide_Info[i].y2 - dh / 2,
								  0x0c, dw1, Dsldh + dh);
		}
	}
}

/***********************************************************************************
 * Function Name	: Progress_bar_sliding
 * Description		: Touch detection of sliding progress bar.
 * Input			: None
 * Output       	: None
 * Return       	: None
 ************************************************************************************/

void Progress_bar_sliding(void)
{
	uint8_t i = 0, rebuf[20];
	int16_t temp;
	int32_t sum = 0;
	float step = 0;
	if (gTpInfo.sta)
	{
		for (i = 0; i < Slide_count; i++)
		{
			if (gSlide_Info[i].lastx != gTpInfo.x[0] || gSlide_Info[i].lasty != gTpInfo.y[0])
			{
				switch (gSlide_Info[i].direction)
				{
				case 0: // From left to right
					if (gTpInfo.x[0] >= gSlide_Info[i].Xs - 40 && gTpInfo.y[0] >= gSlide_Info[i].Ys &&
						gTpInfo.x[0] <= gSlide_Info[i].Xe + 40 && gTpInfo.y[0] <= gSlide_Info[i].Ye &&
						((slide_first_touch == 0 && button_Press_flag == 0) || (slide_first_touch == 1 && slide_num == i)))
					{
						if (slide_first_touch == 0)
						{
							slide_first_touch = 1;
							if (buzzer)
								touch_buzzer();
						}
						slide_num = i;
						button_Press_flag = 1;

						gSlide_Info[i].dx = gTpInfo.x[0];
						if (gTpInfo.x[0] > gSlide_Info[i].Xe)
							gSlide_Info[i].dx = gSlide_Info[i].Xe;
						else if (gTpInfo.x[0] < gSlide_Info[i].Xs)
							gSlide_Info[i].dx = gSlide_Info[i].Xs;

						step = ((float)gSlide_Info[i].V_max - (float)gSlide_Info[i].V_min) / ((float)gSlide_Info[i].Xe - (float)gSlide_Info[i].Xs);
						if (gSlide_Info[i].dx == gSlide_Info[i].Xs)
						{
							var[gSlide_Info[i].p_add * 2] = gSlide_Info[i].V_min >> 8;
							var[gSlide_Info[i].p_add * 2 + 1] = gSlide_Info[i].V_min & 0xff;
						}
						else if (gSlide_Info[i].dx == gSlide_Info[i].Xe)
						{
							var[gSlide_Info[i].p_add * 2] = gSlide_Info[i].V_max >> 8;
							var[gSlide_Info[i].p_add * 2 + 1] = gSlide_Info[i].V_max & 0xff;
						}
						else
						{
							temp = (gSlide_Info[i].dx - gSlide_Info[i].Xs) * step + gSlide_Info[i].V_min;
							var[gSlide_Info[i].p_add * 2] = temp >> 8;
							var[gSlide_Info[i].p_add * 2 + 1] = temp & 0xff;
						}
						gSlide_Info[i].lasty = gTpInfo.y[0];
						if (gSlide_Info[i].lastx != gTpInfo.x[0])
						{
							gSlide_Info[i].lastx = gTpInfo.x[0];
							Slide_flag = 1;
						}
					}
					break;
				case 1: // From right to left
					if (gTpInfo.x[0] >= gSlide_Info[i].Xs - 40 && gTpInfo.y[0] >= gSlide_Info[i].Ys &&
						gTpInfo.x[0] <= gSlide_Info[i].Xe + 40 && gTpInfo.y[0] <= gSlide_Info[i].Ye &&
						((slide_first_touch == 0 && button_Press_flag == 0) || (slide_first_touch == 1 && slide_num == i)))
					{
						if (slide_first_touch == 0)
						{
							slide_first_touch = 1;
							if (buzzer)
								touch_buzzer();
						}
						slide_num = i;
						button_Press_flag = 1;
						gSlide_Info[i].dx = gTpInfo.x[0];
						if (gTpInfo.x[0] > gSlide_Info[i].Xe)
							gSlide_Info[i].dx = gSlide_Info[i].Xe;
						else if (gTpInfo.x[0] < gSlide_Info[i].Xs)
							gSlide_Info[i].dx = gSlide_Info[i].Xs;
						step = ((float)gSlide_Info[i].V_max - (float)gSlide_Info[i].V_min) / ((float)gSlide_Info[i].Xe - (float)gSlide_Info[i].Xs);
						if (gSlide_Info[i].dx == gSlide_Info[i].Xs)
						{
							var[gSlide_Info[i].p_add * 2] = gSlide_Info[i].V_max >> 8;
							var[gSlide_Info[i].p_add * 2 + 1] = gSlide_Info[i].V_max & 0xff;
						}
						else if (gSlide_Info[i].dx == gSlide_Info[i].Xe)
						{
							var[gSlide_Info[i].p_add * 2] = gSlide_Info[i].V_min >> 8;
							var[gSlide_Info[i].p_add * 2 + 1] = gSlide_Info[i].V_min & 0xff;
						}
						else
						{
							temp = (gSlide_Info[i].Xe - gSlide_Info[i].dx) * step + gSlide_Info[i].V_min;
							var[gSlide_Info[i].p_add * 2] = temp >> 8;
							var[gSlide_Info[i].p_add * 2 + 1] = temp & 0xff;
						}
						gSlide_Info[i].lasty = gTpInfo.y[0];
						if (gSlide_Info[i].lastx != gTpInfo.x[0])
						{
							gSlide_Info[i].lastx = gTpInfo.x[0];
							Slide_flag = 1;
						}
					}
					break;
				case 2: // From top to bottom
					if (gTpInfo.x[0] >= gSlide_Info[i].Xs && gTpInfo.y[0] >= gSlide_Info[i].Ys - 40 &&
						gTpInfo.x[0] <= gSlide_Info[i].Xe && gTpInfo.y[0] <= gSlide_Info[i].Ye + 40 &&
						((slide_first_touch == 0 && button_Press_flag == 0) || (slide_first_touch == 1 && slide_num == i)))
					{
						if (slide_first_touch == 0)
						{
							slide_first_touch = 1;
							if (buzzer)
								touch_buzzer();
						}
						slide_num = i;
						button_Press_flag = 1;
						gSlide_Info[i].dy = gTpInfo.y[0];
						if (gTpInfo.y[0] > gSlide_Info[i].Ye)
							gSlide_Info[i].dy = gSlide_Info[i].Ye;
						else if (gTpInfo.y[0] < gSlide_Info[i].Ys)
							gSlide_Info[i].dy = gSlide_Info[i].Ys;

						step = ((float)gSlide_Info[i].V_max - (float)gSlide_Info[i].V_min) / ((float)gSlide_Info[i].Ye - (float)gSlide_Info[i].Ys);
						if (gSlide_Info[i].dy == gSlide_Info[i].Ys)
						{
							var[gSlide_Info[i].p_add * 2] = gSlide_Info[i].V_min >> 8;
							var[gSlide_Info[i].p_add * 2 + 1] = gSlide_Info[i].V_min & 0xff;
						}
						else if (gSlide_Info[i].dy == gSlide_Info[i].Ye)
						{
							var[gSlide_Info[i].p_add * 2] = gSlide_Info[i].V_max >> 8;
							var[gSlide_Info[i].p_add * 2 + 1] = gSlide_Info[i].V_max & 0xff;
						}
						else
						{
							temp = (gSlide_Info[i].dy - gSlide_Info[i].Ys) * step + gSlide_Info[i].V_min;
							var[gSlide_Info[i].p_add * 2] = temp >> 8;
							var[gSlide_Info[i].p_add * 2 + 1] = temp & 0xff;
						}

						gSlide_Info[i].lastx = gTpInfo.x[0];
						if (gSlide_Info[i].lasty != gTpInfo.y[0])
						{
							gSlide_Info[i].lasty = gTpInfo.y[0];
							Slide_flag = 1;
						}
					}
					break;
				case 3: // From bottom to top
					if (gTpInfo.x[0] >= gSlide_Info[i].Xs && gTpInfo.y[0] >= gSlide_Info[i].Ys - 40 &&
						gTpInfo.x[0] <= gSlide_Info[i].Xe && gTpInfo.y[0] <= gSlide_Info[i].Ye + 40 &&
						((slide_first_touch == 0 && button_Press_flag == 0) || (slide_first_touch == 1 && slide_num == i)))
					{
						if (slide_first_touch == 0)
						{
							slide_first_touch = 1;
							if (buzzer)
								touch_buzzer();
						}
						slide_num = i;
						button_Press_flag = 1;
						gSlide_Info[i].dy = gTpInfo.y[0];
						if (gTpInfo.y[0] > gSlide_Info[i].Ye)
							gSlide_Info[i].dy = gSlide_Info[i].Ye;
						else if (gTpInfo.y[0] < gSlide_Info[i].Ys)
							gSlide_Info[i].dy = gSlide_Info[i].Ys;
						step = ((float)gSlide_Info[i].V_max - (float)gSlide_Info[i].V_min) / ((float)gSlide_Info[i].Ye - (float)gSlide_Info[i].Ys);
						if (gSlide_Info[i].dy == gSlide_Info[i].Ys)
						{
							var[gSlide_Info[i].p_add * 2] = gSlide_Info[i].V_max >> 8;
							var[gSlide_Info[i].p_add * 2 + 1] = gSlide_Info[i].V_max & 0xff;
						}
						else if (gSlide_Info[i].dy == gSlide_Info[i].Ye)
						{
							var[gSlide_Info[i].p_add * 2] = gSlide_Info[i].V_min >> 8;
							var[gSlide_Info[i].p_add * 2 + 1] = gSlide_Info[i].V_min & 0xff;
						}
						else
						{
							temp = (gSlide_Info[i].Ye - gSlide_Info[i].dy) * step + gSlide_Info[i].V_min;
							var[gSlide_Info[i].p_add * 2] = temp >> 8;
							var[gSlide_Info[i].p_add * 2 + 1] = temp & 0xff;
						}
						gSlide_Info[i].lastx = gTpInfo.x[0];
						if (gSlide_Info[i].lasty != gTpInfo.y[0])
						{
							gSlide_Info[i].lasty = gTpInfo.y[0];
							Slide_flag = 1;
						}
					}
					break;
				default:
					break;
				}
				if (Slide_flag == 1)
				{
					if (gSlide_Info[i].p_add >= 0x7000)
						reg_operation(gSlide_Info[i].p_add);

					page_date_update(pic_id);
					Disp_slide(i);
					Slide_flag = 0;
				}
			}
		}
	}
	else if (slide_first_touch == 1)
	{
		slide_first_touch = 0;
		button_Press_flag = 0;
		if (gSlide_Info[slide_num].Code == 0xC004)
		{
			rebuf[3] = 0x41;
			rebuf[4] = gSlide_Info[slide_num].p_add >> 8;
			rebuf[5] = gSlide_Info[slide_num].p_add & 0xFF;
			rebuf[6] = var[gSlide_Info[slide_num].p_add * 2];
			rebuf[7] = var[gSlide_Info[slide_num].p_add * 2 + 1];
			LT_SendData_CRC_Frame(rebuf, 5);
		}
	}
}
/********************************************************************/

/***********************************************************************************
 * Function Name	: input_display
 * Description		: Display ASCII data on screen.
 * Input			: None
 * Output       	: None
 * Return       	: None
 ************************************************************************************/
void input_display(void)
{
	uint8_t font_w, Alignment, font_h, buff1[4];
	uint16_t w_temp = 0, h_temp = 0;
	uint16_t dx, dy, dw, dh, x, y, i;
	uint32_t zk_addr, fcolor;

	Flash_Read_UI(buff1, addr_index[2] + gDio_Info[Dio_num].zk_id * 8, 4);
	zk_addr = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
	font_w = gDio_Info[Dio_num].zk_w;
	font_h = gDio_Info[Dio_num].zk_h;
	fcolor = gDio_Info[Dio_num].fcolor;
	Alignment = gDio_Info[Dio_num].Alig;

	for (i = 0; i < 400; i++)
		if (Display_buff[i] == 0x00)
			break;

	x = gDio_Info[Dio_num].dp_x;
	y = gDio_Info[Dio_num].dp_y;

	if (i == 0)
	{
		if (Alignment == 0)
		{
			if (Screen_DIR == 0)
			{
				dx = x;
				dy = y;
				w_temp = input_w + cur_w;
				h_temp = gDio_Info[Dio_num].zk_h;
			}
			else if (Screen_DIR == 1)
			{
				dx = x - gDio_Info[Dio_num].zk_h + 1;
				dy = y - cur_w;
				w_temp = gDio_Info[Dio_num].zk_h;
				h_temp = input_w + 2 * cur_w;
			}
			else if (Screen_DIR == 2)
			{
				dx = x - input_w + 1 - cur_w;
				dy = y - gDio_Info[Dio_num].zk_h + 1;
				w_temp = input_w + cur_w;
				h_temp = gDio_Info[Dio_num].zk_h;
			}
			else if (Screen_DIR == 3)
			{
				dx = x;
				dy = y - input_w + 1 - cur_w;
				w_temp = gDio_Info[Dio_num].zk_h;
				h_temp = input_w + cur_w;
			}
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
								  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
								  Display_Layer, LCD_XSIZE_TFT, dx, dy,
								  0x0c, w_temp, h_temp);
		}
		else if (Alignment == 2)
		{
			if (Screen_DIR == 0)
			{
				dx = x - input_w - cur_w + 1;
				dy = y;
				w_temp = input_w + cur_w;
				h_temp = gDio_Info[Dio_num].zk_h;
			}
			else if (Screen_DIR == 1)
			{
				dx = x - gDio_Info[Dio_num].zk_h + 1;
				dy = y - input_w - cur_w;
				w_temp = gDio_Info[Dio_num].zk_h;
				h_temp = input_w + cur_w;
			}
			else if (Screen_DIR == 2)
			{
				dx = x + cur_w;
				dy = y - gDio_Info[Dio_num].zk_h + 1;
				w_temp = input_w;
				h_temp = gDio_Info[Dio_num].zk_h;
			}
			else if (Screen_DIR == 3)
			{
				dx = x;
				dy = y + cur_w;
				w_temp = gDio_Info[Dio_num].zk_h;
				h_temp = input_w + cur_w;
			}
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
								  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
								  Display_Layer, LCD_XSIZE_TFT, dx, dy,
								  0x0c, w_temp, h_temp);
		}
		input_w = 0;
	}
	else
	{
		if (Alignment == 0)
		{
			if (Screen_DIR == 0)
			{
				dx = x;
				dy = y;
				w_temp = (i + 1) * gDio_Info[Dio_num].zk_w + cur_w;
				h_temp = gDio_Info[Dio_num].zk_h;
			}
			else if (Screen_DIR == 1)
			{
				dx = x - gDio_Info[Dio_num].zk_h + 1;
				dy = y;
				w_temp = gDio_Info[Dio_num].zk_h;
				h_temp = (i + 1) * gDio_Info[Dio_num].zk_w + 2 * cur_w;
			}
			if (Screen_DIR == 2)
			{
				if(x < (i + 1) * gDio_Info[Dio_num].zk_w) dx = 0;
				else dx = x - (i + 1) * gDio_Info[Dio_num].zk_w + 1;
				dy = y - gDio_Info[Dio_num].zk_h + 1;
				if(dx + (i + 1) * gDio_Info[Dio_num].zk_w > LCD_XSIZE_TFT) 
					w_temp = LCD_XSIZE_TFT - dx;
				else w_temp = (i + 1) * gDio_Info[Dio_num].zk_w;
				h_temp = gDio_Info[Dio_num].zk_h;
			}
			else if (Screen_DIR == 3)
			{
				dx = x;
				if(y < (i + 1) * gDio_Info[Dio_num].zk_w - cur_w) dy = 0;
				else dy = y - (i + 1) * gDio_Info[Dio_num].zk_w + cur_w;
				w_temp = gDio_Info[Dio_num].zk_h;
				h_temp = (i + 1) * gDio_Info[Dio_num].zk_w + cur_w;
			}
		}
		else if (Alignment == 2)
		{
			if (Screen_DIR == 0)
			{
				dx = x - (i + 1) * gDio_Info[Dio_num].zk_w - cur_w + 1;
				dy = y;
				w_temp = (i + 1) * gDio_Info[Dio_num].zk_w + cur_w;
				h_temp = gDio_Info[Dio_num].zk_h;
			}
			else if (Screen_DIR == 1)
			{
				dx = x - gDio_Info[Dio_num].zk_h + 1;
				dy = y - (i + 1) * gDio_Info[Dio_num].zk_w - cur_w;
				w_temp = gDio_Info[Dio_num].zk_h;
				h_temp = (i + 1) * gDio_Info[Dio_num].zk_w + cur_w;
			}
			if (Screen_DIR == 2)
			{
				dx = x + cur_w;
				dy = y - gDio_Info[Dio_num].zk_h + 1;
				w_temp = (i + 1) * gDio_Info[Dio_num].zk_w + cur_w;
				h_temp = gDio_Info[Dio_num].zk_h;
			}
			else if (Screen_DIR == 3)
			{
				dx = x;
				dy = y + cur_w;
				w_temp = gDio_Info[Dio_num].zk_h;
				h_temp = (i + 1) * gDio_Info[Dio_num].zk_w;
			}
		}
		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
							  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
							  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
							  0x0c, w_temp, h_temp);

		LT_Print_zk_ASCII(zk_addr, LAY_BUFF1, LCD_XSIZE_TFT,
						  font_w, font_h, Alignment, fcolor, x, y, Display_buff);

		if (data_w > input_w)
			input_w = data_w;

		if (Alignment == 0)
		{
			if (Screen_DIR == 0)
			{
				w_temp = input_w + cur_w;
			}
			else if (Screen_DIR == 1)
			{
				h_temp = input_w + cur_w;
			}
			else if (Screen_DIR == 2)
			{
				dx = x - input_w + 1 - cur_w;
				w_temp = input_w + cur_w;
			}
			else if (Screen_DIR == 3)
			{
				dy = y - input_w + 1 - cur_w;
				h_temp = input_w + cur_w;
			}
		}
		else if (Alignment == 2)
		{
			if (Screen_DIR == 0)
			{
				dx = x - input_w;
				w_temp = input_w;
			}
			else if (Screen_DIR == 1)
			{
				dy = y - input_w;
				h_temp = input_w;
			}
			else if (Screen_DIR == 2)
			{
				w_temp = input_w;
			}
			else if (Screen_DIR == 3)
			{
				h_temp = input_w;
			}
		}

		input_w = data_w;
		LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
							  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
							  Display_Layer, LCD_XSIZE_TFT, dx, dy,
							  0x0c, w_temp, h_temp);
	}
}

uint8_t buff_to_var(uint8_t *buff)
{
	uint8_t i = 0;
	int64_t sum = 0;

	if (neg_flag == 0) // positive number
	{
		for (i = 0; i < i_len; i++)
		{
			sum += pow(10, i_len - 1 - i + gDio_Info[Dio_num].D_num) * (buff[i] - '0');
		}
		for (i = 0; i < d_len; i++)
		{
			sum += pow(10, gDio_Info[Dio_num].D_num - 1 - i) * (buff[i_len + 1 + i] - '0');
		}
	}
	else // negative number
	{
		for (i = 0; i < i_len; i++)
		{
			sum += pow(10, i_len - 1 - i + gDio_Info[Dio_num].D_num) * (buff[i + 1] - '0');
		}
		for (i = 0; i < d_len; i++)
		{
			sum += pow(10, gDio_Info[Dio_num].D_num - 1 - i) * (buff[i_len + 2 + i] - '0');
		}
		sum = sum * (-1);
	}

	if (gDio_Info[Dio_num].Limite_EN == 1)
	{
		if (sum >= gDio_Info[Dio_num].V_min && sum <= gDio_Info[Dio_num].V_max) // The value is within the range limit
		{
			if (gDio_Info[Dio_num].p_tpye == 0x00 || gDio_Info[Dio_num].p_tpye == 0x80)
			{
				var[gDio_Info[Dio_num].p_add * 2 + 1] = sum;
			}
			else if (gDio_Info[Dio_num].p_tpye == 0x01 || gDio_Info[Dio_num].p_tpye == 0x81)
			{
				var[gDio_Info[Dio_num].p_add * 2] = sum >> 8 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 1] = sum & 0xFF;
			}
			else if (gDio_Info[Dio_num].p_tpye == 0x02 || gDio_Info[Dio_num].p_tpye == 0x82)
			{

				var[gDio_Info[Dio_num].p_add * 2] = sum >> 24 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 1] = sum >> 16 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 2] = sum >> 8 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 3] = sum & 0xFF;
			}
			else if (gDio_Info[Dio_num].p_tpye == 0x83)
			{
				if (sum < 0)
				{
					var[gDio_Info[Dio_num].p_add * 2] = sum >> 56 & 0xFF;
					var[gDio_Info[Dio_num].p_add * 2 + 1] = sum >> 48 & 0xFF;
					var[gDio_Info[Dio_num].p_add * 2 + 2] = sum >> 40 & 0xFF;
					var[gDio_Info[Dio_num].p_add * 2 + 3] = sum >> 32 & 0xFF;
					var[gDio_Info[Dio_num].p_add * 2 + 4] = sum >> 24 & 0xFF;
					var[gDio_Info[Dio_num].p_add * 2 + 5] = sum >> 16 & 0xFF;
					var[gDio_Info[Dio_num].p_add * 2 + 6] = sum >> 8 & 0xFF;
					var[gDio_Info[Dio_num].p_add * 2 + 7] = sum & 0xFF;
				}
				else
				{
					var[gDio_Info[Dio_num].p_add * 2] = sum >> 56 & 0xFF;
					var[gDio_Info[Dio_num].p_add * 2 + 1] = sum >> 48 & 0xFF;
					var[gDio_Info[Dio_num].p_add * 2 + 2] = sum >> 40 & 0xFF;
					var[gDio_Info[Dio_num].p_add * 2 + 3] = sum >> 32 & 0xFF;
					var[gDio_Info[Dio_num].p_add * 2 + 4] = sum >> 24 & 0xFF;
					var[gDio_Info[Dio_num].p_add * 2 + 5] = sum >> 16 & 0xFF;
					var[gDio_Info[Dio_num].p_add * 2 + 6] = sum >> 8 & 0xFF;
					var[gDio_Info[Dio_num].p_add * 2 + 7] = sum & 0xFF;
				}
			}
			return 0;
		}
		else
			return 1; // Not within the range of values
	}
	else
	{
		if (gDio_Info[Dio_num].p_tpye == 0x00 || gDio_Info[Dio_num].p_tpye == 0x80)
		{
			var[gDio_Info[Dio_num].p_add * 2 + 1] = sum;
		}
		else if (gDio_Info[Dio_num].p_tpye == 0x01 || gDio_Info[Dio_num].p_tpye == 0x81)
		{
			var[gDio_Info[Dio_num].p_add * 2] = sum >> 8 & 0xFF;
			var[gDio_Info[Dio_num].p_add * 2 + 1] = sum & 0xFF;
		}
		else if (gDio_Info[Dio_num].p_tpye == 0x02 || gDio_Info[Dio_num].p_tpye == 0x82)
		{
			var[gDio_Info[Dio_num].p_add * 2] = sum >> 24 & 0xFF;
			var[gDio_Info[Dio_num].p_add * 2 + 1] = sum >> 16 & 0xFF;
			var[gDio_Info[Dio_num].p_add * 2 + 2] = sum >> 8 & 0xFF;
			var[gDio_Info[Dio_num].p_add * 2 + 3] = sum & 0xFF;
		}
		else if (gDio_Info[Dio_num].p_tpye == 0x83)
		{
			if (sum < 0)
			{
				var[gDio_Info[Dio_num].p_add * 2] = sum >> 56 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 1] = sum >> 48 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 2] = sum >> 40 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 3] = sum >> 32 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 4] = sum >> 24 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 5] = sum >> 16 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 6] = sum >> 8 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 7] = sum & 0xFF;
			}
			else
			{
				var[gDio_Info[Dio_num].p_add * 2] = sum >> 56 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 1] = sum >> 48 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 2] = sum >> 40 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 3] = sum >> 32 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 4] = sum >> 24 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 5] = sum >> 16 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 6] = sum >> 8 & 0xFF;
				var[gDio_Info[Dio_num].p_add * 2 + 7] = sum & 0xFF;
			}
		}
		return 0;
	}
}

/***********************************************************************************
 * Function Name	: data_input
 * Description		: Touch detection of ASCII keyboard.
 * Input			: None
 * Output       	: None
 * Return       	: None
 ************************************************************************************/
void data_input(void)
{
	uint8_t res, i;
	uint8_t width_Hor = 0, w_temp, h_temp; 
	uint8_t buff[12], rebuf[20];
	uint16_t canvas_w = 0;
	uint16_t dx, dy;
	uint32_t zk_addr, color;

	if (gTpInfo.sta && DioKb_flag == 0) // press down
	{
		for (i = 0; i < Dio_count; i++)
		{
			if (gTpInfo.x[0] >= gDio_Info[i].Xs && gTpInfo.y[0] >= gDio_Info[i].Ys &&
				gTpInfo.x[0] <= gDio_Info[i].Xe && gTpInfo.y[0] <= gDio_Info[i].Ye &&
				Dio_flag == 0 && button_Press_flag == 0)
			{
				Dio_num = i;
				Dio_flag = 1;
				button_Press_flag = 1;
				if (buzzer)
					touch_buzzer();
			}
		}
	}
	if (Button_Key_Code_Flag == 1)
	{
		for (i = 0; i < Dio_count; i++)
		{
			if (gDio_Info[i].Xs == 0xFFFF && gDio_Info[i].Ys ==  Button_Key_Code)
			{
				Button_Key_Code_Flag = 0;
				Button_Key_Code = 0;
				Dio_num = i;
				Dio_flag = 1;
				if (Pop_up_flag == 1) // pop-up window
				{
					LT768_BTE_Memory_Copy(Display_Layer, LCD_XSIZE_TFT, 0, 0,
										  Display_Layer, LCD_XSIZE_TFT, 0, 0,
										  LAY_BUFF7, LCD_XSIZE_TFT, 0, 0,
										  0x0c, LCD_XSIZE_TFT, LCD_YSIZE_TFT);
				}
				Flash_Read_UI(buff, addr_index[6] + 12 * gDio_Info[i].kb_id, 12);
				pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
				pic_w = buff[4] + (buff[5] << 8);
				pic_h = buff[6] + (buff[7] << 8);
				pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
				pic_flag = buff[11];

				pic_id_temp = pic_id;
				pic_id = gDio_Info[i].kb_id;
				if (pic_len != 0 && pic_flag != 0xFF)
				{
					canvas_w = Get_Canvas_Width(pic_w);
					Canvas_Image_Start_address(LAY_BUFF1);
					Canvas_image_width(canvas_w);

					LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
														pic_w, pic_h,
														pic_w, pic_add,
														LAY_BUFF1, canvas_w);

					if (pic_flag == 0)
					{
						LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
											  LAY_BUFF1, canvas_w, 0, 0,
											  Display_Layer, LCD_XSIZE_TFT, gDio_Info[i].kb_x, gDio_Info[i].kb_y,
											  0x0c, pic_w, pic_h);

						LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
											  LAY_BUFF1, canvas_w, 0, 0,
											  MAIN_BUFF, LCD_XSIZE_TFT, gDio_Info[i].kb_x, gDio_Info[i].kb_y,
											  0x0c, pic_w, pic_h);
					}
					else if (pic_flag == 1)
					{
						LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
														 Display_Layer, LCD_XSIZE_TFT, gDio_Info[i].kb_x, gDio_Info[i].kb_y,
														 Black, pic_w, pic_h);

						LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
														 MAIN_BUFF, LCD_XSIZE_TFT, gDio_Info[i].kb_x, gDio_Info[i].kb_y,
														 Black, pic_w, pic_h);
					}
					else if (pic_flag == 2)
					{

						BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, gDio_Info[i].kb_x, gDio_Info[i].kb_y,
													   LAY_BUFF1, canvas_w, 0, 0,
													   Display_Layer, LCD_XSIZE_TFT, gDio_Info[i].kb_x, gDio_Info[i].kb_y,
													   pic_w, pic_h);

						BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, gDio_Info[i].kb_x, gDio_Info[i].kb_y,
													   LAY_BUFF1, canvas_w, 0, 0,
													   MAIN_BUFF, LCD_XSIZE_TFT, gDio_Info[i].kb_x, gDio_Info[i].kb_y,
													   pic_w, pic_h);
					}
				}
				/*Dynamic picture coverage judgment*/
				DioKb_flag = 1;
				gDio_Info[i].Xs = gDio_Info[i].kb_x;
				gDio_Info[i].Ys = gDio_Info[i].kb_y;
				gDio_Info[i].Xe = gDio_Info[i].kb_x + pic_w;
				gDio_Info[i].Ye = gDio_Info[i].kb_y + pic_h;

				/************/
				Flash_Read_UI(buff, addr_index[2] + gDio_Info[i].zk_id * 8, 4);
				zk_addr = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
				cur_x = gDio_Info[i].dp_x;
				cur_y = gDio_Info[i].dp_y;
				buff[0] = '|';
				buff[1] = 0;

				if (gDio_Info[i].cursor_color == 0)
					color = Black;
				else
					color = White;

				Flash_Read_UI(&gDio_Info[i].zk_h, zk_addr + 3, 1);
				Flash_Read_UI(&res, zk_addr + 9 + 92, 1);
				if (Screen_DIR == 0)
				{
					dx = cur_x;
					dy = cur_y;
					w_temp = res;
					h_temp = gDio_Info[i].zk_h;
				}
				else if (Screen_DIR == 1)
				{
					dx = cur_x;
					dy = cur_y;
					cur_x = cur_x - gDio_Info[i].zk_h + 1;
					cur_y = cur_y;
					w_temp = gDio_Info[i].zk_h;
					h_temp = res;
				}
				else if (Screen_DIR == 2)
				{
					dx = cur_x;
					dy = cur_y;
					cur_x = cur_x - res + 1;
					cur_y = cur_y - gDio_Info[i].zk_h + 1;
					w_temp = res;
					h_temp = gDio_Info[i].zk_h;
				}
				else if (Screen_DIR == 3)
				{
					dx = cur_x;
					dy = cur_y;
					w_temp = gDio_Info[i].zk_h;
					h_temp = res;
				}

				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, cur_x, cur_y,
									  MAIN_BUFF, LCD_XSIZE_TFT, cur_x, cur_y,
									  LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
									  0x0c, w_temp, h_temp);

				LT_Print_zk_ASCII(zk_addr, LAY_BUFF1, LCD_XSIZE_TFT,
								  gDio_Info[i].zk_w, gDio_Info[i].zk_h, 0, color,
								  dx, dy, buff);

				cur_w = data_w - 2;

				if (Screen_DIR == 3)
				{
					cur_y = cur_y - cur_w + 1;
				}

				LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y, // Store cursor in buffer
									  LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
									  LAY_BUFF5, LCD_XSIZE_TFT, cur_x, cur_y,
									  0x0c, w_temp, h_temp);

				LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y, // Show Cursor
									  LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
									  Display_Layer, LCD_XSIZE_TFT, cur_x, cur_y,
									  0x0c, w_temp, h_temp);

				Get_Touch_info(gDio_Info[i].kb_id);
				button_Press_flag = 0;
				Cusor_time = 0;
				Dio_flag = 0;
				i_len = 0;
				d_len = 0;
				input_len = 0;
				neg_flag = 0;
				dot_flag = 0;
				input_w = 0;
				memset(Display_buff, 0, 400);
			}
		}
	}
	if (gTpInfo.sta == 0 && Dio_flag == 1 && DioKb_flag == 0) // raise
	{
		if (Pop_up_flag == 1) // pop-up window
		{
			LT768_BTE_Memory_Copy(Display_Layer, LCD_XSIZE_TFT, 0, 0,
								  Display_Layer, LCD_XSIZE_TFT, 0, 0,
								  LAY_BUFF7, LCD_XSIZE_TFT, 0, 0,
								  0x0c, LCD_XSIZE_TFT, LCD_YSIZE_TFT);
		}
		Flash_Read_UI(buff, addr_index[6] + 12 * gDio_Info[Dio_num].kb_id, 12);
		pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
		pic_w = buff[4] + (buff[5] << 8);
		pic_h = buff[6] + (buff[7] << 8);
		pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
		pic_flag = buff[11];

		pic_id_temp = pic_id;
		pic_id = gDio_Info[Dio_num].kb_id;
		if (pic_len != 0 && pic_flag != 0xFF)
		{
			canvas_w = Get_Canvas_Width(pic_w);
			Canvas_Image_Start_address(LAY_BUFF1);
			Canvas_image_width(canvas_w);

			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												pic_w, pic_h,
												pic_w, pic_add,
												LAY_BUFF1, canvas_w);

			if (pic_flag == 0)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF1, canvas_w, 0, 0,
									  Display_Layer, LCD_XSIZE_TFT, gDio_Info[Dio_num].kb_x, gDio_Info[Dio_num].kb_y,
									  0x0c, pic_w, pic_h);

				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF1, canvas_w, 0, 0,
									  MAIN_BUFF, LCD_XSIZE_TFT, gDio_Info[Dio_num].kb_x, gDio_Info[Dio_num].kb_y,
									  0x0c, pic_w, pic_h);
			}
			else if (pic_flag == 1)
			{
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
												 Display_Layer, LCD_XSIZE_TFT, gDio_Info[Dio_num].kb_x, gDio_Info[Dio_num].kb_y,
												 Black, pic_w, pic_h);

				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
												 MAIN_BUFF, LCD_XSIZE_TFT, gDio_Info[Dio_num].kb_x, gDio_Info[Dio_num].kb_y,
												 Black, pic_w, pic_h);
			}
			else if (pic_flag == 2)
			{

				BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, gDio_Info[Dio_num].kb_x, gDio_Info[Dio_num].kb_y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   Display_Layer, LCD_XSIZE_TFT, gDio_Info[Dio_num].kb_x, gDio_Info[Dio_num].kb_y,
											   pic_w, pic_h);

				BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, gDio_Info[Dio_num].kb_x, gDio_Info[Dio_num].kb_y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   MAIN_BUFF, LCD_XSIZE_TFT, gDio_Info[Dio_num].kb_x, gDio_Info[Dio_num].kb_y,
											   pic_w, pic_h);
			}
		}
		/*Dynamic picture coverage judgment*/
		DioKb_flag = 1;
		gDio_Info[Dio_num].Xs = gDio_Info[Dio_num].kb_x;
		gDio_Info[Dio_num].Ys = gDio_Info[Dio_num].kb_y;
		gDio_Info[Dio_num].Xe = gDio_Info[Dio_num].kb_x + pic_w;
		gDio_Info[Dio_num].Ye = gDio_Info[Dio_num].kb_y + pic_h;

		/************/
		Flash_Read_UI(buff, addr_index[2] + gDio_Info[Dio_num].zk_id * 8, 4);
		zk_addr = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
		cur_x = gDio_Info[Dio_num].dp_x;
		cur_y = gDio_Info[Dio_num].dp_y;
		buff[0] = '|';
		buff[1] = 0;

		if (gDio_Info[Dio_num].cursor_color == 0)
			color = Black;
		else
			color = White;

		Flash_Read_UI(&gDio_Info[Dio_num].zk_h, zk_addr + 3, 1);
		Flash_Read_UI(&res, zk_addr + 9 + 92, 1);
		if (Screen_DIR == 0)
		{
			dx = cur_x;
			dy = cur_y;
			w_temp = res;
			h_temp = gDio_Info[Dio_num].zk_h;
		}
		else if (Screen_DIR == 1)
		{
			dx = cur_x;
			dy = cur_y;
			cur_x = cur_x - gDio_Info[Dio_num].zk_h + 1;
			cur_y = cur_y;
			w_temp = gDio_Info[Dio_num].zk_h;
			h_temp = res;
		}
		else if (Screen_DIR == 2)
		{
			dx = cur_x;
			dy = cur_y;
			cur_x = cur_x - res + 1;
			cur_y = cur_y - gDio_Info[Dio_num].zk_h + 1;
			w_temp = res;
			h_temp = gDio_Info[Dio_num].zk_h;
		}
		else if (Screen_DIR == 3)
		{
			dx = cur_x;
			dy = cur_y;
			w_temp = gDio_Info[Dio_num].zk_h;
			h_temp = res;
		}

		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, cur_x, cur_y,
							  MAIN_BUFF, LCD_XSIZE_TFT, cur_x, cur_y,
							  LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
							  0x0c, w_temp, h_temp);

		LT_Print_zk_ASCII(zk_addr, LAY_BUFF1, LCD_XSIZE_TFT,
						  gDio_Info[Dio_num].zk_w, gDio_Info[Dio_num].zk_h, 0, color,
						  dx, dy, buff);

		cur_w = data_w - 2;

		if (Screen_DIR == 3)
		{
			cur_y = cur_y - cur_w + 1;
		}

		LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y, // Store cursor in buffer
							  LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
							  LAY_BUFF5, LCD_XSIZE_TFT, cur_x, cur_y,
							  0x0c, w_temp, h_temp);

		LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y, // Show Cursor
							  LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
							  Display_Layer, LCD_XSIZE_TFT, cur_x, cur_y,
							  0x0c, w_temp, h_temp);

		Get_Touch_info(gDio_Info[Dio_num].kb_id);
		button_Press_flag = 0;
		Cusor_time = 0;
		Dio_flag = 0;
		i_len = 0;
		d_len = 0;
		input_len = 0;
		neg_flag = 0;
		dot_flag = 0;
		input_w = 0;
		memset(Display_buff, 0, 400);
	}

	if (gTpInfo.sta && DioKb_flag == 1 && Dio_flag == 0) // Press down within the range of keyboard
	{
		for (i = 0; i < Kb_count; i++)
		{
			if (gTpInfo.x[0] >= gKb_Info[i].Xs + gDio_Info[Dio_num].kb_x && gTpInfo.y[0] >= gKb_Info[i].Ys + gDio_Info[Dio_num].kb_y &&
				gTpInfo.x[0] <= gKb_Info[i].Xe + gDio_Info[Dio_num].kb_x && gTpInfo.y[0] <= gKb_Info[i].Ye + gDio_Info[Dio_num].kb_y &&
				Kb_flag == 0)
			{
				if (gKb_Info[i].press_id != 0xFFFF)
				{
					LT768_BTE_Memory_Copy(LAY_BUFF6, LCD_XSIZE_TFT, gDio_Info[Dio_num].kb_x + gKb_Info[i].Xs, gDio_Info[Dio_num].kb_y + gKb_Info[i].Ys,
										  LAY_BUFF6, LCD_XSIZE_TFT, gDio_Info[Dio_num].kb_x + gKb_Info[i].Xs, gDio_Info[Dio_num].kb_y + gKb_Info[i].Ys,
										  Display_Layer, LCD_XSIZE_TFT, gDio_Info[Dio_num].kb_x + gKb_Info[i].Xs, gDio_Info[Dio_num].kb_y + gKb_Info[i].Ys,
										  0x0c, gKb_Info[i].Xe - gKb_Info[i].Xs + 1, gKb_Info[i].Ye - gKb_Info[i].Ys + 1);
				}
				Kb_num = i;
				Kb_flag = 1;
				if (buzzer)
					touch_buzzer();
			}
		}
	}
	if (gTpInfo.sta == 0 && DioKb_flag == 1 && Kb_flag == 1) // raise
	{
		if (gKb_Info[Kb_num].press_id != 0xFFFF)
		{
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gDio_Info[Dio_num].kb_x + gKb_Info[Kb_num].Xs, gDio_Info[Dio_num].kb_y + gKb_Info[Kb_num].Ys,
								  MAIN_BUFF, LCD_XSIZE_TFT, gDio_Info[Dio_num].kb_x + gKb_Info[Kb_num].Xs, gDio_Info[Dio_num].kb_y + gKb_Info[Kb_num].Ys,
								  Display_Layer, LCD_XSIZE_TFT, gDio_Info[Dio_num].kb_x + gKb_Info[Kb_num].Xs, gDio_Info[Dio_num].kb_y + gKb_Info[Kb_num].Ys,
								  0x0c, gKb_Info[Kb_num].Xe - gKb_Info[Kb_num].Xs + 1, gKb_Info[Kb_num].Ye - gKb_Info[Kb_num].Ys + 1);
		}

		if (gKb_Info[Kb_num].Keyvalue == 0x2E && gDio_Info[Dio_num].D_num > 0 && dot_flag == 0) // '.'
		{
			if ((input_len > 0 && neg_flag == 0) || (input_len > 1 && neg_flag == 1))
			{
				Display_buff[input_len] = gKb_Info[Kb_num].Keyvalue & 0xff;
				input_len++;
				dot_flag = 1;
				input_display();
			}
		}
		else if (gKb_Info[Kb_num].Keyvalue == 0x2D && neg_flag == 0 && i_len == 0 && d_len == 0) // '-'
		{
			if (gDio_Info[Dio_num].p_tpye == 0x80 || gDio_Info[Dio_num].p_tpye == 0x81 || gDio_Info[Dio_num].p_tpye == 0x82 || gDio_Info[Dio_num].p_tpye == 0x83)
			{
				neg_flag = 1;
				Display_buff[input_len] = gKb_Info[Kb_num].Keyvalue & 0xff;
				input_len++;
				input_display();
			}
		}
		else if (gKb_Info[Kb_num].Keyvalue == 0xF0) // cancel
		{
			DioKb_flag = 0;
			if (Pop_up_flag == 0)
				Display_page(pic_id_temp);
			else if (Pop_up_flag == 1)
			{
				Display_Layer = LAY_BUFF7;
				Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
			}
		}
		else if (gKb_Info[Kb_num].Keyvalue == 0xF2) // backspace
		{
			if (dot_flag == 1) // with decimal point
			{
				if (d_len > 0)
					d_len--;
				else
					dot_flag = 0;
			}
			else if (i_len > 0)
				i_len--;

			if (input_len > 0)
			{
				input_len--;
				if (input_len == 0 && neg_flag == 1)
					neg_flag = 0;
				Display_buff[input_len] = 0x00;
				input_display();
			}
		}
		else if (gKb_Info[Kb_num].Keyvalue == 0xF1) // confirm
		{
			res = buff_to_var(Display_buff);
			/*Customized Code_ Triggered once after pressing the confirmation key on the keyboard
			if(gDio_Info[Dio_num].p_add == 0x2002)
			{
				Master_mode03_flag[0] = 1;
				Master_mode03_Var[2*0]   = var[0x2002 * 2];
				Master_mode03_Var[2*0+1] = var[0x2002 * 2+1];
			}
			else if(gDio_Info[Dio_num].p_add == 0x2003)
			{
				Master_mode03_flag[1] = 1;
				Master_mode03_Var[2*1]   = var[0x2003 * 2];
				Master_mode03_Var[2*1+1] = var[0x2003 * 2+1];
			}
			*/
			if (res == 0)
			{
				if (gDio_Info[Dio_num].Code == 0xC002)
				{
					rebuf[3] = 0x41;
					rebuf[4] = gDio_Info[Dio_num].p_add >> 8;
					rebuf[5] = gDio_Info[Dio_num].p_add & 0xFF;
					if (gDio_Info[Dio_num].p_tpye == 0x00 || gDio_Info[Dio_num].p_tpye == 0x80)
					{
						rebuf[6] = var[gDio_Info[Dio_num].p_add * 2];
						rebuf[7] = var[gDio_Info[Dio_num].p_add * 2 + 1];
						LT_SendData_CRC_Frame(rebuf, 5);
					}
					else if (gDio_Info[Dio_num].p_tpye == 0x01 || gDio_Info[Dio_num].p_tpye == 0x81)
					{
						rebuf[6] = var[gDio_Info[Dio_num].p_add * 2];
						rebuf[7] = var[gDio_Info[Dio_num].p_add * 2 + 1];
						LT_SendData_CRC_Frame(rebuf, 5);
					}
					else if (gDio_Info[Dio_num].p_tpye == 0x02 || gDio_Info[Dio_num].p_tpye == 0x82)
					{
						rebuf[6] = var[gDio_Info[Dio_num].p_add * 2];
						rebuf[7] = var[gDio_Info[Dio_num].p_add * 2 + 1];
						rebuf[8] = var[gDio_Info[Dio_num].p_add * 2 + 2];
						rebuf[9] = var[gDio_Info[Dio_num].p_add * 2 + 3];
						LT_SendData_CRC_Frame(rebuf, 7);
					}
					else if (gDio_Info[Dio_num].p_tpye == 0x03 || gDio_Info[Dio_num].p_tpye == 0x83)
					{
						rebuf[6] = var[gDio_Info[Dio_num].p_add * 2];
						rebuf[7] = var[gDio_Info[Dio_num].p_add * 2 + 1];
						rebuf[8] = var[gDio_Info[Dio_num].p_add * 2 + 2];
						rebuf[9] = var[gDio_Info[Dio_num].p_add * 2 + 3];
						rebuf[10] = var[gDio_Info[Dio_num].p_add * 2 + 4];
						rebuf[11] = var[gDio_Info[Dio_num].p_add * 2 + 5];
						rebuf[12] = var[gDio_Info[Dio_num].p_add * 2 + 6];
						rebuf[13] = var[gDio_Info[Dio_num].p_add * 2 + 7];
						LT_SendData_CRC_Frame(rebuf, 11);
					}
				}
				DioKb_flag = 0;
				reg_operation(gDio_Info[Dio_num].p_add);
				if (Pop_up_flag == 0)
					Display_page(pic_id_temp);
				else if (Pop_up_flag == 1)
				{
					Display_Layer = LAY_BUFF7;
					Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
				}
			}
		}
		else if (gKb_Info[Kb_num].Keyvalue >= '0' && gKb_Info[Kb_num].Keyvalue <= '9') // Digital input
		{
			if ((i_len < gDio_Info[Dio_num].I_num && dot_flag == 0) || (d_len < gDio_Info[Dio_num].D_num && dot_flag == 1))
			{
				Display_buff[input_len] = gKb_Info[Kb_num].Keyvalue & 0xff;
				if (dot_flag == 0)
					i_len++; // Integer length plus
				else
					d_len++; // Decimal length plus
				input_len++; // Display buff length plus
				input_display();
			}
		}
		Kb_flag = 0;
	}
}
/********************************************************/

SldMenu_Info gSldMenu_Info[SldMenu_Size];

uint8_t sldmenu_count = 0, sldmenu_num = 0;
uint8_t sldmenu_flag = 0;
uint8_t sld_mum = 0;
uint8_t V_dev = 0;
uint16_t lastx = 0, lasty = 0, Sldw = 0, Sldh = 0;
uint32_t Sld_Offset_Addr = 0;
/***********************************************************************************
 * Function Name	: slideMune
 * Description		: Touch detection of slding menu.
 * Input			: None
 * Output       	: None
 * Return       	: None
 ************************************************************************************/
void slideMune(void)
{
	uint8_t i = 0;
	uint8_t rebuf[20];
	uint16_t temp, sum, V;
	uint16_t dev = 0, pos1;
	int16_t df = 0;

	if (gTpInfo.sta)
	{
		for (i = 0; i < sldmenu_count; i++)
		{
			if ((gTpInfo.x[0] >= gSldMenu_Info[i].Xs && gTpInfo.y[0] >= gSldMenu_Info[i].Ys &&
				 gTpInfo.x[0] <= gSldMenu_Info[i].Xe && gTpInfo.y[0] <= gSldMenu_Info[i].Ye &&
				 button_Press_flag == 0) ||
				sldmenu_flag == 1)
			{
				if (sldmenu_flag == 0) // First press
				{
					DL_menu(i); // Cache pictures

					lastx = gTpInfo.x[0];
					lasty = gTpInfo.y[0];
					sldmenu_num = i;
					sldmenu_flag = 1;
					button_Press_flag = 1;
					if (buzzer)
						touch_buzzer();
					V_dev = 0;
				}
				else // Sliding treatment
				{
					if ((gSldMenu_Info[sldmenu_num].dir & 0x01) == 0) // Slide up and down
					{
						if (Screen_DIR == 3 || Screen_DIR == 2)
							df = gTpInfo.y[0] - lasty;
						else
							df = lasty - gTpInfo.y[0];

						gSldMenu_Info[sldmenu_num].pos = gSldMenu_Info[sldmenu_num].pos + df;

						if (gSldMenu_Info[sldmenu_num].p_add == 0x7004) // day is special
						{
							if (var[0x7003 * 2 + 1] == 1 || var[0x7003 * 2 + 1] == 3 || var[0x7003 * 2 + 1] == 5 || var[0x7003 * 2 + 1] == 7 ||
								var[0x7003 * 2 + 1] == 8 || var[0x7003 * 2 + 1] == 10 || var[0x7003 * 2 + 1] == 12)
							{
								if (gSldMenu_Info[sldmenu_num].V_max > 31)
									dev = (gSldMenu_Info[sldmenu_num].V_max - 31) * (gSldMenu_Info[sldmenu_num].Ye - gSldMenu_Info[sldmenu_num].Ys - gSldMenu_Info[sldmenu_num].L1 - gSldMenu_Info[sldmenu_num].L2);
								V_dev = 0;
							}
							else if (var[0x7003 * 2 + 1] == 4 || var[0x7003 * 2 + 1] == 6 || var[0x7003 * 2 + 1] == 9 || var[0x7003 * 2 + 1] == 11)
							{
								if (gSldMenu_Info[sldmenu_num].V_max > 30)
									dev = (gSldMenu_Info[sldmenu_num].V_max - 30) * (gSldMenu_Info[sldmenu_num].Ye - gSldMenu_Info[sldmenu_num].Ys - gSldMenu_Info[sldmenu_num].L1 - gSldMenu_Info[sldmenu_num].L2);
								V_dev = 1;
							}
							else if (var[0x7003 * 2 + 1] == 2)
							{
								if (Is_Leap_Year(var[0x7002 * 2 + 1] + 2000)) // leap year
								{
									if (gSldMenu_Info[sldmenu_num].V_max > 29)
										dev = (gSldMenu_Info[sldmenu_num].V_max - 29) * (gSldMenu_Info[sldmenu_num].Ye - gSldMenu_Info[sldmenu_num].Ys - gSldMenu_Info[sldmenu_num].L1 - gSldMenu_Info[sldmenu_num].L2);
									V_dev = 2;
								}
								else
								{
									if (gSldMenu_Info[sldmenu_num].V_max > 28)
										dev = (gSldMenu_Info[sldmenu_num].V_max - 28) * (gSldMenu_Info[sldmenu_num].Ye - gSldMenu_Info[sldmenu_num].Ys - gSldMenu_Info[sldmenu_num].L1 - gSldMenu_Info[sldmenu_num].L2);
									V_dev = 3;
								}
							}
						}

						if (gSldMenu_Info[sldmenu_num].dir == 0x80)
						{
							if (gSldMenu_Info[sldmenu_num].pos < 0)
								gSldMenu_Info[sldmenu_num].pos = 0;
							if (gSldMenu_Info[sldmenu_num].pos > (Sldh - dev - (gSldMenu_Info[sldmenu_num].Ye - gSldMenu_Info[sldmenu_num].Ys - gSldMenu_Info[sldmenu_num].L1 - gSldMenu_Info[sldmenu_num].L2)))
								gSldMenu_Info[sldmenu_num].pos = Sldh - dev - (gSldMenu_Info[sldmenu_num].Ye - gSldMenu_Info[sldmenu_num].Ys - gSldMenu_Info[sldmenu_num].L1 - gSldMenu_Info[sldmenu_num].L2);
						}
						else
						{
							if (gSldMenu_Info[sldmenu_num].pos < 0)
								gSldMenu_Info[sldmenu_num].pos = Sldh - dev + gSldMenu_Info[sldmenu_num].pos;
							if (gSldMenu_Info[sldmenu_num].pos > (Sldh - dev))
								gSldMenu_Info[sldmenu_num].pos = gSldMenu_Info[sldmenu_num].pos - (Sldh - dev);
						}
						if (df != 0)
						{
							dp_menu(sldmenu_num);
							lasty = gTpInfo.y[0];
						}
					}
					else if ((gSldMenu_Info[sldmenu_num].dir & 0x01) == 1) // Slide left and right
					{
						if (Screen_DIR == 1 || Screen_DIR == 2)
							df = gTpInfo.x[0] - lastx;
						else
							df = lastx - gTpInfo.x[0];
						gSldMenu_Info[sldmenu_num].pos = gSldMenu_Info[sldmenu_num].pos + df;

						if (gSldMenu_Info[sldmenu_num].p_add == 0x7004) // day is special
						{
							if (var[0x7003 * 2 + 1] == 1 || var[0x7003 * 2 + 1] == 3 || var[0x7003 * 2 + 1] == 5 || var[0x7003 * 2 + 1] == 7 ||
								var[0x7003 * 2 + 1] == 8 || var[0x7003 * 2 + 1] == 10 || var[0x7003 * 2 + 1] == 12)
							{
								if (gSldMenu_Info[sldmenu_num].V_max > 31)
									dev = (gSldMenu_Info[sldmenu_num].V_max - 31) * (gSldMenu_Info[sldmenu_num].Xe - gSldMenu_Info[sldmenu_num].Xs - gSldMenu_Info[sldmenu_num].L1 - gSldMenu_Info[sldmenu_num].L2);
								V_dev = 0;
							}
							else if (var[0x7003 * 2 + 1] == 4 || var[0x7003 * 2 + 1] == 6 || var[0x7003 * 2 + 1] == 9 || var[0x7003 * 2 + 1] == 11)
							{
								if (gSldMenu_Info[sldmenu_num].V_max > 30)
									dev = (gSldMenu_Info[sldmenu_num].V_max - 30) * (gSldMenu_Info[sldmenu_num].Xe - gSldMenu_Info[sldmenu_num].Xs - gSldMenu_Info[sldmenu_num].L1 - gSldMenu_Info[sldmenu_num].L2);
								V_dev = 1;
							}
							else if (var[0x7003 * 2 + 1] == 2)
							{
								if (Is_Leap_Year(var[0x7002 * 2 + 1] + 2000)) // leap year
								{
									if (gSldMenu_Info[sldmenu_num].V_max > 29)
										dev = (gSldMenu_Info[sldmenu_num].V_max - 29) * (gSldMenu_Info[sldmenu_num].Xe - gSldMenu_Info[sldmenu_num].Xs - gSldMenu_Info[sldmenu_num].L1 - gSldMenu_Info[sldmenu_num].L2);
									V_dev = 2;
								}
								else
								{
									if (gSldMenu_Info[sldmenu_num].V_max > 28)
										dev = (gSldMenu_Info[sldmenu_num].V_max - 28) * (gSldMenu_Info[sldmenu_num].Xe - gSldMenu_Info[sldmenu_num].Xs - gSldMenu_Info[sldmenu_num].L1 - gSldMenu_Info[sldmenu_num].L2);
									V_dev = 3;
								}
							}
						}
						if (gSldMenu_Info[sldmenu_num].dir == 0x81)
						{
							if (gSldMenu_Info[sldmenu_num].pos < 0)
								gSldMenu_Info[sldmenu_num].pos = 0; // Scope limit
							if (gSldMenu_Info[sldmenu_num].pos > (Sldw - dev - (gSldMenu_Info[sldmenu_num].Xe - gSldMenu_Info[sldmenu_num].Xs - gSldMenu_Info[sldmenu_num].L1 - gSldMenu_Info[sldmenu_num].L2)))
								gSldMenu_Info[sldmenu_num].pos = Sldw - dev - (gSldMenu_Info[sldmenu_num].Xe - gSldMenu_Info[sldmenu_num].Xs - gSldMenu_Info[sldmenu_num].L1 - gSldMenu_Info[sldmenu_num].L2);
						}
						else
						{
							if (gSldMenu_Info[sldmenu_num].pos < 0)
								gSldMenu_Info[sldmenu_num].pos = Sldw - dev + gSldMenu_Info[sldmenu_num].pos; // Scope limit
							if (gSldMenu_Info[sldmenu_num].pos > (Sldw - dev))
								gSldMenu_Info[sldmenu_num].pos = gSldMenu_Info[sldmenu_num].pos - (Sldw - dev);
						}
						if (df != 0)
						{
							dp_menu(sldmenu_num);
							lastx = gTpInfo.x[0];
						}
					}
				}
			}
		}
	}
	else if (sldmenu_flag == 1 && (gTpInfo.sta == 0 ||
								   !(gTpInfo.x[0] >= gSldMenu_Info[sldmenu_num].Xs && gTpInfo.y[0] >= gSldMenu_Info[sldmenu_num].Ys &&
									 gTpInfo.x[0] <= gSldMenu_Info[sldmenu_num].Xe && gTpInfo.y[0] <= gSldMenu_Info[sldmenu_num].Ye))) // ï¿½ï¿½ï¿½ï¿½
	{
	
		if ((gSldMenu_Info[sldmenu_num].dir & 0x01) == 0) // up and down
			temp = gSldMenu_Info[sldmenu_num].Ye - gSldMenu_Info[sldmenu_num].Ys - gSldMenu_Info[sldmenu_num].L2 - gSldMenu_Info[sldmenu_num].L1;
		else // left and right
			temp = gSldMenu_Info[sldmenu_num].Xe - gSldMenu_Info[sldmenu_num].Xs - gSldMenu_Info[sldmenu_num].L2 - gSldMenu_Info[sldmenu_num].L1;

		if ((gSldMenu_Info[sldmenu_num].pos % temp) > (temp / 2))
		{
			V = (gSldMenu_Info[sldmenu_num].pos / temp + 1) * gSldMenu_Info[sldmenu_num].step + gSldMenu_Info[sldmenu_num].V_min;
			gSldMenu_Info[sldmenu_num].pos = (V - gSldMenu_Info[sldmenu_num].V_min) / gSldMenu_Info[sldmenu_num].step * temp;
			if (V > gSldMenu_Info[sldmenu_num].V_max - V_dev)
				V = gSldMenu_Info[sldmenu_num].V_min;

			var[gSldMenu_Info[sldmenu_num].p_add * 2] = (V >> 8) & 0xFF;
			var[gSldMenu_Info[sldmenu_num].p_add * 2 + 1] = V & 0xFF;
		}
		else
		{
			V = (gSldMenu_Info[sldmenu_num].pos / temp) * gSldMenu_Info[sldmenu_num].step + gSldMenu_Info[sldmenu_num].V_min;
			gSldMenu_Info[sldmenu_num].pos = (V - gSldMenu_Info[sldmenu_num].V_min) / gSldMenu_Info[sldmenu_num].step * temp;
			if (V > gSldMenu_Info[sldmenu_num].V_max - V_dev)
				V = gSldMenu_Info[sldmenu_num].V_min;

			var[gSldMenu_Info[sldmenu_num].p_add * 2] = (V >> 8) & 0xFF;
			var[gSldMenu_Info[sldmenu_num].p_add * 2 + 1] = V & 0xFF;
		}
		if (gSldMenu_Info[sldmenu_num].Code == 0xC006)
		{
			rebuf[3] = 0x41;
			rebuf[4] = gSldMenu_Info[sldmenu_num].p_add >> 8;
			rebuf[5] = gSldMenu_Info[sldmenu_num].p_add & 0xFF;
			rebuf[6] = var[gSldMenu_Info[sldmenu_num].p_add * 2];
			rebuf[7] = var[gSldMenu_Info[sldmenu_num].p_add * 2 + 1];
			LT_SendData_CRC_Frame(rebuf, 5);
		}
		dp_menu(sldmenu_num);
		if (gSldMenu_Info[sldmenu_num].p_add >= 0x7000)
			reg_operation(gSldMenu_Info[sldmenu_num].p_add);

		page_date_update(pic_id);
		sldmenu_flag = 0;
		button_Press_flag = 0;
	}
}

/***********************************************************************************
 * Function Name	: dp_menu
 * Description		: Display menu.
 * Input			: - num: which number of sliding menu
 * Output       	: None
 * Return       	: None
 ************************************************************************************/
void dp_menu(uint8_t num)
{
	uint16_t canvas_w = 0;
	uint16_t SldWH;

	if ((gSldMenu_Info[num].dir & 0x01) == 0) // up and down
	{
		if (TFT_bitcolor == 1 && gSldMenu_Info[num].flag == 2)
			Memory_16bpp_Mode();
		if (gSldMenu_Info[num].pos > Sldh)
			gSldMenu_Info[num].pos = Sldh;

		SldWH = (gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys - gSldMenu_Info[num].L2 - gSldMenu_Info[num].L1);
		canvas_w = Get_Canvas_Width(Sldw);
		if (Screen_DIR == 3 || Screen_DIR == 2)
		{
			if ((Sldh - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH) < gSldMenu_Info[num].L1)
			{
				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, V_dev * SldWH,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, V_dev * SldWH,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, gSldMenu_Info[num].L1 - (Sldh - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH),
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys + (Sldh - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH) - gSldMenu_Info[num].L1,gSldMenu_Info[num].flag);

				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, Sldh - gSldMenu_Info[num].L1 + (Sldh - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH),
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0,  Sldh - gSldMenu_Info[num].L1 + (Sldh - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH),
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
									  gSldMenu_Info[num].L1 - (Sldh - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH),gSldMenu_Info[num].flag);

				if (Sldh - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH >= 0)
				{
					LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, 0, Sldh - SldWH - gSldMenu_Info[num].pos,
										  slideMune_BUFF, canvas_w, 0, Sldh - SldWH - gSldMenu_Info[num].pos,
										  LAY_BUFF1, LCD_XSIZE_TFT, 0, gSldMenu_Info[num].L1,
										  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs, SldWH,gSldMenu_Info[num].flag);
				}
				else
				{
					LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, 0, V_dev * SldWH,
										  slideMune_BUFF, canvas_w, 0, V_dev * SldWH,
										  LAY_BUFF1, LCD_XSIZE_TFT, 0, gSldMenu_Info[num].L1 - (Sldh - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH),
										  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
										  Sldh - gSldMenu_Info[num].pos - V_dev * SldWH,gSldMenu_Info[num].flag);

					LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, 0, Sldh + (Sldh - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH),
										  slideMune_BUFF, canvas_w, 0, Sldh + (Sldh - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH),
										  LAY_BUFF1, LCD_XSIZE_TFT, 0, gSldMenu_Info[num].L1,
										  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
										  SldWH - Sldh + gSldMenu_Info[num].pos + V_dev * SldWH,gSldMenu_Info[num].flag);
				}
			}
			else
			{
				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, Sldh - gSldMenu_Info[num].pos - SldWH - gSldMenu_Info[num].L1,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, Sldh - gSldMenu_Info[num].pos - SldWH - gSldMenu_Info[num].L1,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);

	
				if (gSldMenu_Info[num].pos < gSldMenu_Info[num].L2)
				{
					LT768_BTE_Memory_Copy_1(slideMune_BUFF+ Sld_Offset_Addr, canvas_w, 0, 0,
										  slideMune_BUFF+ Sld_Offset_Addr, canvas_w, 0, 0,
										  LAY_BUFF1, LCD_XSIZE_TFT, 0, gSldMenu_Info[num].pos + SldWH + gSldMenu_Info[num].L1,
										  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
										  gSldMenu_Info[num].L2 - gSldMenu_Info[num].pos,gSldMenu_Info[num].flag);
				}

				LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, 0, Sldh - gSldMenu_Info[num].pos - SldWH,
									  slideMune_BUFF, canvas_w, 0, Sldh - gSldMenu_Info[num].pos - SldWH,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, gSldMenu_Info[num].L1,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs, SldWH,gSldMenu_Info[num].flag);
			}
		}
		else
		{
			if (gSldMenu_Info[num].pos < gSldMenu_Info[num].L1)
			{
				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, 0,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, gSldMenu_Info[num].L1 - gSldMenu_Info[num].pos,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys + gSldMenu_Info[num].pos - gSldMenu_Info[num].L1,
									  gSldMenu_Info[num].flag);

				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, Sldh + gSldMenu_Info[num].pos - gSldMenu_Info[num].L1,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, Sldh + gSldMenu_Info[num].pos - gSldMenu_Info[num].L1,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
									  gSldMenu_Info[num].L1 - gSldMenu_Info[num].pos,gSldMenu_Info[num].flag);
	
			
				LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, 0, gSldMenu_Info[num].pos,
									  slideMune_BUFF, canvas_w, 0, gSldMenu_Info[num].pos,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, gSldMenu_Info[num].L1,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys - gSldMenu_Info[num].L2 - gSldMenu_Info[num].L1,
									  gSldMenu_Info[num].flag);
			}
			else if (gSldMenu_Info[num].pos + V_dev * (gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys - gSldMenu_Info[num].L1 - gSldMenu_Info[num].L2) + gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys - gSldMenu_Info[num].L1 > Sldh)
			{
				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, gSldMenu_Info[num].pos - gSldMenu_Info[num].L1,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, gSldMenu_Info[num].pos - gSldMenu_Info[num].L1,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
									  Sldh - V_dev * (gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys - gSldMenu_Info[num].L1 - gSldMenu_Info[num].L2) - gSldMenu_Info[num].pos + gSldMenu_Info[num].L1,
									  gSldMenu_Info[num].flag);

				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, 0,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, Sldh - gSldMenu_Info[num].pos + gSldMenu_Info[num].L1 - V_dev * (gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys - gSldMenu_Info[num].L1 - gSldMenu_Info[num].L2),
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys - (Sldh - V_dev * (gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys - gSldMenu_Info[num].L1 - gSldMenu_Info[num].L2) - gSldMenu_Info[num].pos + gSldMenu_Info[num].L1),
									  gSldMenu_Info[num].flag);

				if (Sldh - gSldMenu_Info[num].pos - V_dev * (gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys - gSldMenu_Info[num].L1 - gSldMenu_Info[num].L2) >=
					gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys - gSldMenu_Info[num].L2 - gSldMenu_Info[num].L1)
				{
					LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, 0, gSldMenu_Info[num].pos,
										  slideMune_BUFF, canvas_w, 0, gSldMenu_Info[num].pos,
										  LAY_BUFF1, LCD_XSIZE_TFT, 0, gSldMenu_Info[num].L1,
										  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
										  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys - gSldMenu_Info[num].L2 - gSldMenu_Info[num].L1,
										  gSldMenu_Info[num].flag);
		
				}
				else
				{
					LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, 0, gSldMenu_Info[num].pos,
										  slideMune_BUFF, canvas_w, 0, gSldMenu_Info[num].pos,
										  LAY_BUFF1, LCD_XSIZE_TFT, 0, gSldMenu_Info[num].L1,
										  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
										  (Sldh - gSldMenu_Info[num].pos - V_dev * (gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys - gSldMenu_Info[num].L1 - gSldMenu_Info[num].L2)),
										  gSldMenu_Info[num].flag);

					LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, 0, 0,
										  slideMune_BUFF, canvas_w, 0, 0,
										  LAY_BUFF1, LCD_XSIZE_TFT, 0, gSldMenu_Info[num].L1 + Sldh - gSldMenu_Info[num].pos - V_dev * SldWH,
										  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
										  SldWH - (Sldh - gSldMenu_Info[num].pos - V_dev * (gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys - gSldMenu_Info[num].L1 - gSldMenu_Info[num].L2)),
										  gSldMenu_Info[num].flag);
					

				}
			}
			else
			{
				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, gSldMenu_Info[num].pos - gSldMenu_Info[num].L1,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, gSldMenu_Info[num].pos - gSldMenu_Info[num].L1,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);
				
				LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, 0, gSldMenu_Info[num].pos,
									  slideMune_BUFF, canvas_w, 0, gSldMenu_Info[num].pos,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, gSldMenu_Info[num].L1,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys - gSldMenu_Info[num].L2 - gSldMenu_Info[num].L1,gSldMenu_Info[num].flag);
			}
		}
		if (gSldMenu_Info[num].flag == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
								  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
								  Display_Layer, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
								  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
								  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys);

#if encoder_on
			if (gEncoderFlag == 1 && Check_box_flag == 0)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  LAY_BUFF8, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys);
			}
#endif
		}
		else if (gSldMenu_Info[num].flag == 1)
		{
			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
											 Display_Layer, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
											 Black, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys);

#if encoder_on
			if (gEncoderFlag == 1 && Check_box_flag == 0)
			{
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
												 LAY_BUFF8, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
												 Black, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys);
			}
#endif
		}
		else if (gSldMenu_Info[num].flag == 2)
		{
			if (TFT_bitcolor == 1)
			Memory_24bpp_Mode();
			BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
										   LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
										   Display_Layer, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
										   gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
										   gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys);
#if encoder_on
			if (gEncoderFlag == 1 && Check_box_flag == 0)
			{
				BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
											   LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
											   LAY_BUFF8, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
											   gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
											   gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys);
			}
#endif
		}
	}
	else if ((gSldMenu_Info[num].dir & 0x01) == 1) // left and right
	{
		if (TFT_bitcolor == 1 && gSldMenu_Info[num].flag == 2)
			Memory_16bpp_Mode();
		if (gSldMenu_Info[num].pos > Sldw)
			gSldMenu_Info[num].pos = Sldw;
		SldWH = (gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - gSldMenu_Info[num].L2 - gSldMenu_Info[num].L1);
		canvas_w = Get_Canvas_Width(Sldw);
		if (Screen_DIR == 1 || Screen_DIR == 2)
		{
			if ((Sldw - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH) < gSldMenu_Info[num].L1)
			{
				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, V_dev * SldWH, 0,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, V_dev * SldWH, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, gSldMenu_Info[num].L1 - (Sldw - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH), 0,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs + (Sldw - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH) - gSldMenu_Info[num].L1,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);

				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, Sldw - gSldMenu_Info[num].L1 + (Sldw - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH), 0,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, Sldw - gSldMenu_Info[num].L1 + (Sldw - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH), 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  0x0c, gSldMenu_Info[num].L1 - (Sldw - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH),
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);

				if (Sldw - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH >= 0)
				{
					LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, Sldw - SldWH - gSldMenu_Info[num].pos, 0,
										  slideMune_BUFF, canvas_w, Sldw - SldWH - gSldMenu_Info[num].pos, 0,
										  LAY_BUFF1, LCD_XSIZE_TFT, gSldMenu_Info[num].L1, 0,
										  0x0c, SldWH, gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);
				}
				else
				{
					LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, V_dev * SldWH, 0,
										  slideMune_BUFF, canvas_w, V_dev * SldWH, 0,
										  LAY_BUFF1, LCD_XSIZE_TFT, gSldMenu_Info[num].L1 - (Sldw - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH), 0,
										  0x0c, Sldw - gSldMenu_Info[num].pos - V_dev * SldWH,
										  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);

					LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, Sldw + (Sldw - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH), 0,
										  slideMune_BUFF, canvas_w, Sldw + (Sldw - SldWH - gSldMenu_Info[num].pos - V_dev * SldWH), 0,
										  LAY_BUFF1, LCD_XSIZE_TFT, gSldMenu_Info[num].L1, 0,
										  0x0c, SldWH - Sldw + gSldMenu_Info[num].pos + V_dev * SldWH,
										  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);
				}
			}
			else
			{
				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, Sldw - gSldMenu_Info[num].pos - SldWH - gSldMenu_Info[num].L1, 0,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, Sldw - gSldMenu_Info[num].pos - SldWH - gSldMenu_Info[num].L1, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);

				if (gSldMenu_Info[num].pos < gSldMenu_Info[num].L2)
				{
					LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, 0,
										  slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, 0,
										  LAY_BUFF1, LCD_XSIZE_TFT, gSldMenu_Info[num].pos + SldWH + gSldMenu_Info[num].L1, 0,
										  0x0c, gSldMenu_Info[num].L2 - gSldMenu_Info[num].pos,
										  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);
				}
				LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, Sldw - gSldMenu_Info[num].pos - SldWH, 0,
									  slideMune_BUFF, canvas_w, Sldw - gSldMenu_Info[num].pos - SldWH, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, gSldMenu_Info[num].L1, 0,
									  0x0c, SldWH, gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);
			}
		}
		else
		{
			if (gSldMenu_Info[num].pos < gSldMenu_Info[num].L1)
			{
				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, 0,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, gSldMenu_Info[num].L1 - gSldMenu_Info[num].pos, 0,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs + gSldMenu_Info[num].pos - gSldMenu_Info[num].L1,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);

				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, Sldw + gSldMenu_Info[num].pos - gSldMenu_Info[num].L1, 0,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, Sldw + gSldMenu_Info[num].pos - gSldMenu_Info[num].L1, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  0x0c, gSldMenu_Info[num].L1 - gSldMenu_Info[num].pos,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);

				LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, gSldMenu_Info[num].pos, 0,
									  slideMune_BUFF, canvas_w, gSldMenu_Info[num].pos, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, gSldMenu_Info[num].L1, 0,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - gSldMenu_Info[num].L2 - gSldMenu_Info[num].L1,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);
			}
			else if (gSldMenu_Info[num].pos + V_dev * (gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - gSldMenu_Info[num].L1 - gSldMenu_Info[num].L2) + gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - gSldMenu_Info[num].L1 > Sldw)
			{
				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, gSldMenu_Info[num].pos - gSldMenu_Info[num].L1, 0,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, gSldMenu_Info[num].pos - gSldMenu_Info[num].L1, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  0x0c, Sldw - gSldMenu_Info[num].pos + gSldMenu_Info[num].L1,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);

				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, 0,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, 0, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, Sldw - gSldMenu_Info[num].pos + gSldMenu_Info[num].L1 - V_dev * (gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - gSldMenu_Info[num].L1 - gSldMenu_Info[num].L2), 0,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - (Sldw - V_dev * (gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - gSldMenu_Info[num].L1 - gSldMenu_Info[num].L2) - gSldMenu_Info[num].pos + gSldMenu_Info[num].L1),
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);

				if (Sldw - gSldMenu_Info[num].pos - V_dev * (gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - gSldMenu_Info[num].L1 - gSldMenu_Info[num].L2) >
					gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - gSldMenu_Info[num].L2 - gSldMenu_Info[num].L1)
				{

					LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, gSldMenu_Info[num].pos, 0,
										  slideMune_BUFF, canvas_w, gSldMenu_Info[num].pos, 0,
										  LAY_BUFF1, LCD_XSIZE_TFT, gSldMenu_Info[num].L1, 0,
										  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - gSldMenu_Info[num].L2 - gSldMenu_Info[num].L1,
										  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);
				}
				else
				{
					LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, gSldMenu_Info[num].pos, 0,
										  slideMune_BUFF, canvas_w, gSldMenu_Info[num].pos, 0,
										  LAY_BUFF1, LCD_XSIZE_TFT, gSldMenu_Info[num].L1, 0,
										  0x0c, Sldw - gSldMenu_Info[num].pos - V_dev * (gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - gSldMenu_Info[num].L1 - gSldMenu_Info[num].L2),
										  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);

					LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, 0, 0,
										  slideMune_BUFF, canvas_w, 0, 0,
										  LAY_BUFF1, LCD_XSIZE_TFT, gSldMenu_Info[num].L1 + Sldw - gSldMenu_Info[num].pos - V_dev * (gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - gSldMenu_Info[num].L1 - gSldMenu_Info[num].L2), 0,
										  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - gSldMenu_Info[num].L2 - gSldMenu_Info[num].L1 - (Sldw - gSldMenu_Info[num].pos - V_dev * (gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - gSldMenu_Info[num].L1 - gSldMenu_Info[num].L2)),
										  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);
				}
			}
			else
			{
				LT768_BTE_Memory_Copy_1(slideMune_BUFF + Sld_Offset_Addr, canvas_w, gSldMenu_Info[num].pos - gSldMenu_Info[num].L1, 0,
									  slideMune_BUFF + Sld_Offset_Addr, canvas_w, gSldMenu_Info[num].pos - gSldMenu_Info[num].L1, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);

				LT768_BTE_Memory_Copy_1(slideMune_BUFF, canvas_w, gSldMenu_Info[num].pos, 0,
									  slideMune_BUFF, canvas_w, gSldMenu_Info[num].pos, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, gSldMenu_Info[num].L1, 0,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs - gSldMenu_Info[num].L2 - gSldMenu_Info[num].L1,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys,gSldMenu_Info[num].flag);
			}
		}

		if (gSldMenu_Info[num].flag == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
								  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
								  Display_Layer, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
								  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
								  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys);
#if encoder_on
			if (gEncoderFlag == 1 && Check_box_flag == 0)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
									  LAY_BUFF8, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
									  0x0c, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
									  gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys);
			}
#endif
		}
		else if (gSldMenu_Info[num].flag == 1)
		{
			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
											 Display_Layer, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
											 Black, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys);

#if encoder_on
			if (gEncoderFlag == 1 && Check_box_flag == 0)
			{
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
												 LAY_BUFF8, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
												 Black, gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys);
			}
#endif
		}
		else if (gSldMenu_Info[num].flag == 2)
		{
			if (TFT_bitcolor == 1)
				Memory_24bpp_Mode();
			BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
										   LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
										   Display_Layer, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
										   gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
										   gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys);
#if encoder_on
			if (gEncoderFlag == 1 && Check_box_flag == 0)
			{
				BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
											   LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
											   LAY_BUFF8, LCD_XSIZE_TFT, gSldMenu_Info[num].Xs, gSldMenu_Info[num].Ys,
											   gSldMenu_Info[num].Xe - gSldMenu_Info[num].Xs,
											   gSldMenu_Info[num].Ye - gSldMenu_Info[num].Ys);
			}
#endif
		}
	}
}

/***********************************************************************************
 * Function Name	: DL_menu
 * Description		: Get info of sliding menu.
 * Input			: - num: which number of sliding menu
 * Output       	: None
 * Return       	: None
 ************************************************************************************/
void DL_menu(uint8_t num)
{
	uint8_t buff[12] = {0};
	uint16_t canvas_w = 0;
	Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gSldMenu_Info[num].frontID, 12);
	pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
	Sldw = buff[4] + (buff[5] << 8);
	Sldh = buff[6] + (buff[7] << 8);
	pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
	pic_flag = buff[11];

	gSldMenu_Info[num].flag = pic_flag;
	

	if (pic_len != 0 && pic_flag != 0xFF)
	{
		canvas_w = Get_Canvas_Width(Sldw);
		Canvas_Image_Start_address(slideMune_BUFF);
		Canvas_image_width(canvas_w);
		LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
											Sldw, Sldh,
											Sldw, pic_add,
											slideMune_BUFF, canvas_w);
	}

	Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gSldMenu_Info[num].bgID, 12);
	pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
	Sldw = buff[4] + (buff[5] << 8);
	Sldh = buff[6] + (buff[7] << 8);
	pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
	pic_flag = buff[11];

	if(pic_flag == 2 || TFT_bitcolor == 0) 
		Sld_Offset_Addr =  Sldh * canvas_w * 2;
	else if(TFT_bitcolor == 1) 
		Sld_Offset_Addr =  Sldh * canvas_w * 3;

	if (pic_len != 0 && pic_flag != 0xFF)
	{
		Canvas_Image_Start_address(slideMune_BUFF + Sld_Offset_Addr);
		Canvas_image_width(canvas_w);
		LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
											Sldw, Sldh,
											Sldw, pic_add,
											slideMune_BUFF + Sld_Offset_Addr, canvas_w);
	}

}
/*******************************************************/

#define GESTURE_THRESHOLD 50	  // Gesture effective sliding threshold
uint8_t Gesture_press = 0;		  // Gesture effective press/release
uint8_t Gesture_direction = 0xFF; // Gesture direction
uint8_t Gesture_flag = 0;		  // Gesture function on/off

/***********************************************************************************
 * Function Name	: Gesture_touch
 * Description		: Touch detection of gesture.
 * Input			: None
 * Output       	: None
 * Return       	: None
 ************************************************************************************/

void Gesture_touch(void)
{
	uint8_t i = 0;
	uint8_t rebuf[20];
	int16_t Gesture_X, Gesture_Y;

//	if (gTpInfo.sta && Gesture_press == 0)
//		Gesture_press = 1; // press down

	if (gTpInfo.sta && Gesture_press == 0)
	{
		Gesture_press = 1;
		
		// sustain
		Gesture_X = gTpInfo.x[0] - First_pressX;
		Gesture_Y = gTpInfo.y[0] - First_pressY;
		if (Gesture_X >= 25 || Gesture_X <= -25 || Gesture_Y >= 25 || Gesture_Y <= -25)
		{
			Gesture_press = 1;
			// cancel
			Basci_flag = 0;
			Adj_flag = 0;
			Slide_flag = 0;
			Dio_flag = 0;
			AioKb_flag = 0;
			GBKioKb_flag = 0;
			Kb_flag = 0;
//			sldmenu_flag = 0;
			VarKey_flag = 0;
			Pop_up_flag = 0;
//			button_Press_flag = 1;
		}
	}
	if (gTpInfo.sta == 0 && Gesture_press == 1) // raise
	{
		Gesture_press = 0;
		Gesture_X = gTpInfo.x[0] - First_pressX;
		Gesture_Y = gTpInfo.y[0] - First_pressY;
		if (Gesture_X >= GESTURE_THRESHOLD ||
			Gesture_X <= -GESTURE_THRESHOLD ||
			Gesture_Y >= GESTURE_THRESHOLD ||
			Gesture_Y <= -GESTURE_THRESHOLD)
		{
			if (abs(Gesture_Y) > abs(Gesture_X))
			{
				if (Gesture_Y < 0)
					Gesture_direction = 0; // up
				else
					Gesture_direction = 1; // down
			}
			else
			{
				if (Gesture_X < 0)
					Gesture_direction = 2; // left
				else
					Gesture_direction = 3; // right
			}
		}

		if (Gesture_direction != 0xFF)
		{
			if (gGesture_Info.page[Gesture_direction] != 0xFFFF)
			{
				Gesture_flag = 0;
				if (gGesture_Info.Code == 0xC000)
				{
					rebuf[3] = 0x41;
					rebuf[4] = 0xFF;
					rebuf[5] = 0xFF;
					rebuf[6] = (gGesture_Info.value[Gesture_direction] >> 8) & 0xFF;
					rebuf[7] = gGesture_Info.value[Gesture_direction] & 0xFF;
					LT_SendData_CRC_Frame(rebuf, 5);
				}
				Display_page(gGesture_Info.page[Gesture_direction]);
			}

			Gesture_direction = 0xFF;
		}
	}
}

/*******************************************************/

RingSld_Info gRingSld_Info[RingSld_Size];
uint8_t RingSld_count = 0;
uint8_t RingSld_firsttouch = 0;
uint8_t RingSld_firstnum = 0;

/***********************************************************************************
 * Function Name	: DL_RingSld
 * Description		: calculation for ring progress bar.
 * Input			: - num: which number of ring progress bar
 * Output       	: None
 * Return       	: None
 ************************************************************************************/

void DL_RingSld(uint8_t num)
{
	uint8_t buff[12] = {0};
	uint16_t canvas_w1 = 0, canvas_w2 = 0;

	if (gRingSld_Info[num].frontID + 1 == gRingSld_Info[num].bgID) // Picture ID continuous
	{
		Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gRingSld_Info[num].frontID, 12);
		pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
		gRingSld_Info[num].w1 = buff[4] + (buff[5] << 8);
		gRingSld_Info[num].h1 = buff[6] + (buff[7] << 8);
		pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
		gRingSld_Info[num].rm1 = buff[11];

		if (pic_len != 0 && pic_flag != 0xFF)
		{
			canvas_w1 = Get_Canvas_Width(gRingSld_Info[num].w1);
			Canvas_Image_Start_address(LAY_BUFF9);
			Canvas_image_width(canvas_w1);

			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												gRingSld_Info[num].w1, gRingSld_Info[num].h1 * 2,
												gRingSld_Info[num].w1, pic_add,
												LAY_BUFF9, canvas_w1);
		}
	}
	else // Picture ID is discontinuous
	{
		Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gRingSld_Info[num].frontID, 12);
		pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
		gRingSld_Info[num].w1 = buff[4] + (buff[5] << 8);
		gRingSld_Info[num].h1 = buff[6] + (buff[7] << 8);
		pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
		gRingSld_Info[num].rm1 = buff[11];

		if (pic_len != 0 && gRingSld_Info[num].rm1 != 0xFF)
		{
			canvas_w1 = Get_Canvas_Width(gRingSld_Info[num].w1);
			Canvas_Image_Start_address(LAY_BUFF9);
			Canvas_image_width(canvas_w1);

			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												gRingSld_Info[num].w1, gRingSld_Info[num].h1,
												gRingSld_Info[num].w1, pic_add,
												LAY_BUFF9, canvas_w1);
		}

		if (gRingSld_Info[num].bgID != 0xFFFF)
		{
			Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gRingSld_Info[num].bgID, 12);
			pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
			pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
			gRingSld_Info[num].rm1 = buff[11];

			Canvas_Image_Start_address(LAY_BUFF9);
			Canvas_image_width(canvas_w1);

			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, gRingSld_Info[num].h1,
												gRingSld_Info[num].w1, gRingSld_Info[num].h1,
												gRingSld_Info[num].w1, pic_add,
												LAY_BUFF9, canvas_w1);
		}
	}
	gRingSld_Info[num].xR = gRingSld_Info[num].x + gRingSld_Info[num].w1 / 2;
	gRingSld_Info[num].yR = gRingSld_Info[num].y + gRingSld_Info[num].h1 / 2;

	if (gRingSld_Info[num].arrowid != 0xFFFF)
	{
		Flash_Read_UI(buff, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * gRingSld_Info[num].arrowid, 12);

		pic_add = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
		gRingSld_Info[num].w2 = buff[4] + (buff[5] << 8);
		gRingSld_Info[num].h2 = buff[6] + (buff[7] << 8);
		pic_len = buff[8] + (buff[9] << 8) + (buff[10] << 16);
		gRingSld_Info[num].rm2 = buff[11];

		if (pic_len != 0 && gRingSld_Info[num].rm2 != 0xFF)
		{
			canvas_w2 = Get_Canvas_Width(gRingSld_Info[num].w2);
			Canvas_Image_Start_address(LAY_BUFF9 + canvas_w1 * gRingSld_Info[num].h1 * 2 * 2);
			Canvas_image_width(canvas_w2);

			if (pic_len != 0 && gRingSld_Info[num].rm2 != 0xFF)
			{
				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
													gRingSld_Info[num].w2, gRingSld_Info[num].h2,
													gRingSld_Info[num].w2, pic_add,
													LAY_BUFF9 + canvas_w1 * gRingSld_Info[num].h1 * 4, canvas_w2);
			}
		}
	}
}

void RingSld_Display(uint8_t num, uint16_t angle)
{
	uint8_t n = 0, flag = 0, i = 0, kk = 0, Symbol = 0;
	uint8_t buff1[12] = {0};
	uint16_t canvas_w1 = 0, canvas_w2 = 0, num_w, dot_w, h, canvas_numw, canvas_dotw;
	uint16_t xd = 0, dx = 0;
	int16_t temp ,value1 = 0;
	int32_t value = 0, ll;
	uint16_t x_src = 0, y_src = 0, w_des, h_des, x_des, y_des;
	uint32_t zk_addr;

	canvas_w1 = Get_Canvas_Width(gRingSld_Info[num].w1);
	canvas_w2 = Get_Canvas_Width(gRingSld_Info[num].w2);

	if (gRingSld_Info[num].bgID == 0xFFFF)
	{
		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
							  MAIN_BUFF, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
							  LAY_BUFF1, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
							  0x0c, gRingSld_Info[num].w1, gRingSld_Info[num].h1);
	}

	if (gRingSld_Info[num].rm1 == 0)
	{
		if (gRingSld_Info[num].bgID != 0xFFFF)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF9, canvas_w1, 0, gRingSld_Info[num].h1,
								  LAY_BUFF9, canvas_w1, 0, gRingSld_Info[num].h1,
								  LAY_BUFF1, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
								  0x0c, gRingSld_Info[num].w1, gRingSld_Info[num].h1);
		}

		LT768_BTE_Memory_Copy(LAY_BUFF9, canvas_w1, 0, 0,
							  LAY_BUFF9, canvas_w1, 0, 0,
							  LAY_BUFF2, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
							  0x0c, gRingSld_Info[num].w1, gRingSld_Info[num].h1);
	}
	else if (gRingSld_Info[num].rm1 == 1)
	{
		if (gRingSld_Info[num].bgID != 0xFFFF)
		{
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
								  MAIN_BUFF, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
								  LAY_BUFF1, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
								  0x0c, gRingSld_Info[num].w1, gRingSld_Info[num].h1);

			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF9, canvas_w1, 0, gRingSld_Info[num].h1,
											 LAY_BUFF1, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
											 Black, gRingSld_Info[num].w1, gRingSld_Info[num].h1);
		}
		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
							  MAIN_BUFF, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
							  LAY_BUFF2, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
							  0x0c, gRingSld_Info[num].w1, gRingSld_Info[num].h1);

		LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF9, canvas_w1, 0, 0,
										 LAY_BUFF2, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
										 Black, gRingSld_Info[num].w1, gRingSld_Info[num].h1);
	}
	else if (gRingSld_Info[num].rm1 == 2)
	{

		if (gRingSld_Info[num].bgID != 0xFFFF)
		{
			BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
										   LAY_BUFF9, canvas_w1, 0, gRingSld_Info[num].h1,
										   LAY_BUFF1, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
										   gRingSld_Info[num].w1, gRingSld_Info[num].h1);
		}

		BTE_Pixel_16bpp_Alpha_Blending(MAIN_BUFF, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
									   LAY_BUFF9, canvas_w1, 0, 0,
									   LAY_BUFF2, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
									   gRingSld_Info[num].w1, gRingSld_Info[num].h1);
	}

	Canvas_Image_Start_address(LAY_BUFF2);
	Canvas_image_width(LCD_XSIZE_TFT);

	if (angle == 0)
	{
		LT768_DrawSquare_Fill(gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
							  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2,
							  gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);
	}
	else if (angle == 360)
		angle = -1;
	else
	{
		if (Screen_DIR == 1)
		{
			angle = angle + 90;
			angle = angle % 360;
		}
		else if (Screen_DIR == 2)
		{
			angle = angle + 180;
			angle = angle % 360;
		}
		if (Screen_DIR == 3)
		{
			angle = angle + 270;
			angle = angle % 360;
		}
	}

	if (angle >= 0 && angle < 45)
	{
		if (Screen_DIR == 0)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(angle);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
								  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);

			LT768_DrawPentagon_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR - temp, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(angle);
			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR - temp, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(angle);
			LT768_DrawPentagon_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR - temp, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(angle);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
								  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);
			
			LT768_DrawPentagon_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR - temp, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2, Black);
		}
		gRingSld_Info[num].x2 = gRingSld_Info[num].xR - gRingSld_Info[num].Sld_R * fast_sin(angle) - gRingSld_Info[num].w2 / 2;
		gRingSld_Info[num].y2 = gRingSld_Info[num].yR + gRingSld_Info[num].Sld_R * fast_cos(angle) - gRingSld_Info[num].h2 / 2;
	}
	else if (angle >= 45 && angle <= 90)
	{
		if (Screen_DIR == 0)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(90 - angle);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
								  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);

			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + temp,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(90 - angle);
			LT768_DrawTriangle_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + temp,
									gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(90 - angle);
			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + temp,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(90 - angle);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
								  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);

			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + temp,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2, Black);
		}
		gRingSld_Info[num].x2 = gRingSld_Info[num].xR - gRingSld_Info[num].Sld_R * fast_sin(angle) - gRingSld_Info[num].w2 / 2;
		gRingSld_Info[num].y2 = gRingSld_Info[num].yR + gRingSld_Info[num].Sld_R * fast_cos(angle) - gRingSld_Info[num].h2 / 2;
	}
	else if (angle > 90 && angle < 135)
	{
		if (Screen_DIR == 0)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(angle - 90);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
								  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);

			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - temp,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(angle - 90);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR,
								  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);

			LT768_DrawPentagon_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - temp,
									gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(angle - 90);
			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - temp,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(angle - 90);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
								  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);

			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - temp,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2, Black);
		}
		gRingSld_Info[num].x2 = gRingSld_Info[num].xR - gRingSld_Info[num].Sld_R * fast_sin(angle) - gRingSld_Info[num].w2 / 2;
		gRingSld_Info[num].y2 = gRingSld_Info[num].yR + gRingSld_Info[num].Sld_R * fast_cos(angle) - gRingSld_Info[num].h2 / 2;
	}
	else if (angle >= 135 && angle <= 180)
	{
		if (Screen_DIR == 0)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(180 - angle);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
								  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);

			LT768_DrawTriangle_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR - temp, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(180 - angle);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR,
								  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);

			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR - temp, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(180 - angle);
			LT768_DrawTriangle_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR - temp, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(180 - angle);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
								  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);

			LT768_DrawTriangle_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR - temp, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2, Black);
		}
		gRingSld_Info[num].x2 = gRingSld_Info[num].xR - gRingSld_Info[num].Sld_R * fast_sin(angle) - gRingSld_Info[num].w2 / 2;
		gRingSld_Info[num].y2 = gRingSld_Info[num].yR + gRingSld_Info[num].Sld_R * fast_cos(angle) - gRingSld_Info[num].h2 / 2;
	}
	else if (angle > 180 && angle < 225)
	{
		if (Screen_DIR == 0)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(angle - 180);
			LT768_DrawPentagon_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR + temp, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(angle - 180);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR,
								  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);

			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR + temp, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(angle - 180);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
								  gRingSld_Info[num].xR, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);

			LT768_DrawPentagon_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR + temp, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(angle - 180);
			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR + temp, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);
		}

		gRingSld_Info[num].x2 = gRingSld_Info[num].xR - gRingSld_Info[num].Sld_R * fast_sin(angle) - gRingSld_Info[num].w2 / 2;
		gRingSld_Info[num].y2 = gRingSld_Info[num].yR + gRingSld_Info[num].Sld_R * fast_cos(angle) - gRingSld_Info[num].h2 / 2;
	}
	else if (angle >= 225 && angle <= 270)
	{
		if (Screen_DIR == 0)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(270 - angle);
			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - temp,
										 gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(270 - angle);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR,
								  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);

			LT768_DrawTriangle_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - temp,
									gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(270 - angle);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
								  gRingSld_Info[num].xR, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);

			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - temp,
										 gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(270 - angle);
			LT768_DrawTriangle_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - temp,
									gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);
		}

		gRingSld_Info[num].x2 = gRingSld_Info[num].xR - gRingSld_Info[num].Sld_R * fast_sin(angle) - gRingSld_Info[num].w2 / 2;
		gRingSld_Info[num].y2 = gRingSld_Info[num].yR + gRingSld_Info[num].Sld_R * fast_cos(angle) - gRingSld_Info[num].h2 / 2;
	}
	else if (angle > 270 && angle < 315)
	{
		if (Screen_DIR == 0)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(angle - 270);
			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + temp,
										 gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(angle - 270);
			LT768_DrawPentagon_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + temp,
									gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(angle - 270);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
								  gRingSld_Info[num].xR, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);

			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + temp,
										 gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			temp = gRingSld_Info[num].w1 / 2 * fast_tan(angle - 270);
			LT768_DrawPentagon_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + temp,
									gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);

			LT768_DrawSquare_Fill(gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
								  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);
		}
		gRingSld_Info[num].x2 = gRingSld_Info[num].xR - gRingSld_Info[num].Sld_R * fast_sin(angle) - gRingSld_Info[num].w2 / 2;
		gRingSld_Info[num].y2 = gRingSld_Info[num].yR + gRingSld_Info[num].Sld_R * fast_cos(angle) - gRingSld_Info[num].h2 / 2;
	}
	else if (angle >= 315 && angle < 360)
	{
		if (Screen_DIR == 0)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(360 - angle);
			LT768_DrawTriangle_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR + temp, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 1)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(360 - angle);
			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR + temp, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);
		}
		else if (Screen_DIR == 2)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(360 - angle);
			LT768_DrawSquare_Fill(gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
								  gRingSld_Info[num].xR, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);

			LT768_DrawTriangle_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
									gRingSld_Info[num].xR + temp, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
									gRingSld_Info[num].xR, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2, Black);
		}
		else if (Screen_DIR == 3)
		{
			temp = gRingSld_Info[num].h1 / 2 * fast_tan(360 - angle);
			LT768_DrawQuadrilateral_Fill(gRingSld_Info[num].xR, gRingSld_Info[num].yR,
										 gRingSld_Info[num].xR + temp, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR + gRingSld_Info[num].h1 / 2,
										 gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2, Black);

			LT768_DrawSquare_Fill(gRingSld_Info[num].xR - gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR - gRingSld_Info[num].h1 / 2,
								  gRingSld_Info[num].xR + gRingSld_Info[num].w1 / 2, gRingSld_Info[num].yR, Black);
		}
		gRingSld_Info[num].x2 = gRingSld_Info[num].xR - gRingSld_Info[num].Sld_R * fast_sin(angle) - gRingSld_Info[num].w2 / 2;
		gRingSld_Info[num].y2 = gRingSld_Info[num].yR + gRingSld_Info[num].Sld_R * fast_cos(angle) - gRingSld_Info[num].h2 / 2;
	}

	LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF2, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
									 LAY_BUFF1, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
									 Black, gRingSld_Info[num].w1, gRingSld_Info[num].h1);

	if (gRingSld_Info[num].arrowid != 0xFFFF)
	{
		if (gRingSld_Info[num].rm2 == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF9 + canvas_w1 * gRingSld_Info[num].h1 * 2 * 2, canvas_w2, 0, 0,
								  LAY_BUFF9 + canvas_w1 * gRingSld_Info[num].h1 * 2 * 2, canvas_w2, 0, 0,
								  LAY_BUFF1, LCD_XSIZE_TFT, gRingSld_Info[num].x2, gRingSld_Info[num].y2,
								  0x0c, gRingSld_Info[num].w2, gRingSld_Info[num].h2);
		}
		else if (gRingSld_Info[num].rm2 == 1)
		{
			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF9 + canvas_w1 * gRingSld_Info[num].h1 * 2 * 2, canvas_w2, 0, 0,
											 LAY_BUFF1, LCD_XSIZE_TFT, gRingSld_Info[num].x2, gRingSld_Info[num].y2,
											 Black, gRingSld_Info[num].w2, gRingSld_Info[num].h2);
		}
		else if (gRingSld_Info[num].rm2 == 2)
		{

			BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, LCD_XSIZE_TFT, gRingSld_Info[num].x2, gRingSld_Info[num].y2,
										   LAY_BUFF9 + canvas_w1 * gRingSld_Info[num].h1 * 2 * 2, canvas_w2, 0, 0,
										   LAY_BUFF1, LCD_XSIZE_TFT, gRingSld_Info[num].x2, gRingSld_Info[num].y2,
										   gRingSld_Info[num].w2, gRingSld_Info[num].h2);
		}
	}

	if (gRingSld_Info[num].flag == 1)
	{
		memset(Display_buff, 0, 20);
		Flash_Read_UI(buff1, addr_index[2] + gRingSld_Info[num].zk_id * 8, 4);
		zk_addr = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
		value1 = (var[gRingSld_Info[num].p_add * 2] << 8) + var[gRingSld_Info[num].p_add * 2 + 1];
		ll = pow(10, gRingSld_Info[num].I_num + gRingSld_Info[num].D_num);
		value = value1 % ll;
		if (value1 < 0)
		{
			Display_buff[Symbol] = '-';
			Symbol = 1;
			value = ~value1 + 1;
		}
		ll = value;
		do
		{ // Get the number of digits
			n++;
			ll /= 10;
		} while (ll > 0);
		if (gRingSld_Info[num].D_num > 0) // With decimal point
		{
			if (gRingSld_Info[num].D_num >= n)
			{
				Display_buff[Symbol] = '0';
				Display_buff[Symbol + 1] = '.';
				for (i = n; gRingSld_Info[num].D_num > i; i++)
				{
					Display_buff[Symbol + 2 + i - n] = '0';
				}

				sprintf(&Display_buff[Symbol + 2 + gRingSld_Info[num].D_num - n], "%d", value);


			}
			else
			{
				sprintf(&Display_buff[Symbol], "%d", value);

				for (i = 0; i < gRingSld_Info[num].D_num; i++)
				{
					Display_buff[n + Symbol - i] = Display_buff[n - 1 + Symbol - i];
				}

				Display_buff[n + Symbol - gRingSld_Info[num].D_num] = '.';

			}
		}
		else
		{
			sprintf(&Display_buff[Symbol], "%d", value);
		}
		Flash_Read_UI(buff1, zk_addr, 4);
		LT_Print_zk_ASCII(zk_addr, LAY_BUFF1, LCD_XSIZE_TFT, buff1[2], buff1[3], gRingSld_Info[num].Alig, gRingSld_Info[num].color,
						  gRingSld_Info[num].dx, gRingSld_Info[num].dy, Display_buff);
	}
	else if (gRingSld_Info[num].flag == 2)
	{
		memset(Display_buff, 0, 20);
		value1 = (var[gRingSld_Info[num].p_add * 2] << 8) + var[gRingSld_Info[num].p_add * 2 + 1];

		if (value1 < 0)
		{
			Display_buff[kk] = 11;
			kk++;
			value = ~value1 + 1;
		}
		else
		{
			value = value1;
		}
		

		value = value % ((int)pow(10, gRingSld_Info[num].I_num + gRingSld_Info[num].D_num));

		for (i = 0; i < gRingSld_Info[num].I_num; i++) // All integers
		{
			Display_buff[kk] = value / (int)pow(10, gRingSld_Info[num].I_num - i - 1 + gRingSld_Info[num].D_num);

			if (Display_buff[kk] != 0 || flag == 1)
			{
				kk++;
				flag = 1; // The first place is not 0
			}
			value = value % (int)pow(10, gRingSld_Info[num].I_num - i - 1 + gRingSld_Info[num].D_num);
		}

		if (gRingSld_Info[num].D_num > 0)
		{
			if (flag == 0)
			{
				Display_buff[kk] = 0; //"."
				kk++;
			}
			Display_buff[kk] = 10; //"."
			kk++;
		}
		else
		{
			if (flag == 0)
			{
				Display_buff[kk] = 0; //"."
				kk++;
			}
		}
		for (i = 0; i < gRingSld_Info[num].D_num; i++) // Decimal
		{
			Display_buff[kk] = value / (int)pow(10, gRingSld_Info[num].D_num - i - 1);
			kk++;
			value = value % (int)pow(10, i - 1 + gRingSld_Info[num].D_num);
		}

		xd = 0;
		for (i = 0; i < kk; i++) // show
		{
			if (gRingSld_Info[num].s_id + Display_buff[i] >= addr_index[11])
				continue;
			Flash_Read_UI(buff1, addr_index[10] + multi_language * 12 * addr_index[11] + 12 * (gRingSld_Info[num].s_id + Display_buff[i]), 12);
			pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
			num_w = buff1[4] + (buff1[5] << 8);
			h = buff1[6] + (buff1[7] << 8);
			pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
			pic_flag = buff1[11];

			if (pic_len != 0 && pic_flag != 0xFF)
			{
				if (Screen_DIR == 0)
				{
					x_src = xd;
					y_src = 0;
					w_des = num_w;
				}
				else if (Screen_DIR == 1)
				{
					x_src = LCD_XSIZE_TFT - num_w;
					y_src = xd;
					w_des = h;
				}
				else if (Screen_DIR == 2)
				{
					x_src = LCD_XSIZE_TFT - num_w - xd;
					y_src = LCD_YSIZE_TFT - h;
					w_des = num_w;
				}
				else if (Screen_DIR == 3)
				{
					x_src = 0;
					y_src = LCD_YSIZE_TFT - h - xd;
					w_des = h;
				}

				LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, x_src, y_src,
													num_w, h,
													num_w, pic_add,
													LAY_BUFF2, LCD_XSIZE_TFT);
				xd += w_des;
			}
		}

		if (Screen_DIR == 0)
		{
			x_src = 0;
			y_src = 0;
			w_des = xd;
			h_des = h;
			if (gRingSld_Info[num].Alig == 0)		// Left alignment
			{
				x_des = gRingSld_Info[num].dx;
				y_des = gRingSld_Info[num].dy;
			}
			else if (gRingSld_Info[num].Alig == 1)	// Mid alignment
			{
				x_des = gRingSld_Info[num].dx - w_des / 2;
				y_des = gRingSld_Info[num].dy;
			}
			else if (gRingSld_Info[num].Alig == 2)	// Right alignment
			{
				x_des = gRingSld_Info[num].dx - xd;
				y_des = gRingSld_Info[num].dy;
			}
		}
		else if (Screen_DIR == 1)
		{
			x_src = LCD_XSIZE_TFT - num_w;
			y_src = 0;
			w_des = num_w;
			h_des = xd;
			if (gRingSld_Info[num].Alig == 0) // Left alignment
			{
				x_des = gRingSld_Info[num].dx - w_des + 1;
				y_des = gRingSld_Info[num].dy;
			}
			else if (gRingSld_Info[num].Alig == 1) // Mid alignment
			{
				x_des = gRingSld_Info[num].dx - w_des + 1;
				y_des = gRingSld_Info[num].dy - h_des / 2 + 1;
			}
			else if (gRingSld_Info[num].Alig == 2) // Right alignment
			{
				x_des = gRingSld_Info[num].dx - w_des + 1;
				y_des = gRingSld_Info[num].dy - h_des + 1;
			}
		}
		else if (Screen_DIR == 2)
		{
			x_src = LCD_XSIZE_TFT - xd;
			y_src = LCD_YSIZE_TFT - h;
			w_des = xd;
			h_des = h;
			if (gRingSld_Info[num].Alig == 0) // Left alignment
			{
				x_des = gRingSld_Info[num].dx - w_des + 1;
				y_des = gRingSld_Info[num].dy - h_des + 1;
			}
			else if (gRingSld_Info[num].Alig == 1) // Mid alignment
			{
				x_des = gRingSld_Info[num].dx - w_des / 2 + 1;
				y_des = gRingSld_Info[num].dy - h_des + 1;
			}
			else if (gRingSld_Info[num].Alig == 2) // Right alignment
			{
				x_des = gRingSld_Info[num].dx;
				y_des = gRingSld_Info[num].dy - h_des + 1;
			}
		}
		else if (Screen_DIR == 3)
		{
			x_src = 0;
			y_src = LCD_YSIZE_TFT - xd;
			w_des = num_w;
			h_des = xd;
			if (gRingSld_Info[num].Alig == 0) // Left alignment
			{
				x_des = gRingSld_Info[num].dx;
				y_des = gRingSld_Info[num].dy - xd;
			}
			else if (gRingSld_Info[num].Alig == 1) // Mid alignment
			{
				x_des = gRingSld_Info[num].dx;
				y_des = gRingSld_Info[num].dy - xd / 2;
			}
			else if (gRingSld_Info[num].Alig == 2) // Right alignment
			{
				x_des = gRingSld_Info[num].dx;
				y_des = gRingSld_Info[num].dy;
			}
		}

		if (pic_flag == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
								  LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
								  LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
								  0x0c, w_des, h_des);
		}
		else if (pic_flag == 1)
		{

			LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
											 LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
											 Black, w_des, h_des);
		}
		else if (pic_flag == 2)
		{
			BTE_Pixel_16bpp_Alpha_Blending(LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
										   LAY_BUFF2, LCD_XSIZE_TFT, x_src, y_src,
										   LAY_BUFF1, LCD_XSIZE_TFT, x_des, y_des,
										   w_des, h_des);
		}
	}

	LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
						  LAY_BUFF1, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
						  Display_Layer, LCD_XSIZE_TFT, gRingSld_Info[num].x, gRingSld_Info[num].y,
						  0x0c, gRingSld_Info[num].w1, gRingSld_Info[num].h1);
}

/***********************************************************************************
 * Function Name	: RingSld_touch
 * Description		: Touch detection of ring progress bar.
 * Input			: None
 * Output       	: None
 * Return       	: None
 ************************************************************************************/
	void RingSld_touch(void)
{
	uint8_t i;
	uint8_t rebuf[20];
	uint16_t angle = 0, j = 0, V = 0;
	int32_t x1, y1, x2, y2;
	float Xa, Ya, Xb, Yb, Xc, Yc = 0;

	if (gTpInfo.sta)
	{
		for (i = 0; i < RingSld_count; i++)
		{
			if (lastx != gTpInfo.x[0] || lasty != gTpInfo.y[0])
			{
				x1 = gTpInfo.x[0] - gRingSld_Info[i].xR;
				y1 = gTpInfo.y[0] - gRingSld_Info[i].yR;

				if (x1 * x1 + y1 * y1 >= (gRingSld_Info[i].Sld_R - gRingSld_Info[i].tp_R) * (gRingSld_Info[i].Sld_R - gRingSld_Info[i].tp_R) &&
					x1 * x1 + y1 * y1 <= (gRingSld_Info[i].Sld_R + gRingSld_Info[i].tp_R) * (gRingSld_Info[i].Sld_R + gRingSld_Info[i].tp_R) &&
					((RingSld_firsttouch == 0 && button_Press_flag == 0) || (RingSld_firsttouch == 1 && RingSld_firstnum == i)))
				{
					lastx = gTpInfo.x[0];
					lasty = gTpInfo.y[0];
					if (RingSld_firsttouch == 0 || RingSld_firstnum != i)
					{
						if (buzzer)
							touch_buzzer();
						button_Press_flag = 1;
						RingSld_firsttouch = 1;
						RingSld_firstnum = i;
						DL_RingSld(i);
					}
					// 180 to 270 degrees
					if (gTpInfo.x[0] >= gRingSld_Info[i].xR &&
						gTpInfo.x[0] <= gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R + gRingSld_Info[i].tp_R &&
						gTpInfo.y[0] >= gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R - gRingSld_Info[i].tp_R &&
						gTpInfo.y[0] <= gRingSld_Info[i].yR)
					{
						if (gTpInfo.x[0] == (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(0)) ||
							gTpInfo.x[0] - 1 == (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(0)))
						{
							angle = 180;
						}
						else if (gTpInfo.y[0] == (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(90)) ||
								 gTpInfo.y[0] + 1 == (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(90)))
						{
							angle = 270;
						}
						else
						{
							for (j = 1; j < 90; j++)
							{
								Xa = gTpInfo.x[0] - (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(j - 1));
								Ya = gTpInfo.y[0] - (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(j - 1));
								Xb = gTpInfo.x[0] - (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(j));
								Yb = gTpInfo.y[0] - (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(j));
								Xc = gTpInfo.x[0] - (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(j + 1));
								Yc = gTpInfo.y[0] - (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(j + 1));
								if (Xb * Xb + Yb * Yb <= Xa * Xa + Ya * Ya && Xb * Xb + Yb * Yb <= Xc * Xc + Yc * Yc)
								{
									angle = j + 180;
									break;
								}
							}
						}
					}
					// 270 to 360 degrees
					else if (gTpInfo.x[0] >= gRingSld_Info[i].xR && gTpInfo.x[0] <= gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R + gRingSld_Info[i].tp_R &&
							 gTpInfo.y[0] >= gRingSld_Info[i].yR && gTpInfo.y[0] <= gRingSld_Info[i].yR + gRingSld_Info[i].Sld_R + gRingSld_Info[i].tp_R)
					{

						if (gTpInfo.x[0] == (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(180)))
						{
							angle = 0;
						}
						else if (gTpInfo.y[0] == (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(90)) ||
								 gTpInfo.y[0] - 1 == (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(90)))
						{
							angle = 270;
						}
						else
						{
							for (j = 91; j < 180; j++)
							{
								Xa = gTpInfo.x[0] - (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(j - 1));
								Ya = gTpInfo.y[0] - (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(j - 1));
								Xb = gTpInfo.x[0] - (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(j));
								Yb = gTpInfo.y[0] - (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(j));
								Xc = gTpInfo.x[0] - (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(j + 1));
								Yc = gTpInfo.y[0] - (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(j + 1));
								if (Xb * Xb + Yb * Yb <= Xa * Xa + Ya * Ya && Xb * Xb + Yb * Yb <= Xc * Xc + Yc * Yc)
								{
									angle = j + 180;
									break;
								}
							}
						}
					}
					// 0 to 90 degrees
					else if (gTpInfo.x[0] >= gRingSld_Info[i].xR - gRingSld_Info[i].Sld_R - gRingSld_Info[i].tp_R && gTpInfo.x[0] <= gRingSld_Info[i].xR &&
							 gTpInfo.y[0] >= gRingSld_Info[i].yR && gTpInfo.y[0] <= gRingSld_Info[i].yR + gRingSld_Info[i].Sld_R + gRingSld_Info[i].tp_R)
					{
						if (gTpInfo.x[0] == (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(180)))
						{
							angle = 0;
						}
						else if (gTpInfo.y[0] == (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(270)) ||
								 gTpInfo.y[0] - 1 == (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(270)))
						{
							angle = 90;
						}
						else
						{
							for (j = 181; j < 270; j++)
							{
								Xa = gTpInfo.x[0] - (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(j - 1));
								Ya = gTpInfo.y[0] - (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(j - 1));
								Xb = gTpInfo.x[0] - (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(j));
								Yb = gTpInfo.y[0] - (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(j));
								Xc = gTpInfo.x[0] - (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(j + 1));
								Yc = gTpInfo.y[0] - (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(j + 1));
								if (Xb * Xb + Yb * Yb <= Xa * Xa + Ya * Ya && Xb * Xb + Yb * Yb <= Xc * Xc + Yc * Yc)
								{
									angle = j - 180;
									break;
								}
							}
						}
					}
					// 90 to 180 degrees
					else if (gTpInfo.x[0] >= gRingSld_Info[i].xR - gRingSld_Info[i].Sld_R - gRingSld_Info[i].tp_R && gTpInfo.x[0] <= gRingSld_Info[i].xR && gTpInfo.y[0] >= gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R - gRingSld_Info[i].tp_R && gTpInfo.y[0] <= gRingSld_Info[i].yR)
					{
						if (gTpInfo.x[0] == (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(0)) ||
							gTpInfo.x[0] + 1 == (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(0)))
						{
							angle = 180;
						}
						else if (gTpInfo.y[0] == (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(270)) ||
								 gTpInfo.y[0] + 1 == (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(270)))
						{
							angle = 90;
						}
						else
						{
							for (j = 271; j < 360; j++)
							{
								Xa = gTpInfo.x[0] - (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(j - 1));
								Ya = gTpInfo.y[0] - (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(j - 1));
								Xb = gTpInfo.x[0] - (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(j));
								Yb = gTpInfo.y[0] - (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(j));
								Xc = gTpInfo.x[0] - (gRingSld_Info[i].xR + gRingSld_Info[i].Sld_R * fast_sin(j + 1));
								Yc = gTpInfo.y[0] - (gRingSld_Info[i].yR - gRingSld_Info[i].Sld_R * fast_cos(j + 1));
								if (Xb * Xb + Yb * Yb <= Xa * Xa + Ya * Ya && Xb * Xb + Yb * Yb <= Xc * Xc + Yc * Yc)
								{
									angle = j - 180;
									break;
								}
							}
						}
					}

					if (Screen_DIR == 1)
					{
						angle = angle + 270;
						angle = angle % 360;
					}
					else if (Screen_DIR == 2)
					{
						angle = angle + 180;
						angle = angle % 360;
					}
					else if (Screen_DIR == 3)
					{
						angle = angle + 90;
						angle = angle % 360;
					}

					if (angle <= gRingSld_Info[i].Eangle && angle >= gRingSld_Info[i].Sangle)
					{

						V = (gRingSld_Info[i].Vmax - gRingSld_Info[i].Vmin) * (angle - gRingSld_Info[i].Sangle) / (gRingSld_Info[i].Eangle - gRingSld_Info[i].Sangle) + gRingSld_Info[i].Vmin;

						var[gRingSld_Info[i].p_add * 2] = (V >> 8) & 0xFF;
						var[gRingSld_Info[i].p_add * 2 + 1] = V & 0xFF;
						RingSld_Display(i, angle);

						if (gRingSld_Info[i].p_add >= 0x7000)
							reg_operation(gRingSld_Info[i].p_add);
						page_date_update(pic_id);
					}
					else if (angle > gRingSld_Info[i].Eangle && angle < gRingSld_Info[i].Eangle + 10)
					{
						angle = gRingSld_Info[i].Eangle;
						V = (gRingSld_Info[i].Vmax - gRingSld_Info[i].Vmin) * (angle - gRingSld_Info[i].Sangle) / (gRingSld_Info[i].Eangle - gRingSld_Info[i].Sangle) + gRingSld_Info[i].Vmin;

						var[gRingSld_Info[i].p_add * 2] = (V >> 8) & 0xFF;
						var[gRingSld_Info[i].p_add * 2 + 1] = V & 0xFF;
						RingSld_Display(i, angle);

						if (gRingSld_Info[i].p_add >= 0x7000)
							reg_operation(gRingSld_Info[i].p_add);
						page_date_update(pic_id);
					}
					else if (angle < gRingSld_Info[i].Sangle && angle + 10 > gRingSld_Info[i].Sangle)
					{
						angle = gRingSld_Info[i].Sangle;
						V = (gRingSld_Info[i].Vmax - gRingSld_Info[i].Vmin) * (angle - gRingSld_Info[i].Sangle) / (gRingSld_Info[i].Eangle - gRingSld_Info[i].Sangle) + gRingSld_Info[i].Vmin;

						var[gRingSld_Info[i].p_add * 2] = (V >> 8) & 0xFF;
						var[gRingSld_Info[i].p_add * 2 + 1] = V & 0xFF;
						RingSld_Display(i, angle);

						if (gRingSld_Info[i].p_add >= 0x7000)
							reg_operation(gRingSld_Info[i].p_add);
						page_date_update(pic_id);
					}
				}
			}
		}
	}

	else if (RingSld_firsttouch == 1)
	{
		RingSld_firsttouch = 0;
		button_Press_flag = 0;
		if (gRingSld_Info[RingSld_firstnum].Code == 0xC007)
		{
			rebuf[3] = 0x41;
			rebuf[4] = gRingSld_Info[RingSld_firstnum].p_add >> 8;
			rebuf[5] = gRingSld_Info[RingSld_firstnum].p_add & 0xFF;
			rebuf[6] = var[gRingSld_Info[RingSld_firstnum].p_add * 2];
			rebuf[7] = var[gRingSld_Info[RingSld_firstnum].p_add * 2 + 1];
			LT_SendData_CRC_Frame(rebuf, 5);
		}
	}
}

/******************************************************/

/***********************************************************************************
 * Function Name	: Ascii_input
 * Description		: Touch detection of ASCII keyboard.
 * Input			: None
 * Output       	: None
 * Return       	: None
 ************************************************************************************/
void Ascii_input(void)
{
	uint8_t res = 0, i = 0, width_Hor;
	uint8_t rebuf[50], buff1[12] = {0};
	uint16_t canvas_w = 0, k = 0;
	uint16_t w_temp, h_temp;
	uint16_t dx, dy, dxs, dxe, dys, dye;
	uint32_t zk_addr = 0;
	uint32_t color;

	if (gTpInfo.sta && AioKb_flag == 0) // press down
	{
		for (i = 0; i < Aio_count; i++)
		{
			if (gTpInfo.x[0] >= gAio_Info[i].Xs && gTpInfo.y[0] >= gAio_Info[i].Ys &&
				gTpInfo.x[0] <= gAio_Info[i].Xe && gTpInfo.y[0] <= gAio_Info[i].Ye &&
				Aio_flag == 0 && button_Press_flag == 0)
			{
				button_Press_flag = 1;
				Aio_num = i;
				Aio_flag = 1;
				if (buzzer)
					touch_buzzer();
			}
		}
	}
	if (Button_Key_Code_Flag == 1)
	{
		for (i = 0; i < Aio_count; i++)
		{
			if (gAio_Info[i].Xs == 0xFFFF && gAio_Info[i].Ys ==  Button_Key_Code)
			{
				Button_Key_Code_Flag = 0;
				Button_Key_Code = 0;
				Aio_num = i;
				Aio_flag = 1;
				if (Pop_up_flag == 1) // pop-up window
				{
					LT768_BTE_Memory_Copy(Display_Layer, LCD_XSIZE_TFT, 0, 0,
										  Display_Layer, LCD_XSIZE_TFT, 0, 0,
										  LAY_BUFF7, LCD_XSIZE_TFT, 0, 0,
										  0x0c, LCD_XSIZE_TFT, LCD_YSIZE_TFT);
				}
				Flash_Read_UI(buff1, addr_index[6] + 12 * gAio_Info[Aio_num].kb_id, 12);
				pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
				pic_w = buff1[4] + (buff1[5] << 8);
				pic_h = buff1[6] + (buff1[7] << 8);
				pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
				pic_flag = buff1[11];

				if (pic_len != 0 && pic_flag != 0xFF)
				{
					canvas_w = Get_Canvas_Width(pic_w);
					Canvas_Image_Start_address(LAY_BUFF1);
					Canvas_image_width(canvas_w);

					LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
														pic_w, pic_h,
														pic_w, pic_add,
														LAY_BUFF1, canvas_w);

					if (pic_flag == 0)
					{
						LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
											  LAY_BUFF1, canvas_w, 0, 0,
											  MAIN_BUFF, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
											  0x0c, pic_w, pic_h);

						LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
											  LAY_BUFF1, canvas_w, 0, 0,
											  LAY_BUFF2, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
											  0x0c, pic_w, pic_h);
					}
					else if (pic_flag == 1)
					{
						LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
														 MAIN_BUFF, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
														 Black, pic_w, pic_h);

						LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
														 LAY_BUFF2, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
														 Black, pic_w, pic_h);
					}
					else if (pic_flag == 2)
					{
						BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
													   LAY_BUFF1, canvas_w, 0, 0,
													   MAIN_BUFF, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
													   pic_w, pic_h);

						BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
													   LAY_BUFF1, canvas_w, 0, 0,
													   LAY_BUFF2, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
													   pic_w, pic_h);
					}
				}

				/*Dynamic picture coverage judgment*/
				AioKb_flag = 1;
				gAio_Info[Aio_num].Xs = gAio_Info[Aio_num].kb_x;
				gAio_Info[Aio_num].Ys = gAio_Info[Aio_num].kb_y;
				gAio_Info[Aio_num].Xe = gAio_Info[Aio_num].kb_x + pic_w;
				gAio_Info[Aio_num].Ye = gAio_Info[Aio_num].kb_y + pic_h;

				Flash_Read_UI(buff1, addr_index[2] + gAio_Info[Aio_num].zk_id * 8, 4);
				gAio_Info[Aio_num].zk_addr = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
				cur_x = gAio_Info[Aio_num].dp_x;
				cur_y = gAio_Info[Aio_num].dp_y;
				buff1[0] = '|';
				buff1[1] = 0;

				if (gAio_Info[Aio_num].cursor_color == 0)
					color = Black;
				else
					color = White;

				Flash_Read_UI(&res, gAio_Info[Aio_num].zk_addr + 9 + 92, 1);
				if (Screen_DIR == 0)
				{
					dx = cur_x;
					dy = cur_y;
					w_temp = res;
					h_temp = gAio_Info[Aio_num].zk_h;
				}
				else if (Screen_DIR == 1)
				{
					dx = cur_x;
					dy = cur_y;
					cur_x = cur_x - gAio_Info[Aio_num].zk_h + 1;
					cur_y = cur_y;
					w_temp = gAio_Info[Aio_num].zk_h;
					h_temp = res;
				}
				else if (Screen_DIR == 2)
				{
					dx = cur_x;
					dy = cur_y;
					cur_x = cur_x - res + 1;
					cur_y = cur_y - gAio_Info[Aio_num].zk_h + 1;
					w_temp = res;
					h_temp = gAio_Info[Aio_num].zk_h;
				}
				else if (Screen_DIR == 3)
				{
					dx = cur_x;
					dy = cur_y;
					w_temp = gAio_Info[Aio_num].zk_h;
					h_temp = res;
				}
				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, cur_x, cur_y,
									  MAIN_BUFF, LCD_XSIZE_TFT, cur_x, cur_y,
									  LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
									  0x0c, w_temp, h_temp);

				LT_Print_zk_ASCII(gAio_Info[Aio_num].zk_addr, LAY_BUFF1, LCD_XSIZE_TFT,
								  gAio_Info[Aio_num].zk_w, gAio_Info[Aio_num].zk_h, 0, color,
								  dx, dy, buff1);

				cur_w = data_w - 2;
				if (Screen_DIR == 0)
				{
					w_temp = cur_w;
					h_temp = gAio_Info[Aio_num].zk_h;
				}
				else if (Screen_DIR == 1)
				{
					w_temp = gAio_Info[Aio_num].zk_h;
					h_temp = cur_w;
				}
				else if (Screen_DIR == 3)
				{
					cur_y = cur_y - cur_w + 1;
					w_temp = gAio_Info[Aio_num].zk_h;
					h_temp = cur_w;
				}
				LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
									  LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
									  LAY_BUFF5, LCD_XSIZE_TFT, cur_x, cur_y,
									  0x0c, w_temp, h_temp);

				Get_Touch_info(gAio_Info[Aio_num].kb_id);

				button_Press_flag = 0;
				Aio_flag = 0;
				input_len = 0;
				CapsLock = 0;
				input_w = 0;
				memset(Display_buff, 0, 400);

				pic_id_temp = pic_id;
				pic_id = gAio_Info[Aio_num].kb_id;

				if (gAio_Info[Aio_num].scan_mode == 1) // Display ASCII data on keyboard
				{
					for (k = 0; k < gAio_Info[Aio_num].p_len * 2; k++)
					{
						if (var[gAio_Info[Aio_num].p_add * 2 + k] > 0x7F)
							break;
						Display_buff[k] = var[gAio_Info[Aio_num].p_add * 2 + k];
						if (var[gAio_Info[Aio_num].p_add * 2 + k] == 0x00 && var[gAio_Info[Aio_num].p_add * 2 + k + 1] == 0x00)
							break;
					}
					input_len = k;

					if (Screen_DIR == 0)
					{
						dxs = gAio_Info[Aio_num].dp_x;
						dys = gAio_Info[Aio_num].dp_y;
						dxe = LCD_XSIZE_TFT;
						dye = gAio_Info[Aio_num].dp_y + gAio_Info[Aio_num].zk_h;
					}
					else if (Screen_DIR == 1)
					{
						dxs = gAio_Info[Aio_num].dp_x - gAio_Info[Aio_num].zk_h;
						dys = gAio_Info[Aio_num].dp_y;
						dxe = gAio_Info[Aio_num].dp_x;
						dye = LCD_YSIZE_TFT;
					}
					else if (Screen_DIR == 2)
					{
						dxs = 0;
						dys = gAio_Info[Aio_num].dp_y - gAio_Info[Aio_num].zk_h;
						dxe = gAio_Info[Aio_num].dp_x;
						dye = gAio_Info[Aio_num].dp_y;
					}
					else if (Screen_DIR == 3)
					{
						dxs = gAio_Info[Aio_num].dp_x;
						dys = 0;
						dxe = gAio_Info[Aio_num].dp_x + gAio_Info[Aio_num].zk_h;
						dye = gAio_Info[Aio_num].dp_y;
					}

					LT_Print_zk_Font(4, gAio_Info[Aio_num].zk_addr, LAY_BUFF2, LCD_XSIZE_TFT,
									 gAio_Info[Aio_num].zk_w, gAio_Info[Aio_num].zk_h,
									 3, gAio_Info[Aio_num].fcolor, gAio_Info[Aio_num].input_mode,
									 dxs, dys, dxe, dye, 0, 0, Display_buff, input_len);

					LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
										  LAY_BUFF2, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
										  Display_Layer, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
										  0x0c, pic_w, pic_h);
					input_w = data_w;
				}
				else
				{
					LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
										  LAY_BUFF2, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
										  Display_Layer, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
										  0x0c, pic_w, pic_h);
				}

				Cusor_time = 0;
			}
		}
	}
	if (gTpInfo.sta == 0 && Aio_flag == 1 && AioKb_flag == 0)
	{
		if (Pop_up_flag == 1) // pop-up window
		{
			LT768_BTE_Memory_Copy(Display_Layer, LCD_XSIZE_TFT, 0, 0,
								  Display_Layer, LCD_XSIZE_TFT, 0, 0,
								  LAY_BUFF7, LCD_XSIZE_TFT, 0, 0,
								  0x0c, LCD_XSIZE_TFT, LCD_YSIZE_TFT);
		}
		Flash_Read_UI(buff1, addr_index[6] + 12 * gAio_Info[Aio_num].kb_id, 12);
		pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
		pic_w = buff1[4] + (buff1[5] << 8);
		pic_h = buff1[6] + (buff1[7] << 8);
		pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
		pic_flag = buff1[11];

		if (pic_len != 0 && pic_flag != 0xFF)
		{
			canvas_w = Get_Canvas_Width(pic_w);
			Canvas_Image_Start_address(LAY_BUFF1);
			Canvas_image_width(canvas_w);

			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												pic_w, pic_h,
												pic_w, pic_add,
												LAY_BUFF1, canvas_w);

			if (pic_flag == 0)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF1, canvas_w, 0, 0,
									  MAIN_BUFF, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
									  0x0c, pic_w, pic_h);

				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF2, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
									  0x0c, pic_w, pic_h);
			}
			else if (pic_flag == 1)
			{
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
												 MAIN_BUFF, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
												 Black, pic_w, pic_h);

				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
												 LAY_BUFF2, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
												 Black, pic_w, pic_h);
			}
			else if (pic_flag == 2)
			{
				BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   MAIN_BUFF, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
											   pic_w, pic_h);

				BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF2, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
											   pic_w, pic_h);
			}
		}

		/*Dynamic picture coverage judgment*/
		AioKb_flag = 1;
		gAio_Info[Aio_num].Xs = gAio_Info[Aio_num].kb_x;
		gAio_Info[Aio_num].Ys = gAio_Info[Aio_num].kb_y;
		gAio_Info[Aio_num].Xe = gAio_Info[Aio_num].kb_x + pic_w;
		gAio_Info[Aio_num].Ye = gAio_Info[Aio_num].kb_y + pic_h;

		Flash_Read_UI(buff1, addr_index[2] + gAio_Info[Aio_num].zk_id * 8, 4);
		gAio_Info[Aio_num].zk_addr = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
		cur_x = gAio_Info[Aio_num].dp_x;
		cur_y = gAio_Info[Aio_num].dp_y;
		buff1[0] = '|';
		buff1[1] = 0;

		if (gAio_Info[Aio_num].cursor_color == 0)
			color = Black;
		else
			color = White;

		Flash_Read_UI(&res, gAio_Info[Aio_num].zk_addr + 9 + 92, 1);
		if (Screen_DIR == 0)
		{
			dx = cur_x;
			dy = cur_y;
			w_temp = res;
			h_temp = gAio_Info[Aio_num].zk_h;
		}
		else if (Screen_DIR == 1)
		{
			dx = cur_x;
			dy = cur_y;
			cur_x = cur_x - gAio_Info[Aio_num].zk_h + 1;
			cur_y = cur_y;
			w_temp = gAio_Info[Aio_num].zk_h;
			h_temp = res;
		}
		else if (Screen_DIR == 2)
		{
			dx = cur_x;
			dy = cur_y;
			cur_x = cur_x - res + 1;
			cur_y = cur_y - gAio_Info[Aio_num].zk_h + 1;
			w_temp = res;
			h_temp = gAio_Info[Aio_num].zk_h;
		}
		else if (Screen_DIR == 3)
		{
			dx = cur_x;
			dy = cur_y;
			w_temp = gAio_Info[Aio_num].zk_h;
			h_temp = res;
		}
		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, cur_x, cur_y,
							  MAIN_BUFF, LCD_XSIZE_TFT, cur_x, cur_y,
							  LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
							  0x0c, w_temp, h_temp);

		LT_Print_zk_ASCII(gAio_Info[Aio_num].zk_addr, LAY_BUFF1, LCD_XSIZE_TFT,
						  gAio_Info[Aio_num].zk_w, gAio_Info[Aio_num].zk_h, 0, color,
						  dx, dy, buff1);

		cur_w = data_w - 2;
		if (Screen_DIR == 0)
		{
			w_temp = cur_w;
			h_temp = gAio_Info[Aio_num].zk_h;
		}
		else if (Screen_DIR == 1)
		{
			w_temp = gAio_Info[Aio_num].zk_h;
			h_temp = cur_w;
		}
		else if (Screen_DIR == 3)
		{
			cur_y = cur_y - cur_w + 1;
			w_temp = gAio_Info[Aio_num].zk_h;
			h_temp = cur_w;
		}
		LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
							  LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
							  LAY_BUFF5, LCD_XSIZE_TFT, cur_x, cur_y,
							  0x0c, w_temp, h_temp);

		Get_Touch_info(gAio_Info[Aio_num].kb_id);

		button_Press_flag = 0;
		Aio_flag = 0;
		input_len = 0;
		CapsLock = 0;
		input_w = 0;
		memset(Display_buff, 0, 400);

		pic_id_temp = pic_id;
		pic_id = gAio_Info[Aio_num].kb_id;

		if (gAio_Info[Aio_num].scan_mode == 1) // Display ASCII data on keyboard
		{
			for (k = 0; k < gAio_Info[Aio_num].p_len * 2; k++)
			{
				if (var[gAio_Info[Aio_num].p_add * 2 + k] > 0x7F)
					break;
				Display_buff[k] = var[gAio_Info[Aio_num].p_add * 2 + k];
				if (var[gAio_Info[Aio_num].p_add * 2 + k] == 0x00 && var[gAio_Info[Aio_num].p_add * 2 + k + 1] == 0x00)
					break;
			}
			input_len = k;

			if (Screen_DIR == 0)
			{
				dxs = gAio_Info[Aio_num].dp_x;
				dys = gAio_Info[Aio_num].dp_y;
				dxe = LCD_XSIZE_TFT;
				dye = gAio_Info[Aio_num].dp_y + gAio_Info[Aio_num].zk_h;
			}
			else if (Screen_DIR == 1)
			{
				dxs = gAio_Info[Aio_num].dp_x - gAio_Info[Aio_num].zk_h;
				dys = gAio_Info[Aio_num].dp_y;
				dxe = gAio_Info[Aio_num].dp_x;
				dye = LCD_YSIZE_TFT;
			}
			else if (Screen_DIR == 2)
			{
				dxs = 0;
				dys = gAio_Info[Aio_num].dp_y - gAio_Info[Aio_num].zk_h;
				dxe = gAio_Info[Aio_num].dp_x;
				dye = gAio_Info[Aio_num].dp_y;
			}
			else if (Screen_DIR == 3)
			{
				dxs = gAio_Info[Aio_num].dp_x;
				dys = 0;
				dxe = gAio_Info[Aio_num].dp_x + gAio_Info[Aio_num].zk_h;
				dye = gAio_Info[Aio_num].dp_y;
			}

			LT_Print_zk_Font(4, gAio_Info[Aio_num].zk_addr, LAY_BUFF2, LCD_XSIZE_TFT,
							 gAio_Info[Aio_num].zk_w, gAio_Info[Aio_num].zk_h,
							 3, gAio_Info[Aio_num].fcolor, gAio_Info[Aio_num].input_mode,
							 dxs, dys, dxe, dye, 0, 0, Display_buff, input_len);

			LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
								  LAY_BUFF2, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
								  Display_Layer, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
								  0x0c, pic_w, pic_h);
			input_w = data_w;
		}
		else
		{
			LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
								  LAY_BUFF2, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
								  Display_Layer, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x, gAio_Info[Aio_num].kb_y,
								  0x0c, pic_w, pic_h);
		}

		Cusor_time = 0;
	}

	if (gTpInfo.sta && AioKb_flag == 1 && Aio_flag == 0) // Press down within the range of keyboard
	{
		for (i = 0; i < Kb_count; i++)
		{
			if (gTpInfo.x[0] >= gKb_Info[i].Xs + gAio_Info[Aio_num].kb_x && gTpInfo.y[0] >= gKb_Info[i].Ys + gAio_Info[Aio_num].kb_y &&
				gTpInfo.x[0] <= gKb_Info[i].Xe + gAio_Info[Aio_num].kb_x && gTpInfo.y[0] <= gKb_Info[i].Ye + gAio_Info[Aio_num].kb_y &&
				Kb_flag == 0)
			{
				if (gKb_Info[i].press_id != 0xFFFF)
				{
					LT768_BTE_Memory_Copy(LAY_BUFF6, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x + gKb_Info[i].Xs, gAio_Info[Aio_num].kb_y + gKb_Info[i].Ys,
										  LAY_BUFF6, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x + gKb_Info[i].Xs, gAio_Info[Aio_num].kb_y + gKb_Info[i].Ys,
										  Display_Layer, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x + gKb_Info[i].Xs, gAio_Info[Aio_num].kb_y + gKb_Info[i].Ys,
										  0x0c, gKb_Info[i].Xe - gKb_Info[i].Xs + 1, gKb_Info[i].Ye - gKb_Info[i].Ys + 1);
				}
				Kb_num = i;
				Kb_flag = 1;
				if (buzzer)
					touch_buzzer();
			}
		}
	}
	if (gTpInfo.sta == 0 && AioKb_flag == 1 && Kb_flag == 1) // raise
	{
		if ((gKb_Info[Kb_num].press_id != 0xFFFF && gKb_Info[Kb_num].Keyvalue != 0xF3) || CapsLock == 1)
		{
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x + gKb_Info[Kb_num].Xs, gAio_Info[Aio_num].kb_y + gKb_Info[Kb_num].Ys,
								  MAIN_BUFF, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x + gKb_Info[Kb_num].Xs, gAio_Info[Aio_num].kb_y + gKb_Info[Kb_num].Ys,
								  Display_Layer, LCD_XSIZE_TFT, gAio_Info[Aio_num].kb_x + gKb_Info[Kb_num].Xs, gAio_Info[Aio_num].kb_y + gKb_Info[Kb_num].Ys,
								  0x0c, gKb_Info[Kb_num].Xe - gKb_Info[Kb_num].Xs + 1, gKb_Info[Kb_num].Ye - gKb_Info[Kb_num].Ys + 1);
		}

		if (gKb_Info[Kb_num].Keyvalue == 0xF0) // cancel
		{
			AioKb_flag = 0;
			if (Pop_up_flag == 0)
				Display_page(pic_id_temp);
			else if (Pop_up_flag == 1)
			{
				Display_Layer = LAY_BUFF7;
				Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
			}
		}
		else if (gKb_Info[Kb_num].Keyvalue == 0xF1) // confirm
		{
			for (uint16_t j = 0; j < input_len; j++)
			{
				var[gAio_Info[Aio_num].p_add * 2 + j] = Display_buff[j];
			}

			var[gAio_Info[Aio_num].p_add * 2 + input_len] = 0x00;
			var[gAio_Info[Aio_num].p_add * 2 + input_len + 1] = 0x00;

			if (gAio_Info[Aio_num].Code == 0xC009)
			{
				rebuf[3] = 0x41;
				rebuf[4] = gAio_Info[Aio_num].p_add >> 8;
				rebuf[5] = gAio_Info[Aio_num].p_add & 0xFF;
				for (uint8_t j = 0; j < input_len; j++)
				{
					rebuf[6 + j] = var[gAio_Info[Aio_num].p_add * 2 + j];
				}
				if (input_len % 2 > 0)
				{
					rebuf[6 + input_len] = var[gAio_Info[Aio_num].p_add * 2 + input_len];
					LT_SendData_CRC_Frame(rebuf, 3 + input_len + 1);
				}
				else
					LT_SendData_CRC_Frame(rebuf, 3 + input_len);
			}
			if (Pop_up_flag == 0)
				Display_page(pic_id_temp);
			else if (Pop_up_flag == 1)
			{
				Display_Layer = LAY_BUFF7;
				Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
			}
		}
		else if (gKb_Info[Kb_num].Keyvalue == 0xF2) // backspace
		{
			if (input_len > 0)
			{
				Cusor_time = 0;
				input_len--;

				Display_buff[input_len] = 0x00;

				if (Screen_DIR == 0)
				{
					dx = gAio_Info[Aio_num].dp_x;
					dy = gAio_Info[Aio_num].dp_y;
					w_temp = input_w + cur_w;
					h_temp = gAio_Info[Aio_num].zk_h;
					
					dxs = gAio_Info[Aio_num].dp_x;
					dys = gAio_Info[Aio_num].dp_y;
					dxe = gAio_Info[Aio_num].dp_x + w_temp;
					dye = gAio_Info[Aio_num].dp_y + gAio_Info[Aio_num].zk_h;
				}
				else if (Screen_DIR == 1)
				{
					dx = gAio_Info[Aio_num].dp_x - gAio_Info[Aio_num].zk_h + 1;
					dy = gAio_Info[Aio_num].dp_y;
					w_temp = gAio_Info[Aio_num].zk_h;
					h_temp = input_w + cur_w;

					dxs = gAio_Info[Aio_num].dp_x - gAio_Info[Aio_num].zk_h + 1;
					dys = gAio_Info[Aio_num].dp_y;
					dxe = gAio_Info[Aio_num].dp_x;
					dye = gAio_Info[Aio_num].dp_y + input_w + cur_w;
				}
				else if (Screen_DIR == 2)
				{
					dx = gAio_Info[Aio_num].dp_x - input_w - cur_w + 1;
					dy = gAio_Info[Aio_num].dp_y - gAio_Info[Aio_num].zk_h;
					w_temp = input_w + cur_w;
					h_temp = gAio_Info[Aio_num].zk_h;

					dxs = gAio_Info[Aio_num].dp_x - w_temp;
					dys = gAio_Info[Aio_num].dp_y - gAio_Info[Aio_num].zk_h;
					dxe = gAio_Info[Aio_num].dp_x;
					dye = gAio_Info[Aio_num].dp_y;
				}
				else if (Screen_DIR == 3)
				{
					dx = gAio_Info[Aio_num].dp_x;
					dy = gAio_Info[Aio_num].dp_y - input_w + 1 - cur_w;
					w_temp = gAio_Info[Aio_num].zk_h;
					h_temp = input_w + cur_w;

					dxs = gAio_Info[Aio_num].dp_x;
					dys = gAio_Info[Aio_num].dp_y - h_temp;
					dxe = gAio_Info[Aio_num].dp_x + gAio_Info[Aio_num].zk_h;
					dye = gAio_Info[Aio_num].dp_y;
				}

				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
									  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
									  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
									  0x0c, w_temp, h_temp);

				LT_Print_zk_Font(4, gAio_Info[Aio_num].zk_addr, LAY_BUFF1, LCD_XSIZE_TFT, gAio_Info[Aio_num].zk_w, gAio_Info[Aio_num].zk_h,
								 3, gAio_Info[Aio_num].fcolor, gAio_Info[Aio_num].input_mode,
								 dxs, dys, dxe, dye, 0, 0, Display_buff, input_len);

				LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
									  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
									  Display_Layer, LCD_XSIZE_TFT, dx, dy,
									  0x0c, w_temp, h_temp);

				input_w = data_w;
			}
			else
			{
				Cusor_time = 0;
				input_len = 0;
			}
		}
		else if (gKb_Info[Kb_num].Keyvalue == 0xF3) // Caps lock
		{
			if (CapsLock == 0)
				CapsLock = 1;
			else
				CapsLock = 0;
		}
		else if (gKb_Info[Kb_num].Keyvalue == 0xF4)
		{
		}
		else if (gKb_Info[Kb_num].Keyvalue == 0xF5) // Full clearing
		{
			if (Screen_DIR == 0)
			{
				dx = gAio_Info[Aio_num].dp_x;
				dy = gAio_Info[Aio_num].dp_y;
				w_temp = data_w + cur_w;
				h_temp = gAio_Info[Aio_num].zk_h;
			}
			else if (Screen_DIR == 1)
			{
				dx = gAio_Info[Aio_num].dp_x - gAio_Info[Aio_num].zk_h + 1;
				dy = gAio_Info[Aio_num].dp_y;
				w_temp = gAio_Info[Aio_num].zk_h;
				h_temp = input_w + cur_w;
			}
			else if (Screen_DIR == 2)
			{
				dx = gAio_Info[Aio_num].dp_x - data_w - cur_w + 1;
				dy = gAio_Info[Aio_num].dp_y - gAio_Info[Aio_num].zk_h;
				w_temp = data_w + cur_w;
				h_temp = gAio_Info[Aio_num].zk_h;
			}
			else if (Screen_DIR == 3)
			{
				dx = gAio_Info[Aio_num].dp_x;
				dy = gAio_Info[Aio_num].dp_y - input_w + 1 - cur_w;
				w_temp = gAio_Info[Aio_num].zk_h;
				h_temp = input_w + cur_w;
			}

			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
								  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
								  Display_Layer, LCD_XSIZE_TFT, dx, dy,
								  0x0c, w_temp, h_temp);

			memset(Display_buff, 0, input_len);
			input_len = 0;
			input_w = 0;
		}
		else // Normal key value
		{
			if (gAio_Info[Aio_num].p_len * 2 > input_len)
			{
				if (CapsLock == 1)
				{
					Display_buff[input_len] = (gKb_Info[Kb_num].Keyvalue >> 8) & 0xFF;
				}
				else
				{
					Display_buff[input_len] = gKb_Info[Kb_num].Keyvalue & 0xFF;
				}
				input_len++;
			}

			if (Screen_DIR == 0)
			{
				dx = gAio_Info[Aio_num].dp_x;
				dy = gAio_Info[Aio_num].dp_y;
				w_temp = input_w + gAio_Info[Aio_num].zk_w;
				h_temp = gAio_Info[Aio_num].zk_h;
				dxs = gAio_Info[Aio_num].dp_x;
				dys = gAio_Info[Aio_num].dp_y;
				dxe = gAio_Info[Aio_num].dp_x + w_temp;
				dye = gAio_Info[Aio_num].dp_y + gAio_Info[Aio_num].zk_h;
			}
			else if (Screen_DIR == 1)
			{
				dx = gAio_Info[Aio_num].dp_x - gAio_Info[Aio_num].zk_h;
				dy = gAio_Info[Aio_num].dp_y;
				w_temp = gAio_Info[Aio_num].zk_h;
				h_temp = input_w + cur_w + gAio_Info[Aio_num].zk_w;

				dxs = dx;
				dys = gAio_Info[Aio_num].dp_y;
				dxe = gAio_Info[Aio_num].dp_x;
				dye = gAio_Info[Aio_num].dp_y + h_temp;
			}
			else if (Screen_DIR == 2)
			{
				dx = gAio_Info[Aio_num].dp_x - input_w - gAio_Info[Aio_num].zk_w + 1;
				dy = gAio_Info[Aio_num].dp_y - gAio_Info[Aio_num].zk_h;
				w_temp = input_w + gAio_Info[Aio_num].zk_w;
				h_temp = gAio_Info[Aio_num].zk_h;

				dxs = gAio_Info[Aio_num].dp_x - w_temp;
				dys = gAio_Info[Aio_num].dp_y - gAio_Info[Aio_num].zk_h;
				dxe = gAio_Info[Aio_num].dp_x;
				dye = gAio_Info[Aio_num].dp_y;
			}
			else if (Screen_DIR == 3)
			{
				dx = gAio_Info[Aio_num].dp_x;
				dy = gAio_Info[Aio_num].dp_y - input_w + 1 - gAio_Info[Aio_num].zk_w + 1 - cur_w;
				w_temp = gAio_Info[Aio_num].zk_h;
				h_temp = input_w + cur_w + gAio_Info[Aio_num].zk_w;

				dxs = gAio_Info[Aio_num].dp_x;
				dys = gAio_Info[Aio_num].dp_y - h_temp;
				dxe = gAio_Info[Aio_num].dp_x + gAio_Info[Aio_num].zk_h;
				dye = gAio_Info[Aio_num].dp_y;
			}

			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
								  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
								  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
								  0x0c, w_temp, h_temp);

			LT_Print_zk_Font(4, gAio_Info[Aio_num].zk_addr,
							 LAY_BUFF1, LCD_XSIZE_TFT, gAio_Info[Aio_num].zk_w, gAio_Info[Aio_num].zk_h,
							 3, gAio_Info[Aio_num].fcolor, gAio_Info[Aio_num].input_mode,
							 dxs, dys, dxe, dye, 0, 0, Display_buff, input_len);

			if (Screen_DIR == 0)
			{
				w_temp = data_w + cur_w;
			}
			else if (Screen_DIR == 1)
			{
				h_temp = data_w + cur_w;
			}
			else if (Screen_DIR == 2)
			{
				dx = gAio_Info[Aio_num].dp_x - data_w + 1;
				w_temp = data_w;
			}
			else if (Screen_DIR == 3)
			{
				dy = gAio_Info[Aio_num].dp_y - data_w + 1;
				h_temp = data_w;
			}

			LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
								  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
								  Display_Layer, LCD_XSIZE_TFT, dx, dy,
								  0x0c, w_temp, h_temp);

			input_w = data_w;
		}
		Kb_flag = 0;
	}
}
/******************************/

/***********************************************************************************
 * Function Name	: GBK_input
 * Description		: Touch detection of GBK keyboard.
 * Input			: None
 * Output       	: None
 * Return       	: None
 ************************************************************************************/
void GBK_input(void)
{
	uint8_t res = 0, i = 0, l = 0, width_Hor = 0;
	uint8_t rebuf[50], buff1[12] = {0};
	uint16_t canvas_w = 0, k = 0;
	uint16_t w_temp, h_temp;
	uint16_t dx, dy, dxs, dxe, dys, dye;
	uint32_t zk_addr = 0, color;

	if (gTpInfo.sta && GBKioKb_flag == 0) // press down
	{
		for (i = 0; i < GBKio_count; i++)
		{
			if (gTpInfo.x[0] >= gGBKio_Info[i].Xs && gTpInfo.y[0] >= gGBKio_Info[i].Ys &&
				gTpInfo.x[0] <= gGBKio_Info[i].Xe && gTpInfo.y[0] <= gGBKio_Info[i].Ye &&
				GBKio_flag == 0 && button_Press_flag == 0)
			{
				GBKio_num = i;
				GBKio_flag = 1;
				button_Press_flag = 1;
				if (buzzer)
					touch_buzzer();
			}
		}
	}
	if (Button_Key_Code_Flag == 1)
	{
		for (i = 0; i < GBKio_count; i++)
		{
			if (gGBKio_Info[i].Xs == 0xFFFF && gGBKio_Info[i].Ys ==  Button_Key_Code)
			{
				Button_Key_Code_Flag = 0;
				Button_Key_Code = 0;
				GBKio_num = i;
				GBKio_flag = 1;
				if (Pop_up_flag == 1) // pop-up window
				{
					LT768_BTE_Memory_Copy(Display_Layer, LCD_XSIZE_TFT, 0, 0,
										  Display_Layer, LCD_XSIZE_TFT, 0, 0,
										  LAY_BUFF5, LCD_XSIZE_TFT, 0, 0,
										  0x0c, LCD_XSIZE_TFT, LCD_YSIZE_TFT);
				}

				Flash_Read_UI(buff1, addr_index[6] + 12 * gGBKio_Info[GBKio_num].kb_id, 12);
				pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
				pic_w = buff1[4] + (buff1[5] << 8);
				pic_h = buff1[6] + (buff1[7] << 8);
				pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
				pic_flag = buff1[11];

				if (pic_len != 0 && pic_flag != 0xFF)
				{
					canvas_w = Get_Canvas_Width(pic_w);
					Canvas_Image_Start_address(LAY_BUFF1);
					Canvas_image_width(canvas_w);

					LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
														pic_w, pic_h,
														pic_w, pic_add,
														LAY_BUFF1, canvas_w);

					if (pic_flag == 0)
					{
						LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
											  LAY_BUFF1, canvas_w, 0, 0,
											  MAIN_BUFF, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
											  0x0c, pic_w, pic_h);

						LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
											  LAY_BUFF1, canvas_w, 0, 0,
											  LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
											  0x0c, pic_w, pic_h);
					}
					else if (pic_flag == 1)
					{
						LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
														 MAIN_BUFF, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
														 Black, pic_w, pic_h);

						LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
														 LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
														 Black, pic_w, pic_h);
					}
					else if (pic_flag == 2)
					{
						BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
													   LAY_BUFF1, canvas_w, 0, 0,
													   MAIN_BUFF, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
													   pic_w, pic_h);

						BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
													   LAY_BUFF1, canvas_w, 0, 0,
													   LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
													   pic_w, pic_h);
					}
				}
				/*Dynamic picture coverage judgment*/
				GBKioKb_flag = 1;
				gGBKio_Info[GBKio_num].Xs = gGBKio_Info[GBKio_num].kb_x;
				gGBKio_Info[GBKio_num].Ys = gGBKio_Info[GBKio_num].kb_y;
				gGBKio_Info[GBKio_num].Xe = gGBKio_Info[GBKio_num].kb_x + pic_w;
				gGBKio_Info[GBKio_num].Ye = gGBKio_Info[GBKio_num].kb_y + pic_h;

				Flash_Read_UI(buff1, addr_index[2] + gGBKio_Info[GBKio_num].py_zk * 8, 4);
				gGBKio_Info[GBKio_num].py_addr = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
				Flash_Read_UI(buff1, addr_index[2] + gGBKio_Info[GBKio_num].zk_id * 8, 4);
				gGBKio_Info[GBKio_num].zk_addr = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
				cur_x = gGBKio_Info[GBKio_num].dp_x;
				cur_y = gGBKio_Info[GBKio_num].dp_y;
				buff1[0] = '|';
				buff1[1] = 0;

				Flash_Read_UI(&res, gGBKio_Info[GBKio_num].zk_addr + 9 + 92, 1);
				if (Screen_DIR == 0)
				{
					dx = cur_x;
					dy = cur_y;
					w_temp = res;
					h_temp = gGBKio_Info[GBKio_num].zk_h;
				}
				else if (Screen_DIR == 1)
				{
					dx = cur_x;
					dy = cur_y;
					cur_x = cur_x - gGBKio_Info[GBKio_num].zk_h + 1;
					cur_y = cur_y;
					w_temp = gGBKio_Info[GBKio_num].zk_h;
					h_temp = res;
				}
				else if (Screen_DIR == 2)
				{
					dx = cur_x;
					dy = cur_y;
					cur_x = cur_x - res + 1;
					cur_y = cur_y - gGBKio_Info[GBKio_num].zk_h + 1;
					w_temp = res;
					h_temp = gGBKio_Info[GBKio_num].zk_h;
				}
				else if (Screen_DIR == 3)
				{
					dx = cur_x;
					dy = cur_y + res - 1;
					w_temp = gGBKio_Info[GBKio_num].zk_h;
					h_temp = res;
				}

				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, cur_x, cur_y,
									  MAIN_BUFF, LCD_XSIZE_TFT, cur_x, cur_y,
									  LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
									  0x0c, w_temp, h_temp);

				if (gGBKio_Info[GBKio_num].cursor_color == 0)
					color = Black;
				else
					color = White;

				LT_Print_zk_ASCII(gGBKio_Info[GBKio_num].zk_addr, LAY_BUFF1, LCD_XSIZE_TFT,
								  gGBKio_Info[GBKio_num].zk_w, gGBKio_Info[GBKio_num].zk_h, 0, color,
								  dx, dy, buff1);

				cur_w = data_w - 2;

				LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
									  LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
									  LAY_BUFF5, LCD_XSIZE_TFT, cur_x, cur_y,
									  0x0c, w_temp, h_temp);

				button_Press_flag = 0;
				GBK_PY_length = 0;
				input_len = 0;
				CapsLock = 0;
				GBK_CN_EN = 0;
				input_w = 0;
				memset(Display_buff, 0, 400);
				pic_id_temp = pic_id;
				pic_id = gGBKio_Info[GBKio_num].kb_id;
				if (gGBKio_Info[GBKio_num].scan_mode == 0)
				{
					for (k = 0; k < gGBKio_Info[GBKio_num].p_len * 2; k++)
					{
						Display_buff[k] = var[gGBKio_Info[GBKio_num].p_add * 2 + k];
						if (var[gGBKio_Info[GBKio_num].p_add * 2 + k] == 0x00 && var[gGBKio_Info[GBKio_num].p_add * 2 + k + 1] == 0x00)
							break;
					}
					input_len = k;

					if (Screen_DIR == 0)
					{
						dxs = gGBKio_Info[GBKio_num].dp_x;
						dys = gGBKio_Info[GBKio_num].dp_y;
						dxe = LCD_XSIZE_TFT;
						dye = gGBKio_Info[GBKio_num].dp_y + gGBKio_Info[GBKio_num].zk_h;
					}
					else if (Screen_DIR == 1)
					{
						dxs = gGBKio_Info[GBKio_num].dp_x - gGBKio_Info[GBKio_num].zk_h + 1;
						dys = gGBKio_Info[GBKio_num].dp_y;
						dxe = gGBKio_Info[GBKio_num].dp_x;
						dye = LCD_YSIZE_TFT;
					}
					else if (Screen_DIR == 2)
					{
						dxs = 0;
						dys = gGBKio_Info[GBKio_num].dp_y - gGBKio_Info[GBKio_num].zk_h;
						dxe = gGBKio_Info[GBKio_num].dp_x;
						dye = gGBKio_Info[GBKio_num].dp_y;
					}
					else if (Screen_DIR == 3)
					{
						dxs = gGBKio_Info[GBKio_num].dp_x;
						dys = 0;
						dxe = gGBKio_Info[GBKio_num].dp_x + gGBKio_Info[GBKio_num].zk_h;
						dye = gGBKio_Info[GBKio_num].dp_y;
					}

					LT_Print_zk_Font(1, gGBKio_Info[GBKio_num].zk_addr,
									 LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].zk_w, gGBKio_Info[GBKio_num].zk_h,
									 3, gGBKio_Info[GBKio_num].fcolor, 0,
									 dxs, dys, dxe, dye, 0, 0, Display_buff, input_len);

					LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
										  LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
										  Display_Layer, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
										  0x0c, pic_w, pic_h);
					input_w = data_w;
				}
				else
				{
					LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
										  LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
										  Display_Layer, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
										  0x0c, pic_w, pic_h);
				}
				Get_Touch_info(gGBKio_Info[GBKio_num].kb_id);
				GBKio_flag = 0; // 0 after loading the underlay
				memset(GBK_PY_buff, 0, 6);
				Cusor_time = 0;
			}	
		}
	}
	if (gTpInfo.sta == 0 && GBKio_flag == 1 && GBKioKb_flag == 0) // raise
	{
		if (Pop_up_flag == 1) // pop-up window
		{
			LT768_BTE_Memory_Copy(Display_Layer, LCD_XSIZE_TFT, 0, 0,
								  Display_Layer, LCD_XSIZE_TFT, 0, 0,
								  LAY_BUFF5, LCD_XSIZE_TFT, 0, 0,
								  0x0c, LCD_XSIZE_TFT, LCD_YSIZE_TFT);
		}

		Flash_Read_UI(buff1, addr_index[6] + 12 * gGBKio_Info[GBKio_num].kb_id, 12);
		pic_add = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
		pic_w = buff1[4] + (buff1[5] << 8);
		pic_h = buff1[6] + (buff1[7] << 8);
		pic_len = buff1[8] + (buff1[9] << 8) + (buff1[10] << 16);
		pic_flag = buff1[11];

		if (pic_len != 0 && pic_flag != 0xFF)
		{
			canvas_w = Get_Canvas_Width(pic_w);
			Canvas_Image_Start_address(LAY_BUFF1);
			Canvas_image_width(canvas_w);

			LT768_DMA_24bit_Block_Or_Line_16bpp(SPI_X, CLK_DIV, 0, 0,
												pic_w, pic_h,
												pic_w, pic_add,
												LAY_BUFF1, canvas_w);

			if (pic_flag == 0)
			{
				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF1, canvas_w, 0, 0,
									  MAIN_BUFF, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
									  0x0c, pic_w, pic_h);

				LT768_BTE_Memory_Copy(LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF1, canvas_w, 0, 0,
									  LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
									  0x0c, pic_w, pic_h);
			}
			else if (pic_flag == 1)
			{
				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
												 MAIN_BUFF, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
												 Black, pic_w, pic_h);

				LT768_BTE_Memory_Copy_Chroma_key(LAY_BUFF1, canvas_w, 0, 0,
												 LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
												 Black, pic_w, pic_h);
			}
			else if (pic_flag == 2)
			{
				BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   MAIN_BUFF, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
											   pic_w, pic_h);

				BTE_Pixel_16bpp_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
											   LAY_BUFF1, canvas_w, 0, 0,
											   LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
											   pic_w, pic_h);
			}
		}
		/*Dynamic picture coverage judgment*/
		GBKioKb_flag = 1;
		gGBKio_Info[GBKio_num].Xs = gGBKio_Info[GBKio_num].kb_x;
		gGBKio_Info[GBKio_num].Ys = gGBKio_Info[GBKio_num].kb_y;
		gGBKio_Info[GBKio_num].Xe = gGBKio_Info[GBKio_num].kb_x + pic_w;
		gGBKio_Info[GBKio_num].Ye = gGBKio_Info[GBKio_num].kb_y + pic_h;

		Flash_Read_UI(buff1, addr_index[2] + gGBKio_Info[GBKio_num].py_zk * 8, 4);
		gGBKio_Info[GBKio_num].py_addr = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
		Flash_Read_UI(buff1, addr_index[2] + gGBKio_Info[GBKio_num].zk_id * 8, 4);
		gGBKio_Info[GBKio_num].zk_addr = buff1[0] + (buff1[1] << 8) + (buff1[2] << 16) + (buff1[3] << 24);
		cur_x = gGBKio_Info[GBKio_num].dp_x;
		cur_y = gGBKio_Info[GBKio_num].dp_y;
		buff1[0] = '|';
		buff1[1] = 0;

		Flash_Read_UI(&res, gGBKio_Info[GBKio_num].zk_addr + 9 + 92, 1);
		if (Screen_DIR == 0)
		{
			dx = cur_x;
			dy = cur_y;
			w_temp = res;
			h_temp = gGBKio_Info[GBKio_num].zk_h;
		}
		else if (Screen_DIR == 1)
		{
			dx = cur_x;
			dy = cur_y;
			cur_x = cur_x - gGBKio_Info[GBKio_num].zk_h + 1;
			cur_y = cur_y;
			w_temp = gGBKio_Info[GBKio_num].zk_h;
			h_temp = res;
		}
		else if (Screen_DIR == 2)
		{
			dx = cur_x;
			dy = cur_y;
			cur_x = cur_x - res + 1;
			cur_y = cur_y - gGBKio_Info[GBKio_num].zk_h + 1;
			w_temp = res;
			h_temp = gGBKio_Info[GBKio_num].zk_h;
		}
		else if (Screen_DIR == 3)
		{
			dx = cur_x;
			dy = cur_y + res - 1;
			w_temp = gGBKio_Info[GBKio_num].zk_h;
			h_temp = res;
		}

		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, cur_x, cur_y,
							  MAIN_BUFF, LCD_XSIZE_TFT, cur_x, cur_y,
							  LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
							  0x0c, w_temp, h_temp);

		if (gGBKio_Info[GBKio_num].cursor_color == 0)
			color = Black;
		else
			color = White;

		LT_Print_zk_ASCII(gGBKio_Info[GBKio_num].zk_addr, LAY_BUFF1, LCD_XSIZE_TFT,
						  gGBKio_Info[GBKio_num].zk_w, gGBKio_Info[GBKio_num].zk_h, 0, color,
						  dx, dy, buff1);

		cur_w = data_w - 2;

		LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
							  LAY_BUFF1, LCD_XSIZE_TFT, cur_x, cur_y,
							  LAY_BUFF5, LCD_XSIZE_TFT, cur_x, cur_y,
							  0x0c, w_temp, h_temp);

		button_Press_flag = 0;
		GBK_PY_length = 0;
		input_len = 0;
		CapsLock = 0;
		GBK_CN_EN = 0;
		input_w = 0;
		memset(Display_buff, 0, 400);
		pic_id_temp = pic_id;
		pic_id = gGBKio_Info[GBKio_num].kb_id;
		if (gGBKio_Info[GBKio_num].scan_mode == 1)
		{
			for (k = 0; k < gGBKio_Info[GBKio_num].p_len * 2; k++)
			{
				Display_buff[k] = var[gGBKio_Info[GBKio_num].p_add * 2 + k];
				if (var[gGBKio_Info[GBKio_num].p_add * 2 + k] == 0x00 && var[gGBKio_Info[GBKio_num].p_add * 2 + k + 1] == 0x00)
					break;
			}
			input_len = k;

			if (Screen_DIR == 0)
			{
				dxs = gGBKio_Info[GBKio_num].dp_x;
				dys = gGBKio_Info[GBKio_num].dp_y;
				dxe = LCD_XSIZE_TFT;
				dye = gGBKio_Info[GBKio_num].dp_y + gGBKio_Info[GBKio_num].zk_h;
			}
			else if (Screen_DIR == 1)
			{
				dxs = gGBKio_Info[GBKio_num].dp_x - gGBKio_Info[GBKio_num].zk_h + 1;
				dys = gGBKio_Info[GBKio_num].dp_y;
				dxe = gGBKio_Info[GBKio_num].dp_x;
				dye = LCD_YSIZE_TFT;
			}
			else if (Screen_DIR == 2)
			{
				dxs = 0;
				dys = gGBKio_Info[GBKio_num].dp_y - gGBKio_Info[GBKio_num].zk_h;
				dxe = gGBKio_Info[GBKio_num].dp_x;
				dye = gGBKio_Info[GBKio_num].dp_y;
			}
			else if (Screen_DIR == 3)
			{
				dxs = gGBKio_Info[GBKio_num].dp_x;
				dys = 0;
				dxe = gGBKio_Info[GBKio_num].dp_x + gGBKio_Info[GBKio_num].zk_h;
				dye = gGBKio_Info[GBKio_num].dp_y;
			}

			LT_Print_zk_Font(1, gGBKio_Info[GBKio_num].zk_addr,
							 LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].zk_w, gGBKio_Info[GBKio_num].zk_h,
							 3, gGBKio_Info[GBKio_num].fcolor, 0,
							 dxs, dys, dxe, dye, 0, 0, Display_buff, input_len);

			LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
								  LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
								  Display_Layer, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
								  0x0c, pic_w, pic_h);
			input_w = data_w;
		}
		else
		{
			LT768_BTE_Memory_Copy(LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
								  LAY_BUFF2, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
								  Display_Layer, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x, gGBKio_Info[GBKio_num].kb_y,
								  0x0c, pic_w, pic_h);
		}
		Get_Touch_info(gGBKio_Info[GBKio_num].kb_id);
		GBKio_flag = 0; // 0 after loading the underlay
		memset(GBK_PY_buff, 0, 6);
		Cusor_time = 0;
	}

	if (gTpInfo.sta && GBKioKb_flag == 1 && GBKio_flag == 0) // press down within the range of keyboard
	{
		for (i = 0; i < Kb_count; i++)
		{
			if (gTpInfo.x[0] >= gKb_Info[i].Xs + gGBKio_Info[GBKio_num].kb_x && gTpInfo.y[0] >= gKb_Info[i].Ys + gGBKio_Info[GBKio_num].kb_y &&
				gTpInfo.x[0] <= gKb_Info[i].Xe + gGBKio_Info[GBKio_num].kb_x && gTpInfo.y[0] <= gKb_Info[i].Ye + gGBKio_Info[GBKio_num].kb_y &&
				Kb_flag == 0)
			{
				if (gKb_Info[i].press_id != 0xFFFF)
				{
					LT768_BTE_Memory_Copy(LAY_BUFF6, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x + gKb_Info[i].Xs, gGBKio_Info[GBKio_num].kb_y + gKb_Info[i].Ys,
										  LAY_BUFF6, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x + gKb_Info[i].Xs, gGBKio_Info[GBKio_num].kb_y + gKb_Info[i].Ys,
										  Display_Layer, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x + gKb_Info[i].Xs, gGBKio_Info[GBKio_num].kb_y + gKb_Info[i].Ys,
										  0x0c, gKb_Info[i].Xe - gKb_Info[i].Xs + 1, gKb_Info[i].Ye - gKb_Info[i].Ys + 1);
				}
				Kb_num = i;
				Kb_flag = 1;
				if (buzzer)
					touch_buzzer();
			}
		}

		if (GBK_PY_length > 0 && Kb_flag == 0 && GBK_PY_num > 0)
			choice_GBK();
	}

	if (gTpInfo.sta == 0 && GBKioKb_flag == 1 && GBKio_flag == 0 && Kb_flag == 1)
	{
		if ((gKb_Info[Kb_num].press_id != 0xFFFF && gKb_Info[Kb_num].Keyvalue != 0xF3 && gKb_Info[Kb_num].Keyvalue != 0xF4) ||
			(CapsLock == 1 && gKb_Info[Kb_num].Keyvalue == 0xF3) || (GBK_CN_EN == 1 && gKb_Info[Kb_num].Keyvalue == 0xF4))
		{
			LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x + gKb_Info[Kb_num].Xs, gGBKio_Info[GBKio_num].kb_y + gKb_Info[Kb_num].Ys,
								  MAIN_BUFF, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x + gKb_Info[Kb_num].Xs, gGBKio_Info[GBKio_num].kb_y + gKb_Info[Kb_num].Ys,
								  Display_Layer, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].kb_x + gKb_Info[Kb_num].Xs, gGBKio_Info[GBKio_num].kb_y + gKb_Info[Kb_num].Ys,
								  0x0c, gKb_Info[Kb_num].Xe - gKb_Info[Kb_num].Xs + 1, gKb_Info[Kb_num].Ye - gKb_Info[Kb_num].Ys + 1);
		}

		if (GBK_PY_length == 0 && (CapsLock == 1 || GBK_CN_EN == 1)) // Pinyin is not entered and is in uppercase status
		{
			if (gKb_Info[Kb_num].Keyvalue == 0xF0) // Cancel
			{
				GBKioKb_flag = 0;
				if (Pop_up_flag == 0)
					Display_page(pic_id_temp);
				else if (Pop_up_flag == 1)
				{
					Display_Layer = LAY_BUFF7;
					Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
				}
			}
			else if (gKb_Info[Kb_num].Keyvalue == 0xF1) // Confirm
			{
				for (uint16_t j = 0; j < input_len; j++)
				{
					var[gGBKio_Info[GBKio_num].p_add * 2 + j] = Display_buff[j];
				}

				var[gGBKio_Info[GBKio_num].p_add * 2 + input_len] = 0x00;
				var[gGBKio_Info[GBKio_num].p_add * 2 + input_len + 1] = 0x00;

				if (gGBKio_Info[GBKio_num].Code == 0xC00A) // Serial port feedback
				{
					rebuf[3] = 0x41;
					rebuf[4] = gGBKio_Info[GBKio_num].p_add >> 8;
					rebuf[5] = gGBKio_Info[GBKio_num].p_add & 0xFF;

					if (input_len != 0)
					{
						for (uint16_t j = 0; j < input_len; j++)
						{
							rebuf[6 + j] = var[gGBKio_Info[GBKio_num].p_add * 2 + j];
						}
						if (input_len % 2 == 0)
							LT_SendData_CRC_Frame(rebuf, 3 + input_len);
						else
						{
							rebuf[6 + input_len] = var[gGBKio_Info[GBKio_num].p_add * 2 + input_len];
							LT_SendData_CRC_Frame(rebuf, 3 + input_len + 1);
						}
					}
					else if (input_len == 0)
					{
						rebuf[6] = 0x00;
						rebuf[7] = 0x00;
						LT_SendData_CRC_Frame(rebuf, 5);
					}
				}
				if (Pop_up_flag == 0)
					Display_page(pic_id_temp);
				else if (Pop_up_flag == 1)
				{
					Display_Layer = LAY_BUFF7;
					Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
				}
			}
			else if (gKb_Info[Kb_num].Keyvalue == 0xF2) // Backspace
			{
				if (GBK_PY_length == 0)
				{
					if (input_len > 0)
					{
						for (l = 0; l < input_len;)
						{
							if (Display_buff[l] <= 0x7F)
							{
								l++;
								if (l == input_len)
								{
									Display_buff[input_len - 1] = 0x00;
									input_len--;
									break;
								}
							}
							else
							{

								l += 2;
								if (l == input_len)
								{
									Display_buff[input_len - 1] = 0x00;
									Display_buff[input_len - 2] = 0x00;
									input_len -= 2;
									break;
								}
							}
						}
						if (Screen_DIR == 0)
						{
							dx = gGBKio_Info[GBKio_num].dp_x;
							dy = gGBKio_Info[GBKio_num].dp_y;
							w_temp = input_w + cur_w;
							h_temp = gGBKio_Info[GBKio_num].zk_h;

							dxs = gGBKio_Info[GBKio_num].dp_x;
							dys = gGBKio_Info[GBKio_num].dp_y;
							dxe = gGBKio_Info[GBKio_num].dp_x + w_temp;
							dye = gGBKio_Info[GBKio_num].dp_y + gGBKio_Info[GBKio_num].zk_h;
						}
						else if (Screen_DIR == 1)
						{
							dx = gGBKio_Info[GBKio_num].dp_x - gGBKio_Info[GBKio_num].zk_h + 1;
							dy = gGBKio_Info[GBKio_num].dp_y;
							w_temp = gGBKio_Info[GBKio_num].zk_h;
							h_temp = input_w + cur_w;

							dxs = dx;
							dys = dy;
							dxe = gGBKio_Info[GBKio_num].dp_x;
							dye = dy + input_w + cur_w;
						}
						else if (Screen_DIR == 2)
						{
							dx = gGBKio_Info[GBKio_num].dp_x - input_w - cur_w + 1;
							dy = gGBKio_Info[GBKio_num].dp_y - gGBKio_Info[GBKio_num].zk_h;
							w_temp = input_w + cur_w;
							h_temp = gGBKio_Info[GBKio_num].zk_h;

							dxs = gGBKio_Info[GBKio_num].dp_x - w_temp;
							dys = gGBKio_Info[GBKio_num].dp_y - gGBKio_Info[GBKio_num].zk_h;
							dxe = gGBKio_Info[GBKio_num].dp_x;
							dye = gGBKio_Info[GBKio_num].dp_y;
						}
						else if (Screen_DIR == 3)
						{
							dx = gGBKio_Info[GBKio_num].dp_x;
							dy = gAio_Info[Aio_num].dp_y - input_w + 1 - cur_w;
							w_temp = gGBKio_Info[GBKio_num].zk_h;
							h_temp = gGBKio_Info[GBKio_num].dp_y + input_w + cur_w;

							dxs = gGBKio_Info[GBKio_num].dp_x;
							dys = gGBKio_Info[GBKio_num].dp_y - h_temp;
							dxe = gGBKio_Info[GBKio_num].dp_x + gGBKio_Info[GBKio_num].zk_h;
							dye = gGBKio_Info[GBKio_num].dp_y;
						}

						LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
											  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
											  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
											  0x0c, w_temp, h_temp);

						LT_Print_zk_Font(1, gGBKio_Info[GBKio_num].zk_addr,
										 LAY_BUFF1, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].zk_w, gGBKio_Info[GBKio_num].zk_h,
										 3, gGBKio_Info[GBKio_num].fcolor, 0,
										 dxs, dys, dxe, dye, 0, 0, Display_buff, input_len);

						LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
											  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
											  Display_Layer, LCD_XSIZE_TFT, dx, dy,
											  0x0c, w_temp, h_temp);

						input_w = data_w;
					}
				}
				else if (GBK_PY_length > 0)
				{
					GBK_PY_length--;
					GBK_linenum = 0;
					GBK_PY_buff[GBK_PY_length] = 0x00;
					Display_PY_GBK();
				}
			}
			else if (gKb_Info[Kb_num].Keyvalue == 0xF3) // Caps lock
			{
				if (CapsLock == 0)
					CapsLock = 1;
				else
					CapsLock = 0;
			}
			else if (gKb_Info[Kb_num].Keyvalue == 0xF4) // Chinese and English input method switching
			{
				if (GBK_CN_EN == 0)
					GBK_CN_EN = 1;
				else
					GBK_CN_EN = 0; // Pinyin input
			}
			else if (gKb_Info[Kb_num].Keyvalue == 0xF5) // Clear display box of font
			{
				if (Screen_DIR == 0)
				{
					dx = gGBKio_Info[GBKio_num].dp_x;
					dy = gGBKio_Info[GBKio_num].dp_y;
					w_temp = input_w + cur_w;
					h_temp = gGBKio_Info[GBKio_num].zk_h;
				}
				else if (Screen_DIR == 1)
				{
					dx = gGBKio_Info[GBKio_num].dp_x - gGBKio_Info[GBKio_num].zk_h + 1;
					dy = gGBKio_Info[GBKio_num].dp_y;
					w_temp = gGBKio_Info[GBKio_num].zk_h;
					h_temp = input_w + cur_w;
				}
				else if (Screen_DIR == 2)
				{
					dx = gGBKio_Info[GBKio_num].dp_x - input_w - cur_w + 1;
					dy = gGBKio_Info[GBKio_num].dp_y - gGBKio_Info[GBKio_num].zk_h;
					w_temp = input_w + cur_w;
					h_temp = gGBKio_Info[GBKio_num].zk_h;
				}
				else if (Screen_DIR == 3)
				{
					dx = gGBKio_Info[GBKio_num].dp_x;
					dy = gGBKio_Info[GBKio_num].dp_y - input_w + 1 - cur_w;
					w_temp = gGBKio_Info[GBKio_num].zk_h;
					h_temp = input_w + cur_w;
				}
				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
									  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
									  Display_Layer, LCD_XSIZE_TFT, dx, dy,
									  0x0c, w_temp, h_temp);

				memset(Display_buff, 0, input_len);
				input_len = 0;
				input_w = 0;
			}
			else if (gKb_Info[Kb_num].Keyvalue == 0xF7)
			{
			} // move
			else if (gKb_Info[Kb_num].Keyvalue == 0xF8)
			{
			} // move
			else
			{
				if (gGBKio_Info[GBKio_num].p_len * 2 > input_len)
				{
					if (CapsLock == 1)
						Display_buff[input_len] = (gKb_Info[Kb_num].Keyvalue >> 8) & 0xFF;
					else
						Display_buff[input_len] = gKb_Info[Kb_num].Keyvalue & 0xFF;
					input_len++;

					if (Screen_DIR == 0)
					{
						dx = gGBKio_Info[GBKio_num].dp_x;
						dy = gGBKio_Info[GBKio_num].dp_y;
						w_temp = LCD_XSIZE_TFT - gGBKio_Info[GBKio_num].dp_x;
						h_temp = gGBKio_Info[GBKio_num].zk_h;
						dxs = gGBKio_Info[GBKio_num].dp_x;
						dys = gGBKio_Info[GBKio_num].dp_y;
						dxe = LCD_XSIZE_TFT;
						dye = gGBKio_Info[GBKio_num].dp_y + gGBKio_Info[GBKio_num].zk_h;
					}
					else if (Screen_DIR == 1)
					{
						dx = gGBKio_Info[GBKio_num].dp_x - gGBKio_Info[GBKio_num].zk_h + 1;
						dy = gGBKio_Info[GBKio_num].dp_y;
						w_temp = gGBKio_Info[GBKio_num].zk_h;
						h_temp = LCD_YSIZE_TFT;
						dxs = gGBKio_Info[GBKio_num].dp_x - gGBKio_Info[GBKio_num].zk_h + 1;
						dys = gGBKio_Info[GBKio_num].dp_y;
						dxe = gGBKio_Info[GBKio_num].dp_x;
						dye = LCD_YSIZE_TFT;
					}
					else if (Screen_DIR == 2)
					{
						dx = gGBKio_Info[GBKio_num].dp_x - input_w - gGBKio_Info[GBKio_num].zk_w + 1;
						dy = gGBKio_Info[GBKio_num].dp_y - gGBKio_Info[GBKio_num].zk_h;
						w_temp = input_w + gGBKio_Info[GBKio_num].zk_w;
						h_temp = gGBKio_Info[GBKio_num].zk_h;

						dxs = gGBKio_Info[GBKio_num].dp_x - w_temp;
						dys = gGBKio_Info[GBKio_num].dp_y - gGBKio_Info[GBKio_num].zk_h;
						dxe = gGBKio_Info[GBKio_num].dp_x;
						dye = gGBKio_Info[GBKio_num].dp_y;
					}
					else if (Screen_DIR == 3)
					{
						dx = gGBKio_Info[GBKio_num].dp_x;
						dy = 0;
						w_temp = gGBKio_Info[GBKio_num].zk_h;
						h_temp = gGBKio_Info[GBKio_num].dp_y + cur_w;

						dxs = gGBKio_Info[GBKio_num].dp_x;
						dys = 0;
						dxe = gGBKio_Info[GBKio_num].dp_x + gGBKio_Info[GBKio_num].zk_h;
						dye = gGBKio_Info[GBKio_num].dp_y;
					}

					LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
										  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
										  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
										  0x0c, w_temp, h_temp);

					LT_Print_zk_Font(1, gGBKio_Info[GBKio_num].zk_addr,
									 LAY_BUFF1, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].zk_w, gGBKio_Info[GBKio_num].zk_h,
									 3, gGBKio_Info[GBKio_num].fcolor, 0,
									 dxs, dys, dxe, dye, 0, 0, Display_buff, input_len);

					if (Screen_DIR == 0)
					{
						w_temp = data_w + cur_w;
					}
					else if (Screen_DIR == 1)
					{
						h_temp = data_w + 2 * cur_w;
					}
					else if (Screen_DIR == 2)
					{
						dx = gGBKio_Info[GBKio_num].dp_x - data_w + 1;
						w_temp = data_w;
					}
					else if (Screen_DIR == 3)
					{
						dx = gGBKio_Info[GBKio_num].dp_x;
						dy = gGBKio_Info[GBKio_num].dp_y - data_w + 1;
						h_temp = data_w;
					}

					LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
										  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
										  Display_Layer, LCD_XSIZE_TFT, dx, dy,
										  0x0c, w_temp, h_temp);

					input_w = data_w;
				}
			}
		}
		else // English input, pinyin has not been input
		{
			if (gKb_Info[Kb_num].Keyvalue == 0xF0) // Cancel
			{
				GBKioKb_flag = 0;
				Display_page(pic_id_temp);
			}
			else if (gKb_Info[Kb_num].Keyvalue == 0xF1) // Comfirm
			{
				for (uint16_t j = 0; j < input_len; j++)
				{
					var[gGBKio_Info[GBKio_num].p_add * 2 + j] = Display_buff[j];
				}

				var[gGBKio_Info[GBKio_num].p_add * 2 + input_len] = 0x00;
				var[gGBKio_Info[GBKio_num].p_add * 2 + input_len + 1] = 0x00;

				if (gGBKio_Info[GBKio_num].Code == 0xC00A) // Serial port feedback
				{
					rebuf[3] = 0x41;
					rebuf[4] = gGBKio_Info[GBKio_num].p_add >> 8;
					rebuf[5] = gGBKio_Info[GBKio_num].p_add & 0xFF;

					if (input_len != 0)
					{
						for (uint16_t j = 0; j < input_len; j++)
						{
							rebuf[6 + j] = var[gGBKio_Info[GBKio_num].p_add * 2 + j];
						}
						if (input_len % 2 == 0)
							LT_SendData_CRC_Frame(rebuf, 3 + input_len);
						else
						{
							rebuf[6 + input_len] = var[gGBKio_Info[GBKio_num].p_add * 2 + input_len];
							LT_SendData_CRC_Frame(rebuf, 3 + input_len + 1);
						}
					}
					else if (input_len == 0)
					{
						rebuf[6] = 0x00;
						rebuf[7] = 0x00;
						LT_SendData_CRC_Frame(rebuf, 5);
					}
				}
				if (Pop_up_flag == 0)
					Display_page(pic_id_temp);
				else if (Pop_up_flag == 1)
				{
					Display_Layer = LAY_BUFF7;
					Display_pop_up(Pop_up_id, Pop_up_x, Pop_up_y);
				}
			}
			else if (gKb_Info[Kb_num].Keyvalue == 0xF2) // Backspace
			{
			
				if (GBK_PY_length == 0)
				{
					if (input_len > 0)
					{
						for (l = 0; l < input_len;)
						{
							if (Display_buff[l] <= 0x7F)
							{
								l++;
								if (l == input_len)
								{
									Display_buff[input_len - 1] = 0x00;
									input_len--;
									break;
								}
							}
							else
							{
								l += 2;
								if (l == input_len)
								{
									Display_buff[input_len - 1] = 0x00;
									Display_buff[input_len - 2] = 0x00;
									input_len -= 2;
									break;
								}
							}
						}
						if (Screen_DIR == 0)
						{
							dx = gGBKio_Info[GBKio_num].dp_x;
							dy = gGBKio_Info[GBKio_num].dp_y;
							w_temp = LCD_XSIZE_TFT - gGBKio_Info[GBKio_num].dp_x;
							h_temp = gGBKio_Info[GBKio_num].zk_h;

							dxs = gGBKio_Info[GBKio_num].dp_x;
							dys = gGBKio_Info[GBKio_num].dp_y;
							dxe = LCD_XSIZE_TFT;
							dye = gGBKio_Info[GBKio_num].dp_y + gGBKio_Info[GBKio_num].zk_h;
						}
						else if (Screen_DIR == 1)
						{
							dx = gGBKio_Info[GBKio_num].dp_x - gGBKio_Info[GBKio_num].zk_h + 1;
							dy = gGBKio_Info[GBKio_num].dp_y;
							w_temp = gGBKio_Info[GBKio_num].zk_h;
							h_temp = LCD_YSIZE_TFT;

							dxs = dx;
							dys = dy;
							dxe = gGBKio_Info[GBKio_num].dp_x;
							dye = LCD_YSIZE_TFT;
						}
						else if (Screen_DIR == 2)
						{
							dx = gGBKio_Info[GBKio_num].dp_x - input_w - cur_w + 1;
							dy = gGBKio_Info[GBKio_num].dp_y - gGBKio_Info[GBKio_num].zk_h;
							w_temp = input_w + cur_w;
							h_temp = gGBKio_Info[GBKio_num].zk_h;

							dxs = gGBKio_Info[GBKio_num].dp_x - w_temp;
							dys = gGBKio_Info[GBKio_num].dp_y - gGBKio_Info[GBKio_num].zk_h;
							dxe = gGBKio_Info[GBKio_num].dp_x;
							dye = gGBKio_Info[GBKio_num].dp_y;
						}
						else if (Screen_DIR == 3)
						{
							dx = gGBKio_Info[GBKio_num].dp_x;
							dy = gGBKio_Info[GBKio_num].dp_y - input_w + 1 - cur_w;
							w_temp = gGBKio_Info[GBKio_num].zk_h;
							h_temp = input_w + cur_w;

							dxs = gGBKio_Info[GBKio_num].dp_x;
							dys = gGBKio_Info[GBKio_num].dp_y - input_w + 1 - cur_w;
							dxe = gGBKio_Info[GBKio_num].dp_x + gGBKio_Info[GBKio_num].zk_h;
							dye = gGBKio_Info[GBKio_num].dp_y;
						}

						LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
											  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
											  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
											  0x0c, w_temp, h_temp);

						LT_Print_zk_Font(1, gGBKio_Info[GBKio_num].zk_addr,
										 LAY_BUFF1, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].zk_w, gGBKio_Info[GBKio_num].zk_h,
										 3, gGBKio_Info[GBKio_num].fcolor, 0,
										 dxs, dys, dxe, dye, 0, 0, Display_buff, input_len);

						if (Screen_DIR == 0)
						{
							w_temp = input_w + cur_w;
							h_temp = gGBKio_Info[GBKio_num].zk_h;
						}
						else if (Screen_DIR == 1)
						{
							w_temp = gGBKio_Info[GBKio_num].zk_h;
							h_temp = input_w + cur_w;
						}
						LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
											  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
											  Display_Layer, LCD_XSIZE_TFT, dx, dy,
											  0x0c, w_temp, h_temp);

						input_w = data_w;
					}
				}
				else if (GBK_PY_length > 0)
				{
					GBK_PY_length--;
					GBK_linenum = 0;
					GBK_PY_buff[GBK_PY_length] = 0x00;
					Display_PY_GBK();
				}
			}
			else if (gKb_Info[Kb_num].Keyvalue == 0xF3) // Caps lock
			{
				if (CapsLock == 0)
					CapsLock = 1;
				else
					CapsLock = 0;
			}
			else if (gKb_Info[Kb_num].Keyvalue == 0xF4) // Chinese and English input switching
			{
				if (GBK_CN_EN == 0)
					GBK_CN_EN = 1;
				else
					GBK_CN_EN = 0; // Pinyin input
			}
			else if (gKb_Info[Kb_num].Keyvalue == 0xF5) // Clear display box of font
			{
				if (Screen_DIR == 0)
				{
					dx = gGBKio_Info[GBKio_num].dp_x;
					dy = gGBKio_Info[GBKio_num].dp_y;
					w_temp = input_w + cur_w;
					h_temp = gGBKio_Info[GBKio_num].zk_h;
				}
				else if (Screen_DIR == 1)
				{
					dx = gGBKio_Info[GBKio_num].dp_x - gGBKio_Info[GBKio_num].zk_h + 1;
					dy = gGBKio_Info[GBKio_num].dp_y;
					w_temp = gGBKio_Info[GBKio_num].zk_h;
					h_temp = input_w + cur_w;
				}
				else if (Screen_DIR == 2)
				{
					dx = gGBKio_Info[GBKio_num].dp_x - input_w - cur_w + 1;
					dy = gGBKio_Info[GBKio_num].dp_y - gGBKio_Info[GBKio_num].zk_h;
					w_temp = input_w + cur_w;
					h_temp = gGBKio_Info[GBKio_num].zk_h;
				}
				else if (Screen_DIR == 3)
				{
					dx = gGBKio_Info[GBKio_num].dp_x;
					dy = gGBKio_Info[GBKio_num].dp_y - input_w + 1 - cur_w;
					w_temp = gGBKio_Info[GBKio_num].zk_h;
					h_temp = input_w + cur_w;
				}
				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
									  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
									  Display_Layer, LCD_XSIZE_TFT, dx, dy,
									  0x0c, w_temp, h_temp);

				memset(Display_buff, 0, input_len);

				input_len = 0;
				input_w = 0;
			}
			else if (gKb_Info[Kb_num].Keyvalue == 0xF7) // move
			{
				if (GBK_PY_length > 0)
				{
					if (GBK_linenum > 0)
						GBK_linenum--;
					Display_PY_GBK();
				}
			}
			else if (gKb_Info[Kb_num].Keyvalue == 0xF8)
			{
				GBK_linenum++;
				Display_PY_GBK();
			}
			else
			{
				if (GBK_PY_length == 0 && ((gKb_Info[Kb_num].Keyvalue & 0x00FF) > 122 || (gKb_Info[Kb_num].Keyvalue & 0x00FF) < 97))
				{
					if (gGBKio_Info[GBKio_num].p_len * 2 > input_len)
					{
						Display_buff[input_len] = gKb_Info[Kb_num].Keyvalue & 0xFF;
						input_len++;

						if (Screen_DIR == 0)
						{
							dx = gGBKio_Info[GBKio_num].dp_x;
							dy = gGBKio_Info[GBKio_num].dp_y;
							w_temp = LCD_XSIZE_TFT - gGBKio_Info[GBKio_num].dp_x;
							h_temp = gGBKio_Info[GBKio_num].zk_h;
							dxs = gGBKio_Info[GBKio_num].dp_x;
							dys = gGBKio_Info[GBKio_num].dp_y;
							dxe = LCD_XSIZE_TFT;
							dye = gGBKio_Info[GBKio_num].dp_y + gGBKio_Info[GBKio_num].zk_h;
						}
						else if (Screen_DIR == 1)
						{
							dx = gGBKio_Info[GBKio_num].dp_x - gGBKio_Info[GBKio_num].zk_h + 1;
							dy = gGBKio_Info[GBKio_num].dp_y;
							w_temp = gGBKio_Info[GBKio_num].zk_h;
							h_temp = LCD_YSIZE_TFT;
							dxs = gGBKio_Info[GBKio_num].dp_x - gGBKio_Info[GBKio_num].zk_h + 1;
							dys = gGBKio_Info[GBKio_num].dp_y;
							dxe = gGBKio_Info[GBKio_num].dp_x;
							dye = LCD_YSIZE_TFT;
						}
						else if (Screen_DIR == 2)
						{
							dx = gGBKio_Info[GBKio_num].dp_x - input_w - gGBKio_Info[GBKio_num].zk_w + 1;
							dy = gGBKio_Info[GBKio_num].dp_y - gGBKio_Info[GBKio_num].zk_h;
							w_temp = input_w + gGBKio_Info[GBKio_num].zk_w;
							h_temp = gGBKio_Info[GBKio_num].zk_h;

							dxs = gGBKio_Info[GBKio_num].dp_x - w_temp;
							dys = gGBKio_Info[GBKio_num].dp_y - gGBKio_Info[GBKio_num].zk_h;
							dxe = gGBKio_Info[GBKio_num].dp_x;
							dye = gGBKio_Info[GBKio_num].dp_y;
						}
						else if (Screen_DIR == 3)
						{
							dx = gGBKio_Info[GBKio_num].dp_x;
							dy = 0;
							w_temp = gGBKio_Info[GBKio_num].zk_h;
							h_temp = gGBKio_Info[GBKio_num].dp_y + cur_w;

							dxs = gGBKio_Info[GBKio_num].dp_x;
							dys = 0;
							dxe = gGBKio_Info[GBKio_num].dp_x + gGBKio_Info[GBKio_num].zk_h;
							dye = gGBKio_Info[GBKio_num].dp_y;
						}

						LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
											  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
											  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
											  0x0c, w_temp, h_temp);

						LT_Print_zk_Font(1, gGBKio_Info[GBKio_num].zk_addr,
										 LAY_BUFF1, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].zk_w, gGBKio_Info[GBKio_num].zk_h,
										 3, gGBKio_Info[GBKio_num].fcolor, 0,
										 dxs, dys, dxe, dye, 0, 0, Display_buff, input_len);

						if (Screen_DIR == 0)
						{
							w_temp = data_w + cur_w;
							h_temp = gGBKio_Info[GBKio_num].zk_h;
						}
						else if (Screen_DIR == 1)
						{
							w_temp = gGBKio_Info[GBKio_num].zk_h;
							h_temp = data_w + cur_w;
						}
						else if (Screen_DIR == 2)
						{
							dx = gGBKio_Info[GBKio_num].dp_x - data_w + 1;
							w_temp = data_w;
						}
						else if (Screen_DIR == 3)
						{
							dx = gGBKio_Info[GBKio_num].dp_x;
							dy = gGBKio_Info[GBKio_num].dp_y - data_w + 1;
							w_temp = gGBKio_Info[GBKio_num].zk_h;
							h_temp = data_w;
						}
						LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
											  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
											  Display_Layer, LCD_XSIZE_TFT, dx, dy,
											  0x0c, w_temp, h_temp);
						input_w = data_w;
					}
				}
				else
				{
					GBK_linenum = 0;
					if (CapsLock == 0)
					{
						if ((gKb_Info[Kb_num].Keyvalue & 0x00FF) != 0x00 && ((gKb_Info[Kb_num].Keyvalue & 0x00FF) <= 122 || (gKb_Info[Kb_num].Keyvalue & 0x00FF) >= 97))
						{
							if (GBK_PY_length == 0)
							{
								GBK_PY_buff[GBK_PY_length] = gKb_Info[Kb_num].Keyvalue & 0x00FF;
								GBK_PY_length++;
								Display_PY_GBK();
							}
							else if (GBK_PY_length < 6 && GBK_PY_num > 0)
							{
								GBK_PY_buff[GBK_PY_length] = gKb_Info[Kb_num].Keyvalue & 0x00FF;
								GBK_PY_length++;
								Display_PY_GBK();
							}
						}
					}
				}
			}
		}
		Kb_flag = 0;
	}

	if (gTpInfo.sta == 0 && GBKioKb_flag == 1 && GBKio_flag == 0 && Kb_flag == 2)
	{
		if (Screen_DIR == 0)
		{
			dx = gGBKio_Info[GBKio_num].py_x;
			dy = gGBKio_Info[GBKio_num].py_y;
			w_temp = gGBKio_Info[GBKio_num].py_w * 6;
			h_temp = gGBKio_Info[GBKio_num].py_h;
		}
		else if (Screen_DIR == 1)
		{
			dx = gGBKio_Info[GBKio_num].py_x - gGBKio_Info[GBKio_num].py_h + 1;
			dy = gGBKio_Info[GBKio_num].py_y;
			w_temp = gGBKio_Info[GBKio_num].py_h;
			h_temp = gGBKio_Info[GBKio_num].py_w * 6;
		}
		else if (Screen_DIR == 2)
		{
			dx = gGBKio_Info[GBKio_num].py_x - gGBKio_Info[GBKio_num].py_w * 6 + 1;
			dy = gGBKio_Info[GBKio_num].py_y - gGBKio_Info[GBKio_num].py_h + 1;
			w_temp = gGBKio_Info[GBKio_num].py_w * 6;
			h_temp = gGBKio_Info[GBKio_num].py_h;
		}
		else if (Screen_DIR == 3)
		{
			dx = gGBKio_Info[GBKio_num].py_x;
			dy = gGBKio_Info[GBKio_num].py_y - gGBKio_Info[GBKio_num].py_w * 6;
			w_temp = gGBKio_Info[GBKio_num].py_h;
			h_temp = gGBKio_Info[GBKio_num].py_w * 6;
		}

		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
							  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
							  Display_Layer, LCD_XSIZE_TFT, dx, dy,
							  0x0c, w_temp, h_temp);

		if (Screen_DIR == 0)
		{
			dx = gGBKio_Info[GBKio_num].cn_x;
			dy = gGBKio_Info[GBKio_num].cn_y;
			w_temp = (gGBKio_Info[GBKio_num].py_w + gGBKio_Info[GBKio_num].spacing) * 8;
			h_temp = gGBKio_Info[GBKio_num].py_h;
		}
		else if (Screen_DIR == 1)
		{
			dx = gGBKio_Info[GBKio_num].cn_x - gGBKio_Info[GBKio_num].py_h;
			dy = gGBKio_Info[GBKio_num].cn_y;
			w_temp = gGBKio_Info[GBKio_num].py_h;
			h_temp = (gGBKio_Info[GBKio_num].py_w + gGBKio_Info[GBKio_num].spacing) * 8;
		}
		else if (Screen_DIR == 2)
		{
			dx = gGBKio_Info[GBKio_num].cn_x - (gGBKio_Info[GBKio_num].py_w + gGBKio_Info[GBKio_num].spacing) * 8 + 1;
			dy = gGBKio_Info[GBKio_num].cn_y - gGBKio_Info[GBKio_num].py_h + 1;
			w_temp = (gGBKio_Info[GBKio_num].py_w + gGBKio_Info[GBKio_num].spacing) * 8;
			h_temp = gGBKio_Info[GBKio_num].py_h;
		}
		else if (Screen_DIR == 3)
		{
			dx = gGBKio_Info[GBKio_num].cn_x;
			dy = gGBKio_Info[GBKio_num].cn_y - (gGBKio_Info[GBKio_num].py_w + gGBKio_Info[GBKio_num].spacing) * 8;
			w_temp = gGBKio_Info[GBKio_num].py_h;
			h_temp = (gGBKio_Info[GBKio_num].py_w + gGBKio_Info[GBKio_num].spacing) * 8;
		}

		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
							  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
							  Display_Layer, LCD_XSIZE_TFT, dx, dy,
							  0x0c, w_temp, h_temp);

		if (Screen_DIR == 0)
		{
			dx = gGBKio_Info[GBKio_num].dp_x;
			dy = gGBKio_Info[GBKio_num].dp_y;
			w_temp = LCD_XSIZE_TFT - gGBKio_Info[GBKio_num].dp_x;
			h_temp = gGBKio_Info[GBKio_num].zk_h;
			dxs = gGBKio_Info[GBKio_num].dp_x;
			dys = gGBKio_Info[GBKio_num].dp_y;
			dxe = LCD_XSIZE_TFT;
			dye = gGBKio_Info[GBKio_num].dp_y + gGBKio_Info[GBKio_num].zk_h;
		}
		else if (Screen_DIR == 1)
		{
			dx = gGBKio_Info[GBKio_num].dp_x - gGBKio_Info[GBKio_num].zk_h + 1;
			dy = gGBKio_Info[GBKio_num].dp_y;
			w_temp = gGBKio_Info[GBKio_num].zk_h;
			h_temp = LCD_YSIZE_TFT;
			dxs = gGBKio_Info[GBKio_num].dp_x - gGBKio_Info[GBKio_num].zk_h + 1;
			dys = gGBKio_Info[GBKio_num].dp_y;
			dxe = gGBKio_Info[GBKio_num].dp_x;
			dye = LCD_YSIZE_TFT;
		}
		else if (Screen_DIR == 2)
		{
			dx = gGBKio_Info[GBKio_num].dp_x - input_w - gGBKio_Info[GBKio_num].zk_w + 1;
			dy = gGBKio_Info[GBKio_num].dp_y - gGBKio_Info[GBKio_num].zk_h;
			w_temp = input_w + gGBKio_Info[GBKio_num].zk_w;
			h_temp = gGBKio_Info[GBKio_num].zk_h;

			dxs = gGBKio_Info[GBKio_num].dp_x - w_temp;
			dys = gGBKio_Info[GBKio_num].dp_y - gGBKio_Info[GBKio_num].zk_h;
			dxe = gGBKio_Info[GBKio_num].dp_x;
			dye = gGBKio_Info[GBKio_num].dp_y;
		}
		else if (Screen_DIR == 3)
		{
			dx = gGBKio_Info[GBKio_num].dp_x;
			dy = 0;
			w_temp = gGBKio_Info[GBKio_num].zk_h;
			h_temp = gGBKio_Info[GBKio_num].dp_y + cur_w;

			dxs = gGBKio_Info[GBKio_num].dp_x;
			dys = 0;
			dxe = gGBKio_Info[GBKio_num].dp_x + gGBKio_Info[GBKio_num].zk_h;
			dye = gGBKio_Info[GBKio_num].dp_y;
		}

		LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
							  MAIN_BUFF, LCD_XSIZE_TFT, dx, dy,
							  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
							  0x0c, w_temp, h_temp);

		LT_Print_zk_Font(1, gGBKio_Info[GBKio_num].zk_addr,
						 LAY_BUFF1, LCD_XSIZE_TFT, gGBKio_Info[GBKio_num].zk_w, gGBKio_Info[GBKio_num].zk_h,
						 3, gGBKio_Info[GBKio_num].fcolor, 0,
						 dxs, dys, dxe, dye, 0, 0, Display_buff, input_len);

		if (Screen_DIR == 0)
		{
			w_temp = data_w + cur_w;
			h_temp = gGBKio_Info[GBKio_num].zk_h;
		}
		else if (Screen_DIR == 1)
		{
			w_temp = gGBKio_Info[GBKio_num].zk_h;
			h_temp = data_w + cur_w;
		}
		else if (Screen_DIR == 2)
		{
			dx = gGBKio_Info[GBKio_num].dp_x - data_w + 1;
			w_temp = data_w;
		}
		else if (Screen_DIR == 3)
		{
			dx = gGBKio_Info[GBKio_num].dp_x;
			dy = gGBKio_Info[GBKio_num].dp_y - data_w + 1;
			w_temp = gGBKio_Info[GBKio_num].zk_h;
			h_temp = data_w;
		}
		LT768_BTE_Memory_Copy(LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
							  LAY_BUFF1, LCD_XSIZE_TFT, dx, dy,
							  Display_Layer, LCD_XSIZE_TFT, dx, dy,
							  0x0c, w_temp, h_temp);

		input_w = data_w;
		Kb_flag = 0;
	}
}
/***********************/

/***********************************************************************************
 * Function Name	: VarKey_touch
 * Description		: Touch detection of multivariable adjustment.
 * Input			: None
 * Output       	: None
 * Return       	: None
 ************************************************************************************/
void VarKey_touch(void)
{
	uint8_t i = 0;
	uint8_t rebuf[50];
	uint8_t page_flag = 0;
	if (gTpInfo.sta) // press down
	{
		for (i = 0; i < VarKey_count; i++)
		{
			if (gTpInfo.x[0] >= gVarKey_Info[i].Xs && gTpInfo.y[0] >= gVarKey_Info[i].Ys &&
				gTpInfo.x[0] <= gVarKey_Info[i].Xe && gTpInfo.y[0] <= gVarKey_Info[i].Ye &&
				VarKey_flag == 0 && button_Press_flag == 0)
			{
				if (gVarKey_Info[i].press_id != 0xFFFF)
					Display_Icon(gVarKey_Info[i].press_id, gVarKey_Info[i].Xs, gVarKey_Info[i].Ys, 0);

				VarKey_num = i;
				VarKey_flag = 1;
				button_Press_flag = 1;
				if (buzzer)
					touch_buzzer();
			}
		}
	}
	if (Button_Key_Code_Flag == 1)
	{
		for (i = 0; i < VarKey_count; i++)
		{
			if (gVarKey_Info[i].Xs == 0xFFFF && gVarKey_Info[i].Ys ==  Button_Key_Code)
			{
				Button_Key_Code_Flag = 0;
				Button_Key_Code = 0;
				VarKey_num = i;
				for (i = 0; i < 8; i++)
				{
					if (gVarKey_Info[VarKey_num].var_addr[i] != 0xFFFF && gVarKey_Info[VarKey_num].var_addr[i] < 0x7200)
					{
						var[gVarKey_Info[VarKey_num].var_addr[i] * 2] = (gVarKey_Info[VarKey_num].var[i] >> 8) & 0xFF;
						var[gVarKey_Info[VarKey_num].var_addr[i] * 2 + 1] = gVarKey_Info[VarKey_num].var[i] & 0xFF;

						if (gVarKey_Info[VarKey_num].var_addr[i] > 0x7000)
							reg_operation(gVarKey_Info[VarKey_num].var_addr[i]);
						
						else if (gVarKey_Info[VarKey_num].var_addr[i] == 0x7000)
							page_flag = 1;
					}
				}
				if (page_flag == 1)
					reg_operation(0x7000);
				Get_date_variable_Id_Regular_update(pic_id);

				if (gVarKey_Info[VarKey_num].Code == 0xC00B)
				{
					rebuf[3] = 0x41;
					for (i = 0; i < 8; i++)
					{
						rebuf[4 * i + 4] = gVarKey_Info[VarKey_num].var_addr[i] >> 8;
						rebuf[4 * i + 5] = gVarKey_Info[VarKey_num].var_addr[i] & 0xFF;
						rebuf[4 * i + 6] = gVarKey_Info[VarKey_num].var[i] >> 8;
						rebuf[4 * i + 7] = gVarKey_Info[VarKey_num].var[i] & 0xFF;
					}
					LT_SendData_CRC_Frame(rebuf, 33);
				}

				if (gVarKey_Info[VarKey_num].Next_id != 0xFFFF)
					Display_page(gVarKey_Info[VarKey_num].Next_id);
			}
		}
	}
	if (gTpInfo.sta == 0 && VarKey_flag == 1)
	{
		if (gVarKey_Info[VarKey_num].id != 0xFFFF)
			Display_Icon(gVarKey_Info[VarKey_num].id, gVarKey_Info[VarKey_num].Xs, gVarKey_Info[VarKey_num].Ys, 0);

		button_Press_flag = 0;
		VarKey_flag = 0;

		for (i = 0; i < 8; i++)
		{
			if (gVarKey_Info[VarKey_num].var_addr[i] != 0xFFFF && gVarKey_Info[VarKey_num].var_addr[i] < 0x7200)
			{
				var[gVarKey_Info[VarKey_num].var_addr[i] * 2] = (gVarKey_Info[VarKey_num].var[i] >> 8) & 0xFF;
				var[gVarKey_Info[VarKey_num].var_addr[i] * 2 + 1] = gVarKey_Info[VarKey_num].var[i] & 0xFF;
				if (gVarKey_Info[VarKey_num].var_addr[i] > 0x7000)
					reg_operation(gVarKey_Info[VarKey_num].var_addr[i]);
				else if (gVarKey_Info[VarKey_num].var_addr[i] == 0x7000)
					page_flag = 1;
			}
		}
		if (page_flag == 1)
			reg_operation(0x7000);
		Get_date_variable_Id_Regular_update(pic_id);

		if (gVarKey_Info[VarKey_num].Code == 0xC00B)
		{
			rebuf[3] = 0x41;
			for (i = 0; i < 8; i++)
			{
				rebuf[4 * i + 4] = gVarKey_Info[VarKey_num].var_addr[i] >> 8;
				rebuf[4 * i + 5] = gVarKey_Info[VarKey_num].var_addr[i] & 0xFF;
				rebuf[4 * i + 6] = gVarKey_Info[VarKey_num].var[i] >> 8;
				rebuf[4 * i + 7] = gVarKey_Info[VarKey_num].var[i] & 0xFF;
			}
			LT_SendData_CRC_Frame(rebuf, 33);
		}

		if (gVarKey_Info[VarKey_num].Next_id != 0xFFFF)
			Display_page(gVarKey_Info[VarKey_num].Next_id);
	}
}
/*******************************/

/***********************************************************************************
 * Function Name	: pop_up_touch
 * Description		: Touch detection of pop-up window.
 * Input			: None
 * Output			: None
 * Return			: None
 ************************************************************************************/
void pop_up_touch(void)
{
	uint8_t i = 0;
	uint8_t rebuf[20];
	uint16_t w, h;

	if (gTpInfo.sta) // press down
	{
		for (i = 0; i < Pop_up_count; i++)
		{
			if (gTpInfo.x[0] >= gPop_up_Info[i].Xs && gTpInfo.y[0] >= gPop_up_Info[i].Ys &&
				gTpInfo.x[0] <= gPop_up_Info[i].Xe && gTpInfo.y[0] <= gPop_up_Info[i].Ye &&
				Pop_up_press == 0 && button_Press_flag == 0)
			{
				if (gPop_up_Info[i].press_id != 0xFFFF)
					Display_Icon(gPop_up_Info[i].press_id, gPop_up_Info[i].Xs, gPop_up_Info[i].Ys, 0);

				if (Pop_up_flag == 0)
				{
					Pop_UI_rotate_id = pic_id;
					if ((gPop_up_Info[i].function & 0x01) == 1 && Pop_bgflag == 0)
					{
						Canvas_Image_Start_address(LAY_BUFF1);
						Canvas_image_width(LCD_XSIZE_TFT);
						LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT - 1, LCD_YSIZE_TFT - 1, Black);
						BTE_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, 0, 0,
										   LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
										   LAY_BUFF8, LCD_XSIZE_TFT, 0, 0,
										   LCD_XSIZE_TFT, LCD_YSIZE_TFT, 25);
					}
					else
					{
						LT768_BTE_Memory_Copy(Display_Layer, LCD_XSIZE_TFT, 0, 0,
											  Display_Layer, LCD_XSIZE_TFT, 0, 0,
											  LAY_BUFF8, LCD_XSIZE_TFT, 0, 0,
											  0x0c, LCD_XSIZE_TFT, LCD_YSIZE_TFT);
					}
				}
				Pop_up_num = i;
				Pop_up_press = 1;
//				Pop_up_flag = 1;
				button_Press_flag = 1;
				if (buzzer)
					touch_buzzer();
			}
		}
	}
	if (Button_Key_Code_Flag == 1)
	{
		for (i = 0; i < Pop_up_count; i++)
		{
			if (gPop_up_Info[i].Xs == 0xFFFF && Button_Key_Code == gPop_up_Info[i].Ys)
			{
				Button_Key_Code_Flag = 0;
				Button_Key_Code = 0;
				if (Pop_up_flag == 0)
				{
					Pop_UI_rotate_id = pic_id;
					if ((gPop_up_Info[i].function & 0x01) == 1 && Pop_bgflag == 0)
					{
						Canvas_Image_Start_address(LAY_BUFF1);
						Canvas_image_width(LCD_XSIZE_TFT);
						LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT - 1, LCD_YSIZE_TFT - 1, Black);
						BTE_Alpha_Blending(Display_Layer, LCD_XSIZE_TFT, 0, 0,
										   LAY_BUFF1, LCD_XSIZE_TFT, 0, 0,
										   LAY_BUFF8, LCD_XSIZE_TFT, 0, 0,
										   LCD_XSIZE_TFT, LCD_YSIZE_TFT, 25);
					}
					else
					{
						LT768_BTE_Memory_Copy(Display_Layer, LCD_XSIZE_TFT, 0, 0,
											  Display_Layer, LCD_XSIZE_TFT, 0, 0,
											  LAY_BUFF8, LCD_XSIZE_TFT, 0, 0,
											  0x0c, LCD_XSIZE_TFT, LCD_YSIZE_TFT);
					}
				}
				Pop_up_num = i;
				Pop_up_press = 1;
//				Pop_up_flag = 1;
				button_Press_flag = 1;
			}

		}
	}
	if (gTpInfo.sta == 0 && Pop_up_press == 1)
	{
		if (gPop_up_Info[Pop_up_num].Code == 0xC008)
		{
			rebuf[3] = 0x41;
			rebuf[4] = 0xFF;
			rebuf[5] = 0xFF;
			rebuf[6] = gPop_up_Info[Pop_up_num].Keyvalue >> 8;
			rebuf[7] = gPop_up_Info[Pop_up_num].Keyvalue & 0xFF;
			LT_SendData_CRC_Frame(rebuf, 5);
		}

		if (gPop_up_Info[Pop_up_num].id != 0xFFFF)
			Display_Icon(gPop_up_Info[Pop_up_num].id, gPop_up_Info[Pop_up_num].Xs, gPop_up_Info[Pop_up_num].Ys, 0);

		if (gPop_up_Info[Pop_up_num].function & 0x02)
		{
			Flash_Read_UI(rebuf, addr_index[6] + 12 * Pop_up_id, 12);
			w = rebuf[4] + (rebuf[5] << 8);
			h = rebuf[6] + (rebuf[7] << 8);

			LT768_BTE_Memory_Copy(LAY_BUFF8, LCD_XSIZE_TFT, Pop_up_x, Pop_up_y,
								  LAY_BUFF8, LCD_XSIZE_TFT, Pop_up_x, Pop_up_y,
								  Display_Layer, LCD_XSIZE_TFT, Pop_up_x, Pop_up_y,
								  0x0c, w, h);

			LT768_BTE_Memory_Copy(LAY_BUFF8, LCD_XSIZE_TFT, Pop_up_x, Pop_up_y,
								  LAY_BUFF8, LCD_XSIZE_TFT, Pop_up_x, Pop_up_y,
								  MAIN_BUFF, LCD_XSIZE_TFT, Pop_up_x, Pop_up_y,
								  0x0c, w, h);
		}
		Pop_up_flag = 1;
		Pop_up_x = gPop_up_Info[Pop_up_num].pop_up_x;
		Pop_up_y = gPop_up_Info[Pop_up_num].pop_up_y;
		Pop_up_id = gPop_up_Info[Pop_up_num].pop_up_id;
	
		if (gPop_up_Info[Pop_up_num].pop_up_id != 0xFFFF)
			Display_pop_up(gPop_up_Info[Pop_up_num].pop_up_id, gPop_up_Info[Pop_up_num].pop_up_x, gPop_up_Info[Pop_up_num].pop_up_y);

		Pop_up_press = 0;
		button_Press_flag = 0;
	}
}
/******************************************/

uint8_t gFirstTouch = 0;

uint8_t Ges_sc_flag = 0;
uint16_t GeslastXY = 0;
uint16_t Gespos = 0;
uint16_t last_Gespos = 0;

/***********************************************************************************
 * Function Name	: slide_screen
 * Description		: Touch sliding screen display.
 * Input			: None
 * Output			: None
 * Return			: None
 ************************************************************************************/
void slide_screen(void)
{
	uint8_t rebuf[20];
	uint16_t i;
	uint16_t pos;
	int16_t df = 0;

	if (gTpInfo.sta)
	{
		if (gFirstTouch == 0) // First press
		{
			if (Screen_DIR == 0 || Screen_DIR == 2)
			{
				Gespos = LCD_XSIZE_TFT;
				last_Gespos = LCD_XSIZE_TFT;
				if (GeslastXY == 0)
					GeslastXY = gTpInfo.x[0];

				df = GeslastXY - gTpInfo.x[0];
	
				DelayMS(1);
			}
			else if (Screen_DIR == 3 || Screen_DIR == 1)
			{
				Gespos = LCD_YSIZE_TFT;
				last_Gespos = LCD_YSIZE_TFT;
				if (GeslastXY == 0)
					GeslastXY = gTpInfo.y[0];

				df = GeslastXY - gTpInfo.y[0];
			}

			if ((df > 25) || (df < -25))
			{

				gFirstTouch = 1;

				button_Press_flag = 1;
				Basci_flag = 0;
				Adj_flag = 0;
				Slide_flag = 0;
				Dio_flag = 0;
				AioKb_flag = 0;
				Kb_flag = 0;
				VarKey_flag = 0;
				Pop_up_flag = 0;
//				sldmenu_flag = 0;
				GBKioKb_flag = 0;

				if (TFT_bitcolor == 0)
					Select_PIP1_Window_16bpp();
				else if (TFT_bitcolor == 1)
					Select_PIP1_Window_24bpp();
				Select_PIP1_Parameter();
				
				if (Screen_DIR == 0 || Screen_DIR == 2)
				{
					GeslastXY = gTpInfo.x[0];
					PIP_Image_Start_Address(LAY_BUFF12);
					PIP_Image_Width(LCD_XSIZE_TFT * 3);
					PIP_Display_Start_XY(0, gSld_sc_Info.ys);

					LT768_BTE_Memory_Copy(0, LCD_XSIZE_TFT, 0, gSld_sc_Info.ys,
										  0, LCD_XSIZE_TFT, 0, gSld_sc_Info.ys,
										  LAY_BUFF12, LCD_XSIZE_TFT * 3, LCD_XSIZE_TFT, gSld_sc_Info.ys,
										  0x0c, LCD_XSIZE_TFT, gSld_sc_Info.ye - gSld_sc_Info.ys + 1);

					PIP_Window_Image_Start_XY(LCD_XSIZE_TFT, gSld_sc_Info.ys);
					PIP_Window_Width_Height(LCD_XSIZE_TFT, gSld_sc_Info.ye - gSld_sc_Info.ys + 1);
				}
				else if (Screen_DIR == 3 || Screen_DIR == 1)
				{
					GeslastXY = gTpInfo.y[0];
					PIP_Image_Start_Address(LAY_BUFF12);
					PIP_Image_Width(LCD_XSIZE_TFT);
					PIP_Display_Start_XY(gSld_sc_Info.ys, 0);

					LT768_BTE_Memory_Copy(0, LCD_XSIZE_TFT, gSld_sc_Info.ys, 0,
										  0, LCD_XSIZE_TFT, gSld_sc_Info.ys, 0,
										  LAY_BUFF12, LCD_XSIZE_TFT, gSld_sc_Info.ys, LCD_YSIZE_TFT,
										  0x0c, gSld_sc_Info.ye - gSld_sc_Info.ys + 1, LCD_YSIZE_TFT);

					PIP_Window_Image_Start_XY(gSld_sc_Info.ys, LCD_YSIZE_TFT);
					PIP_Window_Width_Height(gSld_sc_Info.ye - gSld_sc_Info.ys + 1, LCD_YSIZE_TFT);
				}
				Enable_PIP1();
			}
		}
		else
		{
			if (Screen_DIR == 0 || Screen_DIR == 2)
			{
				df = GeslastXY - gTpInfo.x[0];
			
				if (df != 0)
				{
					GeslastXY = gTpInfo.x[0];
					Gespos += df;
	
					if (gSld_sc_Info.page[0] == 0xFFFF)
					{
						if (Gespos > LCD_XSIZE_TFT)
							Gespos = LCD_XSIZE_TFT;
					}
					if (gSld_sc_Info.page[1] == 0xFFFF)
					{
						if (Gespos < LCD_XSIZE_TFT)
							Gespos = LCD_XSIZE_TFT;
					}
					pos = Gespos - Gespos % 4;

					if (pos != last_Gespos)
					{
						if (pos > last_Gespos)
						{
							for (i = 4; i <= pos - last_Gespos; i += 4)
							{
								PIP_Window_Image_Start_XY(last_Gespos + i, gSld_sc_Info.ys);
								DelayUS(400);
							}
						}
						else
						{
							for (i = 4; i <= last_Gespos - pos; i += 4)
							{
								PIP_Window_Image_Start_XY(last_Gespos - i, gSld_sc_Info.ys);
								DelayUS(400);
							}
						}
					}
					
					last_Gespos = pos;
				}
			}
			else if (Screen_DIR == 3 || Screen_DIR == 1)
			{
				df = GeslastXY - gTpInfo.y[0];
				if (df != 0)
				{
					GeslastXY = gTpInfo.y[0];
					Gespos += df;

					if (gSld_sc_Info.page[0] == 0xFFFF)
					{
						if (Gespos > LCD_YSIZE_TFT)
							Gespos = LCD_YSIZE_TFT;
					}
					if (gSld_sc_Info.page[1] == 0xFFFF)
					{
						if (Gespos < LCD_YSIZE_TFT)
							Gespos = LCD_YSIZE_TFT;
					}
					pos = Gespos;

					if (pos != last_Gespos)
					{
						if (pos > last_Gespos)
						{
							for (i = 1; i <= pos - last_Gespos; i++)
							{
								PIP_Window_Image_Start_XY(gSld_sc_Info.ys, last_Gespos + i);
								DelayUS(50);
							}
						}
						else
						{
							for (i = 1; i <= last_Gespos - pos; i++)
							{
								PIP_Window_Image_Start_XY(gSld_sc_Info.ys, last_Gespos - i);
								DelayUS(50);
							}
						}
					}
					last_Gespos = pos;
				}
			}
		}
	}
	else if (gFirstTouch == 1)
	{
		gFirstTouch = 0;
		Basci_flag = 0;
		Adj_flag = 0;
		Slide_flag = 0;
		Dio_flag = 0;
		AioKb_flag = 0;
		GBKioKb_flag = 0;
		Kb_flag = 0;
		sldmenu_flag = 0;
		VarKey_flag = 0;
		Pop_up_flag = 0;
		button_Press_flag = 0;

		if (Screen_DIR == 0 || Screen_DIR == 2)
		{
			if (Gespos > (LCD_XSIZE_TFT + LCD_XSIZE_TFT * 3 / 10)) // Turn the page
			{
				for (i = 0; i < LCD_XSIZE_TFT * 2 - last_Gespos; i += 4)
				{
					WDT_FeedDog(); //Clear watchdog
					PIP_Window_Image_Start_XY(last_Gespos + i, gSld_sc_Info.ys);
					DelayUS(1500);
				}
				LT768_BTE_Memory_Copy(LAY_BUFF12, LCD_XSIZE_TFT * 3, LCD_XSIZE_TFT * 2, gSld_sc_Info.ys,
									  LAY_BUFF12, LCD_XSIZE_TFT * 3, LCD_XSIZE_TFT * 2, gSld_sc_Info.ys,
									  LAY_BUFF12, LCD_XSIZE_TFT * 3, LCD_XSIZE_TFT, gSld_sc_Info.ys,
									  0x0c, LCD_XSIZE_TFT, gSld_sc_Info.ye - gSld_sc_Info.ys + 1);

				Gespos = LCD_XSIZE_TFT;
				PIP_Window_Image_Start_XY(Gespos, gSld_sc_Info.ys);

				if (gSld_sc_Info.Code == 0xC00C)
				{
					rebuf[3] = 0x41;
					rebuf[4] = 0xFF;
					rebuf[5] = 0xFF;
					rebuf[6] = (gSld_sc_Info.value[0] >> 8) & 0xFF;
					rebuf[7] = gSld_sc_Info.value[0] & 0xFF;
					LT_SendData_CRC_Frame(rebuf, 5);
				}

				Display_page(gSld_sc_Info.page[0]);
				Disable_PIP1();
				GeslastXY = 0;
			}
			else if (Gespos < LCD_XSIZE_TFT * 7 / 10) 				// Turn the page
			{

				for (i = 0; i < last_Gespos; i += 4)
				{
					WDT_FeedDog(); //Clear watchdog
					PIP_Window_Image_Start_XY(last_Gespos - i, gSld_sc_Info.ys);
					DelayUS(1500);
				}
				LT768_BTE_Memory_Copy(LAY_BUFF12, LCD_XSIZE_TFT * 3, 0, gSld_sc_Info.ys,
									  LAY_BUFF12, LCD_XSIZE_TFT * 3, 0, gSld_sc_Info.ys,
									  LAY_BUFF12, LCD_XSIZE_TFT * 3, LCD_XSIZE_TFT, gSld_sc_Info.ys,
									  0x0c, LCD_XSIZE_TFT, gSld_sc_Info.ye - gSld_sc_Info.ys + 1);

				Gespos = LCD_XSIZE_TFT;
				PIP_Window_Image_Start_XY(Gespos, gSld_sc_Info.ys);

				if (gSld_sc_Info.Code == 0xC00C)
				{
					rebuf[3] = 0x41;
					rebuf[4] = 0xFF;
					rebuf[5] = 0xFF;
					rebuf[6] = (gSld_sc_Info.value[1] >> 8) & 0xFF;
					rebuf[7] = gSld_sc_Info.value[1] & 0xFF;
					LT_SendData_CRC_Frame(rebuf, 5);
				}
				Display_page(gSld_sc_Info.page[1]);
				Disable_PIP1();
				GeslastXY = 0;
			}
			else if (last_Gespos != LCD_XSIZE_TFT)
			{
				if (last_Gespos > LCD_XSIZE_TFT)
				{

					for (i = 0; i < last_Gespos - LCD_XSIZE_TFT; i += 4)
					{
						WDT_FeedDog(); //Clear watchdog
						PIP_Window_Image_Start_XY(last_Gespos - i, gSld_sc_Info.ys);
						DelayUS(1500);
					}
					GeslastXY = 0;
					Gespos = LCD_XSIZE_TFT;
					Disable_PIP1();
				}
				else
				{
					for (i = 0; i < LCD_XSIZE_TFT - last_Gespos; i += 4)
					{
						WDT_FeedDog(); //Clear watchdog
						PIP_Window_Image_Start_XY(last_Gespos + i, gSld_sc_Info.ys);
						DelayUS(1500);
					}
					GeslastXY = 0;
					Gespos = LCD_XSIZE_TFT;
					Disable_PIP1();
				}
			}
			else
				Disable_PIP1();
		}
		else if (Screen_DIR == 3 || Screen_DIR == 1)
		{
			if (Gespos > (LCD_YSIZE_TFT + LCD_YSIZE_TFT * 3 / 10)) 	// Turn the page
			{
				for (i = 0; i < LCD_YSIZE_TFT * 2 - last_Gespos; i++)
				{
					WDT_FeedDog(); //Clear watchdog
					PIP_Window_Image_Start_XY(gSld_sc_Info.ys, last_Gespos + i);
					DelayUS(300);
				}
				LT768_BTE_Memory_Copy(LAY_BUFF12, LCD_XSIZE_TFT, gSld_sc_Info.ys, LCD_YSIZE_TFT * 2,
									  LAY_BUFF12, LCD_XSIZE_TFT, gSld_sc_Info.ys, LCD_YSIZE_TFT * 2,
									  LAY_BUFF12, LCD_XSIZE_TFT, gSld_sc_Info.ys, LCD_YSIZE_TFT,
									  0x0c, gSld_sc_Info.ye - gSld_sc_Info.ys + 1, LCD_YSIZE_TFT);

				Gespos = LCD_YSIZE_TFT;
				PIP_Window_Image_Start_XY(gSld_sc_Info.ys, Gespos);

				if (gSld_sc_Info.Code == 0xC00C)
				{
					rebuf[3] = 0x41;
					rebuf[4] = 0xFF;
					rebuf[5] = 0xFF;
					rebuf[6] = (gSld_sc_Info.value[0] >> 8) & 0xFF;
					rebuf[7] = gSld_sc_Info.value[0] & 0xFF;
					LT_SendData_CRC_Frame(rebuf, 5);
				}

				Display_page(gSld_sc_Info.page[0]);
				Disable_PIP1();
				GeslastXY = 0;
			}
			else if (Gespos < LCD_YSIZE_TFT * 7 / 10) 				// Turn the page
			{
				for (i = 0; i < last_Gespos; i++)
				{
					WDT_FeedDog(); //Clear watchdog
					PIP_Window_Image_Start_XY(gSld_sc_Info.ys, last_Gespos - i);
					DelayUS(300);
				}
				LT768_BTE_Memory_Copy(LAY_BUFF12, LCD_XSIZE_TFT, gSld_sc_Info.ys, 0,
									  LAY_BUFF12, LCD_XSIZE_TFT, gSld_sc_Info.ys, 0,
									  LAY_BUFF12, LCD_XSIZE_TFT, gSld_sc_Info.ys, LCD_YSIZE_TFT,
									  0x0c, gSld_sc_Info.ye - gSld_sc_Info.ys + 1, LCD_YSIZE_TFT);

				Gespos = LCD_YSIZE_TFT;
				PIP_Window_Image_Start_XY(gSld_sc_Info.ys, Gespos);

				if (gSld_sc_Info.Code == 0xC00C)
				{
					rebuf[3] = 0x41;
					rebuf[4] = 0xFF;
					rebuf[5] = 0xFF;
					rebuf[6] = (gSld_sc_Info.value[1] >> 8) & 0xFF;
					rebuf[7] = gSld_sc_Info.value[1] & 0xFF;
					LT_SendData_CRC_Frame(rebuf, 5);
				}
				Display_page(gSld_sc_Info.page[1]);
				Disable_PIP1();
				GeslastXY = 0;
			}
			else if (last_Gespos != LCD_YSIZE_TFT)
			{
				if (last_Gespos > LCD_YSIZE_TFT)
				{
					for (i = 0; i < last_Gespos - LCD_YSIZE_TFT; i++)
					{
						WDT_FeedDog(); //Clear watchdog
						PIP_Window_Image_Start_XY(gSld_sc_Info.ys, last_Gespos - i);
						DelayUS(300);
					}
					GeslastXY = 0;
					Gespos = LCD_YSIZE_TFT;
					Disable_PIP1();
				}
				else
				{
					for (i = 0; i < LCD_YSIZE_TFT - last_Gespos; i++)
					{
						WDT_FeedDog(); //Clear watchdog
						PIP_Window_Image_Start_XY(gSld_sc_Info.ys, last_Gespos + i);
						DelayUS(300);
					}
					GeslastXY = 0;
					Gespos = LCD_YSIZE_TFT;
					Disable_PIP1();
				}
			}
			else
				Disable_PIP1();
		}
	}
	else if (GeslastXY != 0)
		GeslastXY = 0;
}
/***********************/


void get_encoder(uint8_t type, uint8_t *buff, uint16_t addr)
{
	uint16_t i;
	uint16_t len, len1;

	len = buff[4] + (buff[5] << 8);
	len1 = 8;
	Check_box_add = buff[6] + (buff[7] << 8);
	
	for (i = 0; i <= 255; i++)
	{
		gEncoder_addr[i] = len1 + addr;
		len1 = len1 + buff[len1 + 1] + 2;
		if (len1 - 6 >= len)
			break;
	}

	gEncoderFlag = 1;

	gEncoder_num = (var[Check_box_add * 2] << 8) + var[Check_box_add * 2 + 1];
	if (gEncoder_num > i)
		gEncoder_num = i;
	gEncoder_count = i;
}
#if encoder_on
/***********************************************************************************
* Function Name	: Display_encoder
* Description	: Run encoder.
* Input			: - type: encoder info sources
				  - buff: encoder info
* Output		: None
* Return		: None
************************************************************************************/
void Display_encoder(uint8_t *buff, uint8_t type)
{
	uint8_t i, len, loop;
	uint8_t rebuf[50];
	uint8_t page_flag = 0;
	uint16_t p_add = 0, step;
	int16_t sum = 0, V_max, V_min;

	len = buff[1];

	if (type == 4 || type == 0x84)
	{
		for (i = 0; i < 32; i += 4)
		{
			if ((buff[i + 2] + (buff[i + 2 + 1] << 8)) != 0xFFFF &&
				(buff[i + 2] + (buff[i + 2 + 1] << 8)) < 0x7200)
			{

				var[(buff[i + 2] + (buff[i + 2 + 1] << 8)) * 2] = buff[i + 2 + 3];
				var[(buff[i + 2] + (buff[i + 2 + 1] << 8)) * 2 + 1] = buff[i + 2 + 2];
				
				if ((buff[i + 2] + (buff[i + 2 + 1] << 8)) > 0x7000)
					reg_operation(buff[i + 2 ] + (buff[i + 2 + 1] << 8));
				else if ((buff[i + 2] + (buff[i + 2 + 1] << 8)) == 0x7000)
					page_flag = 1;
			}
			
			if (page_flag == 1)
				reg_operation(0x7000);
			
		}
		if (type == 0x84)
		{
			rebuf[3] = 0x41;
			for (i = 0; i < 8; i++)
			{
				rebuf[4 * i + 4] = buff[4 * i + 2 + 1];
				rebuf[4 * i + 5] = buff[4 * i + 2];
				rebuf[4 * i + 6] = buff[4 * i + 2 + 3];
				rebuf[4 * i + 7] = buff[4 * i + 2 + 2];
			}
			LT_SendData_CRC_Frame(rebuf, 33);
		}

		if ((buff[0] + (buff[1] << 8)) < addr_index[7])
			Display_page(buff[0] + (buff[1] << 8));
		else
			page_date_update(pic_id);
	}
	else if (buff[0] == 1 || buff[0] == 0x81) // Multivariable control
	{
		for (i = 0; i < (len - 2); i += 4)
		{
			if ((buff[i + 4] + (buff[i + 4 + 1] << 8)) != 0xFFFF &&
				(buff[i + 4] + (buff[i + 4 + 1] << 8)) < 0x7200)
			{
				var[(buff[i + 4] + (buff[i + 4 + 1] << 8)) * 2] = buff[i + 4 + 3];
				var[(buff[i + 4] + (buff[i + 4 + 1] << 8)) * 2 + 1] = buff[i + 4 + 2];
				if ((buff[i + 4] + (buff[i + 4 + 1] << 8)) > 0x7000)
					reg_operation(buff[i + 4] + (buff[i + 4 + 1] << 8));
				else if ((buff[i + 4] + (buff[i + 4 + 1] << 8)) == 0x7000)
					page_flag = 1;
			}
		}
		
		if (page_flag == 1)
				reg_operation(0x7000);

		if (buff[0] == 0x81)
		{
			rebuf[3] = 0x41;
			for (i = 0; i < 8; i++)
			{
				rebuf[4 * i + 4] = buff[4 * i + 4 + 1];
				rebuf[4 * i + 5] = buff[4 * i + 4];
				rebuf[4 * i + 6] = buff[4 * i + 4 + 3];
				rebuf[4 * i + 7] = buff[4 * i + 4 + 2];
			}
			LT_SendData_CRC_Frame(rebuf, 33);
		}

		if ((buff[2] + (buff[3] << 8)) < addr_index[7])
			Display_page(buff[2] + (buff[3] << 8));
		else
			page_date_update(pic_id);
	}
	else if (buff[0] == 2 || buff[0] == 0x82)
	{
		p_add = buff[2] + (buff[3] << 8);
		V_max = buff[4] + (buff[5] << 8);
		V_min = buff[6] + (buff[7] << 8);
		step = buff[8] + (buff[9] << 8);
		loop = buff[11];

		sum = (var[p_add * 2] << 8) + var[p_add * 2 + 1];

		if (buff[10] == 1) // Add
		{
			if (sum + step > V_max)
			{
				if (loop == 1) // Circulate
				{
					var[p_add * 2] = V_min >> 8 & 0xFF;
					var[p_add * 2 + 1] = V_min & 0xFF;
				}
				else
				{
					var[p_add * 2] = V_max >> 8 & 0xFF;
					var[p_add * 2 + 1] = V_max & 0xFF;
				}
			}
			else
			{
				sum += step;
				var[p_add * 2] = sum >> 8 & 0xFF;
				var[p_add * 2 + 1] = sum & 0xFF;
			}
		}
		else if (buff[10] == 0) // subtract
		{
			if (sum - step < V_min)
			{
				if (loop == 1)
				{
					var[p_add * 2] = V_max >> 8 & 0xFF;
					var[p_add * 2 + 1] = V_max & 0xFF;
				}
				else
				{
					var[p_add * 2] = V_min >> 8 & 0xFF;
					var[p_add * 2 + 1] = V_min & 0xFF;
				}
			}
			else
			{
				sum -= step;
				var[p_add * 2] = sum >> 8 & 0xFF;
				var[p_add * 2 + 1] = sum & 0xFF;
			}
		}
		if (buff[0] == 0x82)
		{
			rebuf[3] = 0x41;
			rebuf[4] = buff[3];
			rebuf[5] = buff[2];
			rebuf[6] = var[p_add * 2];
			rebuf[7] = var[p_add * 2 + 1];
			LT_SendData_CRC_Frame(rebuf, 5);
		}
		if (p_add >= 0x7000)
			reg_operation(p_add);

		page_date_update(pic_id);
	}
	else if (buff[0] == 3 || buff[0] == 0x83)
	{
		if (knob_flag == 0)
			knob_flag = 1;
		else if (knob_flag == 1)
			knob_flag = 0;
	}
}

uint8_t knob_dir = 0, SW_flag = 0, SW_flag4 = 0, key_time = 0;
uint8_t Encoder_read_Flag = 0, Encoder_Freetime = 0;
uint8_t Enc0 = 0, Enc1 = 0;
uint8_t EncNow;
uint16_t Encoder_time = 0;
static uint8_t EncOld, EncX = 0;

uint8_t Double_click_interval = 0;
uint8_t Long_press = 0;
uint8_t Encoder_type = 0;

void EPORT0_2_IRQHandler(void)
{
	EPORT->EPFR |= 0x01 << EPORT_PIN2; // clear flag by writing 1 to it.
	Backlight_count = 0;	
	if (Encoder_read_Flag == 0 && key_time > 1)
	{
		Encoder_time = 0;
		Encoder_read_Flag = 1;
		if ((config_buff[gEncoder_addr[gEncoder_num]] & 0x1F) == 0x04)
		{
			Encoder_type = 4;
			Double_click_interval = config_buff[gEncoder_addr[gEncoder_num] + 36];
			Long_press = config_buff[gEncoder_addr[gEncoder_num] + 71];
		}
		else
			Encoder_type = 0;
	}
}

/***********************************************************************************
 * Function Name	: encoder
 * Description		: Detect encoder rotation and key.
 * Input			: None
 * Output			: None
 * Return			: None
 ************************************************************************************/
void encoder(void)
{
	uint8_t type;
	uint8_t rebuf[10];
	uint16_t addr;
	uint16_t p_add, step;
	int16_t V_max, V_min, sum = 0;

	if (Enc0 == 0)
		EncOld = (PINA ? 0x02 : 0x00) + (PINB ? 0x01 : 0x00); // Remember the status of the encoder at the first call
	if (EncOld == 0x00 || EncOld == 0x03)
		Enc0 = 1;
	EncNow = (PINA ? 0x02 : 0x00) + (PINB ? 0x01 : 0x00); // According to the current status of two IO, they are combined into a hexadecimal 0x00|0x01|0x02|0x03

	if ((EncOld == 0x00 && EncNow == 0x02) || (EncOld == 0x03 && EncNow == 0x01))
		EncX = EncNow;																							  // 00-10|11-01
	if ((EncOld == 0x00 && EncX == 0x02 && EncNow == 0x03) || (EncOld == 0x03 && EncX == 0x01 && EncNow == 0x00)) // 00-10-11|11-01-00 dextroversion
	{
		Encoder_Freetime = 0;
		EncOld = EncNow, EncX = 0;
		Backlight_count = 0;
		if (Backlight_flag == 0 && var[2 * 0x700e + 1] == 1)
		{			
			Backlight_flag = 1;
			Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
			if (var[0x700f * 2 + 1] == 0)
				Start_PWM1();
		}
		else 
		{
			if(EncType == 1)
			{
				if (knob_flag == 1) // Move right at knob control variable to increase
				{
					addr = gEncoder_addr[gEncoder_num];
					p_add = config_buff[addr + 2] + (config_buff[addr + 3] << 8);
					sum = (var[p_add * 2] << 8) + var[p_add * 2 + 1];
					V_max = config_buff[addr + 4] + (config_buff[addr + 5] << 8);
					V_min = config_buff[addr + 6] + (config_buff[addr + 7] << 8);
					step = config_buff[addr + 8] + (config_buff[addr + 9] << 8);

					if (sum + step > V_max)
					{
						var[p_add * 2] = V_max >> 8 & 0xFF;
						var[p_add * 2 + 1] = V_max & 0xFF;
					}
					else
					{
						sum += step;

						var[p_add * 2] = sum >> 8 & 0xFF;
						var[p_add * 2 + 1] = sum & 0xFF;
					}

					if (config_buff[addr] == 0x83)
					{
						rebuf[3] = 0x41;
						rebuf[4] = config_buff[addr + 3];
						rebuf[5] = config_buff[addr + 2];
						rebuf[6] = var[p_add * 2];
						rebuf[7] = var[p_add * 2 + 1];
						LT_SendData_CRC_Frame(rebuf, 5);
					}
				
					if (p_add >= 0x7000)
						reg_operation(p_add);

					page_date_update(pic_id);

				}
				else
				{
					gEncoder_num++;
					if (gEncoder_num > gEncoder_count)
						gEncoder_num = 0;
					var[Check_box_add * 2] = (gEncoder_num >> 8) & 0xFF;
					var[Check_box_add * 2 + 1] = gEncoder_num & 0xFF;
					page_date_update(pic_id);
				}
			}
			else
			{
				if (Enc1 == 0)
				{
					Enc1 = 1;
					knob_dir = 1;
					Encoder_Freetime = 0;
				}
				else
				{
					if (knob_dir == 1)
					{
						Enc1 = 0;
						knob_dir = 0;

						if (knob_flag == 1) // Move right at knob control variable to increase
						{
							addr = gEncoder_addr[gEncoder_num];
							p_add = config_buff[addr + 2] + (config_buff[addr + 3] << 8);
							sum = (var[p_add * 2] << 8) + var[p_add * 2 + 1];
							V_max = config_buff[addr + 4] + (config_buff[addr + 5] << 8);
							V_min = config_buff[addr + 6] + (config_buff[addr + 7] << 8);
							step = config_buff[addr + 8] + (config_buff[addr + 9] << 8);

							if (sum + step > V_max)
							{
								var[p_add * 2] = V_max >> 8 & 0xFF;
								var[p_add * 2 + 1] = V_max & 0xFF;
							}
							else
							{
								sum += step;

								var[p_add * 2] = sum >> 8 & 0xFF;
								var[p_add * 2 + 1] = sum & 0xFF;
							}

							if (config_buff[addr] == 0x83)
							{
								rebuf[3] = 0x41;
								rebuf[4] = config_buff[addr + 3];
								rebuf[5] = config_buff[addr + 2];
								rebuf[6] = var[p_add * 2];
								rebuf[7] = var[p_add * 2 + 1];
								LT_SendData_CRC_Frame(rebuf, 5);
							}
						
							if (p_add >= 0x7000)
								reg_operation(p_add);

							page_date_update(pic_id);

						}
						else
						{
							gEncoder_num++;
							if (gEncoder_num > gEncoder_count)
								gEncoder_num = 0;
							var[Check_box_add * 2] = (gEncoder_num >> 8) & 0xFF;
							var[Check_box_add * 2 + 1] = gEncoder_num & 0xFF;

							page_date_update(pic_id);
						}
					}
					else
					{
						knob_dir = 1;
						Enc1 = 1;
					}
				}
			}
		}
	}
	if ((EncOld == 0x00 && EncNow == 0x01) || (EncOld == 0x03 && EncNow == 0x02))
		EncX = EncNow;																							  // 00-01|11-10
	if ((EncOld == 0x00 && EncX == 0x01 && EncNow == 0x03) || (EncOld == 0x03 && EncX == 0x02 && EncNow == 0x00)) // 00-01-11|11-10-00 turn left
	{
		Encoder_Freetime = 0;
		EncOld = EncNow, EncX = 0;
		Backlight_count = 0;
		if (Backlight_flag == 0 && var[2 * 0x700e + 1] == 1 )
		{			
			Backlight_flag = 1;
			Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
			if (var[0x700f * 2 + 1] == 0)
				Start_PWM1();
		}
		else 
		{
			if(EncType == 1)
			{
				if (knob_flag == 1) // At the knob control variable
				{
					addr = gEncoder_addr[gEncoder_num];
					p_add = config_buff[addr + 2] + (config_buff[addr + 3] << 8);
					sum = (var[p_add * 2] << 8) + var[p_add * 2 + 1];
					V_max = config_buff[addr + 4] + (config_buff[addr + 5] << 8);
					V_min = config_buff[addr + 6] + (config_buff[addr + 7] << 8);
					step = config_buff[addr + 8] + (config_buff[addr + 9] << 8);
					if (sum - step < V_min)
					{
						var[p_add * 2] = V_min >> 8 & 0xFF;
						var[p_add * 2 + 1] = V_min & 0xFF;
					}
					else
					{
						sum -= step;
						var[p_add * 2] = sum >> 8 & 0xFF;
						var[p_add * 2 + 1] = sum & 0xFF;
					}
					if (config_buff[addr] == 0x83)
					{
						rebuf[3] = 0x41;
						rebuf[4] = config_buff[addr + 3];
						rebuf[5] = config_buff[addr + 2];
						rebuf[6] = var[p_add * 2];
						rebuf[7] = var[p_add * 2 + 1];
						LT_SendData_CRC_Frame(rebuf, 5);
					}
					if (p_add >= 0x7000)
						reg_operation(p_add);

					page_date_update(pic_id);
				}
				else
				{
					if (gEncoder_num == 0)
						gEncoder_num = gEncoder_count;
					else
						gEncoder_num--;
					var[Check_box_add * 2] = (gEncoder_num >> 8) & 0xFF;
					var[Check_box_add * 2 + 1] = gEncoder_num & 0xFF;
					
					page_date_update(pic_id);
				}
			}
			else
			{
				if (Enc1 == 0) // First rotation
				{
					knob_dir = 2;
					Enc1 = 1;
					Encoder_Freetime = 0;
				}
				else // Second rotation within 1S between the first rotation
				{
					if (knob_dir == 2)
					{
						Enc1 = 0;
						knob_dir = 0;
						if (knob_flag == 1) // At the knob control variable
						{
							addr = gEncoder_addr[gEncoder_num];
							p_add = config_buff[addr + 2] + (config_buff[addr + 3] << 8);
							sum = (var[p_add * 2] << 8) + var[p_add * 2 + 1];
							V_max = config_buff[addr + 4] + (config_buff[addr + 5] << 8);
							V_min = config_buff[addr + 6] + (config_buff[addr + 7] << 8);
							step = config_buff[addr + 8] + (config_buff[addr + 9] << 8);
							if (sum - step < V_min)
							{
								var[p_add * 2] = V_min >> 8 & 0xFF;
								var[p_add * 2 + 1] = V_min & 0xFF;
							}
							else
							{
								sum -= step;
								var[p_add * 2] = sum >> 8 & 0xFF;
								var[p_add * 2 + 1] = sum & 0xFF;
							}
							if (config_buff[addr] == 0x83)
							{
								rebuf[3] = 0x41;
								rebuf[4] = config_buff[addr + 3];
								rebuf[5] = config_buff[addr + 2];
								rebuf[6] = var[p_add * 2];
								rebuf[7] = var[p_add * 2 + 1];
								LT_SendData_CRC_Frame(rebuf, 5);
							}
							if (p_add >= 0x7000)
								reg_operation(p_add);

							page_date_update(pic_id);
						}
						else
						{
							if (gEncoder_num == 0)
								gEncoder_num = gEncoder_count;
							else
								gEncoder_num--;
							var[Check_box_add * 2] = (gEncoder_num >> 8) & 0xFF;
							var[Check_box_add * 2 + 1] = gEncoder_num & 0xFF;
							page_date_update(pic_id);
						}
					}
					else
					{
						knob_dir = 2;
						Enc1 = 1;
					}
				}
			}
		}
	}
	if(SW_flag)
	{
		Backlight_count = 0;
		if (Backlight_flag == 0 && var[2 * 0x700e + 1] == 1)
		{			
			SW_flag = 0;
			SW_flag4 = 0;
			Backlight_flag = 1;
			Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
			if (var[0x700f * 2 + 1] == 0)
				Start_PWM1();
		}
		else
		{
			if (SW_flag == 1)
			{
				SW_flag = 0;
				Enc1 = 0;
				knob_dir = 0;
				Display_encoder(&config_buff[gEncoder_addr[gEncoder_num]], 0);
			}
			else if (SW_flag4 == 3 && SW_flag == 4)	
			{
				SW_flag = 0;
				Enc1 = 0;
				knob_dir = 0;
				if ((config_buff[gEncoder_addr[gEncoder_num]] & 0x2F) == 0x24)
					type = 0x84;
				else
					type = 4;
				Display_encoder(&config_buff[gEncoder_addr[gEncoder_num] + 72], type);
			}
			else if (SW_flag4 == 2 && SW_flag == 3)
			{
				SW_flag = 0;
				Enc1 = 0;
				knob_dir = 0;
				if ((config_buff[gEncoder_addr[gEncoder_num]] & 0x4F) == 0x44)
					type = 0x84;
				else
					type = 4;
				Display_encoder(&config_buff[gEncoder_addr[gEncoder_num] + 37], type);
			}
			else if (SW_flag4 == 1 && SW_flag == 2)
			{
				SW_flag4 = 0;
				SW_flag = 0;
				Enc1 = 0;
				knob_dir = 0;
				if ((config_buff[gEncoder_addr[gEncoder_num]] & 0x8F) == 0x84)
					type = 0x84;
				else
					type = 4;
				Display_encoder(&config_buff[gEncoder_addr[gEncoder_num] + 2], type);
			}
		}
	}
	if (SW_flag4 == 2 || SW_flag4 == 3)
	{
		Backlight_count = 0;
		if (Read_Encoder_key == 1)
		{
			SW_flag4 = 0;
			Encoder_read_Flag = 0;
			Encoder_time = 0;
			key_time = 0;
		}
	}
}
/****************/
#endif
/***********************************************************************************
 * Function Name	: BackLight_control
 * Description		: Automatically adjust backlight.
 * Input			: None
 * Output			: None
 * Return			: None
 ************************************************************************************/
void BackLight_control(void)
{
	if (Backlight_first_touch == 1 && Backlight_touch == 0)
	{
		Backlight_flag = 1;
		Backlight_first_touch = 0;
	}
	
	if (Backlight_count >= ((var[0x7010 * 2] << 8) + var[0x7010 * 2 + 1 ]) * 100 && Backlight_flag == 1)
	{
		Set_Timer1_Compare_Buffer(gDutyBuf[var[0x700f * 2 + 1]]);
		if (var[0x700f * 2 + 1] == 0)
			Stop_PWM1();
		Backlight_flag = 0;
		Backlight_count = 0;
	}
}
/**********/
#include "reset_drv.h"
#define Run_Boot_ID			(*(volatile UINT32 *)(0x20000000+0x37000))	//APP_Update_ID0: 0x32021688
void APP(u32 appxaddr)
{
	
	Run_Boot_ID = 0x47210659;

	LT768_PWM1_Init(0,0,5,800,0);
	DelayMS(200);

	Disable_Interrupts;
	Soft_Reset();	


	
}