// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : pwm_reg.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef PWM_REG_H_
#define PWM_REG_H_

#include "type.h"
#pragma anon_unions
typedef struct
{
	union
	{
		__IO unsigned int PWM_PR;//0x00
		struct
		{
			__IO unsigned char PWM_PR_CP0;//0x00
			__IO unsigned char PWM_PR_CP1;//0x01
			__IO unsigned char PWM_PR_DZ0;//0x02
			__IO unsigned char PWM_PR_DZ1;//0x03
		};
	};
	__IO unsigned int PWM_CSR;//0x04
	union
	{
		__IO unsigned int PWM_CR;//0x08
		struct
		{
			__IO unsigned char PWM_CR_CH0;//0x08
			__IO unsigned char PWM_CR_CH1;//0x09
			__IO unsigned char PWM_CR_CH2;//0x0a
			__IO unsigned char PWM_CR_CH3;//0x0b
		};
	};
	__IO unsigned int PWM_CNR0;//0x0c
	__IO unsigned int PWM_CMR0;//0x10
	__IO unsigned int PWM_TR0;//0x14

	__IO unsigned int PWM_CNR1;//0x18
	__IO unsigned int PWM_CMR1;//0x1c
	__IO unsigned int PWM_TR1;//0x20

	__IO unsigned int PWM_CNR2;//0x24
	__IO unsigned int PWM_CMR2;//0x28
	__IO unsigned int PWM_TR2;//0x2c

	__IO unsigned int PWM_CNR3 ;//0x30
	__IO unsigned int PWM_CMR3;//0x34
	__IO unsigned int PWM_TR3;//0x38

	__IO unsigned int PWM_IER;//0x3c
	__IO unsigned char PWM_IFR ;//0x40
	     unsigned char RESERVED1[3];//0x41~0x43
	 union
	{
	    __IO unsigned int PWM_CCR0 ;//0x44
		struct
		{
			__IO unsigned char PWM_CCR0_CH0;//0x44
			     unsigned char RESERVED2[1];//0x45
			__IO unsigned char PWM_CCR0_CH1;//0x46
			     unsigned char RESERVED3[1];//0x47
		};
	};
	union
	{
		 __IO unsigned int PWM_CCR1;//0x48
		struct
		{
			__IO unsigned char PWM_CCR1_CH2;//0x48
				 unsigned char RESERVED4[1];//0x49
			__IO unsigned char PWM_CCR1_CH3;//0x4a
				 unsigned char RESERVED5[1];//0x4b
		};
	};
	__IO unsigned int PWM_CRLR0 ;//0x4c
	__IO unsigned int PWM_CFLR0 ;//0x50
	__IO unsigned int PWM_CRLR1 ;//0x54
	__IO unsigned int PWM_CFLR1 ;//0x58
	__IO unsigned int PWM_CRLR2 ;//0x5c

	__IO unsigned int PWM_CFLR2;// 0x60)
	__IO unsigned int PWM_CRLR3;// 0x64
	__IO unsigned int PWM_CFLR3;// 0x68
	union
	{
		__IO unsigned int PWM_PCR;//0x6c
		struct
		{
			__IO unsigned char PWM_PCR_PDR;//0x6c
			__IO unsigned char PWM_PCR_PULLEN;//0x6d
			__IO unsigned char PWM_PCR_PDDR;//0x6e
			     unsigned char RESERVED6[1];//0x6f
		};
	};

}PWM_TypeDef;

//PWM_CR
#define CH0EN			  0x00000001
#define CH0INV			0x00000004
#define CH0MOD			0x00000008

#define CH1EN			0x00000001
#define CH1INV			0x00000004
#define CH1MOD			0x00000008

#define CH2EN			0x00000001
#define CH2INV			0x00000004
#define CH2MOD			0x00000008

#define CH3EN			0x00000001
#define CH3INV			0x00000004
#define CH3MOD			0x00000008

#define DZEN0			0x00000010
#define DZEN1			0x00000020
//PWM_IER
#define PIER0			0x00000001
#define PIER1			0x00000002
#define PIER2			0x00000004
#define PIER3			0x00000008
//PWM_IFR
#define PIFR0			0x00000001
#define PIFR1			0x00000002
#define PIFR2			0x00000004
#define PIFR3			0x00000008
//PWM_CCR0&PWM_CCR1
#define INV				0x00000001
#define RL_IE			0x00000002
#define FL_IE			0x00000004
#define CAPCHxEN		0x00000008
#define CAPIF			0x00000010
#define CFLRD			0x00000080
#define CRLRD			0x00000040

#define PWM_INT_NUM 	24

#endif /* PWM_REG_H_ */
