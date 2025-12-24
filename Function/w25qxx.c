/*******************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     w25qxx.c
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief	 Nor Flash Library Functions
 ********************************************************************************/

#include "w25qxx.h"
uint8_t W25Q256 = 0;

/********************************************************************************
* Function Name	: LT_W25qxx_ReadSR
* Description  	: Read the value of the register.
* Input        	: - SRx:Register address
				  - 3 status registers
				  - W25X_READ_SR1: address 0x05
							 BIT   7   6   5   4   3   2   1   0
								  SPR  RV TB  BP2  BP1 BP0 WEL BUSY
				  - W25X_READ_SR2: address 0x35
				  - W25X_READ_SR3: address 0x15
* Output       	: None
* Return       	: None
*********************************************************************************/
uint8_t LT_W25qxx_ReadSR(void)
{
	uint8_t byte = 0;
	nSS_Active();
	SPI_Master_FIFO_Data_Put(W25X_ReadStatusReg);
	byte = SPI_Master_FIFO_Data_Put(0Xff);
	nSS_Inactive();
	return byte;
}

/********************************************************************************
 * Function Name	: LT_W25qxx_Write_Enable
 * Description  	: Write Enable(set WEL to 1).
 * Input        	: None
 * Output       	: None
 * Return       	: None
 *********************************************************************************/
void LT_W25qxx_Write_Enable(void)
{
	nSS_Active();
	SPI_Master_FIFO_Data_Put(W25X_WriteEnable);
	nSS_Inactive();
}

/********************************************************************************
 * Function Name	: LT_W25QXX_Write_Disable
 * Description  	: Write Disable
 * Input        	: None
 * Output       	: None
 * Return       	: None
 *********************************************************************************/

void LT_W25QXX_Write_Disable(void)
{
	nSS_Active();
	SPI_Master_FIFO_Data_Put(W25X_WriteDisable);
	nSS_Inactive();
}

/********************************************************************************
 * Function Name	: LT_W25qxx_Wait_Busy
 * Description  	: Wait for BUSY bit to clear.
 * Input        	: None
 * Output       	: None
 * Return       	: None
 *********************************************************************************/
void LT_W25qxx_Wait_Busy(void)
{
	while ((LT_W25qxx_ReadSR() & 0x01) == 0x01)
		;
}

/********************************************************************************
 * Function Name	: LT_W25qxx_ReadID
 * Description  	: Read W25qxx ID.
 * Input        	: None
 * Output       	: None
 * Return       	: Flash ID
 *********************************************************************************/
uint16_t LT_W25qxx_ReadID(void)
{
	uint16_t Temp = 0;
	nSS_Active();
	SPI_Master_FIFO_Data_Put(W25X_ManufactDeviceID);
	SPI_Master_FIFO_Data_Put(0x00);
	SPI_Master_FIFO_Data_Put(0x00);
	SPI_Master_FIFO_Data_Put(0x00);
	Temp |= SPI_Master_FIFO_Data_Put(0xFF) << 8;
	Temp |= SPI_Master_FIFO_Data_Put(0xFF);
	nSS_Inactive();
	return Temp;
}

/********************************************************************************
* Function Name	: LT_W25qxx_Read
* Description  	: Read data of specified length from the specified address.
* Input        	: - pBuffer: buffer for save data
				  - ReadAddr: Start address of reading data
				  - NumByteToRead: number bytes to read
* Output       	: None
* Return       	: None
*********************************************************************************/
void LT_W25qxx_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	uint16_t i;
	nSS_Active();
	SPI_Master_FIFO_Data_Put(W25X_ReadData);

	if (W25Q256)
		SPI_Master_FIFO_Data_Put((uint8_t)((ReadAddr) >> 24));
	SPI_Master_FIFO_Data_Put((uint8_t)((ReadAddr) >> 16));
	SPI_Master_FIFO_Data_Put((uint8_t)((ReadAddr) >> 8));
	SPI_Master_FIFO_Data_Put((uint8_t)ReadAddr);
	for (i = 0; i < NumByteToRead; i++)
	{
		pBuffer[i] = SPI_Master_FIFO_Data_Put(0XFF);
	}
	nSS_Inactive();
}

/********************************************************************************
* Function Name	: LT_W25QXX_Write_Page
* Description  	: On a page, write data of specified length at the specified address.
* Input        	: - pBuffer: data start address
				  - WriteAddr: Start address of writing data
				  - NumByteToWrite: number bytes to write(max one page:256 bytes)
* Output       	: None
* Return       	: None
*********************************************************************************/

