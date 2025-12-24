/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     pit.c
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief
 *************************************************************************/

#include "pit.h"
#include "bsp.h"
#include "pwm.h"
#include "data.h"
#include "ctp.h"
#include "pit32_drv.h"
#include "sys.h"
#include "dac.h"
#include "pwm_drv.h"
#include "uart.h"

uint8_t flag_MB_REG[20];

#if (UARTBUS_OPTION == 0 || UARTBUS_OPTION == 3 || UARTBUS_OPTION == 4)
// Dynamic display timer
void PIT1_Init(void)
{
	NVIC_Init(1, 0, PIT1_IRQn, 2);

	PIT1->PCSR &= (~PCSR_EN);
	PIT1->PCSR = (5 << 8) | PCSR_OVW | PCSR_PIE | PCSR_RLD | PCSR_PDBG; // 75mhz/32  Interrupt mode
	PIT1->PMR = 23437;													// 10ms
	PIT1->PCSR |= PCSR_EN;
}

void PIT1_IRQHandler(void)
{

	PIT1->PCSR |= (1 << 2); // clear PIF interrupt flag

		
	if (Var_Count_num > 0)
		Var_Count_flag = 1;
	if(Pointer_Count_num >0)
		Pointer_Count_flag = 1;
	if (Gif_num > 0)
		Gif_flag = 1;
	if (Adj_loop > 0)
		Adj_time++;
	if (DioKb_flag == 1)
		Cusor_time++;
	if (AioKb_flag == 1)
		Cusor_time++;
	if (GBKioKb_flag == 1)
		Cusor_time++;
	if (Scroll_num > 0)
		Scroll_flag = 1;
	if (curve_num > 0)
		curve_flag = 1;
	if (Backlight_flag)
		Backlight_count++;
	if (buzzer_start)
	{
		buzzer_10ms_count++;
		if (buzzer_10ms_count >= 5)
		{
//			UART_WriteGpioData(SCI2, UART_TX, Bit_RESET);
            SPI_WriteGpioData(SPI3, SPI_MOSI,Bit_RESET);
			buzzer_start = 0;
			buzzer_10ms_count = 0;
		}
	}

	page_refreshing++;
	if(ctp_press_t < 0xFF) ctp_press_t++;

#if encoder_on

	if (SW_flag4 != 2 || SW_flag4 != 3)
		key_time++;

	if (Encoder_read_Flag == 1)
	{
		Backlight_count = 0;
		if (Encoder_type == 0)
		{
			Encoder_time++;
			if (Encoder_time > 1)
			{
				if (Read_Encoder_key == 1)
				{
					Encoder_read_Flag = 0;
					Encoder_time = 0;
					key_time = 0;
					SW_flag = 1;
				}
			}
		}
		else if (Encoder_type == 4)
		{
			Encoder_time++;
			if (Encoder_time > 1)
			{
				if (SW_flag4 == 1)
				{
					SW_flag4 = 2;
					SW_flag = 3;
					Encoder_read_Flag = 0;
					Encoder_time = 0;
					key_time = 0;
				}
				if (Encoder_time > (Long_press * 5) && SW_flag4 != 3)
				{
					SW_flag4 = 3;
					SW_flag = 4;
					Encoder_read_Flag = 0;
					Encoder_time = 0;
					key_time = 0;
				
				}
				if (Read_Encoder_key == 1)
				{
					SW_flag4 = 1;
					Encoder_read_Flag = 0;
					Encoder_time = 0;
					key_time = 0;
				}
			}
		}
	}
	if (SW_flag4 == 1)
	{
		Encoder_time++;
		if (Encoder_time > Double_click_interval * 5) // Double-click valid time exceeded
		{
			SW_flag4 = 1;
			SW_flag = 2;
		}
	}
	if (Enc1)
	{
		Encoder_Freetime++;
		if (Encoder_Freetime > 100)
		{
			Encoder_Freetime = 0;
			knob_dir = 0;
			Enc1 = 0;
		}
	}
#endif
}
#endif

volatile uint8_t Addr_ModBus_Sla = 0x01;
#if (UARTBUS_OPTION == 1)

