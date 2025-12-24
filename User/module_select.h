/*******************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     module_select.h
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief    This file contains the macro definitions that can be modifiedc
 *            on the UartTFT Application.
 ********************************************************************************/

#ifndef _module_select_h
#define _module_select_h

#define IIC_BUS 			0		    // Communication, 1:IIC protocol
#define UARTBUS_OPTION 		0		    // Communication, 0:Levetop protocol as slave   1:modbus as slave   2:modbus as host 3:Hid 4:Vcom
#define DualFlash 			0		
#define encoder_on 			0		    // 1:Open 0:Close
#define Touch_selection		0		    // 0:CTP  1:RTP
#define second_UI_add		46626118	//UI Engineering Address
  
#define Lcd_spi             0           //需要用SPI对液晶进行初始化
#define DBG_EN              1

#define PageCFG_Size 		3072	
#define Data_wLen_Size 		30
#define pData_wLen_Size 	30		
#define StrLen_Size 		300		

#define Basci_Size 			20		// Basic touch control
#define Adjust_Size 		10		// Variable adjustment
#define Slide_Size 			4		// Slide progress bar adjustment
#define Dio_Size 			20		// Data keyboard
#define KeyBoard_Size 		60		// Keyboard keys
#define Aio_Size 			10 		// ASCII keyboard
#define GBKio_Size 			10		// GBK keyboard
#define Batch_Size 			20		// Multivariable adjustment
#define PopUp_Size 			8		// pop-up window
#define Gif_Size 			20		// gif
#define RTC_Size 			6		// Digital clock
#define Clock_Size 			2		// Analog clock
#define Scroll_Size 		4		// Text scroll
#define Curve_Size 			8		// Curve
#define SldMenu_Size 		6		// Slide menu
#define RingSld_Size 		4		// Ring progress bar touch
#define Count_size 			10		// Curve channel count
#define Var_count_size		4		// Automatic Variable count	
#define Pointer_count_size  4		//Pointer display count
#endif
