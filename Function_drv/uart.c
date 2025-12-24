/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     uart.c
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief
 *************************************************************************/

#include "uart.h"
#include "bsp.h"
#include "pit.h"
#include "pit32_drv.h"
#include "ioctrl_drv.h"
#include "usb_drv.h"
#include "usb_const.h"
//////////////////////////////////////////////////////////////////////////////////

uint8_t page_refreshing = 0;
/*****************************CRC check*****************************/

// CRC value of high byte
const uint8_t auchCRCHi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40};

// CRC value of low byte
const char auchCRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40};

/* The function returns CRC with uint16_t type*/
uint16_t CRC16(uint8_t *puchMsg, uint16_t usDataLen)
{
	uint8_t uchCRCHi = 0xFF;			// CRC high byte initialization
	uint8_t uchCRCLo = 0xFF;			// CRC low byte initialization
	uint16_t uIndex;		 			// CRC query table index
	while (usDataLen--)					// Complete the entire message buffer
	{
		uIndex = uchCRCLo ^ *puchMsg++;	// calculate CRC
		uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
		uchCRCHi = auchCRCLo[uIndex];
	}
	return (uchCRCHi << 8 | uchCRCLo);
}

/*  Serial port printing debugging(SCI3) */

// Add the following code to support the printf function without selecting use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
// Solve some bugs that may report errors when using HAL library
int _ttywrch(int ch)
{
	ch = ch;
	return ch;
}

// Support functions required by the standard library
struct __FILE
{
	int handle;
	/* Whatever you require here. If the only file you are using is */
	/* standard output using printf() for debugging, no file handling */
	/* is required. */
};

/* FILE is typedef?? d in stdio.h. */
FILE __stdout;

// Definition_ sys_ Exit () to avoid using half-host mode
void _sys_exit(int x)
{
	x = x;
}

// Redefine fputc function
int fputc(int ch, FILE *f)
{
	while ((SCI3->SCISR1 & 0x40) == 0x00)
	{
	};
	SCI3->SCIDRL = (uint8_t)ch;

	return ch;
}
#endif

void UartDBG_Init(uint32_t pclk, uint32_t bound)
{
	uint32_t band_rate = 0;
	SCI3->SCIBRDF = (((pclk * 8 / bound) + 1) / 2) & 0x003f;
	band_rate = (pclk * 4 / bound) >> 6;
	SCI3->SCIBDH = (uint8_t)((band_rate >> 8) & 0x00ff);
	SCI3->SCIBDL = (uint8_t)(band_rate & 0x00ff);

	SCI3->SCICR2 |= (1 << 2) | (1 << 3) | (1 << 5);	// enable receive, send enable and receive interrupt	
}

/*  SCI1 communication drive  */
#include "dmac_drv.h"
#if (UARTBUS_OPTION == 1)
DMA_CHANNEL_REG *sci_dma_channel[2][DMAC_CHNUM] = {{(DMA_CHANNEL_REG *)(DMA1_BASE_ADDR), (DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0x58),
													(DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0xB0), (DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0x108)},
												   {(DMA_CHANNEL_REG *)(DMA2_BASE_ADDR), (DMA_CHANNEL_REG *)(DMA2_BASE_ADDR + 0x58),
													(DMA_CHANNEL_REG *)(DMA2_BASE_ADDR + 0xB0), (DMA_CHANNEL_REG *)(DMA2_BASE_ADDR + 0x108)}}; // global struct variable for for Channel registers

DMA_CONTROL_REG *sci_dma_control[2] = {(DMA_CONTROL_REG *)(DMA1_BASE_ADDR + 0x2C0),
									   (DMA_CONTROL_REG *)(DMA2_BASE_ADDR + 0x2C0)}; // global struct variable for for DMAC registers
#endif

void SCI1_Init(uint32_t pclk, uint32_t bound,uint8_t parity)	
{
	uint8_t dmaNum = 0;
	uint8_t n = 2;

	uint32_t band_rate = 0;
	SCI1->SCIBRDF = (((pclk * 8 / bound) + 1) / 2) & 0x003f;
	band_rate = (pclk * 4 / bound) >> 6;
	SCI1->SCIBDH = (uint8_t)((band_rate >> 8) & 0x00ff);
	SCI1->SCIBDL = (uint8_t)(band_rate & 0x00ff);

#if (UARTBUS_OPTION == 1)
	SCI1->SCIFCR |= SCIFCR_TFEN;
	SCI1->SCIDCR |= SCIDCR_TXDMAE_MASK; // Uart_DMA enable

	sci_dma_control[dmaNum]->DMA_CHEN &= (~(CHANNEL_WRITE_ENABLE(n) | CHANNEL_ENABLE(n)));	// DMA disable

	//	sci_dma_control[dmaNum]->DMA_CONFIG = 0;	//For debug
	sci_dma_channel[dmaNum][n]->DMA_DADDR = (uint32_t)&SCI1->SCIDRL;
	sci_dma_channel[dmaNum][n]->DMA_CTRL = DNOCHG | SIEC | M2P_DMA;
	//	sci_dma_control[dmaNum]->DMA_CONFIG = 1;	//For debug

	sci_dma_channel[dmaNum][n]->DMA_CFG = (HS_SEL_SRC_SOFT) | (HS_SEL_DST_HARD) | (FIFO_EMPTY) | (CH_PRIOR2);
	sci_dma_channel[dmaNum][n]->DMA_CFG_HIGH = SRC_PER_SCI1_TX | DST_PER_SCI1_TX;
#endif

	if(parity_flag & 0x01)	
	{
		if(parity_flag & 0x02)
			SCI1->SCICR1 |= (1 << 0);	//奇校验
		else
			SCI1->SCICR1 &=~(1 << 0);	//偶校验
		SCI1->SCICR1 |= (1 << 1) | (1 << 4);	//打开校验和设置11位帧
	}
	SCI1->SCICR2 |= (1 << 2) | (1 << 3) | (1 << 5);	// enable receive, send enable and receive interrupt
	NVIC_Init(0, 0, SCI1_IRQn, 2);
}

void SendData_SCI1(uint8_t data)
{
	SCI1->SCIDRL = (uint8_t)data;
}

void LT_SingleByteToPc(uint8_t val)
{
	while ((SCI1->SCISR1 & 0x40) == 0x00)
	{
	};
	SCI1->SCIDRL = val;
}

void LT_ClearRxBuf(void)
{
	uint16_t i = 0;
	for (i = 0; i < RX_SIZE; i++)
	{
		gUsartRx.Buf[i] = 0;
	}
}

