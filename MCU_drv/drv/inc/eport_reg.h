// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : eport_reg.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef EPORT_REG_H_
#define EPORT_REG_H_

#include "type.h"


typedef struct
{
	__IO unsigned short EPPAR;  //0x00
	__IO unsigned char  EPIER;  //0x02
	__IO unsigned char  EPDDR;  //0x03
	__IO unsigned char  EPPDR;  //0x04
//	__IO unsigned char  EPDR;   //0x05
	union
	{
		__IO unsigned char  EPDR;
		struct
		{
			__IO unsigned char pin0:1;
			__IO unsigned char pin1:1;
			__IO unsigned char pin2:1;
			__IO unsigned char pin3:1;
			__IO unsigned char pin4:1;
			__IO unsigned char pin5:1;
			__IO unsigned char pin6:1;
			__IO unsigned char pin7:1;
		}U8EPDR;
	};
	__IO unsigned char  EPPUER; //0x06
	__IO unsigned char  EPFR;   //0x07
	__IO unsigned char  EPODER; //0x08
	__IO unsigned char  EPLPR;  //0x09

}EPORT_TypeDef;

#define EPORT0_INT_NUM				0x25
#define EPORT1_INT_NUM				0x26
#define EPORT2_INT_NUM				0x27
#define EPORT3_INT_NUM				0x28
#define EPORT4_INT_NUM				0x29
#define EPORT5_INT_NUM				0x2a
#define EPORT6_INT_NUM				0x2b
#define EPORT7_INT_NUM				0x2c

#endif /* EPORT_REG_H_ */
