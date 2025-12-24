// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : pci_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "uart.h"
#include "iomacros.h"
#include "pci_drv.h"
#include "cpm_drv.h"
#include "common.h"

/*******************************************************************************
* Function Name  : PCI_Init_Key
* Description    : PCI密钥初始化
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
static void PCI_Init_Key(void)
{
	PCI->PCI_WRITEEN = 0x40000000;
	PCI->PCI_WRITEEN = 0x80000000;
	PCI->PCI_WRITEEN = 0xC0000000;
}

/*******************************************************************************
* Function Name  : Nvram_Init_Key
* Description    : NVram密钥初始化
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
static void Nvram_Init_Key(void)
{
	PCI->PCI_NVRAMKEY = 0x35;
	PCI->PCI_NVRAMKEY = 0x79;
	PCI->PCI_NVRAMKEY = 0xBD;
	PCI->PCI_NVRAMKEY = 0xF1;
}

/*******************************************************************************
* Function Name  : PCI_Clk_Init_Key
* Description    : PCI 时钟密钥初始化
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
static void PCI_Clk_Init_Key(void)
{
	PCI->PCI_CLKCER = 0x12;
	PCI->PCI_CLKCER = 0x15;
	PCI->PCI_CLKCER = 0x35;
	PCI->PCI_CLKCER = 0x24;
}

/*******************************************************************************
* Function Name  : PCI_Init
* Description    : PCI模块初始化
* 				         注意：
* 				         每次上电前要执行该程序，否则无法读写nvram中的数据
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void PCI_Init(UINT8 clk_sel)
{
	//enable register operation
	PCI_Init_Key();
	PCI->PCI_WRITEEN = 0xC000003F;

	//input RTC clock
	PCI->PCI_RTCIER |= 0x01;		//RTC Interface Enable
	while((PCI->PCI_RTCIER & 0x80000000) != 0x80000000);
	PCI->PCI_RTCIER &= ~(1 << 6);
		
	//RTC32K Clock Enable
	PCI->PCI_DETPCR |= 0x80000000;//保证纽扣电池有电
	
	//2019-4-3
	//IRC frequency and current trim config 
	//PCI->PCI_IRCCR = 0x84;	 //必须使能RTC时钟后再trim
	
	if(clk_sel == EXTERNAL_CLK_SEL)
	{
		PCI_Clk_Init_Key();
		PCI->PCI_CLKSR |= 0x00000080;	
	}
}

/*******************************************************************************
* Function Name  : Nvram_Init
* Description    : nvram模块初始化
* 				         注意：
* 				    写NVRAM前，需要使能；读取的时候不需要。
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void Nvram_Init(void)
{
	Nvram_Init_Key();

	PCI->PCI_NVRAMCR |= ((UINT32)1 << 31);
}

/*******************************************************************************
* Function Name  : Nvram_Write
* Description    : NVram写操作
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void Nvram_Write(void)
{
	Nvram_Init();

	IO_WRITE32(SRAM_START_ADDR_M4, 0x12345678);
	IO_WRITE32((SRAM_START_ADDR_M4 + 0x40), 0xfedcba89);
}

/*******************************************************************************
* Function Name  : Nvram_Read
* Description    : NVram读操作
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void Nvram_Read(void)
{
	printf("[SRAM_START_ADDR]: %08x\r\n", *(unsigned int *)SRAM_START_ADDR_M4);
	printf("[SRAM_START_ADDR+0x200]: %08x\r\n", *(unsigned int *)(SRAM_START_ADDR_M4 + 0x40));
}

/*******************************************************************************
* Function Name  : Nvram_Status
* Description    : 判断NVram是否被清除
* Input          : None
*
* Output         : 0 - 没有被清除
*                  1 - 被清除
* Return         : None
******************************************************************************/
int Nvram_Status(void)
{
	unsigned int flag;

	flag = PCI->PCI_DETSLR;
	if((flag & PCI_NVRAM_FLAG) == PCI_NVRAM_FLAG)
	{
		return 1;
	}

	return 0;
}
/*******************************************************************************
* Function Name  : PCI_SDIOInit
* Description    : SDIO初始化，将fail num清零
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void PCI_SDIO_Init(void)
{			
	PCI->PCI_CR &= ~(0x0F << 8);
	
	//清0操作
	PCI->PCI_SDIOCR = 0x000f0000;

}

/*******************************************************************************
* Function Name  : PCI_IntInit
* Description    : 开启PCI的安全中断
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void PCI_Interrupt_Init(void)
{
	//使能PCI中断功能
	PCI->PCI_DETSR |= ((UINT32)0xE6 << 24);
	NVIC_Init(3, 3, PCI_DET_IRQn, 2);
}

/*******************************************************************************
* Function Name  : PCI_SDIO_Static_Set
* Description    : 设置SDIO静态工作方式，不开启的SDIO管脚可以接地，cmp_data值写0即可
* Input          : channel - 开启的SDIO通道号
*                  cmp_data - 比较电压值:0 - 该路SDIO为低电压维持，高电压触发；1 - 该路SDIO为高电压维持，低电压触发
* Output         : None
* Return         : None
******************************************************************************/
void PCI_SDIO_Static_Set(unsigned char channel, unsigned char cmp_data)
{
	//sdio touch set
	if (cmp_data == 0)
	{
		PCI->PCI_SDIOCR &= (~channel);
	}
	else
	{
		PCI->PCI_SDIOCR |= channel;
	}
	//enable
	PCI->PCI_SDIOCR |= (channel << 24);//enable
}

