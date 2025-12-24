// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : ccm_reg.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef CCM_REG_H_
#define CCM_REG_H_

#include "type.h"

typedef struct
{
	__IO unsigned short RESERVED0;//0x00  FD configuration register
	__IO unsigned short CCR;//0x02   chip configuration register
	__IO unsigned short CIR;//0x04   chip identification register
	__IO unsigned short PHYPA;//0x06   PHY parameter configurationregister
	__IO unsigned short RESERVED1;	//0x08
	__IO unsigned short CTR;//0x0a chip test register
	__IO unsigned int   RESERVED2;		//0x0C

	__IO unsigned int  PCFG12;//0x10
	__IO unsigned int  PCFG3; //0x14
	__IO unsigned int  RTCCFG12;//0x18
	__IO unsigned char RTCCFG3;//0x1C
	__IO unsigned char RTC_SR;//0x20

}CCM_TypeDef;


#endif /* CCM_REG_H_ */
