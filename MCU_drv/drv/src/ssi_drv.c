// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : ssi_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "delay.h"
#include "uart.h"
#include "common.h"
#include "ssi_drv.h"
#include "dmac_drv.h"
#include "delay.h"

volatile int dma_int =0;

/*******************************************************************************
* Function Name  : SSI_Standard_Init
* Description    : SSI Standard 初始化
* Input          : - SSIx: SSI 基地址
*
* Output         : None
* Return         : None
******************************************************************************/
void SSI_Standard_Init(SSI_TypeDef *SSIx)
{
	while(SSIx->SSI_SR&SR_BUSY){;}

	SSIx->SSI_SSIENR = 0x00;
	SSIx->SSI_CTRLR1 = 0x00;
	SSIx->SSI_CTRLR0 =  0x07;
	SSIx->SSI_BAUDR = 0x06;
	SSIx->SSI_TXFTLR = 0x00;
	SSIx->SSI_RXFTLR = 0x00;
	SSIx->SSI_SPI_CTRLR0 = 0x8000;
	SSIx->SSI_IMR = 0x00;
	SSIx->SSI_SSIENR = 0x01;
}	

/*******************************************************************************
* Function Name  : SSI_EFlash_QPI_Enter
* Description    : SSI EFlash 进入QPI模式
* Input          : - SSIx: SSI 基地址
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_QPI_Enter(SSI_TypeDef *SSIx)
{
	volatile unsigned char status;
	volatile unsigned char temp;
	
	if(SSIx==SSI2)	
	{
	  rSWAPCR|=0x18;
	}	
	
	SSI_Standard_Init(SSIx);
	
	SSIx->SSI_DR=QPI_ENTER_CMD;

	__asm("nop"); __asm("nop"); __asm("nop");
	

	while((SSIx->SSI_SR&SR_TFE)!=SR_TFE){;}
	while(SSIx->SSI_SR&SR_BUSY){;}
		
	while(SSIx->SSI_SR&SR_RFNE)
	{
			temp=SSIx->SSI_DR;
	}
	
	do
	{
		status=SSI_EFlash_QPI_Get_Status1(SSIx);		 
	}while(status&0x01);
	
}

/*******************************************************************************
* Function Name  : SSI_EFlash_QPI_EXIT
* Description    : SSI EFlash 退出QPI模式
* Input          : - SSIx: SSI 基地址
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_QPI_EXIT(SSI_TypeDef *SSIx)
{
	volatile unsigned char status;
	volatile unsigned char temp;
	
	SSI_QPI_Init(SSIx, CMD_WRITE,1,2);
	
	SSIx->SSI_DR=QPI_EXIT_CMD;

	__asm("nop"); __asm("nop"); __asm("nop");
	
	while((SSIx->SSI_SR&SR_TFE)!=SR_TFE){;}
	while(SSIx->SSI_SR&SR_BUSY){;}
	
	SSI_Standard_Init(SSIx);
	do
	{
		status=SSI_EFlash_Get_Status1(SSIx);		 
	}while(status&0x01);
	
}

void SSI_DUAL_Init(SSI_TypeDef *SSIx, UINT32 data_len, UINT8 waitCycles)
{
	SSIx->SSI_SSIENR = 0x00;	 
	SSIx->SSI_BAUDR =  QUAD_BAUDR;
	SSIx->SSI_CTRLR1 = data_len-1;
		
	SSIx->SSI_CTRLR0 = 07;
	SSIx->SSI_SPI_CTRLR0 = 0;
		
	SSIx->SSI_CTRLR0 |= (CTRLR0_SPI_FRF_DUAL|CTRLR0_FRF_MOT|CTRLR0_DFS_VALUE(0x07)|CTRLR0_TMOD_RX_ONLY);
	SSIx->SSI_SPI_CTRLR0|= ( CTRLR0_TRANS_TYPE_TT0
													|CTRLR0_ADDR_L_VALUE(0x06)
													|CTRLR0_INST_L_VALUE(0x02)
													|CTRLR0_WAIT_CYCLES_VALUE(waitCycles)
													|CTRLR0_CLK_STRETCH_EN_MASK);
	SSIx->SSI_TXFTLR = 0x00010000;
	SSIx->SSI_RXFTLR = 0x7;	
	
	if(QUAD_BAUDR==0x02&&g_sys_clk>=80000000)//high speed
	{
	 SSIx->SSI_RX_SAMPLE_DELAY=0x00010001;
	}
	
	SSIx->SSI_IMR = 0x00;
	SSIx->SSI_SSIENR = 0x01;
}

/*******************************************************************************
* Function Name  : SSI_QUAD_Init
* Description    : SSI QUAD 初始化
* Input          : - SSIx      : SSI 基地址
*								 ：- read      ：读写标志
*								 ：- num       ：数据帧
*								 ：- waitCycles：等待时间
* Output         : None
* Return         : None
******************************************************************************/
void SSI_QUAD_Init(SSI_TypeDef *SSIx, int read, int num, int waitCycles)
{
	 while(SSIx->SSI_SR&SR_BUSY){;}
	
   SSIx->SSI_SSIENR = 0x00;
	 SSIx->SSI_CTRLR1 = num;
	 if(read)
	 {
	    SSIx->SSI_CTRLR0 = 0x00800807;
	 }
   else
   {		 
	    SSIx->SSI_CTRLR0 = 0x00800407;
	 }	 
	 SSIx->SSI_BAUDR =  0x08;
	 if(read)
	 {
	    SSIx->SSI_TXFTLR = 0x00010000;
			SSIx->SSI_RXFTLR = 0x07;	 
	 }
   else
   {		 
	    SSIx->SSI_TXFTLR = 0x00000000;
	    SSIx->SSI_RXFTLR = 0x00;
	 }	 
   SSIx->SSI_SPI_CTRLR0 = 0x40000218|(waitCycles<<11);
	 SSIx->SSI_IMR = 0x00;
	 SSIx->SSI_SSIENR = 0x01;
}


