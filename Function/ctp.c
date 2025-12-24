/************************************************************************
  * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
  * @file     ctp.c
  * @author   UartTFT Application Team
  * @version  V1.0.0
  * @date     2023-02-24
  * @brief     
*************************************************************************/


#include "ctp.h"
#include "i2c_drv.h"
#include "eport_drv.h"
#include "spi_drv.h"
#include "tp_drv.h"
#include "cst3xx.h"
#include "iic.h"

uint8_t fingerNumber = 0;
uint8_t ctp_active_index = 0;
uint8_t ctp_press_t;

void Checksum(uint8_t *p) // Configuration information check 0x80ff (checked algorithm is correct)
{
	int i = 0;
	unsigned int sum_buffer = 0;

	for (; i < 184; i++)
	{
		sum_buffer += *(p + i);
	}

	*(p + 184) = ~sum_buffer + 1;
	//	printf("%x\r\n", *(p + 184));
}



/* Initialization function of IO port related to capacitive touch screen */
void CTP_Init(void)
{
	CTP_io_Init();
	gTpInfo.scan = FT5216_Scan;
}


/***********************************************************************************
* Function Name	: CT_Read_Nbyte
* Description	: Read touch screen register
* Input			: - sla_add : Device address
				  - t_add : Register address to be read
				  - n : Number of data to be read
				  - *s : Address for storing data to be read
* Output		: None
* Return		: Success:1 Fail:0
************************************************************************************/
uint8_t CT_Read_Nbyte(const uint8_t sla_add, const uint16_t add, uint16_t n, uint8_t *s) 
{
	uint8_t temps;
	uint16_t tempn;
	I2C_start(); // Start CTI2C bus
	I2C_write_byte(sla_add | CT_WRITE_MASK);

	if (gt911flag == 1)
		I2C_write_byte(add >> 8);


	I2C_write_byte(add);
	I2C_stop();
	I2C_start(); // Start CTI2C bus again
	I2C_write_byte(sla_add | CT_READ_MASK);
	for (tempn = 0; tempn < n; tempn++)
	{
		/*If you have read all the data, do not send the reply signal, but send the non-reply bit and stop bit directly */
		if (tempn + 1 < n) 
		{
			temps = I2C_read_byte(0);
			*(s + tempn) = temps;
		}
	}
	temps = I2C_read_byte(1);
	*(s + tempn) = temps;
	I2C_stop();
	return CT_COM_OK;
}


/***********************************************************************************
* Function Name	: CT_Write_Nbyte
* Description	: Write touch screen register
* Input			: - sla_add : Device address
				  - t_add : Register address to be written
				  - n : Number of data to be written
				  - *s : Address to store data to be written
* Output		: None
* Return		: Success:1 Fail:0
************************************************************************************/
uint8_t CT_Write_Nbyte(const uint8_t sla_add, const uint16_t add, uint16_t n, const uint8_t *s) 
{

	uint16_t tempn;
	I2C_start();							 // Start TP bus
	I2C_write_byte(sla_add | CT_WRITE_MASK); // Send addressing bytes

	if (gt911flag == 1)
		I2C_write_byte(add >> 8);

	I2C_write_byte(add); // Send the starting address to write

	for (tempn = 0; tempn < n; tempn++)
		I2C_write_byte(s[tempn]);
	/*debug
	for(tempn=0;tempn<n;tempn++)
	{
		ack=1;//Answer bit
		cack_time=0;
		temps=*(s+tempn);
		while(ack)
		{
			ack= I2C_write_byte(temps);
			cack_time++;
			if(cack_time>CT_CACK_TIMEOUT)//在规定时间cack_timeout内收不到应答信号，返回出错信号
			return CT_ACK_FAIL;
		}
	}
	*/
	I2C_stop(); // TP stop
	return CT_COM_OK;
}

