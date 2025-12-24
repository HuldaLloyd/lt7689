// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : pci_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __PCI_DRV_H__
#define __PCI_DRV_H__

#include "type.h"
#include "pci_reg.h"
#include "memmap.h"

#define PCI                   ((PCI_TypeDef *)(PCI_BASE_ADDR))

//nvram address
#define SRAM_START_ADDR_M4   (PCI_BASE_ADDR + 0x100)
#define SRAM_END_ADDR_M4     (PCI_BASE_ADDR + 0x180)

typedef enum
{
	SDIO_PIN0 = 0,
	SDIO_PIN1,
	SDIO_PIN2,
	SDIO_PIN3,
	SDIO_PIN4,
	SDIO_PIN5,
	SDIO_PIN6,
	SDIO_PIN7,

}SDIO_PINx;

#define IS_PCI_PINx(PIN) (((PIN) == SDIO_PIN0) || \
		                    ((PIN) == SDIO_PIN1) || \
		                    ((PIN) == SDIO_PIN2) || \
		                    ((PIN) == SDIO_PIN3) || \
		                    ((PIN) == SDIO_PIN4) || \
		                    ((PIN) == SDIO_PIN5) || \
		                    ((PIN) == SDIO_PIN6) || \
                            ((PIN) == SDIO_PIN7))

//VD
#define PCI_VD_H_LEVEL_1         0x03     //3.62
#define PCI_VD_H_LEVEL_2         0x02     //3.70
#define PCI_VD_H_LEVEL_3         0x01     //3.79
#define PCI_VD_H_LEVEL_4         0x00     //3.88

#define PCI_VD_L_LEVEL_1         0x00     //1.42
#define PCI_VD_L_LEVEL_2         0x01     //1.48
#define PCI_VD_L_LEVEL_3         0x02     //1.53
#define PCI_VD_L_LEVEL_4         0x03     //1.59
#define PCI_VD_L_LEVEL_5         0x04     //1.66
#define PCI_VD_L_LEVEL_6         0x05     //1.73
#define PCI_VD_L_LEVEL_7         0x06     //1.81
#define PCI_VD_L_LEVEL_8         0x07     //1.90

//TD
#define PCI_TD_H_LEVEL_1         0x00    //coldest
#define PCI_TD_H_LEVEL_2         0x01    //colder
#define PCI_TD_H_LEVEL_3         0x02    //-39
#define PCI_TD_H_LEVEL_4         0x03    //-35
#define PCI_TD_H_LEVEL_5         0x04    //-31
#define PCI_TD_H_LEVEL_6         0x05    //-27
#define PCI_TD_H_LEVEL_7         0x06    //-23
#define PCI_TD_H_LEVEL_8         0x07    //-19
#define PCI_TD_H_LEVEL_9         0x08    //-15
#define PCI_TD_H_LEVEL_10         0x09    //-11
#define PCI_TD_H_LEVEL_11         0x0a    //-7
#define PCI_TD_H_LEVEL_12         0x0b    //-3
#define PCI_TD_H_LEVEL_13         0x0c    //1
#define PCI_TD_H_LEVEL_14         0x0d    //5
#define PCI_TD_H_LEVEL_15         0x0e    //9
#define PCI_TD_H_LEVEL_16         0x0f    //13
#define PCI_TD_H_LEVEL_17         0x10    //17
#define PCI_TD_H_LEVEL_18         0x11    //21
#define PCI_TD_H_LEVEL_19         0x12    //25
#define PCI_TD_H_LEVEL_20         0x13    //29
#define PCI_TD_H_LEVEL_21         0x14    //33
#define PCI_TD_H_LEVEL_22         0x15    //37
#define PCI_TD_H_LEVEL_23         0x16    //41
#define PCI_TD_H_LEVEL_24         0x17    //45
#define PCI_TD_H_LEVEL_25         0x18    //49
#define PCI_TD_H_LEVEL_26         0x19    //53
#define PCI_TD_H_LEVEL_27         0x1a    //57
#define PCI_TD_H_LEVEL_28         0x1b    //61
#define PCI_TD_H_LEVEL_29         0x1c    //65
#define PCI_TD_H_LEVEL_30         0x1d    //69
#define PCI_TD_H_LEVEL_31         0x1e    //73
#define PCI_TD_H_LEVEL_32         0x1f    //77
#define PCI_TD_H_LEVEL_33         0x20    //81
#define PCI_TD_H_LEVEL_34         0x21    //85
#define PCI_TD_H_LEVEL_35         0x22    //89
#define PCI_TD_H_LEVEL_36         0x23    //93
#define PCI_TD_H_LEVEL_37         0x24    //97
#define PCI_TD_H_LEVEL_38         0x25    //101
#define PCI_TD_H_LEVEL_39         0x26    //105
#define PCI_TD_H_LEVEL_40         0x27    //109
#define PCI_TD_H_LEVEL_41         0x28    //113
#define PCI_TD_H_LEVEL_42         0x29    //117
#define PCI_TD_H_LEVEL_43         0x2a    //121
#define PCI_TD_H_LEVEL_44         0x2b    //125

