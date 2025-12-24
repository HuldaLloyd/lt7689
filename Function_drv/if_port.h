/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     if_port.h
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief    Different drive interfaces
 *************************************************************************/

#ifndef _if_port_h
#define _if_port_h
#include "sys.h"
#include "spi_drv.h"
#include "eport_drv.h"

//-------- Only one peripheral can be opened ----------
#define IF_SPI 1 // Use SPI to drive LT768

// Select SPI drive
#if IF_SPI
#define SS_RESET EPORT_WriteGpioData(EPORT_PIN12, 0)
#define SS_SET EPORT_WriteGpioData(EPORT_PIN12, 1)

uint16_t SPI2_ReadWriteByte(uint16_t TxData);
void SPI2_WriteByte(uint16_t TxData);
#endif

void Parallel_Init(void);

void LCD_CmdWrite(uint8_t cmd);
void LCD_DataWrite(uint8_t data);
void LCD_DataWrite_Pixel(uint16_t data);
uint8_t LCD_StatusRead(void);
uint16_t LCD_DataRead(void);

void Delay_us(uint16_t time);
void Delay_ms(uint16_t time);

void SPI_CmdWrite(uint8_t cmd);
void SPI_DataWrite(uint8_t data);
uint16_t SPI_DataRead(void);
uint8_t SPI_StatusRead(void);

#endif