// uart feedback -- package frame header(Levetop protocol) + data + length + CRC
void LT_SendData_CRC_Frame(uint8_t *buf, uint8_t len)
{
#if !IIC_BUS

#if (UARTBUS_OPTION == 0)
	uint16_t i = 0;
	uint16_t TxToPc_crc;
	uint8_t crc[2] = {0};

	*(buf + 0) = SCI_C0;
	*(buf + 1) = SCI_C1;
	if (CRC_Enable_Flag)
	{
		TxToPc_crc = CRC16(buf + 3, len);
		crc[0] = (uint8_t)(TxToPc_crc & 0x00ff);
		crc[1] = (uint8_t)((TxToPc_crc >> 8) & 0x00ff);

		len += 2;

		*(buf + len + 1) = crc[0];
		*(buf + len + 2) = crc[1];
	}
	*(buf + 2) = len;

	for (i = 0; i < 3 + len; i++)
	{
		while ((SCI1->SCISR1 & 0x40) == 0x00)
		{
		};
		SendData_SCI1(*(buf + i));
	}
	// For debug
	// sci_dma_control[0]->DMA_CHEN &= ~(CHANNEL_WRITE_ENABLE(2)|CHANNEL_ENABLE(2));//DMA disable
	// sci_dma_channel[0][2]->DMA_SADDR = (uint32_t)buf;
	// sci_dma_channel[0][2]->DMA_CTRL_HIGH = len+3;
	//
	// //sci_dma_control[0]->DMA_MASKTFR |= CHANNEL_UMASK(2);
	// sci_dma_control[0]->DMA_CHEN |= (CHANNEL_WRITE_ENABLE(2)|CHANNEL_ENABLE(2));

#endif
#endif
	
#if (UARTBUS_OPTION == 3)
	uint16_t i = 0;
	uint16_t TxToPc_crc;
	uint8_t crc[2] = {0};
	uint8_t count1,count2;
	uint8_t rebuf[512] = {0};
	*(buf + 0) = SCI_C0;
	*(buf + 1) = SCI_C1;
	if (CRC_Enable_Flag)
	{
		TxToPc_crc = CRC16(buf + 3, len);
		crc[0] = (uint8_t)(TxToPc_crc & 0x00ff);
		crc[1] = (uint8_t)((TxToPc_crc >> 8) & 0x00ff);
		
		len += 2;
		
		*(buf + len + 1) = crc[0];
		*(buf + len + 2) = crc[1];
	}
	*(buf + 2) = len;

	#if USB_MODE_1D1
	count1 = (3 + len)  / 64;
	count2 = ((3 + len) % 64);

	
	for (i = 0;i < count1; i++)
	{
		USB_HID_EP_TX_ISR(INDEX_EP2,&buf[64 * i],64);
		DelayUS(1200);
	}
	if (count2 > 0)
	{
		memset(&rebuf,0,64);
		memcpy(rebuf,&buf[64 * count1],count2);
		USB_HID_EP_TX_ISR(INDEX_EP2,rebuf,64);

	}
	#elif USB_MODE_2D0
	
	count1 = (3 + len)  / 512;
	count2 = ((3 + len) % 512);


	
	for (i = 0;i < count1; i++)
	{
		USB_HID_EP_TX_ISR(INDEX_EP2,&buf[512 * i],512);
		DelayUS(1200);
	}
	if (count2 > 0)
	{
		memset(&rebuf,0,512);
		memcpy(rebuf,&buf[512 * count1],count2);
		USB_HID_EP_TX_ISR(INDEX_EP2,rebuf,512);
	}
	#endif

#endif

#if (UARTBUS_OPTION == 4)
	uint16_t i = 0;
	uint16_t TxToPc_crc;
	uint8_t crc[2] = {0};

	*(buf + 0) = SCI_C0;
	*(buf + 1) = SCI_C1;
	if (CRC_Enable_Flag)
	{
		TxToPc_crc = CRC16(buf + 3, len);
		crc[0] = (uint8_t)(TxToPc_crc & 0x00ff);
		crc[1] = (uint8_t)((TxToPc_crc >> 8) & 0x00ff);

		len += 2;

		*(buf + len + 1) = crc[0];
		*(buf + len + 2) = crc[1];
	}
	*(buf + 2) = len;

	usb_cdc_send(INDEX_EP1, buf,  3 + len);
	

#endif
}


void LT_SendFlashData_CRC_Frame(uint8_t *buf, uint16_t len)
{

#if !IIC_BUS

#if (UARTBUS_OPTION == 0)
	uint16_t i = 0;
	uint16_t TxToPc_crc;
	uint8_t crc[2] = {0};

	*(buf + 0) = SCI_C0;
	*(buf + 1) = SCI_C1;
	*(buf + 2) = 0;
	if (CRC_Enable_Flag)
	{
		TxToPc_crc = CRC16(buf + 5, len);
		crc[0] = (uint8_t)(TxToPc_crc & 0x00ff);
		crc[1] = (uint8_t)((TxToPc_crc >> 8) & 0x00ff);
		
		len += 2;
		*(buf + len + 3) = crc[0];
		*(buf + len + 4) = crc[1];
	}
	*(buf + 3) = (len >> 8) & 0xFF;
	*(buf + 4) = len & 0xFF;
	for (i = 0; i < 5 + len; i++)
	{
		while ((SCI1->SCISR1 & 0x40) == 0x00)
		{
		};
		SendData_SCI1(*(buf + i));
	}
#endif
#endif
	
#if (UARTBUS_OPTION == 3)
	
	uint16_t i = 0;
	uint16_t TxToPc_crc;
	uint8_t crc[2] = {0};
	uint8_t count1,count2;

	*(buf + 0) = SCI_C0;
	*(buf + 1) = SCI_C1;
	*(buf + 2) = 0;
	if (CRC_Enable_Flag)
	{
		TxToPc_crc = CRC16(buf + 5, len);
		crc[0] = (uint8_t)(TxToPc_crc & 0x00ff);
		crc[1] = (uint8_t)((TxToPc_crc >> 8) & 0x00ff);
		
		len += 2;
		*(buf + len + 3) = crc[0];
		*(buf + len + 4) = crc[1];
	}
	*(buf + 3) = (len >> 8) & 0xFF;
	*(buf + 4) = len & 0xFF;

	
	#if USB_MODE_1D1
	count1 = (5 + len)  / 64;
	count2 = ((5 + len) % 64);

	for (i = 0;i < count1; i++)
	{
		USB_HID_EP_TX_ISR(INDEX_EP2,&buf[64 * i],64);
		DelayUS(1200);
	}
	if (count2 > 0)
	{
		memset(&buf[64 * count1 + count2],0,64 - count2);
		USB_HID_EP_TX_ISR(INDEX_EP2,&buf[64 * count1],64);
	}
	#elif USB_MODE_2D0
	
	count1 = (5 + len)  / 512;
	count2 = ((5 + len) % 512);


	for (i = 0;i < count1; i++)
	{
		USB_HID_EP_TX_ISR(INDEX_EP2,&buf[512 * i],512);
		DelayUS(1200);
	}
	if (count2 > 0)
	{
		memset(&buf[512 * count1 + count2],0,512 - count2);
		USB_HID_EP_TX_ISR(INDEX_EP2,&buf[512 * count1],512);
	}
	#endif

#endif
	
#if (UARTBUS_OPTION == 4)
	uint16_t i = 0;
	uint16_t TxToPc_crc;
	uint8_t crc[2] = {0};

	*(buf + 0) = SCI_C0;
	*(buf + 1) = SCI_C1;
	*(buf + 2) = 0;
	if (CRC_Enable_Flag)
	{
		TxToPc_crc = CRC16(buf + 5, len);
		crc[0] = (uint8_t)(TxToPc_crc & 0x00ff);
		crc[1] = (uint8_t)((TxToPc_crc >> 8) & 0x00ff);
		
		len += 2;
		*(buf + len + 3) = crc[0];
		*(buf + len + 4) = crc[1];
	}
	*(buf + 3) = (len >> 8) & 0xFF;
	*(buf + 4) = len & 0xFF;

	usb_cdc_send(INDEX_EP1, buf,  5 + len);
#endif
}
// Uart feedback -- Packaging data + CRC
void LT_SendData_CRC_NoFrame(uint8_t *buf, uint8_t len)
{
	uint16_t i = 0;
	uint16_t TxToPc_crc;
	uint8_t crc[2] = {0};

	TxToPc_crc = CRC16(buf, len);
	crc[1] = (uint8_t)((TxToPc_crc >> 8) & 0x00ff);
	crc[0] = (uint8_t)(TxToPc_crc & 0x00ff);
	*(buf + len) = crc[0];
	*(buf + len + 1) = crc[1];

#if (UARTBUS_OPTION == 2)
	for (i = 0; i < len + 2; i++)
	{
		while ((SCI1->SCISR1 & 0x40) == 0x00)
		{
		};
		SendData_SCI1(*(buf + i));
	}
#endif
#if (UARTBUS_OPTION == 1)
	sci_dma_control[0]->DMA_CHEN &= ~(CHANNEL_WRITE_ENABLE(2) | CHANNEL_ENABLE(2)); // DMA disable
	sci_dma_channel[0][2]->DMA_SADDR = (uint32_t)buf;
	sci_dma_channel[0][2]->DMA_CTRL_HIGH = len + 2;

	// sci_dma_control[0]->DMA_MASKTFR |= CHANNEL_UMASK(2);
	sci_dma_control[0]->DMA_CHEN |= (CHANNEL_WRITE_ENABLE(2) | CHANNEL_ENABLE(2));
#endif
}

