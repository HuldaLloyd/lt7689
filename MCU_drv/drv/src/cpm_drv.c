// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : cpm_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "uart.h"
#include "delay.h"
#include "cpm_drv.h"
#include "ccm_drv.h"
#include "iomacros.h"
#include "cache_drv.h"
//#include "apdu_drv.h"
#include "ioctrl_drv.h"
#include "clk_switch_drv.h"
#include "eflash_drv.h"
//#include "test_demo.h"

/*******************************************************************************
* Function Name  : PCI_IRQHandler
* Description    : PCI唤醒休眠中断
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void PCI_IRQHandler(void)
{
	CPM->CPM_PADWKINTCR |= 0x1F; //clear wake up flag;
}

static void CPM_overwrite_test_mode(unsigned int ovwr_data)
{
	CPM_Write_CoreTestKey();
	CPM->CPM_VCCCTMR |= ovwr_data;
	CPM->CPM_VCCCTMR &= 0x3fffffff;
}

/*******************************************************************************
* Function Name  : CPM_Sleep
* Description    : CPU进入Sleep模式
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void CPM_Sleep(void)
{
	#if SUPPER_EDGE_WAKEUP
		CPM->CPM_SLPCFGR = ((CPM->CPM_SLPCFGR & (~0xdc600000))|0x1c600000);  //low power mode,eport 0~4 sleep enable,supper edge wake up
	#else
		CPM->CPM_SLPCFGR &= (~0xdc600000);  //low power mode,eport 0~4 sleep disable,don't supper edge wake up
	#endif
	CPM->CPM_SLPCFGR2 &= ~(VDD_WK_SWOFF|VDD_PD_RETENT|CPM_IPS_SLPEN);
	
	IOCTRL->SWAP_CONTROL_REG |=(1<<2)|(1<<7);    //sdclk/sddat[0:3] swap enable
	
	CPM->CPM_MPDSLPCR = 0X0028DF13;
	
//	CPM->CPM_VCCCTMR = 0X40000000;
//	CPM->CPM_VCCCTMR = 0X80000000;
//	CPM->CPM_VCCCTMR = 0XC0000000;
//	CPM->CPM_VCCCTMR |= (0XC0000000|0X00060000);  //overwrite OSCL/OSCH STABLE TRIM
	CPM_overwrite_test_mode(0x01860000);	//overwrite OSCL STABLE/OSCH STABLE/VCC/CARDLDO TRIM.
	
	CPM->CPM_SLPCNTR = 0X00000080;
	CPM->CPM_WKPCNTR = 0X00000080;
	CPM->CPM_FILTCNTR = 0X00000040;
	
	CPM->CPM_OSCLSTIMER = 0X00000000;
	CPM->CPM_OSCHSTIMER = 0X001001FF;
	
	CPM->CPM_CARDTRIMR = (CPM->CPM_CARDTRIMR | (WKP_DFILT_GTE | WKP_DFILT_EN | WKP_AFILT_BYPASS)) &~ WKP_DFILT_BYPASS;//wake-up digit filter enable, analog filter bypass.
	
	rETIMCFG = (rETIMCFG & 0xFFFFFF0F)|(0X04 << 4); //limit peri_clk_en = 30us
	
#ifdef TSI_DEMO
	CPM->CPM_PADWKINTCR = 0x000303FF; //wake up -pad/disable pad ss3(ss3 与 TSI_CH4复用)/usbdet wake up enable/sddc/cpi_atimer/pci_det wake up disbale
#else
	CPM->CPM_PADWKINTCR = 0x001313FF; //wake up -pad/pad ss3/usbdet wake up enable/sddc/cpi_atimer/pci_det wake up disbale
#endif

	NVIC_Init(3, 3, PCI_IRQn, 2);

	Sys_Standby();
}

/*******************************************************************************
* Function Name  : CPM_PowerOff_1
* Description    : CPU进入PowerOff_1模式
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void CPM_PowerOff_1(void)
{
	#if SUPPER_EDGE_WAKEUP
		CPM->CPM_SLPCFGR = ((CPM->CPM_SLPCFGR & (~0xdc680000))|0x40280000);  //Power OFF 1 mode,eport 0 sleep enable,supper edge wake up
	#else
		CPM->CPM_SLPCFGR = ((CPM->CPM_SLPCFGR & (~0xdc680000))|0x40000000);  //Power OFF 1 mode,eport 0 sleep disable,don't supper edge wake up
	#endif
	
	CPM->CPM_SLPCFGR2 = ((CPM->CPM_SLPCFGR2 &(~(VDD_WK_SWOFF|CPM_IPS_SLPEN)))|VDD_PD_RETENT);
	
	IOCTRL->SWAP_CONTROL_REG |=(1<<2)|(1<<7);   //sdclk/sddat[0:3] swap enable
	
	CPM->CPM_MPDSLPCR = 0X00001000;

//	CPM->CPM_VCCCTMR = 0X40000000;
//	CPM->CPM_VCCCTMR = 0X80000000;
//	CPM->CPM_VCCCTMR = 0XC0000000;
//	CPM->CPM_VCCCTMR |= (0XC0000000|0X00860000);  //overwrite OSCL/OSCH STABLE TRIM
	CPM_overwrite_test_mode(0x01860000);	//overwrite OSCL STABLE/OSCH STABLE/VCC/CARDLDO TRIM.

	CPM->CPM_SLPCNTR = 0x000f0080;
	CPM->CPM_WKPCNTR = 0X00000080;
	CPM->CPM_FILTCNTR = 0x00000040;
	
	CPM->CPM_OSCLSTIMER = 0X00000000;
	CPM->CPM_OSCHSTIMER = 0X001001FF;
	
	CPM->CPM_CARDTRIMR = (CPM->CPM_CARDTRIMR | (WKP_DFILT_GTE | WKP_DFILT_EN | WKP_AFILT_BYPASS)) &~ WKP_DFILT_BYPASS;//wake-up digit filter enable, analog filter bypass.
	
	rETIMCFG = (rETIMCFG & 0xFFFFFF0F)|(0X04 << 4); //limit peri_clk_en = 30us
	
#ifdef TSI_DEMO
	CPM->CPM_PADWKINTCR = 0x000303FF; //wake up -pad/disable pad ss3(ss3 与 TSI_CH4复用)/usbdet wake up enable/sddc/cpi_atimer/pci_det wake up disbale
#else
	CPM->CPM_PADWKINTCR = 0x001313FF; //wake up -pad/pad ss3/usbdet wake up enable/sddc/cpi_atimer/pci_det wake up disbale
#endif	

	CPM->CPM_VCCGTRIMR |= (1<<12) | (1<<13);	//auto latch & auto latch clear.
	
	NVIC_Init(3, 3, PCI_IRQn, 2);
	
	Sys_Standby();
}

/*******************************************************************************
* Function Name  : CPM_PowerOff_1p5
* Description    : CPU进入PowerOff_1p5模式
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void CPM_PowerOff_1p5(void)
{
	#if SUPPER_EDGE_WAKEUP
		CPM->CPM_SLPCFGR = ((CPM->CPM_SLPCFGR & (~0xdc680000))|0x40280000);  //Power OFF 1.5 mode,eport 0 sleep enable,supper edge wake up
	#else
		CPM->CPM_SLPCFGR = ((CPM->CPM_SLPCFGR & (~0xdc680000))|0x40000000);  //Power OFF 1.5 mode,eport 0 sleep disable,don't supper edge wake up
	#endif
//	CPM->CPM_SLPCFGR2 = ((CPM->CPM_SLPCFGR2 &(~VDD_WK_SWOFF))|VDD_PD_RETENT);?!!
	CPM->CPM_SLPCFGR2 = ((CPM->CPM_SLPCFGR2 &(~(VDD_PD_RETENT|CPM_IPS_SLPEN)))|VDD_WK_SWOFF);
	
	IOCTRL->SWAP_CONTROL_REG |=(1<<2)|(1<<7);   //sdclk/sddat[0:3] swap enable
	
	CPM->CPM_MPDSLPCR = 0X00000000;
	
//	CPM->CPM_VCCCTMR = 0X40000000;
//	CPM->CPM_VCCCTMR = 0X80000000;
//	CPM->CPM_VCCCTMR = 0XC0000000;
//	CPM->CPM_VCCCTMR |= (0XC0000000|0X00860000);   //overwrite OSCL/OSCH STABLE TRIM
	CPM_overwrite_test_mode(0x01860000);	//overwrite OSCL STABLE/OSCH STABLE/VCC/CARDLDO TRIM.
	
	CPM->CPM_SLPCNTR = 0x000f0080;
	CPM->CPM_WKPCNTR = 0X00000080;
	CPM->CPM_FILTCNTR = 0x00000040;
	
	CPM->CPM_OSCLSTIMER = 0X00000000;
	CPM->CPM_OSCHSTIMER = 0X001001FF;
	
	CPM->CPM_CARDTRIMR = (CPM->CPM_CARDTRIMR | (WKP_DFILT_GTE | WKP_DFILT_EN | WKP_AFILT_BYPASS)) &~ WKP_DFILT_BYPASS;//wake-up digit filter enable, analog filter bypass.
	
	rETIMCFG = (rETIMCFG & 0xFFFFFF0F)|(0X04 << 4); //limit peri_clk_en = 30us
	
#ifdef TSI_DEMO
	CPM->CPM_PADWKINTCR = 0x000303FF; //wake up -pad/disable pad ss3(ss3 与 TSI_CH4复用)/usbdet wake up enable/sddc/cpi_atimer/pci_det wake up disbale
#else
	CPM->CPM_PADWKINTCR = 0x001313FF; //wake up -pad/pad ss3/usbdet wake up enable/sddc/cpi_atimer/pci_det wake up disbale
#endif	
	
	CPM->CPM_VCCGTRIMR |= (1<<12) | (1<<13);	//auto latch & auto latch clear.
	
	NVIC_Init(3, 3, PCI_IRQn, 2);
	
	Sys_Standby();
}

/*******************************************************************************
* Function Name  : CPM_PowerOff_2
* Description    : CPU进入power off 2模式
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void CPM_PowerOff_2(void)
{
	CPM->CPM_SLPCFGR = ((CPM->CPM_SLPCFGR & (~0Xdc600000))|0x80000000);  //Power OFF 2 mode,eport 1~4 sleep disable,don't supper edge wake up
	
	CPM->CPM_SLPCFGR2 &= ~(VDD_WK_SWOFF|VDD_PD_RETENT|CPM_IPS_SLPEN);
	
	IOCTRL->SWAP_CONTROL_REG |=(1<<2)|(1<<7);          //sdclk/sddat[0:3] swap enable
	
	CPM->CPM_MPDSLPCR = 0X0028df13;
	
//	CPM->CPM_VCCCTMR = 0X40000000;
//	CPM->CPM_VCCCTMR = 0X80000000;
//	CPM->CPM_VCCCTMR = 0XC0000000;
//	CPM->CPM_VCCCTMR |= (0XC0000000|0X00060000);    //overwrite OSCL/OSCH STABLE TRIM
	CPM_overwrite_test_mode(0x01860000);	//overwrite OSCL STABLE/OSCH STABLE/VCC/CARDLDO TRIM.
	
	CPM->CPM_SLPCNTR = 0X00000080;
	CPM->CPM_WKPCNTR = 0X00000080;
	CPM->CPM_FILTCNTR = 0x00000040;
	
	CPM->CPM_OSCLSTIMER = 0X00000000;
	CPM->CPM_OSCHSTIMER = 0X001001FF;
	
	CPM->CPM_CARDTRIMR = (CPM->CPM_CARDTRIMR | (WKP_DFILT_GTE | WKP_DFILT_EN | WKP_AFILT_BYPASS)) &~ WKP_DFILT_BYPASS;//wake-up digit filter enable, analog filter bypass.
	
	rETIMCFG = (rETIMCFG & 0xFFFFFF0F)|(0X04 << 4); //limit peri_clk_en = 30us
	
	CPM->CPM_PADWKINTCR = 0x001313FF; //wake up -pad/pad ss3/usbdet wake up enable/sddc/cpi_atimer/pci_det wake up disbale

	NVIC_Init(3, 3, PCI_IRQn, 2);

	Sys_Standby();
}

/******************************************************************************
* Function Name  : cpm_set_sysclk_div
* Description    : 设置OSC120M分频作为系统时钟
* Input          :  div： the divide value of OSC120M_CLK
*
* Output         : None
* Return         : None
******************************************************************************/
static void cpm_set_sysclk_div(UINT32 div)
{
	CPM->CPM_SCDIVR &= ~(SYS_DIV_MASK);
	CPM->CPM_SCDIVR |= (div << SYS_DIV_SHIFT);
	
	CPM->CPM_CDIVUPDR |= SYSDIV_UPD;
}

