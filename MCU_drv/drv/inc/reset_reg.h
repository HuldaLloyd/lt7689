#ifndef __RESET_REG_H__
#define __RESET_REG_H__

#include "memmap.h"
#include "type.h"

typedef struct
{
	__IO unsigned int RCR;    //offset0x00:0~3 Reset Control Register
	__IO unsigned short RFU;  //offset0x04:4~5 reserve
	__IO unsigned char RTR;   //offset0x06:6   Reset Test Register
	__IO unsigned char RSR;   //offset0x07:7   Reset Status Register
}RESET_TypeDef;

#define SOFTRST  0x80000000
#define FPOR     (1<<8)
#define CRE			 (1<<23)

#define CHIP_RESET    ((RESET_TypeDef *)(RESET_BASE_ADDR))

#endif /* __RESET_REG_H__ */

