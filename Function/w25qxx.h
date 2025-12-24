/*******************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     w25qxx.h
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief	 Nor Flash Library Functions
 ********************************************************************************/


#ifndef _w25qxx_h
#define _w25qxx_h

#include "LT768.h"
#include "type.h"

// Instruction list
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg 0x05
#define W25X_WriteStatusReg 0x01
#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B
#define W25X_FastReadDual 0x3B
#define W25X_PageProgram 0x02
#define W25X_BlockErase 0xD8
#define W25X_SectorErase 0x20
#define W25X_ChipErase 0xC7
#define W25X_PowerDown 0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID 0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID 0x9F

uint16_t LT_W25qxx_ReadID(void);
void LT_W25qxx_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void LT_W25qxx_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void LT_W25qxx_Erase_Chip(void);
void LT_W25qxx_Erase_Sector(uint32_t Dst_Addr);
void LT_W25qxx_BlockErase64KB(uint32_t addr);
void LT_W25QXX_Enter_4Byte_AddressMode(void);

extern uint8_t W25Q256;
#endif
