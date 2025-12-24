/*******************************************************************************
  * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
  * @file     data.h
  * @author   UartTFT Application Team
  * @version  V1.0.0
  * @date     2023-02-24
  * @brief
 ********************************************************************************/

#ifndef _data_h
#define _data_h
#include "type.h"
#include "module_select.h"

extern volatile uint8_t var[];


#if R_TOUCH_FLAG
#include "rtp.h"
#endif

#if FT_TOUCH_FLAG
#include "ft5216.h"
#endif

#define RX_SIZE (4096 + 2) // Macro used to control USART receiving data

typedef struct
{
	uint8_t Flag; // Data receiving completion flag
	uint8_t Buf[RX_SIZE];
	uint16_t Count;
} USART_RX_INFO;

extern USART_RX_INFO gUsartRx; // For serial port reception


#if (UARTBUS_OPTION == 3 || UARTBUS_OPTION == 4)
typedef struct
{
	uint8_t Flag; // Data receiving completion flag
	uint8_t Buf[2048 + 20];
	uint16_t Count;
} Hid_RX_INFO;

extern Hid_RX_INFO gHidRx; // For serial port reception
#endif

#define CT_MAX_TOUCH 5 // Number of points supported by capacitive screen
typedef struct
{
	uint8_t (*scan)(void);	  // Scan the touch screen;
	uint8_t sta;			  // Status flag,  0:Not pressed	1:First press	2:Keep pressing
	uint16_t x[CT_MAX_TOUCH]; // Screen coordinates
	uint16_t y[CT_MAX_TOUCH]; // Screen coordinates
	uint16_t px;			  // Physical coordinates
	uint16_t py;			  // Physical coordinates

	uint8_t flag; // Read from flash. When the value is 0x5a, it means that there are four parameters in flash
	float xfac;
	float yfac;
	float xoff;
	float yoff;
} TP_INFO;
extern TP_INFO gTpInfo; 

typedef union
{

	float x;
	char s[4];
} TP_TEST;

typedef union
{

	float xfac;
	char s[4];
} TP_xfac;

typedef union
{

	float xoff;
	char s[4];
} TP_xoff;

typedef union
{

	float yfac;
	char s[4];
} TP_yfac;

typedef union
{

	float yoff;
	char s[4];
} TP_yoff;
extern TP_xfac gTpxfac;
extern TP_xoff gTpxoff;
extern TP_yfac gTpyfac;
extern TP_yoff gTpyoff;

extern uint8_t First_press;
extern uint16_t First_pressX, First_pressY;
#endif
