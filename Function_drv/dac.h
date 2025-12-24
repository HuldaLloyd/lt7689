/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     dac.h
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief
 *************************************************************************/

#ifndef __DAC_H__
#define __DAC_H__

#include "type.h"
#include "dac_drv.h"

uint16_t etARRValue(uint16_t sample);
extern uint8_t wav_reduce_para;
extern uint8_t wav_reduce_para_last;
void DAC_SetData(uint16_t data);

void LT_PlayWav_DAC(void);
void Wav_DAC_Init(uint16_t num);
void close_wav_dac(void);
extern uint8_t gWavFlag;
extern uint8_t gWavType;
#endif