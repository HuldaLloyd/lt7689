/*
 * adc_dma.c
 */
#include "memmap.h"
#include "dmac_drv.h"
#include "dmac_reg.h"
#include "type.h"
#include "adc_drv.h"

#define ADC_DMA_MASK      DMA_CH0_MASK

static DMA_LLI g_dma_lli[2];

#define MCC_DMA_LLI_INT_EN

#define  DMA_CHANNEL  m_dma_channel[0]
#define  DMA_CONTROL  m_dma_control

void DmaLliRegInit(DMA_LLI *dma_lli)
{
    DMA_CHANNEL->DMA_SADDR = dma_lli->src_addr;
    DMA_CHANNEL->DMA_DADDR = dma_lli->dst_addr;
    DMA_CHANNEL->DMA_LLP = (u32)dma_lli;
    DMA_CHANNEL->DMA_CTRL = dma_lli->control0;
    DMA_CHANNEL->DMA_CTRL_HIGH =  dma_lli->len;
}


void AdcDmaStart(void)
{
    ADC->CFGR1 |= DMAEN;

    DMA_CONTROL->DMA_CHEN = (ADC_DMA_MASK) | (ADC_DMA_MASK << 8);
}

void AdcDmaStop(void)
{
    DMA_CHANNEL->DMA_CFG |= 1 << 8;	//suspend
    while(((DMA_CHANNEL->DMA_CFG) & (1 << 9)) == 0);	//waite FIFO empty

    DMA_CONTROL->DMA_CHEN = (DMA_CONTROL->DMA_CHEN | (ADC_DMA_MASK << 8)) & (~(ADC_DMA_MASK));

    DMA_CHANNEL->DMA_CFG &= ~(1 << 8);	//clear suspend

    //DMA_CONTROL->DMA_CONFIG &= ~1;	//DMA disable

    ADC->CFGR1 &= ~DMAEN;
}


void DmaInit(u8 *pbuff, u32 buff_size)
{
    {
        g_dma_lli[0].src_addr = (u32)(&(ADC->uFIFO_DAT.FIFO));
        g_dma_lli[0].dst_addr = (u32)(&pbuff[0]);
        g_dma_lli[0].next_lli = (u32)(&(g_dma_lli[1]));
        g_dma_lli[0].control0 = DMA_IE | SNOCHG | DI | DWIDTH_HW | SWIDTH_HW | LLP_SRC_EN | LLP_DST_EN | P2M_DMA;
        g_dma_lli[0].len = buff_size / 4;

        g_dma_lli[1].src_addr = (u32)(&(ADC->uFIFO_DAT.FIFO));
        g_dma_lli[1].dst_addr = (u32)(&pbuff[buff_size / 2]);
        g_dma_lli[1].next_lli = (u32)(&(g_dma_lli[0]));
        g_dma_lli[1].control0 = DMA_IE | SNOCHG | DI | DWIDTH_HW | SWIDTH_HW | LLP_SRC_EN | LLP_DST_EN | P2M_DMA;
        g_dma_lli[1].len = buff_size / 4;
    }

    DMA_CHANNEL->DMA_CFG &= SRC_HARD;
    DMA_CHANNEL->DMA_CFG_HIGH = SRC_PER_QADC;
    DMA_CONTROL->DMA_MASKTFR = (ADC_DMA_MASK) | (ADC_DMA_MASK << 8);

    DMA_CONTROL->DMA_CONFIG = 0x01;
#ifdef MCC_DMA_LLI_INT_EN
    DMA_CONTROL->DMA_MASKBLOCK = (ADC_DMA_MASK) | (ADC_DMA_MASK << 8);
#endif

    DmaLliRegInit(&g_dma_lli[0]);
}

void AdcInit(s32 t1ch, s32 t3ch, s32 batch)
{
    u32 cfgr1, cfgr2, smpr, chsel = 0;
    u32 seq_len = 0; //track2

    if(batch >= 0)
    {
        chsel |= (0x09 & 0x1F);
        seq_len ++;
		
        chsel <<= 8;
        chsel |= (batch & 0x1F);
        seq_len ++;
    }

    if(t3ch >= 0)
    {
        chsel <<= 8;
        chsel |= (t3ch & 0x1F);
        seq_len ++;
    }

    //t2ch
    chsel <<= 8;
    chsel |= (0x0A & 0x1F);

    if(t1ch >= 0)
    {
        chsel <<= 8;
        chsel |= (t1ch & 0x1F);
        seq_len ++;
    }


    cfgr1 = (seq_len << 24)				// sequence length
            | 0x00100000			// continuous mode
            | (0 << 10)				// right aligned
            | (2 << 4)				// DMATH
            | (1 << 8);				// 10bits

    cfgr2 = 0x010;//0x008

    smpr = 6;//6;//4;//24;
    //ADC->IER = EOSEQIE;//EOCIE;

    ADC->CFGR1 = cfgr1;
    ADC->CFGR2 = cfgr2;
    ADC->CHSELR1 = chsel;
    ADC->CHSELR2 = 0;
    ADC->SMPR = smpr;

    ADC->CR = ADEN;
    while (ADRDY != (ADC->ISR & ADRDY));

    //ADC->CR = 0x5;
}

void AdcStart(void)
{
	ADC->CR = 0x5;
}

void AdcStop(void)
{
    ADC->CR |= 1 << 3;
    while(ADC->CR & (1 << 3));

    //ADC->CR &= ~1;
    ADC->CR |= (1 << 1);//disable ADC
    while(ADC->CR & (1 << 1));
}