/*******************************************************************************
* Function Name  : PCI_SDIO_Dynamic_Set
* Description    : 设置SDIO动态工作方式
* Input          : channel - 开启的SDIO通道号
* Output         : None
* Return         : None
******************************************************************************/
void PCI_SDIO_Dynamic_Set(unsigned char channel)
{
	PCI->PCI_SDIOCR |= (channel << 20);

	//dynamic seed
	PCI->PCI_SDIOCR |= (0x55 << 8);

	//enasble sdio 0_1
	if((channel & PCI_SDIO_DYNAMIC_CHANNEL_0) == PCI_SDIO_DYNAMIC_CHANNEL_0)
	{
		PCI->PCI_SDIOCR |= (0x03 << 24);
	}
	
	//enasble sdio 2_3
	if((channel & PCI_SDIO_DYNAMIC_CHANNEL_1) == PCI_SDIO_DYNAMIC_CHANNEL_1)
	{
		PCI->PCI_SDIOCR |= (0x03 << 26);
	}

	//enasble sdio 4_5
	if((channel & PCI_SDIO_DYNAMIC_CHANNEL_2) == PCI_SDIO_DYNAMIC_CHANNEL_2)
	{
		PCI->PCI_SDIOCR |= (0x03 << 28);
	}

	//enasble sdio 6_7
	if((channel & PCI_SDIO_DYNAMIC_CHANNEL_3) == PCI_SDIO_DYNAMIC_CHANNEL_3)
	{
		PCI->PCI_SDIOCR |= ((UINT32)0x03 << 30);
	}
}

/*******************************************************************************
* Function Name  : PCI_VD_Set
* Description    : 设置VD
* Input          : VDH_Value:高压检测阈值
*                  VDL_Value:低压检测阈值
* Output         : None
* Return         : None
******************************************************************************/
void PCI_VD_Set(unsigned char VDH_Value, unsigned char VDL_Value)
{
	//set H_VD value
	PCI->PCI_VDCR &= ~(3<<2);
	PCI->PCI_VDCR |= (VDH_Value<<2);

	//set L_VD value
	PCI->PCI_VDCR &= ~(7<<4);
	PCI->PCI_VDCR |= (VDL_Value<<4);

	//enable H_VD and L_VD detect
	PCI->PCI_VDCR |= 0x03;
	PCI->PCI_VDCR |= ((UINT32)1<<31);
}

