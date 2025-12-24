// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : adc_reg.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __ADC_REG_H__
#define __ADC_REG_H__

#include "type.h"

#pragma anon_unions	
typedef struct
{
	__IO unsigned int ISR;//0x00
	__IO unsigned int IER;//0x04
	__IO unsigned int CR;//0x08
	__IO unsigned int CFGR1;//0x0c
	__IO unsigned int CFGR2  ;//0x10
	__IO unsigned int SMPR   ;//0x14
	__IO unsigned int WDG    ;//0x18
	__IO unsigned int TR     ;//0x1c
	unsigned int 	RESERVED0[3];//0x20--0x28
	__IO unsigned int CHSELR1;//0x2c
	__IO unsigned int CHSELR2;//0x30
	unsigned int 	RESERVED1[3];//0x34--0x3c
	unsigned int 	RESERVED2[3];//0x40--0x48

	union
	{
		__IO unsigned int FIFO   ;//0x4c
		struct
		{
			__IO unsigned short DAT      ;//     0x4c
			__IO unsigned short NC     ;//     0x4e
		}sDAT;
	}uFIFO_DAT;
	__IO unsigned int TEST   ;//0x50
	__IO unsigned int ISR2   ;//0x54
	__IO unsigned int DGATR   ;//0x58
	__IO unsigned int DBUFR   ;//0x5c
	__IO unsigned int FIFOTOR   ;//0x60
	unsigned int  RESERVED3[7]  ;//0x64-0x7c
	unsigned int  RESERVED4[9]  ;//0x80-0xa0
	__IO unsigned int CHSELR3;//0xa4
}ADC_TypeDef;

//ADC_CFGR1
#define ADC_DIFF			(0<<31)
#define ADC_OVERMODE	(0<<30)
#define ADC_QCLK_DIS		(0<<29)
#define ADC_SMP_OBE		(0<<28)
#define ADC_SEQ_LEN		(0<<24)	//采样一个字
#define ADC_DISCEN		(0<<23)
#define ADC_AUTOFF		(0<<22)
#define ADC_WAIT			(0<<21)
#define ADC_CONT			(0<<20)
#define ADC_VREF			(1<<15)
#define ADC_TRIGMODE		(0<<11)
#define ADC_ALIGN		(1<<10)	//Right alignment
#define ADC_RES			(0<<8)
#define ADC_DMATH		(0<<4)
#define ADC_DMAEN		(0<<0)
#define ADC_SEQ_LEN_1   (0<<24)
#define ADC_SEQ_LEN_2   (1<<24)
#define ADC_SEQ_LEN_3   (2<<24)
#define ADC_SEQ_LEN_4   (3<<24)
#define ADC_SEQ_LEN_5   (4<<24)
#define ADC_SEQ_LEN_6   (5<<24)
#define ADC_SEQ_LEN_7   (6<<24)
#define ADC_SEQ_LEN_8   (7<<24)

#define ADC_OVRMOD_LAST_CONV      (1<<30)
#define ADC_OVRMOD_OLD_CONV       (0<<30)

#define ADC_CONTINUOUS_CONV_MODE  (1<<20)
#define ADC_SINGLE_CONV_MODE      (0<<20)

#define ADC_EXTERNAL_VREF         (1<<15)
#define ADC_INTERNAL_VREF         (0<<15)

#define ADC_RIGHT_ALIGN           (0<<10)
#define ADC_LEFT_ALIGN            (1<<10)

#define ADC_CONV_RESOLUTION_12BIT (0<<8)
#define ADC_CONV_RESOLUTION_10BIT (1<<8)
#define ADC_CONV_RESOLUTION_8BIT  (2<<8)
#define ADC_CONV_RESOLUTION_6BIT  (3<<8)
//ADC_CFGR2
#define ADC_QPR			(0<<8)
#define ADC_STCNT		(0<<1)

#define AWD             0x00000080
#define EMPTY           0x00000040
#define FULL            0x00000020
#define OVR             0x00000010
#define EOSEQ           0x00000008
#define EOC             0x00000004
#define EOSMP           0x00000002
#define ADRDY           0x00000001
#define AWDIE           0x00000080
#define OVRIE           0x00000010
#define EOSEQIE         0x00000008
#define EOCIE           0x00000004
#define EOSMPIE         0x00000002
#define ADRDYIE         0x00000001
#define ADSTP           0x00000008
#define ADSTART         0x00000004
#define ADDIS           0x00000002
#define ADEN            0x00000001
#define DIFF            0x80000000
#define OVRMOD          0x40000000
#define SEQLEN          0x07000000
#define DISCEN          0x00800000
#define AUTOFF          0x00400000
#define WAIT            0x00200000
#define CONT            0x00100000
#define TRIGSCR         0x00070000
#define TRIGMODE        0x00003800
#define ALIGN1           0x00000400
#define RES             0x00000300
#define DMATH           0x00000038
#define DMADSIZ         0x00000006
#define DMAEN           0x00000001

#define QPR             0x00000F00
#define STCNT           0x000000FF
#define AWDCH           0x0000000F
#define AWDSGL          0x00000040
#define AWDEN           0x00000080


#define HT              0x1FFF0000
#define LT              0x00001FFF
#define CCW0(n) 		(n)
#define CCW1(n) 		(n<<8)
#define CCW2(n) 		(n<<16)
#define CCW3(n) 		(n<<24)

#define CCW4(n) 		(n)
#define CCW5(n) 		(n<<8)
#define CCW6(n) 		(n<<16)
#define CCW7(n) 		(n<<24)

#define FIFO_DATA       0x0000FFFF

#define ADC_TRIG_RISING		1
#define ADC_TRIG_FALLING	2
#define ADC_TRIG_BOTH		3
#define ADC_TRIG_HIGH		4
#define ADC_TRIG_LOW		5
#define ADC_TRIG_ONE_SHOT	6

#define ADC_TRIG			11

// ADC FPGA simulation
#define ADC_LD0		(0x1<<16)
#define ADC_LD1		(0x1<<17)
#define ADC_LD2		(0x1<<18)
#define ADC_LD3		(0x1<<19)
#define ADC_LD4		(0x1<<20)
#define ADC_LD5		(0x1<<21)
#define ADC_LD6		(0x1<<22)
#define ADC_LD7		(0x1<<23)
#define ADC_LD8		(0x1<<24)
#define ADC_LD9		(0x1<<25)
#define ADC_LD10	(0x1<<26)
#define ADC_LD11	(0x1<<27)
#define ADC_LD12	(0x1<<28)
#define ADC_LD13	(0x1<<29)
#define ADC_LD14	(0x1<<30)
#define ADC_LD15	(0x1<<31)

#define ADC_LT(th)        (th & 0xfff)
#define ADC_HT(th)        ((th << 16) & 0xfff0000)
#define ADC_AWDCH(n)      (n)




#endif /* __ADC_REG_H__ */
