// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : pci_reg.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __PCI_REG_H__
#define __PCI_REG_H__

#include "type.h"

typedef struct
{
	__IO unsigned int  PCI_VDCR;//0x00
	__IO unsigned int  PCI_TDCR;//0x04
	__IO unsigned int  PCI_SDIOCR;//0x08
	__IO unsigned int  PCI_CR;//0x0c

	__IO unsigned int  PCI_ATIMPR;//0x10
	__IO unsigned int  PCI_NVRAMCR;//0x14
	__IO unsigned int  PCI_DETPCR;//0x18
	__IO unsigned int  PCI_SDIOPCR;//0x1c

	__IO unsigned int  PCI_DETSR;//0x20
	__IO unsigned int  PCI_SAER;//0x24
	__IO unsigned int  PCI_SAR;//0x28
	     unsigned int  PCI_SDIOGPIOCR;//0x2c

	__IO unsigned int  PCI_WRITEEN;//0x30
	__IO unsigned int  reserve_2;
	__IO unsigned int  PCI_NVRAMKEY;//0x38
	__IO unsigned int  PCI_RTCIER;//0x3c

	__IO unsigned int  PCI_WSR;//0x40

	__IO unsigned int  PCI_DETSLR;//0x44
	__IO unsigned int  PCI_IRCCR;//0x48
	__IO unsigned int  PCI_CLKSR;//0x4c
	__IO unsigned int  PCI_CLKCER;//0x50

}PCI_TypeDef;

#endif /* __PCI_REG_H__ */
