// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : dmac_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef __DMAC_DRV_H__
#define __DMAC_DRV_H__

#include "type.h"
#include "dmac_reg.h"

#define DMAC_HWNUM				(1)
#define DMAC_CHNUM				(4)

#define DMACCH0					(0)
#define DMACCH1					(1)
#define DMACCH2					(2)
#define DMACCH3					(3)

enum
{
	WIDTH_BYTE = 0,
	WIDTH_HALFWORD,
	WIDTH_WORD,
};

typedef enum
{
	dma_channel_1 = 0,
	dma_channel_2,
	dma_channel_3,
	dma_channel_4 ,
}DMA_CHANNEL;

#define CHANNEL_UMASK(n)         (((1<<n)<<8) | (1<<n))
#define CHANNEL_WRITE_ENABLE(n)  ((1<<n)<<8)
#define CHANNEL_ENABLE(n)        (1<<n)
#define CHANNEL_STAT(n)          (1<<n)

extern DMA_CHANNEL_REG *m_dma_channel[];//global struct variable for for Channel registers
extern DMA_CONTROL_REG *m_dma_control;//global struct variable for for DMAC registers

/*******************************************************************************
* Function Name  : dma_m2mtran
* Description    : dma传送函数
* Input          : - channel: 通道号
*				   - width: 数据宽度
*				   - src ： 起始地址
*				   - dest ： 目的地址
*				   - length ： 传输数据长度
*
* Output         : None
* Return         : FALSE - 失败
*                  TRUE  - 成功
******************************************************************************/
extern bool dma_m2mtran(UINT8 channel,UINT8 width,UINT32 src, UINT32 dest,UINT32 length);

/*******************************************************************************
* Function Name  : dma_spitran
* Description    : spi dma传送函数
* Input          : - spiid: SPI ID号
*				   - psend: 发送数据地址
*				   - precv ：接收数据地址
*				   - length ： 传输数据长度
*				   - binten : 是否开启中断模式
*
* Output         : None
* Return         : None
******************************************************************************/
extern void dma_spitran(UINT8 spiid,UINT8* psend,UINT8* precv,UINT32 length,BOOL binten);

/*******************************************************************************
* Function Name  : dma_spi_LLIReceive
* Description    : spi dma链表接收函数
* Input          : -n     ：dma通道号
* 				   - spiid: SPI ID号
*				   - precv ：接收数据地址
*				   - length ： 传输数据长度
*
* Output         : None
* Return         : None
******************************************************************************/
extern void dma_spi_LLIReceive(DMA_CHANNEL n,UINT8 spiid, UINT8 *dest,UINT32 length);

/*******************************************************************************
* Function Name  : dma_channle_stop
* Description    : 停止DMA通道函数
* Input          : -n     ：dma通道号
*
* Output         : None
* Return         : None
******************************************************************************/
extern void dma_channle_stop(DMA_CHANNEL n);

/*******************************************************************************
* Function Name  : dma_getLength
* Description    : 获取DMA链表缓冲数据长度
* Input          : -n     ：dma通道号
*
* Output         : None
* Return         : 当前缓冲数据长度
******************************************************************************/
extern UINT32  dma_getLength(DMA_CHANNEL n);

 /*******************************************************************************
 * Function Name  : DMA_Init
 * Description    : DMA初始化
 * Input          : None
 *
 * Output         : None
 * Return         : None
 ******************************************************************************/
extern void DMA_Init(UINT32 dma_base_addr);

/*******************************************************************************
* Function Name  : DMA_REG_Init
* Description    : DMA寄存器初始化
* Input          : - dmac_base_addr: DMA基地址
*
* Output         : None
* Return         : None
******************************************************************************/
extern void DMA_REG_Init(UINT32 dmac_base_addr);

extern void dma_lli_reg_init(UINT8 n,DMA_LLI *dma_lli);
extern void DMA_DAC_Tran(UINT8 channel, UINT32 src,UINT32 length);
extern void DMA_dis(UINT8 n);

#endif /* __DMAC_DRV_H__ */
