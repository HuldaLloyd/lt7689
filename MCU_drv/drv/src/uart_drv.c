// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : uart_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "uart.h"
#include "cpm_drv.h"
#include "iomacros.h"
#include "uart_drv.h"
#include "uart_reg.h"
#include "stddef.h"
#include "stdarg.h"
#include "stdlib.h"

#define UART_RECV_MAX_LEN  512

typedef struct _uart_recv_buf
{
	UINT16 rp;//read pointer
	UINT16 wp;//write pointer
	UINT8  dat[UART_RECV_MAX_LEN];//buffer
}UartRecvBufStruct;

static UartRecvBufStruct g_Uart1RecvBufStruct = {0, 0};//uart1 buffer
static UartRecvBufStruct g_Uart2RecvBufStruct = {0, 0};//uart2 buffer
static UartRecvBufStruct g_Uart3RecvBufStruct = {0, 0};//uart3 buffer

UART_InitTypeDef  UART_BT_InitStruct;

/*******************************************************************************
* Function Name  : UART_ISR
* Description    : UART中断处理
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
static void UART_ISR(UART_TypeDef *UARTx, UartRecvBufStruct *UARTxRecvBufStruct)
{
	if((UARTx->SCISR1 & SCISR1_RDRF_MASK) == SCISR1_RDRF_MASK)
	{
		UARTxRecvBufStruct->dat[UARTxRecvBufStruct->wp] = UARTx->SCIDRL;
		UARTxRecvBufStruct->wp += 1;
		if (UARTxRecvBufStruct->wp >= UART_RECV_MAX_LEN)
			UARTxRecvBufStruct->wp = 0;
	}
}

/*******************************************************************************
* Function Name  : UART3_ISR
* Description    : UART2中断处理
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
//void SCI1_IRQHandler(void)
//{
//	UART_ISR(SCI1, &g_Uart1RecvBufStruct);
//}

/*******************************************************************************
* Function Name  : UART4_ISR
* Description    : UART4中断处理
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void SCI2_IRQHandler(void)
{
	UART_ISR(SCI2, &g_Uart2RecvBufStruct);
}

/*******************************************************************************
* Function Name  : UART2_ISR
* Description    : UART2中断处理
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
//void SCI3_IRQHandler(void)
//{
//	UART_ISR(SCI3, &g_Uart3RecvBufStruct);
//}


/*******************************************************************************
* Function Name  : UART_Init
* Description    : UART初始化
* Input          : - UARTx: 取值SCI2 or SCI_BT
*                  - UART_InitStruct：UARTx  initialization parameter structure Poniter
*
* Output         : None
* Return         : None
******************************************************************************/
void UART_Init(UART_TypeDef *UARTx, UART_InitTypeDef *UART_InitStruct)
{
	UINT16 bandrate_I;
	UINT8 bandrate_F;
	UINT8 bandrate_h;
	UINT8 bandrate_l;


	if (SCI1 == UARTx)
	{
		g_Uart1RecvBufStruct.wp = 0;
		g_Uart1RecvBufStruct.rp = 0;
		if (UART_InitStruct->UART_Mode == UART_INT_MODE)
		{
			NVIC_Init(3, 3, SCI1_IRQn, 2);
		}
	}
	else 	if (SCI2 == UARTx)
	{
		g_Uart2RecvBufStruct.wp = 0;
		g_Uart2RecvBufStruct.rp = 0;
		if (UART_InitStruct->UART_Mode == UART_INT_MODE)
		{
			NVIC_Init(3, 3, SCI2_IRQn, 2);
		}
	}
	else
	{
		g_Uart3RecvBufStruct.wp = 0;
		g_Uart3RecvBufStruct.rp = 0;
		if (UART_InitStruct->UART_Mode == UART_INT_MODE)
		{
			NVIC_Init(3, 3, SCI3_IRQn, 2);
		}
	}

	bandrate_I = ((UINT16)(g_ips_clk*4/UART_InitStruct->UART_BaudRate))>>6;
	bandrate_F = (((UINT16)(g_ips_clk*8/UART_InitStruct->UART_BaudRate)+1)/2)&0x003f;
	bandrate_h = (UINT8)((bandrate_I>>8)&0x00ff);
	bandrate_l = (UINT8)(bandrate_I&0x00ff);

	UARTx->SCIBRDF = bandrate_F;		//Write float before Interger
	UARTx->SCIBDH = bandrate_h;
	UARTx->SCIBDL = bandrate_l;

	UARTx->SCICR1 = 0x00;
	UARTx->SCICR1 |= UART_InitStruct->UART_FrameLength; //11 bit frame
	if (UART_InitStruct->UART_Parity == UART_PARITY_NONE)
	{
		UARTx->SCICR1 |= ParityDIS;	//parity disable
	}
	else
	{
		UARTx->SCICR1 |= ParityEN;	//parity disable
		UARTx->SCICR1 |= UART_InitStruct->UART_Parity;//odd parity
	}


	UARTx->SCICR2 = 0;

	if (UART_InitStruct->UART_Mode == UART_INT_MODE)
	{
		//Setup SCI interrupt
		UARTx->SCICR2 |= SCICR2_RIE_MASK|SCICR2_RE_MASK;
	}
}

