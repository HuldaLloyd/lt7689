// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : usb_hid_control.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "usb_reg.h"
#include "usb_const.h"
#include "usb_hid_descriptor.h"
#include "usb_drv.h"
//#include "debug.h"
#include "ccm_drv.h"
#include "memmap.h"
#include "iomacros.h"
#include "cpm_drv.h"
//#include "LT32U03.h"
#include "sys.h"
#include "string.h"
#include "bsp.h"
#include "data.h"
#include "uart.h"
extern const unsigned char* HID_strTable[4];

#define DEBUG_USB 0

UINT8 led_value;
unsigned  char flag = 0;
UINT8 hid_rx_buf[512];
UINT8 hid_response_buf[512];

UINT8 g_u8LifeCycle;
UINT8 g_u8AuthFlg;
UINT8 report_buffer[64];

usb_Setup *Setup_Pkt;

typedef  struct  _device_request {
	UINT8  bmRequestType;
	UINT8  bRequest;
	UINT16 wValue;
	UINT16 wIndex;
	UINT16 wCount;
}DEVICE_REQUEST;

DEVICE_REQUEST dev_std_req;

static void USB_HID_GetStatus(void);
static void USB_HID_GetDescriptor(void);
static void USB_HID_EP0_SendStall(void);
//static void write_hid_ep0_buf(UINT8 *in_buf,UINT16 uiLen);


void USB_HID_Init(void)
{
	
	Setup_Pkt=(usb_Setup *)g_dev_std_req;
	
	g_cbwPacket=(UINT8*)g_cbwPacket_l;
	g_databuf=USB_BUFFER_ADDR;

	//USB Interrupt and register Init
#if USB_MODE_1D1
	g_usbVer = 0;//USB1.1, 同时配置USB配置项为USB1.1模式
	
#elif USB_MODE_2D0
	g_usbVer = 1;//USB2.0, 同时配置USB配置项为USB2.0模式
#else
	g_usbVer = 0;//USB1.1, 同时配置USB配置项为USB1.1模式
#endif

	//0:internal oscillator;  1:external oscillator
	USBC_PHY_Init(0);
	
	CCM->PHYPA |= 0x0e00;	//no need to supply VBUS.

	g_cswPacket[0]=0x55;
	g_cswPacket[1]=0x53;
	g_cswPacket[2]=0x42;
	g_cswPacket[3]=0x53;

	/* Global USB Register */
	gUSBC_fifoReg = (USBCFIFO_Reg*)(USBC_BASE_ADDR+0x60);

	gUSBC_ComReg = (USBCCommonReg*)USBC_BASE_ADDR;
	gUSBC_IdxReg = (USBCIndexedReg*)(USBC_BASE_ADDR+0x10);
	//USB data buffer
	g_databuf = USB_BUFFER_ADDR;
	//The suspend mode is disable before BULK-Only tranfer start
	g_suspendMode = 0;
	/* Setup USB register */
	//enable usb common interrupt
	//0		1		2		3		4		5		6		7 (bit)
	//Susp	Resume	Reset	SOF		Conn	Discon	SessReq	VBusErr
	gUSBC_ComReg->INTRUSBE = USB_INTERRUPT_RESET  |\
	                         USB_INTERRUPT_CONNECT|\
					 USB_INTERRUPT_DISCON |\
					 USB_INTERRUPT_SUSPEND|\
					 USB_INTERRUPT_RESUME;
	//enable ep0 and ep1 tx interrupts,clear other tx interrupts
	gUSBC_ComReg->INTRTXE_L = USB_INTERRUPT_EP0|(1<<INDEX_EP2);
	
	gUSBC_ComReg->INTRRXE_L = (1<<INDEX_EP1);

	//ensure ep0 control/status regesters appeare in the memory map.
	gUSBC_ComReg->EINDEX = CONTROL_EP;
	
	HID_SetFIFO_Addr(0,0);

	//Enable Soft connection
	if(g_usbVer == 1)
		gUSBC_ComReg->UCSR  = USB_POWER_SOFT_CONN|USB_POWER_HS_ENAB;
	else

	gUSBC_ComReg->UCSR  = USB_POWER_SOFT_CONN;

	g_USBAddrFlag = 0;
	g_USBNewAddr = 0;

	NVIC_Init(0, 0, USBC_IRQn, 1);
}
/*
Description:USB Read data from Endpoint FIFO
Inputs:
	ep,endpoint number
	out_buf,points to memory data will be moved to.
	uiLens,data length
*/
void read_hid_ep_buf(UINT8 *out_buf,UINT32 ep,UINT16 uiLens)
{
	UINT16	i=0;

	for(i=0;i<uiLens;i++)
	{
		out_buf[i]=IO_READ8(ep);
	}
	ClearRx();	//clear RxPktRdy bit
}

