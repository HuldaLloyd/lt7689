/************************************************************************
* Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
* @file    	main.c
* @author  	UartTFT Application Team
* @version  V1.0.0
* @date     2023-02-24
* @brief 	This file include main function.

*************************************************************************/

#include "sys.h"
#include "uart.h"
#include "delay.h"
#include "data.h"
#include "common.h"
#include "usb_const.h"
#include "string.h"
#include "levetop.h"
#include "eflash_drv.h"
#include "cache_drv.h"
#include "spi_drv.h"
#include "eport_drv.h"
#include "ccm_drv.h"
#include "LT768_Lib.h"
#include "ioctrl_drv.h"
#include "pit32_drv.h"
#include "pwm.h"
#include "pit.h"
#include "dac.h"
#include "flash.h"
#include "bsp.h"
#include "ctp.h"
#include "rtp.h"
#include "math.h"
#include "iomacros.h"
#include "wdt_drv.h"
#include "usb_drv.h"
#include "usb_const.h"
extern unsigned int gBaudrate;
uint8_t  hid_rec_index;
static void Printf_Version(void)
{
#if DBG_EN	

	printf("/**************************************************/\n");
	printf(" date = %s,time = %s    \r\n", (char *)__DATE__, (char *)__TIME__);
	printf(" LT7689_LoadDown_Uart_V1.01  \r\n");
	printf(" system clk = %dMHz         \r\n", g_sys_clk / 1000000);
	printf(" ips clk    = %dMHz         \r\n", g_ips_clk / 1000000);
	printf("/**************************************************/\n");
#endif
}

void Check_version(void)
{
	uint8_t showbuff[10];

	if (encryption_key == 1)
	{
		LT768_Select_Internal_Font_Init(24,1,1,1,0);
		LT768_DrawSquare_Fill(0,0,LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1,Blue2) ;
		LT768_Print_Internal_Font_String(0,10,Red,White,"Illegal UI file");
		Display_ON();
		LT768_PWM1_Init(1, 0, 5, 800, 500);
		while(1) WDT_FeedDog(); //Clear watchdog;
	}
	
	if(UI_version < 1136 || UI_version >= 5500)
	{
		Flash_Read_UI(showbuff, 0x80 + 0x24, 7);
		LT768_Select_Internal_Font_Init(24,1,1,1,0);
		LT768_DrawSquare_Fill(0,0,LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1,Blue2) ;
		LT768_Print_Internal_Font_String(0,10,Red,White,"Firmware matching error");
		LT768_Print_Internal_Font_String(0,50,Black,White,"MCU_Code version : V2.100");
		LT768_Print_Internal_Font_String(0,90,Black,White,"UI_Editor-II version :");
		LT768_Print_Internal_Font_String(275,90,Black,White,showbuff);
		Display_ON();
		LT768_PWM1_Init(1, 0, 5, 800, 500);
		while(1) WDT_FeedDog(); //Clear watchdog;
	}
}