void LT_W25QXX_Write_Page(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint16_t i;
	LT_W25qxx_Write_Enable();
	nSS_Active();
	SPI_Master_FIFO_Data_Put(W25X_PageProgram);

	if (W25Q256)
		SPI_Master_FIFO_Data_Put((uint8_t)((WriteAddr) >> 24));

	SPI_Master_FIFO_Data_Put((uint8_t)((WriteAddr) >> 16));
	SPI_Master_FIFO_Data_Put((uint8_t)((WriteAddr) >> 8));
	SPI_Master_FIFO_Data_Put((uint8_t)WriteAddr);

	LCD_CmdWrite(0xB8);
	SS_RESET;
	SPI2_ReadWriteByte(0x80);

	for (i = 0; i < NumByteToWrite; i++)
	{
		SPI2_ReadWriteByte(pBuffer[i]);
	}
	SS_SET;
	nSS_Inactive();
	LT_W25qxx_Wait_Busy();
}

/********************************************************************************
* Function Name	: LT_W25qxx_Write
* Description  	: Write SPI FLASH without inspection.
				  You must ensure that all the data written in the address range is 0XFF, otherwise the data written in non-0XFF will fail.
				  With auto-feed function
* Input        	: - LT_W25qxx_Write: data start address
				  - WriteAddr: Start address of writing data
				  - NumByteToWrite: number bytes to write(max 0xFFFF bytes)
* Output       	: None
* Return       	: None
*********************************************************************************/
void LT_W25qxx_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint16_t pageremain;
	pageremain = 256 - WriteAddr % 256;
	if (NumByteToWrite <= pageremain)
		pageremain = NumByteToWrite;
	while (1)
	{
		LT_W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
		if (NumByteToWrite == pageremain)
			break;
		else
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;

			NumByteToWrite -= pageremain;
			if (NumByteToWrite > 256)
				pageremain = 256;
			else
				pageremain = NumByteToWrite;
		}
	}
}

/********************************************************************************
 * Function Name	: LT_W25qxx_Erase_Chip
 * Description  	: Full Erase.
 * Input        	: None
 * Output       	: None
 * Return       	: None
 *********************************************************************************/
void LT_W25qxx_Erase_Chip(void)
{
	LT_W25qxx_Write_Enable();
	LT_W25qxx_Wait_Busy();
	nSS_Active();
	SPI_Master_FIFO_Data_Put(W25X_ChipErase);
	nSS_Inactive();
	LT_W25qxx_Wait_Busy();
}

/********************************************************************************
 * Function Name	: LT_W25qxx_Erase_Sector
 * Description  	: Erase a sector (4*1024 bytes).
 * Input        	: - addr:sector address
 * Output       	: None
 * Return       	: None
 *********************************************************************************/
void LT_W25qxx_Erase_Sector(uint32_t Dst_Addr)
{
	Dst_Addr *= 4096;
	LT_W25qxx_Write_Enable();
	LT_W25qxx_Wait_Busy();
	nSS_Active();
	SPI_Master_FIFO_Data_Put(W25X_SectorErase);

	if (W25Q256)
		SPI_Master_FIFO_Data_Put((uint8_t)((Dst_Addr) >> 24));

	SPI_Master_FIFO_Data_Put((uint8_t)((Dst_Addr) >> 16));
	SPI_Master_FIFO_Data_Put((uint8_t)((Dst_Addr) >> 8));
	SPI_Master_FIFO_Data_Put((uint8_t)Dst_Addr);
	nSS_Inactive();
	LT_W25qxx_Wait_Busy();
}

/********************************************************************************
 * Function Name	: LT_W25qxx_BlockErase64KB
 * Description  	: Erase a block (64*1024 bytes).
 * Input        	: - addr:block address
 * Output       	: None
 * Return       	: None
 *********************************************************************************/
void LT_W25qxx_BlockErase64KB(uint32_t addr)
{
	addr *= 65536;
	LT_W25qxx_Write_Enable();
	LT_W25qxx_Wait_Busy();
	nSS_Active();
	SPI_Master_FIFO_Data_Put(W25X_BlockErase);

	if (W25Q256)
		SPI_Master_FIFO_Data_Put((uint8_t)((addr) >> 24));

	SPI_Master_FIFO_Data_Put((uint8_t)((addr) >> 16));
	SPI_Master_FIFO_Data_Put((uint8_t)((addr) >> 8));
	SPI_Master_FIFO_Data_Put((uint8_t)addr);
	nSS_Inactive();
	LT_W25qxx_Wait_Busy();
}

void LT_W25QXX_Enter_4Byte_AddressMode(void)
{
	nSS_Active();
	SPI_Master_FIFO_Data_Put(0xB7);
	nSS_Inactive();
}