/*
Description:USB write data to Endpoint FIFO
Inputs:
	ep,endpoint number
	out_buf,points to memory data will be moved to.
	uiLens,data length
*/
void  write_hid_ep_buf(UINT32 ep,UINT8 *out_buf,UINT16 uiLens)
{
	UINT16	i;
	for(i=0;i<uiLens;i++)
		IO_WRITE8(ep,out_buf[i]);
}

/*
Description:Set ep0 enter idle,ClearFeature request will clear STALL condition.
*/
static void USB_HID_EP0_SendStall(void)
{
	UINT8 ucReg=gUSBC_IdxReg->E0CSR_L;

	ucReg |=DEV_CSR0_SENDSTALL;
	gUSBC_IdxReg->E0CSR_L = ucReg;
}

/**************************************************************************************
Description:USB Write data to Endpoint0  FIFO,after write over,will generate a interrupt.
Inputs:
	in_buf,data source 
	uiLen,data length
**************************************************************************************/
void write_hid_ep0_buf(UINT8 *in_buf,UINT16 uiLen)
{
	UINT8 ucReg=gUSBC_IdxReg->E0CSR_L;
	
	write_hid_ep_buf(USB_FIFO_ENDPOINT_0,in_buf,uiLen);

	//set TxPktRdy =1 and DataEnd =1
	ucReg |= DEV_CSR0_DATAEND|DEV_CSR0_TXPKTRDY;
	gUSBC_IdxReg->E0CSR_L=ucReg;
	//It will generate a TX interrupt.
}

/*
Description:support usb high speed test mode
//\\add by ctzhu at 2008-04-08
*/
void USB_HID_TestMode(void)
{
	UINT8 test_packet[53] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 
		0xAA, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 
		0xEE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xBF, 0xDF, 
		0xEF, 0xF7, 0xFB, 0xFD, 0xFC, 0x7E, 0xBF, 0xDF, 
		0xEF, 0xF7, 0xFB, 0xFD, 0x7E};
	
	switch(g_dev_std_req[5])
	{
		case 1:		//Test_J
			gUSBC_ComReg->TSTMODE = 0x02;
//			asm{wait}
			break;
		case 2:		//Test_K
			gUSBC_ComReg->TSTMODE = 0x04;
//			asm{wait}
			break;
		case 3:		//Test_SE0_NAK
			gUSBC_ComReg->TSTMODE = 0x01;
//			asm{wait}
			break;
		case 4:		//Test_Packet
			write_hid_ep_buf(USB_FIFO_ENDPOINT_0,test_packet,53);
			gUSBC_IdxReg->E0CSR_L = 0x02;
			gUSBC_ComReg->TSTMODE = 0x08;
//			asm{wait}
			break;
		default:
			break;			
	}
}

/*
Description: Set the size of send data from FIFO
	Size = 0; Set the MAX vaule of data size(USB1.1/2.0)
	Size != 0; Set the request value of data size
*/
void USB_HID_SetTxSize(UINT16 Size)
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
		if(g_usbVer == 1){
			gUSBC_IdxReg->TXMAXP_L = USB_MAX_PACKET_SIZE_LOW;
			gUSBC_IdxReg->TXMAXP_H = USB_MAX_PACKET_SIZE_HIGH;
		}else

		{
			gUSBC_IdxReg->TXMAXP_L = USB_MAX_PACKET_SIZE_LOW_V11;
			gUSBC_IdxReg->TXMAXP_H = USB_MAX_PACKET_SIZE_HIGH_V11;
		}	
	}

}

