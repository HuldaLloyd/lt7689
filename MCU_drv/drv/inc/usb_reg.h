// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : usb_reg.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __USBDEV_REG_H__
#define __USBDEV_REG_H__

#include "type.h"
#include "memmap.h"

/* Data Definition */
/******************************
*Notice: USB register is little endian
*	      Access these register by byte
*******************************/
//Data Struct of Common USB Registers

#pragma anon_unions

typedef struct _USBCCommonReg{
	volatile UINT8	FADDRR;	//Function address register
	volatile UINT8	UCSR;		//USB control and status register
	
	volatile UINT8	INTRTX_L;	//Low byte of Interrupt register for Endpoint0 and Tx Endpoint
	volatile UINT8	INTRTX_H;	//High byte of Interrupt register for Endpoint0 and Tx Endpoint	
	
	volatile UINT8	INTRRX_L;	//Low byte of Interrupt register for Rx Endpoint
	volatile UINT8	INTRRX_H;	//High byte of Interrupt register for Rx Endpoint
	
	volatile UINT8	INTRTXE_L;	//Low byte of Interrupt enable register for IntrTx
	volatile UINT8	INTRTXE_H;	//High byte of Interrupt enable register for IntrTx
	
	volatile UINT8	INTRRXE_L;	//Low byte of Interrupt enable register for IntrRx
	volatile UINT8	INTRRXE_H;	//High byte of Interrupt enable register for IntrRx
	
	volatile UINT8	INTRUSB;	//Interrupt register for common USB interrupts
	volatile UINT8	INTRUSBE;	//Interrupt enable register for IntrUSB
	
	volatile UINT8	FNUMR_L;	//Low byte of Frame number
	volatile UINT8	FNUMR_H;	//High byte of Frame number
	
	volatile UINT8	EINDEX;		//Index register for selecting the endpoint status and control register
	volatile UINT8	TSTMODE;	//Enables the USB test modes
} USBCCommonReg;

//Data Struct of Indexed Registers
typedef struct _USBCIndexedReg{
	volatile UINT8	TXMAXP_L;		//Low byte of Maximum packet size for peripheral Tx endpoint
	volatile UINT8	TXMAXP_H;		//High byte of Maximum packet size for peripheral Tx endpoint
	union{
		  volatile UINT8	E0CSR_L;	//Low byte of Control Status register for Endpoint0
		  volatile UINT8	TXCSR_L;	//Low byte of Control Status register for peripheral Tx endpoint
		};
	union{
		  volatile UINT8	E0CSR_H;	//High byte of Control Status register for Endpoint0
		  volatile UINT8	TXCSR_H;	//High byte of Control Status register for peripheral Tx endpoint
		};
	volatile UINT8	RXMAXP_L;			//Low byte of Maximum packet size for peripheral Rx endpoint
	volatile UINT8	RXMAXP_H;			//High byte of Maximum packet size for peripheral Rx endpoint
	
	volatile UINT8	RXCSR_L;			//Low byte of Control Status register for peripheral Rx endpoint
	volatile UINT8	RXCSR_H;			//High byte of Control Status register for peripheral Rx endpoint
	union{
		  volatile UINT8	E0COUNTR_L;	//Low byte of Number of received bytes in Endpoint0 FIFO
		  volatile UINT8	RXCount_L;		//Low byte of Number of bytes in peripheral Rx endpoint FIFO
		};
	union{
		  volatile UINT8	E0COUNTR_H;	//High byte of Number of received bytes in Endpoint0 FIFO
		  volatile UINT8	RXCount_H;		//High byte of Number of bytes in peripheral Rx endpoint FIFO
		};
	volatile UINT8	TXTYPE;
	union{
		  volatile UINT8	NAKLIMIT0;
		  volatile UINT8	TXINTERVAL;
		};	
} USBCIndexedReg;

//Dynamic FIFO
typedef	struct	_USBCFIFO_Reg{
	volatile UINT8	OTGCTRL;
	volatile UINT8	RESERVED;
	volatile UINT8	TXFIFOSZ;	//Tx Endpoint FIFO size,double buffer only set in one register(TX_fifosz/RX_fifosz)
	volatile UINT8	RXFIFOSZ;	//Rx Endpoint FIFO size,MAX FIFO size is 1024byte
	volatile UINT8	TX_fifoadd_L;	//Tx Endpoint FIFO address(Low 8bit)
	volatile UINT8	TX_fifoadd_H;	//Tx Endpoint FIFO address(High 8bit)
	volatile UINT8	RX_fifoadd_L;	//Rx Endpoint FIFO address(Low 8bit)
	volatile UINT8	RX_fifoadd_H;	//Rx Endpoint FIFO address(High 8bit)
} USBCFIFO_Reg;
//Define the USB buffer start address
#define		USB_BUFFER_ADDR					(EVB_SRAM_ADDR+USB_FIFO_OFFSET)

//Define the USB buffer size(unit:byte)used in USB Demo
//#define		USB_BUFFER_SIZE_H				0x40
//#define		USB_BUFFER_SIZE_L				0x00

