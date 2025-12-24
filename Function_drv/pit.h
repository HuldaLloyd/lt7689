/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     pit.h
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief
 *************************************************************************/
#ifndef PIT_H_
#define PIT_H_

#include "type.h"
#include <string.h>
#include "module_select.h"	

typedef struct
{
  uint8_t CMD;		// Modbus read and write instructions
  uint16_t Addr;	// Modbus slave register address
  uint16_t Num;		// Read and write data length(2Bytes)
} MB_CMD_RXINFO;
extern volatile MB_CMD_RXINFO gMB_CMDRX;
extern volatile uint8_t Feedback_ModbusBuf[];
extern uint8_t flag_MB_REG[];



extern volatile uint8_t Addr_ModBus_Sla;


extern volatile uint32_t pit_1ms_count;
extern volatile uint8_t uart_flag;

extern volatile uint32_t pit_1ms_count3;
extern volatile uint8_t timeout_flag;

extern volatile uint32_t tx_10ms_cnt;
extern volatile uint8_t tx_500ms_flag;

extern void LT_UsartTimeEnable(void);
extern void LT_UsartTimeDisable(void);
extern void LT_UsartTimeSetCounter(UINT32 Counter);

void PIT1_Init(void);
void PIT1_Init_SD(void);
void PIT2_Init(UINT16 arr);

#endif /* PIT_H_ */