/*******************************************************************************
* Function Name  : CPM_Write_CoreTestKey
* Description    : 顺序写入core test key
* Input          : None
*															
* Output         : None
* Return         : None
******************************************************************************/
void CPM_Write_CoreTestKey(void)
{
	unsigned int tmp = (CPM->CPM_VCCCTMR & 0x3FFFFFFF);
	
	CPM->CPM_VCCCTMR = tmp;
	CPM->CPM_VCCCTMR = (tmp|0X40000000);
	CPM->CPM_VCCCTMR = (tmp|0X80000000);
	CPM->CPM_VCCCTMR = (tmp|0XC0000000);
}

/*******************************************************************************
* Function Name  : CPM_VCC5V_Bypass
* Description    : 关闭VCC5V LDO
* Input          : None
*															
* Output         : None
* Return         : None
******************************************************************************/
void CPM_VCC5V_Bypass(void)
{
	CPM_Write_CoreTestKey();
	
	CPM->CPM_VCCCTMR |= (1<<23);
	
	CPM->CPM_VCCGTRIMR &= ~((UINT32)1<<31);  //V33 switch disable
}

/*******************************************************************************
* Function Name  : CPM_Vref_Trim
* Description    : trim参考电压
* Input          : trim_value：b'00 is 1.05V; b'01 is 1.1V; b'10 is 1.15V; b'11 is 1.21V。
*															
* Output         : None
* Return         : None
******************************************************************************/
void CPM_Vref_Trim(unsigned int trim_value)
{
	CPM_Write_CoreTestKey();
	
	CPM->CPM_VCCCTMR |= (1<<23);
	
	if(CPM_VREF_TRIM_090 == trim_value)
	{
		CPM->CPM_VCCGTRIMR = (CPM->CPM_VCCGTRIMR)|(1<<15);
	}
	else
	{
		CPM->CPM_VCCGTRIMR = (CPM->CPM_VCCGTRIMR)& ~(1<<15);
		CPM->CPM_VCCGTRIMR = (CPM->CPM_VCCGTRIMR&(~0x0f))|(trim_value &0x0f);
	}
}