/*****************************Levetop protocol as slave*****************************/
#if (UARTBUS_OPTION == 0)
uint8_t Rx_Buffer_short[256] = {0};
uint8_t Rx_Buffer_long[4096 + 2] = {0};

uint16_t Rx_Count_short = 0;
uint16_t Rx_Count_long = 0;
uint16_t Rx_Num = 0;
uint16_t Respond_Num = 0;
uint16_t Respond_Count = 0;

uint8_t Respond_C0_OK = 0;
uint8_t Respond_C1_OK = 0;
uint8_t Respond_Length = 0;
uint8_t Respond_Flag = 0;
uint16_t Respond_FlashLength = 0;
void SCI1_IRQHandler(void)
{
	uint8_t ch = 0;
	if ((SCI1->SCISR1 & 0x20) == 0x20)
	{
		ch = SCI1->SCIDRL;
		if (Rx_Count_long > 4096 + 2)
			Rx_Count_long = 0;
		Rx_Buffer_long[Rx_Count_long++ % RX_SIZE] = ch;
		Rx_Num++;
	}
}

// Get each frame of data from the uart port receiving buffer
void Main_From_UartRxBuff(void)
{
	uint16_t i = 0;
	if (Respond_Num < Rx_Num)
	{
		for (i = 0; i < (Rx_Num - Respond_Num); i++)
		{
			if (Respond_C1_OK)
			{
				gUsartRx.Buf[gUsartRx.Count++] = Rx_Buffer_long[(i + Respond_Count) % RX_SIZE];
				if (Respond_Flag == 1)
				{
					Respond_Flag = 0;
					Respond_Length = Rx_Buffer_long[(i + Respond_Count) % RX_SIZE];
					if (Respond_Length == 0)
					{
						Respond_Flag = 2;
					}
				}
				else if (Respond_Length == 0)
				{
					if (Respond_Flag == 2)
					{
						Respond_FlashLength = Rx_Buffer_long[(i + Respond_Count) % RX_SIZE];
						Respond_Flag = 3;
					}
					else if(Respond_Flag == 3)
					{
						Respond_FlashLength = (Respond_FlashLength << 8) + Rx_Buffer_long[(i + Respond_Count) % RX_SIZE] + 2;
						Respond_Flag = 0;
					}
				}
				
				if ((gUsartRx.Count > Respond_Length && Respond_Length != 0) || (gUsartRx.Count > Respond_FlashLength && Respond_Length == 0 && Respond_Flag == 0))
				{
					gUsartRx.Flag = 1;
					Respond_C1_OK = 0;
					Respond_C0_OK = 0;
					break;
				}
			}
			if (Respond_C0_OK && !Respond_C1_OK && Rx_Buffer_long[(i + Respond_Count) % RX_SIZE] != SCI_C1)
			{
				Respond_C0_OK = 0;
				gUsartRx.Count = 0;
			}

			if (!Respond_C0_OK && Rx_Buffer_long[(i + Respond_Count) % RX_SIZE] == SCI_C0)
			{
				Respond_C0_OK = 1;
				gUsartRx.Count = 0;
			}
			if (!Respond_C1_OK && Rx_Buffer_long[(i + Respond_Count) % RX_SIZE] == SCI_C1 && Respond_C0_OK)
			{
				Respond_C1_OK = 1;
				Respond_Flag = 1;
				gUsartRx.Count = 0;
			}

//			if ((Respond_C1_OK || Respond_C0_OK) && gUsartRx.Count >= 255)
//			{
//				Respond_C0_OK = 0;
//				Respond_C1_OK = 0;
//				Respond_Flag = 0;
//				break;
//			}
//			if ((Respond_C1_OK || Respond_C0_OK) && Respond_Flag == 0 && gUsartRx.Count >= 255)
//			{
//				Respond_C0_OK = 0;
//				Respond_C1_OK = 0;
//				Respond_Flag = 0;
//				break;
//			}
		}
		Respond_Count = (i + Respond_Count) % RX_SIZE;
		Respond_Num += i;
	}
	else if (Respond_Num == Rx_Num && Rx_Num != 0)
	{
		Respond_Num = 0;
		Rx_Num = 0;
		Respond_Count = 0;
		Rx_Count_long = 0;
	}
}
#endif

#if (UARTBUS_OPTION == 1)
void SCI1_IRQHandler(void)
{
	uint8_t ch = 0;
	uint8_t n = 0, m = 0;

	if ((SCI1->SCISR1 & 0x20) == 0x20)
	{
		ch = SCI1->SCIDRL;
		if (gUsartRx.Count > 4096 + 2)
			gUsartRx.Count = 0;
		gUsartRx.Buf[gUsartRx.Count++] = ch;

		pit_1ms_count = 0;
		uart_flag = 1;
	}
}

