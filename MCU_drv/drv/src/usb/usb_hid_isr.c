// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : usb_hid_isr.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "usb_reg.h"
#include "usb_const.h"
#include "usb_drv.h"

unsigned char usb_tx_int_flag = 0;
extern void USB_HID_SetTxSize(UINT16 Size);
extern void write_hid_ep_buf(UINT32 ep,UINT8 *out_buf,UINT16 uiLens);
/*
Description:USB interrupt service routine
*/
void USB_HID_ISR(void)
{
	//check common interrupt
	g_msgflags |= gUSBC_ComReg->INTRUSB;

    //Check RX interrupt
	
	//printf("INTRRX_L=0x%x\r\n",gUSBC_ComReg->INTRRX_L); 
	
	if((gUSBC_ComReg->INTRRX_L)&(1<<USB_ENDPOINT_INDEX))
 	{
		g_msgflags |= 0x10;
 	}
	//check TX interrupt
	g_msgflags1 |=gUSBC_ComReg->INTRTX_L;
}
//#pragma interrupt off

/*
Description: USB Endpoint0 Interrupt Service Routine,respond control transfer
*/
void USB_HID_EP0_ISR(void)
{
	unsigned char ucReg=0;

	//enable ep0 register map to be accessed
	gUSBC_ComReg->EINDEX = CONTROL_EP;

	ucReg=gUSBC_IdxReg->E0CSR_L;	
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
	if(ucReg & 0x01)
	{
		USB_HID_Ep0Handler();
	}

}


/////////////////////////////////////////////////////////////////////////////////////////
unsigned short USB_HID_EP1_RX_ISR(unsigned char* pData)
{
	uint16_t ucRegLow;
	uint16_t  number;
	
	number = 0;
	gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;
	ucRegLow  = gUSBC_IdxReg->RXCSR_L;
	if(ucRegLow & DEV_RXCSR_SENTSTALL)
	{
		ucRegLow  &= 0xBF;
		ucRegLow  |= DEV_RXCSR_CLR_DATA_TOG;
		gUSBC_IdxReg->RXCSR_L  = ucRegLow;
	}
	
	if(ucRegLow & DEV_RXCSR_RXPKTRDY)
  	{
  		number = usb_receive(pData);
  	}
	
  	return (number);
}

void USB_HID_EP1_TX_ISR(unsigned char* pData,unsigned short len)
{
	gUSBC_ComReg->EINDEX  = USB_ENDPOINT_INDEX;
	usb_send(pData,len);	
}

void USB_HID_EP_TX_ISR(unsigned char epID,unsigned char* pData,unsigned short len)
{
	UINT32 ep_ADDR;
	
	gUSBC_ComReg->EINDEX  = epID;//USB_ENDPOINT_INDEX;
	
	switch(epID)
	{
		case INDEX_EP1: ep_ADDR = USB_FIFO_ENDPOINT_1; break;
		case INDEX_EP2: ep_ADDR = USB_FIFO_ENDPOINT_2; break;
		case INDEX_EP3: ep_ADDR = USB_FIFO_ENDPOINT_3; break;
		case INDEX_EP4: ep_ADDR = USB_FIFO_ENDPOINT_4; break;
		case INDEX_EP5: ep_ADDR = USB_FIFO_ENDPOINT_5; break;
		case INDEX_EP6: ep_ADDR = USB_FIFO_ENDPOINT_6; break;
		case INDEX_EP7: ep_ADDR = USB_FIFO_ENDPOINT_7; break;
		default:  return ;
	}
	
	//usb_send(pData,len);	
	USB_HID_SetTxSize(len);
	write_hid_ep_buf(ep_ADDR,pData,len);
	SetTx();
	
	//while ( ( gUSBC_ComReg->INTRTX_L & (1<<epID)) ==0);
}
