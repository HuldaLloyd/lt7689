// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : usbdev_scsi.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "usb_reg.h"
#include "usb_const.h"
#include "usb_drv.h"
#include "iomacros.h"
//#include "debug.h"
#include "string.h"

//Process the useful lba
static void enum_realDisk(unsigned long lba,unsigned char* buffer);
static void UsbRead10(void);
static void DMAWaitDone(void);
static void DMATransfer(UINT32 memaddr, UINT32 len, UINT16 dcr);
static void VendorWriteData(void);
extern unsigned short make_len_alignTo512(unsigned long value);
extern unsigned short make_len_alignTo64(unsigned long value);
//static void copy_data_to_buffer(void);

//UINT8 rev_buf[512];

void UsbSetTXsize(UINT16 size,UINT8 mode)
{
	UINT8 data8;


	if(size)
	{
//		STH(size,TXMAXP);
		gUSBC_IdxReg->TXMAXP_L = size;
		gUSBC_IdxReg->TXMAXP_H = (size>>8);
	}
	else
	{
		if(g_usbVer == 1)
		{
			gUSBC_IdxReg->TXMAXP_L = USB_MAX_PACKET_SIZE_LOW;
			gUSBC_IdxReg->TXMAXP_H = USB_MAX_PACKET_SIZE_HIGH;
		}
		else
		{
			gUSBC_IdxReg->TXMAXP_L = USB_MAX_PACKET_SIZE_LOW_V11;
			gUSBC_IdxReg->TXMAXP_H = USB_MAX_PACKET_SIZE_HIGH_V11;
		}
	}

	if(mode==1)		//double buffer mode
	{
		data8 = gUSBC_fifoReg->TXFIFOSZ;
		data8 |= 0x10;		//enable double buffer
		gUSBC_fifoReg->TXFIFOSZ = data8;
		//Enable DMA mode 1 and Double Buffer Related AutoSet function
		gUSBC_IdxReg->TXCSR_H = DEV_TXCSR_AUTO_SET|DEV_TXCSR_TXMODE|DEV_TXCSR_DMA_ENAB;
	}else{
		data8 = gUSBC_fifoReg->TXFIFOSZ;
		data8 &= 0xef;		//disable double buffer
		gUSBC_fifoReg->TXFIFOSZ = data8;
		gUSBC_IdxReg->TXCSR_H = 0;
	}


}
/*
Description:Receive data from host.(BULK-OUT to host)
*/
void USBDev_Data_BulkOut(void)
{
	unsigned short uiRxCount;

	uiRxCount = gUSBC_IdxReg->RXCount_H;
	uiRxCount <<=8;
	uiRxCount += gUSBC_IdxReg->RXCount_L; 
	 		   
	if(uiRxCount==0)
		return;
 
	if(uiRxCount==31)
	{
	  read_ep_buf(g_cbwPacket,USB_ENDPOINT_FIFO_ADDR,31);
		// constructe a csw packet
		//dCSWTag,got from CBW
		g_cswPacket[4]=g_cbwPacket[4];
		g_cswPacket[5]=g_cbwPacket[5];
		g_cswPacket[6]=g_cbwPacket[6];
		g_cswPacket[7]=g_cbwPacket[7];
		//dCSWDataResidue
		g_cswPacket[8]=(0>>24) & 0xFF;
		g_cswPacket[9]=(0>>16) & 0xFF;
		g_cswPacket[10]=(0>>8) & 0xFF;
		g_cswPacket[11]=(0) & 0xFF;
		//bCSWStatus
		g_cswPacket[12]=0;
		//Command and Sub Command
		g_bulkPhase = g_cbwPacket[15];
		g_subCmd = g_cbwPacket[28];

		//process command
		switch(g_bulkPhase)
		{
			case 0x00:
			case 0x1e:
				USBDev_Packet_BulkIn((unsigned char *)g_cswPacket,13,1);								
				break;
			case 0x12:
				USBDev_Inquiry();
				break;
			case 0x23:
			case 0x25:
				USBDev_ReadCapacity();
				break;
			case 0x1a:
				USBDev_ModeSense();
				break;
			case 0x03:	
				USBDev_RequestSense();
				break;
			case 0x28:		
				//get LBA and sector number
				g_wrLBA = g_cbwPacket[17];//CBWCB[2]
				g_wrLBA <<=8;
				g_wrLBA += g_cbwPacket[18];
				g_wrLBA <<=8;
				g_wrLBA += g_cbwPacket[19];
				g_wrLBA <<=8;
				g_wrLBA += g_cbwPacket[20];
				g_transLens = ((g_cbwPacket[22]<<8)|g_cbwPacket[23])*512;//512 byte per sector	
				g_TxRxCounter=0;
				g_databuf = (USB_BUFFER_ADDR+0x200);
				USBDev_Data_BulkIn();	
				break;
			//Vendor Command
			case 0x2A:
				g_transLens = ((g_cbwPacket[22]<<8)|g_cbwPacket[23])*512;//512 byte per sector
				break;
//////****************添加用户程序分支*************************************////////
			case 0xC0:  								 //用户定义程序 写命令
//			  printf("user write cmd 0xC0 \r\n");	
//				g_transLens = ((g_cbwPacket[21]<<8)|g_cbwPacket[22]);
                g_transLens = ((g_cbwPacket[11]<<24)|(g_cbwPacket[10]<<16)|(g_cbwPacket[9]<<8)|g_cbwPacket[8]);
				g_databuf = (USB_BUFFER_ADDR);
				g_userdatabuf = (USER_DATA_ADDR);
				g_TxRxCounter = 0;		//reset TxRx counter
				break;
			case 0xC1:

				//用户定义程序 读命令
//				printf("user read cmd 0xC1 \r\n");
//				g_transLens = ((g_cbwPacket[21]<<8)|g_cbwPacket[22]);
                g_transLens = ((g_cbwPacket[11]<<24)|(g_cbwPacket[10]<<16)|(g_cbwPacket[9]<<8)|g_cbwPacket[8]);
				g_databuf = (USB_BUFFER_ADDR);
				g_userdatabuf = (USER_DATA_ADDR);
				g_TxRxCounter = 0;
				USBDev_Data_BulkIn();
				 break;
///////////////////////////////////////////////////////////////////////////////////
			default:
				//reply error to all other command		
				g_cswPacket[12] = 1;
				USBDev_Packet_BulkIn((unsigned char *)g_cswPacket,13,1);					
				break;
			}
	}
	else
	{	
		VendorWriteData();
 	}
}