/*******************************************************************************
* Function Name  : CPM_SysClkSelect
* Description    : 设置sys_sel分频作为系统时钟
* Input          :  - sys_sel： 系统主时钟源SYSCLK_SEL_OSC8M, SYSCLK_SEL_OSC160, SYSCLK_SEL_USBPHY or SYSCLK_SEL_OSCEXT
*                   - div：时钟预分频(用户真正希望的分频)
*
* Output         :  - None
* Return         :  - None
******************************************************************************/
void CPM_SysClkSelect(UINT32 sys_sel, UINT32 div)
{
	EFLASH_Set_RWSC(0x07); //配置eflash rwsc 为默认值，防止频率切换时卡死
	
	switch(sys_sel)
	{
		case SYSCLK_OSC8M:
			CPM->CPM_OCSR |= OSC8M_EN;
			while(OSC8M_STABLE != (CPM->CPM_OCSR & OSC8M_STABLE));
		
			CPM->CPM_CSWCFGR &= ~(SYSCLK_SEL_MASK);
			CPM->CPM_CSWCFGR |= SYSCLK_SEL_OSC8M;
			while(CLKSEL_ST_OSC8M != (CPM->CPM_CSWCFGR & CLKSEL_ST_OSC8M));
			break;
		
		case SYSCLK_OSC160M:
			CPM->CPM_OCSR |= OSC160M_EN;
			while(OSC160M_STABLE != (CPM->CPM_OCSR & OSC160M_STABLE));
		
			CPM->CPM_CSWCFGR &= ~(SYSCLK_SEL_MASK);
			CPM->CPM_CSWCFGR |= SYSCLK_SEL_OSC160M;
			while(CLKSEL_ST_OSC160M != (CPM->CPM_CSWCFGR & CLKSEL_ST_OSC160M));
			break;
		
		case SYSCLK_USBPHY240M:
			CPM->CPM_OCSR |= USB_PHY240M_EN;
			while(USB_PHY240M_STABLE != (CPM->CPM_OCSR & USB_PHY240M_STABLE));
		
			CPM->CPM_CSWCFGR &= ~(SYSCLK_SEL_MASK);
			CPM->CPM_CSWCFGR |= SYSCLK_SEL_USBPHY240M;
			while(CLKSEL_ST_USBPHY240M != (CPM->CPM_CSWCFGR & CLKSEL_ST_USBPHY240M));
			break;
				
		case SYSCLK_OSCEXT:
			CPM->CPM_OCSR |= OSCEXT_EN;
			while(OSCEXT_STABLE != (CPM->CPM_OCSR & OSCEXT_STABLE));
		
			CPM->CPM_CSWCFGR &= ~(SYSCLK_SEL_MASK);
			CPM->CPM_CSWCFGR |= SYSCLK_SEL_OSCEXT;
			while(CLKSEL_ST_OSCEXT != (CPM->CPM_CSWCFGR & CLKSEL_ST_OSCEXT));
			break;
		
		default:
			break;
	}
	
	cpm_set_sysclk_div(div);
}