/*******************************************************************************
* Function Name  : UART_ByteRecieved
* Description    : UART接收到数据
* Input          : - UARTx:取值SCI1、 SCI2
*
* Output         : - None
*
* Return         : - TRUE:接收到数据
*                  - FALSE:没有接收到数据
******************************************************************************/
INT8 UART_ByteRecieved(UART_TypeDef *UARTx)
{
	UartRecvBufStruct *UARTxRecvBufStruct;
	if (UARTx == SCI1)
	{
		 UARTxRecvBufStruct = &g_Uart1RecvBufStruct;
	}
	else
  {
			if (UARTx == SCI2)
	    {
		     UARTxRecvBufStruct = &g_Uart2RecvBufStruct;
	    }
			else
      {
		      UARTxRecvBufStruct = &g_Uart3RecvBufStruct;
      }
	}

	if (UARTxRecvBufStruct->wp != UARTxRecvBufStruct->rp)
	{
		return TRUE;
	}
	return FALSE;
}
/*******************************************************************************
* Function Name  : UART_RecvByte
* Description    : UART接收一个字节
* Input          : - UARTx:取值SCI2 or SCI1
*
*
* Output         : - dat：接收数据缓冲
*
* Return         : - TRUE:接收到数据
*                  - FALSE:没有接收到数据
******************************************************************************/
INT8 UART_RecvByte(UART_TypeDef *UARTx, UINT8 *dat)
{
	UartRecvBufStruct *UARTxRecvBufStruct;
	if (UARTx == SCI1)
	{
		 UARTxRecvBufStruct = &g_Uart1RecvBufStruct;
	}
	else
  {
			if (UARTx == SCI2)
	    {
		     UARTxRecvBufStruct = &g_Uart2RecvBufStruct;
	    }
			else
      {
		      UARTxRecvBufStruct = &g_Uart3RecvBufStruct;
      }
	}
	if (UARTxRecvBufStruct->wp != UARTxRecvBufStruct->rp)
	{
		*dat = UARTxRecvBufStruct->dat[UARTxRecvBufStruct->rp];
		UARTxRecvBufStruct->rp += 1;
		if (UARTxRecvBufStruct->rp >= UART_RECV_MAX_LEN)
			UARTxRecvBufStruct->rp = 0;
		return TRUE;
	}
	return FALSE;
}