/*
Description:Device sends data to host(BULK-IN to host)
*/
void USBDev_Data_BulkIn(void)
{
	if(g_transLens)
	{
		switch(g_bulkPhase)
		{
			case 0x28:
				if(g_TxRxCounter==0)
				{
					enum_realDisk(g_wrLBA,(unsigned char*)g_databuf);
				}
				
				UsbRead10();
				
				if(g_TxRxCounter == 8)
				{
					g_TxRxCounter = 0;
					g_wrLBA++;
				}
				break;
			case 0xC1:
				if (g_TxRxCounter == 0)
				{
					//用户可以将发送的数据存在g_userdatabuf(512B)
					memcpy((UINT8 *)g_databuf,(UINT8 *)g_userdatabuf,512);
				}
				
				UsbRead10();	
				
				if(g_TxRxCounter == 8)
				{
					g_TxRxCounter = 0;
				}
				break;			

			default:	
				USBDev_Packet_BulkIn((unsigned char *)g_cswPacket,13,1);	//write csw response			
				break;			
		}
	}
	else
	{
		USBDev_Packet_BulkIn((unsigned char *)g_cswPacket,13,1);	//write csw response	
	}
}

static void UsbRead10(void)
{
	UINT16 DmaCtl = 0;

	if(g_usbVer == 1) //USB 2.0
	{	//DMA
		if(g_transLens < USB_MAX_PACKET_SIZE)
		{
			UsbSetTXsize(g_transLens,0);
			DmaCtl = CNTL_DMAEN(1) | CNTL_DIRECTION(1) | CNTL_DMAMODE(0)
					/*| CNTL_INTERE(1)*/ | CNTL_EP(USB_ENDPOINT_INDEX) | CNTL_BUSERR(0) | CNTL_BURSTMODE(USBDMA_BURST_LEN);
			DMATransfer((UINT32)g_databuf, g_transLens, DmaCtl);
			DMAWaitDone();
			SetTx();
			g_transLens = 0;
		}
		else
		{
			UsbSetTXsize(USB_MAX_PACKET_SIZE,0);
			DmaCtl = CNTL_DMAEN(1) | CNTL_DIRECTION(1) | CNTL_DMAMODE(0)
					/*| CNTL_INTERE(1)*/ | CNTL_EP(USB_ENDPOINT_INDEX) | CNTL_BUSERR(0) | CNTL_BURSTMODE(USBDMA_BURST_LEN);
			DMATransfer((UINT32)g_databuf, USB_MAX_PACKET_SIZE, DmaCtl);
			DMAWaitDone();
			SetTx();
			g_transLens -= USB_MAX_PACKET_SIZE;
			g_TxRxCounter = 8;
		}
	}
	else  //USB 1.1
	{//CPU
		if(g_transLens < USB_MAX_PACKET_SIZE_V11)
		{
			USBDev_Packet_BulkIn((unsigned char *)(g_databuf+g_TxRxCounter*USB_MAX_PACKET_SIZE_V11),g_transLens,0);
			g_transLens = 0;
		}
		else
		{
			USBDev_Packet_BulkIn((unsigned char *)(g_databuf+g_TxRxCounter*USB_MAX_PACKET_SIZE_V11),USB_MAX_PACKET_SIZE_V11,0);
			g_transLens -= USB_MAX_PACKET_SIZE_V11;
			g_TxRxCounter ++;
		}
	}
}