// For debug
// void DMA1_IRQHandler(void)
// {
//	uint32_t dma_isr_errflag = 0;
// #ifdef MCC_DMA_INT
//		MCC_DMA_ISR();
// #endif
//	//complet int
//	if(m_dma_control->DMA_STATTFR & 0x04)//dma done
//	{
////		sci_dma_control[0]->DMA_CHEN &= ~(CHANNEL_WRITE_ENABLE(2)|CHANNEL_ENABLE(2));
//	}
//
//	if(m_dma_control->DMA_STATERR & 0x0f)//dma error
//	{
//		dma_isr_errflag = m_dma_control->DMA_STATERR & 0x0f;
//		m_dma_control->DMA_CLRERR = dma_isr_errflag;
//	}
//
//	m_dma_control->DMA_CLRTFR = m_dma_control->DMA_STATTFR;
//	m_dma_control->DMA_CLRBLOCK=m_dma_control->DMA_STATBLOCK;
//	m_dma_control->DMA_CLRSRC=m_dma_control->DMA_STATSRC;
//	m_dma_control->DMA_CLRDST=m_dma_control->DMA_STATDST;
//	m_dma_control->DMA_CLRERR=m_dma_control->DMA_STATERR;
// }

#endif

/*   ModBus protocol as host  */
#if (UARTBUS_OPTION == 2)
// Uart interrupt reception and pit1 (5ms) interrupt cooperate to complete the acquisition of each frame data
void SCI1_IRQHandler(void)
{
	uint8_t ch = 0;
	uint8_t n = 0, m = 0;

	if ((SCI1->SCISR1 & 0x20) == 0x20)
	{
		ch = SCI1->SCIDRL;
		if (gUsartRx.Count > 4096 + 2)
			gUsartRx.Count = 0;
		gUsartRx.Buf[gUsartRx.Count++] = ch;

		//		LT_UsartTimeSetCounter(23437);	//10ms
		pit_1ms_count = 0;
		uart_flag = 1;
	}
}

// char Master_Roll_buff[1600]={0};
//{
//	0xA5, 0x01, 0x03, 0x00,0x00, 0x00,0x46, 0x20,0x00, 0x00,0xc8,  0x05, 0x00, 0x00,0x00, 0x00,
//	0xA5, 0x01, 0x06, 0x00,0x41, 0x00,0x02, 0x20,0x41, 0x00,0x64,  0x05, 0x00, 0x00,0x00, 0x00,
//	0xA5, 0x01, 0x03, 0x00,0x23, 0x00,0x3C, 0x20,0x23, 0x00,0xc8,  0x05, 0x02, 0x30,0x35, 0x00,
//	0xA5, 0x01, 0x06, 0x00,0x02, 0x00,0x02, 0x20,0x02, 0x00,0xc8,  0x05, 0x03, 0x00,0x00, 0x00,
//	0xA5, 0x01, 0x06, 0x00,0x03, 0x00,0x02, 0x20,0x03, 0x00,0xc8,  0x05, 0x03, 0x00,0x01, 0x00
// };
/* Valid | Node | Instruction | Register address | Length | Variable address | Response time ms | Number of retransmissions | Instruction execution mode and corresponding parameters | Reserved

Command execution mode:
0x00 means that the command is executed on all pages, and the following parameters can be set to a fixed value of 0x0000;
0x01 indicates that the command is only executed on the specified page, and the parameter immediately following is set as the page ID number;
0x02 indicates that the corresponding pointer address data is 0x4C54 before the instruction is executed (the data of the pointer address will be cleared after execution), and the following parameter is set as the pointer address;
0x03 indicates that the command is executed only at the mark position 1 of the corresponding number of the customization mode, and the following parameter is the mark position number;
*/

#define MODBUS_ADDR 0xC000

volatile uint8_t tx_en_flag = 1;		// Allow sending flag bit
volatile uint8_t tx_repeat_flag = 0;	// Whether to resend flag bit
volatile uint8_t tx_repeat_cnt = 0;		// Cumulative number of retransmissions

volatile USART_TX_INFO gUsartTx;		// Modbus operation parameter information area

uint8_t Send_ModbusBuf[256] = {0};

volatile uint8_t Master_mode03_flag[100] = {0};	// Customized variables
volatile uint8_t Master_mode03_Var[200] = {0};	// Customized variables

