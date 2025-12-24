/*******************************************************************************
  * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
  * @file     flash.c
  * @author   UartTFT Application Team
  * @version  V1.0.0
  * @date     2023-02-24
  * @brief
 ********************************************************************************/

#include "flash.h"
#include "LT768.h"
#include "LT768_Lib.h"

#include "if_port.h"
#include "bsp.h"

/*SPI1 DMA channel base address*/
DMA_CHANNEL_REG *spi1_dma_channel[4] = {(DMA_CHANNEL_REG *)(DMA1_BASE_ADDR), (DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0x58),
										(DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0xB0), (DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0x108)}; // global struct variable for for Channel registers
/*SPI1 DMA config base address */
DMA_CONTROL_REG *spi1_dma_control = (DMA_CONTROL_REG *)(DMA1_BASE_ADDR + 0x2C0); // global struct variable for for DMAC registers

										
/*Hardware*/
uint8_t Flash_CS = 1; // Used to determine whether it is CS1 or CS0

void MCU_SS_RESET(void)
{
	if (Flash_CS == 1)
		MCU_CS1_RESET; // SPI1.SS
	else
		MCU_CS0_RESET;
}

void MCU_SS_SET(void)
{
	if (Flash_CS == 1)
		MCU_CS1_SET; // SPI1.SS
	else
		MCU_CS0_SET;
}

void Close_SPI1(void)
{
	SPI_Cmd(SPI1, DISABLE);
	SPI_ConfigGpio(SPI1, SPI_SS, GPIO_INPUT);
	SPI_ConfigGpio(SPI1, SPI_MOSI, GPIO_INPUT);
	SPI_ConfigGpio(SPI1, SPI_MISO, GPIO_INPUT);
	SPI_ConfigGpio(SPI1, SPI_SCK, GPIO_INPUT);
	SPI1->SPIPURD = 1;
#if DualFlash
	MCU_CS0_INPUT
#endif
}

void SPI1_Init(void)
{
	SPI_InitTypeDef SPI_InitStruct;

	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;

	SPI_Init(SPI1, &SPI_InitStruct); // Initialize peripheral SPIx register
	SPI_Cmd(SPI1, ENABLE);
	SPI_ConfigGpio(SPI1, SPI_SS, GPIO_OUTPUT);
#if DualFlash
	MCU_CS0_OUTPUT
#endif 
}
/***********/


uint16_t SPI1_ReadWriteByte(uint16_t TxData)
{
	while ((SPI1->SPISRHW) & (SPISR_TXFFULL_MASK)) // The waiting sending area is empty
	{
	}
	SPI1->SPIDR = TxData; // Send a byte

	while ((SPI1->SPISRHW) & (SPISR_RXFEMP_MASK)) // Wait for receiving a byte
	{
	}
	return SPI1->SPIDR; // Return received data
}