/*******************************************************************************
* Function Name  : SSI_QPI_Init
* Description    : SSI QUAD 初始化 (以byte为传输单位)
* Input          : - SSIx      : SSI 基地址
*								 ：- mode      ：执行模式
*								   CMD_READ		 ：发送读指令，无地址位
*								   CMD_WRITE	 ：发送写指令，无地址位不支持inst+addr模式，配置成inst+3B数据，不能配置成inst+addr(addr+data),因为硬件加密时会把address中的数加密，而inst+3B数据没有地址位，不会对3B数据加密
*								   DATA_READ	 ：发送读数据指令，有地址位
*								   DATA_WRITE	 ：发送写数据指令，有地址位
*								 ：- data_len  ：数据帧   (写最大配置为256 byte,读最大配置为64k byte)
*								 ：- waitCycles：等待时间
* Output         : None
* Return         : None
******************************************************************************/
void SSI_QPI_Init(SSI_TypeDef *SSIx, QPI_OPT_MODE mode, UINT32 data_len, UINT8 waitCycles)
{
	SSIx->SSI_SSIENR = 0x00;	 
	SSIx->SSI_BAUDR =  QUAD_BAUDR;
	SSIx->SSI_CTRLR1 = data_len-1;
		
	SSIx->SSI_CTRLR0 = 07;
	SSIx->SSI_SPI_CTRLR0 = 0;
		
	if(mode == CMD_READ)
	{
		SSIx->SSI_CTRLR0 |= (CTRLR0_SPI_FRF_QUAD|CTRLR0_FRF_MOT|CTRLR0_DFS_VALUE(0x07)|CTRLR0_TMOD_RX_ONLY);
		SSIx->SSI_SPI_CTRLR0|= ( CTRLR0_TRANS_TYPE_TT2
														|CTRLR0_ADDR_L_VALUE(0x00)
														|CTRLR0_INST_L_VALUE(0x02)
														|CTRLR0_WAIT_CYCLES_VALUE(waitCycles)
														|CTRLR0_CLK_STRETCH_EN_MASK);
		SSIx->SSI_TXFTLR = 0x00010000;
	  SSIx->SSI_RXFTLR = 0x07;
	}
	else if(mode == CMD_WRITE)
	{
		SSIx->SSI_CTRLR0 |= (CTRLR0_SPI_FRF_QUAD|CTRLR0_FRF_MOT|CTRLR0_DFS_VALUE(0x07)|CTRLR0_TMOD_TX_ONLY);
		SSIx->SSI_SPI_CTRLR0|= ( CTRLR0_TRANS_TYPE_TT2
														|CTRLR0_ADDR_L_VALUE(0x00)
														|CTRLR0_INST_L_VALUE(0x02)
														|CTRLR0_WAIT_CYCLES_VALUE(waitCycles)
														|CTRLR0_CLK_STRETCH_EN_MASK);
		SSIx->SSI_TXFTLR = 0x00;
		SSIx->SSI_RXFTLR = 0x00;
	}
	else if(mode == DATA_READ)
	{
		SSIx->SSI_CTRLR0 |= (CTRLR0_SPI_FRF_QUAD|CTRLR0_FRF_MOT|CTRLR0_DFS_VALUE(0x07)|CTRLR0_TMOD_RX_ONLY);
		SSIx->SSI_SPI_CTRLR0|= ( CTRLR0_TRANS_TYPE_TT2
														|CTRLR0_ADDR_L_VALUE(0x06)
														|CTRLR0_INST_L_VALUE(0x02)
														|CTRLR0_WAIT_CYCLES_VALUE(waitCycles)
														|CTRLR0_CLK_STRETCH_EN_MASK);
		SSIx->SSI_TXFTLR = 0x00010000;
	  SSIx->SSI_RXFTLR = 0x07;
	}
	else //DATA_WRITE
	{
		SSIx->SSI_CTRLR0 |= (CTRLR0_SPI_FRF_QUAD|CTRLR0_FRF_MOT|CTRLR0_DFS_VALUE(0x07)|CTRLR0_TMOD_TX_ONLY);
		SSIx->SSI_SPI_CTRLR0|= ( CTRLR0_TRANS_TYPE_TT2
														|CTRLR0_ADDR_L_VALUE(0x06)
														|CTRLR0_INST_L_VALUE(0x02)
														|CTRLR0_WAIT_CYCLES_VALUE(waitCycles)
														|CTRLR0_CLK_STRETCH_EN_MASK);
		SSIx->SSI_TXFTLR = 0x00020000;
		SSIx->SSI_RXFTLR = 0x00;
		SSIx->SSI_DMATDLR = 0x02;	
	}

	
	if(QUAD_BAUDR==0x02&&g_sys_clk>=80000000)//high speed
	{
	 SSIx->SSI_RX_SAMPLE_DELAY=0x00010001;
	}
	
	SSIx->SSI_IMR = 0x00;
	SSIx->SSI_SSIENR = 0x01;
}

 /*******************************************************************************
* Function Name  : SSI_QUAD_Burst_Init
* Description    : SSI QUAD 初始化  (以byte为传输单位，Burst传输,传输长度需要是4的倍数）
* Input          : - SSIx      : SSI 基地址
*								 ：- read      ：读写标志 1:read  0:write
*								 ：- num       ：数据帧   (写最大配置为256 byte,读最大配置为64k byte)
*								 ：- waitCycles：等待时间
* Output         : None
* Return         : None
******************************************************************************/
void SSI_QUAD_Burst_Init(SSI_TypeDef *SSIx, int read, int num, int waitCycles)
{
	 while(SSIx->SSI_SR&SR_BUSY){;}
	
   SSIx->SSI_SSIENR = 0x00;
	 SSIx->SSI_CTRLR1 = num-1;
	 if(read)
	 {
	    SSIx->SSI_CTRLR0 = 0x00800807;
	 }
   else
   {		 
	    SSIx->SSI_CTRLR0 = 0x00800407;
	 }	 
	 SSIx->SSI_BAUDR =  QUAD_BAUDR;
	 if(read)
	 {
	    SSIx->SSI_TXFTLR = 0x00010000;
	    SSIx->SSI_RXFTLR = 0x07;	 
		  SSIx->SSI_DMARDLR= 0x03;		
	 }
   else
   {		 
	    SSIx->SSI_TXFTLR = 0x00020000;
	    SSIx->SSI_RXFTLR = 0x00;
	 }	 
	 if(QUAD_BAUDR==0x02&&g_sys_clk>=80000000)//high speed
	 {
		 SSIx->SSI_RX_SAMPLE_DELAY=0x00010000;
	 }
   SSIx->SSI_SPI_CTRLR0 = 0x40000218|(waitCycles<<11);
	 SSIx->SSI_IMR = 0x00;
	 SSIx->SSI_SSIENR = 0x01;
}