#define PCI_TD_L_LEVEL_1         0x0a    //-39
#define PCI_TD_L_LEVEL_2         0x0b    //-35
#define PCI_TD_L_LEVEL_3         0x0c    //-31
#define PCI_TD_L_LEVEL_4         0x0d    //-27
#define PCI_TD_L_LEVEL_5         0x0e    //-23
#define PCI_TD_L_LEVEL_6         0x0f    //-19
#define PCI_TD_L_LEVEL_7         0x10    //-15
#define PCI_TD_L_LEVEL_8         0x11    //-11
#define PCI_TD_L_LEVEL_9         0x12    //-7
#define PCI_TD_L_LEVEL_10         0x13    //-3
#define PCI_TD_L_LEVEL_11         0x14    //1
#define PCI_TD_L_LEVEL_12         0x15    //5
#define PCI_TD_L_LEVEL_13         0x16    //9
#define PCI_TD_L_LEVEL_14         0x17    //13
#define PCI_TD_L_LEVEL_15         0x18    //17
#define PCI_TD_L_LEVEL_16         0x19    //21
#define PCI_TD_L_LEVEL_17         0x1a    //25
#define PCI_TD_L_LEVEL_18         0x1b    //29
#define PCI_TD_L_LEVEL_19         0x1c    //33
#define PCI_TD_L_LEVEL_20         0x1d    //37
#define PCI_TD_L_LEVEL_21         0x1e    //41
#define PCI_TD_L_LEVEL_22         0x1f    //45

#define PCI_TD_TRIM_1            0x00    //-32
#define PCI_TD_TRIM_2            0x01    //-30
#define PCI_TD_TRIM_3            0x02    //-28
#define PCI_TD_TRIM_4            0x03    //-26
#define PCI_TD_TRIM_5            0x04    //-24
#define PCI_TD_TRIM_6            0x05    //-22
#define PCI_TD_TRIM_7            0x06    //-20
#define PCI_TD_TRIM_8            0x07    //-18
#define PCI_TD_TRIM_9            0x08    //-16
#define PCI_TD_TRIM_10            0x09    //-14
#define PCI_TD_TRIM_11            0x0a    //-12
#define PCI_TD_TRIM_12            0x0b    //-10
#define PCI_TD_TRIM_13            0x0c    //-8
#define PCI_TD_TRIM_14            0x0d    //-6
#define PCI_TD_TRIM_15            0x0e    //-4
#define PCI_TD_TRIM_16            0x0f    //-2
#define PCI_TD_TRIM_17            0x10    //0
#define PCI_TD_TRIM_18            0x11    //2
#define PCI_TD_TRIM_19            0x12    //4
#define PCI_TD_TRIM_20            0x13    //6
#define PCI_TD_TRIM_21            0x14    //7
#define PCI_TD_TRIM_22            0x15    //10
#define PCI_TD_TRIM_23            0x16    //12
#define PCI_TD_TRIM_24            0x17    //14
#define PCI_TD_TRIM_25            0x18    //16
#define PCI_TD_TRIM_26            0x19    //18
#define PCI_TD_TRIM_27            0x1a    //20
#define PCI_TD_TRIM_28            0x1b    //22
#define PCI_TD_TRIM_29            0x1c    //24
#define PCI_TD_TRIM_30            0x1d    //26
#define PCI_TD_TRIM_31            0x1e    //28
#define PCI_TD_TRIM_32            0x1f    //30

//sdio
#define PCI_SDIO_STATIC_CMP_DATA       0xFF
#define PCI_SDIO_STATIC_CHANNEL_0      0x01
#define PCI_SDIO_STATIC_CHANNEL_1      0x02
#define PCI_SDIO_STATIC_CHANNEL_2      0x04
#define PCI_SDIO_STATIC_CHANNEL_3      0x08
#define PCI_SDIO_STATIC_CHANNEL_4      0x10
#define PCI_SDIO_STATIC_CHANNEL_5      0x20
#define PCI_SDIO_STATIC_CHANNEL_6      0x40
#define PCI_SDIO_STATIC_CHANNEL_7      0x80

