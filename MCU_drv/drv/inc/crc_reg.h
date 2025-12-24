// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : crc_reg.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef CRC_REG_H_
#define CRC_REG_H_

#include "type.h"

typedef struct
{
	__IO unsigned int CRC_CR;//0x00  ----CRC Control Register
	__IO unsigned int CRC_RR;//0x04 ----CRC Result Register
	__IO unsigned int CRC_IDR;//0x08-----CRC initial Data register
	__IO unsigned int CRC_DR;//0x0c-----CRC Data In Register
	__IO unsigned int CRC_EDMADR;//0x10-----DMAC DATA In Register
	__IO unsigned int CRC_EDMAOKR;//0x14-----EDMAC CRC OK Register

}CRC_TypeDef;

#define MODE_CRC_32_MASK              (1<<31)
#define MODE_CRC_16_MASK              (1<<30)
#define MODE_CRC_8_MASK               (1<<29)
#define CRC_UPDATA_MASK               (1<<28)
#define CRC_SUORCE_SEL_MASK           (1<<27)
#define CRC_BIGENDDING_MASK           (1<<26)
#define CRC_EDMA_SEL_MASK             (1<<24)
#define CRC_EDMA_UPDATA_MASK          (1<<20)
#define EDMA_CRC_LAT_MASK             (1<<19)
#define CRC_DECODE_MASK               (1<<18)
#define SEND_CRC_CODE_EN_MASK         (1<<17)
#define WRITE_CRC_CODE_EN_MASK        (1<<16)
#define CRC_OK_MASK                   (1<<0)



#endif /* CRC_REG_H_ */
