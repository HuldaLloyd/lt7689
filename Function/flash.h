/*******************************************************************************
  * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
  * @file     flash.h
  * @author   UartTFT Application Team
  * @version  V1.0.0
  * @date     2023-02-24
  * @brief
 ********************************************************************************/

#ifndef _FLASH_h
#define _FLASH_h
#include "sys.h"
#include "dmac_drv.h"

extern DMA_CHANNEL_REG *spi1_dma_channel[];
extern DMA_CONTROL_REG *spi1_dma_control;

void MCU_SS_RESET(void);
void MCU_SS_SET(void);
extern uint8_t Flash_CS;
#define Flash_SS_RESET 	MCU_SS_RESET()
#define Flash_SS_SET 	MCU_SS_SET()
#define MCU_CS0_RESET 	EPORT_WriteGpioData(EPORT_PIN3, Bit_RESET);//EPORT_WriteGpioData(EPORT_PIN1, Bit_RESET);
#define MCU_CS0_SET 	EPORT_WriteGpioData(EPORT_PIN3, Bit_SET);//EPORT_WriteGpioData(EPORT_PIN1, Bit_SET); 
#define MCU_CS0_OUTPUT 	EPORT_ConfigGpio(EPORT_PIN3, GPIO_OUTPUT);
#define MCU_CS0_INPUT 	EPORT_ConfigGpio(EPORT_PIN3, GPIO_INPUT);
#define MCU_CS1_RESET 	SPI1->U8SPIPORT.SS = 0
#define MCU_CS1_SET 	SPI1->U8SPIPORT.SS = 1

void Close_SPI1(void);
void SPI1_Init(void);
void Mcu_Write_SR(uint8_t reg, uint8_t val);
uint8_t Mcu_ReadSR(uint8_t reg);
void Flash_Read_UI(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void Flash_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void Mcu_ReadPageAddr_Data(uint8_t *pBuffer, uint16_t ColumnAddr, uint32_t PageAddr, uint16_t NumByteToRead);
uint16_t Mcu_ReadID(void);
void Flash_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
#endif
