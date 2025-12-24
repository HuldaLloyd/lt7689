// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : pwm_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "pwm_drv.h"
#include "ccm_drv.h"
#include "uart.h"
#include "string.h"
#include "iomacros.h"
#include "common.h"
	

/*******************************************************************************
* Function Name  : PWM_OutputInit
* Description    : PWM输出初始化，输出脉冲
* Input          : - PWMx : PWM channel；where x can be 0, 1, 2, 3 to select the PWM peripheral.
*                  - PWM_Prescaler: PWM预分频值=PWM_Prescaler+1
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
void PWM_OutputInit(UINT8 PWMx, UINT8 PWM_Prescaler, UINT8 PWM_Csr, UINT16 PWM_Period, UINT16 PWM_Width, UINT8 PWM_CHxINV_Flg)
{
	assert_param(IS_PWM_PORTx(PWMx));
	assert_param(IS_PWM_CLK_DIV(PWM_Csr));
	
//	PWM_INPUT_OUTPUT_EN;
	switch(PWMx)
	{
		case PWM_PORT0:
			
			CCM->PCFG3 |= (1<<0);
			PWM->PWM_CR_CH0 &= ~CH0EN;

			PWM->PWM_CSR &= (~(7<<0));//Timer0 Clock Source Selection
			PWM->PWM_CSR |= ((PWM_Csr&0x07)<<0);
			PWM->PWM_PR_CP0 = (PWM_Prescaler&0xff);
			PWM->PWM_CR_CH0 |= CH0MOD;//1 = Auto-load Mode
			PWM->PWM_CMR0 = PWM_Width;
			PWM->PWM_CNR0 = PWM_Period;
			PWM->PWM_PCR_PDDR |= 0x1;
			PWM->PWM_PCR_PULLEN |= 0x1;
			PWM->PWM_PCR_PDR |= 0x01;
			PWM->PWM_CR_CH0 |= (PWM_CHxINV_Flg<<2);
			
			PWM->PWM_CR_CH0 |= CH0EN;
			break;
		case PWM_PORT1:
			
			CCM->PCFG3 |= (1<<1);
			PWM->PWM_CR_CH1 &= ~CH1EN;

			PWM->PWM_CSR &= (~(7<<4));//Timer1 Clock Source Selection
			PWM->PWM_CSR |= ((PWM_Csr&0x07)<<4);
			PWM->PWM_PR_CP0 = (PWM_Prescaler&0xff);
			PWM->PWM_CR_CH1 |= CH1MOD;//1 = Auto-load Mode
			PWM->PWM_CMR1 = PWM_Width;
			PWM->PWM_CNR1 = PWM_Period;
			PWM->PWM_PCR_PDDR |= 0x2;
			PWM->PWM_PCR_PULLEN |= 0x2;
			PWM->PWM_PCR_PDR |= 0x02;
			PWM->PWM_CR_CH1 |= (PWM_CHxINV_Flg<<2);
			
			PWM->PWM_CR_CH1 |= CH1EN;
			break;
		case PWM_PORT2:
			
			CCM->PCFG3 |= (1<<2);
			PWM->PWM_CR_CH2 &= ~CH2EN;
			
			PWM->PWM_CSR &= (~(7<<8));//Timer2 Clock Source Selection
			PWM->PWM_CSR |= (PWM_Csr&0x07)<<8;
			PWM->PWM_PR_CP1 = (PWM_Prescaler&0xff);
			PWM->PWM_CR_CH2 = CH2MOD;
			PWM->PWM_CMR2 = PWM_Width;
			PWM->PWM_CNR2 = PWM_Period;
			PWM->PWM_PCR_PDDR |= 0x4;
			PWM->PWM_PCR_PULLEN |= 0x4;
			PWM->PWM_PCR_PDR |= 0x04;
			PWM->PWM_CR_CH2 |= (PWM_CHxINV_Flg<<2);
			
			PWM->PWM_CR_CH2 |= CH2EN;
			break;
		case PWM_PORT3:
			
			CCM->PCFG3 |= (1<<2);
			PWM->PWM_CR_CH3 &= ~CH3EN;
			
			PWM->PWM_CSR &= (~(7<<12));//Timer3 Clock Source Selection
			PWM->PWM_CSR |= (PWM_Csr&0x07)<<12;
			PWM->PWM_PR_CP1 = (PWM_Prescaler&0xff);
			PWM->PWM_CR_CH3 = CH3MOD;
			PWM->PWM_CMR3 = PWM_Width;
			PWM->PWM_CNR3 = PWM_Period;
			PWM->PWM_PCR_PDDR |= 0x8;
			PWM->PWM_PCR_PULLEN |= 0x8;
			PWM->PWM_PCR_PDR |= 0x08;
			PWM->PWM_CR_CH3 |= (PWM_CHxINV_Flg<<2);
			
			PWM->PWM_CR_CH3 |= CH3EN;
			break;
		default:
			break;
	}
}
void PWM3_OutputInit(UINT8 PWMx, UINT8 PWM_Prescaler, UINT8 PWM_Csr, UINT16 PWM_Period, UINT16 PWM_Width, UINT8 PWM_CHxINV_Flg)
{
	assert_param(IS_PWM_PORTx(PWMx));
	assert_param(IS_PWM_CLK_DIV(PWM_Csr));
	
//	PWM_INPUT_OUTPUT_EN;
	switch(PWMx)
	{

		case PWM_PORT3:
			
			CCM->PCFG3 |= (1<<2);
			PWM->PWM_CR_CH3 &= ~CH3EN;
			
			PWM->PWM_CSR &= (~(7<<12));//Timer3 Clock Source Selection
			PWM->PWM_CSR |= (PWM_Csr&0x07)<<12;
			//PWM->PWM_PR_CP1 = (PWM_Prescaler&0xff);
			PWM->PWM_CR_CH3 = CH3MOD;
			PWM->PWM_CMR3 = PWM_Width;
			PWM->PWM_CNR3 = PWM_Period;
			PWM->PWM_PCR_PDDR |= 0x8;
			PWM->PWM_PCR_PULLEN |= 0x8;
			PWM->PWM_PCR_PDR |= 0x08;
			PWM->PWM_CR_CH3 |= (PWM_CHxINV_Flg<<2);
			
			//PWM->PWM_CR_CH3 |= CH3EN;
			break;
		default:
			break;
	}
}
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
void PWM_InputInit(UINT8 PWMx, UINT8 pwm_prescaler, UINT8 pwm_csr,UINT16 pwm_period)
{
	
	assert_param(IS_PWM_PORTx(PWMx));
	PWM_INPUT_OUTPUT_EN;
	
	switch(PWMx)
	{
		case PWM_PORT0:
			PWM->PWM_CR_CH0 &= ~CH0EN;

			PWM->PWM_CSR |= pwm_csr;
			PWM->PWM_PR_CP0 = pwm_prescaler;
			PWM->PWM_CR_CH0 |= CH0MOD;
			PWM->PWM_CNR0 = pwm_period;
			PWM->PWM_PCR_PDDR &= 0xe;
			PWM->PWM_CCR0_CH0 = (UINT8)(CAPCHxEN|FL_IE|RL_IE);
			PWM->PWM_CR_CH0 |= CH0EN;
		NVIC_Init(3, 3, PWM0_IRQn, 2);
		 break;

		case PWM_PORT1:

			PWM->PWM_CR_CH1 &= ~CH1EN;

			PWM->PWM_CSR |= pwm_csr<<4;
			PWM->PWM_PR_CP0 = pwm_prescaler;
			PWM->PWM_CR_CH1 |= CH1MOD;
			PWM->PWM_CNR1 = pwm_period;
			PWM->PWM_PCR_PDDR &= 0xd;
			PWM->PWM_CCR0_CH1 = (UINT8)(CAPCHxEN|FL_IE|RL_IE);

			PWM->PWM_CR_CH1 |= CH1EN;
			NVIC_Init(3, 3, PWM1_IRQn, 2);
		 break;

		case PWM_PORT2:
			PWM->PWM_CR_CH2 &= ~CH2EN;

			PWM->PWM_CSR |= pwm_csr<<8;
			PWM->PWM_PR_CP1 = pwm_prescaler;
			PWM->PWM_CR_CH2 |= CH2MOD;
			PWM->PWM_CNR2 = pwm_period;
			PWM->PWM_PCR_PDDR &= 0x0b;
			PWM->PWM_CCR1_CH2 = (UINT8)(CAPCHxEN|FL_IE|RL_IE);

			PWM->PWM_CR_CH2 |= CH2EN;
		NVIC_Init(3, 3, PWM2_IRQn, 2);
		 break;
		case PWM_PORT3:
			PWM->PWM_CR_CH3 &= ~CH3EN;

			PWM->PWM_CSR |= pwm_csr<<12;
			PWM->PWM_PR_CP1 = pwm_prescaler;
			PWM->PWM_CR_CH3 |= CH3MOD;
			PWM->PWM_CNR3 = pwm_period;
			PWM->PWM_PCR_PDDR &= 0x07;
			PWM->PWM_CCR1_CH3 = (UINT8)(CAPCHxEN|FL_IE|RL_IE);

			PWM->PWM_CR_CH3 |= CH3EN;
		NVIC_Init(3, 3, PWM3_IRQn, 2);
		 break;
		default:			
		 break;
	}
	
}

/*******************************************************************************
* Function Name  : PWM_ISR
* Description    : PWM_中断处理
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void PWM_ISR(void)
{
	UINT8  bitstatus = 0x00;

	//DelayMS(10);		//消抖
	bitstatus = PWM->PWM_PCR_PDR;
	////printf(" %02x", PWM->PWM_PCR_PDR);
	
	if(PWM->PWM_CCR0_CH0&CAPIF)	//PORT0
	{
		if (bitstatus &(Bit_SET<<PWM_PORT0))	//高电平
		{
			//printf("Enter PWM_PORT0_RISING interrupt.\r\n");
		}
		else	//低电平
		{
			//printf("Enter PWM_PORT0_FALLING interrupt.\r\n");
		}		
		PWM->PWM_CCR0_CH0 |= CAPIF;
	}

	if(PWM->PWM_CCR0_CH1&CAPIF)	//PORT1
	{
		if (bitstatus &(Bit_SET<<PWM_PORT1))	//高电平
		{
			//printf("Enter PWM_PORT1_RISING interrupt.\r\n");
		}
		else	//低电平
		{
			//printf("Enter PWM_PORT1_FALLING interrupt.\r\n");
		}		
		PWM->PWM_CCR0_CH1 |= CAPIF;
	}

	if(PWM->PWM_CCR1_CH2&CAPIF)	//PORT2
	{
		if (bitstatus &(Bit_SET<<PWM_PORT2))	//高电平
		{
			//printf("Enter PWM_PORT2_RISING interrupt.\r\n");
		}
		else	//低电平
		{
			//printf("Enter PWM_PORT2_FALLING interrupt.\r\n");
		}		
		PWM->PWM_CCR1_CH2 |= CAPIF;
	}

	if(PWM->PWM_CCR1_CH3&CAPIF)	//PORT3
	{
		if (bitstatus &(Bit_SET<<PWM_PORT3))	//高电平
		{
			//printf("Enter PWM_PORT3_RISING interrupt.\r\n");
		}
		else	//低电平
		{
			//printf("Enter PWM_PORT3_FALLING interrupt.\r\n");
		}		
		PWM->PWM_CCR1_CH3 |= CAPIF;
	}
}

void PWM0_IRQHandler(void)
{
	PWM_ISR();
}


void PWM1_IRQHandler(void)
{
	PWM_ISR();
}


void PWM2_IRQHandler(void)
{
	PWM_ISR();
}


void PWM3_IRQHandler(void)
{
	PWM_ISR();
}



/*******************************************************************************
* Function Name  : PWM_Start
* Description    : PWM打开
* Input          : - PWMx : PWM channel；where x can be 0, 1, 2, 3 to select the PWM peripheral.
*
* Output         : None
* Return         : None
*******************************************************************************/
void PWM_Start(UINT8 PWMx)
{
	switch(PWMx)
	{
		case PWM_PORT0:
			PWM->PWM_CR_CH0 |= CH0EN;
			break;
		case PWM_PORT1:
			PWM->PWM_CR_CH1 |= CH1EN;
			break;
		case PWM_PORT2:
			PWM->PWM_CR_CH2 |= CH2EN;
			break;
		case PWM_PORT3:
			PWM->PWM_CR_CH3 |= CH3EN;
			break;
		default:
			break;
	}
}

