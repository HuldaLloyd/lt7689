/*******************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     w25n01.c
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief	 Nand Flash Library Functions
 ********************************************************************************/

#include "w25n01.h"
#include "LT768.h"
#include "LT768_Lib.h"
#include "dmac_drv.h"
#include "if_port.h"

/********************************************************************************
* Function Name	: W25N01GV_ReadID
* Description  	: Read W25N01GV ID.
* Input        	: None
* Output       	: None
* Return       	: Flash ID
*********************************************************************************/
uint32_t W25N01GV_ReadID(void)
{
	uint8_t Mfr_id = 0;
	uint8_t Device_idL = 0;
	uint8_t Device_idH = 0;
	uint32_t id = 0;

	nSS_Active();					
	SPI_Master_FIFO_Data_Put(0x9f); 
	SPI_Master_FIFO_Data_Put(0xff); 
	Mfr_id = SPI_Master_FIFO_Data_Put(0xff);
	Device_idH = SPI_Master_FIFO_Data_Put(0xff);
	Device_idL = SPI_Master_FIFO_Data_Put(0xff);
	nSS_Inactive();

	id = Mfr_id;
	id = (id << 8) + Device_idH;
	id = (id << 8) + Device_idL; 
	return id;
}

/********************************************************************************
* Function Name	: W25N01GV_Write_Enable
* Description  	: Enable to write.
* Input        	: None
* Output       	: None
* Return       	: None
*********************************************************************************/

void W25N01GV_Write_Enable(void)
{
	nSS_Active();									
	SPI_Master_FIFO_Data_Put(W25N01GV_WriteEnable); 
	nSS_Inactive();									
}

/********************************************************************************
* Function Name	: W25N01GV_Write_Disable
* Description  	: Disable to write.
* Input        	: None
* Output       	: None
* Return       	: None
*********************************************************************************/
void W25N01GV_Write_Disable(void)
{
	nSS_Active();									
	SPI_Master_FIFO_Data_Put(W25N01GV_WriteDisable); 
	nSS_Inactive();									
}

/********************************************************************************
* Function Name	: W25N01GV_Write_SR
* Description  	: Write the value of the register.
* Input        	: - SRx:Register address
				  - 3 status registers
				  - Protection register 1:    address 0xA0
				  - Configuration register 2: address 0xB0
				  - Status register 3:        address 0xC0  (Read Only)	
* Output       	: None
* Return       	: None
*********************************************************************************/
void W25N01GV_Write_SR(uint8_t reg, uint8_t val)
{
	nSS_Active();									   // Enable
	SPI_Master_FIFO_Data_Put(W25N01GV_WriteStatusReg); // Send write status register command
	SPI_Master_FIFO_Data_Put(reg);					   // Register address
	SPI_Master_FIFO_Data_Put(val);					   // Write a byte
	nSS_Inactive();									   // Disable
}

/********************************************************************************
* Function Name	: W25N01GV_ReadSR
* Description  	: Read the value of the register.
* Input        	: - SRx:Register address
				  - 3 status registers
				  - Protection register 1:    address 0xA0
				  - Configuration register 2: address 0xB0
				  - Status register 3:        address 0xC0  (Read Only)	
* Output       	: None
* Return       	: None
*********************************************************************************/
uint8_t W25N01GV_ReadSR(uint8_t reg)
{
	uint8_t byte = 0;
	nSS_Active();									  // Enable
	SPI_Master_FIFO_Data_Put(W25N01GV_ReadStatusReg); // Send read status register command
	SPI_Master_FIFO_Data_Put(reg);					  // Register address
	byte = SPI_Master_FIFO_Data_Put(0Xff);			  // Read a byte
	nSS_Inactive();									  // Disable
	return byte;
}

