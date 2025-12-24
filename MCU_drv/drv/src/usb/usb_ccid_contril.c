// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : USB_CCID_control.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "usb_reg.h"
#include "usb_const.h"
#include "usb_drv.h"
#include "ccm_drv.h"
#include "cpm_drv.h"
#include "iomacros.h"
//#include "lt32u03.h"
#include "sys.h"

extern void USB_CCID_BusReset(void);

extern void USB_CCID_EP0_ISR(void);

extern void USB_CCID_Ep0Handle(void);

extern void USB_CCID_GetDataRate(void);

extern void USB_CCID_GetStatus(void);



extern void USB_CCID_SetTxSize(UINT16 Size);

/*************************************************
Function: write_ep0_buf_notEnd
Description: 通过端点0发送数据，不是最后一包
Calls:
Called By:
Input:
    INT8U* in_buf:要发送的数据的起始地址
    INT16U uiLens:要发送的数据长度
Output: 无
Return: 无
Others: 无
*************************************************/
static void write_ep0_buf_notEnd(UINT8 *in_buf, UINT16 uiLen)
{
	UINT8  ucReg  = gUSBC_IdxReg->E0CSR_L;
//	UINT16 i = 0;

	write_ep_buf(USB_FIFO_ENDPOINT_0,in_buf,uiLen);
	ucReg  |=  DEV_CSR0_TXPKTRDY;    // buffer填满后，告诉USB开始发送，不是最后一包
	gUSBC_IdxReg->E0CSR_L  =  ucReg;
}

/*
Description: Set the size of send data from FIFO
	Size = 0; Set the MAX vaule of data size(USB1.1/2.0)
	Size != 0; Set the request value of data size
*/
static void USB_CCID_SetTxSize(UINT16 Size)
{
	if(Size)
	{
		//Set Tx Transfer Data size
		gUSBC_IdxReg->TXMAXP_L = Size;
		gUSBC_IdxReg->TXMAXP_H = (Size>>8);
	}
	else
	{
		//Set MAX Tx Transfer size
		if(g_usbVer == 1)
		{
			gUSBC_IdxReg->TXMAXP_L = USB_MAX_PACKET_SIZE_LOW;
			gUSBC_IdxReg->TXMAXP_H = USB_MAX_PACKET_SIZE_HIGH;
		}else{
			gUSBC_IdxReg->TXMAXP_L = USB_MAX_PACKET_SIZE_LOW_V11;
			gUSBC_IdxReg->TXMAXP_H = USB_MAX_PACKET_SIZE_HIGH_V11;
		}
	}
}


/*
Description:USB Bus Reset Interrupt
*/
static void USB_CCID_BusReset(void)
{
//	UINT8 ucMode;
	UINT8 tempL, tempH;
	UINT8 i = 0;;

	gUSBC_ComReg->FADDRR = 0;

	//write FAddr 0
	for (i=0; i<2; i++)
	{
		//access DATA_OUT_EP register map
		gUSBC_ComReg->EINDEX =  INDEX_EP1+i;

		//set FIFO size
		if(g_usbVer == 1)
		{
			gTxMaxPacket = USB_MAX_PACKET_SIZE;
			gRxMaxPacket = USB_MAX_PACKET_SIZE;
			gUSBC_fifoReg->TXFIFOSZ = 0x06;
			gUSBC_fifoReg->RXFIFOSZ = 0x06;
		}
		else
		{
			gTxMaxPacket = USB_MAX_PACKET_SIZE_V11;
			gRxMaxPacket = USB_MAX_PACKET_SIZE_V11;
			gUSBC_fifoReg->TXFIFOSZ = 0x03;
			gUSBC_fifoReg->RXFIFOSZ = 0x03;
		}

		//set fifo offset address
		SetFIFO_Addr(0,0);		//default FIFO address

		if(g_usbVer == 1)
		{
		   tempL = USB_MAX_PACKET_SIZE_LOW;
		   tempH = USB_MAX_PACKET_SIZE_HIGH;
		}
		else
		{
		   tempL = USB_MAX_PACKET_SIZE_LOW_V11;
		   tempH = USB_MAX_PACKET_SIZE_HIGH_V11;
		}

		gUSBC_IdxReg->TXCSR_L = DEV_TXCSR_CLR_DATA_TOG;
		gUSBC_IdxReg->TXCSR_H = 0;

		gUSBC_IdxReg->TXMAXP_L = tempL;
		gUSBC_IdxReg->TXMAXP_H = tempH;

	 	gUSBC_IdxReg->RXCSR_L = DEV_RXCSR_CLR_DATA_TOG;
	 	gUSBC_IdxReg->RXCSR_H = 0x0;

		gUSBC_IdxReg->RXMAXP_L = tempL;
		gUSBC_IdxReg->RXMAXP_H = tempH;
		//=================================

		//Flush Tx Ep FIFO
		gUSBC_IdxReg->TXCSR_L = DEV_TXCSR_FLUSH_FIFO;

		//Flush Rx Ep FIFO
		gUSBC_IdxReg->RXCSR_L = DEV_RXCSR_FLUSH_FIFO;
	}

}