uint16_t Mcu_ReadID(void)
{
	uint16_t Temp = 0;
	
	Disable_SFlash_SPI();
	SPI1_Init();
	Flash_SS_RESET; // Enabling device
	SPI1_ReadWriteByte(0x90);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	Temp |= SPI1_ReadWriteByte(0xFF) << 8;
	Temp |= SPI1_ReadWriteByte(0xFF);
	Flash_SS_SET;
	if (Flash_CS == 0)
	{
		Close_SPI1();
		Enable_SFlash_SPI();
		LT768_SPI_Init(1, 0);
	}
	else
	{
		Close_SPI1();
		Enable_SFlash_SPI();
		LT768_SPI_Init(1, 1);
	}
	return Temp;
}
void SPI1_DMA_Tran(uint8_t *Tx_Addr, uint8_t *Rx_Addr, uint32_t length)
{
	//	DMA_Init(DMA1_BASE_ADDR);
	SPI1->SPIDMACR |= 0x03; // DMA enabling SPI TX RX

	//	spi1_dma_control->DMA_CONFIG = 1;
	// Tx configuration
	spi1_dma_channel[0]->DMA_SADDR = (uint32_t)Tx_Addr;
	spi1_dma_channel[0]->DMA_DADDR = (uint32_t)&SPI1->SPIDR;
	spi1_dma_channel[0]->DMA_CTRL_HIGH = length;
	spi1_dma_channel[0]->DMA_CTRL = DNOCHG | SIEC | M2P_DMA;
	spi1_dma_channel[0]->DMA_CFG = (HS_SEL_SRC_SOFT);
	spi1_dma_channel[0]->DMA_CFG_HIGH = DST_PER_SPI_TX(0) | SRC_PER_SPI_TX(0);

	// Rx configuration
	spi1_dma_channel[1]->DMA_SADDR = (uint32_t)&SPI1->SPIDR;
	spi1_dma_channel[1]->DMA_DADDR = (uint32_t)Rx_Addr;
	spi1_dma_channel[1]->DMA_CTRL_HIGH = length;
	spi1_dma_channel[1]->DMA_CTRL = SNOCHG | DIEC | P2M_DMA;
	spi1_dma_channel[1]->DMA_CFG = (HS_SEL_DST_SOFT);
	spi1_dma_channel[1]->DMA_CFG_HIGH = SRC_PER_SPI_RX(3) | DST_PER_SPI_RX(3);

	spi1_dma_control->DMA_MASKTFR |= CHANNEL_UMASK(0) | CHANNEL_UMASK(1);
	spi1_dma_control->DMA_CHEN |= CHANNEL_WRITE_ENABLE(0) | CHANNEL_ENABLE(0) | CHANNEL_WRITE_ENABLE(1) | CHANNEL_ENABLE(1);
}

void SPI1_DMA_Wait(void)
{

	while ((spi1_dma_control->DMA_RAWTFR & (CHANNEL_STAT(0) | CHANNEL_STAT(1))) != (CHANNEL_STAT(0) | CHANNEL_STAT(1)))
		;
	spi1_dma_control->DMA_CLRTFR = (CHANNEL_STAT(0) | CHANNEL_STAT(1));

	//	spi1_dma_control->DMA_CHEN = 0;
	spi1_dma_control->DMA_CHEN &= ~(CHANNEL_WRITE_ENABLE(0) | CHANNEL_ENABLE(0) | CHANNEL_WRITE_ENABLE(1) | CHANNEL_ENABLE(1));
	//	spi1_dma_control->DMA_CONFIG = 0;
	SPI1->SPIDMACR &= (~0x03); // Disable DMA of SPI TX RX
	// DMA_Init(DMA2_BASE_ADDR);
}

void Mcu_Write_Enable(void) //  write enable
{
	Flash_SS_RESET;			 
	SPI1_ReadWriteByte(0x06); 
	Flash_SS_SET;			  
}
void Mcu_Write_Disable(void) // Write prohibition
{
	Flash_SS_RESET;			  
	SPI1_ReadWriteByte(0x04); 
	Flash_SS_SET;			  
}

void Mcu_Write_SR(uint8_t reg, uint8_t val)
{
	Mcu_Write_Enable();
	Flash_SS_RESET;

	SPI1_ReadWriteByte(0x1F); // Send write status register command
	SPI1_ReadWriteByte(reg);  // Register address
	SPI1_ReadWriteByte(val);  // Write a byte

	Flash_SS_SET; 
	Mcu_Write_Disable();
}
uint8_t Mcu_ReadSR(uint8_t reg) // Read the status register of W25N01
{
	uint8_t byte = 0;
	Flash_SS_RESET;					 
	SPI1_ReadWriteByte(0x0F);		 // Send read status register command
	SPI1_ReadWriteByte(reg);		 // Register address
	byte = SPI1_ReadWriteByte(0Xff); // Read a byte
	Flash_SS_SET;					 
	return byte;
}

