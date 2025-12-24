/************************************************************************
  * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
  * @file     dac.c
  * @author   UartTFT Application Team
  * @version  V1.0.0
  * @date     2023-02-24
  * @brief    
*************************************************************************/


#include "dac.h"

#include "pit.h"
#include "data.h"
#include "bsp.h"
#include "pwm.h"
#include "pit32_drv.h"
#include "dmac_drv.h"
#include "levetop.h"

/*SPI1 DMA channel base address*/
DMA_CHANNEL_REG *dac_dma_channel[4] = {(DMA_CHANNEL_REG *)(DMA2_BASE_ADDR), (DMA_CHANNEL_REG *)(DMA2_BASE_ADDR + 0x58),
									   (DMA_CHANNEL_REG *)(DMA2_BASE_ADDR + 0xB0), (DMA_CHANNEL_REG *)(DMA2_BASE_ADDR + 0x108)}; // global struct variable for for Channel registers
/*SPI1 DMA config base address */
DMA_CONTROL_REG *dac_dma_control = (DMA_CONTROL_REG *)(DMA2_BASE_ADDR + 0x2C0); // global struct variable for for DMAC registers

UINT16 etARRValue(UINT16 sample)
{
	UINT16 arrValue;
	/* Update OCA values to match the. WAV file sampling rate */
	switch (sample)
	{
	case 8000:
		arrValue = (uint16_t)(75000000 / 8000);
		break; /* 8KHz = 75MHz / 9375 */
	case 11025:
		arrValue = (uint16_t)(75000000 / 11025);
		break; /* 11.025KHz = 75MHz / 6802 */
	case 16000:
		arrValue = (uint16_t)(75000000 / 16000);
		break; /* 16KHz = 75MHz / 4687 */
	case 22050:
		arrValue = (uint16_t)(75000000 / 22050);
		break; /* 22.05KHz = 75MHz / 3401 */
	case 44100:
		arrValue = (uint16_t)(75000000 / 44100);
		break; /* 44.1KHz = 75MHz / 1700 */
	case 48000:
		arrValue = (uint16_t)(75000000 / 48000);
		break; /* 48KHz = 75MHz / 1562 */
	default:
		arrValue = 0;
		break;
	}
	return arrValue - 1;
}

void DAC_SetData(UINT16 data)
{
	DAC_Write_Data(data);
	DAC->DAC_SWTR |= SW_TRIG;
	DAC_Wait_Load_Done();
}

#include "dmac_drv.h"

#define DAC_TIMER ((PIT32_TypeDef *)PIT2_BASE_ADDR)

uint8_t gWavState = 0;
uint8_t dac_data_buff_1[4096];
uint8_t dac_data_buff_2[4096];
uint32_t gWavFlashAddr = 0;
uint16_t numflag = 0;
uint16_t bufflen = 0;
uint8_t finishflag = 0;
uint8_t wbuf_select = 0;
uint8_t gWavAddrFull_Flag = 0;
uint8_t last_select = 0;
uint8_t gWavType = 0;
uint8_t gWavFlag = 0;
uint16_t buffnum;
uint16_t bufflast;
uint32_t gWavLen = 0;

static void DAC_Timer_Init(unsigned int rate)
{
	DAC_TIMER->PCSR &= (~PCSR_EN);

	DAC_TIMER->PCSR &= (~PCSR_EN);
	DAC_TIMER->PCSR = (0 << 8) | PCSR_OVW | PCSR_RLD;

	DAC_TIMER->PMR = (g_ips_clk / 1000000 * (1000000 / (float)rate));
	//	PIT32->PCSR = (PIT32_CLK_DIV_1<<8)|PCSR_OVW|PCSR_RLD;
}

static void DAC_Timer_Enable(void)
{
	DAC_TIMER->PCSR |= PCSR_EN;
}

static void DAC_Timer_Disable(void)
{
	DAC_TIMER->PCSR &= ~PCSR_EN;
}

void DAC_DMA_Tran(UINT8 channel, UINT32 src, UINT32 length)
{
	//	DMA_Init(DMA2_BASE_ADDR);
	// dac_dma_control->DMA_CONFIG = 1;
	dac_dma_channel[channel]->DMA_SADDR = src;
	dac_dma_channel[channel]->DMA_DADDR = 0x40021004;
	//	dac_dma_channel[channel]->DMA_CTRL = SIEC|DNOCHG|M2P_DMA|DWIDTH_B|SWIDTH_B|INTEN;
	dac_dma_channel[channel]->DMA_CTRL = SIEC | DNOCHG | M2P_DMA | DWIDTH_HW | SWIDTH_HW | INTEN;
	dac_dma_channel[channel]->DMA_CTRL_HIGH = length;

	dac_dma_channel[channel]->DMA_CFG = 0;
	dac_dma_channel[channel]->DMA_CFG_HIGH = DST_PER_DAC;

	dac_dma_control->DMA_MASKTFR = CHANNEL_UMASK(channel);
	dac_dma_control->DMA_CHEN = CHANNEL_WRITE_ENABLE(channel) | CHANNEL_ENABLE(channel);
}
void DAC_DMA_dis(UINT8 n)
{
	dac_dma_channel[n]->DMA_CFG |= 1 << 8;
	dac_dma_control->DMA_CHEN |= CHANNEL_WRITE_ENABLE(n);
	dac_dma_control->DMA_CHEN &= ~CHANNEL_ENABLE(n);
	dac_dma_channel[n]->DMA_CFG &= ~(1 << 8);
	dac_dma_control->DMA_CONFIG = 0;
}