// The transmission mechanism of host timing and repeated serial port data transmission
void Uart_cmd_Send(void)	
{
	uint8_t i = 0,j = 0;			
	uint16_t num=0, data_temp=0;
	uint8_t byte_temp = 0;	
	uint16_t sum=0, count=0, cnt=0;
	
	if (tx_en_flag & tx_500ms_flag)	// Allowed to send and within the fixed period
	{
		if (Cnt_ModbusTX == 0)
			tx_10ms_cnt = 0;		// It is cleared only after entering the transmission

		if (tx_repeat_flag)
		{
			tx_repeat_cnt++;
			/* If the cumulative number of retransmissions exceeds the maximum or the broadcast mode is exceeded,
			skip and execute the next command */
			if (tx_repeat_cnt > gUsartTx.RP_Cnt || gUsartTx.Sla_Add == 0)
			{
				tx_repeat_cnt = 0;
				tx_repeat_flag = 0;
				if (gUsartTx.Mode == 0x02)		// Data clearing of parameter pointer address in mode 2
				{
					var[2 * (gUsartTx.Opt_Add)] = 0;
					var[2 * (gUsartTx.Opt_Add) + 1] = 0;
				}
				else if (gUsartTx.Mode == 0x03) // Customized Code_ The flag bit corresponding to mode 3 is cleared
				{
					Master_mode03_flag[gUsartTx.Opt_Add] = 0;
				}
			}
		}

		if (tx_repeat_flag)
		{
			tx_repeat_flag = 0;
			Cnt_ModbusTX -= 16;
		}
		else
		{
			tx_repeat_cnt = 0;
			if (Cnt_ModbusTX >= Sum_ModbusTX)	// After all instructions are executed, wait for the next timing cycle to start before sending again
			{
				Cnt_ModbusTX = 0;
				//				tx_500ms_flag = 0;
				if (tx_10ms_cnt <= 50)
					tx_500ms_flag = 0;

				return;
			}
		}

		gUsartTx.Flag    = var[MODBUS_ADDR + Cnt_ModbusTX + 0];
		gUsartTx.Sla_Add = var[MODBUS_ADDR + Cnt_ModbusTX + 1];
		gUsartTx.CMD     = var[MODBUS_ADDR + Cnt_ModbusTX + 2];
		gUsartTx.REG_Add = var[MODBUS_ADDR + Cnt_ModbusTX + 3] << 8 | var[MODBUS_ADDR + Cnt_ModbusTX + 4];
		gUsartTx.Len     = var[MODBUS_ADDR + Cnt_ModbusTX + 5] << 8 | var[MODBUS_ADDR + Cnt_ModbusTX + 6];
		gUsartTx.Var_Add = var[MODBUS_ADDR + Cnt_ModbusTX + 7] << 8 | var[MODBUS_ADDR + Cnt_ModbusTX + 8];
		gUsartTx.Timeout = var[MODBUS_ADDR + Cnt_ModbusTX + 9] << 8 | var[MODBUS_ADDR + Cnt_ModbusTX + 10];
		gUsartTx.RP_Cnt  = var[MODBUS_ADDR + Cnt_ModbusTX + 11];
		gUsartTx.Mode    = var[MODBUS_ADDR + Cnt_ModbusTX + 12];
		gUsartTx.Opt_Add = var[MODBUS_ADDR + Cnt_ModbusTX + 13] << 8 | var[MODBUS_ADDR + Cnt_ModbusTX + 14];

		if (gUsartTx.Flag == 0xA5)
		{
			if (gUsartTx.Mode == 0x00) // Mode 0
			{
				if (gUsartTx.CMD == 0x01)
				{
					Send_ModbusBuf[0] = gUsartTx.Sla_Add;
					Send_ModbusBuf[1] = gUsartTx.CMD;
					Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
					Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
					Send_ModbusBuf[4] = gUsartTx.Len >> 8;
					Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
					LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
					tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
					pit_1ms_count3 = 0;
					timeout_flag = 1;
				}
				else if (gUsartTx.CMD == 0x02)
				{
					Send_ModbusBuf[0] = gUsartTx.Sla_Add;
					Send_ModbusBuf[1] = gUsartTx.CMD;
					Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
					Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
					Send_ModbusBuf[4] = gUsartTx.Len >> 8;
					Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
					LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
					tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
					pit_1ms_count3 = 0;
					timeout_flag = 1;
				}				
				else if (gUsartTx.CMD == 0x03)
				{
					Send_ModbusBuf[0] = gUsartTx.Sla_Add;
					Send_ModbusBuf[1] = gUsartTx.CMD;
					Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
					Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
					Send_ModbusBuf[4] = gUsartTx.Len >> 8;
					Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
					LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
					tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
					pit_1ms_count3 = 0;
					timeout_flag = 1;
				}
				else if (gUsartTx.CMD == 0x04)
				{
					Send_ModbusBuf[0] = gUsartTx.Sla_Add;
					Send_ModbusBuf[1] = gUsartTx.CMD;
					Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
					Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
					Send_ModbusBuf[4] = gUsartTx.Len >> 8;
					Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
					LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
					tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
					pit_1ms_count3 = 0;
					timeout_flag = 1;
				}				
				else if (gUsartTx.CMD == 0x05)
				{
					Send_ModbusBuf[0] = gUsartTx.Sla_Add;
					Send_ModbusBuf[1] = gUsartTx.CMD;
					Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
					Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
					
					num = gUsartTx.REG_Add%16;
					data_temp = (var[(gUsartTx.Var_Add) * 2]<<8 | var[(gUsartTx.Var_Add) * 2 + 1]);					
					Send_ModbusBuf[4] = (data_temp & (1<<num))?0xFF:0x00;
					Send_ModbusBuf[5] = 0x00;
					LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
					tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
					pit_1ms_count3 = 0;
					timeout_flag = 1;
				}				
				else if (gUsartTx.CMD == 0x06)
				{
					Send_ModbusBuf[0] = gUsartTx.Sla_Add;
					Send_ModbusBuf[1] = gUsartTx.CMD;
					Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
					Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
					Send_ModbusBuf[4] = var[(gUsartTx.Var_Add) * 2];
					Send_ModbusBuf[5] = var[(gUsartTx.Var_Add) * 2 + 1];
					LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
					tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
					pit_1ms_count3 = 0;
					timeout_flag = 1;
				}
				else if (gUsartTx.CMD == 0x0F)
				{
					Send_ModbusBuf[0] = gUsartTx.Sla_Add;
					Send_ModbusBuf[1] = gUsartTx.CMD;
					Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
					Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
					Send_ModbusBuf[4] = gUsartTx.Len >> 8;
					Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
					Send_ModbusBuf[6] = gUsartTx.Len%8?(gUsartTx.Len/8+1):(gUsartTx.Len/8);										
					
					num = (gUsartTx.REG_Add+gUsartTx.Len-1)/16 - gUsartTx.REG_Add/16 + 1;
					count = 0;
					for (i = 0; i < num; i++)
					{
						data_temp = (var[(gUsartTx.Var_Add+i) * 2]<<8 | var[(gUsartTx.Var_Add+i) * 2 + 1]);
						if(i==0)
							sum = ( (gUsartTx.Len>=count+16)?16:(gUsartTx.Len-count) )-gUsartTx.REG_Add%16;
						else						
							sum = (gUsartTx.Len>=count+16)?16:(gUsartTx.Len-count);
						
						for(j = 0; j <sum; j++)
						{
							if(i==0)
							{
								if( data_temp&(1<<(j+gUsartTx.REG_Add%16)) )
									byte_temp |= (1<<(cnt%8));
								else
									byte_temp &=~(1<<(cnt%8));								
							}
							else
							{
								if( data_temp&(1<<j) )
									byte_temp |= (1<<(cnt%8));
								else
									byte_temp &=~(1<<(cnt%8));								
							}
							
							cnt++;
							if((cnt!=0 && cnt%8==0) || cnt>=gUsartTx.Len )
							{
								if(cnt%8==0)	
									Send_ModbusBuf[7 + cnt/8-1] = byte_temp;
								else	
									Send_ModbusBuf[7 + cnt/8] = byte_temp;
								byte_temp = 0;
							}									
						}
						count+=sum;								
					}

					LT_SendData_CRC_NoFrame(Send_ModbusBuf, 7+Send_ModbusBuf[6]);
					tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
					pit_1ms_count3 = 0;
					timeout_flag = 1;
				}				
				else if (gUsartTx.CMD == 0x10)
				{
					Send_ModbusBuf[0] = gUsartTx.Sla_Add;
					Send_ModbusBuf[1] = gUsartTx.CMD;
					Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
					Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
					Send_ModbusBuf[4] = gUsartTx.Len >> 8;
					Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
					Send_ModbusBuf[6] = gUsartTx.Len*2;			

					for (i = 0; i < gUsartTx.Len; i++)			
					{
						Send_ModbusBuf[7 + 2 * i + 0] = var[(gUsartTx.Var_Add) * 2 + 2 * i + 0];
						Send_ModbusBuf[7 + 2 * i + 1] = var[(gUsartTx.Var_Add) * 2 + 2 * i + 1];
					}
					LT_SendData_CRC_NoFrame(Send_ModbusBuf, 7 + Send_ModbusBuf[6]);	
					tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
					pit_1ms_count3 = 0;
					timeout_flag = 1;
				}
			}
			else if (gUsartTx.Mode == 0x01) // Mode 1
			{
				if (gUsartTx.Opt_Add == (var[0x7000 * 2] << 8 | var[0x7000 * 2 + 1]))
				{
					if (gUsartTx.CMD == 0x01)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}
					else if (gUsartTx.CMD == 0x02)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}					
					else if (gUsartTx.CMD == 0x03)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}
					else if (gUsartTx.CMD == 0x04)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}					
					else if (gUsartTx.CMD == 0x05)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						
						num = gUsartTx.REG_Add%16;
						data_temp = (var[(gUsartTx.Var_Add) * 2]<<8 | var[(gUsartTx.Var_Add) * 2 + 1]);					
						Send_ModbusBuf[4] = (data_temp & (1<<num))?0xFF:0x00;
						Send_ModbusBuf[5] = 0x00;
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}					
					else if (gUsartTx.CMD == 0x06)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = var[(gUsartTx.Var_Add) * 2];
						Send_ModbusBuf[5] = var[(gUsartTx.Var_Add) * 2 + 1];
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}
					else if (gUsartTx.CMD == 0x0F)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						Send_ModbusBuf[6] = gUsartTx.Len%8?(gUsartTx.Len/8+1):(gUsartTx.Len/8);									
						
						num = (gUsartTx.REG_Add+gUsartTx.Len-1)/16 - gUsartTx.REG_Add/16 + 1;
						count = 0;
						for (i = 0; i < num; i++)
						{
							data_temp = (var[(gUsartTx.Var_Add+i) * 2]<<8 | var[(gUsartTx.Var_Add+i) * 2 + 1]);
							if(i==0)
								sum = ( (gUsartTx.Len>=count+16)?16:(gUsartTx.Len-count) )-gUsartTx.REG_Add%16;
							else						
								sum = (gUsartTx.Len>=count+16)?16:(gUsartTx.Len-count);
							
							for(j = 0; j <sum; j++)
							{
								if(i==0)
								{
									if( data_temp&(1<<(j+gUsartTx.REG_Add%16)) )
										byte_temp |= (1<<(cnt%8));
									else
										byte_temp &=~(1<<(cnt%8));								
								}
								else
								{
									if( data_temp&(1<<j) )
										byte_temp |= (1<<(cnt%8));
									else
										byte_temp &=~(1<<(cnt%8));								
								}
								
								cnt++;
								if((cnt!=0 && cnt%8==0) || cnt>=gUsartTx.Len )
								{
									if(cnt%8==0)	
										Send_ModbusBuf[7 + cnt/8-1] = byte_temp;
									else	
										Send_ModbusBuf[7 + cnt/8] = byte_temp;
									byte_temp = 0;
								}									
							}
							count+=sum;								
						}

						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 7+Send_ModbusBuf[6]);
						tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}					
					else if (gUsartTx.CMD == 0x10)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						Send_ModbusBuf[6] = gUsartTx.Len*2;			

						for (i = 0; i < gUsartTx.Len; i++)			
						{
							Send_ModbusBuf[7 + 2 * i + 0] = var[(gUsartTx.Var_Add) * 2 + 2 * i + 0];
							Send_ModbusBuf[7 + 2 * i + 1] = var[(gUsartTx.Var_Add) * 2 + 2 * i + 1];
						}
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 7 + Send_ModbusBuf[6]);	
						tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}
				}
				//				else	tx_repeat_cnt = 0;
			}
			else if (gUsartTx.Mode == 0x02) // Mode 2
			{
				if (var[2 * (gUsartTx.Opt_Add)] == 0x4C && var[2 * (gUsartTx.Opt_Add) + 1] == 0x54)
				{
					if (gUsartTx.CMD == 0x01)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}
					else if (gUsartTx.CMD == 0x02)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}										
					else if (gUsartTx.CMD == 0x03)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}
					else if (gUsartTx.CMD == 0x04)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}					
					
					else if (gUsartTx.CMD == 0x05)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						
						num = gUsartTx.REG_Add%16;
						data_temp = (var[(gUsartTx.Var_Add) * 2]<<8 | var[(gUsartTx.Var_Add) * 2 + 1]);					
						Send_ModbusBuf[4] = (data_temp & (1<<num))?0xFF:0x00;
						Send_ModbusBuf[5] = 0x00;
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}					
					else if (gUsartTx.CMD == 0x06)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = var[(gUsartTx.Var_Add) * 2];
						Send_ModbusBuf[5] = var[(gUsartTx.Var_Add) * 2 + 1];
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}
					else if (gUsartTx.CMD == 0x0F)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						Send_ModbusBuf[6] = gUsartTx.Len%8?(gUsartTx.Len/8+1):(gUsartTx.Len/8);										
						
						num = (gUsartTx.REG_Add+gUsartTx.Len-1)/16 - gUsartTx.REG_Add/16 + 1;
						count = 0;
						for (i = 0; i < num; i++)
						{
							data_temp = (var[(gUsartTx.Var_Add+i) * 2]<<8 | var[(gUsartTx.Var_Add+i) * 2 + 1]);
							if(i==0)
								sum = ( (gUsartTx.Len>=count+16)?16:(gUsartTx.Len-count) )-gUsartTx.REG_Add%16;
							else						
								sum = (gUsartTx.Len>=count+16)?16:(gUsartTx.Len-count);
							
							for(j = 0; j <sum; j++)
							{
								if(i==0)
								{
									if( data_temp&(1<<(j+gUsartTx.REG_Add%16)) )
										byte_temp |= (1<<(cnt%8));
									else
										byte_temp &=~(1<<(cnt%8));								
								}
								else
								{
									if( data_temp&(1<<j) )
										byte_temp |= (1<<(cnt%8));
									else
										byte_temp &=~(1<<(cnt%8));								
								}
								
								cnt++;
								if((cnt!=0 && cnt%8==0) || cnt>=gUsartTx.Len )
								{
									if(cnt%8==0)	
										Send_ModbusBuf[7 + cnt/8-1] = byte_temp;
									else	
										Send_ModbusBuf[7 + cnt/8] = byte_temp;
									byte_temp = 0;
								}									
							}
							count+=sum;								
						}

						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 7+Send_ModbusBuf[6]);
						tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}					
					else if (gUsartTx.CMD == 0x10)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						Send_ModbusBuf[6] = gUsartTx.Len*2;			

						for (i = 0; i < gUsartTx.Len; i++)			
						{
							Send_ModbusBuf[7 + 2 * i + 0] = var[(gUsartTx.Var_Add) * 2 + 2 * i + 0];
							Send_ModbusBuf[7 + 2 * i + 1] = var[(gUsartTx.Var_Add) * 2 + 2 * i + 1];
						}
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 7 + Send_ModbusBuf[6]);	
						tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}
				}
			}
			else if (gUsartTx.Mode == 0x03) // Customized Code_ Mode 3
			{
				if (Master_mode03_flag[gUsartTx.Opt_Add] == 1)
				{
					if (gUsartTx.CMD == 0x01)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}
					else if (gUsartTx.CMD == 0x02)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}										
					else if (gUsartTx.CMD == 0x03)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}
					else if (gUsartTx.CMD == 0x04)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}										
					else if (gUsartTx.CMD == 0x05)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						
						num = gUsartTx.REG_Add%16;
						data_temp = (var[(gUsartTx.Var_Add) * 2]<<8 | var[(gUsartTx.Var_Add) * 2 + 1]);					
						Send_ModbusBuf[4] = (data_temp & (1<<num))?0xFF:0x00;
						Send_ModbusBuf[5] = 0x00;
						var[(gUsartTx.Var_Add) * 2] = Master_mode03_Var[(gUsartTx.Opt_Add) * 2];
						var[(gUsartTx.Var_Add) * 2 + 1] = Master_mode03_Var[(gUsartTx.Opt_Add) * 2 + 1];						
						
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}					
					else if (gUsartTx.CMD == 0x06)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = Master_mode03_Var[(gUsartTx.Opt_Add) * 2];
						Send_ModbusBuf[5] = Master_mode03_Var[(gUsartTx.Opt_Add) * 2 + 1];
						var[(gUsartTx.Var_Add) * 2] = Master_mode03_Var[(gUsartTx.Opt_Add) * 2];
						var[(gUsartTx.Var_Add) * 2 + 1] = Master_mode03_Var[(gUsartTx.Opt_Add) * 2 + 1];

						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 6);
						tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}
					else if (gUsartTx.CMD == 0x0F)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						Send_ModbusBuf[6] = gUsartTx.Len%8?(gUsartTx.Len/8+1):(gUsartTx.Len/8);										
						
						num = (gUsartTx.REG_Add+gUsartTx.Len-1)/16 - gUsartTx.REG_Add/16 + 1;
						count = 0;
						for (i = 0; i < num; i++)
						{
							data_temp = (var[(gUsartTx.Var_Add+i) * 2]<<8 | var[(gUsartTx.Var_Add+i) * 2 + 1]);
							if(i==0)
								sum = ( (gUsartTx.Len>=count+16)?16:(gUsartTx.Len-count) )-gUsartTx.REG_Add%16;
							else						
								sum = (gUsartTx.Len>=count+16)?16:(gUsartTx.Len-count);
							
							for(j = 0; j <sum; j++)
							{
								if(i==0)
								{
									if( data_temp&(1<<(j+gUsartTx.REG_Add%16)) )
										byte_temp |= (1<<(cnt%8));
									else
										byte_temp &=~(1<<(cnt%8));								
								}
								else
								{
									if( data_temp&(1<<j) )
										byte_temp |= (1<<(cnt%8));
									else
										byte_temp &=~(1<<(cnt%8));								
								}
								
								cnt++;
								if((cnt!=0 && cnt%8==0) || cnt>=gUsartTx.Len )
								{
									if(cnt%8==0)	
										Send_ModbusBuf[7 + cnt/8-1] = byte_temp;
									else	
										Send_ModbusBuf[7 + cnt/8] = byte_temp;
									byte_temp = 0;
								}									
							}
							count+=sum;								
						}

						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 7+Send_ModbusBuf[6]);
						tx_en_flag = 0;	// The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}					
					else if (gUsartTx.CMD == 0x10)
					{
						Send_ModbusBuf[0] = gUsartTx.Sla_Add;
						Send_ModbusBuf[1] = gUsartTx.CMD;
						Send_ModbusBuf[2] = gUsartTx.REG_Add >> 8;
						Send_ModbusBuf[3] = gUsartTx.REG_Add & 0xFF;
						Send_ModbusBuf[4] = gUsartTx.Len >> 8;
						Send_ModbusBuf[5] = gUsartTx.Len & 0xFF;
						Send_ModbusBuf[6] = gUsartTx.Len*2;			

						for (i = 0; i < gUsartTx.Len; i++)			
						{
							Send_ModbusBuf[7 + 2 * i + 0] = var[(gUsartTx.Var_Add) * 2 + 2 * i + 0];
							Send_ModbusBuf[7 + 2 * i + 1] = var[(gUsartTx.Var_Add) * 2 + 2 * i + 1];
						}
						LT_SendData_CRC_NoFrame(Send_ModbusBuf, 7 + Send_ModbusBuf[6]);	
						tx_en_flag = 0; // The serial port will allow the sending flag bit to be cleared after sending
						pit_1ms_count3 = 0;
						timeout_flag = 1;
					}
				}
			}
		}

		Cnt_ModbusTX += 16;
	}
}
#endif

