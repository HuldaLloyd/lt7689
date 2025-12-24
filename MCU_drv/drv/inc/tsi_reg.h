#ifndef _TSI_REG_H_
#define _TSI_REG_H_

#include "type.h"

#define TSI_BASE      0x40023000
#define TSI_ISR       *(volatile UINT32*) (TSI_BASE + 0x00)
#define TSI_IER       *(volatile UINT32*) (TSI_BASE + 0x04)
#define TSI_CR        *(volatile UINT32*) (TSI_BASE + 0x08)
#define TSI_TSTR      *(volatile UINT32*) (TSI_BASE + 0x0c)
#define TSI_AWDCR     *(volatile UINT32*) (TSI_BASE + 0x10)
#define TSI_AWDTR     *(volatile UINT32*) (TSI_BASE + 0x14)
#define TSI_CHCR      *(volatile UINT32*) (TSI_BASE + 0x18)
#define TSI_CH0DR     *(volatile UINT32*) (TSI_BASE + 0x1c)
#define TSI_CH1DR     *(volatile UINT32*) (TSI_BASE + 0x20)
#define TSI_CH2DR     *(volatile UINT32*) (TSI_BASE + 0x24)
#define TSI_CH3DR     *(volatile UINT32*) (TSI_BASE + 0x28)
#define TSI_CH4DR     *(volatile UINT32*) (TSI_BASE + 0x2c)
#define TSI_CH5DR     *(volatile UINT32*) (TSI_BASE + 0x30)
#define TSI_CH6DR     *(volatile UINT32*) (TSI_BASE + 0x34)
#define TSI_CH7DR     *(volatile UINT32*) (TSI_BASE + 0x38)
#define TSI_CH8DR     *(volatile UINT32*) (TSI_BASE + 0x3c)
#define TSI_CH9DR     *(volatile UINT32*) (TSI_BASE + 0x40)
#define TSI_CHaDR     *(volatile UINT32*) (TSI_BASE + 0x44)
#define TSI_CHbDR     *(volatile UINT32*) (TSI_BASE + 0x48)
#define TSI_CHcDR     *(volatile UINT32*) (TSI_BASE + 0x4c)
#define TSI_CHdDR     *(volatile UINT32*) (TSI_BASE + 0x50)
#define TSI_CHeDR     *(volatile UINT32*) (TSI_BASE + 0x54)
#define TSI_CHfDR     *(volatile UINT32*) (TSI_BASE + 0x58)
#define TSI_FIFO      *(volatile UINT32*) (TSI_BASE + 0x60)
#define TSI_BASE03    *(volatile UINT32*) (TSI_BASE + 0x64)
#define TSI_BASE47    *(volatile UINT32*) (TSI_BASE + 0x68)
#define TSI_BASE8b    *(volatile UINT32*) (TSI_BASE + 0x6c)
#define TSI_BASEcf    *(volatile UINT32*) (TSI_BASE + 0x70)
#define TSI_CHEN      *(volatile UINT32*) (TSI_BASE + 0x74)
#define TSI_DATA      *(volatile UINT32*) (TSI_BASE + 0xc0)
	
#define TSI_DELTA03      *(volatile UINT32*) (TSI_BASE + 0x80)
#define TSI_DELTA47      *(volatile UINT32*) (TSI_BASE + 0x84)
#define TSI_DELTA8b      *(volatile UINT32*) (TSI_BASE + 0x88)
#define TSI_DELTAcf      *(volatile UINT32*) (TSI_BASE + 0x8c)
	
#define TSI_CID_COARSE03      *(volatile UINT32*) (TSI_BASE + 0x90)
#define TSI_CID_COARSE47      *(volatile UINT32*) (TSI_BASE + 0x94)
#define TSI_CID_COARSE8b      *(volatile UINT32*) (TSI_BASE + 0x98)
#define TSI_CID_COARSEcf      *(volatile UINT32*) (TSI_BASE + 0x9c)
//typedef struct
//{
//	__IO unsigned int TSI_ISR;   //0x00
//	__IO unsigned int TSI_IER;   //0x04
//	__IO unsigned int TSI_CR;    //0x08
//	__IO unsigned int TSI_TSTR;  //0x0c
//	__IO unsigned int TSI_AWDCR; //0x10
//	__IO unsigned int TSI_AWDTR; //0x14
//	__IO unsigned int TSI_CHCR;  //0x18
//	__IO unsigned int TSI_CH0DR; //0x1c
//	__IO unsigned int TSI_CH1DR; //0x20
//	__IO unsigned int TSI_CH2DR; //0x24
//	__IO unsigned int TSI_CH3DR; //0x28
//	__IO unsigned int TSI_CH4DR; //0x2c
//	__IO unsigned int TSI_CH5DR; //0x30	
//	__IO unsigned int TSI_CH6DR; //0x34
//	__IO unsigned int TSI_CH7DR; //0x38
//	__IO unsigned int TSI_CH8DR; //0x3c
//	__IO unsigned int TSI_CH9DR; //0x40
//	__IO unsigned int TSI_CHaDR; //0x44
//	__IO unsigned int TSI_CHbDR; //0x48
//	__IO unsigned int TSI_CHcDR; //0x4c
//	__IO unsigned int TSI_CHdDR; //0x50	
//	__IO unsigned int TSI_CHeDR; //0x54
//	__IO unsigned int TSI_CHfDR; //0x58
//	__IO unsigned int TSI_RESERVED; //0x5C
//	__IO unsigned int TSI_FIFO;  //0x60
//	__IO unsigned int TSI_BASE03; //0x64
//	__IO unsigned int TSI_BASE47; //0x68
//	__IO unsigned int TSI_BASE8b;  //0x6c
//	__IO unsigned int TSI_BASEcf;  //0x70
//	__IO unsigned int TSI_CHEN;    //0x74	
//	__IO unsigned int TSI_DATA;    //0xc0

//}TSI_TypeDef;


//#define AVRG_MODE (1<<24)
//#define SCAN_MODE (1<<7)

//#define DELTA_SHIFT (16)
//#define PRESS_BIT  (1<<9)
//#define LIFT_BIT   (1<<8)
//#define AWDEN_BIT  (1<<7) //enable watchdog
//#define AVRG_BIT   (1<<6) //average mode
//#define AWDMOD_BIT (1<<5) //1, change bigger than value; 0, change over a high and low threshold

//#define TSI_EN_BIT (1<<0)

//#define SCAN_FINISH_BIT (1<<17)
//#define AWD_BIT (1<<7)

//#define SCAN_FIN_IE_BIT (1<<30)
//#define AWD_FLAG_IE_BIT (1<<29)

//#define COARSE_MOD_BIT  (1<<30)

#endif /**/