/********************************************************************************
* Function Name	: W25N01GV_Wait_Busy
* Description  	: Wait for BUSY bit to clear.
* Input        	: None
* Output       	: None
* Return       	: None
*********************************************************************************/
void W25N01GV_Wait_Busy(void)
{
	while ((W25N01GV_ReadSR(0xC0) & 0x01) == 0x01)
		; // Wait for BUSY bit to clear
}


/********************************************************************************
* Function Name	: W25N01GV_Erase_Block
* Description  	: W25N01 block erase(128K)
* Input        	: block : block num
* Output       	: None
* Return       	: None
*********************************************************************************/
void W25N01GV_Erase_Block(uint16_t block)
{
	uint16_t addr = 0;
	addr = 64 * block;
	W25N01GV_Write_SR(0xa0, 0x00); // Turn off flash protection function

	W25N01GV_Write_Enable(); // SET WEL
	W25N01GV_Wait_Busy();

	nSS_Active();					// Enable 
	SPI_Master_FIFO_Data_Put(0xD8); // Send chip erase command
	SPI_Master_FIFO_Data_Put(0xff); // Send chip erase command
	SPI_Master_FIFO_Data_Put((uint8_t)(addr >> 8));
	SPI_Master_FIFO_Data_Put((uint8_t)(addr));
	nSS_Inactive(); // Disable

	W25N01GV_Wait_Busy();		   // Wait for chip erase to finish
	W25N01GV_Write_SR(0xa0, 0x7c); // Turn on flash protection function
}


/********************************************************************************
* Function Name	: LT_W25N01GV_STD_Write_Page_DMA
* Description  	: Write data at the specified address of W25N01
* Input        	: - pBuffer: data start address
                  - PageNum: page
                  - PageAddr: Page address
                  - NumByteToWrite: number bytes to write(max one page:2048 bytes)
* Output       	: None
* Return       	: None
*********************************************************************************/

void W25N01GV_WritePageAddr_Data(uint8_t *pBuffer, uint32_t PageNum, uint32_t PageAddr, uint16_t NumByteToWrite)
{
	uint16_t i;
	// W25N01GV_Erase_Block(WriteAddr);
	W25N01GV_Write_SR(0xa0, 0x00); // Turn off flash protection function

	//------дBUFF------
	W25N01GV_Write_Enable();		// SET WEL
	nSS_Active();					// Enabling device
	SPI_Master_FIFO_Data_Put(0x02); // Send the write BUFF command

	SPI_Master_FIFO_Data_Put((PageAddr) >> 8); // BUFF address
	SPI_Master_FIFO_Data_Put(PageAddr);

	LCD_CmdWrite(0xB8);
	SS_RESET;
	SPI2_ReadWriteByte(0x80);

	for (i = 0; i < NumByteToWrite; i++)
		SPI2_ReadWriteByte(pBuffer[i]); // Number of cyclic writes

	SS_SET;
	nSS_Inactive();		  // Cancel film selection
	W25N01GV_Wait_Busy(); // Wait for writing to end

	//-------BUFF data writing flash--------
	W25N01GV_Write_Enable();
	nSS_Active();											
	SPI_Master_FIFO_Data_Put(0x10);							// Send page write command
	SPI_Master_FIFO_Data_Put(0xff);							// Send 16it address
	SPI_Master_FIFO_Data_Put((uint8_t)((PageNum) >> 8)); 	// Send 16it address
	SPI_Master_FIFO_Data_Put((uint8_t)(PageNum));
	nSS_Inactive();
	W25N01GV_Wait_Busy(); // Wait for writing to end

	W25N01GV_Write_SR(0xa0, 0x7c); // Turn on flash protection function
}

/********************************************************************************
* Function Name	: W25N01GV_ReadPageAddr_Data
* Description  	: Read data of specified length from the specified address.
* Input        	: - pBuffer: buffer for save data
                  - ReadAddr: Start address of reading data
                  - NumByteToRead: number bytes to read
* Output       	: None
* Return       	: None
*********************************************************************************/

