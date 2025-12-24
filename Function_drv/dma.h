/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     dma.h
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief
 *************************************************************************/

#ifndef INC_DMA_H_
#define INC_DMA_H_

#include "type.h"

void SPI2_DMA_Tran(UINT8 *Tx_Addr, UINT8 *Rx_Addr, UINT32 length);
void SPI2_DMA_Wait(void);

#endif /* INC_DMA_H_ */