//---------------------------------Clear write and read buffers-------------------------//
void ClearBuff(uint8_t *p)
{
	int i = 0;
	for (; i < 20; i++)
	{
		*(p + i) = 0;
	}
}
//----------------------------------------------------------------------------//

/*********************************Read XY axis function***************************************/
void TP_read_XY(void)
{
	uint16_t x, y;
	uint8_t i = 0;
	uint8_t READ_BUFF[10];
	uint8_t mode,temp;

	if (ft5216flag == 1)
	{
		I2C_start(); // Start CTI2C bus
		I2C_write_byte(0x70);
		I2C_write_byte(0x02);
		I2C_stop();

		I2C_start(); // Start CTI2C bus again
		I2C_write_byte(0x71);
		fingerNumber = I2C_read_byte(0) & 0xF; // ACK

		for (i = 0; i < 6; i++)
			READ_BUFF[i] = I2C_read_byte(0);
		I2C_read_byte(1); // NACK
		I2C_stop();

		if (READ_BUFF[0] & 0x80)
		{
			x = READ_BUFF[0] & 0xF; // Calculate X axis
			x = x << 8;
			x += READ_BUFF[1];

			y = READ_BUFF[2] & 0xF;
			y = y << 8;
			y += READ_BUFF[3];

			Backlight_count = 0;
			Backlight_touch = 1;
			if (Backlight_flag == 0 && var[2 * 0x700e + 1] == 1)
			{
				if (Backlight_first_touch == 0)
				{
					if (buzzer)
					touch_buzzer();
					I2C_start(); // Start CTI2C bus
					I2C_write_byte(0x70);
					I2C_write_byte(0x02);
					I2C_stop();

					I2C_start(); // Start CTI2C bus again
					I2C_write_byte(0x71);
					fingerNumber = I2C_read_byte(0) & 0xF; // ACK

					for (i = 0; i < 6; i++)
						READ_BUFF[i] = I2C_read_byte(0);
					I2C_read_byte(1); // NACK
					I2C_stop();
					if (READ_BUFF[0] & 0x40)
					{
						Backlight_touch = 0;
						gTpInfo.sta = 0;
						First_press = 0;
					}
				}
				Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
				if (var[0x700f * 2 + 1] == 0)
					Start_PWM1();
				Backlight_first_touch = 1;
			}
			else
			{
				gTpInfo.sta = 1;
				Backlight_touch = 0;
				gTpInfo.x[0] = x;
				gTpInfo.y[0] = y;
				if (gTpInfo.x[0] > LCD_XSIZE_TFT)
					gTpInfo.x[0] = LCD_XSIZE_TFT;
				if (gTpInfo.y[0] > LCD_YSIZE_TFT)
					gTpInfo.y[0] = LCD_YSIZE_TFT;


				if (First_press == 0)
				{
					First_press = 1;
					First_pressX = gTpInfo.x[0];
					First_pressY = gTpInfo.y[0];
				}
			}
			//printf("x=%d y=%d \r\n",gTpInfo.x[0],gTpInfo.y[0]);
		}
		else if (READ_BUFF[0] & 0x40)
		{
			Backlight_touch = 0;
			gTpInfo.sta = 0;
			First_press = 0;
		}

	}

	if (gt911flag == 1)
	{
		I2C_start();
		I2C_write_byte(0x28);
		I2C_write_byte(0x81);
		I2C_write_byte(0x4E);
		I2C_stop();

		I2C_start(); 
		I2C_write_byte(0x29);

		for (i = 0; i < 8; i++)
		{
			READ_BUFF[i] = I2C_read_byte(0);
		}
		READ_BUFF[i] = I2C_read_byte(1);
		I2C_stop();

		if (READ_BUFF[0] & 0x80) // buffer status Data is valid only when it is 1
		{
			x = READ_BUFF[3];
			x = x << 8 | READ_BUFF[2];
			y = READ_BUFF[5];
			y = y << 8 | READ_BUFF[4];

			Backlight_count = 0;
			Backlight_touch = 1;

			if (Backlight_flag == 0 && var[2 * 0x700e + 1] == 1)
			{

				if (Backlight_first_touch == 0)
				{
					if (buzzer)
						touch_buzzer();
				}

				if ((READ_BUFF[0] & 0x0F) == 0)
				{
					Backlight_touch = 0;
					gTpInfo.sta = 0;
					First_press = 0;
					Backlight_flag = 1;
				}
				Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
				if (var[0x700f * 2 + 1] == 0)
					Start_PWM1();
				Backlight_first_touch = 1;
			}
			else
			{
				Backlight_touch = 0;
				gTpInfo.x[0] = x * LCD_XSIZE_TFT / X_Output_Max;
				gTpInfo.y[0] = y * LCD_YSIZE_TFT / Y_Output_Max;
				gTpInfo.sta = READ_BUFF[0] & 0x0F;
				if (gTpInfo.x[0] > LCD_XSIZE_TFT)
					gTpInfo.x[0] = LCD_XSIZE_TFT;
				if (gTpInfo.y[0] > LCD_YSIZE_TFT)
					gTpInfo.y[0] = LCD_YSIZE_TFT;
				if (gTpInfo.sta == 1)
				{
					if (First_press == 0)
					{
						First_press = 1;
						First_pressX = gTpInfo.x[0];
						First_pressY = gTpInfo.y[0];
					}
				}
				else
					First_press = 0;
//				 printf("x=%d y=%d \r\n",gTpInfo.x[0],gTpInfo.y[0]);
			}

			I2C_start();
			I2C_write_byte(0x28);
			I2C_write_byte(0x81);
			I2C_write_byte(0x4E);
			I2C_write_byte(0x00);
			I2C_stop();
		}
	
	}
	if(CST3240flag)
	{
		i2c_start();
		i2c_write(0xb4);
		i2c_write(0xD0);
		i2c_write(0x00);
		i2c_stop();
		
		i2c_start();
		i2c_write(0xb4 + 1);
		
		for (i = 0; i < 4; i++)
			READ_BUFF[i] = i2c_read(1);
			
		i2c_read(0);	
		i2c_stop();
		
//		for (i = 0; i < 8; i++)
//			printf("READ_BUFF[%d]:0x%x\r\n",i,READ_BUFF[i]);

		if ((READ_BUFF[0] & 0x0f) == 0x06)
		{
			x = READ_BUFF[3] & 0xF0; // Calculate X axis
			x = x >> 4;
			x += (READ_BUFF[1] << 4);

			y = READ_BUFF[3] & 0x0F;
//			y = y << 8;
			y += (READ_BUFF[2] << 4);

			Backlight_count = 0;
			Backlight_touch = 1;
			if (Backlight_flag == 0 && var[2 * 0x700e + 1] == 1)
			{
				if (Backlight_first_touch == 0)
				{
					if (buzzer)
						touch_buzzer();
					i2c_start();
					i2c_write(0xb4);
					i2c_write(0xD0);
					i2c_write(0x00);
					i2c_stop();
					
					i2c_start();
					i2c_write(0xb4 + 1);
					
					for (i = 0; i < 4; i++)
						READ_BUFF[i] = i2c_read(1);
						
					i2c_read(0);	
					i2c_stop();
					
					if ((READ_BUFF[0] & 0x0f) == 0x00)
					{
						Backlight_touch = 0;
						gTpInfo.sta = 0;
						First_press = 0;
					}
				}
				Set_Timer1_Compare_Buffer(gDutyBuf[var[0x7001 * 2 + 1]]);
				if (var[0x700f * 2 + 1] == 0)
					Start_PWM1();
				Backlight_first_touch = 1;
			}
			else
			{
				gTpInfo.sta = 1;
				Backlight_touch = 0;
				gTpInfo.x[0] = x;
				gTpInfo.y[0] = y;
				if (gTpInfo.x[0] > LCD_XSIZE_TFT)
					gTpInfo.x[0] = LCD_XSIZE_TFT;
				if (gTpInfo.y[0] > LCD_YSIZE_TFT)
					gTpInfo.y[0] = LCD_YSIZE_TFT;
//				printf("gTpInfo.x[0]:%d    gTpInfo.y[0]:%d\r\n",gTpInfo.x[0],gTpInfo.y[0]);

				if (First_press == 0)
				{
					First_press = 1;
					First_pressX = gTpInfo.x[0];
					First_pressY = gTpInfo.y[0];
				}
			}
		}
		else if ((READ_BUFF[0] & 0x0f) == 0x00)
		{
			Backlight_touch = 0;
			gTpInfo.sta = 0;
			First_press = 0;
		}
	}
}

