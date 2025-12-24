/*******************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     w25n01.h
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief	 Nand Flash Library Functions
 ********************************************************************************/
 
#ifndef _w25n01_h
#define _w25n01_h
#include "sys.h"

#define W25_DEVICE_ID 0XEFAA21

#define W25N01GV_ID 0x9f
#define W25N01GV_ReadStatusReg 0x0f  
#define W25N01GV_WriteStatusReg 0x1f 
#define W25N01GV_WriteEnable 0x06
#define W25N01GV_WriteDisable 0x04

uint32_t W25N01GV_ReadID(void);
void W25N01GV_Wait_Busy(void);
void W25N01GV_Erase_Block(uint16_t block);
void W25N01GV_WritePageAddr_Data(uint8_t *pBuffer, uint32_t PageNum, uint32_t PageAddr, uint16_t NumByteToWrite);
void W25N01GV_ReadPageAddr_Data(uint8_t *pBuffer, uint32_t PageNum, uint32_t PageAddr, uint16_t NumByteToRead);
void LT_W25N01_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void Write_Page(uint16_t page);
void W25N01GV_ContinuousRead_Mode(void);
void W25N01GV_BUF1Set(void);
void W25QXX_Erase_128k(void);

#endif
