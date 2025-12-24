/*******************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     bsp_user.c
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief    This file contains customized functions written by the customer.
 ********************************************************************************/

#include "bsp_user.h"
#include "data.h"
#include "bsp.h"

uint8_t refresh_flag = 0;
void cunstomize(u16 id) // user define command
{
	if (id == 1)
	{
		// omatically refresh when the variable changes or display when the page is just entered
		if ((var[0x1001 * 2 + 1] != var_temp[0x1001 * 2 + 1]) || refresh_flag == 1)
		{
			// user part
		}
	}
}