//usb FIFO register
#define		USB_FIFO_ENDPOINT_0				USBC_BASE_ADDR + 0x20
#define		USB_FIFO_ENDPOINT_1				USBC_BASE_ADDR + 0x24
#define		USB_FIFO_ENDPOINT_2				USBC_BASE_ADDR + 0x28
#define		USB_FIFO_ENDPOINT_3				USBC_BASE_ADDR + 0x2C
#define		USB_FIFO_ENDPOINT_4				USBC_BASE_ADDR + 0x30
#define		USB_FIFO_ENDPOINT_5				USBC_BASE_ADDR + 0x34
#define		USB_FIFO_ENDPOINT_6				USBC_BASE_ADDR + 0x38
#define		USB_FIFO_ENDPOINT_7				USBC_BASE_ADDR + 0x3C
//Half-Word Access
#define		USB_FIFO_EP1TX_PTR0				USBC_BASE_ADDR + 0x40
#define		USB_FIFO_EP1TX_PTR1				USBC_BASE_ADDR + 0x42
#define		USB_FIFO_EP2TX_PTR0				USBC_BASE_ADDR + 0x44
#define		USB_FIFO_EP2TX_PTR1				USBC_BASE_ADDR + 0x46
#define		USB_FIFO_EP3TX_PTR0				USBC_BASE_ADDR + 0x48
#define		USB_FIFO_EP3TX_PTR1				USBC_BASE_ADDR + 0x4A
#define		USB_FIFO_EP0_PTR0				USBC_BASE_ADDR + 0x58

//USB Internal DMA Register
//DMA configurable up to 8 channels
#define DMA_INTR							(USBC_BASE_ADDR +0x200) //8bit
//channel 1
#define DMA_CNTL1							(USBC_BASE_ADDR +0x204) //16bit
#define DMA_CNTL1_L							(USBC_BASE_ADDR +0x204)
#define DMA_CNTL1_H							(USBC_BASE_ADDR +0x205)
#define DMA_ADDR1							(USBC_BASE_ADDR +0x208) //32bit
#define DMA_COUNT1							(USBC_BASE_ADDR +0x20c) //32bit
//channel 2
#define DMA_CNTL2							(USBC_BASE_ADDR +0x214) //16bit
#define DMA_CNTL2_L							(USBC_BASE_ADDR +0x214)
#define DMA_CNTL2_H							(USBC_BASE_ADDR +0x215)
#define DMA_ADDR2							(USBC_BASE_ADDR +0x218) //32bit
#define DMA_COUNT2							(USBC_BASE_ADDR +0x21c) //32bit
//channel 3
#define DMA_CNTL3							(USBC_BASE_ADDR +0x224) //16bit
#define DMA_CNTL3_L							(USBC_BASE_ADDR +0x224)
#define DMA_CNTL3_H							(USBC_BASE_ADDR +0x225)
#define DMA_ADDR3							(USBC_BASE_ADDR +0x228) //32bit
#define DMA_COUNT3							(USBC_BASE_ADDR +0x22c) //32bit
//channel 4
#define DMA_CNTL4							(USBC_BASE_ADDR +0x234) //16bit
#define DMA_CNTL4_L							(USBC_BASE_ADDR +0x234)
#define DMA_CNTL4_H							(USBC_BASE_ADDR +0x235)
#define DMA_ADDR4							(USBC_BASE_ADDR +0x238) //32bit
#define DMA_COUNT4							(USBC_BASE_ADDR +0x23c) //32bit
//channel 5
#define DMA_CNTL5							(USBC_BASE_ADDR +0x244) //16bit
#define DMA_CNTL5_L							(USBC_BASE_ADDR +0x244)
#define DMA_CNTL5_H							(USBC_BASE_ADDR +0x245)
#define DMA_ADDR5							(USBC_BASE_ADDR +0x248) //32bit
#define DMA_COUNT5							(USBC_BASE_ADDR +0x24c) //32bit
//channel 6
#define DMA_CNTL6							(USBC_BASE_ADDR +0x254) //16bit
#define DMA_CNTL6_L							(USBC_BASE_ADDR +0x254)
#define DMA_CNTL6_H							(USBC_BASE_ADDR +0x255)
#define DMA_ADDR6							(USBC_BASE_ADDR +0x258) //32bit
#define DMA_COUNT6							(USBC_BASE_ADDR +0x25c) //32bit
//channel 7
#define DMA_CNTL7							(USBC_BASE_ADDR +0x264) //16bit
#define DMA_CNTL7_L							(USBC_BASE_ADDR +0x264)
#define DMA_CNTL7_H							(USBC_BASE_ADDR +0x265)
#define DMA_ADDR7							(USBC_BASE_ADDR +0x268) //32bit
#define DMA_COUNT7							(USBC_BASE_ADDR +0x26c) //32bit
//channel 8
#define DMA_CNTL8							(USBC_BASE_ADDR +0x274) //16bit
#define DMA_CNTL8_L							(USBC_BASE_ADDR +0x274)
#define DMA_CNTL8_H							(USBC_BASE_ADDR +0x275)
#define DMA_ADDR8							(USBC_BASE_ADDR +0x278) //32bit
#define DMA_COUNT8							(USBC_BASE_ADDR +0x27c) //32bit

#define INTR_DMA1							(1<<0)
#define INTR_DMA2							(1<<1)
#define INTR_DMA3							(1<<2)
#define INTR_DMA4							(1<<3)
#define INTR_DMA5							(1<<4)
#define INTR_DMA6							(1<<5)
#define INTR_DMA7							(1<<6)
#define INTR_DMA8							(1<<7)

//dma cntl
#define CNTL_DMAEN(x)						((x)<<0)
#define CNTL_DIRECTION(x)					((x)<<1)
#define CNTL_DMAMODE(x)						((x)<<2)
#define CNTL_INTERE(x)						((x)<<3)
#define CNTL_EP(x)							((x)<<4)
#define CNTL_BUSERR(x)						((x)<<8)
#define CNTL_BURSTMODE(x)					((x)<<9)		
			


#endif