/*******************************************************************************
* Function Name  : SSI_DMA_ISR
* Description    : SSI 中断响应函数
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void SSI_DMA_ISR(void)
{
	if(m_dma_control->DMA_STATTFR&0x01) dma_int=1;
	if(m_dma_control->DMA_STATTFR&0x02) dma_int=2;
//	if(m_dma_control->DMA_STATTFR&0x04) dma_int=3;
//	if(m_dma_control->DMA_STATTFR&0x08) dma_int=4;

//	printf("=================DMA Channel %d ISR=============channel\r\n\r\n",dma_int-1);
//	printf("TFR: 0x%x\r\n",m_dma_control->DMA_STATTFR);
//	printf("BLOCK: 0x%x\r\n",m_dma_control->DMA_STATBLOCK);
//	printf("SRC: 0x%x\r\n",m_dma_control->DMA_STATSRC);
//	printf("DST: 0x%x\r\n",m_dma_control->DMA_STATDST);
//	printf("ERR: 0x%x\r\n",m_dma_control->DMA_STATERR);
//		
//	printf("Total int: 0x%x\r\n\r\n",m_dma_control->DMA_STATUSINT);

//	m_dma_control->DMA_CLRTFR = m_dma_control->DMA_STATTFR;
//	m_dma_control->DMA_CLRBLOCK=m_dma_control->DMA_STATBLOCK;
//	m_dma_control->DMA_CLRSRC=m_dma_control->DMA_STATSRC;
//	m_dma_control->DMA_CLRDST=m_dma_control->DMA_STATDST;
//	m_dma_control->DMA_CLRERR=m_dma_control->DMA_STATERR;
}


//#ifdef  SSI_DMA_INT
//#if 0
//void DMA_IRQHandler()
//{
//   printf("DMA0 IRQ!\r\n");
//	 isr_dma();
//}
//#endif

//void DMA2_IRQHandler()
//{
//   printf("DMA1 IRQ!\r\n");
//	 isr_dma();
//}
//#endif


/*******************************************************************************
* Function Name  : SSI_EFlash_Get_Status2
* Description    : 获取SSI Flash 状态2 
* Input          : - SSIx: SSI 基地址
* Output         : None
* Return         : None
******************************************************************************/
unsigned char SSI_EFlash_Get_Status2(SSI_TypeDef *SSIx)
{ 
	volatile unsigned char retVal;
		
	SSIx->SSI_DR=GET_SAT2_CMD;
	SSIx->SSI_DR=DUMMY_BYTE;
	
	__asm("nop"); __asm("nop"); __asm("nop");
		
//	while((SSIx->SSI_SR&SR_BUSY)!=SR_BUSY){}
	while((SSIx->SSI_SR&SR_TFE)!=SR_TFE){;}

	while(SSIx->SSI_SR&SR_BUSY){}
	
	 while(SSIx->SSI_SR&SR_RFNE)
	 {
			retVal=SSIx->SSI_DR;
	 }	
	
	 return retVal;
}	

unsigned char SSI_EFlash_QPI_Get_Status2(SSI_TypeDef *SSIx)
{ 
	volatile unsigned char retVal;
	
	SSI_QPI_Init(SSIx, CMD_READ , 1, 0);
		
	SSIx->SSI_DR=GET_SAT2_CMD;
	
	__asm("nop"); __asm("nop"); __asm("nop");
		
//	while((SSIx->SSI_SR&SR_BUSY)!=SR_BUSY){}
	while((SSIx->SSI_SR&SR_TFE)!=SR_TFE){;}

	while(SSIx->SSI_SR&SR_BUSY){}
	
	 while(SSIx->SSI_SR&SR_RFNE)
	 {
			retVal=SSIx->SSI_DR;
	 }	
	
	 return retVal;
}	