void PeripheralInit(void)
{
	/*system configuration,Obtain clock and other relevant parameters */
	Sys_Init();
	Enable_Interrupts;

//	IOCTRL->SCI_CONTROL_REG &= 0x80FFFFFF;
//	IOCTRL->SCI_CONTROL_REG |= (((u32)0x2E) << 24); // Replace 4 IO ports with serial ports
#if DualFlash
	IOCTRL->SCI_CONTROL_REG &= 0x80FFFFFF;
	IOCTRL->SCI_CONTROL_REG |= (((u32)0x22) << 24); // Replace 4 IO ports with serial ports
#endif
	/****Encoder IO port initialization****/
#if encoder_on
	IOCTRL->SCI_CONTROL_REG &= 0x80FFFFFF;
	IOCTRL->SCI_CONTROL_REG |= (((u32)0x2A) << 24); // Replace 4 IO ports with serial ports
	UART_ConfigGpio(SCI3, UART_TX, GPIO_INPUT);
	UART_ConfigGpio(SCI2, UART_RX, GPIO_INPUT);
	EPORT_ConfigGpio(EPORT_PIN2, GPIO_INPUT);
	EPORT_Init(EPORT_PIN2, FALLING_EDGE_INT);
#endif

    //SPI_ConfigGpio(SPI3,SPI_MOSI,GPIO_OUTPUT);//配置P11为输出 蜂鸣器 
    //SPI_WriteGpioData(SPI3, SPI_MOSI,0);//拉低蜂鸣器
    
#if Lcd_spi //核心板没有多余的IO用于初始化
    //SPI_ConfigGpio(SPI3,SPI_MOSI,GPIO_OUTPUT);//配置P11为输出
    //SPI_ConfigGpio(SPI3,SPI_MISO,GPIO_OUTPUT); //配置P12为输出
    //EPORT_ConfigGpio(EPORT_PIN2, GPIO_OUTPUT);//配置P31为输出

	//Lcd_init();//初始化LCD
	//Delay_ms(1000);
	//Delay_ms(1000);
	//Delay_ms(1000);
	//Delay_ms(1000);
	

#endif

	UART_ConfigGpio(SCI2, UART_TX, GPIO_OUTPUT);
	

	//PWM_OutputInit(PWM_PORT1, 0, PWM_CLK_DIV_1, 11, 5, 0); // PWM analog 6M crystal oscillator
	PWM_OutputInit(PWM_PORT1, 2, PWM_CLK_DIV_1, 4, 2, 0); // PWM analog 5M crystal oscillator
	
	Close_SPI1();
	EPORT_ConfigGpio(EPORT_PIN6, GPIO_OUTPUT);
	EPORT_WriteGpioData(EPORT_PIN6, Bit_RESET);

	SCI1_Init(g_ips_clk, 115200,0);	
	UartDBG_Init(g_ips_clk, 115200);	

	PIT1_Init();					  // Timer initialization for dynamic display
	spi1_dma_control->DMA_CONFIG = 1; // turn on DMA1
	LT768_Init();					  // LT768 initialization
	
	Check_version();
#if !IIC_BUS
	SCI1_Init(g_ips_clk, gBaudrate,parity_flag);	
#endif

#if IIC_BUS
	I2C_Slave_TEST(0x36); // Initialize IIC slave device addressz
#endif
	Printf_Version();	// For debugging
	LT_RtcInit();
	Address_Index(); 		// Get address index


#if !IIC_BUS
	#if Touch_selection
		//IOCTRL->SCI_CONTROL_REG &= 0x80FFFFFF;
		//IOCTRL->SCI_CONTROL_REG |= (((u32)0x22) << 24); // Replace 4 IO ports with serial ports
		//LT_TpInit();//XPT2046
		Init_NS2009();//NS2009
		//LT_TpAdjust();
	#endif
#endif

	Display_ON();
	
	if (Start_page > addr_index[7])
		Display_page(0);
	else
		Display_page(Start_page);

#if !IIC_BUS
	#if !Touch_selection
		CTP_Init();
	#endif
	
	#if Touch_selection
		DelayMS(20);
	#endif
#endif
    LT768_PWM1_Init(1, 0, 5, 800, gDutyBuf[var[0x7001 * 2 + 1]]);
	
#if (UARTBUS_OPTION == 2)
	Get_ModBus_CMD_info();

#elif (UARTBUS_OPTION == 3)
	USB_HID_Init();
	
#elif (UARTBUS_OPTION == 4)
	USB_CDC_Init();
#endif	

    //BZ 
    SPI_ConfigGpio(SPI3, SPI_MOSI, GPIO_OUTPUT);
//    
//    while(1)
//    {
//        SPI_WriteGpioData(SPI3, SPI_MOSI, 1);
//        DelayMS(1500);
//        SPI_WriteGpioData(SPI3, SPI_MOSI, 0);
//        DelayMS(1500);
//    }

}

extern UINT8 hid_rx_buf[512];

