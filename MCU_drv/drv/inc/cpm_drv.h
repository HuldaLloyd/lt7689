// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : cpm_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef __CPM_DRV_H__
#define __CPM_DRV_H__

#include "type.h"
#include "memmap.h"
#include "cpm_reg.h"

//系统时钟源
#define OSC108M		108*1000*1000
#define OSC120M		120*1000*1000
#define OSC150M		150*1000*1000
#define OSC160M		160*1000*1000	//必须二分频使用

typedef enum		//系统时钟最高256分频
{
	SYS_CLK_DIV_1 = 0,
	SYS_CLK_DIV_2,
	SYS_CLK_DIV_3,
	SYS_CLK_DIV_4,
	SYS_CLK_DIV_5,
	SYS_CLK_DIV_6,
	SYS_CLK_DIV_7,
	SYS_CLK_DIV_8,
	SYS_CLK_DIV_9,
	SYS_CLK_DIV_10,
	SYS_CLK_DIV_11,
	SYS_CLK_DIV_12,
	SYS_CLK_DIV_13,
	SYS_CLK_DIV_14,
	SYS_CLK_DIV_15,
	SYS_CLK_DIV_16
}SYS_CLK_DIV;

typedef enum		//IPS 时钟最高16分频
{
//	IPS_CLK_DIV_1 = 0,
	IPS_CLK_DIV_2 = 1,
	IPS_CLK_DIV_3,
	IPS_CLK_DIV_4,
	IPS_CLK_DIV_5,
	IPS_CLK_DIV_6,
	IPS_CLK_DIV_7,
	IPS_CLK_DIV_8,
	IPS_CLK_DIV_9,
	IPS_CLK_DIV_10,
	IPS_CLK_DIV_11,
	IPS_CLK_DIV_12,
	IPS_CLK_DIV_13,
	IPS_CLK_DIV_14,
	IPS_CLK_DIV_15,
	IPS_CLK_DIV_16
}IPS_CLK_DIV;

//打开需要trim的时钟


#define CPM                 ((CPM_TypeDef *)(CPM_BASE_ADDR))


#define EFLASH_SYS_CLK_20M		20000000
#define EFLASH_SYS_CLK_30M		30000000
#define EFLASH_SYS_CLK_40M		40000000
#define EFLASH_SYS_CLK_60M		60000000
#define EFLASH_SYS_CLK_80M		80000000
#define EFLASH_SYS_CLK_120M   120000000

#define SUPPER_EDGE_WAKEUP       0

#define CPM_VREF_TRIM_090			0x10		//0.90V 
#define CPM_VREF_TRIM_105			0x00		//1.05V 
#define CPM_VREF_TRIM_110			0x01		//1.10V
#define CPM_VREF_TRIM_115			0x02		//1.15V
#define CPM_VREF_TRIM_121			0x03		//1.21V

#define VDD_PD_RETENT    (1<<0)
#define VDD_WK_SWOFF     (1<<5)
#define	CPM_IPS_SLPEN    (1<<9)
#define WKP_AFILT_BYPASS (1<<15)
#define WKP_DFILT_GTE    (1<<28)
#define WKP_DFILT_BYPASS (1<<29)
#define WKP_DFILT_EN     (1<<30)

typedef enum _SYSCLK_SEL_
{
	SYSCLK_OSC8M = 0,
	SYSCLK_OSC160M,		
	SYSCLK_USBPHY240M,
	SYSCLK_OSCEXT
}SysClkSelect;

#define MODULE_CLK_EFMULP          1                                                                             
#define MODULE_CLK_SDRAM_SM        2                                                                       
#define MODULE_CLK_SDRAM           3                                                                       
#define MODULE_CLK_MCC_MULTI       5                                                                                
#define MODULE_CLK_MCCADR          6                                                                       
#define MODULE_CLK_ADC             7                                                                       
#define MODULE_CLK_CLCD_MULTI      8                                                                             
#define MODULE_CLK_MESH            9                                                                       
#define MODULE_CLK_TC              10                                                                       
#define MODULE_CLK_DMA2D_SRAM      11                                                                       
#define MODULE_CLK_MIPI_SAMPLE     12                                                                       
#define MODULE_CLK_DCMI_PIX        13                                                                       
#define MODULE_CLK_DCMI_SENSOR     14                                                                       
#define MODULE_CLK_CLKOUT          15                                                                       
#define MODULE_CLK_CCM             16                                                                       
#define MODULE_CLK_RIM             17                                                                       
#define MODULE_CLK_EFM_IPS         18                                                                       
#define MODULE_CLK_CPM_IPS         19                                                                       
#define MODULE_CLK_EPORT           20                                                                       
#define MODULE_CLK_EPORT1          21                                                                       
#define MODULE_CLK_EPORT2          22                                                                       
#define MODULE_CLK_EPORT3          23                                                                       
#define MODULE_CLK_EPORT4          24                                                                       
#define MODULE_CLK_SRAM2LCD        25                                                                       
#define MODULE_CLK_TRACE           26 
                                                                      