#define PCI_SDIO_DYNAMIC_CHANNEL_0      0x01
#define PCI_SDIO_DYNAMIC_CHANNEL_1      0x02
#define PCI_SDIO_DYNAMIC_CHANNEL_2      0x04
#define PCI_SDIO_DYNAMIC_CHANNEL_3      0x08

//detect
#define PCI_VD_L_FLAG               0x00000001
#define PCI_VD_H_FLAG               0x00000002
#define PCI_TD_L_FLAG               0x00000010
#define PCI_TD_H_FLAG               0x00000020

#define PCI_NVRAM_FLAG              0x00010000

#define PCI_SDIO_FLAG               0x800000

#define PCI_SDIO_CHANNEL_0          0x0100
#define PCI_SDIO_CHANNEL_1          0x0200
#define PCI_SDIO_CHANNEL_2          0x0400
#define PCI_SDIO_CHANNEL_3          0x0800
#define PCI_SDIO_CHANNEL_4          0x1000
#define PCI_SDIO_CHANNEL_5          0x2000
#define PCI_SDIO_CHANNEL_6          0x4000
#define PCI_SDIO_CHANNEL_7          0x8000

//SDIO pull up or pull down
#define SDIO_PULL_DOWN              0x00
#define SDIO_PULL_UP                0x01

//SDIO pull up or down current trim
#define SDIO_CURRENT_LEVEL_0        0x00 //默认,关闭
#define SDIO_CURRENT_LEVEL_1        0x01
#define SDIO_CURRENT_LEVEL_2        0x02 //20M
#define SDIO_CURRENT_LEVEL_3        0x03
#define SDIO_CURRENT_LEVEL_4        0x04 //10M
#define SDIO_CURRENT_LEVEL_5        0x05
#define SDIO_CURRENT_LEVEL_6        0x06
#define SDIO_CURRENT_LEVEL_7        0x07


/*******************************************************************************
* Function Name  : PCI_Init
* Description    : PCI模块初始化
* 				         注意：
* 				         每次上电前要执行该程序，否则无法读写nvram中的数据
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_Init(UINT8 clk_sel);

/*******************************************************************************
* Function Name  : Nvram_Init
* Description    : nvram模块初始化
* 				         注意：
* 				    写NVRAM前，需要使能；读取的时候不需要。
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void Nvram_Init(void);

extern void Nvram_Write(void);

extern void Nvram_Read(void);

/*******************************************************************************
* Function Name  : Nvram_Status
* Description    : 判断NVram是否被清除
* Input          : None
*
* Output         : 0 - 没有被清除
*                  1 - 被清除
* Return         : None
******************************************************************************/
extern int Nvram_Status(void);

/*******************************************************************************
* Function Name  : PCI_SDIO_Init
* Description    : SDIO初始化，将fail num清零
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_SDIO_Init(void);

/*******************************************************************************
* Function Name  : PCI_SDIO_Static_Set
* Description    : 设置SDIO静态工作方式，不开启的SDIO管脚可以接地，cmp_data值写0即可
* Input          : channel - 开启的SDIO通道号
*                  cmp_data - 比较电压值:0 - 该路SDIO为低电压维持，高电压触发；1 - 该路SDIO为高电压维持，低电压触发
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_SDIO_Static_Set(unsigned char channel, unsigned char cmp_data);

/*******************************************************************************
* Function Name  : PCI_SDIO_Dynamic_Set
* Description    : 设置SDIO动态工作方式
* Input          : channel - 开启的SDIO通道号
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_SDIO_Dynamic_Set(unsigned char channel);

/*******************************************************************************
* Function Name  : PCI_VD_Set
* Description    : 设置VD
* Input          : VDH_Value:高压检测阈值
*                  VDL_Value:低压检测阈值
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_VD_Set(unsigned char VDH_Value, unsigned char VDL_Value);

/*******************************************************************************
* Function Name  : PCI_TD_Set
* Description    : 设置TD
* Input          : TDH_Value:高温检测阈值
*                  TDL_Value:低温检测阈值
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_TD_Set(unsigned char TDH_Value, unsigned char TDL_Value, unsigned char TD_Trim);

/*******************************************************************************
* Function Name  : PCI_Clear_Status
* Description    : 清除PCI安全触发标志位
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_Clear_Status(void);

/*******************************************************************************
* Function Name  : PCI_Detect
* Description    : 判断PCI安全触发源
* Input          : None
* Output         : None
* Return         : 0 - 没有触发
*                  1 - 有触发
******************************************************************************/
extern int PCI_Detect(void);


