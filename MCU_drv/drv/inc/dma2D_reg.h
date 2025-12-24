// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : dma2D_reg.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef __DMA2D_REG_H__
#define __DMA2D_REG_H__

//------------------------------------------------------------------
//                        Headers
//------------------------------------------------------------------
#include "type.h"
#include "memmap.h"
//------------------------------------------------------------------

typedef struct
{
  __IO UINT32 CR;            /*!< DMA2D Control Register,                         Address offset: 0x00 */
  __IO UINT32 ISR;           /*!< DMA2D Interrupt Status Register,                Address offset: 0x04 */
  __IO UINT32 IFCR;          /*!< DMA2D Interrupt Flag Clear Register,            Address offset: 0x08 */
  __IO UINT32 FGMAR;         /*!< DMA2D Foreground Memory Address Register,       Address offset: 0x0C */
  __IO UINT32 FGOR;          /*!< DMA2D Foreground Offset Register,               Address offset: 0x10 */
  __IO UINT32 BGMAR;         /*!< DMA2D Background Memory Address Register,       Address offset: 0x14 */
  __IO UINT32 BGOR;          /*!< DMA2D Background Offset Register,               Address offset: 0x18 */
  __IO UINT32 FGPFCCR;       /*!< DMA2D Foreground PFC Control Register,          Address offset: 0x1C */
  __IO UINT32 FGCOLR;        /*!< DMA2D Foreground Color Register,                Address offset: 0x20 */
  __IO UINT32 BGPFCCR;       /*!< DMA2D Background PFC Control Register,          Address offset: 0x24 */
  __IO UINT32 BGCOLR;        /*!< DMA2D Background Color Register,                Address offset: 0x28 */
  __IO UINT32 FGCMAR;        /*!< DMA2D Foreground CLUT Memory Address Register,  Address offset: 0x2C */
  __IO UINT32 BGCMAR;        /*!< DMA2D Background CLUT Memory Address Register,  Address offset: 0x30 */
  __IO UINT32 OPFCCR;        /*!< DMA2D Output PFC Control Register,              Address offset: 0x34 */
  __IO UINT32 OCOLR;         /*!< DMA2D Output Color Register,                    Address offset: 0x38 */
  __IO UINT32 OMAR;          /*!< DMA2D Output Memory Address Register,           Address offset: 0x3C */
  __IO UINT32 OOR;           /*!< DMA2D Output Offset Register,                   Address offset: 0x40 */
  __IO UINT32 NLR;           /*!< DMA2D Number of Line Register,                  Address offset: 0x44 */
  __IO UINT32 LWR;           /*!< DMA2D Line Watermark Register,                  Address offset: 0x48 */
  __IO UINT32 AMTCR;         /*!< DMA2D AHB Master Timer Configuration Register,  Address offset: 0x4C */
       UINT32 RESERVED[236]; /*!< Reserved, 0x50-0x3FF */
  __IO UINT32 FGCLUT[256];   /*!< DMA2D Foreground CLUT,                          Address offset:400-7FF */
  __IO UINT32 BGCLUT[256];   /*!< DMA2D Background CLUT,                          Address offset:800-BFF */
} DMA2D_TypeDef;

#define DMA2D               ((DMA2D_TypeDef *)DMA2D_BASE_ADDR)


#endif /* __DMA2D_REG_H__ */