/*******************************************************************************
* Function Name  : ModuleClk_On
* Description    : 打开某个时钟模块
* Input          :  - module： 时钟模块
* Output         : None
* Return         : None
******************************************************************************/
void ModuleClk_On(UINT32 module)
{
	if (module <= 31)
	{
		CPM->CPM_MULTICGTCR |= (1<<module);
	}
	else if (module <= 63)
	{
		CPM->CPM_SYSCGTCR |= (1<<(module-32));
	}
	else if (module <= 95)
	{
		CPM->CPM_IPSCGTCR |= (1<<(module-64));
	}
	else if (module <= 127)
	{
		CPM->CPM_ARITHCGTCR |= (1<<(module-96));
	}
	else
	{
		CPM->CPM_IPSCGTCR |= (1<<(module-128));
	}
}

/*******************************************************************************
* Function Name  : ModuleClk_Off
* Description    : 关闭某个时钟模块
* Input          :  - module： 时钟模块
* Output         : None
* Return         : None
******************************************************************************/
void ModuleClk_Off(UINT32 module)
{
	if (module <= 31)
	{
		CPM->CPM_MULTICGTCR &= ~(1<<module);
	}
	else if (module <= 63)
	{
		CPM->CPM_SYSCGTCR &= ~(1<<(module-32));
	}
	else if (module <= 95)
	{
		CPM->CPM_IPSCGTCR &= ~(1<<(module-64));
	}
	else if (module <= 127)
	{
		CPM->CPM_ARITHCGTCR &= ~(1<<(module-96));
	}
	else
	{
		CPM->CPM_IPSCGTCR &= ~(1<<(module-128));
	}
}