void Mcu_ReadPageAddr_Data(uint8_t *pBuffer, uint16_t ColumnAddr, uint32_t PageAddr, uint16_t NumByteToRead)
{

	uint16_t i;
	uint8_t wrbuff[2048];


	
	if (Flash_type == 0)
	{
		Flash_SS_RESET;			  // Enabling device
		SPI1_ReadWriteByte(0x03); // Send read command

		if (W25Q256)
			SPI1_ReadWriteByte((uint8_t)((PageAddr) >> 24)); // 256Flash sending 32bit address
		SPI1_ReadWriteByte((uint8_t)((PageAddr) >> 16));		 // Send 24bit address
		SPI1_ReadWriteByte((uint8_t)((PageAddr) >> 8));
		SPI1_ReadWriteByte((uint8_t)PageAddr);

#if 1
		SPI1_DMA_Tran(wrbuff, pBuffer, NumByteToRead);
		SPI1_DMA_Wait();
#else
		for (i = 0; i < NumByteToRead; i++)
		{
			pBuffer[i] = SPI1_ReadWriteByte(0XFF); // Cycle reading
		}
#endif

		Flash_SS_SET;
	}
	else if (Flash_type == 1)
	{
		Mcu_Write_SR(0xB0, Mcu_ReadSR(0xB0) | 0x08);
		//------Read data from flash to BUFF-----
		Flash_SS_RESET; // Enabling device
		SPI1_ReadWriteByte(0x13);
		SPI1_ReadWriteByte(0x00);		   // Empty period
		SPI1_ReadWriteByte(PageAddr >> 8); // Send the 16bit page address and extract the data of the corresponding page to BUFF
		SPI1_ReadWriteByte(PageAddr);
		Flash_SS_SET;
		while ((Mcu_ReadSR(0xc0) & 0x01) == 0x01)
			;

		Mcu_Write_SR(0xB0, Mcu_ReadSR(0xB0) | 0x08);

		//------Then read data from BUFF to SCM-----
		Flash_SS_RESET;
		SPI1_ReadWriteByte(0x03);
		SPI1_ReadWriteByte(ColumnAddr >> 8);
		SPI1_ReadWriteByte(ColumnAddr);
		SPI1_ReadWriteByte(0x00);

		SPI1_DMA_Tran(wrbuff, pBuffer, NumByteToRead);
		SPI1_DMA_Wait();
		/*
			for(i=0;i<NumByteToRead;i++)
			{
				pBuffer[i]= SPI1_ReadWriteByte(0xFF);   	//Cycle reading
			}
		*/
		Flash_SS_SET; //---------------------
		while ((Mcu_ReadSR(0xc0) & 0x01) == 0x01)
			;
	}

}