/*******************************************************************************
* Function Name  : UART_SendByte
* Description    : UART发送一个字节
* Input          : - UARTx: 取值SCI2 or SCI_BT
*                  - SendByte：发送的字节
*
* Output         : None
* Return         : None
******************************************************************************/
void UART_SendByte(UART_TypeDef *UARTx, UINT8 SendByte)
{

	UARTx->SCICR2 |= SCICR2_TE_MASK;
	while((UARTx->SCISR1 & SCISR1_TDRE_MASK)==0);
	UARTx->SCIDRL = SendByte&0xff;
	while((UARTx->SCISR1 & SCISR1_TC_MASK)==0);
	UARTx->SCICR2 &= ~SCICR2_TE_MASK;
}


/*******************************************************************************
* Function Name  : UART_SendData
* Description    : UART发送多个字节
* Input          : - UARTx:取值SCI2 or SCI_BT
*                  - send：发送数据缓冲
*                  - len：发送的数据长度
*
* Output         : None
* Return         : None
******************************************************************************/
void UART_SendData(UART_TypeDef *UARTx, UINT8 *send, UINT16 len)
{
	UINT16 i = 0;

	UARTx->SCICR2 |= SCICR2_TE_MASK;
	for ( ; i < len; i++)
	{
		while((UARTx->SCISR1 & SCISR1_TDRE_MASK)==0);
		UARTx->SCIDRL = send[i]&0xff;
		while((UARTx->SCISR1 & SCISR1_TC_MASK)==0);
	}
	UARTx->SCICR2 &= ~SCICR2_TE_MASK;
}


/*******************************************************************************
* Function Name  : UART_ConfigGpio
* Description    : UART配置成GPIO用途
* Input          : - UARTx: 取值SCI2 or SCI_BT
*                  - UART_Pin对应的PIN脚，取值UART_TX、UART_RX
*                  - UART_Dir：设置GPIO方向   GPIO_OUTPUT：输出  GPIO_INPUT：输入
*
* Output         : None
* Return         : None
******************************************************************************/
void UART_ConfigGpio(UART_TypeDef *UARTx, UART_PINx UART_Pin, UINT8 UART_Dir)
{
	UARTx->SCICR1 = 0x80;

	if (UART_Dir == GPIO_OUTPUT)
	{
		UARTx->SCIDDR |= (1<<UART_Pin);//output
	}
	else if (UART_Dir == GPIO_INPUT)
	{
		UARTx->SCIDDR &= (~(1<<UART_Pin));//input
	}

}

/*******************************************************************************
* Function Name  : UART_ReadGpioData
* Description    : 获取UART_Pin对应引脚的电平
* Input          : - UARTx: 取值SCI2 or SCI_BT
*                  - UART_Pin对应的PIN脚，取值UART_TX、UART_RX
*
* Output         : None
* Return         : Bit_SET:高电平  Bit_RESET：低电平
******************************************************************************/
INT8 UART_ReadGpioData(UART_TypeDef *UARTx, UART_PINx UART_Pin)
{
	INT8 bitstatus = 0x00;

	bitstatus = UARTx->SCIPORT;

	if (bitstatus &(Bit_SET<<UART_Pin))
		bitstatus = Bit_SET;
	else
		bitstatus = Bit_RESET;

	return bitstatus;
}

/*******************************************************************************
* Function Name  : UART_WriteGpioData
* Description    : 设置UART_Pin对应引脚的电平
* Input          : - UARTx: 取值SCI2 or SCI_BT
*                  - UART_Pin对应的PIN脚，取值UART_TX、UART_RX
*                  - bitVal：设置的电平，Bit_SET：设置为高电平  Bit_RESET：设置为低电平
*
* Output         : None
* Return         : None
******************************************************************************/
void UART_WriteGpioData(UART_TypeDef *UARTx, UART_PINx UART_Pin, UINT8 bitVal)
{
	if (bitVal == Bit_SET)
		UARTx->SCIPORT |= (Bit_SET<<UART_Pin);
	else
		UARTx->SCIPORT &= (~(Bit_SET<<UART_Pin));
}