/*
Description:USB Bus Reset Interrupt
*/
void SetAddrForEIndex(uint8_t ep,uint32_t offset)
{
	uint8_t ucMode;
	uint8_t tempL, tempH;	

	gUSBC_ComReg->EINDEX = ep;
    //device into idle state
    //check whether device works on HS.
    ucMode = gUSBC_ComReg->UCSR; //need clear suspend flag?
    if(ucMode & 0x10)
    {
        g_usbVer = 1;//USB2.0
    }
    else
    {
        g_usbVer = 0;
    }
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
    //gUSBC_fifoReg->TX_fifoadd_L = ((USB_FIFO_OFFSET>>3)&0x00FF);
    //gUSBC_fifoReg->TX_fifoadd_H = ((USB_FIFO_OFFSET>>11)&0x00FF);
    //gUSBC_fifoReg->RX_fifoadd_L = ((USB_FIFO_OFFSET>>3)&0x00FF);
    //gUSBC_fifoReg->RX_fifoadd_H = ((USB_FIFO_OFFSET>>11)&0x00FF);
    gUSBC_fifoReg->TX_fifoadd_L = ((offset>>3)&0x00FF);
    gUSBC_fifoReg->TX_fifoadd_H = ((offset>>11)&0x00FF);
    gUSBC_fifoReg->RX_fifoadd_L = (((offset+512)>>3)&0x00FF);
    gUSBC_fifoReg->RX_fifoadd_H = (((offset+512)>>11)&0x00FF);
    
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
     gUSBC_IdxReg->RXCSR_H = 0;
 
    gUSBC_IdxReg->RXMAXP_L = tempL;
    gUSBC_IdxReg->RXMAXP_H = tempH;
 
    //Flush Tx Ep FIFO
    gUSBC_IdxReg->TXCSR_L = DEV_TXCSR_FLUSH_FIFO;
    
    //Flush Rx Ep FIFO
    gUSBC_IdxReg->RXCSR_L = DEV_RXCSR_FLUSH_FIFO;
}

/*
Description:USB Bus Reset Interrupt
*/
void USB_HID_BusReset(void)
{
	UINT8 ucMode;

	//write FAddr 0
	gUSBC_ComReg->FADDRR = 0;
	//device into idle state
	//check whether device works on HS.
	ucMode = gUSBC_ComReg->UCSR; //need clear suspend flag?
	if(ucMode & 0x10)
		g_usbVer = 1;//USB2.0
	else
		g_usbVer = 0;
		
	SetAddrForEIndex(INDEX_EP1,0);
	SetAddrForEIndex(INDEX_EP2,1024);
	SetAddrForEIndex(INDEX_EP3,1024*2);
	SetAddrForEIndex(INDEX_EP4,1024*3);
	SetAddrForEIndex(INDEX_EP5,1024*4);
	
}
extern unsigned  char flag;

void USB_HID_getreport(void)
{
	UINT8 ucReg;
	ucReg  = dev_std_req.wCount>>8;
	gUSBC_IdxReg->E0CSR_L  = DEV_CSR0_SERVICE_RXPKTRDY;
	write_ep0_buf(report_buffer,ucReg);
}