static void VendorWriteData(void)
{
//	UINT32 i;
	if(g_usbVer==1)		//USB2.0
	{
		
		if(g_transLens < USB_MAX_PACKET_SIZE)
		{
			read_ep_buf((UINT8 *)g_databuf,USB_ENDPOINT_FIFO_ADDR,g_transLens);
			g_transLens = 0;

		}
		else
		{
			read_ep_buf((UINT8 *)g_databuf,USB_ENDPOINT_FIFO_ADDR,USB_MAX_PACKET_SIZE);
			g_transLens -= USB_MAX_PACKET_SIZE;
		}
		
		if(g_bulkPhase == 0xC0)
		{
			//接收数据存在g_userdatabuf(64*8B)，用户可以搬走接收的数据
			memcpy((UINT8 *)USER_DATA_ADDR,(UINT8 *)g_databuf,USB_MAX_PACKET_SIZE);
		}
	}
	else		//USB1.1
	{
		if(g_transLens < USB_MAX_PACKET_SIZE_V11)
		{
			read_ep_buf((UINT8 *)g_databuf,USB_ENDPOINT_FIFO_ADDR,g_transLens);
			g_transLens = 0;
		}
		else
		{
			read_ep_buf((UINT8 *)g_databuf,USB_ENDPOINT_FIFO_ADDR,USB_MAX_PACKET_SIZE_V11);
			g_transLens -= USB_MAX_PACKET_SIZE_V11;
		}
		
		if(g_bulkPhase == 0xC0)
		{
				//接收数据存在g_userdatabuf(64*8B)，用户可以搬走接收的数据
			memcpy((UINT8 *)(g_userdatabuf+g_TxRxCounter*USB_MAX_PACKET_SIZE_V11),(UINT8 *)g_databuf,USB_MAX_PACKET_SIZE_V11);
			g_TxRxCounter = ((g_TxRxCounter == 7)?0:(g_TxRxCounter+1));
			
		}
	}
	
	if(g_transLens == 0)
	{
//		printf("return csw packet:\r\n");
		USBDev_Packet_BulkIn((unsigned char *)g_cswPacket,13,1);			//send csw packetb
	}
}


static void enum_realDisk(unsigned long lba,unsigned char* buffer)
{
	unsigned long fat_sum;
	unsigned char* addr;
	unsigned short reserve,i;
	unsigned long dbr_offset;
	
	addr = (unsigned char*)&DBR[0];
	reserve = *(addr+0x0e);
	fat_sum = *(addr+0x17);
	fat_sum = (fat_sum<<8) + *(addr+0x16);
	addr = (unsigned char*)buffer;
	
	dbr_offset = MBR[454];
	
	if(lba == 0){
	   memcpy(buffer,(unsigned long*)MBR,512);
	}
	else if(lba == dbr_offset){
	   memcpy(buffer,(unsigned long*)DBR,512);
	}
	else if(lba == (reserve+dbr_offset)){
		for(i=0; i<512; i++)
			*(addr+i) = 0;
		*addr = 0xf8;
		*(addr+1) = 0xff;
		*(addr+2) = 0xff;
		*(addr+3) = 0xff;
		return;
	}
	else if(lba == (fat_sum+reserve+dbr_offset)){
		for(i=0; i<512; i++)
			*(addr+i) = 0;	
		*addr = 0xf8;
		*(addr+1) = 0xff;
		*(addr+2) = 0xff;
		*(addr+3) = 0xff;
		return;
	}
	else{
		for(i=0; i<512; i++)
			*(addr+i) = 0;	
		return;
	}
	return;
}

