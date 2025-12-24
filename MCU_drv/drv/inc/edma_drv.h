// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : edma_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef EDMA_DRV_H_
#define EDMA_DRV_H_

#include "type.h"
#include "edma_reg.h"
#include "memmap.h"

////////////////////////////////////////////////////////////////////////////////
//EDMAC PN TABLE
#define EDMA_SRAM   (0x00000000)  //SRAM
#define EDMA_SPI1		(0x00000000)	//SPI1
#define EDMA_SPI2		(0x00000100)	//SPI2
#define EDMA_USI1		(0x00000200)	//USI1
#define EDMA_USI2		(0x00000300)	//USI2
#define EDMA_SCI1		(0x00000800)  //SCI1
#define EDMA_SCI3	  (0x00000b00)	//SCI3
#define EDMA_SCI2	  (0x00000d00)	//SCI2
#define EDMA_SPI3	  (0x00000e00)	//SPI3

#define IS_EDMA_EDMA_PN(PN) (((PN) == EDMA_SRAM) || \
		                     ((PN) == EDMA_SPI1) || \
		                     ((PN) == EDMA_SPI2) || \
		                     ((PN) == EDMA_USI1) || \
		                     ((PN) == EDMA_USI2) || \
		                     ((PN) == EDMA_SCI3) || \
		                     ((PN) == EDMA_SCI1) || \
		                     ((PN) == EDMA_SCI2) || \
                             ((PN) == EDMA_SPI3))

/////////////////////////////////////////////////////////////////////////////////

//EDMAC Control Register of Channel  (EDMACCR)//offset at 0x00000000
#define EDMAC_TTYPE0	(0x00000000)//sram to sram
#define EDMAC_TTYPE1	(0x00000020)//peripheral to sram
#define EDMAC_TTYPE2	(0x00000040)//sram to peripheral
#define EDMAC_TTYPE3	(0x00000060)//sram to peripheral then peripheral to sram

#define IS_EDMA_TTYPE(TTYPE) (((TTYPE) == EDMAC_TTYPE0) || \
		                      ((TTYPE) == EDMAC_TTYPE1) || \
		                      ((TTYPE) == EDMAC_TTYPE2) || \
                              ((TTYPE) == EDMAC_TTYPE3))


//#define EDMA_INT

#define EDMA_CH0		((EDMA_TypeDef *)(EDMAC_BASE_ADDR))
#define EDMA_CH1		((EDMA_TypeDef *)(EDMAC_BASE_ADDR+0x40))

#define IS_EDMA_ALL_PERIPH(PERIPH) (((PERIPH) == EDMA_CH0) || \
                                    ((PERIPH) == EDMA_CH1))


/*******************************************************************************
* Function Name  : EDMA_CHx
* Description    : EDMA 传输处理
* Input          :  - EDMA_CHx： where x can be 0 to 1 to select the EDMA peripheral；
*                   - sendAddr：EDMAC Read Buffer Address
*                   - recvAddr：EDMAC Write Buffer Address
*                   - ttpye:    Channel Transfer Type 取值：EDMAC_TTYPE0~EDMAC_TTYPE3
*                   - pn：    Channel Periperal Number
*                   - len： Channel Minor Transfer Data Sum(in byte)
*
* Output         : None
* Return         : None
******************************************************************************/
extern void EDMA_CHx(EDMA_TypeDef *EDMA0_CHx, UINT32 readAddr,  UINT32 writeAddr, UINT32 ttpye, UINT32 pn, UINT32 len);

/*******************************************************************************
* Function Name  : EDMA_CHx_SAMEDATA
* Description    : EDMA 传输固定数值
* Input          :  - EDMA_CHx： where x can be 0 to 1 to select the EDMA peripheral；
*                   - sendAddr：EDMAC Read Buffer Address
*                   - recvAddr：EDMAC Write Buffer Address
*                   - ttpye:    Channel Transfer Type 取值：EDMAC_TTYPE0~EDMAC_TTYPE3
*                   - pn：    Channel Periperal Number
*                   - len： Channel Minor Transfer Data Sum(in byte)
*
* Output         : None
* Return         : None
******************************************************************************/
void EDMA_CHx_SAMEDATA(EDMA_TypeDef *EDMA_CHx, UINT32 sendAddr,  UINT32 recvAddr, UINT32 ttpye, UINT32 pn, UINT32 len);

/*******************************************************************************
* Function Name  : EDMA_Init
* Description    : EDMA初始化
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void EDMA_Init(void);



#endif /* EDMA_DRV_H_ */