/*******************************************************************************
* Function Name  : SSI_EFlash_Prog_Status2
* Description    : 设置SSI Flash 状态2 
* Input          : - SSIx: SSI 基地址
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_Prog_Status2(SSI_TypeDef *SSIx, unsigned char val)
{
	volatile unsigned char status;
	volatile unsigned char retVal;

	SSIx->SSI_DR=PROG_STA2_CMD;
	SSIx->SSI_DR=val;

	__asm("nop"); __asm("nop"); __asm("nop");

//	while((SSIx->SSI_SR&SR_BUSY)==0x00){;}
	while((SSIx->SSI_SR&SR_TFE)!=SR_TFE){;}
	while(SSIx->SSI_SR&SR_BUSY){;}

	while(SSIx->SSI_SR&SR_RFNE)
	{
		retVal=SSIx->SSI_DR;
	}	

	do
	{
	 status=SSI_EFlash_Get_Status1(SSIx);		 
	}while(status&1);
}	


/*******************************************************************************
* Function Name  : SSI_EFlash_Set_Read_Para
* Description    : SSI EFlash 设置读参数
* Input          : - SSIx: SSI 基地址
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_Set_Read_Para(SSI_TypeDef *SSIx)
{
	volatile unsigned char temp;

	SSIx->SSI_SSIENR = 0x00;
	SSIx->SSI_CTRLR1 = 0x00;
	SSIx->SSI_CTRLR0 = 0x00800407;
	SSIx->SSI_SPI_CTRLR0 = 0x40000202|(6<<11);
	SSIx->SSI_BAUDR =  0x10;
	SSIx->SSI_IMR = 0x00;
	SSIx->SSI_SSIENR = 0x01;

	SSIx->SSI_DR=SET_READ_PARA_CMD;
	SSIx->SSI_DR=0x21; //0x21;

	__asm("nop"); __asm("nop"); __asm("nop");
		
//	while((SSIx->SSI_SR&SR_BUSY)==0x00){;}
	while((SSIx->SSI_SR&SR_TFE)!=SR_TFE){;}

	while(SSIx->SSI_SR&SR_BUSY){;}

	while(SSIx->SSI_SR&SR_RFNE)
	{
		temp=SSIx->SSI_DR;
	}		
}		

/*******************************************************************************
* Function Name  : Read_ID_Test
* Description    : 读取SPIFlash ID  
* Input          : - SSIx: SSI 基地址
* Return         : Flash ID
******************************************************************************/
UINT16 Read_ID(SSI_TypeDef *SSIx)
{
	int i;
	volatile unsigned short ID;

	SSIx->SSI_DR=READ_ID_CMD;
	SSIx->SSI_DR = 0x00;
	SSIx->SSI_DR = 0x00;
	SSIx->SSI_DR = 0x00;

	SSIx->SSI_DR=DUMMY_BYTE;
	SSIx->SSI_DR=DUMMY_BYTE;	

	__asm("nop");	__asm("nop");	__asm("nop");

	while((SSIx->SSI_SR&SR_TFE)!=SR_TFE){;}
	while(SSIx->SSI_SR&SR_BUSY){;}

	for(i=0; i<4; i++)
	{
		ID = SSIx->SSI_DR;
	}	 

	ID = SSIx->SSI_DR;
	ID |= SSIx->SSI_DR << 8;
	
	return ID;
}		

/*******************************************************************************
* Function Name  : SSI_EFlash_Read
* Description    : 读SSI Flash 数据  
* Input          : - SSIx: SSI 基地址
*								 ：- addr：数据地址
*								 ：- buf ：数据缓存
*								 ：- num ：数据长度
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_DUAL_Read(SSI_TypeDef *SSIx, unsigned int addr, unsigned char *buf, int num)
{   
	volatile unsigned char temp;
	int i=0;
	
	SSI_DUAL_Init(SSIx,num,8);

	SSIx->SSI_DR = DUAL_READ_CMD;
	SSIx->SSI_DR = addr;
	
	__asm("nop"); __asm("nop"); __asm("nop");
	
	while((SSIx->SSI_SR&SR_TFE)!=SR_TFE){;}

	while( ((num>0)&&(SSIx->SSI_SR&SR_BUSY))||(SSIx->SSI_SR&SR_RFNE) )
	{
		if(SSIx->SSI_SR&SR_RFNE)
		{
				*(buf+i)=SSIx->SSI_DR;
				i++;
				num--;
		}	
	}	
}	

/*******************************************************************************
* Function Name  : SSI_EFlash_Get_Status1
* Description    : 获取SSI Flash 状态1 
* Input          : - SSIx: SSI 基地址
* Output         : None
* Return         : None
******************************************************************************/
unsigned char SSI_EFlash_Get_Status1(SSI_TypeDef *SSIx)
{ 
	volatile unsigned char retVal;

	SSIx->SSI_DR=GET_SAT1_CMD;
	SSIx->SSI_DR=DUMMY_BYTE;	
	
	__asm("nop"); __asm("nop"); __asm("nop");

//	while((SSIx->SSI_SR&SR_BUSY)!=SR_BUSY){;}
	while((SSIx->SSI_SR&SR_TFE)!=SR_TFE){;}

	while(SSIx->SSI_SR&SR_BUSY){;}

	while(SSIx->SSI_SR&SR_RFNE)
	{
		retVal=SSIx->SSI_DR;
	}	

	return retVal;
}	

/*******************************************************************************
* Function Name  : SSI_EFlash_Get_Status1
* Description    : 获取SSI Flash 状态1 
* Input          : - SSIx: SSI 基地址
* Output         : None
* Return         : None
******************************************************************************/
unsigned char SSI_EFlash_QPI_Get_Status1(SSI_TypeDef *SSIx)
{ 
	volatile unsigned char retVal;
	
	SSI_QPI_Init(SSIx, CMD_READ , 1, 2);

	SSIx->SSI_DR=GET_SAT1_CMD;	
	
	__asm("nop"); __asm("nop"); __asm("nop");

//	while((SSIx->SSI_SR&SR_BUSY)!=SR_BUSY){;}
	while((SSIx->SSI_SR&SR_TFE)!=SR_TFE){;}

	while(SSIx->SSI_SR&SR_BUSY){;}

	while(SSIx->SSI_SR&SR_RFNE)
	{
		retVal=SSIx->SSI_DR;
	}	

	return retVal;
}	