void PIT1_Init(void)
{
	NVIC_Init(1, 0, PIT1_IRQn, 2);

	PIT1->PCSR &= (~PCSR_EN);
	PIT1->PCSR = (3 << 8) | PCSR_OVW | PCSR_PIE | PCSR_RLD | PCSR_PDBG; // 75mhz/8  Interrupt mode
	PIT1->PMR = 9375;													// 1ms
	PIT1->PCSR |= PCSR_EN;
}

#define MB_ERR_CMD 0x01
#define MB_ERR_ADDR 0x02
#define MB_ERR_DATA 0x03
#define MB_ERR_FAULT 0x04

volatile MB_CMD_RXINFO gMB_CMDRX;
volatile uint8_t MB_SLA_RXBUF[256] = {0};
volatile uint16_t MB_SLA_Len = 0;

volatile uint8_t Feedback_ModbusBuf[256] = {0};

volatile uint32_t pit_1ms_count = 0;
volatile uint8_t uart_flag = 0;
volatile uint32_t pit_1ms_count2 = 0;

void PIT1_IRQHandler(void)
{
	uint16_t CRC_MCU = 0, CRC_RX = 0;
	uint16_t i = 0, j = 0;
	uint8_t num = 0, cnt = 0;
	uint16_t temp = 0;
	PIT1->PCSR |= PCSR_PIF; // clear PIF interrupt flag
	pit_1ms_count++;
	pit_1ms_count2++;
	// If the serial port is detected for 5ms and no data is received, the reception is completed
	if (pit_1ms_count == 5 && uart_flag)
	{
		pit_1ms_count = 0;
		uart_flag = 0;

		MB_SLA_Len = gUsartRx.Count;
		memcpy(MB_SLA_RXBUF, gUsartRx.Buf, MB_SLA_Len);
		gUsartRx.Count = 0;

		if (MB_SLA_RXBUF[0] == Addr_ModBus_Sla || MB_SLA_RXBUF[0] == 0) // Transceiver node consistency or broadcast mode
		{
			CRC_MCU = (MB_SLA_RXBUF[MB_SLA_Len - 1] << 8) + (MB_SLA_RXBUF[MB_SLA_Len - 2] & 0x00FF);
			CRC_RX = CRC16(MB_SLA_RXBUF, MB_SLA_Len - 2);
			if (CRC_MCU == CRC_RX) // CRC check is correct
			{
				gMB_CMDRX.CMD = MB_SLA_RXBUF[1];
				gMB_CMDRX.Addr = MB_SLA_RXBUF[2] << 8 | MB_SLA_RXBUF[3];

				// Read command response (broadcast mode read function is meaningless)
				if (gMB_CMDRX.CMD == 0x03 && MB_SLA_RXBUF[0] != 0)
				{
					gMB_CMDRX.Num = MB_SLA_RXBUF[4] << 8 | MB_SLA_RXBUF[5];
					if (gMB_CMDRX.Addr <= 0x6FFF)
					{
						Feedback_ModbusBuf[0] = Addr_ModBus_Sla;
						Feedback_ModbusBuf[1] = gMB_CMDRX.CMD;
						Feedback_ModbusBuf[2] = 2 * gMB_CMDRX.Num;
						for (i = 0; i < gMB_CMDRX.Num; i++)
						{
							Feedback_ModbusBuf[3 + 2 * i] = var[2 * (gMB_CMDRX.Addr + i)];
							Feedback_ModbusBuf[4 + 2 * i] = var[2 * (gMB_CMDRX.Addr + i) + 1];
						}
						LT_SendData_CRC_NoFrame(Feedback_ModbusBuf, 3 + Feedback_ModbusBuf[2]);
					}
					else if (gMB_CMDRX.Addr >= 0x7000 && gMB_CMDRX.Addr <= 0x71FF)
					{
						if (gMB_CMDRX.Addr <= 0x7007 || gMB_CMDRX.Addr + gMB_CMDRX.Num >= 0x7002)
						{
							var[0x7002 * 2 + 1] = calendar.w_year - 2000;
							var[0x7003 * 2 + 1] = calendar.w_month;
							var[0x7004 * 2 + 1] = calendar.w_date;
							var[0x7005 * 2 + 1] = calendar.hour;
							var[0x7006 * 2 + 1] = calendar.min;
							var[0x7007 * 2 + 1] = calendar.sec;
						}

						Feedback_ModbusBuf[0] = Addr_ModBus_Sla;
						Feedback_ModbusBuf[1] = gMB_CMDRX.CMD;
						Feedback_ModbusBuf[2] = 2 * gMB_CMDRX.Num;
						for (i = 0; i < gMB_CMDRX.Num; i++)
						{
							Feedback_ModbusBuf[3 + 2 * i] = var[2 * (gMB_CMDRX.Addr + i)];
							Feedback_ModbusBuf[4 + 2 * i] = var[2 * (gMB_CMDRX.Addr + i) + 1];
						}
						LT_SendData_CRC_NoFrame(Feedback_ModbusBuf, 3 + Feedback_ModbusBuf[2]);
					}
					else
					{
						Feedback_ModbusBuf[0] = Addr_ModBus_Sla;
						Feedback_ModbusBuf[1] = gMB_CMDRX.CMD + 0x80;
						Feedback_ModbusBuf[2] = MB_ERR_ADDR;
						LT_SendData_CRC_NoFrame(Feedback_ModbusBuf, 3);
					}
				}
				else if (gMB_CMDRX.CMD == 0x06) // Write instruction response
				{
					if (gMB_CMDRX.Addr <= 0x6FFF)
					{
						var[2 * gMB_CMDRX.Addr] = MB_SLA_RXBUF[4];
						var[2 * gMB_CMDRX.Addr + 1] = MB_SLA_RXBUF[5];

						if (MB_SLA_RXBUF[0] != 0)
							LT_SendData_CRC_NoFrame(MB_SLA_RXBUF, 6);
					}
					else if (gMB_CMDRX.Addr >= 0x7000 && gMB_CMDRX.Addr <= 0x71FF)
					{
						var[2 * gMB_CMDRX.Addr] = MB_SLA_RXBUF[4];
						var[2 * gMB_CMDRX.Addr + 1] = MB_SLA_RXBUF[5];
						flag_MB_REG[gMB_CMDRX.Addr - 0x7000] = 1;

						if (MB_SLA_RXBUF[0] != 0)
							LT_SendData_CRC_NoFrame(MB_SLA_RXBUF, 6);
					}
					else
					{
						Feedback_ModbusBuf[0] = Addr_ModBus_Sla;
						Feedback_ModbusBuf[1] = gMB_CMDRX.CMD + 0x80;
						Feedback_ModbusBuf[2] = MB_ERR_ADDR;
						if (MB_SLA_RXBUF[0] != 0)
							LT_SendData_CRC_NoFrame(Feedback_ModbusBuf, 3);
					}
				}
				else if (gMB_CMDRX.CMD == 0x10) // Write multiple instruction response
				{
					gMB_CMDRX.Num = MB_SLA_RXBUF[4] << 8 | MB_SLA_RXBUF[5];
					if (gMB_CMDRX.Addr <= 0x6FFF) // In the variable area
					{
						// Save data
						for (i = 0; i < gMB_CMDRX.Num; i++)
						{
							var[2 * (gMB_CMDRX.Addr + i)] = MB_SLA_RXBUF[7 + 2 * i];
							var[2 * (gMB_CMDRX.Addr + i) + 1] = MB_SLA_RXBUF[8 + 2 * i];
						}
						if (MB_SLA_RXBUF[0] != 0)
							LT_SendData_CRC_NoFrame(MB_SLA_RXBUF, 6);
					}
					else if (gMB_CMDRX.Addr >= 0x7000 && gMB_CMDRX.Addr <= 0x71FF) // In register area
					{
						for (i = 0; i < gMB_CMDRX.Num; i++)
						{
							var[2 * (gMB_CMDRX.Addr + i)] = MB_SLA_RXBUF[7 + 2 * i];
							var[2 * (gMB_CMDRX.Addr + i) + 1] = MB_SLA_RXBUF[8 + 2 * i];
							flag_MB_REG[gMB_CMDRX.Addr - 0x7000 + i] = 1;
						}
						if (MB_SLA_RXBUF[0] != 0)
							LT_SendData_CRC_NoFrame(MB_SLA_RXBUF, 6);
					}
					else if (gMB_CMDRX.Addr >= 0xC001 && gMB_CMDRX.Addr <= 0xCFFF) // Curve channel write data
					{
						gMB_CMDRX.Num = MB_SLA_RXBUF[4] << 8 | MB_SLA_RXBUF[5];

						temp = gMB_CMDRX.Addr & 0xFFF;
						for (i = 0; i < Curve_Size; i++)
						{
							if (temp & (1 << i))
								num++; // Record how many groups of channels
						}

						i = 0;
						j = 0;
						while (j < gMB_CMDRX.Num) // Store data to corresponding channels respectively
						{
							if (temp & (1 << i))
							{
								Curve_buff[i][Curve_count[i]] = MB_SLA_RXBUF[7 + 2 * j] << 8 | MB_SLA_RXBUF[8 + 2 * j];
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
						/* A consensus was reached that when multi-channel buffering data was triggered,
						if some channel data was missing, the missing part would be supplemented */
						if (gMB_CMDRX.Num % num)
						{
							for (i = 0; i < Curve_Size; i++)
							{
								if (temp & (1 << i))
									cnt++;
								if (cnt > gMB_CMDRX.Num % num && (temp & (1 << i)))
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
						if (MB_SLA_RXBUF[0] != 0)
							LT_SendData_CRC_NoFrame(MB_SLA_RXBUF, 6);
					}
					else if (gMB_CMDRX.Addr >= 0xE001 && gMB_CMDRX.Addr <= 0xEFFF) // Curve Channel Clear Data
					{
						temp = gMB_CMDRX.Addr & 0xFFF;
						for (i = 0; i < Curve_Size; i++) // Store data to corresponding channels respectively
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
						if (MB_SLA_RXBUF[0] != 0)
							LT_SendData_CRC_NoFrame(MB_SLA_RXBUF, 6);
					}
					else
					{
						Feedback_ModbusBuf[0] = Addr_ModBus_Sla;
						Feedback_ModbusBuf[1] = gMB_CMDRX.CMD + 0x80;
						Feedback_ModbusBuf[2] = MB_ERR_ADDR;
						if (MB_SLA_RXBUF[0] != 0)
							LT_SendData_CRC_NoFrame(Feedback_ModbusBuf, 3);
					}
				}
				else
				{
					Feedback_ModbusBuf[0] = Addr_ModBus_Sla;
					Feedback_ModbusBuf[1] = gMB_CMDRX.CMD + 0x80;
					Feedback_ModbusBuf[2] = MB_ERR_CMD;
					if (MB_SLA_RXBUF[0] != 0)
						LT_SendData_CRC_NoFrame(Feedback_ModbusBuf, 3);
				}
			}
			else
			{
			}
		}
		else // The sending and receiving nodes are inconsistent
		{
		}
	}

	if (pit_1ms_count2 >= 10)
	{
		pit_1ms_count2 = 0;

		if (Var_Count_num > 0)
		Var_Count_flag = 1;
		if(Pointer_Count_num >0)
			Pointer_Count_flag = 1;
		if (Gif_num > 0)
			Gif_flag = 1;
		if (Adj_loop > 0)
			Adj_time++;
		if (DioKb_flag == 1)
			Cusor_time++;
		if (AioKb_flag == 1)
			Cusor_time++;
		if (GBKioKb_flag == 1)
			Cusor_time++;
		if (Scroll_num > 0)
			Scroll_flag = 1;
		if (curve_num > 0)
			curve_flag = 1;
		if (Backlight_flag)
			Backlight_count++;
		if (buzzer_start)
		{
			buzzer_10ms_count++;
			if (buzzer_10ms_count >= 5)
			{
				UART_WriteGpioData(SCI2, UART_TX, Bit_RESET);
				buzzer_start = 0;
				buzzer_10ms_count = 0;
			}
		}

		page_refreshing++;
		if(ctp_press_t < 0xFF) ctp_press_t++;

#if encoder_on

	if (SW_flag4 != 2 || SW_flag4 != 3)
		key_time++;

	if (Encoder_read_Flag == 1)
	{
		Backlight_count = 0;
		if (Encoder_type == 0)
		{
			Encoder_time++;
			if (Encoder_time > 1)
			{
				if (Read_Encoder_key == 1)
				{
					Encoder_read_Flag = 0;
					Encoder_time = 0;
					key_time = 0;
					SW_flag = 1;
				}
			}
		}
		else if (Encoder_type == 4)
		{
			Encoder_time++;
			if (Encoder_time > 1)
			{
				if (SW_flag4 == 1)
				{
					SW_flag4 = 2;
					SW_flag = 3;
					Encoder_read_Flag = 0;
					Encoder_time = 0;
					key_time = 0;
				}
				if (Encoder_time > (Long_press * 5) && SW_flag4 != 3)
				{
					SW_flag4 = 3;
					SW_flag = 4;
					Encoder_read_Flag = 0;
					Encoder_time = 0;
					key_time = 0;
				}
				if (Read_Encoder_key == 1)
				{
					SW_flag4 = 1;
					Encoder_read_Flag = 0;
					Encoder_time = 0;
					key_time = 0;
				}
			}
		}
	}
	if (SW_flag4 == 1)
	{
		Encoder_time++;
		if (Encoder_time > Double_click_interval * 5) // Double-click valid time exceeded
		{
			SW_flag4 = 1;
			SW_flag = 2;
		}
	}
	if (Enc1)
	{
		Encoder_Freetime++;
		if (Encoder_Freetime > 100)
		{
			Encoder_Freetime = 0;
			knob_dir = 0;
			Enc1 = 0;
		}
	}
#endif
	}
}

#endif

#if (UARTBUS_OPTION == 2)
void PIT1_Init(void)
{
	NVIC_Init(1, 0, PIT1_IRQn, 2);

	PIT1->PCSR &= (~PCSR_EN);
	PIT1->PCSR = (3 << 8) | PCSR_OVW | PCSR_PIE | PCSR_RLD | PCSR_PDBG; // 75mhz/8  Interrupt mode
	PIT1->PMR = 9375;													// 1ms
	PIT1->PCSR |= PCSR_EN;
}

volatile uint32_t pit_1ms_count = 0;
volatile uint8_t uart_flag = 0;

volatile uint32_t pit_1ms_count2 = 0;
volatile uint32_t tx_10ms_cnt = 0;
volatile uint8_t tx_500ms_flag = 0;

volatile uint32_t pit_1ms_count3 = 0;
volatile uint8_t timeout_flag = 0;
volatile uint8_t MB_SLA_RXBUF[256] = {0};
volatile uint16_t MB_SLA_Len = 0;
void PIT1_IRQHandler(void)
{
	uint16_t CRC_MCU = 0, CRC_RX = 0;
	uint16_t i = 0;

	PIT1->PCSR |= PCSR_PIF; // clear PIF interrupt flag
	pit_1ms_count++;
	pit_1ms_count2++;
	pit_1ms_count3++;

	if (pit_1ms_count >= 5 && uart_flag) // If the serial port is detected for 5ms and no data is received, the reception is completed
	{
		pit_1ms_count = 0;
		uart_flag = 0;
		gUsartRx.Flag = 1;

		MB_SLA_Len = gUsartRx.Count;
		memcpy(MB_SLA_RXBUF, gUsartRx.Buf, MB_SLA_Len);
		gUsartRx.Count = 0;

		CRC_MCU = (MB_SLA_RXBUF[MB_SLA_Len - 1] << 8) + (MB_SLA_RXBUF[MB_SLA_Len - 2] & 0x00FF);
		CRC_RX = CRC16(MB_SLA_RXBUF, MB_SLA_Len - 2);

		if (CRC_MCU == CRC_RX) // CRC check is correct
		{
			if (MB_SLA_RXBUF[0] == gUsartTx.Sla_Add) // The sending and receiving nodes are consistent
			{
				pit_1ms_count3 = 0;
				timeout_flag = 0;
				
				if (MB_SLA_RXBUF[1] == gUsartTx.CMD)	
				{
					if (gUsartTx.CMD == 0x01 || gUsartTx.CMD == 0x02) // Read command response	
					{
						if ( (gUsartTx.Len%8==0 && gUsartTx.Len/8==MB_SLA_RXBUF[2]) || (gUsartTx.Len%8!=0 && gUsartTx.Len/8+1==MB_SLA_RXBUF[2]) ) // Judge whether the number of read bytes sent and responded is the same
						{
							for (i = 0; i < gUsartTx.Len; i++) // Save variables
							{
								//For debug
//								if( MB_SLA_RXBUF[3+i/8]&(1<<(i%8)) )	
//									var[2 * (gUsartTx.Var_Add) + (gUsartTx.REG_Add%8+i)/8] |= 1<<( (gUsartTx.REG_Add+i)%8);
//								else
//									var[2 * (gUsartTx.Var_Add) + (gUsartTx.REG_Add%8+i)/8] &=~(1<<( (gUsartTx.REG_Add+i)%8) );

								if( (gUsartTx.REG_Add+i)%16 < 8)	
								{
									if( MB_SLA_RXBUF[3+i/8]&(1<<(i%8)) )	
										var[2 * (gUsartTx.Var_Add + (gUsartTx.REG_Add%8+i)/16)+1] |= 1<<( (gUsartTx.REG_Add+i)%8);
									else
										var[2 * (gUsartTx.Var_Add + (gUsartTx.REG_Add%8+i)/16)+1] &=~(1<<( (gUsartTx.REG_Add+i)%8) );									
								}
								else
								{
									if( MB_SLA_RXBUF[3+i/8]&(1<<(i%8)) )	
										var[2 * (gUsartTx.Var_Add + (gUsartTx.REG_Add%8+i)/16)] |= 1<<( (gUsartTx.REG_Add+i)%8);
									else
										var[2 * (gUsartTx.Var_Add + (gUsartTx.REG_Add%8+i)/16)] &=~(1<<( (gUsartTx.REG_Add+i)%8) );									
								}								
							}

							/*   Customization
							if(Master_mode03_flag[0] == 1)	//?????Code_????????????§Õ??????¡À??????????????????????
							{
								var[0x2002*2]   = Master_mode03_Var[0];
								var[0x2002*2+1] = Master_mode03_Var[1];
							}
							if(Master_mode03_flag[1] == 1)	//?????Code_????????????§Õ??????¡À??????????????????????
							{
								var[0x2003*2]   = Master_mode03_Var[2];
								var[0x2003*2+1] = Master_mode03_Var[3];
							}
							*/

							if (gUsartTx.Mode == 0x02) // Data clearing of parameter pointer address in mode 2
							{
								var[2 * (gUsartTx.Opt_Add)] = 0;
								var[2 * (gUsartTx.Opt_Add) + 1] = 0;
							}
							else if (gUsartTx.Mode == 0x03) // Customized Code_ The flag bit corresponding to mode 3 is cleared
							{
								Master_mode03_flag[gUsartTx.Opt_Add] = 0;
							}
							tx_repeat_cnt = 0;
							tx_repeat_flag = 0; // The response will not need to be retransmitted, and the next command will be executed
						}
						else
						{
							tx_repeat_flag = 1;
						}
					}					
					else if (gUsartTx.CMD == 0x03 || gUsartTx.CMD == 0x04) // Read command response
					{
						if (gUsartTx.Len == MB_SLA_RXBUF[2] / 2) // Judge whether the number of read bytes sent and responded is the same
						{
							for (i = 0; i < MB_SLA_RXBUF[2] / 2; i++) // Save variables
							{
								var[2 * (gUsartTx.Var_Add) + 2 * i] = MB_SLA_RXBUF[3 + 2 * i];
								var[2 * (gUsartTx.Var_Add) + 2 * i + 1] = MB_SLA_RXBUF[3 + 2 * i + 1];

								if (gUsartTx.Var_Add >= 0x7000 && gUsartTx.Var_Add <= 0x71FF)
								{
									flag_MB_REG[gUsartTx.Var_Add - 0x7000 + i] = 1;
								}
							}

							/*   Customization
							if(Master_mode03_flag[0] == 1)	//?????Code_????????????§Õ??????¡À??????????????????????
							{
								var[0x2002*2]   = Master_mode03_Var[0];
								var[0x2002*2+1] = Master_mode03_Var[1];
							}
							if(Master_mode03_flag[1] == 1)	//?????Code_????????????§Õ??????¡À??????????????????????
							{
								var[0x2003*2]   = Master_mode03_Var[2];
								var[0x2003*2+1] = Master_mode03_Var[3];
							}
							*/

							if (gUsartTx.Mode == 0x02) // Data clearing of parameter pointer address in mode 2
							{
								var[2 * (gUsartTx.Opt_Add)] = 0;
								var[2 * (gUsartTx.Opt_Add) + 1] = 0;
							}
							else if (gUsartTx.Mode == 0x03) // Customized Code_ The flag bit corresponding to mode 3 is cleared
							{
								Master_mode03_flag[gUsartTx.Opt_Add] = 0;
							}
							tx_repeat_cnt = 0;
							tx_repeat_flag = 0; // The response will not need to be retransmitted, and the next command will be executed
						}
						else
						{
							tx_repeat_flag = 1;
						}
					}
					else if (gUsartTx.CMD == 0x05 || gUsartTx.CMD == 0x06) // Write instruction response
					{
						if (gUsartTx.REG_Add == (MB_SLA_RXBUF[2] << 8 | MB_SLA_RXBUF[3])) // Judge whether the register address of sending and responding is the same
						{
							if (gUsartTx.Mode == 0x02) // Data clearing of parameter pointer address in mode 2
							{
								var[2 * (gUsartTx.Opt_Add)] = 0;
								var[2 * (gUsartTx.Opt_Add) + 1] = 0;
							}
							else if (gUsartTx.Mode == 0x03) // Customized Code_ The flag bit corresponding to mode 3 is cleared
							{
								Master_mode03_flag[gUsartTx.Opt_Add] = 0;
							}
							tx_repeat_cnt = 0;
							tx_repeat_flag = 0; // The response will not need to be retransmitted, and the next command will be executed
						}
						else
						{
							tx_repeat_flag = 1;
						}
					}
					else if (gUsartTx.CMD == 0x0F || gUsartTx.CMD == 0x10) // Write multiple instruction response
					{
						if (gUsartTx.REG_Add == (MB_SLA_RXBUF[2] << 8 | MB_SLA_RXBUF[3])) // Judge whether the register address of sending and responding is the same
						{
							if (gUsartTx.Mode == 0x02) // Data clearing of parameter pointer address in mode 2
							{
								var[2 * (gUsartTx.Opt_Add)] = 0;
								var[2 * (gUsartTx.Opt_Add) + 1] = 0;
							}
							else if (gUsartTx.Mode == 0x03) // Customized Code_ The flag bit corresponding to mode 3 is cleared
							{
								Master_mode03_flag[gUsartTx.Opt_Add] = 0;
							}
							tx_repeat_cnt = 0;
							tx_repeat_flag = 0; // The response will not need to be retransmitted, and the next command will be executed
						}
						else
						{
							tx_repeat_flag = 1;
						}
					}
				}
				else // Resend if the answer is wrong
				{
					tx_repeat_flag = 1;
				}
			}
			else // Resend if the sending and receiving nodes are inconsistent
			{
				tx_repeat_flag = 1;
			}
		}
		else
		{
			tx_repeat_flag = 1; // Resend if CRC check error
		}
		if (gUsartTx.Sla_Add != 0)
			tx_en_flag = 1; // After receiving a frame of unicast mode slave response, it is allowed to send flag bit enable
	}

	if (pit_1ms_count3 > gUsartTx.Timeout && timeout_flag)
	{
		pit_1ms_count3 = 0;
		timeout_flag = 0;
		tx_repeat_flag = 1;
		tx_en_flag = 1; // After receiving a frame, allow to send flag bit enable
	}

	if (pit_1ms_count2 > 10)
	{
		pit_1ms_count2 = 0;

		tx_10ms_cnt++;
		if (tx_10ms_cnt > 50)
		{

			tx_500ms_flag = 1;
		}

		if (Gif_num > 0)
			Gif_flag = 1;
		if (Adj_loop > 0)
			Adj_time++;
		if (DioKb_flag == 1)
			Cusor_time++;
		if (AioKb_flag == 1)
			Cusor_time++;
		if (GBKioKb_flag == 1)
			Cusor_time++;
		if (Scroll_num > 0)
			Scroll_flag = 1;
		if (curve_num > 0)
			curve_flag = 1;
		if (Backlight_flag)
			Backlight_count++;
		if (buzzer_start)
		{
			buzzer_10ms_count++;
			if (buzzer_10ms_count >= 5)
			{
				UART_WriteGpioData(SCI2, UART_TX, Bit_RESET);
				buzzer_start = 0;
				buzzer_10ms_count = 0;
			}
		}

		page_refreshing++;
		if(ctp_press_t < 0xFF) ctp_press_t++;

#if encoder_on

	if (SW_flag4 != 2 || SW_flag4 != 3)
		key_time++;

	if (Encoder_read_Flag == 1)
	{
		Backlight_count = 0;
		if (Encoder_type == 0)
		{
			Encoder_time++;
			if (Encoder_time > 1)
			{
				if (Read_Encoder_key == 1)
				{
					Encoder_read_Flag = 0;
					Encoder_time = 0;
					key_time = 0;
					SW_flag = 1;
				}
			}
		}
		else if (Encoder_type == 4)
		{
			Encoder_time++;
			if (Encoder_time > 1)
			{
				if (SW_flag4 == 1)
				{
					SW_flag4 = 2;
					SW_flag = 3;
					Encoder_read_Flag = 0;
					Encoder_time = 0;
					key_time = 0;
				}
				if (Encoder_time > (Long_press * 5) && SW_flag4 != 3)
				{
					SW_flag4 = 3;
					SW_flag = 4;
					Encoder_read_Flag = 0;
					Encoder_time = 0;
					key_time = 0;
				}
				if (Read_Encoder_key == 1)
				{
					SW_flag4 = 1;
					Encoder_read_Flag = 0;
					Encoder_time = 0;
					key_time = 0;
				}
			}
		}
	}
	if (SW_flag4 == 1)
	{
		Encoder_time++;
		if (Encoder_time > Double_click_interval * 5) // Double-click valid time exceeded
		{
			SW_flag4 = 1;
			SW_flag = 2;
		}
	}
	if (Enc1)
	{
		Encoder_Freetime++;
		if (Encoder_Freetime > 100)
		{
			Encoder_Freetime = 0;
			knob_dir = 0;
			Enc1 = 0;
		}
	}
#endif

	}
}
#endif

void PIT2_Init(UINT16 arr)
{
	NVIC_Init(2, 0, PIT2_IRQn, 2);

	PIT2->PCSR &= (~PCSR_EN);
	PIT2->PCSR = (0 << 8) | PCSR_OVW | PCSR_PIE | PCSR_RLD | PCSR_PDBG; // 75mhz/1  Interrupt mode
	PIT2->PMR = arr - 3;

	PIT2->PCSR &= ~PCSR_EN;
}

void PIT2_IRQHandler(void)
{
}

// Disable timer 14
void LT_UsartTimeDisable(void)
{
	PIT1->PCSR &= ~PCSR_PIE;
	PIT1->PCSR &= ~PCSR_EN;
}

// Enable timer 14
void LT_UsartTimeEnable(void)
{

	PIT1->PCSR |= PCSR_PIE;
	PIT1->PCSR |= PCSR_EN;
}

// Set timer 14 load value
void LT_UsartTimeSetCounter(UINT32 Counter)
{
	PIT1->PMR = Counter;
	PIT1->PCSR |= PCSR_PIF;
	LT_UsartTimeEnable();
}