/*******************************************************************************
* Function Name  : get_usb_det_sta
* Description    : 获取USB_DET脚状态
* Input          : None
*
* Output         : None
* Return         : - TRUE：    检测到有USB插入
*                  - FALSE:    检测无USB插入
******************************************************************************/
BOOL get_usb_det_sta(void)
{
	if(CPM->CPM_PADWKINTCR&1)
	{
		CPM->CPM_PADWKINTCR |= 1;//wirte 1  to clear USBDET_STAT
		return TRUE;
	}

	return FALSE;

}

/*******************************************************************************
* Function Name  : Clock_Out_Select
* Description    : clock out管脚输出时钟频率的选择
*
* Input          : clk - SELECT_SYSTEM_CLK，系统时钟
*                        SELECT_ARITH_CLK ，算法时钟
*                        SELECT_EFLASH_CLK，Eflash时钟
*                        SELECT_RTC32K_CLK，外部RTC32K时钟
* Output         : None
* Return         : None
******************************************************************************/
void Clock_Out_Select(unsigned char clk)
{
	CPM->CPM_CSWCFGR &= ~(CLKOUT_SEL_MASK);
	CPM->CPM_CSWCFGR |= (clk << CLKOUT_SEL_SHIFT);
}


/*******************************************************************************
* Function Name  : Get_Sys_Clock
* Description    : 返回当前系统频率，单位Hz
*
* Input          : 无
* Output         : 系统频率
* Return         : None
******************************************************************************/
UINT32 Get_Sys_Clock(void)
{
	UINT8 clk_src;
	UINT32 clk_freq;
	
	clk_src = CPM->CPM_CSWCFGR & SYSCLK_SEL_MASK;
	
	switch((clk_src & 0x3))
	{
		case SYSCLK_OSC8M:
			clk_freq = 8*1000*1000;
			break;
		case SYSCLK_OSC160M:
			if(g_trim_clk == OSC_108M_HZ)
			{
				clk_freq = OSC108M;
			}
			else if(g_trim_clk == OSC_120M_HZ)
			{
				clk_freq = OSC120M;
			}
			else if(g_trim_clk == OSC_150M_HZ)
			{
				clk_freq = OSC150M;
			}
			else
			{
				clk_freq = OSC160M;
			}
			break;
		case SYSCLK_USBPHY240M:
			clk_freq = 240*1000*1000;
			break;
		case SYSCLK_OSCEXT:
			clk_freq = 12*1000*1000;
			break;
		default:
			clk_freq = 0;
			break;
	}
	
	return ((UINT32)(clk_freq / (((CPM->CPM_SCDIVR & SYS_DIV_MASK) >> SYS_DIV_SHIFT) + 1)));
}