/*******************************************************************************
* Function Name  : PCI_TD_Set
* Description    : 设置TD
* Input          : TDH_Value:高温检测阈值
*                  TDL_Value:低温检测阈值
*                  TD_Trim  :温度阈值调整
* Output         : None
* Return         : None
******************************************************************************/
void PCI_TD_Set(unsigned char TDH_Value, unsigned char TDL_Value, unsigned char TD_Trim)
{
	//set H_TD value
	PCI->PCI_TDCR &= ~0x3F;
	PCI->PCI_TDCR |= TDH_Value;

	//set L_TD value
	PCI->PCI_TDCR &= ~(0x1F << 8);
	PCI->PCI_TDCR |= (TDL_Value << 8);

	//enable TD detect
	PCI->PCI_TDCR |= ((UINT32)1<<31);
}

/*******************************************************************************
* Function Name  : PCI_Clear_Status
* Description    : 清除PCI安全触发标志位和nvram标志位
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void PCI_Clear_Status(void)
{
	PCI->PCI_DETSLR |= ((0xFF) | (0xFF << 8) | (0x01 << 16));
	PCI->PCI_DETSR |= ((0xFF << 8) | (0xFF << 16));
}

/*******************************************************************************
* Function Name  : PCI_Detect
* Description    : 判断PCI安全触发源
* Input          : None
* Output         : None
* Return         : 0 - 没有触发
*                  1 - 有触发
******************************************************************************/
int PCI_Detect(void)
{
	unsigned int flag;
	int ret_val = 0;

	flag = PCI->PCI_DETSLR;

	//PCI VD
	if((flag & PCI_VD_L_FLAG) == PCI_VD_L_FLAG)
	{
		printf("PCI VD_L touch!\r\n");
		ret_val = 1;
	}

	if((flag & PCI_VD_H_FLAG) == PCI_VD_H_FLAG)
	{
		printf("PCI VD_H touch!\r\n");
		ret_val = 1;
	}

	//PCI TD
	if((flag & PCI_TD_L_FLAG) == PCI_TD_L_FLAG)
	{
		printf("PCI TD_L touch!\r\n");
		ret_val = 1;
	}

	if((flag & PCI_TD_H_FLAG) == PCI_TD_H_FLAG)
	{
		printf("PCI TD_H touch!\r\n");
		ret_val = 1;
	}

	//PCI SDIO
	if((flag & PCI_SDIO_CHANNEL_0) == PCI_SDIO_CHANNEL_0)
	{
		if((PCI->PCI_SDIOCR & 0x100000) == 0x100000)
		{
			printf("SDIO dynamic-0 touch!\r\n");
		}
		else
		{
			printf("SDIO static-0 touch!\r\n");
		}
		ret_val = 1;
	}

	if((flag & PCI_SDIO_CHANNEL_1) == PCI_SDIO_CHANNEL_1)
	{
		printf("SDIO static-1 touch!\r\n");
		ret_val = 1;
	}

	if((flag & PCI_SDIO_CHANNEL_2) == PCI_SDIO_CHANNEL_2)
	{
		if((PCI->PCI_SDIOCR & 0x200000) == 0x200000)
		{
			printf("SDIO dynamic-1 touch!\r\n");
		}
		else
		{
			printf("SDIO static-2 touch!\r\n");
		}
		ret_val = 1;
	}

	if((flag & PCI_SDIO_CHANNEL_3) == PCI_SDIO_CHANNEL_3)
	{
		printf("SDIO static-3 touch!\r\n");
		ret_val = 1;
	}

	if((flag & PCI_SDIO_CHANNEL_4) == PCI_SDIO_CHANNEL_4)
	{
		if((PCI->PCI_SDIOCR & 0x400000) == 0x400000)
		{
			printf("SDIO dynamic-2 touch!\r\n");
		}
		else
		{
			printf("SDIO static-4 touch!\r\n");
		}
		ret_val = 1;
	}

	if((flag & PCI_SDIO_CHANNEL_5) == PCI_SDIO_CHANNEL_5)
	{
		printf("SDIO static-5 touch!\r\n");
		ret_val = 1;
	}

	if((flag & PCI_SDIO_CHANNEL_6) == PCI_SDIO_CHANNEL_6)
	{
		if((PCI->PCI_SDIOCR & 0x800000) == 0x800000)
		{
			printf("SDIO dynamic-3 touch!\r\n");
		}
		else
		{
			printf("SDIO static-6 touch!\r\n");
		}
		ret_val = 1;
	}

	if((flag & PCI_SDIO_CHANNEL_7) == PCI_SDIO_CHANNEL_7)
	{
		printf("SDIO static-7 touch!\r\n");
		ret_val = 1;
	}

	//NVRAM flag
	if((flag & PCI_NVRAM_FLAG) == PCI_NVRAM_FLAG)
	{
		printf("nvram clean!\r\n");
	}

	return ret_val;
}