/*
Description:USB standard command GetStatus
*/
static void USB_CCID_GetStatus(void)
{
	UINT8 txdat[2]={0,0};

	//set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
	gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY|DEV_CSR0_DATAEND;

    switch(g_dev_std_req[0])//dev_std_req->bmRequestType
	{
		case 0x80:
		case 0x81:
		case 0x82:
			write_ep0_buf(txdat,2);
			break;
		default:
	        USBDev_EP0_SendStall();
			break;
	}

}

static void write_ep0_buf_long(UINT16 requstLen, UINT8 *databuf, UINT16 dataLen)
{
	UINT16 sentLen = 0;

	if (requstLen > dataLen)
		requstLen = dataLen;

	if (dataLen > USB_MAX_PACKET_SIZE_EP0)		//描述符大小超过一个包大小，需分包发送
	{
		if (requstLen > USB_MAX_PACKET_SIZE_EP0)
		{
#ifndef USB_USEINT
			gUSBC_ComReg->INTRTXE_L &=0xfe;		//关闭EP0中断
#endif
			while ((requstLen-sentLen) > USB_MAX_PACKET_SIZE_EP0)
			{
				write_ep0_buf_notEnd(databuf+sentLen,USB_MAX_PACKET_SIZE_EP0);
				while((gUSBC_ComReg->INTRTX_L &0x01)==0x00);
				sentLen += USB_MAX_PACKET_SIZE_EP0;
			}
			write_ep0_buf(databuf+sentLen,requstLen-sentLen);
#ifndef USB_USEINT
			gUSBC_ComReg->INTRTXE_L |=0x01;		//打开EP0中断
#endif
		}
		else
		{
			write_ep0_buf(databuf,requstLen);
		}
	}
	else	//一包就可发送完成
	{
		requstLen = (requstLen>dataLen)?dataLen:requstLen;		//host可能会发送长度字节为FF的请求
		write_ep0_buf(databuf,requstLen);
	}
}

