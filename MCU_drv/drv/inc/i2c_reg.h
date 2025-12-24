// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : i2c_reg.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef I2C_REG_H_
#define I2C_REG_H_

#include "type.h"


typedef struct
{
	__IO unsigned char SAHR		 ;//0x0
	__IO unsigned char SALR		 ;//0x1
	__IO unsigned char CR		 ;//0x2
	__IO unsigned char CPR		 ;//0x3
	__IO unsigned char CSR		 ;//0x4
	__IO unsigned char CDR		 ;//0x5
	__IO unsigned char SSHTR	 ;//0x6
	__IO unsigned char SHSIR	 ;//0x7
	__IO unsigned char PCR		 ;//0x8
	__IO unsigned char PDR		 ;//0x9
	__IO unsigned char DDR		 ;//0xa
	__IO unsigned char FCTR		 ;//0xb
	__IO unsigned char NSFTVR1;   //0xc
	__IO unsigned char NSFTVR5;   //0xd
}I2C_TypeDef;

#define CR_EN		0x01
#define CR_IEN		0x02
#define CR_MSMOD	0x04
#define CR_ACKEN	0x08
#define CR_REPSTA	0x10
#define CR_AMIE		0x20
#define CR_HMSEN	0x20

#define SR_TF		0x01
#define SR_RC		0x02
#define SR_AASLV	0x04
#define SR_BBUSY	0x08
#define SR_ARBL		0x10
#define SR_RXTX		0x20
#define SR_DACK		0x40
#define SR_AACK		0x80

#define SLV_HS      0x01

#define I2C_SLAVE_ADDR 	        0x30
#define I2C_MASTER_ADDR 		0x02

#define SDA_OPEN_DRAIN_OUTPUT  do{I2C->PCR |= 0x40;}while(0)
#define SDA_COMOS_OUTPUT       do{I2C->PCR &= 0xBF;}while(0)
#define SDA_PULLUP_EN          do{I2C->PCR |= 0x02;}while(0)
#define SDA_PULLUP_DIS         do{I2C->PCR &= 0xFD;}while(0)
#define SDA_PULLDOWN_EN        do{;}while(0)
#define SDA_PULLDOWN_DIS       do{;}while(0)

#define SCL_OPEN_DRAIN_OUTPUT  do{I2C->PCR |= 0x10;}while(0)
#define SCL_COMOS_OUTPUT       do{I2C->PCR &= 0xEF;}while(0)
#define SCL_PULLUP_EN          do{I2C->PCR |= 0x01;}while(0)
#define SCL_PULLUP_DIS         do{I2C->PCR &= 0xFE;}while(0)
#define SCL_PULLDOWN_EN        do{;}while(0)
#define SCL_PULLDOWN_DIS       do{;}while(0)


#endif /* I2C_REG_H_ */