void W25N01GV_ReadPageAddr_Data(uint8_t *pBuffer, uint32_t PageNum, uint32_t PageAddr, uint16_t NumByteToRead)
{
	uint16_t i;

	nSS_Active(); 
	SPI_Master_FIFO_Data_Put(0x13);
	SPI_Master_FIFO_Data_Put(0x00);
	SPI_Master_FIFO_Data_Put((uint8_t)((PageNum) >> 8)); 
	SPI_Master_FIFO_Data_Put((uint8_t)(PageNum));
	nSS_Inactive();
	W25N01GV_Wait_Busy();

	W25N01GV_Write_SR(0xb0, W25N01GV_ReadSR(0xb0) | 0x08); 

	nSS_Active();
	SPI_Master_FIFO_Data_Put(0x03);
	SPI_Master_FIFO_Data_Put((PageAddr) >> 8);
	SPI_Master_FIFO_Data_Put(PageAddr);
	SPI_Master_FIFO_Data_Put(0x00);

	SPI_CmdWrite(0xB8);
	for (i = 0; i < NumByteToRead; i++)
	{
		SPI_DataWrite(0xFF);			
		pBuffer[i] = SPI_DataRead();
	}
	nSS_Active(); //---------------------
	LT768_SPI_Init(1, 1);
	W25N01GV_Wait_Busy();
}

/********************************************************************************
* Function Name	: W25N01GV_ReadPageAddr_Data
* Description  	: Read data of specified length from the specified address.
* Input        	: - pBuffer: buffer for save data
                  - ReadAddr: Start address of reading data
                  - NumByteToRead: number bytes to read
* Output       	: None
* Return       	: None
*********************************************************************************/
void LT_W25N01_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead) 
{
	uint16_t i;
	uint16_t page, endpage, remainder;
	remainder = ReadAddr % 2048;				   
	page = (ReadAddr / 2048);					   
	endpage = ((ReadAddr + NumByteToRead) / 2048); 
	LT768_SPI_Init(1, 1);

	if (page == endpage)
	{
		W25N01GV_ReadPageAddr_Data(pBuffer, page, remainder, NumByteToRead);
	}
	else
	{
		W25N01GV_ReadPageAddr_Data(pBuffer, page, remainder, 2048 - remainder);

		for (i = 1; page + i < endpage; i++)
		{
			W25N01GV_ReadPageAddr_Data(&pBuffer[2048 * i - remainder], page + i, 0, 2048);
		}

		W25N01GV_ReadPageAddr_Data(&pBuffer[2048 * i - remainder], endpage, 0, NumByteToRead - (2048 * i - remainder));
	}
}

void Write_Page(uint16_t page)
{
	nSS_Active();
	SPI_Master_FIFO_Data_Put(0x13);
	SPI_Master_FIFO_Data_Put(0x00);
	SPI_Master_FIFO_Data_Put((uint8_t)(page >> 8));
	SPI_Master_FIFO_Data_Put((uint8_t)page);
	nSS_Inactive();
	W25N01GV_Wait_Busy();
}

// Configure BUF=1: continuous read mode
void W25N01GV_ContinuousRead_Mode(void)
{
	uint8_t statut = 0;
	statut = W25N01GV_ReadSR(0xb0);
	statut &= cClrb3;
	statut &= cClrb4;
	statut |= cSetb4;
	W25N01GV_Write_SR(0xb0, statut);
}

// Configure BUF=0: page reading mode
void W25N01GV_PageRead_Mode(void)
{
	uint8_t statut = 0;
	statut = W25N01GV_ReadSR(0xb0);
	statut |= cSetb3;
	statut |= cSetb4;
	W25N01GV_Write_SR(0xb0, statut);
}

void W25N01GV_BUF1Set(void)
{
	uint8_t statut = 0;
	statut = W25N01GV_ReadSR(0xb0);
	statut |= cSetb3;
	statut &= cClrb4;
	//	statut |= cSetb4;
	W25N01GV_Write_SR(0xb0, statut);
}