/*
Description:USB standard command GetDescriptor
*/
static void USB_CCID_GetDescriptor(void)
{
	UINT8 i;
	UINT8 USBDEV_Descriptor[18];
	UINT16 Configuration_Descriptor_Len;
	UINT8 USBDEV_Configuration_Descriptor[0x5D];
	UINT8 ucReg = g_dev_std_req[6];
	UINT8 ucReg2 = g_dev_std_req[7];
	UINT8 reqIndex = g_dev_std_req[3]; 
	UINT8 ucIndex = g_dev_std_req[2];
	UINT16 ucRegLen;

	//set ServiceRxPktRdy to clear RxPktRdy
	gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY;
	ucRegLen = ((UINT16)ucReg2<<8)+ucReg;
	//Init all Descriptors
	for(i=0; i<18; i++)
	{
		USBDEV_Descriptor[i] = USB_CCID_Device_Descriptor[i];//ok
	}

	Configuration_Descriptor_Len = (USB_CCID_Configuration_Descriptor[3]<<8)+USB_CCID_Configuration_Descriptor[2];
	for(i = 0; i < Configuration_Descriptor_Len; i++)
	{
		USBDEV_Configuration_Descriptor[i] = USB_CCID_Configuration_Descriptor[i];//
	}

	switch(reqIndex)
	{
		//get device descriptor request
		case DEVICE_TYPE://device descriptor
		{
			if(ucReg <= USB_CCID_Device_Descriptor[0])
			{
				write_ep0_buf((UINT8*)USBDEV_Descriptor, ucReg);
			}
			else
			{
				write_ep0_buf((UINT8*)USBDEV_Descriptor, 8);
			}
			break;
		}
		//get configuration descriptor request
		case CONFIG_TYPE:
		{

			write_ep0_buf_long(ucRegLen, (UINT8*)USB_CCID_Configuration_Descriptor, Configuration_Descriptor_Len);
			break;
		}
		//get string descriptor request
		case STRING_TYPE:
		{
			if (ucIndex==0)
			{
				//USBDEV_String_Descriptor[0]=0x04;
				write_ep0_buf((UINT8*)UsbLanguageID, 4);
			}
			else if(ucIndex==1)
			{
				ucReg = (ucReg>UsbStrDescManufacturer[0])?UsbStrDescManufacturer[0]:ucReg;
				write_ep0_buf((UINT8*)UsbStrDescManufacturer, ucReg);
			}
			else if(ucIndex==2)
			{
				ucReg = (ucReg>USB_CCID_StrDescProduct[0])?USB_CCID_StrDescProduct[0]:ucReg;
				write_ep0_buf((UINT8*)USB_CCID_StrDescProduct,ucReg);
			}
			else if(ucIndex==3)//serial code
			{
				ucReg = (ucReg>UsbStrDescSerialNumber[0])?UsbStrDescSerialNumber[0]:ucReg;
				write_ep0_buf((UINT8*)UsbStrDescSerialNumber,ucReg);
			}
			else if(ucIndex == 4)
			{
				ucReg = (ucReg>SmartCard_Reader[0])?SmartCard_Reader[0]:ucReg;
				write_ep0_buf((UINT8*)SmartCard_Reader,ucReg);
			}
			break;
		}
		case DEVICE_QUALIFIER:
		{
			if(ucReg <= USB_CCID_Device_Qualifier_Descriptor[0])
			{
				write_ep0_buf((UINT8*)USB_CCID_Device_Qualifier_Descriptor, ucReg);
			}
			else
			{
				write_ep0_buf((UINT8*)USB_CCID_Device_Qualifier_Descriptor, 8);
			}
			break;
		}
		//get other_speed_configuration descriptor
		case OTHER_SPEED:
		{
			USBDEV_Configuration_Descriptor[1]=0x07;//other_speed_configuration
			if(ucReg < 10)
			{
				//in normal,first getting configuration descriptor only is to get
				//configuration size
				write_ep0_buf((UINT8*)USBDEV_Configuration_Descriptor,ucReg);
			}
			break;
		}
		case REPORT_TYPE:                 //HID协议里规定的报告描述符
		{
			break;
		}
		default:
		{
			USBDev_EP0_SendStall();
			break;
		}
	}
}

/*************************************************
Function: USB_CCID_GetDataRate
Description: 获取数据速率
Calls:
Called By:
Input:无
Output: 无
Return: 无
Others: 无
*************************************************/
void USB_CCID_GetDataRate(void)
{
	UINT16 ucRegLen;
	UINT8 ucReg= g_dev_std_req[6];//dev_std_req->wCount>>8;
	UINT8 ucReg2= g_dev_std_req[7];//dev_std_req->wCount
	UINT16 DataRate_Len = 0xB0;		//sizeof(DATA_RATE);

	//非接和接触使用相同的速率
	ucRegLen = ((UINT16)ucReg2<<8)+ucReg;
	write_ep0_buf_long(ucRegLen, (UINT8*)DATA_RATE, DataRate_Len);
}

static void USB_CCID_Ep0Handler(void)
{
	UINT8 ucReq=0;
	
	 const UINT8 cardPresent[2] = {0x50, 0x03};     //卡插入

	read_ep_buf(g_dev_std_req, USB_FIFO_ENDPOINT_0, 8);

	ucReq = g_dev_std_req[1];//dev_std_req->bRequest;

	if(1 == ((g_dev_std_req[0]>>5) & 0x3))//类请求
	{
		switch(ucReq)
		{
			case 0x01:
			{
				//直接返回，等待接收数据过程的OUT包

				break;
			}
			case 0x02:
			{
				break;
			}
			case 0x03:					//SET_CONTROL_LINR_STATE
			{
					USB_CCID_GetDataRate();
					USB_CCID_send(INDEX_EP2,(UINT8 *)cardPresent ,2);
					break;
			}
			default:
			{
				USBDev_EP0_SendStall();
				break;
			}
		}
	}
	else
	{
		switch(ucReq)
		{
			case 0:
				USB_CCID_GetStatus();
				break;
			case 1:

				//ClearFeature
				//set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
				gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY|DEV_CSR0_DATAEND;
				break;
			case 3:
				//SetFeature
				//set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
				gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY|DEV_CSR0_DATAEND;
				if(g_dev_std_req[0]==0x0)	//test mode
					USBDev_TestMode();
				break;
			case 5:
				//SetAddress
				ucReq = g_dev_std_req[2];// & 0x7f;dev_std_req->wValue>>8
				//set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
				gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY|DEV_CSR0_DATAEND;
				//gpram_USBAddrFlag = 1;
				g_USBNewAddr = ucReq;
				break;
			case 6:
				USB_CCID_GetDescriptor();
				break;
			case 8:
				//GetConfiguration
				//set ServiceRxPktRdy to clear RxPktRdy
				gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY|DEV_CSR0_DATAEND;

				write_ep0_buf(&ucReq, 1);
				break;
			case 9:

				//SetConfiguration
				//set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
				gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY|DEV_CSR0_DATAEND;
				break;
			case 10:
				//GetInterface
				gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY;
				write_ep0_buf(&ucReq,1);
				break;
			case 11:
				//SetInterface
				gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY|DEV_CSR0_DATAEND;
				break;

			default:
				USBDev_EP0_SendStall();
				break;
		}
	}
}