int main(void)
{
	uint16_t cx, cy;
	uint8_t w_temp, h_temp;

	NVIC_SetVectorTable(0x800A000, 0);//0x8008000
	PeripheralInit();
	
	WDT_Init(0x4786*2);	
	while (1)
	{
		
		WDT_FeedDog(); //Clear watchdog
#if !IIC_BUS
#if (UARTBUS_OPTION == 0)
		Main_From_UartRxBuff();
		if (gUsartRx.Flag)
			LT_ReceiveCmd(gUsartRx.Buf);

#elif (UARTBUS_OPTION == 1)
		LT_ModBus_REG_Cmd();

#elif (UARTBUS_OPTION == 2)
		if (Sum_ModbusTX)
			Uart_cmd_Send();

		LT_ModBus_REG_Cmd();		
#elif (UARTBUS_OPTION == 3 || UARTBUS_OPTION == 4)
		
		Hid_Main_From_UartRxBuff();
		if (gHidRx.Flag)
			LT_Hid_ReceiveCmd(gHidRx.Buf);	
#else

#endif
#endif
	
#if IIC_BUS
		// Based on the IIC protocol, executes the functions of the registers
		LT_IIC_REG_Cmd();
#endif

		if (var[0x700e * 2 + 1])
			BackLight_control(); // Backlight control
		if (page_refreshing > 20)
		{
			page_refreshing = 0;
			Get_date_variable_Id_Regular_update(pic_id);
		}
		if(Count_reg_num > 0)
		{
			Count_reg_num--;
			reg_operation(Count_reg[Count_reg_num]);
		}
        
//		if (gWavFlag == 1 || gWavType == 1)
//			LT_PlayWav_DAC();
#if !IIC_BUS
		/*The current hardware that CTP and IIC slave use the same set of IOs
		If you use the IIC slave function, you need to block the touch function first*/
		gTpInfo.scan();
#endif

		show_gif();				// gif
		show_Clock();	 		// Analog clock
		show_RTC();		 		// Digital clock
		scroll_text();	 		// Text scroll1
		Display_Curve(); 		// Curve
		show_Count();			// Time counter
		Check_Var_Count();
		Check_Pointer();
#if encoder_on
		if (gEncoderFlag)
			encoder();
#endif
		if (Ges_sc_flag)
			slide_screen();		// gesture_sliding
		if (Gesture_flag)
			Gesture_touch(); 	// gesture_no_sliding
		
		Basic_touch();			// Basic touch control
		Adj_touch();			// Variable adjustment
		Progress_bar_sliding();	// Sliding progress bar
		data_input();			// Data input
		slideMune();			// Slide menu
		RingSld_touch();		// Ring progress bar with touch
		Ascii_input();			// ASCII keyboard
		GBK_input();			// GBK keyboard
		VarKey_touch();			// Multivariable adjustment
		pop_up_touch();			// Pop-up window

		/****Keyboard cursor display****/
		if (Cusor_time > 60)
		{
			Cusor_time = 0;
			if (DioKb_flag == 1)
			{
				if (Screen_DIR == 0)
				{
					if (gDio_Info[Dio_num].Alig == 0)
						cx = cur_x + input_w;
					else
						cx = cur_x;
					cy = cur_y;
					w_temp = cur_w;
					h_temp = gDio_Info[Dio_num].zk_h;
				}
				else if (Screen_DIR == 1)
				{
					if (gDio_Info[Dio_num].Alig == 0)
						cy = cur_y + input_w;
					else
						cy = cur_y;
					cx = cur_x;
					w_temp = gDio_Info[Dio_num].zk_h;
					h_temp = cur_w;
				}
				else if (Screen_DIR == 2)
				{
					if (gDio_Info[Dio_num].Alig == 0)
						cx = cur_x - input_w;
					else
						cx = cur_x;
					cy = cur_y;
					w_temp = cur_w;
					h_temp = gDio_Info[Dio_num].zk_h;
				}
				else if (Screen_DIR == 3)
				{
					if (gDio_Info[Dio_num].Alig == 0)
						cy = cur_y - input_w;
					else
						cy = cur_y;
					cx = cur_x;
					w_temp = gDio_Info[Dio_num].zk_h;
					h_temp = cur_w;
				}
			}
			else if (AioKb_flag == 1)
			{
				if (Screen_DIR == 0)
				{
					cx = cur_x + input_w;
					cy = cur_y;
					w_temp = cur_w;
					h_temp = gAio_Info[Aio_num].zk_h;
				}
				else if (Screen_DIR == 1)
				{
					cy = cur_y + input_w;
					cx = cur_x;
					w_temp = gAio_Info[Aio_num].zk_w;
					h_temp = cur_w;
				}
				else if (Screen_DIR == 2)
				{
					cx = cur_x - input_w;
					cy = cur_y;
					w_temp = cur_w;
					h_temp = gAio_Info[Aio_num].zk_h;
				}
				else if (Screen_DIR == 3)
				{
					cy = cur_y - input_w;
					cx = cur_x;
					w_temp = gAio_Info[Aio_num].zk_h;
					h_temp = cur_w;
				}
			}
			else if (GBKioKb_flag == 1)
			{
				if (Screen_DIR == 0)
				{
					cx = cur_x + input_w;
					cy = cur_y;
					w_temp = cur_w;
					h_temp = gGBKio_Info[GBKio_num].zk_h;
				}
				else if (Screen_DIR == 1)
				{
					cy = cur_y + input_w;
					cx = cur_x;
					w_temp = gGBKio_Info[GBKio_num].zk_w;
					h_temp = cur_w;
				}
				else if (Screen_DIR == 2)
				{
					cx = cur_x - input_w;
					cy = cur_y;
					w_temp = cur_w;
					h_temp = gGBKio_Info[GBKio_num].zk_h;
				}
				else if (Screen_DIR == 3)
				{
					cy = cur_y - input_w;
					cx = cur_x;
					w_temp = gGBKio_Info[GBKio_num].zk_h;
					h_temp = cur_w;
				}
			}
			if (Cusor_sw == 0)
			{
				Cusor_sw++;
				LT768_BTE_Memory_Copy(MAIN_BUFF, LCD_XSIZE_TFT, cx, cy,
									  MAIN_BUFF, LCD_XSIZE_TFT, cx, cy,
									  Display_Layer, LCD_XSIZE_TFT, cx, cy,
									  0x0c, w_temp, h_temp);
			}
			else
			{
				Cusor_sw = 0;
				LT768_BTE_Memory_Copy(LAY_BUFF5, LCD_XSIZE_TFT, cur_x, cur_y,
									  LAY_BUFF5, LCD_XSIZE_TFT, cur_x, cur_y,
									  Display_Layer, LCD_XSIZE_TFT, cx, cy,
									  0x0c, w_temp, h_temp);
			}
		}
	}
}