uint8_t FT5216_Scan(void)
{
	uint8_t i;
	uint8_t READ_BUFF[10];
	if (ctp_active_index == 1)
	{
		ctp_press_t = 0;
		TP_read_XY();
		ctp_active_index = 0;
		return 1;
	}
	else
	{
	if (ft5216flag == 1)
	{
		if (gTpInfo.sta == 1 && ctp_press_t > 2)
		{
			I2C_start(); 
			I2C_write_byte(0x70);
			I2C_write_byte(0x02);
			I2C_stop();

			I2C_start(); 
			I2C_write_byte(0x71);
			fingerNumber = I2C_read_byte(0) & 0xF; // ACK

			for (i = 0; i < 6; i++)
				READ_BUFF[i] = I2C_read_byte(0);
			I2C_read_byte(1); // NACK
			I2C_stop();
			if (READ_BUFF[0] & 0x40)
				gTpInfo.sta = 0;
		}
	}

		return 0;
	}
}
#if IIC_BUS
void I2C_Slave_TEST(uint8_t sla_addr)
{
	I2C->CR = 0x0;
	I2C->CR |= CR_EN;

	delay(100);

	I2C->CR |= CR_ACKEN;

	I2C->CR |= CR_IEN;
	I2C->CR &= ~CR_MSMOD;

	I2C->SAHR = sla_addr << 1;
	/*Configure interrupts in slave mode*/
	NVIC_Init(3, 3, I2C_1_IRQn, 2);
}

