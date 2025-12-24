// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : cpm_reg.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __CPM_REG_H__
#define __CPM_REG_H__

#include "type.h"

#define INTERNAL_CLK_SEL			0
#define EXTERNAL_CLK_SEL			1

typedef struct
{
	__IO unsigned int  CPM_SLPCFGR;//0x00
	__IO unsigned int  CPM_SLPCR;//0x04
	__IO unsigned int  CPM_SCDIVR;//0x08
	__IO unsigned int  CPM_PCDIVR1;//0x0c
	
	__IO unsigned int  CPM_PCDIVR2;//0x10
	__IO unsigned int  CPM_PCDIVR3;//0x14
	__IO unsigned int  CPM_CDIVUPDR;//0x18
	__IO unsigned int  CPM_CDIVENR;//0x1c

	__IO unsigned int  CPM_OCSR;//0x20
	__IO unsigned int  CPM_CSWCFGR;//0x24
	__IO unsigned int  CPM_CTICKR;//0x28
	__IO unsigned int  CPM_CHIPCFGR;//0x2c

	__IO unsigned int  CPM_PWRCR;//0x30
	__IO unsigned int  CPM_SLPCNTR;//0x34
	__IO unsigned int  CPM_WKPCNTR;//0x38
	__IO unsigned int  CPM_MULTICGTCR;//0x3c

	__IO unsigned int  CPM_SYSCGTCR;//0x40
	__IO unsigned int  CPM_AHB3CGTCR;//0x44
	__IO unsigned int  CPM_ARITHCGTCR;//0x48
	__IO unsigned int  CPM_IPSCGTCR;//0x4c

	__IO unsigned int  CPM_VCCGTRIMR;//0x50
	__IO unsigned int  CPM_VCCLTRIMR;//0x54
	__IO unsigned int  CPM_VCCVTRIMR;//0x58
	__IO unsigned int  CPM_VCCCTMR;//0x5c

	__IO unsigned int  CPM_O8MTRIMR;//0x60
	     unsigned int  RESERVED1;//0x64
	__IO unsigned int  CPM_O600MTRIMR;//0x68
	__IO unsigned int  CPM_CARDTRIMR;//0x6c

	__IO unsigned int  CPM_OSCLSTIMER;//0x70
	__IO unsigned int  CPM_OSCHSTIMER;//0x74
	__IO unsigned int  CPM_OSCESTIMER;//0x78
	__IO unsigned int  CPM_PWRSR;//0x7c

	     unsigned int  RESERVED2[3];//0x80~0x88
	__IO unsigned int  CPM_RTCTRIMR;//0x8c

	__IO unsigned int  CPM_PADWKINTCR;//0x90
	__IO unsigned int  CPM_FILTCNTR;//0x94
	__IO unsigned int  CPM_CARDPOCR;//0x98
	__IO unsigned int  CPM_RTCSTIMER;//0x9c

	__IO unsigned int  CPM_MPDSLPCR;//0xa0
	__IO unsigned int  CPM_MRMCR;//0xa4
	     unsigned int  RESERVED3;//0xa8
	__IO unsigned int  CPM_MULTIRSTCR;//0xac

	__IO unsigned int  CPM_SYSRSTCR;//0xb0
	__IO unsigned int  CPM_AHB2RSTCR;//0xb4
	__IO unsigned int  CPM_ARITHRSTTCR;//0xb8
	__IO unsigned int  CPM_IPRSTCR;//0xbc

	__IO unsigned int  CPM_SLPCFGR2;//0xc0
	     unsigned int  RESERVED4[3];//0xc4~0xcc

	__IO unsigned int  CPM_PDNCNTR;//0xd0
	__IO unsigned int  CPM_PONCNTR;//0xd4


}CPM_TypeDef;


//----------------------
//  CPM BIT DEFINE
//----------------------
/* CPM_SCDIVR */
#define SYS_DIV_MASK			(0xff)
#define TRACE_DIV_MASK		(0xff00)
#define CLKOUT_DIV_MASK		(0xff0000)

#define SYS_DIV_SHIFT				(0)
#define TRACE_DIV_SHIFT			(8)
#define CLKOUT_DIV_SHIFT		(16)

/*CPM_PCDIVER1 MACRO*/
#define IPS_DIV_MASK					(0xf)
#define AHB3_DIV_MASK					(0xf00)
#define ARITH_DIV_MASK				(0xf000)
#define SDRAM_DIV_MASK				(0xf0000)
#define SDRAM_SM_MASK					(0x7000000)
#define SDRAM2LCD_DIV_MASK		(0xf0000000)

#define IPS_DIV_SHIFT					(0)
#define AHB3_DIV_SHIFT				(8)
#define ARITH_DIV_SHIFT				(12)
#define SDRAM_DIV_SHIFT				(16)
#define SDRAM_SM_SHIFT				(24)
#define SDRAM2LCD_DIV_SHIFT		(28)

/*CPM_PCDIVER2 MACRO*/
#define MCC_DIV_MASK			(0x1ff)
#define MCCADR_DIV_MASK		(0xe00)
#define ADC_DIV_MASK			(0xf000)
#define CLCD_DIV_MASK			(0xf0000)
#define MESH_DIV_MASK			(0xf000000)
#define TC_DIV_MASK				(0xf0000000)