/*
Description: USB Endpoint0 Interrupt Service Routine,respond control transfer
*/
static void USB_CCID_EP0_ISR(void)
{
	UINT8 ucReg=0;

	//enable ep0 register map to be accessed
	gUSBC_ComReg->EINDEX = CONTROL_EP;

	ucReg = gUSBC_IdxReg->E0CSR_L;

	//clear SentStall bit
	if(ucReg &0x04)
	{
		ucReg &=0xFB;
	}
	//clear SetupEnd bit
	if(ucReg & 0x10)
	{
		//set ServiceSetupEnd = 1;
		ucReg |=0x80;
	}
	gUSBC_IdxReg->E0CSR_L = ucReg;

	//if RxPktRdy=1,Data come into ep0 buf
	if(ucReg & DEV_CSR0_RXPKTRDY)
	{
		USB_CCID_Ep0Handler();
	}
}

void USB_CCID_ISR(void)
{
	
	UINT8  ucISR;
	UINT8  index;

	index = gUSBC_ComReg->EINDEX;	//解决USB中断模式的问题。先保存端点，执行完后恢复端点。

	//check common interrupt
	g_msgflags |= gUSBC_ComReg->INTRUSB;
	//check TX interrupt
	g_msgflags1 |=gUSBC_ComReg->INTRTX_L;
	//Check RX interrupt
	ucISR = gUSBC_ComReg->INTRRX_L;

	if(ucISR & (1<<USB_ENDPOINT_INDEX))
 		g_msgflags |= 0x10;

	//to process setup packet   //OK
	if(g_msgflags1 & 0x1)
	{
		if(g_USBNewAddr)
		{
			gUSBC_ComReg->FADDRR = g_USBNewAddr;
			g_USBNewAddr = 0;	//set usb address once
		}
		g_msgflags1 &= 0xFE;
		USB_CCID_EP0_ISR();
	}
	//Endpoint1 RX handler
	if(g_msgflags&0x10)
	{
		g_msgflags &= 0xEF;	//Clear RX Interrupt Bit
		g_uchUSBStatus |= BIT0;
	}
	
	//device send data to host
	if(g_msgflags1 & (1<<USB_ENDPOINT_INDEX))    //OK
	{
		g_msgflags1 &=(~(1<<USB_ENDPOINT_INDEX));
		g_uchUSBStatus |= BIT4;
	}


	// if bus reset
	if(g_msgflags & 0x04)   //OK
	{
		g_msgflags &= 0xFB;
		USB_CCID_BusReset();

	}
	if(g_msgflags & 0x1)      //OK
	{
		g_msgflags &=0xFE;
		//USBDev_Suspend();
	}
	//Clear Last Completed Command
	if(g_bulkPhase == 0xFF)
		g_msgflags1 &= (~(1<<USB_ENDPOINT_INDEX));

	gUSBC_ComReg->EINDEX = index; 	//恢复端点
}

/*************************************************
Function: USB_CCID_send
Description: USB端点1发送一包数据
Calls:
Called By:
Input:
    INT8U *buf:发送数据的起始地址
    INT16U len:发送数据的长度
Output: 无
Return: 无
Others: 无
*************************************************/
void USB_CCID_send(UINT8 usb_ep, UINT8 *buf, UINT16 len)
{
	if ((usb_ep == CONTROL_EP)||(usb_ep > INDEX_EP7))
		return ;
	//access DATA_OUT_EP register map
	gUSBC_ComReg->EINDEX = INDEX_EP1;//DEV_TXCSR_TXPKTRDY

	while((gUSBC_IdxReg->TXCSR_L & DEV_TXCSR_TXPKTRDY) == DEV_TXCSR_TXPKTRDY);//检查发送缓冲是否有数据，等待为空；
	USB_CCID_SetTxSize(len);

	//write data into TX FIFO and wait for send
	write_ep_buf(USB_ENDPOINT_FIFO_ADDR+(usb_ep-1)*0x04, buf, len);

	SetTx();		//set TxPktRdy=1
	gUSBC_ComReg->EINDEX = INDEX_EP2;//
}

