/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     if_port.c
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief    Different drive interfaces
 *************************************************************************/

#include "if_port.h"

// ------------------------------------------------------------ SPI drive --------------------------------------------------------------------

#if IF_SPI

void SPI2_Init(void)
{
	SPI_InitTypeDef SPI_InitStruct;

	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;

	// According to SPI_ The parameter specified in InitStruct initializes the peripheral SPIx registe
	SPI_Init(SPI2, &SPI_InitStruct);
	SPI_Cmd(SPI2, ENABLE);

	EPORT_ConfigGpio(EPORT_PIN12, GPIO_OUTPUT);
}

uint16_t SPI2_ReadWriteByte(uint16_t TxData)
{

	while ((SPI2->SPISRHW) & (SPISR_TXFFULL_MASK))	// The waiting sending area is empty
	{
	}
	SPI2->SPIDR = TxData;	// Send a byte

	while ((SPI2->SPISRHW) & (SPISR_RXFEMP_MASK))	// Wait for receiving a byte
	{
	}

	return SPI2->SPIDR;		// Return received data
}
void SPI2_WriteByte(uint16_t TxData)
{
	UINT8 uint8_t;

	SPI2->SPIDR = TxData;	// Send a byte

	while ((SPI2->SPISRHW) & (SPISR_RXFEMP_MASK))	// Wait for receiving a byte
	{
	}

	uint8_t = SPI2->SPIDR;
}
void SPI_CmdWrite(uint8_t cmd)
{
	SS_RESET;
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(cmd);
	SS_SET;
}

void SPI_DataWrite(uint8_t data)
{
	SS_RESET;
	SPI2_ReadWriteByte(0x80);
	SPI2_ReadWriteByte(data);
	SS_SET;
}

void SPI_DataWrite_Pixel(uint16_t data)
{
	SPI2_ReadWriteByte(data);
	SPI2_ReadWriteByte(data >> 8);
}

uint8_t SPI_StatusRead(void)
{
	uint8_t temp = 0;
	SS_RESET;
	SPI2_ReadWriteByte(0x40);
	temp = SPI2_ReadWriteByte(0xff);
	SS_SET;
	return temp;
}

uint16_t SPI_DataRead(void)
{
	uint16_t temp = 0;
	SS_RESET;
	SPI2_ReadWriteByte(0xc0);
	temp = SPI2_ReadWriteByte(0xff);
	SS_SET;
	return temp;
}
#endif

//-----------------------------------------------------------------------------------------------------------------------------------

void LCD_CmdWrite(uint8_t cmd)
{
	SS_RESET;
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(cmd);
	SS_SET;
}

void LCD_DataWrite(uint8_t data)
{
	SS_RESET;
	SPI2_ReadWriteByte(0x80);
	SPI2_ReadWriteByte(data);
	SS_SET;
}

void LCD_DataWrite_Pixel(uint16_t data)
{
	SPI2_ReadWriteByte(data);
	SPI2_ReadWriteByte(data >> 8);
}

uint8_t LCD_StatusRead(void)
{
	uint8_t temp = 0;

#if IF_SPI
	temp = SPI_StatusRead();
#endif

	return temp;
}

uint16_t LCD_DataRead(void)
{
	uint16_t temp = 0;

	SS_RESET;
	SPI2_ReadWriteByte(0xc0);
	temp = SPI2_ReadWriteByte(0xff);
	SS_SET;

	return temp;
}

void Delay_us(uint16_t time)
{
	uint16_t i = 0;
	while (time--)
	{
		i = 12; // Self-defined
		while (i--);
	}
}

void Delay_ms(uint16_t time)
{
	uint16_t i = 0;
	while (time--)
	{
		i = 12000; // Self-defined
		while (i--);
	}
}

void Parallel_Init(void)
{

#if IF_SPI
	SPI2_Init();
#endif
}
