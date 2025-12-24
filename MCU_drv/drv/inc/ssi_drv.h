// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : ssi_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __SSI_DRV_H__
#define __SSI_DRV_H__

#include "memmap.h"
#include "ssi_reg.h"
//#include "test_demo.h"

#ifdef SSI_DEMO
#define SSI_DMA_INT
#endif

#define  QUAD_BAUDR (0x02)      /*必须为偶数，且最小值为2*/

#define rSWAPCR		(*(unsigned char *)0x4000001c)

//w25q128 CMD
#define DUMMY_BYTE				0xa5
#define READ_ID_CMD				0x90  
#define WRITE_EN_CMD			0x06
#define SECT_ERASE_CMD		0x20
#define GET_SAT1_CMD			0x05
#define READ_CMD					0x03
#define PAGE_PROG_CMD			0x02
#define GET_SAT2_CMD			0x35
#define PROG_STA2_CMD			0x31
#define DUAL_READ_CMD			0x3b
#define QUAD_PROG_CMD			0x32
#define QUAD_READ_CMD			0x6b
#define QPI_READ_CMD			0x0b
#define QPI_ENTER_CMD			0x38
#define QPI_EXIT_CMD			0xFF
#define SET_READ_PARA_CMD	0xc0

typedef enum
{
	CMD_READ = 0,
	CMD_WRITE,
	DATA_READ,
	DATA_WRITE,
}QPI_OPT_MODE;

extern volatile int dma_int;

extern void SSI_DMA_ISR(void);
extern void SSI_Standard_Init(SSI_TypeDef *SSIx);
extern void SSI_DUAL_Init(SSI_TypeDef *SSIx, UINT32 data_len, UINT8 waitCycles);
extern void SSI_QUAD_Init(SSI_TypeDef *SSIx, int read, int num, int waitCycles);
extern void SSI_QPI_Init(SSI_TypeDef *SSIx, QPI_OPT_MODE mode, UINT32 data_len, UINT8 waitCycles);
extern void SSI_EFlash_Set_Read_Para(SSI_TypeDef *SSIx);
extern void SSI_EFlash_QPI_Enter(SSI_TypeDef *SSIx);
extern void SSI_EFlash_QPI_EXIT(SSI_TypeDef *SSIx);


extern UINT16 Read_ID(SSI_TypeDef *SSIx);
extern void SSI_EFlash_Read(SSI_TypeDef *SSIx, unsigned int addr, unsigned char *buf, int num);
extern unsigned char SSI_EFlash_Get_Status1(SSI_TypeDef *SSIx);
extern unsigned char SSI_EFlash_QPI_Get_Status1(SSI_TypeDef *SSIx);
extern void SSI_EFlash_Write_Enable(SSI_TypeDef *SSIx);
extern void SSI_EFlash_QPI_Write_Enable(SSI_TypeDef *SSIx);
extern void SSI_EFlash_Sector_Erase(SSI_TypeDef *SSIx, unsigned int addr);
extern void SSI_EFlash_QPI_Sector_Erase(SSI_TypeDef *SSIx, unsigned int addr);
extern void SSI_EFlash_Program(SSI_TypeDef *SSIx, unsigned int addr, unsigned char *buf, int num);
extern unsigned char SSI_EFlash_Get_Status2(SSI_TypeDef *SSIx);
extern void SSI_EFlash_Prog_Status2(SSI_TypeDef *SSIx, unsigned char val);
extern void SSI_EFlash_QUAD_Program(SSI_TypeDef *SSIx, unsigned int addr, unsigned char *buf, int num);
extern void SSI_EFlash_QUAD_Read(SSI_TypeDef *SSIx, unsigned int addr, unsigned char *buf, int num);
extern void SSI_EFlash_QPI_Program(SSI_TypeDef *SSIx, unsigned int addr, unsigned char *buf, int num);
extern void SSI_EFlash_QPI_Read(SSI_TypeDef *SSIx, unsigned int addr, unsigned char *buf, int num);
extern void SSI_Std_Dma_Trig(SSI_TypeDef *SSIx, unsigned cmd, unsigned int addr, int dmaConf);
extern void SSI_EFlash_DUAL_Read(SSI_TypeDef *SSIx, unsigned int addr, unsigned char *buf, int num);
/*******************************************************************************
* Function Name  : void SSI_STD_DMA_Send
* Description    : spi dma传送函数
* Input          : - SPIx: SPI 基地址
*                - dma_ch: DMA channel
*				          - psend: 发送数据地址
*                  _ addr: SPI flash地址
*				         - length： 传输数据长度
*				         - binten: 是否开启中断模式
*
* Output         : None
* Return         : None
******************************************************************************/
extern void SSI_STD_DMA_Send(SSI_TypeDef *SSIx,int dma_ch, UINT8* psend, UINT32 addr, UINT32 length, BOOL binten);
/*******************************************************************************
* Function Name  : SSI_Std_DMA_Read
* Description    : spi dma接收函数
* Input          : - SPIx: SPI 基地址
*                - dma_ch: DMA channel
*				          - pread: 发送数据地址
*                  - addr: spi flash地址
*				        - length ： 传输数据长度
*				        - binten : 是否开启中断模式
*
* Output         : None
* Return         : None
******************************************************************************/
extern void SSI_STD_DMA_Read(SSI_TypeDef *SSIx,int dma_ch, UINT8* pread, UINT32 addr, UINT32 length, BOOL binten);

/*******************************************************************************
* Function Name  : SSI_DUAL_DMA_Read
* Description    : spi dma接收函数
* Input          : - SPIx: SPI 基地址
*                - dma_ch: DMA channel
*				          - pread: 发送数据地址
*                  - addr: spi flash地址
*				        - length ： 传输数据长度
*				        - binten : 是否开启中断模式
*
* Output         : None
* Return         : None
******************************************************************************/
extern void SSI_DUAL_DMA_Read(SSI_TypeDef *SSIx,int dma_ch, UINT8* pread, UINT32 addr, UINT32 length, BOOL binten);

#endif /* __SSI_DRV_H__ */

