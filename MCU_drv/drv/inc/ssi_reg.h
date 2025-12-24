// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : ssi_reg.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __SSI_REG_H__
#define __SSI_REG_H__

#include "type.h"

typedef struct
{
__IO unsigned int SSI_CTRLR0;    	//0x00
__IO unsigned int SSI_CTRLR1;    	//0x04
__IO unsigned int SSI_SSIENR;   	//0x08
__IO unsigned int SSI_MWCR;      	//0x0c
__IO unsigned int SSI_SER;       	//0x10
__IO unsigned int SSI_BAUDR;     	//0x14
__IO unsigned int SSI_TXFTLR;    	//0x18
__IO unsigned int SSI_RXFTLR;    	//0x1c
__IO unsigned int SSI_TXFLR;     	//0x20
__IO unsigned int SSI_RXFLR;     	//0x24
__IO unsigned int SSI_SR;        	//0x28
__IO unsigned int SSI_IMR;        	//0x2c
__IO unsigned int SSI_ISR;        	//0x30
__IO unsigned int SSI_RISR;       	//0x34
__IO unsigned int SSI_TXOICR;          	//0x38
__IO unsigned int SSI_RXOICR;          	//0x3c
__IO unsigned int SSI_RXUICR;          	//0x40
__IO unsigned int SSI_MSTICR;          	//0x44
__IO unsigned int SSI_ICR;             	//0x48
__IO unsigned int SSI_DMACR;           	//0x4c
__IO unsigned int SSI_DMATDLR;         	//0x50
__IO unsigned int SSI_DMARDLR;         	//0x54
__IO unsigned int SSI_IDR;             	//0x58
__IO unsigned int SSI_SSIC_VERSION_ID; 	//0x5c
__IO unsigned int SSI_DR;              	//0x60
__IO unsigned int RESERVERED[35];	
__IO unsigned int SSI_RX_SAMPLE_DELAY; 	//0xf0
__IO unsigned int SSI_SPI_CTRLR0;      	//0xf4
__IO unsigned int SSI_DDR_DRIVE_EDGE;  	//0xf8
__IO unsigned int SSI_XIP_MODE_BITS;   	//0xfc 
__IO unsigned int XIP_INCR_INST;        //0x100
__IO unsigned int XIP_WRAP_INST;        //0x104
__IO unsigned int XIP_CTRL;             //0x108
__IO unsigned int XIP_SER;              //0x10C
__IO unsigned int XRXOICR;              //0x110	
__IO unsigned int XIP_CNT_TIME_OUT;     //0x114	

}SSI_TypeDef;

#define SSI1  (SSI_TypeDef *)(0x60000000)
#define SSI2  (SSI_TypeDef *)(0x4004a000)

#define SR_TXE				0x20
#define SR_RFF        0x10
#define SR_RFNE       0x08
#define SR_TFE				0x04
#define SR_TFNF       0x02
#define SR_BUSY       0x01

#define DMACR_TDMAE   0x02
#define DMACR_RDMAE   0x01

//SSI_CTRLR0
#define CTRLR0_DFS_VALUE(x)				(x<<0)
#define CTRLR0_FRF_MOT						(0x00<<6)	
#define CTRLR0_TMOD_VALUE(x)			(x<<10)
#define CTRLR0_TMOD_TX_AND_RX			(0x00<<10)
#define CTRLR0_TMOD_TX_ONLY				(0x01<<10)
#define CTRLR0_TMOD_RX_ONLY				(0x02<<10)
#define CTRLR0_TMOD_EEPROM_READ		(0x03<<10)
#define CTRLR0_CFS_VALUE(x)				(x<<16)
#define CTRLR0_SCPH_MASK					(1<<8)
#define CTRLR0_SCPOL_MASK					(1<<9)
#define CTRLR0_SRL_MASK						(1<<13)
#define CTRLR0_SSTE_MASK					(1<<14)
#define CTRLR0_SPI_FRF_STD				(0x00<<22)
#define CTRLR0_SPI_FRF_DUAL				(0x01<<22)
#define CTRLR0_SPI_FRF_QUAD				(0x02<<22)

//SPI_CTRLR0
#define CTRLR0_TRANS_TYPE_TT0				(0x00<<0)		//Instruction and Address STD mode
#define CTRLR0_TRANS_TYPE_TT1				(0x01<<0)		//Instruction STD mode and Address CTRLR0.SPI_FRF mode
#define CTRLR0_TRANS_TYPE_TT2				(0x02<<0)		//Instruction and Address CTRLR0.SPI_FRF mode
#define CTRLR0_ADDR_L_VALUE(x)			(x<<2)
#define CTRLR0_INST_L_VALUE(x)			(x<<8)
#define CTRLR0_WAIT_CYCLES_VALUE(x)	(x<<11)
#define CTRLR0_CLK_STRETCH_EN_MASK	(1<<30)

#endif /* __SSI_REG_H__ */