uint16_t Rx_len = 0, Tx_len = 0;
uint16_t reg_addr = 0;
uint8_t flag_IIC_REG[20];
uint16_t ID_IIC_WAV = 0;

// IIC slave transceiver interrupt function
void I2C1_IRQHandler(void)
{
	volatile uint8_t tmp = 0;
	static uint16_t curve_rev_val = 0;
	static uint8_t curve_rev_num = 0;
	static uint8_t curve_rev_tmp = 0;
	static uint16_t curve_rev_data = 0;
	static uint8_t curve_rev_cnt = 0;

	uint8_t i = 0;

	uint8_t i2c_status, i2c_hs_status;

	i2c_status = I2C->CSR;
	i2c_hs_status = I2C->SHSIR;

	if ((i2c_hs_status & SLV_HS) != SLV_HS)
	{
		if ((i2c_status & SR_AASLV) == SR_AASLV) // The current slave device is addressed by master  Slave address matching
		{
			I2C->CR &= ~CR_AMIE;			   // Under normal circumstances, turn off the slave address matching interrupt
			if ((i2c_status & SR_RC) == SR_RC) // Receiving is interrupted. The received address and data will come in
			{
				Tx_len = 0;
				if ((I2C->CSR & SR_TF) == SR_TF) // Transmission completion interrupt
				{
					if (Rx_len == 0) // Upper 8 bits of receiving address
					{
						reg_addr = I2C->CDR;
					}
					else if (Rx_len == 1) // Lower 8 bits of receiving address
					{
						reg_addr = (reg_addr << 8) | I2C->CDR;
						if (reg_addr >= 0xC001 && reg_addr <= 0xCFFF) // Judge whether it is the curve buffer address, special processing
						{
							curve_rev_val = reg_addr & 0xFFF;
							curve_rev_cnt = 0;
							curve_rev_num = 0;
							for (i = 0; i < Curve_Size; i++)
							{
								if (curve_rev_val & (1 << i))
									curve_rev_num++; // Record how many groups of channels
							}
						}
					}
					else // receive data
					{
						if (reg_addr <= 0x6FFF) // In the variable area
						{
							// Direct saving of variable data
							var[2 * reg_addr + Rx_len - 2] = I2C->CDR;
						}
						else if (reg_addr >= 0x7000 && reg_addr <= 0x71FF) // Register region
						{
							// WAV register data needs special processing
							if (2 * reg_addr + Rx_len - 2 == 2 * 0x700a)
							{
								ID_IIC_WAV = I2C->CDR;
							}
							else if (2 * reg_addr + Rx_len - 2 == 2 * 0x700a + 1)
							{
								ID_IIC_WAV = (ID_IIC_WAV << 8) | I2C->CDR;
							}
							// The remaining register data is saved directly
							else
							{
								var[2 * reg_addr + Rx_len - 2] = I2C->CDR;
							}

							if (Rx_len % 2) // The update flag of the corresponding register will be triggered every time 2Bytes are received
							{
								flag_IIC_REG[(2 * (reg_addr - 0x7000) + Rx_len - 2) / 2] = 1;
							}
						}
						else if (reg_addr >= 0xC001 && reg_addr <= 0xCFFF) // Curve channel write data
						{
							for (i = curve_rev_cnt; i < Curve_Size; i++)
							{
								if (curve_rev_val & (1 << i)) // Poll to detect whether 8 channels have triggered updates
								{
									if (Rx_len % 2 == 0)
									{
										curve_rev_data = (I2C->CDR) << 8; // Receive the first byte of curve data
									}
									else if (Rx_len % 2 == 1)
									{
										Curve_buff[i][Curve_count[i]] = curve_rev_data | (I2C->CDR); // Receive the second byte of curve data
										Curve_count[i]++;
										if (Curve_count[i] >= 1024)
										{
											Curve_count[i] = 0;
										}
										curve_update_flag[i] = 1;

										curve_rev_cnt++;
										if (curve_rev_cnt >= curve_rev_num)
											curve_rev_cnt = 0;
									}
									break;
								}
							}
						}
						else // Other addresses only receive but do not process
						{
							i = I2C->CDR;
						}
					}

					Rx_len++;
					I2C->CR |= CR_ACKEN;
				}
				else // Received address
				{
					Rx_len = 0;
					I2C->CR |= CR_ACKEN;
				}
			}
			else // No receive interrupt, read signal received
			{
				if ((i2c_status & SR_TF) == SR_TF) // Transmission completion interrupt
				{
					Rx_len = 0;
					if ((i2c_status & SR_DACK) == SR_DACK)
					{
						// Direct feedback data
						if (reg_addr <= 0x6FFF) // In the variable area
						{
							I2C->CDR = var[2 * reg_addr + Tx_len];
						}
						else if (reg_addr >= 0x7000 && reg_addr <= 0x71FF) // In register area
						{
							I2C->CDR = var[2 * reg_addr + Tx_len];
						}
						else
						{
							I2C->CDR = 0xFF;
						}

						Tx_len++;
					}
					else
					{
						tmp = I2C->CDR;
						tmp++; // add for warning
					}
				}
				else
				{
					;
				}
			}
		}
		else
		{
			// asm("bkpt");
		}
	}
	else
	{
		I2C->SSHTR = (I2C->SSHTR & 0xc0) | 0x1;
		I2C->SSHTR = I2C->SSHTR | 0x1;

		I2C->SHSIR |= SLV_HS;
	}
}
#endif