#define MODULE_CLK_DMAC1           33                                                                             
#define MODULE_CLK_DMAC2           34                                                                       
#define MODULE_CLK_CLCD1           35                                                                       
#define MODULE_CLK_CRC0            36                                                                       
#define MODULE_CLK_CRC1            37                                                                       
#define MODULE_CLK_MIPI            38                                                                       
#define MODULE_CLK_DMA2D           40                                                                             
#define MODULE_CLK_DCM1            41                                                                       
#define MODULE_CLK_PXLP            42                                                                       
#define MODULE_CLK_AHB2_MUX        43                                                                       
#define MODULE_CLK_SRAMD           44                                                                       
#define MODULE_CLK_SRAM0           45                                                                       
#define MODULE_CLK_SRAM1           46                                                                       
#define MODULE_CLK_SRAM2           47                                                                       
#define MODULE_CLK_SRAM3           48 
                                                                      
#define MODULE_CLK_SSI4            64                                                                              
#define MODULE_CLK_SSI5            65                                                                       
#define MODULE_CLK_SSI6            66                                                                       
#define MODULE_CLK_USBC            67                                                                       
#define MODULE_CLK_ROM             68                                                                        
#define MODULE_CLK_AHB3_MUX        69
                                                                       
#define MODULE_CLK_SCB2            96                                                                              
#define MODULE_CLK_AES             97                                                                       
#define MODULE_CLK_SM4             98                                                                       
#define MODULE_CLK_SSF33           99                                                                       
#define MODULE_CLK_RF              100                                                                       
#define MODULE_CLK_CRYPTO          101                                                                       
#define MODULE_CLK_SHA             102                                                                       
#define MODULE_CLK_EDMAC0          103                                                                       
#define MODULE_CLK_DES             104                                                                       
#define MODULE_CLK_ZUC             105
                                                                       
#define MODULE_CLK_IO_CTRL         128                                                                          
#define MODULE_CLK_WDT             129                                                                       
#define MODULE_CLK_RTC             130                                                                       
#define MODULE_CLK_PIT1            131                                                                       
#define MODULE_CLK_PIT2            132                                                                       
#define MODULE_CLK_USI1            133                                                                       
#define MODULE_CLK_EDMAC1          134                                                                       
#define MODULE_CLK_SPI1            135                                                                       
#define MODULE_CLK_SPI2            136                                                                       
#define MODULE_CLK_SPI3            137                                                                       
#define MODULE_CLK_SCI1            138                                                                       
#define MODULE_CLK_SCI2            139                                                                       
#define MODULE_CLK_USI2            140                                                                       
//#define MODULE_CLK_USI3            141                                                                       
#define MODULE_CLK_I2C1            142                                                                       
#define MODULE_CLK_PWM0            143                                                                       
#define MODULE_CLK_I2C2            144                                                                       
#define MODULE_CLK_I2C3            145                                                                       
#define MODULE_CLK_SCI3            146                                                                       
//#define MODULE_CLK_SCI4            147                                                                       
#define MODULE_CLK_QADC            148                                                                       
#define MODULE_CLK_DAC             149                                                                       
#define MODULE_CLK_MCC             150                                                                       
#define MODULE_CLK_TSI             151                                                                       
#define MODULE_CLK_LD              152                                                                       
#define MODULE_CLK_TRNG            153                                                                       
#define MODULE_CLK_PGD             154                                                                       
#define MODULE_CLK_SEC_DET         155                                                                       
#define MODULE_CLK_PCI             156                                                                       
#define MODULE_CLK_PMURTC          157                                                                       
#define MODULE_CLK_AHB2IPS         158  

typedef enum _DIVIDER_TYPE_
{
	SYS_DIVIDER = 0,
	TRACE_DIVIDER,
	CLKOUT_DIVIDER,
	IPS_DIVIDER,
	AHB3_DIVIDER,
	ARITH_DIVIDER,
	SDRAM_DIVIDER,
	SDRAM_SM_DIVIER,
	SDRAM2LCD_DIVIDER,
	MCC_DIVIDER,
	MCCADR_DIVIDER,
	ADC_DIVIDER,
	CLCD_DIVIDER,
	MESH_DIVIDER,
	TC_DIVIDER,
	DMA2D_SRAM_DIVIDER,
	MIPI_SAMPLE_DIVIDER,
	DCMI_PIX_DIVIDER,
	DCMI_SENSOR_DIVIDER
}CLK_DIVIDER;