/*   Idle interrupt reception(SCI1)   */
/*For debug */
/*
#include "dmac_drv.h"

#define Uart_Channel	2        //DMA channel

DMA_CHANNEL_REG *sci_dma_channel[2][DMAC_CHNUM] = {{(DMA_CHANNEL_REG *)(DMA1_BASE_ADDR ),(DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0x58 ),
									(DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0xB0 ),(DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0x108)},
												   {(DMA_CHANNEL_REG *)(DMA2_BASE_ADDR ),(DMA_CHANNEL_REG *)(DMA2_BASE_ADDR + 0x58 ),
									(DMA_CHANNEL_REG *)(DMA2_BASE_ADDR + 0xB0 ),(DMA_CHANNEL_REG *)(DMA2_BASE_ADDR + 0x108)}};//global struct variable for for Channel registers

DMA_CONTROL_REG *sci_dma_control[2] = {(DMA_CONTROL_REG*)(DMA1_BASE_ADDR+0x2C0),
																				(DMA_CONTROL_REG*)(DMA2_BASE_ADDR+0x2C0)};//global struct variable for for DMAC registers

uint16_t dma_uartRX(UART_TypeDef *UARTx,uint8_t n, uint8_t *dest,uint32_t len)
{
	uint8_t dmaNum = 0;
	uint16_t ret = 0;
	uint16_t temp = 0;

	if(UARTx == SCI2)
	{
		dmaNum =1;
	}
	else					//SCI1 SCI3
	{
		dmaNum =0;
	}

	sci_dma_control[dmaNum]->DMA_CHEN &= (~(CHANNEL_WRITE_ENABLE(n)|CHANNEL_ENABLE(n)));////DMA disable

	sci_dma_control[dmaNum]->DMA_CONFIG = 0;
	sci_dma_channel[dmaNum][n]->DMA_SADDR = (uint32_t)&UARTx->SCIDRL;
	sci_dma_channel[dmaNum][n]->DMA_DADDR = (uint32_t)dest;
	sci_dma_channel[dmaNum][n]->DMA_CTRL = DIEC|SNOCHG|P2M_DMA;
	sci_dma_channel[dmaNum][n]->DMA_CTRL_HIGH =  len;
	sci_dma_control[dmaNum]->DMA_CONFIG = 1;

	sci_dma_channel[dmaNum][n]->DMA_CFG = (HS_SEL_SRC_HARD)|(HS_SEL_DST_SOFT)|(FIFO_EMPTY)|(CH_PRIOR2);

	if(UARTx == SCI1)
	{
		sci_dma_channel[dmaNum][n]->DMA_CFG_HIGH = (0xb<<7);
	}
	else if(UARTx == SCI2)
	{
		sci_dma_channel[dmaNum][n]->DMA_CFG_HIGH = (0x0<<7);
	}
	else
	{
		sci_dma_channel[dmaNum][n]->DMA_CFG_HIGH = (0xd<<7);
	}

	sci_dma_control[dmaNum]->DMA_CHEN |= (CHANNEL_WRITE_ENABLE(n)|CHANNEL_ENABLE(n));

	 return ret;
}

uint32_t DMA_uart_GetRecvLen(UART_TypeDef *UARTx,uint8_t n)
{
  uint8_t dmaNum = 0;

	if(UARTx == SCI2)
	{
		dmaNum =1;
	}
	else					//SCI1 SCI3
	{
		dmaNum =0;
	}

	return sci_dma_channel[dmaNum][n]->DMA_CTRL_HIGH;

}

void SCI1_DMA_Init(uint32_t pclk,uint32_t bound)
{
	uint32_t band_rate=0;

	NVIC_Init(0, 0, SCI1_IRQn, 2);

	SCI1->SCIBRDF=(((pclk*8/bound)+1)/2)&0x003f;
	band_rate =(pclk*4/bound)>>6;
	SCI1->SCIBDH  =(uint8_t)((band_rate>>8)&0x00ff);
	SCI1->SCIBDL  =(uint8_t)(band_rate&0x00ff);

	SCI1->SCICR1 &=~(1<<4);	//Data frame??1+8+1
	SCI1->SCICR1 &=~(1<<1);	//No verification
	SCI1->SCIRXTOCTR = 64;
	SCI1->SCIFCR = 0;
	SCI1->SCIFCR  |= (SCIFCR_RFEN|SCIFCR_RXFLSEL_1_8|SCIFCR_TXFLSEL_1_8);	//RX FIFOenable
	SCI1->SCIFCR2 |= (SCIFCR2_RXFCLR|SCIFCR2_RXFTOE);	//clean fifo , time over
	SCI1->SCICR1  |=  SCICR1_ILT_MASK; 					//Idle frame starts from end bit
	SCI1->SCICR2  |= (SCICR2_TE_MASK|SCICR2_RE_MASK|SCICR2_ILIE_MASK);	//Idle interrupt, receive, send enable

//	SCI3->SCICR2  |= SCICR2_RE_MASK;
	SCI1->SCIDCR |= SCIDCR_RXDMAE_MASK;	//Uart_DMA enable
	dma_uartRX(SCI1,Uart_Channel, Rx_Buffer_short,256);
}

//void SCI1_IRQHandler(void)
//{
//	uint16_t tmp,ret;
//
//	UART_TypeDef *UARTx=SCI1;
//
//	//UART_Enter idle interrupt
//	if(UARTx->SCISR1 & SCISR1_IDLE_MASK)      //The receiver is idle
//	{
//		while((UARTx->SCIFSR&SCIFSR_REMPTY_MASK) == 0) //FIFO not empty, Wait for the received data to be transmitted to DMA
//		{
//			if( UARTx->SCIFSR& (SCIFSR_RFULL_MASK|SCIFSR_RTOS_MASK) )  //FIFO overflow??receive time-out
//			{
//				break;
//			}
//		}
//
//		tmp = UARTx->SCIDRL;
//		UARTx->SCIFCR2 |= SCIFCR2_RXFCLR;

//		Rx_Count_short = DMA_uart_GetRecvLen(UARTx,Uart_Channel);
//		gUsartRx.Flag = 1;
//		printf("len:%d\r\n",Rx_Count_short);
//	//	SCI3->SCICR2  |= SCICR2_RE_MASK;
//	//	SCI3->SCIDCR |= SCIDCR_RXDMAE_MASK;	//Uart_DMA enable
//		dma_uartRX(UARTx,Uart_Channel, Rx_Buffer_short,256);
//	}

//}
*/