/*******************************************************************************
* Function Name  : ARITH_Clk_Op
* Description    : ARITH模块时钟操作
*
* Input          : -div_op：
*                      DIVIDER_DISABLE：禁止分频
*                      UPDATA_DIVIDER：更新分频系数
*                      GET_NOW_DIVIDER：获取当前分频系数
*                  -op_data:
*                      操作参数
* Output         : 当前分频系数
* Return         : None
******************************************************************************/
UINT32 ARITH_Clk_Op(UINT32 div_op, UINT32 op_data)
{
	UINT32 return_val;
	
	return_val = 0;
	
	switch(div_op)
	{
		case DIVIDER_DISABLE:
			CPM->CPM_CDIVENR &= ~(ARITH_DIVEN);
			break;
		case UPDATA_DIVIDER:
			CPM->CPM_CDIVENR |= ARITH_DIVEN;
			CPM->CPM_PCDIVR1 &= ~(ARITH_DIV_MASK);
			CPM->CPM_PCDIVR1 |= (op_data << ARITH_DIV_SHIFT);
			CPM->CPM_CDIVUPDR |= PERDIV_UPD|SYSDIV_UPD;
			break;
		case GET_NOW_DIVIDER:
			return_val = ((CPM->CPM_PCDIVR1 & ARITH_DIV_MASK) >> ARITH_DIV_SHIFT);
			break;
		case GET_NOW_CLKGATE:
			return_val = (CPM->CPM_ARITHCGTCR);
			break;
		case CLKGATE_RESTORE:
			CPM->CPM_ARITHCGTCR = op_data;
			break;
		default:
			break;
	}
	
	return return_val;
}


