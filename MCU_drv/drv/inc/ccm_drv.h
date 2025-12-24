// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : ccm_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef CCM_DRV_H_
#define CCM_DRV_H_

#include "ccm_reg.h"
#include "memmap.h"


#define CCM      ((CCM_TypeDef *)(CCM_BASE_ADDR))

#define SYS_CLK_INTERNAL_OSC   do{CCM->CCR |= (1<<11);}while(0)
#define SYS_CLK_EXTERNAL_OSC   do{CCM->CCR &= ~(1<<11);}while(0)
#define PERIPH_BRIDGE_PAE_DIS  do{CCM->CCR &= ~(1<<6);}while(0)
#define PERIPH_BRIDGE_PAE_EN   do{CCM->CCR |= (1<<6);}while(0)
#define PERIPH_BRIDGE_RAE_DIS  do{CCM->CCR &= ~(1<<5);}while(0)
#define PERIPH_BRIDGE_RAE_EN   do{CCM->CCR |= (1<<5);}while(0)
#define BME_DIS                do{CCM->CCR &= ~(1<<3);}while(0)
#define BME_EN                 do{CCM->CCR |= (1<<3);}while(0)
#define BMD_DIS                do{CCM->CCR &= ~(1<<2);}while(0)
#define BMD_EN                 do{CCM->CCR |= (1<<2);}while(0)


#define USBPHY_POWER_OFF       do{CCM->PHYPA |= (1<<6);}while(0)//USBPHY power on
#define USBPHY_POWER_ON        do{CCM->PHYPA &= ~(1<<6);}while(0)//USBPHY power off
#define USBPHY_CLK_SEL_60M     do{CCM->PHYPA |= (1<<4);}while(0)//usbphy clk sel 60M
#define USBPHY_CLK_SEL_48M     do{CCM->PHYPA &= ~(1<<4);}while(0)//usbphy clk sel 48M
#define ADC_TEST_MODE_DIS      do{CCM->PHYPA &= ~(1<<0);}while(0)//ADC test mode enable
#define ADC_TEST_MODE_EN       do{CCM->PHYPA |= (1<<0);}while(0)//ADC test mode disable

#define PIN_TDO_PULLUP_DIS     do{CCM->PCFG3 &= ~(1<<10);}while(0)
#define PIN_TDO_PULLUP_EN      do{CCM->PCFG3 |= (1<<10);}while(0)

#define PWM_INPUT_OUTPUT_EN    do{CCM->PCFG3 |= (7<<0);}while(0)
#define PWM_INPUT_OUTPUT_DIS   do{CCM->PCFG3 &= ~(7<<0);}while(0)

#define RTC_INTERFACE_DIS      do{CCM->RTCCFG12 &= ~(1<<15);}while(0)
#define RTC_INTERFACE_EN       do{CCM->RTCCFG12 |= (1<<15);}while(0)

typedef enum
{
    CLOCK_OUT = 0,
    RESET_OUT,

}CCM_PINx;

/*******************************************************************************
* Function Name  : read_chip_cid
* Description    : read  the chip’s ID and Revision number
* Input          : None
*
* Output         : - cid:  cid[15~8]：a unique version identification number for the chip
*                          cid[15~8]：the full-layer mask revision number;
*                          The revision numbers are assigned in chronological order.
* Return         : None
******************************************************************************/
extern void read_chip_cid(UINT16 *cid);

/*******************************************************************************
* Function Name  : config_freq_detect_point
* Description    : Configuration the frequency detect point
* Input          :  - fd_hi:  Configure the high frequency detect point
*                   - fd_low: Configure the low frequency detect point
*
* Output         : None
*
* Return         : None
******************************************************************************/
//extern void config_freq_detect_point(UINT8 fd_hi, UINT8 fd_low);


/*******************************************************************************
* Function Name  : config_osc_bias_trim_value
* Description    : config osc bias trim value
* Input          :  - trim_val:  osc bias trim value
*
*
* Output         : None
*
* Return         : None
******************************************************************************/
extern void config_osc_bias_trim_value(UINT8 trim_val);

/*******************************************************************************
* Function Name  : CCM_ConfigGpio
* Description    : 将芯片clock out、POR(reset out)管脚配置成GPIO用途
* Input          : - CCM_name: 芯片对应的PIN脚，取值CLOCK_OUT或者RESET_OUT
*                  - CCM_Dir：设置GPIO方向   GPIO_OUTPUT：输出  GPIO_INPUT：输入
*
* Output         : None
* Return         : None
******************************************************************************/
extern void CCM_ConfigGpio(UINT8 CCM_Name, UINT8 CCM_Dir);

/*******************************************************************************
* Function Name  : CCM_Gpio_Diable
* Description    : 将芯片clock out、POR(reset out)管脚的GPIO功能关掉，恢复成之前的功能
* Input          : - CCM_name: 芯片对应的PIN脚，取值CLOCK_OUT或者RESET_OUT
*
* Output         : None
* Return         : None
******************************************************************************/
extern void CCM_Gpio_Diable(UINT8 CCM_Name);

/*******************************************************************************
* Function Name  : CCM_ReadGpioData
* Description    : 获取芯片GPIO对应引脚的电平
* Input          : - CCM_name: 芯片对应的PIN脚，取值CLOCK_OUT或者RESET_OUT
*
* Output         : None
* Return         : Bit_SET:高电平  Bit_RESET：低电平
******************************************************************************/
extern UINT8 CCM_ReadGpioData(UINT8 CCM_Name);

/*******************************************************************************
* Function Name  : CCM_WriteGpioData
* Description    : 设置芯片GPIO管脚对应管脚的电平
* Input          : - CCM_name: 芯片对应的PIN脚，取值CLOCK_OUT或者RESET_OUT
*                  - bitVal：设置的电平，Bit_SET：设置为高电平  Bit_RESET：设置为低电平
*
* Output         : None
* Return         : None
******************************************************************************/
extern void CCM_WriteGpioData(UINT8 CCM_Name, UINT8 bitVal);
#endif /* CCM_DRV_H_ */