typedef enum _CLK_DIVIDER_OPERATION_
{
	DIVIDER_DISABLE = 0,
	UPDATA_DIVIDER,
	GET_NOW_DIVIDER,
	GET_NOW_CLKGATE,
	CLKGATE_RESTORE
}CLK_DIV_OP;

/*******************************************************************************
* Function Name  : CPM_Write_CoreTestKey
* Description    : 顺序写入core test key
* Input          : None
*															
* Output         : None
* Return         : None
******************************************************************************/
void CPM_Write_CoreTestKey(void);

/*******************************************************************************
* Function Name  : CPM_VCC5V_Bypass
* Description    : 关闭VCC5V LDO
* Input          : None
*															
* Output         : None
* Return         : None
******************************************************************************/
extern void CPM_VCC5V_Bypass(void);
	
/*******************************************************************************
* Function Name  : CPM_Vref_Trim
* Description    : trim参考电压
* Input          : trim_value：0x10 is 0.90V; b'00 is 1.05V; b'01 is 1.1V; b'10 is 1.15V; b'11 is 1.21V。
*
* Output         : None
* Return         : None
******************************************************************************/
extern void CPM_Vref_Trim(unsigned int trim_value);

/*******************************************************************************
* Function Name  : CPM_Sleep
* Description    : CPU进入Sleep模式
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void CPM_Sleep(void);

/*******************************************************************************
* Function Name  : CPM_PowerOff_1
* Description    : CPU进入PowerOff_1模式
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void CPM_PowerOff_1(void);

/*******************************************************************************
* Function Name  : CPM_PowerOff_1p5
* Description    : CPU进入PowerOff_1p5模式
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void CPM_PowerOff_1p5(void);

/*******************************************************************************
* Function Name  : CPM_PowerOff_2
* Description    : CPU进入power off 2模式
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void CPM_PowerOff_2(void);

/*******************************************************************************
* Function Name  : CPM_Card_Sleep
* Description    : 卡模式下CPU进入Sleep模式
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void CPM_Card_Sleep(void);

/*******************************************************************************
* Function Name  : CPM_Card_PowerOff1
* Description    : 卡模式下CPU进入power off 1模式
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void CPM_Card_PowerOff1(void);

/*******************************************************************************
* Function Name  : CPM_Card_PowerOff1p5
* Description    : 卡模式下CPU进入power off 1.5模式
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void CPM_Card_PowerOff1p5(void);

/*******************************************************************************
* Function Name  : CPM_SysClkSelect
* Description    : 设置sys_sel分频作为系统时钟
* Input          :  - sys_sel： 系统主时钟源SYSCLK_SEL_OSC1M, SYSCLK_SEL_OSC160M, SYSCLK_SEL_USBPHY or SYSCLK_SEL_OSCEXT
*                   - div：时钟预分频
*
* Output         : None
* Return         : None
******************************************************************************/
extern void CPM_SysClkSelect(UINT32 sys_sel, UINT32 div);

/*******************************************
*
*  打开某个模块时钟
*
********************************************/
extern void ModuleClk_On(UINT32 module);

/*******************************************
*
*  关闭某个模块时钟
*
*******************************************/
extern void ModuleClk_Off(UINT32 module);

/*******************************************************************************
* Function Name  : get_usb_det_sta
* Description    : 获取USB_DET脚状态
* Input          : None
*
* Output         : None
* Return         : - TRUE：           检测到有USB插入
*                  - FALSE:    检测无USB插入
******************************************************************************/
extern BOOL get_usb_det_sta(void);

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
extern void Clock_Out_Select(unsigned char clk);

/*******************************************************************************
* Function Name  : Get_Sys_Clock
* Description    : 返回当前系统频率，单位Hz
*
* Input          : 无
* Output         : 系统频率
* Return         : None
******************************************************************************/
extern UINT32 Get_Sys_Clock(void);

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
extern UINT32 ARITH_Clk_Op(UINT32 div_op, UINT32 op_data);

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
extern UINT32 SDRAM_Clk_Op(UINT32 div_op, UINT32 op_data);

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
extern UINT32 IPS_Clk_Op(UINT32 div_op, UINT32 op_data);

/*******************************************************************************
* Function Name  : USBC_PHY_Init
* Description    : 初始化USBC PHY时钟
*
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void USBC_PHY_Init(UINT8 osc_type);

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
extern void Sys_Clk_Init(int sys_clk_trim, SYS_CLK_DIV sys_clk_div, IPS_CLK_DIV ips_clk_div);

#endif /* __CPM_DRV_H__ */