/*******************************************************************************
* Function Name  : SSI_EFlash_Write_Enable
* Description    : SSI Flash写使能 
* Input          : - SSIx: SSI 基地址
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_Write_Enable(SSI_TypeDef *SSIx)
{
	volatile unsigned char status;
	volatile unsigned char temp;
	
	if((SSI_EFlash_Get_Status1(SSIx)&0x02) == 0)
	{
		SSIx->SSI_DR=WRITE_EN_CMD;
	
		__asm("nop"); __asm("nop"); __asm("nop");
		
		while((SSIx->SSI_SR&SR_TFE)==0x00){;}
			
		while(SSIx->SSI_SR&SR_BUSY){;}
		
		while(SSIx->SSI_SR&SR_RFNE)
		{
				temp=SSIx->SSI_DR;//清空fifo
		}	
		
		do
		{
			status=SSI_EFlash_Get_Status1(SSIx);		 
		}while(status&0x01);
	}
}	

/*******************************************************************************
* Function Name  : SSI_EFlash_QPI_Write_Enable
* Description    : SSI Flash写使能 
* Input          : - SSIx: SSI 基地址
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_QPI_Write_Enable(SSI_TypeDef *SSIx)
{
	volatile unsigned char status;
	volatile unsigned char temp;
	
	if((SSI_EFlash_QPI_Get_Status1(SSIx)&0x02) == 0)
	{
		SSI_QPI_Init(SSIx, CMD_WRITE , 1, 2);
		
		SSIx->SSI_DR=WRITE_EN_CMD;
	
		__asm("nop"); __asm("nop"); __asm("nop");
		
		while((SSIx->SSI_SR&SR_TFE)==0x00){;}
			
		while(SSIx->SSI_SR&SR_BUSY){;}
		
		do
		{
			status=SSI_EFlash_QPI_Get_Status1(SSIx);		 
		}while(status&0x01);
	}
}	

/*******************************************************************************
* Function Name  : SSI_EFlash_Sector_Erase
* Description    : 擦除SSI Flash 
* Input          : - SSIx: SSI 基地址
* Input          : - addr: 擦除地址
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_Sector_Erase(SSI_TypeDef *SSIx, unsigned int addr)
{
	volatile unsigned char status;
	unsigned char temp;
	
	temp = temp;
	
	SSI_EFlash_Write_Enable(SSIx);

	SSIx->SSI_DR=SECT_ERASE_CMD;
	SSIx->SSI_DR=(addr>>16)&0xff;
	SSIx->SSI_DR=(addr>>8)&0xff;
	SSIx->SSI_DR=(addr>>0)&0xff;
	
	__asm("nop"); __asm("nop"); __asm("nop");
	
	while((SSIx->SSI_SR&SR_TFE)==0x00){;}
		
	while(SSIx->SSI_SR&SR_BUSY){;}

	while(SSIx->SSI_SR&SR_RFNE)
	{
			temp=SSIx->SSI_DR;//清空fifo
	}

	do
	{
	 status=SSI_EFlash_Get_Status1(SSIx);
	 //printf("status: 0x%02x\r\n", status);
	}while(status&0x01);
	 
}

/*******************************************************************************
* Function Name  : SSI_EFlash_QPI_Sector_Erase
* Description    : 擦除SSI Flash 
* Input          : - SSIx: SSI 基地址
* Input          : - addr: 擦除地址
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_QPI_Sector_Erase(SSI_TypeDef *SSIx, unsigned int addr)
{
	volatile unsigned char status;
	unsigned char temp;
	
	temp = temp;
	
	SSI_EFlash_QPI_Write_Enable(SSIx);
	
	SSI_QPI_Init(SSIx, CMD_WRITE , 3, 2);  

	SSIx->SSI_DR=SECT_ERASE_CMD;
	SSIx->SSI_DR=(addr>>16)&0xFF;
	SSIx->SSI_DR=(addr>>8)&0xFF;
	SSIx->SSI_DR=addr&0xFF;
	
	__asm("nop"); __asm("nop"); __asm("nop");
	
	while((SSIx->SSI_SR&SR_TFE)==0x00){;}
		
	while(SSIx->SSI_SR&SR_BUSY){;}

	do
	{
	 status=SSI_EFlash_QPI_Get_Status1(SSIx);
	 //printf("status: 0x%02x\r\n", status);
	}while(status&0x01);
	
	while(SSIx->SSI_SR&SR_BUSY){;}
	 
}

/*******************************************************************************
* Function Name  : SSI_EFlash_Program
* Description    : SSI Flash 写数据  
* Input          : - SSIx: SSI 基地址
*								 ：- addr：数据地址
*								 ：- buf ：数据缓存
*								 ：- num ：数据长度
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_Program(SSI_TypeDef *SSIx, unsigned int addr, unsigned char *buf, int num)
{
	volatile unsigned char temp;
	volatile unsigned char status;
	
	SSI_EFlash_Write_Enable(SSIx);	

	SSIx->SSI_DR=PAGE_PROG_CMD;
	SSIx->SSI_DR=(addr>>16)&0xff;
	SSIx->SSI_DR=(addr>>8)&0xff;
	SSIx->SSI_DR=(addr>>0)&0xff;
	 
#ifdef INTERRUPT_TEST
	 SSIx->SSI_IMR=0x01;
#endif	 

	while(num>0)
	{	
	if(SSIx->SSI_SR&SR_TFNF)
	{
		 SSIx->SSI_DR=*buf;
		 buf++;
		 num--; 
	}	

	if(SSIx->SSI_SR&SR_RFNE)
	{
		 temp=SSIx->SSI_DR;
	}	
	}	


	while(SSIx->SSI_SR&SR_BUSY){;}
	 

	while(SSIx->SSI_SR&SR_RFNE)
	{
		 temp=SSIx->SSI_DR;
	}	
	 
	do
	{
	 status=SSI_EFlash_Get_Status1(SSIx);
	 //printf("[status]-2: 0x%02x\r\n", status);
	}while(status&0x01);
}	

/*******************************************************************************
* Function Name  : SSI_EFlash_Read
* Description    : 读SSI Flash 数据  
* Input          : - SSIx: SSI 基地址
*								 ：- addr：数据地址
*								 ：- buf ：数据缓存
*								 ：- num ：数据长度
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_Read(SSI_TypeDef *SSIx, unsigned int addr, unsigned char *buf, int num)
{   
	volatile unsigned char temp;
	int i=0,j=0;
	int txnum;

	txnum=num;
	SSIx->SSI_DR = READ_CMD;
	SSIx->SSI_DR = (addr>>16)&0xff;
	SSIx->SSI_DR = (addr>>8)&0xff;
	SSIx->SSI_DR = (addr>>0)&0xff;
	
	__asm("nop"); __asm("nop"); __asm("nop");
	
	while(num>0)
	{
		if( (SSIx->SSI_SR&SR_TFNF)&&(txnum>0) )
		{
			SSIx->SSI_DR=DUMMY_BYTE;
			txnum--;
		}	 
			 
		if(SSIx->SSI_SR&SR_RFNE)
		{
			if(j < 4)
			{
				temp=SSIx->SSI_DR;
				j++;
			}
			else
			{
				*(buf+i)=SSIx->SSI_DR;
				 i++;
				 num--;
			}
		}
	}

	while(SSIx->SSI_SR&SR_BUSY){;}
}	

/*******************************************************************************
* Function Name  : SSI_EFlash_QUAD_Program
* Description    : SSI Flash QUAD 方式写数据  
* Input          : - SSIx: SSI 基地址
*								 ：- addr：数据地址
*								 ：- buf ：数据缓存
*								 ：- num ：数据长度
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_QUAD_Program(SSI_TypeDef *SSIx, unsigned int addr, unsigned char *buf, int num)
{
	volatile unsigned char temp;
	volatile unsigned char status;

	SSI_QUAD_Init(SSIx, 0, num-1, 8);


	SSIx->SSI_DR=QUAD_PROG_CMD;
	SSIx->SSI_DR=addr;

//	__asm("nop");
//	__asm("nop");
//	__asm("nop");

//	while((SSIx->SSI_SR&SR_BUSY)==0x00){;}
////	while((SSIx->SSI_SR&SR_TFE)!=SR_TFE){;}
//	while(SSIx->SSI_SR&SR_BUSY){;}


	while(num>0)
	{	
	 if(SSIx->SSI_SR&SR_TFNF)
	 {
		 SSIx->SSI_DR=*buf;
		 buf++;
		 num--; 
	 }	

	 if(SSIx->SSI_SR&SR_RFNE)
	 {
		 temp=SSIx->SSI_DR;
	 }	
	}	

	while(SSIx->SSI_SR&SR_BUSY){;}

	while(SSIx->SSI_SR&SR_RFNE)
	{
		 temp=SSIx->SSI_DR;
	}	

	SSI_Standard_Init(SSIx);
	do
	{
	 status=SSI_EFlash_Get_Status1(SSIx);
	 //printf("[status]-2: 0x%02x\r\n", status);
	}while(status&0x01);	
}

/*******************************************************************************
* Function Name  : SSI_EFlash_QUAD_Read
* Description    : SSI Flash QUAD 方式读数据  
* Input          : - SSIx: SSI 基地址
*								 ：- addr：数据地址
*								 ：- buf ：数据缓存
*								 ：- num ：数据长度
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_QUAD_Read(SSI_TypeDef *SSIx, unsigned int addr, unsigned char *buf, int num)
{
	volatile unsigned char temp;
	volatile unsigned char status;
	int i=0;
//	int dataFlag=0;

	SSI_QUAD_Init(SSIx, 1, num-1, 8);

	SSIx->SSI_DR=QUAD_READ_CMD;
	SSIx->SSI_DR=addr;
	
	__asm("nop");__asm("nop");__asm("nop");
	
//	while((SSIx->SSI_SR&SR_BUSY)!= SR_BUSY){;}
	while((SSIx->SSI_SR&SR_TFE)!=SR_TFE){;}

	while( ((num>0)&&(SSIx->SSI_SR&SR_BUSY))||(SSIx->SSI_SR&SR_RFNE) )
	{
		if(SSIx->SSI_SR&SR_RFNE)
		{
				*(buf+i)=SSIx->SSI_DR;
				i++;
				num--;
		}	
	}	
	
 
//   while(SSIx->SSI_SR&SR_RFNE)
//	 {
//			 *(buf+i)=SSIx->SSI_DR;
//		   i++;
//			 num--;
//	 }		 
}

/*******************************************************************************
* Function Name  : SSI_EFlash_QPI_Program
* Description    : SSI Flash QUAD 方式写数据  
* Input          : - SSIx: SSI 基地址
*								 ：- addr：数据地址
*								 ：- buf ：数据缓存
*								 ：- num ：数据长度
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_QPI_Program(SSI_TypeDef *SSIx, unsigned int addr, unsigned char *buf, int num)
{
	volatile unsigned char temp;
	volatile unsigned char status;
	
	SSI_EFlash_QPI_Write_Enable(SSIx);
	SSI_QPI_Init(SSIx, DATA_WRITE, num, 2);
	
	SSIx->SSI_DR=PAGE_PROG_CMD;
	SSIx->SSI_DR=addr;

	while(num>0)
	{	
		if(SSIx->SSI_SR&SR_TFNF)
		{
		 SSIx->SSI_DR=*buf;
		 buf++;
		 num--; 
		}	
	}	

	while(SSIx->SSI_SR&SR_BUSY){;}

	do
	{
	 status=SSI_EFlash_QPI_Get_Status1(SSIx);
	 //printf("[status]-2: 0x%02x\r\n", status);
	}while(status&0x01);	
}

/*******************************************************************************
* Function Name  : SSI_EFlash_QPI_Read
* Description    : SSI Flash QUAD 方式读数据  
* Input          : - SSIx: SSI 基地址
*								 ：- addr：数据地址
*								 ：- buf ：数据缓存
*								 ：- num ：数据长度
* Output         : None
* Return         : None
******************************************************************************/
void SSI_EFlash_QPI_Read(SSI_TypeDef *SSIx, unsigned int addr, unsigned char *buf, int num)
{
//	volatile unsigned char temp;
//	volatile unsigned char status;
	int i=0;
	
	SSI_QPI_Init(SSIx, DATA_READ, num, 2);

	SSIx->SSI_DR=QPI_READ_CMD;
	SSIx->SSI_DR=addr;
	
	__asm("nop"); __asm("nop"); __asm("nop");

	while((SSIx->SSI_SR&SR_TFE)!=SR_TFE){;}

	while( ((num>0)&&(SSIx->SSI_SR&SR_BUSY))||(SSIx->SSI_SR&SR_RFNE) )
	{
		if(SSIx->SSI_SR&SR_RFNE)
		{
				*(buf+i)=SSIx->SSI_DR;
				i++;
				num--;
		}	
	}		 
}

