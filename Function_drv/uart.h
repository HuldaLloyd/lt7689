/************************************************************************
  * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
  * @file     uart.h
  * @author   UartTFT Application Team
  * @version  V1.0.0
  * @date     2023-02-24
  * @brief     
*************************************************************************/

#ifndef __UART_H__
#define __UART_H__

#include "type.h"
#include "sys.h"
#include "stdio.h"
#include "memmap.h"
#include "uart_drv.h"
#include "module_select.h"

extern uint8_t page_refreshing;
extern uint8_t Rx_Buffer_long[];
extern uint16_t Rx_Count_long;
extern uint16_t Rx_Num;

uint16_t CRC16(uint8_t *puchMsg, uint16_t usDataLen);

void UartDBG_Init(uint32_t pclk, uint32_t bound);

void SCI1_Init(uint32_t pclk, uint32_t bound,uint8_t parity);	
void LT_SingleByteToPc(uint8_t val);
void LT_ClearRxBuf(void);
void LT_SendData_CRC_Frame(uint8_t *buf, uint8_t len);
void LT_SendFlashData_CRC_Frame(uint8_t *buf, uint16_t len);
void LT_SendData_CRC_NoFrame(uint8_t *buf, uint8_t len);
void Main_From_UartRxBuff(void);

/* MODBUS: variables sent and received by the host*/
extern volatile uint8_t tx_en_flag;
extern volatile uint8_t tx_repeat_flag;
extern volatile uint8_t tx_repeat_cnt;

typedef struct
{
	uint8_t Flag;	  // Valid flag bit is 0xA5
	uint8_t Sla_Add;  // Modbus slave node
	uint8_t CMD;	  // Modbus read and write instructions
	uint16_t REG_Add; // Modbus slave register address
	uint8_t Len;	  // read and write data length(Bytes)
	uint16_t Var_Add; // Modbus host variable address
	uint16_t Timeout; // Maximum response time
	uint8_t RP_Cnt;	  // Maximum number of retransmissions
	uint8_t Mode;	  // Instruction execution mode
	uint16_t Opt_Add; // Parameters corresponding to instruction execution mode
} USART_TX_INFO;
extern volatile USART_TX_INFO gUsartTx;			// For serial port reception

extern volatile uint8_t Master_mode03_flag[];	// Customized variables
extern volatile uint8_t Master_mode03_Var[];	// Customized variables

void Uart_cmd_Send(void);


#if (UARTBUS_OPTION == 3 || UARTBUS_OPTION == 4)
#define Hid_SIZE (4096 + 20)
extern uint8_t Hid_Rx_Buffer_long[Hid_SIZE];
extern uint16_t Hid_Rx_Count_long;
extern uint16_t Hid_Rx_Num;
extern uint16_t Hid_Respond_Num;
extern uint16_t Hid_Respond_Count;

extern uint8_t Hid_receive_flag;
extern uint16_t Hid_receive_num ;
extern uint16_t Hid_receive_count;
void Hid_Main_From_UartRxBuff(void);
#endif

#endif
