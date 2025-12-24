/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     iic.c
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief
 *************************************************************************/

#include "iic.h"

#define TIMEOUT 50000
#define hwI2C0_Device_Addr 0x28

extern uint8_t bI2C0_TxM_Data[16];
extern uint8_t bI2C0_RxM_Data[128];
uint8_t IIC_Buf[128];

uint32_t u32Tmp;
uint8_t tmpID, tmpEvent;
uint32_t u16Tmp, tmpX, tmpY;
uint16_t tpX[10], tpY[10];
uint32_t TPmaxX, TPmaxY;
uint8_t tpActive;
void i2c_delay(uint8_t us)
{
	uint8_t i;
	for (i = 0; i < us; i++)
		asm("NOP");
}

void I2cInit(void)
{
	I2C_ConfigGpio(I2C_SDA, GPIO_OUTPUT);
	I2C_ConfigGpio(I2C_SCL, GPIO_OUTPUT);
	SPI_ConfigGpio(SPI3, SPI_SS, GPIO_OUTPUT);

	EPORT_ConfigGpio(EPORT_PIN4, GPIO_INPUT);
}

void i2c_sda_output(void)
{
	I2C_ConfigGpio(I2C_SDA, GPIO_OUTPUT);
}

void i2c_sda_input(void)
{
	I2C_ConfigGpio(I2C_SDA, GPIO_INPUT);
}

void i2c_set_scl(uint8_t index)
{
	if (index)
		I2C_WriteGpioData(I2C_SCL, 1);
	else
		I2C_WriteGpioData(I2C_SCL, 0);
}

void i2c_set_sda(uint8_t index)
{
	if (index)
		I2C_WriteGpioData(I2C_SDA, 1);
	else
		I2C_WriteGpioData(I2C_SDA, 0);
}

uint8_t i2c_get_sda(void)
{
	if (I2C_ReadGpioData(I2C_SDA))
		return 1;
	else
		return 0;
}

void i2c_start(void)
{
	i2c_sda_output();
	i2c_set_sda(1);
	i2c_set_scl(1);
	DelayUS(20);
	i2c_set_sda(0);
	DelayUS(2);
	i2c_set_scl(0);
	DelayUS(2);
}

void i2c_stop(void)
{
	i2c_sda_output();
	i2c_set_scl(0);
	i2c_set_sda(0);
	i2c_set_scl(1);
	DelayUS(2);
	i2c_set_sda(1);
	DelayUS(2);
}

uint8_t i2c_write(uint8_t value)
{
	uint8_t i, j, ack;

	i2c_sda_output();
	i2c_set_scl(0);
	for (i = 0; i < 8; i++)
	{
		if (value & 0x80)
			i2c_set_sda(1);
		else
			i2c_set_sda(0);

		DelayUS(2);
		i2c_set_scl(1);
		DelayUS(3);
		i2c_set_scl(0);
		value = value << 1;
	}
	i2c_sda_input();
	DelayUS(20);
	i2c_set_scl(1);
	DelayUS(1);
	ack = i2c_get_sda();
	i2c_set_scl(0);

	return ack;
}

uint8_t i2c_read(uint8_t ack)
{
	uint8_t i, j, u8Tmp;

	i2c_sda_input();
	u8Tmp = 0x00;
	for (i = 0; i < 8; i++)
	{
		u8Tmp = u8Tmp << 1;

		i2c_set_scl(0);
		DelayUS(3);
		i2c_set_scl(1);
		DelayUS(2);
		if (i2c_get_sda())
			u8Tmp |= 0x01;
	}

	i2c_set_scl(0);
	i2c_sda_output();

	if (ack)
		i2c_set_sda(0);
	else
		i2c_set_sda(1);

	DelayUS(20);
	i2c_set_scl(1);
	DelayUS(2);
	i2c_set_scl(0);

	return u8Tmp;
}

void read_xy(void)
{
	uint8_t i;
	uint32_t crcVal;
	i2c_start();
	i2c_write(0x82);
	i2c_write(0x10);
	i2c_stop();

	i2c_start();
	i2c_write(0x82 + 1);

	for (i = 0; i < 31; i++)
	{
		IIC_Buf[i] = i2c_read(1);
	}

	i2c_read(0);
	i2c_stop();

	u32Tmp = IIC_Buf[1] & 0x7F;
	u32Tmp = (u32Tmp << 8) & 0xFF00;
	u32Tmp += IIC_Buf[2];
	tmpX = u32Tmp;

	u32Tmp = IIC_Buf[3] & 0x7F;
	u32Tmp = (u32Tmp << 8) & 0xFF00;
	u32Tmp += IIC_Buf[4];
	tmpY = u32Tmp;
}