/*******************************************************************************
* Function Name  : SSI_STD_DMA_Trig
* Description    : 启动SSI STD DMA  
* Input          : - SSIx   : SSI 基地址
*								 ：- cmd    ：操作指令
*								 ：- addr   ：地址
*								 ：- dmaConf：传输方式
* Output         : None
* Return         : None
******************************************************************************/
void SSI_STD_DMA_Trig(SSI_TypeDef *SSIx, unsigned cmd, unsigned int addr, int dmaConf)
{	
	SSIx->SSI_DR=cmd;
	SSIx->SSI_DR=(addr>>16)&0xff;
	SSIx->SSI_DR=(addr>>8)&0xff;
	SSIx->SSI_DR=(addr>>0)&0xff;

	SSIx->SSI_DMACR=dmaConf;
}	

/*******************************************************************************
* Function Name  : SSI_STD_DMA_Send
* Description    : spi dma传送函数
* Input          : - SPIx: SPI 基地址
*                - dma_ch: DMA channel
*				          - psend: 发送数据地址
*                  _ addr: SPI flash地址
*				         - length： 传输数据长度
*				         - binten: 是否开启中断模式
*
* Output         : None
* Return         : None
******************************************************************************/
void SSI_STD_DMA_Send(SSI_TypeDef *SSIx,int dma_ch, UINT8* psend, UINT32 addr, UINT32 length, BOOL binten)
{
//	unsigned char status;
	

	m_dma_control->DMA_CONFIG = 1;
	//Tx
	m_dma_channel[dma_ch]->DMA_SADDR = (UINT32)psend;
	m_dma_channel[dma_ch]->DMA_DADDR = (UINT32)&SSIx->SSI_DR;
	if(binten == TRUE)
	{
		m_dma_channel[dma_ch]->DMA_CTRL = INTEN|DNOCHG|SIEC|M2P_DMA|DWIDTH_B|SWIDTH_B|SBSIZE_4|DBSIZE_4;
	}
	else
	{
		m_dma_channel[dma_ch]->DMA_CTRL = DNOCHG|SIEC|M2P_DMA|DWIDTH_B|SWIDTH_B|SBSIZE_4|DBSIZE_4;
	}
	m_dma_channel[dma_ch]->DMA_CTRL_HIGH = length ;  //最大长度为0x0FFF
	m_dma_channel[dma_ch]->DMA_CFG = (HS_SEL_DST_HARD);
	if(SSIx == SSI1)
	{
		m_dma_channel[dma_ch]->DMA_CFG_HIGH = DST_PER_SPI_TX(3);
	}
	else if(SSIx == SSI2)
	{
		m_dma_channel[dma_ch]->DMA_CFG_HIGH = DST_PER_SPI_TX(2);
	}
	else
	{
		printf("unknown SSIx\r\n");
		while(1){;}
	}
	//enable dma channel
	if(binten == TRUE)
	{	
	   m_dma_control->DMA_MASKTFR = CHANNEL_UMASK(dma_ch);
	}	
	m_dma_control->DMA_CHEN = (CHANNEL_WRITE_ENABLE(dma_ch)|CHANNEL_ENABLE(dma_ch));
	//printf("1m_dma_control->DMA_RAWTFR: 0x%08x\r\n", m_dma_control->DMA_RAWTFR);
	
	SSI_STD_DMA_Trig(SSIx, PAGE_PROG_CMD, addr, DMACR_TDMAE);

	if(binten == TRUE)
	{
		while(1)
		{
			if(dma_int == CHANNEL_STAT(dma_ch) )
			{
				dma_int = 0;
				break;
			}
		}
	}
	else
	{
		//delay(0x1000);
		while((m_dma_control->DMA_RAWTFR & CHANNEL_STAT(dma_ch)) != CHANNEL_STAT(dma_ch));
    //printf("2m_dma_control->DMA_RAWTFR: 0x%08x\r\n", m_dma_control->DMA_RAWTFR);
		
		m_dma_control->DMA_CLRTFR =  m_dma_control->DMA_STATTFR;
		//printf("3m_dma_control->DMA_RAWTFR: 0x%08x\r\n", m_dma_control->DMA_RAWTFR);

	}
	
	
	while(m_dma_control->DMA_CHEN & CHANNEL_STAT(dma_ch)){;}
	m_dma_control->DMA_CHEN = 0;
	m_dma_control->DMA_CONFIG = 0;
	SSIx->SSI_DMACR = 0;

}


