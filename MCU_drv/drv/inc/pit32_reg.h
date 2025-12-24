/*
 * pit32_reg.h
 *
 *  Created on: 2017Äê11ÔÂ29ÈÕ
 *      Author: nltao
 */

#ifndef PIT32_REG_H_
#define PIT32_REG_H_


#include "type.h"
#include "memmap.h"
#pragma anon_unions
typedef struct
{
	union
	{
		__IO unsigned short PCSR    ;//     0x00
		struct
		{
			__IO unsigned char PCSR_H      ;//     0x00
			__IO unsigned char PCSR_L     ;//     0x01
		};
	};
	    unsigned short RESERVED;//0x02~0x03

	__IO unsigned int PMR;  //		0x4
	__IO unsigned int PCNTR;//		0x8

}PIT32_TypeDef;



#define PIT_EN						(1 << 0)
#define PIT_RLD						(1 << 1)
#define PIT_PIF						(1 << 2)
#define PIT_PIE						(1 << 3)
#define PIT_OVW						(1 << 4)
#define PIT_DOZE				    (1 << 6)


#define PCSR_EN		0x01
#define PCSR_RLD	0x02
#define PCSR_PIF	0x04
#define PCSR_PIE	0x08
#define PCSR_OVW	0x10
#define PCSR_PDBG	0x20
#define PCSR_PDOZE	0x40
#define PCSR_PRE	0x0F

#endif /* PIT32_REG_H_ */
