// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : pwm_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef PWM_DRV_H_
#define PWM_DRV_H_

#include "pwm_reg.h"

#define PWM        ((PWM_TypeDef *)(PWM_BASE_ADDR))

//PWM_PORT DEFINITION
#define PWM_PORT0		0
#define PWM_PORT1		1
#define PWM_PORT2		2
#define PWM_PORT3		3

#define IS_PWM_PORTx(port) (((port) == PWM_PORT0) || \
                            ((port) == PWM_PORT1) || \
                            ((port) == PWM_PORT2) || \
                            ((port) == PWM_PORT3))


//PWM Clock divider
#define PWM_CLK_DIV_1   4
#define PWM_CLK_DIV_2   0
#define PWM_CLK_DIV_4   1
#define PWM_CLK_DIV_8   2
#define PWM_CLK_DIV_16  3
#define IS_PWM_CLK_DIV(div) (((div) == PWM_CLK_DIV_1) || \
                             ((div) == PWM_CLK_DIV_2) || \
                             ((div) == PWM_CLK_DIV_4) || \
                             ((div) == PWM_CLK_DIV_8) || \
                             ((div) == PWM_CLK_DIV_16))


#define PWM_PULLUP_EN   1
#define PWM_PULLUP_DIS  0

#define IS_PWM_PULLUP_STA(sta) (((sta) == PWM_PULLUP_EN) || \
                                 ((sta) == PWM_PULLUP_DIS))

/*******************************************************************************
* Function Name  : PWM_OutputInit
* Description    : PWM输出初始化，输出脉冲
* Input          : - PWMx : PWM channel；where x can be 0, 1, 2, 3 to select the PWM peripheral.
*                  - PWM_Prescaler: PWM预分频值
*                  - PWM_Csr：Clock Source Selection
*                             取值：PWM_CLK_DIV_1、PWM_CLK_DIV_16、PWM_CLK_DIV_8、PWM_CLK_DIV_4、PWM_CLK_DIV_2
*                  - PWM_Period：PWM Counter/Timer Loaded Value, data range : 65535~0 (Unit : 1 PWM clock cycle)
*                                One PWM cycle width = PWM_Period + 1
*                  - PWM_Width：PWM Comparator Register Value;data range : 65535~0 (Unit : 1 PWM clock cycle)
*                               used to determine PWM output duty ratio
*                               PWM_Width >= PWM_Period : PWM output is always high
*                               PWM_Width >= PWM_Period : PWM output high = (PWM_Width + 1) unit
*                               PWM_Width = 0 : PWM output high = 1 unit
*                  - PWM_CHxINV_Flg：Timer  Inverter ON/OFF flag
*                                    1 = Inverter ON
*                                    0 = Inverter OFF
*
* Output         : None
* Return         : None
*******************************************************************************/
extern void PWM_OutputInit(UINT8 PWMx, UINT8 PWM_Prescaler, UINT8 PWM_Csr, UINT16 PWM_Period,  UINT16 PWM_Width, UINT8 PWM_CHxINV_Flg);
extern void PWM3_OutputInit(UINT8 PWMx, UINT8 PWM_Prescaler, UINT8 PWM_Csr, UINT16 PWM_Period,  UINT16 PWM_Width, UINT8 PWM_CHxINV_Flg);
/*******************************************************************************
* Function Name  : PWM_InputInit
* Description    : PWM输入初始化，可触发中断
* Input          : - - PWMx : PWM channel；where x can be 0, 1, 2, 3 to select the PWM peripheral.
*                  - PWM_Prescaler: PWM预分频值
*                  - PWM_Csr：Clock Source Selection
*                             取值：PWM_CLK_DIV_1、PWM_CLK_DIV_16、PWM_CLK_DIV_8、PWM_CLK_DIV_4、PWM_CLK_DIV_2
*                  - PWM_Period：PWM Counter/Timer Loaded Value, data range : 65535~0 (Unit : 1 PWM clock cycle)
*                                One PWM cycle width = PWM_Period + 1
*
* Output         : None
* Return         : None
*******************************************************************************/
extern void PWM_InputInit(UINT8 PWMx, UINT8 pwm_prescaler, UINT8 pwm_csr,UINT16 pwm_period);

/*******************************************************************************
* Function Name  : PWM_ISR
* Description    : PWM中断处理函数
* Input          : - None
*
* Output         : None
* Return         : None
*******************************************************************************/
extern void PWM_ISR(void);

/*******************************************************************************
* Function Name  : PWM_Start
* Description    : PWM打开
* Input          : - PWMx : PWM channel；where x can be 0, 1, 2, 3 to select the PWM peripheral.
*
* Output         : None
* Return         : None
*******************************************************************************/
extern void PWM_Start(UINT8 PWMx);

/*******************************************************************************
* Function Name  : PWM_Stop
* Description    : PWM关闭
* Input          : - PWMx : PWM channel；where x can be 0, 1, 2, 3 to select the PWM peripheral.
*
* Output         : None
* Return         : None
*******************************************************************************/
extern void PWM_Stop(UINT8 PWMx);


/*******************************************************************************
* Function Name  : PWM_ConfigGpio
* Description    : PWM配置成GPIO用途
* Input          : - PWMx: PWM channel；where x can be 0, 1, 2 to select the PWM peripheral.
*
*                  - SPI_Dir：设置GPIO方向   GPIO_OUTPUT：输出  GPIO_INPUT：输入
*                  - PullUp_En：设置相应的脚上拉使能
*                               PWM_PULLUP_EN:  enable上拉
*                               PWM_PULLUP_DIS: disable上拉
*
* Output         : None
* Return         : None
******************************************************************************/
INT8 PWM_ConfigGpio(UINT8 PWMx, UINT8 GPIO_Dir, UINT8 PullUp_En);


/*******************************************************************************
* Function Name  : PWM_ReadGpioData
* Description    : 获取PWMx对应引脚的电平
* Input          : - PWMx: PWM channel；where x can be 0, 1, 2 to select the PWM peripheral.
*                  - PWM_Port: 0,1,2,3
*
* Output         : None
* Return         : Bit_SET:高电平  Bit_RESET：低电平  -1：fail
******************************************************************************/
INT8 PWM_ReadGpioData(UINT8 PWMx);

/*******************************************************************************
* Function Name  : PWM_WriteGpioData
* Description    : 设置PWMx对应引脚的电平
* Input          : - PWMx: PWM channel；where x can be 0, 1, 2 to select the PWM peripheral.
*                  - PWM_Port: 0,1,2,3
*                  - bitVal：设置的电平，Bit_SET：设置为高电平  Bit_RESET：设置为低电平
*
* Output         : None
* Return         : 0: 设置成功    other：设置失败
******************************************************************************/
INT8 PWM_WriteGpioData(UINT8 PWMx, UINT8 bitVal);


#endif /* PWM_DRV_H_ */