/*******************************************************************************
* Function Name  : PCI_IRQHandler
* Description    : PCI安全触发中断处理函数
*                  PCI中断功能，在芯片重新上电后，要重新配置。即断电后配置不会保留。
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void PCI_DET_IRQHandler(void)
{
	unsigned int flag;

	flag = PCI->PCI_DETSLR;

	//PCI VD
	if((flag & PCI_VD_L_FLAG) == PCI_VD_L_FLAG)
	{
		printf("PCI VD_L touch!\r\n");
		PCI->PCI_DETSLR |= PCI_VD_L_FLAG;
	}

	if((flag & PCI_VD_H_FLAG) == PCI_VD_H_FLAG)
	{
		printf("PCI VD_H touch!\r\n");
		PCI->PCI_DETSLR |= PCI_VD_H_FLAG;
	}

	//PCI TD
	if((flag & PCI_TD_L_FLAG) == PCI_TD_L_FLAG)
	{
		printf("PCI TD_L touch!\r\n");
		PCI->PCI_DETSLR |= PCI_TD_L_FLAG;
	}

	if((flag & PCI_TD_H_FLAG) == PCI_TD_H_FLAG)
	{
		printf("PCI TD_H touch!\r\n");
		PCI->PCI_DETSLR |= PCI_TD_H_FLAG;
	}

	//PCI SDIO
	if((flag & PCI_SDIO_CHANNEL_0) == PCI_SDIO_CHANNEL_0)
	{
		if((PCI->PCI_SDIOCR & 0x100000) == 0x100000)
		{
			printf("SDIO dynamic-0 touch!\r\n");
		}
		else
		{
			printf("SDIO static-0 touch!\r\n");
		}

		PCI->PCI_DETSLR |= PCI_SDIO_CHANNEL_0;
	}

	if((flag & PCI_SDIO_CHANNEL_1) == PCI_SDIO_CHANNEL_1)
	{
		printf("SDIO static-1 touch!\r\n");

		PCI->PCI_DETSLR |= PCI_SDIO_CHANNEL_1;
	}

	if((flag & PCI_SDIO_CHANNEL_2) == PCI_SDIO_CHANNEL_2)
	{
		if((PCI->PCI_SDIOCR & 0x200000) == 0x200000)
		{
			printf("SDIO dynamic-1 touch!\r\n");
		}
		else
		{
			printf("SDIO static-2 touch!\r\n");
		}

		PCI->PCI_DETSLR |= PCI_SDIO_CHANNEL_2;
	}

	if((flag & PCI_SDIO_CHANNEL_3) == PCI_SDIO_CHANNEL_3)
	{
		printf("SDIO static-3 touch!\r\n");

		PCI->PCI_DETSLR |= PCI_SDIO_CHANNEL_3;
	}

	if((flag & PCI_SDIO_CHANNEL_4) == PCI_SDIO_CHANNEL_4)
	{
		if((PCI->PCI_SDIOCR & 0x400000) == 0x400000)
		{
			printf("SDIO dynamic-2 touch!\r\n");
		}
		else
		{
			printf("SDIO static-4 touch!\r\n");
		}

		PCI->PCI_DETSLR |= PCI_SDIO_CHANNEL_4;
	}

	if((flag & PCI_SDIO_CHANNEL_5) == PCI_SDIO_CHANNEL_5)
	{
		printf("SDIO static-5 touch!\r\n");

		PCI->PCI_DETSLR |= PCI_SDIO_CHANNEL_5;
	}

	if((flag & PCI_SDIO_CHANNEL_6) == PCI_SDIO_CHANNEL_6)
	{
		if((PCI->PCI_SDIOCR & 0x800000) == 0x800000)
		{
			printf("SDIO dynamic-3 touch!\r\n");
		}
		else
		{
			printf("SDIO static-6 touch!\r\n");
		}

		PCI->PCI_DETSLR |= PCI_SDIO_CHANNEL_6;
	}

	if((flag & PCI_SDIO_CHANNEL_7) == PCI_SDIO_CHANNEL_7)
	{
		printf("SDIO static-7 touch!\r\n");

		PCI->PCI_DETSLR |= PCI_SDIO_CHANNEL_7;
	}

	//NVRAM flag
	if((flag & PCI_NVRAM_FLAG) == PCI_NVRAM_FLAG)
	{
		printf("nvram clean!\r\n");
		PCI->PCI_DETSLR |= PCI_NVRAM_FLAG;
	}
	
	PCI->PCI_DETSR |= ((0xFF << 8) | (0xFF << 16));
}

/*******************************************************************************
* Function Name  : PCI_SDIO_PullControl
* Description    : PCI SDIO上下拉功能配置函数
* Input          : channel - 开启的SDIO通道号，例如PCI_SDIO_STATIC_CHANNEL_0
*                  mode    - Pull up or Pull down
* Output         : None
* Return         : None
******************************************************************************/
void PCI_SDIO_PullControl(unsigned char channel, unsigned char mode)
{
	if(mode == SDIO_PULL_UP)
	{
		PCI->PCI_SDIOPCR &= ~(channel << 16);
		PCI->PCI_SDIOPCR |= (channel << 24);
	}
	else
	{
		PCI->PCI_SDIOPCR &= ~(channel << 24);
		PCI->PCI_SDIOPCR |= (channel << 16);
	}

	//enable
	PCI->PCI_SDIOPCR |= 0x01;
}

