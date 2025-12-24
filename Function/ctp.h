/************************************************************************
  * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
  * @file     ctp.h
  * @author   UartTFT Application Team
  * @version  V1.0.0
  * @date     2023-02-24
  * @brief     
*************************************************************************/

#ifndef __CTP_H
#define __CTP_H

#include "LT768_Lib.h"
#include "delay.h"
#include "data.h"

/************************Error code*****************************/
#define CT_COM_OK (0)
#define CT_ACK_FAIL (1)

#define FT6X06_ID (0x6206)
#define CT_IC (FT6X06_ID) // Control IC used
#define CT_ADDR (0x70)    // Device address
#define CT_WRITE_MASK (0x00)
#define CT_READ_MASK (0x01)
#define CT_CACK_TIMEOUT (3000) // Wait for ACK timeout

void CTP_Init(void);
uint8_t CT_Read_Nbyte(const uint8_t sla_add, const uint16_t add, uint16_t n, uint8_t *s);
uint8_t CT_Write_Nbyte(const uint8_t sla_add, const uint16_t add, uint16_t n, const uint8_t *s); 
void TP_read_XY(void);
uint8_t FT5216_Scan(void);
extern uint8_t ctp_active_index;
extern uint8_t ctp_press_t;

void Checksum(uint8_t *p); // Configuration information check 0x80ff (checked algorithm is correct)
#if IIC_BUS
void I2C_Slave_TEST(uint8_t sla_addr);

extern uint8_t flag_IIC_REG[];
extern uint16_t ID_IIC_WAV;
#endif

#endif