void dac_dma_tran(uint8_t *data, uint32_t len)
{
	uint32_t tran_l = 0;
	uint8_t *wDataBufOut = data;
	tran_l = (len >> 1);
	len -= (tran_l << 1);

	//	DMA_Init(DMA2_BASE_ADDR);

	DAC_DMA_Tran(0, (uint32_t)wDataBufOut, tran_l);
}

void WAV_amplifier_EN(void)
{
	SPI_ConfigGpio(SPI3, SPI_MOSI, GPIO_OUTPUT);
	SPI_WriteGpioData(SPI3, SPI_MOSI, 1);
}

void WAV_amplifier_DIS(void)
{
	SPI_ConfigGpio(SPI3, SPI_MOSI, GPIO_OUTPUT);
	SPI_WriteGpioData(SPI3, SPI_MOSI, 0);
}

void close_wav_dac(void)
{
	if (!gWavType)
	{
		var[0x700a * 2] = 0;
		var[0x700a * 2 + 1] = 0;
	}

	gWavFlag = 0;
	gWavAddrFull_Flag = 0;
	WAV_amplifier_DIS();
	DAC_Timer_Disable();
	DAC_DMA_dis(0); // DAC uses channel 0 of DMA2
}

void Wav_DAC_Init(uint16_t num)
{
	uint8_t buff[8];
	WAV_amplifier_DIS();
	gWavType = 0;
	close_wav_dac();
	if (num > 0 && num != 0x8000 && (num & 0x7FFF) <= addr_index[5])
	{
		if (num & 0x8000)
			gWavType = 1;
		else
			gWavType = 0;

		Flash_Read_UI(buff, addr_index[4] + ((num & 0x7FFF) - 1) * 8, 8);
		gWavFlashAddr = buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
		gWavLen = buff[4] + (buff[5] << 8) + (buff[6] << 16) + (buff[7] << 24);
		if (gWavLen == 0)
		{
			gWavType = 0;
			close_wav_dac();
		}
		else
		{
			bufflen = 4096;
			if (Flash_type == 1)
				bufflen = 4096;
			buffnum = gWavLen / bufflen;  // Total number of data packages
			bufflast = gWavLen % bufflen; // Data volume of the last packet
			if (bufflast > 0)
				buffnum++;
			else if (bufflast == 0)
				bufflast = bufflen;

			wbuf_select = 0;
			last_select = 0;
			numflag = 0;
			gWavState = 1;
			gWavFlag = 1;
		}
	}
	else
	{
		gWavType = 0;
		close_wav_dac();
	}
}

uint8_t wav_reduce_para = 0;
uint8_t wav_reduce_para_last = 0;
void DMA2_IRQHandler(void)
{
	uint32_t i = 0;
	int16_t temp = 0;

	if (dac_dma_control->DMA_STATTFR & 0x0f)
	{
		finishflag = 1;
		numflag++;
	}

	dac_dma_control->DMA_CLRTFR = dac_dma_control->DMA_STATTFR;
	dac_dma_control->DMA_CLRBLOCK = dac_dma_control->DMA_STATBLOCK;
	dac_dma_control->DMA_CLRSRC = dac_dma_control->DMA_STATSRC;
	dac_dma_control->DMA_CLRDST = dac_dma_control->DMA_STATDST;
	dac_dma_control->DMA_CLRERR = dac_dma_control->DMA_STATERR;

	if (wbuf_select == 0 && numflag<=buffnum) // Transfer the next packet of data			
		dac_dma_tran(dac_data_buff_1, bufflen);
	else if (wbuf_select == 1 && numflag<=buffnum)									
		dac_dma_tran(dac_data_buff_2, bufflen);

	if (numflag == (buffnum - 1)) // Last packet of data
	{
		bufflen = bufflast;
		last_select = 1;
	}
	else if (numflag == buffnum) // Transfer completed
	{
		last_select = 2;
		gWavAddrFull_Flag = 1;
	}
}