void Flash_Read_UI(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	uint16_t ReadNum = 0; // Read the amount of data per page
	uint16_t ColumnAddr;  // In-page address
	uint16_t PageAddr;
	/*debug
		LT_ReadFlash(pBuffer,ReadAddr,NumByteToRead);
	*/
	Disable_SFlash_SPI();
	SPI1_Init();
	if (UI_rotate == 1 || UI_rotate == 3)
		ReadAddr += UI_rotate_addr;
	if (Flash_type == 0)
	{
		uint16_t ReadNum = 0;

		if (NumByteToRead < 2048)
			ReadNum = NumByteToRead;
		else
			ReadNum = 2048;
		while (1)
		{
			Mcu_ReadPageAddr_Data(pBuffer, 0, ReadAddr, ReadNum);
			if (NumByteToRead == ReadNum)
				break;
			NumByteToRead -= ReadNum;
			ReadAddr += ReadNum;
			pBuffer += ReadNum;
			if (NumByteToRead < 2048) // You don't need to fill one page next time
			{
				ReadNum = NumByteToRead;
			}
			else
				ReadNum = 2048; // Write one page next time

		}
	}
	else
	{
		// Judge the corresponding in-page address, page address, and the amount of data written during the first reading
#if DualFlash
		if (ReadAddr >= 2048*64*(1024-20)) 
		{
			ReadAddr -= 2048*64*(1024-20);
			Flash_CS = 0;
		}
		else Flash_CS = 1;
#endif		
		ColumnAddr = (ReadAddr % 2048);
		PageAddr = ReadAddr / 2048;

		if (NumByteToRead < (2048 - ColumnAddr))
			ReadNum = NumByteToRead;
		else
			ReadNum = 2048 - ColumnAddr;

		while (1)
		{
			Mcu_ReadPageAddr_Data(pBuffer, ColumnAddr, PageAddr, ReadNum);
			/* If the data that has been written meets the requirements, the writing is completed and exit. 
			Because NumByteToWrite is decreasing, when it is reduced to the last time, 
			the data to be written must be equal to the remaining data. */
			if (NumByteToRead == ReadNum)
				break;

			pBuffer += ReadNum;		  // Array head moves with
			ColumnAddr = 0;			  // The next read operation must start from the 0 address ion the page.
			PageAddr++;				  // The next read operation must start from the new page.
			NumByteToRead -= ReadNum; // The total amount of read data is reduced according to the amount that has been read.

			if (NumByteToRead < 2048) // You don't need to fill one page next time
			{
				ReadNum = NumByteToRead;
			}
			else
				ReadNum = 2048; // Write one page next time
		}
	}
	if (Flash_CS == 0)
	{
		Close_SPI1();
		Enable_SFlash_SPI();
		LT768_SPI_Init(1, 0);
	}
	else
	{
		Close_SPI1();
		Enable_SFlash_SPI();
		LT768_SPI_Init(1, 1);
	}
}

void Flash_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	uint16_t ReadNum = 0; // Read the amount of data per page
	uint16_t ColumnAddr;  // In-page address
	uint16_t PageAddr;
	/*debug
		LT_ReadFlash(pBuffer,ReadAddr,NumByteToRead);
	*/
	Disable_SFlash_SPI();
	SPI1_Init();

	if (Flash_type == 0)
	{
		uint16_t ReadNum = 0;

		if (NumByteToRead < 2048)
			ReadNum = NumByteToRead;
		else
			ReadNum = 2048;
		while (1)
		{
			Mcu_ReadPageAddr_Data(pBuffer, 0, ReadAddr, ReadNum);
			if (NumByteToRead == ReadNum)
				break;
			NumByteToRead -= ReadNum;
			ReadAddr += ReadNum;
			pBuffer += ReadNum;
			if (NumByteToRead < 2048) // You don't need to fill one page next time
			{
				ReadNum = NumByteToRead;
			}
			else
				ReadNum = 2048; // Write one page next time

		}
	}
	else
	{
		// Judge the corresponding in-page address, page address, and the amount of data written during the first reading
#if DualFlash
		if (ReadAddr >= 2048*64*(1024-20)) 
		{
			ReadAddr -= 2048*64*(1024-20);
			Flash_CS = 0;
		}
		else Flash_CS = 1;
#endif		
		ColumnAddr = (ReadAddr % 2048);
		PageAddr = ReadAddr / 2048;

		if (NumByteToRead < (2048 - ColumnAddr))
			ReadNum = NumByteToRead;
		else
			ReadNum = 2048 - ColumnAddr;

		while (1)
		{
			Mcu_ReadPageAddr_Data(pBuffer, ColumnAddr, PageAddr, ReadNum);
			/* If the data that has been written meets the requirements, the writing is completed and exit. 
			Because NumByteToWrite is decreasing, when it is reduced to the last time, 
			the data to be written must be equal to the remaining data. */
			if (NumByteToRead == ReadNum)
				break;

			pBuffer += ReadNum;		  // Array head moves with
			ColumnAddr = 0;			  // The next read operation must start from the 0 address ion the page.
			PageAddr++;				  // The next read operation must start from the new page.
			NumByteToRead -= ReadNum; // The total amount of read data is reduced according to the amount that has been read.

			if (NumByteToRead < 2048) // You don't need to fill one page next time
			{
				ReadNum = NumByteToRead;
			}
			else
				ReadNum = 2048; // Write one page next time
		}
	}
	if (Flash_CS == 0)
	{
		Close_SPI1();
		Enable_SFlash_SPI();
		LT768_SPI_Init(1, 0);
	}
	else
	{
		Close_SPI1();
		Enable_SFlash_SPI();
		LT768_SPI_Init(1, 1);
	}
}

