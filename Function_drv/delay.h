/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     delay.c
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief
 *************************************************************************/
 
#ifndef __DELAY_H__
#define __DELAY_H__

#include "type.h"

void delay(vu32 time);

void DelayMS(vu32 delaymS);

void DelayUS(vu32 delayuS);
#endif