void USB_HID_setreport(void)
{
	gUSBC_ComReg->EINDEX = CONTROL_EP;
	gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY;
	//setreport_flag=1;
}
void USB_HID_Ep0Handler(void)
{
	UINT8 ucReq=0;
	//read stand request from USBC FIFO to g_dev_std_req
	read_hid_ep_buf(g_dev_std_req,USB_FIFO_ENDPOINT_0,8);
	
	
	memcpy((UINT8*)&dev_std_req,g_dev_std_req,8);
	ucReq=g_dev_std_req[1];//dev_std_req->bRequest;
	

	#if DEBUG_USB
	printf("std req:");
	for(loop=0;loop<8;loop++)
		printf("%02x ",g_dev_std_req[loop]);
	printf("\r\n");
#if 0
	printf("request type:%02x,",dev_std_req.bmRequestType);
	printf("request :%02x,",dev_std_req.bRequest);
	printf("request values:%04x,",dev_std_req.wValue);
	printf("request index:%04x,",dev_std_req.wIndex);
	printf("request count:%04x,",dev_std_req.wCount);
#endif
	#endif

	switch(Setup_Pkt->bRequest)//switch(ucReq)
	{
		case 0://get status
			USB_HID_GetStatus();
			break;
		case 1://hid get report
			USB_HID_getreport();
			break;
		case 3://SetFeature
			//set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
			gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY|DEV_CSR0_DATAEND;
			//if(g_dev_std_req[0]==0x0)	//test mode
			//	USB_HID_TestMode();
			break;
		case 5://SetAddress
			//ucReq = g_dev_std_req[2];// & 0x7f;dev_std_req->wValue>>8
			//set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
			gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY|DEV_CSR0_DATAEND;			
			g_USBNewAddr = Setup_Pkt->wValue_l;//ucReq;
			break;
		case 6:
			USB_HID_GetDescriptor();
			break;
		case 8:
			//set ServiceRxPktRdy to clear RxPktRdy
			ucReq = 0;
			gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY|DEV_CSR0_DATAEND;				
			write_ep0_buf(&ucReq,1);			
			break;
		case 9:
			if(dev_std_req.bmRequestType == 0x21)
			{
#if DEBUG_USB
			printf (" set report\r\n:");
			#endif//SetConfigurationUSB_HID_setreport();
			}
			else
			{
#if DEBUG_USB
			printf ("set configuration\r\n:");
			#endif//SetConfiguration//set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
			gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY|DEV_CSR0_DATAEND;
			}
			break;
		case 10:
			if(dev_std_req.bmRequestType == 0x21)
			{
				//USB_HID_EP0_SendStall();
				gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY|DEV_CSR0_DATAEND;
				break;
			}
			else
			{
				ucReq = 0;
				gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY;
				write_ep0_buf(&ucReq,1);
			}
			break;		
		case 11://SetInterface
			gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY|DEV_CSR0_DATAEND;			
			break;
		default:
			USB_HID_EP0_SendStall();
			break;	
	}
}


/*
Description:USB standard command GetDescriptor
*/
static void USB_HID_GetDescriptor(void)
{
	gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY;

	switch(Setup_Pkt->wValue_h)//switch(reqIndex)
	{
		
		case mDEVICE://get device descriptor request
			if(Setup_Pkt->wLength_l<19) write_ep0_buf((UINT8*)USB_hid_Device_Descriptor,Setup_Pkt->wLength_l);
			else write_ep0_buf((UINT8*)USB_hid_Device_Descriptor,8);
			break;
		
		case mCONFIGURATION://get configuration descriptor request						
			if(Setup_Pkt->wLength_l<10) 
				write_ep0_buf((UINT8*)USB_hid_Configuration_Descriptor,Setup_Pkt->wLength_l);
			else  write_ep0_buf((UINT8*)USB_hid_Configuration_Descriptor,sizeof(USB_hid_Configuration_Descriptor));
			break;
		
		case mSTRING://get string descriptor request		
			write_ep0_buf((UINT8*)HID_strTable[Setup_Pkt->wValue_l],HID_strTable[Setup_Pkt->wValue_l][0]);			
			break;

		case mDEVICE_QUALIFIE:
			//printf("mDEVICE_QUALIFIE\r\n");
			break;
		
		case mOTHER_SPEED_CONFIGURATION://get other_speed_configuration descriptor
			//printf("mOTHER_SPEED_CONFIGURATION\r\n");
			break;
		
		case mREPORT:                 //HID
			//ucReg = USB_HID_Configuration_Descriptor[25];			
			write_ep0_buf((UINT8*)USB_hid_report_Descriptor2,sizeof(USB_hid_report_Descriptor2));
			break;
		default:
			USB_HID_EP0_SendStall();
			break;	
	}
}

