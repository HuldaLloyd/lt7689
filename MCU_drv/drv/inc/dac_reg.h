// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : ccm_reg.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef CCM_REG_H_
#define CCM_REG_H_

#include "type.h"
#include "memmap.h"

typedef struct
{
	__IO unsigned int DAC_CR;					//0x00
	__IO unsigned int DAC_DR;					//0x04
	__IO unsigned int DAC_SWTR;				//0x08
	__IO unsigned int DAC_DOR;				//0x0c
	__IO unsigned int DAC_FSR;				//0x10
	__IO unsigned int DAC_TRIMR;			//0x14
}DAC_TypeDef;

#define DAC      ((DAC_TypeDef *)(DAC_BASE_ADDR))

#define DACEN						(1<<0)
#define BUFDIS					(1<<1)
#define ALIGN						(1<<2)
#define RES							(1<<3)
#define DMATH						(1<<16)
#define DMAEN						(1<<23)
#define STH							(1<<24)
#define STHIE						(1<<28)
#define UDFIE						(1<<29)
#define OVFIE						(1<<30)
#define FCLR						((UINT32)1<<31)

#define TSEL_MASK				(0x00000600)
#define TSEL_SHIFT			(9)

#define EXT_TMOD_MASK		(0x00007000)
#define EXT_TMOD_SHIFT	(12)

#define SW_TRIG					(1<<0)

#define SER							(1<<29)
#define OVF							(1<<30)
#define LD_DONE					((UINT32)1<<31)

#endif /* CCM_REG_H_ */