#if (UARTBUS_OPTION == 3 || UARTBUS_OPTION == 4)
uint8_t Hid_Rx_Buffer_long[Hid_SIZE] = {0};
uint16_t Hid_Rx_Count_long;
uint16_t Hid_Rx_Num = 0;
uint16_t Hid_Respond_Num = 0;
uint16_t Hid_Respond_Count = 0;
uint8_t Hid_Respond_C0_OK = 0;
uint8_t Hid_Respond_C1_OK = 0;
uint8_t Hid_Respond_Length = 0;
uint8_t Hid_Respond_Flag = 0;
uint16_t Hid_Respond_FlashLength = 0;

uint8_t Hid_receive_flag = 0;
uint16_t Hid_receive_num = 0;
uint16_t Hid_receive_count = 0;

void Hid_Main_From_UartRxBuff(void)
{
	uint16_t i = 0;
	if (Hid_Respond_Num < Hid_Rx_Num)
	{
		
		for (i = 0; i < (Hid_Rx_Num - Hid_Respond_Num); i++)
		{
			if (Hid_Respond_C1_OK)
			{
				gHidRx.Buf[gHidRx.Count++] = Hid_Rx_Buffer_long[(i + Hid_Respond_Num) % Hid_SIZE];
//				printf("gHidRx.Buf[%d]=%x \r\n",gHidRx.Count-1,gHidRx.Buf[gHidRx.Count-1]);
//				printf("gHidRx.Count=%x \r\n",gHidRx.Count);
				if (Hid_Respond_Flag == 1)
				{
					Hid_Respond_Flag = 0;
					Hid_Respond_Length = Hid_Rx_Buffer_long[(i + Hid_Respond_Num) % Hid_SIZE];
					//printf("Hid_Respond_Length=%x \r\n",Hid_Respond_Length);
					if (Hid_Respond_Length == 0)
					{
						Hid_Respond_Flag = 2;
					}
				}
				else if (Hid_Respond_Length == 0)
				{
					if (Hid_Respond_Flag == 2)
					{
						Hid_Respond_FlashLength = Hid_Rx_Buffer_long[(i + Hid_Respond_Num) % Hid_SIZE];
						Hid_Respond_Flag = 3;
					}
					else if(Hid_Respond_Flag == 3)
					{
						Hid_Respond_FlashLength = (Hid_Respond_FlashLength << 8) + Hid_Rx_Buffer_long[(i + Hid_Respond_Num) % Hid_SIZE] + 2;
						Hid_Respond_Flag = 0;

					}
				}
				
				if ((gHidRx.Count > Hid_Respond_Length && Hid_Respond_Length != 0) || (gHidRx.Count > Hid_Respond_FlashLength && Hid_Respond_Length == 0 && Hid_Respond_Flag == 0))
				{
					
					gHidRx.Flag = 1;
					Hid_Respond_C1_OK = 0;
					Hid_Respond_C0_OK = 0;
					break;
				}
			}
			if (Hid_Respond_C0_OK && !Hid_Respond_C1_OK && Hid_Rx_Buffer_long[(i + Hid_Respond_Count) % Hid_SIZE] != SCI_C1)
			{
				Hid_Respond_C0_OK = 0;
				gHidRx.Count = 0;
			}

			if (!Hid_Respond_C0_OK && Hid_Rx_Buffer_long[(i + Hid_Respond_Count) % Hid_SIZE] == SCI_C0)
			{
				Hid_Respond_C0_OK = 1;
				gHidRx.Count = 0;
			}
			if (!Hid_Respond_C1_OK && Hid_Rx_Buffer_long[(i + Hid_Respond_Count) % Hid_SIZE] == SCI_C1 && Hid_Respond_C0_OK)
			{
				Hid_Respond_C1_OK = 1;
				Hid_Respond_Flag = 1;
				gHidRx.Count = 0;
			}


		}
		Hid_Respond_Count = (i + Hid_Respond_Count) % Hid_SIZE;
		Hid_Respond_Num += i;
	}
	else if (Hid_Respond_Num == Hid_Rx_Num && Hid_Rx_Num != 0)
	{
		Hid_Respond_Num = 0;
		Hid_Rx_Num = 0;
		Hid_Respond_Count = 0;
		Hid_Rx_Count_long = 0;
	}
}
#endif