/*
Description:USB standard command GetStatus
*/
static void USB_HID_GetStatus(void)
{
	UINT8 txdat[2]={0,0};
	
	//set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
	gUSBC_IdxReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY|DEV_CSR0_DATAEND;

    switch(g_dev_std_req[0])//dev_std_req->bmRequestType
	{
		case 0x80:
		case 0x81:
			write_ep0_buf(txdat,2);
			break;
		default:
	        USB_HID_EP0_SendStall();
			break;
	}	
	
}

//set fifo offset address(0~0x2000,only 8k SRAM can set to be FIFO)
void HID_SetFIFO_Addr(UINT32 bufferAddr,UINT8 mode)
{
	//gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;
	if(mode==0)
	{	
		//reset to default fifo address
		gUSBC_fifoReg->TX_fifoadd_L = ((USB_FIFO_OFFSET>>3)&0x00FF);
		gUSBC_fifoReg->TX_fifoadd_H = ((USB_FIFO_OFFSET>>11)&0x00FF);
		gUSBC_fifoReg->RX_fifoadd_L = (((USB_FIFO_OFFSET+512)>>3)&0x00FF);
		gUSBC_fifoReg->RX_fifoadd_H = (((USB_FIFO_OFFSET+512)>>11)&0x00FF);
	}
	else if(mode==1)
	{
		//Set Tx fifo Address
		gUSBC_fifoReg->TX_fifoadd_L = ((bufferAddr>>3)&0x00FF);
		gUSBC_fifoReg->TX_fifoadd_H = ((bufferAddr>>11)&0x00FF);	
	}
	else if(mode==2)
	{
		//Set Rx fifo Address
		gUSBC_fifoReg->RX_fifoadd_L = ((bufferAddr>>3)&0x00FF);
		gUSBC_fifoReg->RX_fifoadd_H = ((bufferAddr>>11)&0x00FF);
	
	}
}

UINT16 usb_receive(UINT8 *buf)
{
	UINT16   uiRxCount;
	UINT16 loop = 0;
	
	loop = loop;

	uiRxCount   =  gUSBC_IdxReg->RXCount_H;
	uiRxCount   <<= 8;
	uiRxCount += gUSBC_IdxReg->RXCount_L;

	if(uiRxCount == 0)
		return 0;

	if(uiRxCount > 0)
	{
		read_hid_ep_buf((UINT8 *)buf,USB_ENDPOINT_FIFO_ADDR,uiRxCount);

#if DEBUG_USB
		printf("rx len:%04x\r\n",uiRxCount);
		for(loop=0;loop<uiRxCount;loop++)
		{
			printf("%02x ",buf[loop]);
		}
#endif
	}
	return  uiRxCount;
}


void usb_send(UINT8 *buf,UINT16 len)
{
//	SetFIFO_Addr(USB_FIFO_OFFSET,0);
	USB_HID_SetTxSize(len);
	write_hid_ep_buf(USB_ENDPOINT_FIFO_ADDR,buf,len);
	SetTx();
}

void Data_Process(void)
{
	memset(hid_response_buf, 0, 512);
	
#if 0
	memcpy(hid_response_buf,hid_rx_buf,512);
#else
	if(hid_rx_buf[0] == 0xcc)
	{
		if(hid_rx_buf[1] == 0x01)  //boot
		{
			hid_response_buf[0] = 0x90;
			hid_response_buf[1] = 0x0;
		}
		else if(hid_rx_buf[1] == 0x02)  //取32Byte随机数
		{
			hid_response_buf[32] = 0x90;
			hid_response_buf[33] = 0x0;
		}
	}
	else
	{
		hid_response_buf[0] = 0xAA;
		hid_response_buf[1] = 0x55;
	}
#endif
	
}

extern UINT8  hid_rec_index;