/*******************************************************************************
* Function Name  : PCI_SDIO_PullControl
* Description    : PCI安全触发中断处理函数
*                  PCI中断功能，在芯片重新上电后，要重新配置。即断电后配置不会保留。
* Input          : channel - 开启的SDIO通道号，例如PCI_SDIO_STATIC_CHANNEL_0
*                  mode    - Pull up or Pull down
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_SDIO_PullControl(unsigned char channel, unsigned char mode);

/*******************************************************************************
* Function Name  : PCI_SDIO_Current_Trim
* Description    : PCI SDIO上下拉功能电流Trim值
* Input          : channel - 开启的SDIO通道号，例如PCI_SDIO_STATIC_CHANNEL_0
*                  mode    - Pull up or Pull down
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_SDIO_Current_Trim(unsigned char value);

/*******************************************************************************
* Function Name  : SDIO_ConfigGpio
* Description    : SDIO配置成GPIO用途
* Input          : - SDIO_PINx: EPORT Pin；where x can be 0~7 to select the SDIO peripheral.
*                  - GpioDir：设置GPIO方向   GPIO_OUTPUT：输出  GPIO_INPUT：输入
*
* Output         : None
* Return         : 0 - 成功;other - 失败
******************************************************************************/
extern int SDIO_ConfigGpio(SDIO_PINx GpioNo, UINT8 GpioDir);

/*******************************************************************************
* Function Name  : SDIO_WriteGpioData
* Description    : 设置SDIO_PINx对应引脚的电平
* Input          : - SDIO_PINx: SDIO Pin；where x can be 0~7 to select the SDIO peripheral.
*                  - bitVal：设置的电平，Bit_SET：设置为高电平  Bit_RESET：设置为低电平
*
* Output         : None
* Return         : 0: 设置成功    other：设置失败
******************************************************************************/
extern int SDIO_WriteGpioData(SDIO_PINx GpioNo, UINT8 bitVal);

/*******************************************************************************
* Function Name  : SDIO_ReadGpioData
* Description    : 获取SDIO_PINx对应引脚的电平
* Input          : - SDIO_PINx: SDIO Pin；where x can be 0~7 to select the SDIO peripheral.
*
* Output         : None
* Return         : Bit_SET:高电平  Bit_RESET：低电平  -1：失败
******************************************************************************/
extern int SDIO_ReadGpioData(SDIO_PINx GpioNo);

/*******************************************************************************
* Function Name  : PCI_Interrupt_Init
* Description    : 开启PCI的安全中断
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_Interrupt_Init(void);

/*******************************************************************************
* Function Name  : PCI_NVSRAM_WtiteByte
* Description    : 按字节向nvram写数据
* Input          : - addr: 写入数据的地址.
*                  - dat：写入的数据
*
* Output         : None
* Return         : other - 成功;0 - 失败
******************************************************************************/
extern BOOL PCI_NVSRAM_WtiteByte(UINT32 addr, UINT8 dat);

/*******************************************************************************
* Function Name  : PCI_NVSRAM_ReadByte
* Description    : 按字节读取nvram数据
* Input          : - addr: 数据的地址.
*                  - dat：数据
*
* Output         : None
* Return         : other - 成功;0 - 失败
******************************************************************************/
extern BOOL PCI_NVSRAM_ReadByte(UINT32 addr, UINT8 *dat);

/*******************************************************************************
* Function Name  : PCI_Open
* Description    : PCI模块Open
*                  PCI_Init的简化版，执行PCI_Open后不能够操作TD、VD、SDIO等寄存器，但是能够读写NVSRAM，读出和清除安全触发标志位
*                  该函数使用的前提是，调用过PCI_Init 
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_Open(void);

/*******************************************************************************
* Function Name  : PCI_Close
* Description    : PCI模块Close
* 				   关闭PCI模块的读写功能，会消除PCI_Init和PCI_Open函数增加的3uA功耗；
*                  同时，无法正常读取NVSRAM数据，读取PCI的触发标志位。
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_Close(void);

#endif /* __PCI_DRV_H__ */