uint8_t MCU_W25QXX_ReadSR(void)   //读取W25QXX的状态寄存器
{
	uint8_t byte=0;
	Flash_SS_RESET;                            		//使能器件
	SPI1_ReadWriteByte(W25X_ReadStatusReg); 	//发送读取状态寄存器命令
	byte=SPI1_ReadWriteByte(0Xff);          	//读取一个字节
	Flash_SS_SET;                            		//取消片选
	return byte;
}
void MCU_W25QXX_SectorErase4KB(uint32_t addr)
{
	addr*=4096;
	Flash_SS_RESET;                         //使能器件
	SPI1_ReadWriteByte(W25X_WriteEnable); 	//发送写使能
	Flash_SS_SET;                 	//SET WEL
	
	while((MCU_W25QXX_ReadSR()&0x01)==0x01);  
	Flash_SS_RESET;                           	  	//使能器件
	SPI1_ReadWriteByte(W25X_SectorErase);   			//发送片擦除命令
	
	if (W25Q256)
	SPI1_ReadWriteByte((uint8_t)((addr)>>24));  	//256Flash发送32bit地址	

	
	SPI1_ReadWriteByte((UINT8)((addr)>>16)); 			//发送24bit地址
	SPI1_ReadWriteByte((UINT8)((addr)>>8));
	SPI1_ReadWriteByte((UINT8)addr);
	Flash_SS_SET;                            	//取消片选
	while((MCU_W25QXX_ReadSR()&0x01)==0x01);     				   	//等待芯片擦除结束
}
void MCU_W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
	uint16_t i;
	
	Flash_SS_RESET;                         //使能器件
	SPI1_ReadWriteByte(W25X_WriteEnable); 	//发送写使能
	Flash_SS_SET;    	//SET WEL
	Flash_SS_RESET;                            							//使能器件
	SPI1_ReadWriteByte(W25X_PageProgram);      					//发送写页命令
	
	if (W25Q256)
	SPI1_ReadWriteByte((uint8_t)((WriteAddr)>>24));  	//256Flash发送32bit地址

	SPI1_ReadWriteByte((UINT8)((WriteAddr)>>16)); 					//发送24bit地址
	SPI1_ReadWriteByte((UINT8)((WriteAddr)>>8));
	SPI1_ReadWriteByte((UINT8)WriteAddr);
	

	for(i=0;i<NumByteToWrite;i++)SPI1_ReadWriteByte(pBuffer[i]);	//循环写数

	Flash_SS_SET;                           							//取消片选
	while((MCU_W25QXX_ReadSR()&0x01)==0x01);  		// 等待BUSY位清空				   								//等待写入结束
}