#define MCC_DIV_SHIFT				(0)
#define MCCADR_DIV_SHIFT		(9)
#define ADC_DIV_SHIFT				(12)
#define CLCD_DIV_SHIFT			(16)
#define MESH_DIV_SHIFT			(24)
#define TC_DIV_SHIFT				(28)

/*CPM_PCDIVER3 MACRO*/
#define DMA2D_SRAM_DIV_MASK			(0xf)
#define MIPI_SAMPLE_DIV_MASK		(0xf0)
#define DCMI_PIX_DIV_MASK				(0xf00)
#define DCMI_SENSOR_DIV_MASK		(0x3f0000)

#define DMA2D_SRAM_DIV_SHIFT		(0)
#define MIPI_SAMPLE_DIV_SHIFT		(4)
#define DCMI_PIX_DIV_SHIFT			(8)
#define DCMI_SENSOR_DIV_SHIFT		(16)

/*CPM_CDIVUPDR MACRO*/
#define PERDIV_UPD		(1)
#define SYSDIV_UPD		(2)

/*CPM_CDIVENR MACRO*/
#define IPS_DIVEN						(1<<0)
#define AHB3_DIVEN					(1<<2)
#define ARITH_DIVEN					(1<<3)
#define SDRAM_DIVEN					(1<<4)
#define SDRAM_SM_DIVEN			(1<<6)
#define SDRAM2LCD_DIVEN			(1<<7)
#define MCC_DIVEN						(1<<8)
#define ADC_DIVEN						(1<<10)
#define CLCD_DIVEN					(1<<11)
#define MESH_DIVEN					(1<<12)
#define TC_DIVEN						(1<<13)
#define TRACE_DIVEN					(1<<14)
#define CLKOUT_DIVEN				(1<<15)
#define DMA2D_SRAM_DIVEN		(1<<16)
#define MIPI_SAMPLE_DIVEN		(1<<17)
#define DCMI_PIX_DIVEN			(1<<18)
#define DCMI_SENSOR_DIVEN		(1<<19)

/*CPM_OCSR MACRO*/
#define OSC8M_EN							(1<<0)
#define PMU128K_EN						(1<<1)
#define USB_PHY240M_EN				(1<<2)
#define OSC160M_EN						(1<<3)
#define OSCEXT_EN							(1<<4)
#define RTC32K_EN							(1<<5)
#define OSC8M_STABLE					(1<<8)
#define PMU128K_STABLE				(1<<9)
#define USB_PHY240M_STABLE		(1<<10)
#define OSC160M_STABLE				(1<<11)
#define OSCEXT_STABLE					(1<<12)
#define RTC32K_STABLE					(1<<13)
#define PMU2K_VALID						(1<<14)
#define TRNG_OSCEN_MASK				(0xf000000)

#define TRNG_OSCEN_SHIFT			(24)

/* CPM_CSWCFGR MACRO*/
#define SYSCLK_SEL_MASK							(0x3)
#define DCMI_SENSOR_SEL_MASK				(0x3f)
#define OSCL_SEL										(1<<6)
#define OSCH_SEL										(1<<7)
#define SYS_SEL_ST_MASK							(0xf00)
#define DCMI_SENSOR_SEL_ST_MASK			(0x70000)
#define OSCL_SEL_ST_MASK						(0x300000)
#define OSCH_SEL_ST_MASK						(0xc00000)
#define CLKOUT_SEL_MASK							(0x3000000)
#define CLKOUT_SEL_ST_MASK					(0xf0000000)

#define SYS_SEL_SHIFT								(0)
#define DCMI_SENSOR_SEL_SHIFT				(4)
#define SYS_SEL_ST_SHIFT						(8)
#define DCMI_SENSOR_SEL_ST_SHIFT		(16)
#define OSCL_SEL_ST_SHIFT						(20)
#define OSCH_SEL_ST_SHIFT						(22)
#define CLKOUT_SEL_SHIFT						(24)
#define CLKOUT_SEL_ST_SHIFT					(28)

#define SYSCLK_SEL_OSC8M				(0x00)
#define SYSCLK_SEL_OSC160M			(0x01)
#define SYSCLK_SEL_USBPHY240M		(0x02)
#define SYSCLK_SEL_OSCEXT				(0x03)

#define CLKSEL_ST_OSC8M					(0x0100)
#define CLKSEL_ST_OSC160M				(0x0200)
#define CLKSEL_ST_USBPHY240M		(0x0400)
#define CLKSEL_ST_OSCEXT				(0x0800)


/* CPM_SRSTCR1 */
#define USBC_RST_RELEASE_BIT			(1<<8)
#define USBPHY_RST_RELEASE_BIT		(1<<13)
#define PWRCR_PHY_PSWEN_BIT				(1<<24)
#define PWRCR_PHY_RSTMASK_BIT			(1<<25)
#define PWRCR_PHY_O_ISOEN_BIT			(1<<26)
#define PWRCR_PHY_I_ISOEN_BIT			(1<<27)
#define USBPHY_PLL_SRM						(1<<28)
#define USBPHY_CFG_SRM						(1<<29)

#endif /* __CPM_REG_H__ */