/*************************************************
Function: USB_CCID_receive
Description: USB端点1接收一包数据
Calls:
Called By:
Input:
    INT8U *buf:接收数据的起始地址
Output: 无
Return: 接收数据的长度
Others: 无
*************************************************/
UINT16 USB_CCID_receive(UINT8 usb_ep, UINT8 *buf)
{
	UINT16   uiRxCount = 0;
	UINT8    ucRegLow;

	if ((usb_ep == CONTROL_EP)||(usb_ep > INDEX_EP7))
		return uiRxCount;

	//access DATA_OUT_EP register map
	gUSBC_ComReg->EINDEX = usb_ep;
	
	ucRegLow = gUSBC_IdxReg->RXCSR_L;
	//Clear sentstall and restart data toggle.
	if(ucRegLow & DEV_RXCSR_SENTSTALL)
	{
		ucRegLow &= 0xBF;//clear SendStall bit
		ucRegLow |= DEV_RXCSR_CLR_DATA_TOG;//set ClrDataTog
		gUSBC_IdxReg->RXCSR_L = ucRegLow;
	}

	//Start receive data packet
	if(ucRegLow & DEV_RXCSR_RXPKTRDY)	 //RxPktRdy=1
	{
		uiRxCount = gUSBC_IdxReg->RXCount_H;
		uiRxCount <<= 8;
		uiRxCount += gUSBC_IdxReg->RXCount_L;

		read_ep_buf(buf, USB_ENDPOINT_FIFO_ADDR+((usb_ep-1)<<2), uiRxCount);
		ClearRx();
	}
	return uiRxCount;
}

void USB_CCID_Init(void)
{
	//USB Interrupt and register Init
	//g_usbVer = 0;//USB1.1, 同时配置USB配置项为USB1.1模式
	//0:internal oscillator;  1:external oscillator
	// USBC_PHY_Init(0);
	g_usbVer = 1;//USB2.0, 同时配置USB配置项为USB2.0模式
	//0:internal oscillator;  1:external oscillator
	USBC_PHY_Init(1);
	
	CCM->PHYPA |= 0x0e00;	//no need to supply VBUS.

	/* Global USB Register */
	gUSBC_fifoReg = (USBCFIFO_Reg*)(USBC_BASE_ADDR+0x60);

	gUSBC_ComReg = (USBCCommonReg*)USBC_BASE_ADDR;
	
	gUSBC_IdxReg = (USBCIndexedReg*)(USBC_BASE_ADDR+0x10);
	
	//The suspend mode is disable before BULK-Only tranfer start
	g_suspendMode = 0;
	/* Setup USB register */
	//enable usb common interrupt
	//0		1		2		3		4		5		6		7 (bit)
	//Susp	Resume	Reset	SOF		Conn	Discon	SessReq	VBusErr
	gUSBC_ComReg->INTRUSBE = USB_INTERRUPT_RESET   |\
	                         USB_INTERRUPT_CONNECT |\
							 USB_INTERRUPT_DISCON  |\
							 USB_INTERRUPT_SUSPEND |\
							 USB_INTERRUPT_RESUME;
	//enable ep0 and ep1 tx interrupts,clear other tx interrupts

	gUSBC_ComReg->INTRTXE_L = USB_INTERRUPT_EP0   |\
	                          (1<<INDEX_EP1);
	gUSBC_ComReg->INTRRXE_L =(1<<INDEX_EP2) ;// (1<<INDEX_EP1)      |\
	                          //(1<<INDEX_EP2);

	//ensure ep0 control/status regesters appeare in the memory map.
	gUSBC_ComReg->EINDEX = CONTROL_EP;
	
	SetFIFO_Addr(0,0);

	//Enable Soft connection
	if(g_usbVer == 1)
		gUSBC_ComReg->UCSR  = USB_POWER_SOFT_CONN|USB_POWER_HS_ENAB;
	else
		gUSBC_ComReg->UCSR  = USB_POWER_SOFT_CONN;

	g_USBAddrFlag = 0;
	g_USBNewAddr = 0;

		//BIT_SET(IO_READ32(EIC_IER), INTERRUPT_SOURCE_USB);
	NVIC_Init(0, 0, USBC_IRQn, 1);
}