/*******************************************************************************
* Function Name  : PWM_Stop
* Description    : PWM关闭
* Input          : - PWMx : PWM channel；where x can be 0, 1, 2, 3 to select the PWM peripheral.
*
* Output         : None
* Return         : None
*******************************************************************************/
void PWM_Stop(UINT8 PWMx)
{
	switch(PWMx)
	{
		case PWM_PORT0:
			PWM->PWM_CR_CH0 &= ~CH0EN;
			break;
		case PWM_PORT1:
			PWM->PWM_CR_CH1 &= ~CH1EN;
			break;
		case PWM_PORT2:
			PWM->PWM_CR_CH2 &= ~CH2EN;
			break;
		case PWM_PORT3:
			PWM->PWM_CR_CH3 &= ~CH3EN;
			break;
		default:
			break;
	}
}

/*******************************************************************************
* Function Name  : PWM_ConfigGpio
* Description    : PWM配置成GPIO用途
* Input          : - PWMx: PWM channel；where x can be 0, 1, 2 to select the PWM peripheral.
*                  - PWM_Port: 0,1,2,3
*                  - SPI_Dir：设置GPIO方向   GPIO_OUTPUT：输出  GPIO_INPUT：输入
*                  - PullUp_En：设置相应的脚上拉使能
*                               PWM_PULLUP_EN:  enable上拉
*                               PWM_PULLUP_DIS: disable上拉
*
* Output         : None
* Return         : None
******************************************************************************/
INT8 PWM_ConfigGpio(UINT8 PWMx, UINT8 GPIO_Dir, UINT8 PullUp_En)
{
	UINT8 ch = PWMx;

	assert_param(IS_PWM_PULLUP_STA(PullUp_En));
	assert_param(IS_GPIO_DIR_BIT(GPIO_Dir));

	if (ch > PWM_PORT3)
		return -1;

	PWM_INPUT_OUTPUT_EN;

	if (GPIO_Dir == GPIO_OUTPUT)
	{
		PWM->PWM_PCR_PDDR |= GPIO_Dir<<ch;//output
	}
	else if (GPIO_Dir == GPIO_INPUT)
	{
		PWM->PWM_PCR_PDDR &= (~(GPIO_Dir<<ch));//input
	}

	PWM->PWM_PCR_PULLEN &= (~(1<<ch));
	PWM->PWM_PCR_PULLEN |= PullUp_En<<ch;

	return 0;
}