extern const unsigned char dirData[];
void enum_realDisk_SD(unsigned long lba,unsigned char* buffer)
{
	unsigned long fat_sum;
	unsigned char* addr;
	unsigned short reserve,i;
	unsigned long dbr_offset;
	addr = (unsigned char*)&DBR[0];
	reserve = *(addr+0x0e);
	fat_sum = *(addr+0x17);
	fat_sum = (fat_sum<<8) + *(addr+0x16);
	addr = (unsigned char*)buffer;
	dbr_offset = MBR[454];
	if(lba == 0){
		memcpy(buffer,(unsigned long*)MBR,512);
	}
	else if(lba == dbr_offset){
		memcpy(buffer,(unsigned long*)DBR,512);
	}
	else if(lba == (reserve+dbr_offset)){
		for(i=0; i<512; i++)
			*(addr+i) = 0;
		*addr = 0xf8;
		*(addr+1) = 0xff;
		*(addr+2) = 0xff;
		*(addr+3) = 0xff;
		for(i=0; i<31; i++)
		{
			*((unsigned short*)(addr+4)+i) = 3+i;
		}
		*((unsigned short*)(addr+4)+i) = 0xffff;
		return;
	}
	else if(lba == (fat_sum+reserve+dbr_offset)){
		for(i=0; i<512; i++)
			*(addr+i) = 0;
		*addr = 0xf8;
		*(addr+1) = 0xff;
		*(addr+2) = 0xff;
		*(addr+3) = 0xff;
		for(i=0; i<31; i++)
		{
			*((unsigned short*)(addr+4)+i) = 3+i;
		}
		*((unsigned short*)(addr+4)+i) = 0xffff;
		return;
	}
	else if(lba == 0x217)
	{
		for(i=0; i<512; i++)
			*(addr+i) = 0;
		memcpy(addr,dirData,32);
		return;
	}
	else{
		for(i=0; i<512; i++)
			*(addr+i) = 0x00;
		return;
	}
	return;
}

unsigned short make_len_alignTo512(unsigned long value)
{
	unsigned short ret_value;
	
	ret_value = value>>9;
	if(value&0x1ff)
		ret_value++;
	if(ret_value == 0)
	{
		ret_value++;
	}
		
	return ret_value;
}

unsigned short make_len_alignTo64(unsigned long value)
{
	unsigned short ret_value;
	
	ret_value = value>>6;
	if(value&0x3f)
		ret_value++;
	if(ret_value == 0)
	{
		ret_value++;
	}
		
	return ret_value;
}
extern volatile unsigned int g_Dma_Done;
static void DMAWaitDone(void)
{
//	UINT8 DmaStatus = 0;
	
#if 0
	//wait DMA complete
	do
	{

	}while((g_Dma_Done & INTR_DMA1) != 0x01);		//INTR status is delay a long time after DMA done
#else
	while(IO_READ32(DMA_CNTL1) & 0x01);
#endif

}

void DMAWaitDone_Tx(void)
{
	UINT16 txcsr;

	//Wait TxpktRdy is Clear
	do{
		txcsr = gUSBC_IdxReg->TXCSR_L;
		
	}while((txcsr&DEV_TXCSR_FIFO_NOT_EMPTY)==DEV_TXCSR_FIFO_NOT_EMPTY);
}

static void DMATransfer(UINT32 memaddr, UINT32 len, UINT16 dcr)
{
	IO_WRITE32(DMA_ADDR1,memaddr);

	IO_WRITE32(DMA_COUNT1,len);

	IO_WRITE16(DMA_CNTL1,dcr);
}
