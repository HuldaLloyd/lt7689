#include "type.h"
#include "ioctrl_drv.h"

void SWD_Enable_Cmd(bool index)
{
	if(index == TRUE)
	{
				IOCTRL->SWAP_CONTROL_REG |= (1<<1);
	}
	else
	{
				IOCTRL->SWAP_CONTROL_REG &= ~(1<<1);
	}
}

/*******************************************************************************
* Function Name  :IO_Ctrl_SCI_Swap
* Description    :SCI GINT 复用功能切换
* Input          :-sg_sel   引脚选择
*									-sw 			:复用功能开关
*									 TRUE  :功能打开
*                  FALSE :功能关闭
*
* Output         : None
* Return         : None
******************************************************************************/
void IO_Ctrl_SCI_Swap(IOCTRL_SG_SEL sg_sel, bool sw)
{
	if(TRUE == sw)//on
	{
		switch(sg_sel)
		{
			case TX1_GINT4:
				IOCTRL->SCI_CONTROL_REG |= SCICR_TX1_GINT4_SWAP_MASK;	//开启GINT4的RX1复用功能
				break;
			case RX1_GINT0:
				IOCTRL->SCI_CONTROL_REG |= SCICR_RX1_GINT0_SWAP_MASK;	//开启GINT0的TX1复用功能
				break;
			case TX2_GINT5:
				IOCTRL->SCI_CONTROL_REG |= SCICR_TX2_GINT5_SWAP_MASK;	//开启GINT5的RX2复用功能
				break;
			case RX2_GINT3:
				IOCTRL->SCI_CONTROL_REG |= SCICR_RX2_GINT3_SWAP_MASK;	//开启GINT3的RX2复用功能
				break;
			case TX3_GINT1:
				IOCTRL->SCI_CONTROL_REG |= SCICR_TX3_GINT1_SWAP_MASK;	//开启GINT1的TX3复用功能
				break;
			case RX3_GINT2:
				IOCTRL->SCI_CONTROL_REG |= SCICR_RX3_GINT2_SWAP_MASK;	//开启GINT2的TX3复用功能
				break;
			default:
				break;
		}
	}
	else	//off
	{
		switch(sg_sel)
		{
			case TX1_GINT4:
				IOCTRL->SCI_CONTROL_REG &= ~SCICR_TX1_GINT4_SWAP_MASK;	//关闭GINT4的RX1复用功能
				break;
			case RX1_GINT0:
				IOCTRL->SCI_CONTROL_REG &= ~SCICR_RX1_GINT0_SWAP_MASK;	//关闭GINT0的TX1复用功能
				break;
			case TX2_GINT5:
				IOCTRL->SCI_CONTROL_REG &= ~SCICR_TX2_GINT5_SWAP_MASK;	//关闭GINT5的RX2复用功能
				break;
			case RX2_GINT3:
				IOCTRL->SCI_CONTROL_REG &= ~SCICR_RX2_GINT3_SWAP_MASK;	//关闭GINT3的RX2复用功能
				break;
			case TX3_GINT1:
				IOCTRL->SCI_CONTROL_REG &= ~SCICR_TX3_GINT1_SWAP_MASK;	//关闭GINT1的TX3复用功能
				break;
			case RX3_GINT2:
				IOCTRL->SCI_CONTROL_REG &= ~SCICR_RX3_GINT2_SWAP_MASK;	//关闭GINT2的TX3复用功能
				break;
			default:
				break;
		}
	}

}