void LT_PlayWav_DAC(void)
{
	uint32_t i = 0;
	int32_t temp = 0;

	if (wav_reduce_para != 0 && wav_reduce_para_last == 0)
		WAV_amplifier_EN();
	else if (wav_reduce_para == 0 && wav_reduce_para_last != 0)
		WAV_amplifier_DIS();
	wav_reduce_para_last = wav_reduce_para;
	
	if (gWavAddrFull_Flag == 1 && finishflag == 1 && numflag>buffnum)	
	{
		gWavAddrFull_Flag = 0;
		numflag = 0;
		last_select = 0;
		bufflen = 4096;
		close_wav_dac();
		if (gWavType == 1)
			gWavState = 1;
	}
	else if (gWavAddrFull_Flag == 0)	
	{
		if (gWavState == 1)
		{
			gWavState = 0;
			// Read the first and second packet data
			Flash_Read_UI(dac_data_buff_1, gWavFlashAddr + numflag * bufflen, bufflen);
			for (i = 0; i < bufflen / 2; i++)
			{
				temp = (dac_data_buff_1[2 * i] >> 4) | (dac_data_buff_1[2 * i + 1] << 4);
				temp = (((temp - 2048) * wav_reduce_para) >> 4) + 2048;
				dac_data_buff_1[2 * i] = (temp << 4) & 0xF0;
				dac_data_buff_1[2 * i + 1] = (temp >> 4) & 0xFF;
			}

			numflag++;
			Flash_Read_UI(dac_data_buff_2, gWavFlashAddr + numflag * bufflen, bufflen);
			for (i = 0; i < bufflen / 2; i++)
			{
				temp = (dac_data_buff_2[2 * i] >> 4) | (dac_data_buff_2[2 * i + 1] << 4);
				temp = (((temp - 2048) * wav_reduce_para) >> 4) + 2048;
				dac_data_buff_2[2 * i] = (temp << 4) & 0xF0;
				dac_data_buff_2[2 * i + 1] = (temp >> 4) & 0xFF;
			}

			// Initialize DMA and DAC functions
			DAC_Init(LEFTALIGNED_12BITS, TRIGGER_PIT, DET_ON_RISING);
			dac_dma_control->DMA_CONFIG = 1;

			NVIC_Init(2, 0, DMA2_IRQn, 2);
			DAC_Timer_Init(22050);
			DAC_Timer_Enable();

			if (wav_reduce_para)
				WAV_amplifier_EN();

			dac_dma_tran(dac_data_buff_1, bufflen);
			finishflag = 0;
			wbuf_select = 1;
		}
		if (wbuf_select == 0 && finishflag == 1 && last_select == 0)
		{
			wbuf_select = 1;
			finishflag = 0;

			Flash_Read_UI(dac_data_buff_2, gWavFlashAddr + numflag * bufflen, bufflen);
			for (i = 0; i < bufflen / 2; i++)
			{
				temp = (dac_data_buff_2[2 * i] >> 4) | (dac_data_buff_2[2 * i + 1] << 4);
				temp = (((temp - 2048) * wav_reduce_para) >> 4) + 2048;
				dac_data_buff_2[2 * i] = (temp << 4) & 0xF0;
				dac_data_buff_2[2 * i + 1] = (temp >> 4) & 0xFF;
			}

		} // DMA2_IRQHandler
		else if (wbuf_select == 1 && finishflag == 1 && last_select == 0)
		{
			wbuf_select = 0;
			finishflag = 0;

			Flash_Read_UI(dac_data_buff_1, gWavFlashAddr + numflag * bufflen, bufflen);
			for (i = 0; i < bufflen / 2; i++)
			{
				temp = (dac_data_buff_1[2 * i] >> 4) | (dac_data_buff_1[2 * i + 1] << 4);
				temp = (((temp - 2048) * wav_reduce_para) >> 4) + 2048;
				dac_data_buff_1[2 * i] = (temp << 4) & 0xF0;
				dac_data_buff_1[2 * i + 1] = (temp >> 4) & 0xFF;
			}
		}
		else if (last_select == 1 && finishflag == 1)
		{
			finishflag = 0;
			if (wbuf_select)
			{
				wbuf_select = 0;
				Flash_Read_UI(dac_data_buff_1, gWavFlashAddr + numflag * bufflen, bufflen);
				for (i = 0; i < bufflen / 2; i++)
				{
					temp = (dac_data_buff_1[2 * i] >> 4) | (dac_data_buff_1[2 * i + 1] << 4);
					temp = (((temp - 2048) * wav_reduce_para) >> 4) + 2048;
					dac_data_buff_1[2 * i] = (temp << 4) & 0xF0;
					dac_data_buff_1[2 * i + 1] = (temp >> 4) & 0xFF;
				}
			}
			else
			{
				wbuf_select = 1;
				Flash_Read_UI(dac_data_buff_2, gWavFlashAddr + numflag * bufflen, bufflen);
				for (i = 0; i < bufflen / 2; i++)
				{
					temp = (dac_data_buff_2[2 * i] >> 4) | (dac_data_buff_2[2 * i + 1] << 4);
					temp = (((temp - 2048) * wav_reduce_para) >> 4) + 2048;
					dac_data_buff_2[2 * i] = (temp << 4) & 0xF0;
					dac_data_buff_2[2 * i + 1] = (temp >> 4) & 0xFF;
				}
			}
		}
	}
}