extern uint16_t allnum;
uint16_t allnum = 0;
void USB_HID_Polling(void)
{
	uint16_t reNum;
	uint16_t num = 0;
	if(g_msgflags&0x10)
	{
		g_msgflags &=0xEF;	//Clear RX Interrupt Bit
		reNum = USB_HID_EP1_RX_ISR(hid_rx_buf);
		
		#if (UARTBUS_OPTION == 3)
		
		
		if (hid_rx_buf[0] == SCI_C0 && hid_rx_buf[1] == SCI_C1 && Hid_receive_flag != 1)
		{
			Hid_Rx_Buffer_long[Hid_Rx_Count_long++ % Hid_SIZE] = hid_rx_buf[0];
			Hid_Rx_Buffer_long[Hid_Rx_Count_long++ % Hid_SIZE] = hid_rx_buf[1];
			Hid_Rx_Buffer_long[Hid_Rx_Count_long++ % Hid_SIZE] = hid_rx_buf[2];
			Hid_Rx_Num += 3;
			if (hid_rx_buf[2] != 0)
			{
				Hid_receive_count = hid_rx_buf[2];
				num = 3;
			}
			else 
			{
				Hid_Rx_Buffer_long[Hid_Rx_Count_long++ % Hid_SIZE] = hid_rx_buf[3];
				Hid_Rx_Buffer_long[Hid_Rx_Count_long++ % Hid_SIZE] = hid_rx_buf[4];
				Hid_Rx_Num += 2;
				Hid_receive_count = (hid_rx_buf[3] << 8) + hid_rx_buf[4];
				num = 5;
		
			}
			
			for(uint16_t i = 0; i < Hid_receive_count; i++)
			{
				
				Hid_Rx_Buffer_long[Hid_Rx_Count_long++ % Hid_SIZE] = hid_rx_buf[i + num];
				//printf("Hid_Rx_Buffer_long[%d]=%x \r\n",(Hid_Rx_Count_long % Hid_SIZE) - 1,Hid_Rx_Buffer_long[(Hid_Rx_Count_long % Hid_SIZE) - 1]);
				Hid_Rx_Num ++;
				
				#if USB_MODE_1D1
				if (i + num == 63)
				{
					Hid_receive_flag = 1;
					Hid_receive_num = i + 1;
					break;
				}
				#elif USB_MODE_2D0
				
				if (i + num == 511)
				{
					Hid_receive_flag = 1;
					Hid_receive_num = i + 1;
					break;
				}
				#endif
			}
		}
		else if (Hid_receive_flag == 1)
		{
			Hid_receive_flag = 0;
			for(uint16_t i = 0; i < Hid_receive_count - Hid_receive_num; i++)
			{
				Hid_Rx_Buffer_long[Hid_Rx_Count_long++ % Hid_SIZE] = hid_rx_buf[i];
				Hid_Rx_Num ++;
				
				
				#if USB_MODE_1D1
				if (i == 63)
				{
					Hid_receive_num = Hid_receive_num + 64;
					if (Hid_receive_count > Hid_receive_num)
						Hid_receive_flag = 1;
					break;
				}
				#elif USB_MODE_2D0
				
				if (i == 511)
				{
					Hid_receive_num = Hid_receive_num + 512;
					if (Hid_receive_count > Hid_receive_num)
						Hid_receive_flag = 1;
					break;
				}
				#endif
				/*
				if (i == 63)
				{
					Hid_receive_num = Hid_receive_num + 64;
					if (Hid_receive_count > Hid_receive_num)
						Hid_receive_flag = 1;
					break;
				}
				*/
			}
			
		}
		#endif
		hid_rec_index = 1;
	}

	// if bus reset
	if(g_msgflags & 0x04)
	{
		g_msgflags &=0xFB;
		USB_HID_BusReset();
	}

	//to process setup packet
	if(g_msgflags1 & 0x1)
	{
		if(g_USBNewAddr){
			gUSBC_ComReg->FADDRR = g_USBNewAddr;
			//set usb address once
			g_USBNewAddr = 0;
		}
		g_msgflags1 &=0xFE;
		USB_HID_EP0_ISR();
	}

	// if suspend change
	if(g_msgflags & 0x1)
	{
		g_msgflags &=0xFE;
#ifdef __USB_SUSPEND_EN__
	USBDev_Suspend();
#endif
	}
}



