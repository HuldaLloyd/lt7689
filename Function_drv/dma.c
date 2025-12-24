/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     dma.c
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-2-24
 * @brief
 *************************************************************************/

#include "dma.h"
#include "dmac_drv.h"
#include "spi_drv.h"

void SPI2_DMA_Tran(UINT8 *Tx_Addr, UINT8 *Rx_Addr, UINT32 length)
{
	DMA_Init(DMA1_BASE_ADDR);
	SPI2->SPIDMACR |= 0x03;		// Enable DMA of SPI TX RX

	m_dma_control->DMA_CONFIG = 1;
	// Tx configuration
	m_dma_channel[0]->DMA_SADDR = (UINT32)Tx_Addr;
	m_dma_channel[0]->DMA_DADDR = (UINT32)&SPI2->SPIDR;
	m_dma_channel[0]->DMA_CTRL_HIGH = length;
	m_dma_channel[0]->DMA_CTRL = DNOCHG | SIEC | M2P_DMA;
	m_dma_channel[0]->DMA_CFG = (HS_SEL_SRC_SOFT);
	m_dma_channel[0]->DMA_CFG_HIGH = DST_PER_SPI_TX(1) | SRC_PER_SPI_TX(1);

	// Rx configuration
	m_dma_channel[1]->DMA_SADDR = (UINT32)&SPI2->SPIDR;
	m_dma_channel[1]->DMA_DADDR = (UINT32)Rx_Addr;
	m_dma_channel[1]->DMA_CTRL_HIGH = length;
	m_dma_channel[1]->DMA_CTRL = SNOCHG | DIEC | P2M_DMA;
	m_dma_channel[1]->DMA_CFG = (HS_SEL_DST_SOFT);
	m_dma_channel[1]->DMA_CFG_HIGH = SRC_PER_SPI_RX(4) | DST_PER_SPI_RX(4);

	m_dma_control->DMA_MASKTFR = CHANNEL_UMASK(0) | CHANNEL_UMASK(1);
	m_dma_control->DMA_CHEN = CHANNEL_WRITE_ENABLE(0) | CHANNEL_ENABLE(0) | CHANNEL_WRITE_ENABLE(1) | CHANNEL_ENABLE(1);
}

void SPI2_DMA_Wait(void)
{
	while ((m_dma_control->DMA_RAWTFR & (CHANNEL_STAT(0) | CHANNEL_STAT(1))) != (CHANNEL_STAT(0) | CHANNEL_STAT(1)));

	m_dma_control->DMA_CLRTFR = (CHANNEL_STAT(0) | CHANNEL_STAT(1));

	m_dma_control->DMA_CHEN = 0;
	m_dma_control->DMA_CONFIG = 0;
	SPI2->SPIDMACR &= ~0x03;	// Disable DMA of SPI TX RX
}