/*******************************************************************************
* Function Name  : SSI_STD_DMA_Read
* Description    : spi dma接收函数
* Input          : - SPIx: SPI 基地址
*                - dma_ch: DMA channel
*				          - pread: 发送数据地址
*                  - addr: spi flash地址
*				        - length ： 传输数据长度
*				        - binten : 是否开启中断模式
*
* Output         : None
* Return         : None
******************************************************************************/
void SSI_STD_DMA_Read(SSI_TypeDef *SSIx,int dma_ch, UINT8* pread, UINT32 addr, UINT32 length, BOOL binten)
{
//	unsigned char status;
	int num2;
	
	m_dma_control->DMA_CONFIG = 1;
	//Rx
	m_dma_channel[dma_ch]->DMA_SADDR = (UINT32)&SSIx->SSI_DR;
	m_dma_channel[dma_ch]->DMA_DADDR = (UINT32)pread;
	if(binten == TRUE)
	{
		m_dma_channel[dma_ch]->DMA_CTRL = INTEN|DIEC|SNOCHG|P2M_DMA|DWIDTH_B|SWIDTH_B;
	}
	else
	{
		m_dma_channel[dma_ch]->DMA_CTRL = DIEC|SNOCHG|P2M_DMA|DWIDTH_B|SWIDTH_B;
	}
	m_dma_channel[dma_ch]->DMA_CTRL_HIGH = length+4 ;  //最大长度为0x0FFF
	m_dma_channel[dma_ch]->DMA_CFG = (HS_SEL_SRC_HARD);
	if(SSIx == SSI1)
	{
		m_dma_channel[dma_ch]->DMA_CFG_HIGH = SRC_PER_SPI_RX(2);
	}
	else if(SSIx == SSI2)
	{
		m_dma_channel[dma_ch]->DMA_CFG_HIGH = SRC_PER_SPI_RX(5);
	}
	else
	{
		printf("unknown SSIx\r\n");
		while(1){;}
	}
	//enable dma channel
	if(binten == TRUE)
	{	
	   m_dma_control->DMA_MASKTFR = CHANNEL_UMASK(dma_ch);
	}	
	m_dma_control->DMA_CHEN = (CHANNEL_WRITE_ENABLE(dma_ch)|CHANNEL_ENABLE(dma_ch));
	//printf("1m_dma_control->DMA_RAWTFR: 0x%08x\r\n", m_dma_control->DMA_RAWTFR);
	

	 num2=length;
   SSI_STD_DMA_Trig(SSIx, READ_CMD, addr, DMACR_RDMAE);

   while(num2)
   {	
	   while( (SSIx->SSI_SR&SR_TFNF)&&(num2>0) )
	   {	
			  SSIx->SSI_DR=DUMMY_BYTE;
			  num2--;	
				while(0);
	   }
   }		 
 	 
	 while(SSIx->SSI_SR&SR_BUSY){;}
	 
	 if(binten == TRUE)
	 {
			while(1)
			{
				if(dma_int == CHANNEL_STAT(dma_ch) )
				{
					dma_int = 0;
					break;
				}
			}			
			
	 }	 
	 else
	 {			
		  while((m_dma_control->DMA_RAWTFR & CHANNEL_STAT(dma_ch)) != CHANNEL_STAT(dma_ch));    		
		  m_dma_control->DMA_CLRTFR =  m_dma_control->DMA_STATTFR;
	 }	 

	while(m_dma_control->DMA_CHEN & CHANNEL_STAT(dma_ch)){;}
	m_dma_control->DMA_CHEN = 0;
	m_dma_control->DMA_CONFIG = 0;
	SSIx->SSI_DMACR = 0;
}