/*******************************************************************************
* Function Name  : PWM_ReadGpioData
* Description    : 获取PWMx对应引脚的电平
* Input          : - PWMx: PWM channel；where x can be 0, 1, 2 to select the PWM peripheral.
*                  - PWM_Port: 0,1,2,3
*
* Output         : None
* Return         : Bit_SET:高电平  Bit_RESET：低电平  -1：fail
******************************************************************************/
INT8 PWM_ReadGpioData(UINT8 PWMx)
{
	INT8  bitstatus = 0x00;

	if (PWMx > PWM_PORT3)
		return -1;

	bitstatus = PWM->PWM_PCR_PDR;

	if (bitstatus &(Bit_SET<<PWMx))
		bitstatus = Bit_SET;
	else
		bitstatus = Bit_RESET;

	return bitstatus;
}

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
INT8 PWM_WriteGpioData(UINT8 PWMx, UINT8 bitVal)
{
	assert_param(IS_GPIO_BIT_ACTION(bitVal));

	if (PWMx > PWM_PORT3)
		return -1;
	if (bitVal > 2)
		return -2;

	if (bitVal == Bit_SET)
		PWM->PWM_PCR_PDR |= (Bit_SET<<PWMx);
	else
		PWM->PWM_PCR_PDR &= (~(Bit_SET<<PWMx));

	return 0;
}