uint8_t MUC_W25N01GV_ReadSR(uint8_t reg)
{
	uint8_t byte = 0;
	Flash_SS_RESET;									  // Enable
	SPI1_ReadWriteByte(W25N01GV_ReadStatusReg); // Send read status register command
	SPI1_ReadWriteByte(reg);					  // Register address
	byte = SPI1_ReadWriteByte(0Xff);			  // Read a byte
	Flash_SS_SET;									  // Disable
	return byte;
}
void MCU_W25N01GV_Erase_Block(uint16_t block)   
{               
	uint32_t addr = 0;	
	addr = 64 * block;  //输入的是页地址，一个块有64页。

	
	Flash_SS_RESET; 							//使能器件
	SPI1_ReadWriteByte(0x1F);          //发送写取状态寄存器命令    
	SPI1_ReadWriteByte(0xA0);                      //寄存器地址 
	SPI1_ReadWriteByte(0x00);               	      //写入一个字节      	      
	Flash_SS_SET; 
	
    Flash_SS_RESET;                         			//使能器件   
    SPI1_ReadWriteByte(0x06); 	//发送写使能  
	Flash_SS_SET; 
    while ((MUC_W25N01GV_ReadSR(0xC0) & 0x01) == 0x01);
	
    Flash_SS_RESET;  						    //使能器件
    SPI1_ReadWriteByte(0xD8);        	//发送片擦除命令  
	SPI1_ReadWriteByte((uint8_t)(addr>>16));        	//发送片擦除命令  
	SPI1_ReadWriteByte((uint8_t)(addr>>8)); 
	SPI1_ReadWriteByte((uint8_t)(addr));
	Flash_SS_SET;                         //取消片选     	
	
	while ((MUC_W25N01GV_ReadSR(0xC0) & 0x01) == 0x01);				        //等待芯片擦除结束
	Flash_SS_RESET;									   // Enable
	SPI1_ReadWriteByte(W25N01GV_WriteStatusReg);   // Send write status register command
	SPI1_ReadWriteByte(0xa0);					   // Register address
	SPI1_ReadWriteByte(0x7c);					   // Write a byte
	Flash_SS_SET;                				   // 开flash保护功能
}
void MCU_W25N01GV_WritePageAddr_Data(uint8_t* pBuffer,uint16_t ColumnAddr,uint32_t PageAddr,uint16_t NumByteToWrite)   //不能写入超过2048 Byte（一页2048个Byte，后面的64 Byte非数据位）
{
 	uint16_t i;
 	
	Flash_SS_RESET;                         			//使能器件   
    SPI1_ReadWriteByte(0x06); 	//发送写使能  
	Flash_SS_SET; 
	
	Flash_SS_RESET; 							//使能器件
	SPI1_ReadWriteByte(0x1F);          //发送写取状态寄存器命令    
	SPI1_ReadWriteByte(0xA0);                      //寄存器地址 
	SPI1_ReadWriteByte(0x00);               	      //写入一个字节      	      
	Flash_SS_SET;                                 //取消片选 
	
	Flash_SS_RESET;                          		//使能器件   
    SPI1_ReadWriteByte(0x04); 	//发送写禁止指令    
	Flash_SS_SET; 
	
	//------写BUFF------
    Flash_SS_RESET;                         			//使能器件   
    SPI1_ReadWriteByte(0x06); 	//发送写使能  
	Flash_SS_SET; 	//SET WEL 
	
    Flash_SS_RESET;                           //使能器件   
    SPI1_ReadWriteByte(0x02);      	    //发送写BUFF命令   
    SPI1_ReadWriteByte(ColumnAddr>>8); 	//BUFF地址   
    SPI1_ReadWriteByte(ColumnAddr);

	
		
	for(i=0;i<NumByteToWrite;i++)
		SPI1_ReadWriteByte(pBuffer[i]);//循环写数 
		

	Flash_SS_SET;                          //取消片选 
	
	while ((MUC_W25N01GV_ReadSR(0xC0) & 0x01) == 0x01);
	
	//-------BUFF数据写入flash--------
	Flash_SS_RESET;                         			//使能器件   
    SPI1_ReadWriteByte(0x06); 	//发送写使能  
	Flash_SS_SET; 
	
	Flash_SS_RESET;                          	//使能器件   
	SPI1_ReadWriteByte(0x10);      	    	//发送写页命令   
	SPI1_ReadWriteByte(PageAddr>>16); 	       	 	//发送16it地址    
	SPI1_ReadWriteByte(PageAddr>>8); 		//发送16it地址    
	SPI1_ReadWriteByte(PageAddr);   
	Flash_SS_SET;   
	
	while ((MUC_W25N01GV_ReadSR(0xC0) & 0x01) == 0x01);
	
	Flash_SS_RESET;									   // Enable
	SPI1_ReadWriteByte(W25N01GV_WriteStatusReg); // Send write status register command
	SPI1_ReadWriteByte(0xa0);					   // Register address
	SPI1_ReadWriteByte(0x7c);					   // Write a byte
	Flash_SS_SET;
	 
} 
void Flash_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   //函数内部自动将32位地址转换为对应的页地址和页内地址
{             
	uint16_t WriteNum = 0;         //写入每一页的数据量
	uint16_t ColumnAddr;           //页内地址
	uint32_t PageAddr;             //页地址
	uint16_t pageremain;
	uint16_t block = 0;
	uint32_t block_addr = 0;
	
	Disable_SFlash_SPI();
	SPI1_Init();
	
	if (Flash_type == 0)
	{
		if(WriteAddr % (4 *1024) == 0)
		{
			block_addr = WriteAddr / (4 *1024);
			MCU_W25QXX_SectorErase4KB(block_addr);
		}
		pageremain=256-WriteAddr%256; //单页剩余的字节数
		if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
		while(1)
		{
			MCU_W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
			if(NumByteToWrite==pageremain)break;//写入结束了
			else //NumByteToWrite>pageremain
			{
				pBuffer+=pageremain;
				WriteAddr+=pageremain;

				NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
				if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
				else pageremain=NumByteToWrite; 	  //不够256个字节了
			}
		}
	}
	else
	{
	//判断第一次写入时，对应的页内地址，页地址，写入的数据量。
		#if DualFlash
		if (WriteAddr >= 2048*64*(1024-20)) 
		{
			WriteAddr -= 2048*64*(1024-20);
			Flash_CS = 0;
		}
		else Flash_CS = 1;
		#endif
		
	
		if(WriteAddr % (128 *1024) == 0)
		{
			block_addr = WriteAddr / (128 *1024);
			MCU_W25N01GV_Erase_Block(block_addr);
		}
		
		
		ColumnAddr = (WriteAddr%2048);
		PageAddr = WriteAddr/2048;
		
		if(NumByteToWrite<(2048-ColumnAddr)) WriteNum = NumByteToWrite;         
		else WriteNum = 2048-ColumnAddr;
		
		
		while(1)
		{
	//		printf("ColumnAddr:%d\r\n",ColumnAddr);
	//		printf("PageAddr:%d\r\n",PageAddr);
			
			MCU_W25N01GV_WritePageAddr_Data(pBuffer,ColumnAddr,PageAddr,WriteNum);
			if(NumByteToWrite == WriteNum)break;    //如果已经写入的数据达到要求，则写入完毕，退出。因为NumByteToWrite是不断减少的，当减到最后一次的时候，要写入的数据必定等于剩余的数据。
			
			pBuffer += WriteNum;                    //数组头跟着移动
			ColumnAddr = 0;           				//下一次写入操作必定从页内0地址写起。		
			PageAddr++;								//下一次写入操作必定从新页开始写。
			NumByteToWrite -= WriteNum;				//总写入数据量根据已经写入的量进行减少。
			
			if(NumByteToWrite<2048)                //下一次不需要写满一页
			{
				 WriteNum =NumByteToWrite;
			}
			else WriteNum = 2048;                  //下一次要写满一页
		}
	}
	
	if (Flash_CS == 0)
	{
		Close_SPI1();
		Enable_SFlash_SPI();
		LT768_SPI_Init(1, 0);
	}
	else
	{
		Close_SPI1();
		Enable_SFlash_SPI();
		LT768_SPI_Init(1, 1);
	}
}