/*******************************************************************************
* Function Name  : PCI_SDIO_Current_Trim
* Description    : PCI SDIO上下拉功能电流Trim值
* Input          : channel - 开启的SDIO通道号，例如PCI_SDIO_STATIC_CHANNEL_0
*                  mode    - Pull up or Pull down
* Output         : None
* Return         : None
******************************************************************************/
void PCI_SDIO_Current_Trim(unsigned char value)
{
	PCI->PCI_SDIOPCR &= 0xFFFFFFF1;

	PCI->PCI_SDIOPCR |= value << 1;
}


/*******************************************************************************
* Function Name  : SDIO_ConfigGpio
* Description    : SDIO配置成GPIO用途
* Input          : - SDIO_PINx: EPORT Pin；where x can be 0~7 to select the SDIO peripheral.
*                  - GpioDir：设置GPIO方向   GPIO_OUTPUT：输出  GPIO_INPUT：输入
*
* Output         : None
* Return         : 0 - 成功;other - 失败
******************************************************************************/
int SDIO_ConfigGpio(SDIO_PINx GpioNo, UINT8 GpioDir)
{
	//UINT32 temp = 0x01;
	assert_param(IS_PCI_PINx(GpioNo));
	assert_param(IS_GPIO_DIR_BIT(GpioDir));

	if (GpioNo > SDIO_PIN7)
		return -1;

	//使能PCI SDIO作为GPIO功能
	PCI->PCI_SDIOPCR |= (0x01 << 8);

	//PCI SDIO做GPIO的时候，需要配置成为下拉模式
	PCI_SDIO_Current_Trim(SDIO_CURRENT_LEVEL_2);
	PCI_SDIO_PullControl(GpioNo, SDIO_PULL_DOWN);


	if (GpioDir == GPIO_INPUT)//EPORT_EPDDR
	{
		PCI->PCI_SDIOGPIOCR &= ~((1 << GpioNo) << 16);
	}
	else
	{
		PCI->PCI_SDIOGPIOCR |= ((1 << GpioNo) << 16);
	}

	return 0;
}