/*******************************************************************************
* Function Name  : SSI_QUAD_DMA_Trig
* Description    : 启动SSI QUAD DMA  
* Input          : - SSIx   : SSI 基地址
*								 ：- cmd    ：操作指令
*								 ：- addr   ：地址
*								 ：- dmaConf：传输方式
* Output         : None
* Return         : None
******************************************************************************/
void SSI_DUAL_DMA_Trig(SSI_TypeDef *SSIx, unsigned cmd, unsigned int addr, int dmaConf)
{
	SSIx->SSI_DR=cmd;
	SSIx->SSI_DR=addr;

	SSIx->SSI_DMACR=dmaConf;	 
}	


/*******************************************************************************
* Function Name  : SSI_DUAL_DMA_Read
* Description    : spi dma接收函数
* Input          : - SPIx: SPI 基地址
*                - dma_ch: DMA channel
*				          - pread: 发送数据地址
*                  - addr: spi flash地址
*				        - length ： 传输数据长度
*				        - binten : 是否开启中断模式
*
* Output         : None
* Return         : None
******************************************************************************/
void SSI_DUAL_DMA_Read(SSI_TypeDef *SSIx,int dma_ch, UINT8* pread, UINT32 addr, UINT32 length, BOOL binten)
{
//	unsigned char status;
//	int num2;
//	int j;
	
	m_dma_control->DMA_CONFIG = 1;
	//Rx
	m_dma_channel[dma_ch]->DMA_SADDR = (UINT32)&SSIx->SSI_DR;
	m_dma_channel[dma_ch]->DMA_DADDR = (UINT32)pread;
	if(binten == TRUE)
	{
		m_dma_channel[dma_ch]->DMA_CTRL = INTEN|DIEC|SNOCHG|P2M_DMA|DWIDTH_B|SWIDTH_B;
	}
	else
	{
		m_dma_channel[dma_ch]->DMA_CTRL = DIEC|SNOCHG|P2M_DMA|DWIDTH_B|SWIDTH_B;
	}
	m_dma_channel[dma_ch]->DMA_CTRL_HIGH = length ;  //最大长度为0x0FFF
	m_dma_channel[dma_ch]->DMA_CFG = (HS_SEL_SRC_HARD);
	if(SSIx == SSI1)
	{
		m_dma_channel[dma_ch]->DMA_CFG_HIGH = SRC_PER_SPI_RX(2);
	}
	else if(SSIx == SSI2)
	{
		m_dma_channel[dma_ch]->DMA_CFG_HIGH = SRC_PER_SPI_RX(5);
	}
	else
	{
		printf("unknown SSIx\r\n");
		while(1)
		{
			;
		}	
	}
	//enable dma channel
	if(binten == TRUE)
	{	
	   m_dma_control->DMA_MASKTFR = CHANNEL_UMASK(dma_ch);
	}	
	m_dma_control->DMA_CHEN = CHANNEL_WRITE_ENABLE(dma_ch)|CHANNEL_ENABLE(dma_ch);
	//printf("1m_dma_control->DMA_RAWTFR: 0x%08x\r\n", m_dma_control->DMA_RAWTFR);
	
	SSI_DUAL_DMA_Trig(SSIx, DUAL_READ_CMD, addr, DMACR_RDMAE);
	
	if(binten == TRUE)
	{
			while(1)
			{
				if(dma_int == CHANNEL_STAT(dma_ch) )
				{
					dma_int = 0;
					break;
				}
			}
	}	 
	else
	{			
		while((m_dma_control->DMA_RAWTFR & CHANNEL_STAT(dma_ch)) != CHANNEL_STAT(dma_ch));    		
		m_dma_control->DMA_CLRTFR =  m_dma_control->DMA_STATTFR;
	}	 

	while(m_dma_control->DMA_CHEN & CHANNEL_STAT(dma_ch))
	{
	   ;
	}	
	m_dma_control->DMA_CHEN = 0;
	m_dma_control->DMA_CONFIG = 0;
	SSIx->SSI_DMACR = 0;
}



