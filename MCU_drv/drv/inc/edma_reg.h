// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : edma_reg.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef EDMA_REG_H_
#define EDMA_REG_H_

#include "type.h"
#include "memmap.h"
typedef struct
{
	__IO unsigned int EDMACCR;//0x00@0x40
	__IO unsigned int EDMACCSR;//0x04@0x44
	__IO unsigned int EDMACRBAR;//0x08@0x48
	__IO unsigned int EDMACWBAR;//0x0c@0x4c
	__IO unsigned int EDMACMINSUMR;//0x10@0x50
	__IO unsigned int EDMACMINCNTR;//0x14@0x54
	__IO unsigned int EDMACMAJSUMR;//0x18@0x58
	__IO unsigned int EDMACMAJCNTR;//0x1c@0x5c
	__IO unsigned int EDMACSPAR;   //0x20@0x60

}EDMA_TypeDef;


//EDMAC Control State Register (EDMACCSR)
#define EDMAC_MINORDONE_MASK			(0x00000002)	//MINOR DONE
#define EDMAC_EDMAEN_MASK					(0x00010000)	//EDMAEN
#define EDMAC_MAJORDONE_MASK			(0x00000008)	//MAJOR DONE


#define EDMAC_CHVALID_MASK				(0x00000002)
#define EDMAC_MINDONEIE_MASK			(0x00000001)



#endif /* EDMA_REG_H_ */
