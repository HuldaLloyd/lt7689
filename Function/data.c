/*******************************************************************************
  * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
  * @file     data.c
  * @author   UartTFT Application Team
  * @version  V1.0.0
  * @date     2023-02-24
  * @brief
 ********************************************************************************/

#include "data.h"

USART_RX_INFO gUsartRx; // Structure used to store received results

#if (UARTBUS_OPTION == 3 || UARTBUS_OPTION == 4)
Hid_RX_INFO gHidRx; // Structure used to store received results
#endif

TP_INFO gTpInfo; // Relevant parameters required for touch screen
TP_xfac gTpxfac;
TP_xoff gTpxoff;
TP_yfac gTpyfac;
TP_yoff gTpyoff;

uint8_t  First_press = 0;
uint16_t First_pressX, First_pressY;