/*******************************************************************************
* Function Name  : IPS_Clk_Op
* Description    : IPS模块时钟操作
*
* Input          : -div_op：
*                      DIVIDER_DISABLE：禁止分频
*                      UPDATA_DIVIDER：更新分频系数
*                      GET_NOW_DIVIDER：获取当前分频系数
*                  -op_data:
*                      操作参数
* Output         : 当前分频系数
* Return         : None
******************************************************************************/
UINT32 IPS_Clk_Op(UINT32 div_op, UINT32 op_data)
{
	UINT32 return_val;
	
	return_val = 0;
	
	if(op_data == 0)
	{
		op_data = 1;
	}
	
	switch(div_op)
	{
		case DIVIDER_DISABLE:
			CPM->CPM_CDIVENR &= ~(IPS_DIVEN);
			break;
		case UPDATA_DIVIDER:
			CPM->CPM_CDIVENR |= IPS_DIVEN;
			CPM->CPM_PCDIVR1 &= ~(IPS_DIV_MASK);
			CPM->CPM_PCDIVR1 |= (op_data << IPS_DIV_SHIFT);
			CPM->CPM_CDIVUPDR = PERDIV_UPD;
			break;
		case GET_NOW_DIVIDER:
			return_val = ((CPM->CPM_PCDIVR1 & IPS_DIV_MASK) >> IPS_DIV_SHIFT);
			break;
		case GET_NOW_CLKGATE:
			return_val = (CPM->CPM_IPSCGTCR);
			break;
		case CLKGATE_RESTORE:
			CPM->CPM_IPSCGTCR = op_data;
			break;
		default:
			break;
	}
	
	return return_val;
}

/*******************************************************************************
* Function Name  : SDRAM_Clk_Op
* Description    : SDRAM模块时钟操作
*
* Input          : -div_op：
*                      DIVIDER_DISABLE：禁止分频
*                      UPDATA_DIVIDER：更新分频系数
*                      GET_NOW_DIVIDER：获取当前分频系数
*                  -op_data:
*                      操作参数
* Output         : 当前分频系数
* Return         : None
******************************************************************************/
UINT32 SDRAM_Clk_Op(UINT32 div_op, UINT32 op_data)
{
	UINT32 return_val;
	
	return_val = 0;
	
	switch(div_op)
	{
		case DIVIDER_DISABLE:
			CPM->CPM_CDIVENR &= ~(SDRAM_DIVEN);
			break;
		case UPDATA_DIVIDER:
			CPM->CPM_CDIVENR |= SDRAM_DIVEN;
			CPM->CPM_PCDIVR1 &= ~(SDRAM_DIV_MASK);
			CPM->CPM_PCDIVR1 |= (op_data << SDRAM_DIV_SHIFT);
			CPM->CPM_CDIVUPDR = PERDIV_UPD;
			break;
		case GET_NOW_DIVIDER:
			return_val = ((CPM->CPM_PCDIVR1 & SDRAM_DIV_MASK) >> SDRAM_DIV_SHIFT);
			break;
		default:
			break;
	}
	
	return return_val;
}

/*******************************************************************************
* Function Name  : USBC_PHY_Init
* Description    : 初始化USBC PHY时钟
*
* Input          : -OSC_Type:
*                      0:internal oscillator
*                      1:external oscillator
* Output         : None
* Return         : None
******************************************************************************/
void USBC_PHY_Init(UINT8 OSC_Type)
{
	if (1 == OSC_Type)
	{
		//Config 12MHz Clk
		CPM->CPM_OSCESTIMER = 0x3000;
		CPM->CPM_OCSR |= OSCEXT_EN;		//Enable 12MHz Clock
	}
	
	//Enable PHY Power Switch
	CPM->CPM_CHIPCFGR |= PWRCR_PHY_PSWEN_BIT;			//enable usbphy power switch

	//delay at least 10ms, sys clk is 120MHz, in release obj, one clock_cycle is 6 clock
	//so delay(x) = (6 * x / 120)us = 0.05x us, we use 11ms here.so 11ms = (6 * x / 120)=>x=220000	
	DelayMS(11);
	
	//Enable PHY Regulator
	CCM->PHYPA &= ~0xFF;     //Controlled by USBC instead of by manual

	//delay at least 10us
	delay(300);
	CPM->CPM_CHIPCFGR &= ~PWRCR_PHY_I_ISOEN_BIT;		//disable input usbphy isolation
	CPM->CPM_CHIPCFGR &= ~PWRCR_PHY_O_ISOEN_BIT;		//disable output usbphy isolation
	if(1 == OSC_Type)
	{
			 while((CPM->CPM_OCSR & OSCEXT_STABLE )!= OSCEXT_STABLE);         //wait for ext 12mhz to be stable
	}
	DelayMS(10);		//delay at least 10ms
	CPM->CPM_CHIPCFGR &= ~USBPHY_CFG_SRM;
	DelayMS(1);		//delay at least 1ms
	CPM->CPM_CHIPCFGR &= ~USBPHY_PLL_SRM;
	DelayMS(1);			//delay at least 1ms
	CPM->CPM_CHIPCFGR &= ~PWRCR_PHY_RSTMASK_BIT;		//diable usbphy reset mask, release the reset signal
	DelayMS(1);		//delay at least 1ms
}