/*******************************************************************************
* Function Name  : SDIO_WriteGpioData
* Description    : 设置SDIO_PINx对应引脚的电平
* Input          : - SDIO_PINx: SDIO Pin；where x can be 0~7 to select the SDIO peripheral.
*                  - bitVal：设置的电平，Bit_SET：设置为高电平  Bit_RESET：设置为低电平
*
* Output         : None
* Return         : 0: 设置成功    other：设置失败
******************************************************************************/
int SDIO_WriteGpioData(SDIO_PINx GpioNo, UINT8 bitVal)
{

	assert_param(IS_PCI_PINx(GpioNo));
	assert_param(IS_GPIO_BIT_ACTION(bitVal));

	if (GpioNo > SDIO_PIN7)
		return -1;

	PCI->PCI_SDIOGPIOCR |= ((1 << GpioNo) << 16);

	if (bitVal == Bit_SET)
	{
		PCI->PCI_SDIOGPIOCR |= (Bit_SET << GpioNo);
	}
	else
	{
		PCI->PCI_SDIOGPIOCR &= ~(Bit_SET << GpioNo);
	}

	return 0;
}

/*******************************************************************************
* Function Name  : SDIO_ReadGpioData
* Description    : 获取SDIO_PINx对应引脚的电平
* Input          : - SDIO_PINx: SDIO Pin；where x can be 0~7 to select the SDIO peripheral.
*
* Output         : None
* Return         : Bit_SET:高电平  Bit_RESET：低电平  -1：失败
******************************************************************************/
int SDIO_ReadGpioData(SDIO_PINx GpioNo)
{
	int  bitstatus = 0x00;

	assert_param(IS_PCI_PINx(GpioNo));

	if (GpioNo > SDIO_PIN7)
		return -1;

	PCI->PCI_SDIOGPIOCR &= ~((1 << GpioNo) << 16);

	bitstatus = (PCI->PCI_SDIOGPIOCR >> 8);
	if (bitstatus & (Bit_SET << GpioNo))
	{
		return Bit_SET;
	}
	else
	{
		return Bit_RESET;
	}
}

/*******************************************************************************
* Function Name  : PCI_NVSRAM_WtiteByte
* Description    : 按字节向nvram写数据
* Input          : - addr: 写入数据的地址.
*                  - dat：写入的数据
*
* Output         : None
* Return         : other - 成功;0 - 失败
******************************************************************************/
BOOL PCI_NVSRAM_WtiteByte(UINT32 addr, UINT8 dat)
{
	if((addr < SRAM_START_ADDR_M4) || (addr > SRAM_END_ADDR_M4))
		return FALSE;

	IO_WRITE8(addr, dat);

	return TRUE;
}

/*******************************************************************************
* Function Name  : PCI_NVSRAM_ReadByte
* Description    : 按字节读取nvram数据
* Input          : - addr: 数据的地址.
*                  - dat：数据
*
* Output         : None
* Return         : other - 成功;0 - 失败
******************************************************************************/
BOOL PCI_NVSRAM_ReadByte(UINT32 addr, UINT8 *dat)
{
	if((addr < SRAM_START_ADDR_M4) || (addr > SRAM_END_ADDR_M4))
		return FALSE;

	*dat = IO_READ8(addr);

	return TRUE;
}

/*******************************************************************************
* Function Name  : PCI_Open
* Description    : PCI模块Open
*                  PCI_Init的简化版，执行PCI_Open后不能够操作TD、VD、SDIO等寄存器，但是能够读写NVSRAM，读出和清除安全触发标志位
*                  该函数使用的前提是，调用过PCI_Init 
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void PCI_Open(void)
{
	//enable register operation
	PCI_Init_Key();
	PCI->PCI_WRITEEN = 0xC0000020;//*

	PCI->PCI_RTCIER |= 0x01;
	while((PCI->PCI_RTCIER & 0x80000000) != 0x80000000);
	PCI->PCI_RTCIER &= ~(1 << 6);
}

/*******************************************************************************
* Function Name  : PCI_Close
* Description    : PCI模块Close
* 				   关闭PCI模块的读写功能，会消除PCI_Init和PCI_Open函数增加的3uA功耗；
*                  同时，无法正常读取NVSRAM数据，读取PCI的触发标志位。
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void PCI_Close(void)
{
	PCI->PCI_RTCIER &= ~0x01;
}