/*******************************************************************************
* Function Name  : Sys_Clk_Init
* Description    : 初始化系统时钟
*
* Input          : -sys_clk_trim:
*                      0:OSC_108M_HZ,
*								       1:OSC_120M_HZ ,
*								       2:OSC_150M_HZ,
*								       3:OSC_160M_HZ
*                : -sys_clk_div
*                      0:SYS_CLK_DIV_1,
*								       1:SYS_CLK_DIV_2,
*								       ... ...
*                : -ips_clk_div
*                      1:IPS_CLK_DIV_2,
*								       2:SYS_CLK_DIV_3,
*								       ... ...
* Output         : None
* Return         : None
******************************************************************************/
void Sys_Clk_Init(int sys_clk_trim, SYS_CLK_DIV sys_clk_div, IPS_CLK_DIV ips_clk_div)
{
	UINT8 vref_trim_110_flg = 0;
	UINT32 sys_clk_t[4] = {108000000, 120000000, 150000000, 160000000};
	UINT32 old_sys_clk = 0; 
	UINT32 cur_sys_clk = 0;
		
	if ((sys_clk_trim > OSC_160M_HZ) || (sys_clk_trim < 0))
	{	
		g_sys_clk = Get_Sys_Clock();//get system clock	
		g_ips_clk = g_sys_clk /(IPS_Clk_Op(GET_NOW_DIVIDER, 0)  + 1);	//get ips clock		
		return;
	}	
	
	old_sys_clk = Get_Sys_Clock();//get system clock	
	g_trim_clk = sys_clk_trim;
	
	if((sys_clk_trim == OSC_160M_HZ)&&(sys_clk_div == SYS_CLK_DIV_1))
	{
		sys_clk_div = SYS_CLK_DIV_2;
	}

	cur_sys_clk = sys_clk_t[sys_clk_trim]/(sys_clk_div+1);

	if(cur_sys_clk == 150000000)
	{
		if (cur_sys_clk != old_sys_clk)
			CPM_Vref_Trim(CPM_VREF_TRIM_121);
	}
	else if ((old_sys_clk <= 60000000) && (cur_sys_clk > 60000000))
	{
		CPM_Vref_Trim(CPM_VREF_TRIM_110);
	}
	else
	{
		vref_trim_110_flg = 1;//如果从150M切换到大于60M的情况下，需要先切换完时钟然后在切换电压
	}
  
	CPM_OSC_Switch(sys_clk_trim);//trim 160M时钟源
	//配置系统时钟源 ，是取用8M源 还是120M源(最高160M)
	CPM_SysClkSelect(SYSCLK_OSC160M, sys_clk_div); 
	
	//配置IPS时钟的分频系数不可以为0
	IPS_Clk_Op(UPDATA_DIVIDER, ips_clk_div);
	
	//get system clock
	g_sys_clk = Get_Sys_Clock();
	
	//get ips clock
	g_ips_clk = g_sys_clk /(IPS_Clk_Op(GET_NOW_DIVIDER, 0)  + 1);	
	
	if(g_sys_clk <= 60000000)	//系统时钟60M以下，电压trim到0.9V，可以降低功耗
	{
		CPM_Vref_Trim(CPM_VREF_TRIM_090);
	}
	else if (vref_trim_110_flg)
		CPM_Vref_Trim(CPM_VREF_TRIM_110);
}

