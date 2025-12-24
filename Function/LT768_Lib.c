/*******************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     LT768_Lib.h
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief    LT768x All Function Functions
 ********************************************************************************/

#include "LT768_Lib.h"
#include "delay.h"
#include "uart.h"
#include "module_select.h"
#include "w25qxx.h"
#include "bsp.h"
#include "levetop.h"

uint16_t N_CCLK;
uint16_t N_MCLK;

uint8_t CCLK; // Core clock frequency of LT768
uint8_t MCLK; // Clock frequency of SDRAM
uint8_t SCLK; // Scan clock frequency of LCD

uint8_t TFT_color = 0; // Determine which color arrangement
uint16_t LCD_XSIZE_TFT = 800;
uint16_t LCD_YSIZE_TFT = 480;
uint16_t LCD_VBPD = 0;
uint16_t LCD_VFPD = 0;
uint16_t LCD_VSPW = 0;
uint16_t LCD_HBPD = 0;
uint16_t LCD_HFPD = 0;
uint16_t LCD_HSPW = 0;

uint8_t gPCLKRISING = 0;
uint8_t gHSYNCPolarity = 0;
uint8_t gVSYNCPolarity = 0;
uint8_t gDEPolarity = 0;

//---------------------------------------------------------------------------------------------------------------------------------

// Reset LT768
void LT768_HW_Reset(void)
{
	EPORT_ConfigGpio(EPORT_PIN7, GPIO_OUTPUT);

	EPORT_WriteGpioData(EPORT_PIN7, 0);
	DelayMS(100);
	EPORT_WriteGpioData(EPORT_PIN7, 1);
	DelayMS(100);
}

// Check LT768 system
void System_Check_Temp(void)
{
	uint8_t buf[1] = {0};
	uint16_t retry = 0;
	uint8_t i = 0;
	uint8_t temp = 0;
	uint8_t system_ok = 0;
	do
	{
		if ((LCD_StatusRead() & 0x02) == 0x00)
		{
			Delay_ms(1); // If MCU speed is too fast, use it if necessary
			LCD_CmdWrite(0x01);
			Delay_ms(1); // If MCU speed is too fast, use it if necessary
			temp = LCD_DataRead();
			if ((temp & 0x80) == 0x80) // Check whether CCR register PLL is ready
			{
				system_ok = 1;
				i = 0;
			}
			else
			{
				Delay_ms(1); // If MCU speed is too fast, use it if necessary
				LCD_CmdWrite(0x01);
				Delay_ms(1); // If MCU speed is too fast, use it if necessary
				LCD_DataWrite(0x80);
			}
		}
		else
		{
			system_ok = 0;
			i++;
		}
		if (system_ok == 0 && i == 5)
		{
			LT768_HW_Reset(); // note1
			i = 0;
		}
		retry++;
		if (retry >= 0XFFFE)
		{
			buf[0] = 0x0b;

			break;
		}
	} while (system_ok == 0);
}

void LT768_PLL_Initial(void)
{
	uint32_t temp = 0;
	uint32_t temp1;

	uint16_t lpllOD_sclk, lpllOD_cclk, lpllOD_mclk;
	uint16_t lpllR_sclk, lpllR_cclk, lpllR_mclk;
	uint16_t lpllN_sclk, lpllN_cclk, lpllN_mclk;

	temp = (LCD_HBPD + LCD_HFPD + LCD_HSPW + LCD_XSIZE_TFT) * (LCD_VBPD + LCD_VFPD + LCD_VSPW + LCD_YSIZE_TFT) * 60;

	temp1 = (temp % 1000000) / 100000;
	if (temp1 > 5)
		temp = temp / 1000000 + 1;
	else
		temp = temp / 1000000;

	SCLK = temp;

	CCLK = 100;
	MCLK = 120;
	if (SCLK > 65)
		SCLK = 65;
#if XI_4M

	lpllOD_sclk = 3;
	lpllOD_cclk = 3;
	lpllOD_mclk = 3;
	lpllR_sclk = 2;
	lpllR_cclk = 2;
	lpllR_mclk = 2;
	lpllN_mclk = 2 * MCLK;
	lpllN_cclk = 2 * CCLK;
	lpllN_sclk = 2 * SCLK;

	N_CCLK = lpllN_cclk;
	N_MCLK = lpllN_mclk;

#endif

#if XI_5M

	lpllOD_sclk = 3;
	lpllOD_cclk = 3;
	lpllOD_mclk = 3;
	lpllR_sclk = 5;
	lpllR_cclk = 5;
	lpllR_mclk = 5;
	lpllN_mclk = 4 * MCLK;
	lpllN_cclk = 4 * CCLK;
	lpllN_sclk = 4 * SCLK;

	N_CCLK = lpllN_cclk;
	N_MCLK = lpllN_mclk;

#endif

#if XI_6M

	lpllOD_sclk = 3;
	lpllOD_cclk = 3;
	lpllOD_mclk = 3;
	lpllR_sclk = 6;
	lpllR_cclk = 6;
	lpllR_mclk = 6;
	lpllN_mclk = 4 * MCLK;
	lpllN_cclk = 4 * CCLK;
	lpllN_sclk = 4 * SCLK;

	N_CCLK = lpllN_cclk;
	N_MCLK = lpllN_mclk;

#endif

#if XI_8M

	lpllOD_sclk = 3;
	lpllOD_cclk = 3;
	lpllOD_mclk = 3;
	lpllR_sclk = 2;
	lpllR_cclk = 2;
	lpllR_mclk = 2;
	lpllN_mclk = MCLK;
	lpllN_cclk = CCLK;
	lpllN_sclk = SCLK;

	N_CCLK = lpllN_cclk;
	N_MCLK = lpllN_mclk;

#endif

#if XI_10M

	lpllOD_sclk = 3;
	lpllOD_cclk = 3;
	lpllOD_mclk = 3;
	lpllR_sclk = 5;
	lpllR_cclk = 5;
	lpllR_mclk = 5;
	lpllN_mclk = 2 * MCLK;
	lpllN_cclk = 2 * CCLK;
	lpllN_sclk = 2 * SCLK;

	N_CCLK = lpllN_cclk;
	N_MCLK = lpllN_mclk;

#endif

#if XI_12M

	lpllOD_sclk = 3;
	lpllOD_cclk = 3;
	lpllOD_mclk = 3;
	lpllR_sclk = 3;
	lpllR_cclk = 3;
	lpllR_mclk = 3;
	lpllN_mclk = MCLK;
	lpllN_cclk = CCLK;
	lpllN_sclk = SCLK;

	N_CCLK = lpllN_cclk;
	N_MCLK = lpllN_mclk;
#endif

	LCD_CmdWrite(0x05);
	LCD_DataWrite((lpllOD_sclk << 6) | (lpllR_sclk << 1) | ((lpllN_sclk >> 8) & 0x1));
	LCD_CmdWrite(0x07);
	LCD_DataWrite((lpllOD_mclk << 6) | (lpllR_mclk << 1) | ((lpllN_mclk >> 8) & 0x1));
	LCD_CmdWrite(0x09);
	LCD_DataWrite((lpllOD_cclk << 6) | (lpllR_cclk << 1) | ((lpllN_cclk >> 8) & 0x1));

	LCD_CmdWrite(0x06);
	LCD_DataWrite(lpllN_sclk);
	LCD_CmdWrite(0x08);
	LCD_DataWrite(lpllN_mclk);
	LCD_CmdWrite(0x0a);
	LCD_DataWrite(lpllN_cclk);

	LCD_CmdWrite(0x00);
	DelayUS(1);
	LCD_DataWrite(0x80);

	DelayMS(1);
}

void LT768_SDRAM_initail(uint8_t mclk)
{
	uint16_t sdram_itv;

	LCD_RegisterWrite(0xe0, 0x29);
	LCD_RegisterWrite(0xe1, 0x03); // CAS:2=0x02?ACAS:3=0x03
	sdram_itv = (64000000 / 8192) / (1000 / mclk);
	sdram_itv -= 2;

	LCD_RegisterWrite(0xe2, sdram_itv);
	LCD_RegisterWrite(0xe3, sdram_itv >> 8);
	LCD_RegisterWrite(0xe4, 0x01);
	Check_SDRAM_Ready();
	DelayMS(1);
}

void Set_LCD_Panel(void)
{
	//**[01h]**//
	if (TFT_bitcolor == 0)
	{
		TFT_16bit();
		RGB_16b_16bpp();
	}
	else if (TFT_bitcolor == 1)
	{

		TFT_24bit();
		RGB_16b_24bpp_mode1();
	}

	// TFT_18bit();

	//	Host_Bus_8bit();    // Host bus 8bit
	Host_Bus_16bit(); 		// Host bus 16bit

	MemWrite_Left_Right_Top_Down();
	// MemWrite_Down_Top_Left_Right();

	//**[03h]**//
	Graphic_Mode();
	Memory_Select_SDRAM();

	VSCAN_T_to_B();			// REG[12h]:from top to bottom
	// VSCAN_B_to_T();		//From bottom to top

	switch (TFT_color)
	{
	case 0:
		PDATA_Set_RGB(); // REG[12h]:Select RGB output
		break;
		/*debug
			case 1:
			PDATA_Set_RBG();
			break;

			case 2:
			PDATA_Set_GRB();
			break;

			case 3:
			PDATA_Set_GBR();
			break;

			case 4:
			PDATA_Set_BRG();
			break;
		*/

	case 1:
		PDATA_Set_BGR();
		break;

	default:
		break;
	}

	if (gPCLKRISING == 0)
		PCLK_Rising();
	else
		PCLK_Falling(); 	// REG[12h]:Falling edge

	if (gHSYNCPolarity == 0)
		HSYNC_Low_Active(); // REG[13h]:
	else
		HSYNC_High_Active();

	if (gVSYNCPolarity == 0)
		VSYNC_Low_Active(); // REG[13h]:
	else
		VSYNC_High_Active();

	if (gDEPolarity == 0)
		DE_Low_Active();
	else
		DE_High_Active(); 
	//DE_Low_Active(); 
	LCD_HorizontalWidth_VerticalHeight(LCD_XSIZE_TFT, LCD_YSIZE_TFT);
	LCD_Horizontal_Non_Display(LCD_HBPD);
	LCD_HSYNC_Start_Position(LCD_HFPD);
	LCD_HSYNC_Pulse_Width(LCD_HSPW);
	LCD_Vertical_Non_Display(LCD_VBPD);
	LCD_VSYNC_Start_Position(LCD_VFPD);
	LCD_VSYNC_Pulse_Width(LCD_VSPW);

	Memory_XY_Mode(); // Block mode (X-Y coordination addressing);Block mode

	if (TFT_bitcolor == 0)
		Memory_16bpp_Mode();
	else if (TFT_bitcolor == 1)
		Memory_24bpp_Mode();
}

void LT768_initial(void)
{
	uint32_t ID;
	LT768_SPI_Init(1, 1);
	/*
	ID = LT_W25qxx_ReadID();
	MCU and Flash communication test reservation
	ID = Mcu_ReadID();
	printf("ID=%x \r\n",ID);
	*/
	if (W25N01GV_ReadID()==0xEFAA21)
	{
		Flash_type = 1;
	}
	else
	{
		Flash_type = 0;
		ID = LT_W25qxx_ReadID();
		if (ID == 0xEF18 || ID == 0xEF19)
		{

			nSS_Active();
			SPI_Master_FIFO_Data_Put(0xB7);
			nSS_Inactive();
			W25Q256 = 1;
		}  
	}

	LT_ReadParam();
#if 0 //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	LCD_XSIZE_TFT=480;
	LCD_YSIZE_TFT=854;
    LCD_HBPD=30;
	LCD_HFPD=12;
    LCD_HSPW=1;
	LCD_VBPD=30;
	LCD_VFPD=13;
	LCD_VSPW=6;
	gPCLKRISING = 0;
	gHSYNCPolarity = 0;
	gVSYNCPolarity = 0;
	gDEPolarity = 1;
	TFT_color = 0;// Determine which color arrangement
	TFT_bitcolor = 0;
	parity_flag = 0;
#endif
	LT768_PLL_Initial();
	LT768_SDRAM_initail(MCLK);
	Set_LCD_Panel();
}

extern u32 Readcan(void);
void LT768_Init(void)
{

	uint8_t buf[1] = {0};
	uint16_t retry = 0;
	Parallel_Init();
	Delay_ms(10);		 			// delay for LT768 power on
	LT768_HW_Reset();		 		// LT768 reset
	System_Check_Temp(); 			// Check whether the reset is successful
	Delay_ms(10);
	while (LCD_StatusRead() & 0x02) // Initial_Display_test	and  set SW2 pin2 = 1
	{
		retry++;
		if (retry >= 0XFFFE)
		{
			buf[0] = 0x07;
			break;
		}
	}

	LT768_initial();

	Select_SFI_Dual_Mode0(); // Use the fast read FLASH command (3Bh): see the read data instruction 3Bh of W25Q128

	if (TFT_bitcolor == 0)
		Select_Main_Window_16bpp();
	else if (TFT_bitcolor == 1)
		Select_Main_Window_24bpp();

	Main_Image_Start_Address(0);
	Main_Image_Width(LCD_XSIZE_TFT);
	Main_Window_Start_XY(0, 0);
	Canvas_Image_Start_address(0);
	Canvas_image_width(LCD_XSIZE_TFT);
	Active_Window_XY(0, 0);
	Active_Window_WH(LCD_XSIZE_TFT, LCD_YSIZE_TFT);
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void MPU8_8bpp_Memory_Write(
	uint16_t x 			// X coordinate
	,
	uint16_t y 			// Y coordinate
	,
	uint16_t w 			// width
	,
	uint16_t h 			// height
	,
	const uint8_t *data // Data header address
)
{
	uint16_t i, j;
	Graphic_Mode();
	Active_Window_XY(x, y);
	Active_Window_WH(w, h);
	Goto_Pixel_XY(x, y);
	LCD_CmdWrite(0x04);
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data);
			data++;
		}
	}
	Check_Mem_WR_FIFO_Empty();
}

void MPU8_16bpp_Memory_Write(
	uint16_t x 			// X coordinate
	,
	uint16_t y 			// Y coordinate
	,
	uint16_t w 			// width
	,
	uint16_t h 			// height
	,
	const uint8_t *data // Data header address
)
{
	uint16_t i, j;
	Graphic_Mode();
	Active_Window_XY(x, y);
	Active_Window_WH(w, h);
	Goto_Pixel_XY(x, y);
	LCD_CmdWrite(0x04);
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data);
			data++;
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data);
			data++;
		}
	}
	Check_Mem_WR_FIFO_Empty();
}

void MPU8_24bpp_Memory_Write(
	uint16_t x 			// X coordinate
	,
	uint16_t y 			// Y coordinate
	,
	uint16_t w 			// width
	,
	uint16_t h 			// height
	,
	const uint8_t *data // Data header address
)

{
	uint16_t i, j;
	Graphic_Mode();
	Active_Window_XY(x, y);
	Active_Window_WH(w, h);
	Goto_Pixel_XY(x, y);
	LCD_CmdWrite(0x04);
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data);
			data++;
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data);
			data++;
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data);
			data++;
		}
	}
	Check_Mem_WR_FIFO_Empty();
}

void MPU16_16bpp_Memory_Write(
	uint16_t x 			// X coordinate
	,
	uint16_t y 			// Y coordinate
	,
	uint16_t w 			// width
	,
	uint16_t h 			// height
	,
	const uint16_t *data // Data header address
)
{
	uint16_t i, j;
	Graphic_Mode();
	Active_Window_XY(x, y);
	Active_Window_WH(w, h);
	Goto_Pixel_XY(x, y);
	LCD_CmdWrite(0x04);

	SS_RESET;
	SPI2_ReadWriteByte(0x80);
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			LCD_DataWrite_Pixel(*data);
			data++;
		}
	}
	SS_SET;
	Check_Mem_WR_FIFO_Empty();
}

void MPU16_24bpp_Mode1_Memory_Write(
	uint16_t x 			// X coordinate
	,
	uint16_t y 			// Y coordinate
	,
	uint16_t w 			// width
	,
	uint16_t h 			// height
	,
	const uint16_t *data // Data header address
)
{
	uint16_t i, j;
	Graphic_Mode();
	Active_Window_XY(x, y);
	Active_Window_WH(w, h);
	Goto_Pixel_XY(x, y);
	LCD_CmdWrite(0x04);
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w / 2; j++)
		{
			LCD_DataWrite(*data);
			Check_Mem_WR_FIFO_not_Full();
			data++;
			LCD_DataWrite(*data);
			Check_Mem_WR_FIFO_not_Full();
			data++;
			LCD_DataWrite(*data);
			Check_Mem_WR_FIFO_not_Full();
			data++;
		}
	}
	Check_Mem_WR_FIFO_Empty();
}

void MPU16_24bpp_Mode2_Memory_Write(
	uint16_t x 			// X coordinate
	,
	uint16_t y 			// Y coordinate
	,
	uint16_t w 			// width
	,
	uint16_t h 			// height
	,
	const uint16_t *data // Data header address
)
{
	uint16_t i, j;
	Graphic_Mode();
	Active_Window_XY(x, y);
	Active_Window_WH(w, h);
	Goto_Pixel_XY(x, y);
	LCD_CmdWrite(0x04);
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data);
			data++;
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data);
			data++;
		}
	}
	Check_Mem_WR_FIFO_Empty();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void LT768_Drawpoint(
	uint16_t X1 		// X coordinate
	,
	uint16_t Y1 		// Y coordinate
	,
	uint32_t LineColor 	// line color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(LineColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(LineColor);
	Line_Start_XY(X1, Y1);
	Line_End_XY(X1, Y1);
	Start_Line();
	Check_2D_Busy();
}

int RGB(int r, int g, int b)
{
	u32 temp = 0;
	temp = (r << 16) | (g << 8) | b;
	return temp;
}

uint32_t Color_Val(float ratio, uint32_t Color_24)
{
	uint8_t r, g, b;
	r = ((Color_24 >> 11) & 0x1f) * ratio;
	g = ((Color_24 >> 5) & 0x3f) * ratio;
	b = ((Color_24 >> 0) & 0x1f) * ratio;

	return ((r << 11) + (g << 5) + b);
}

uint16_t Read_Color_16bpp(uint16_t X0, uint16_t Y0, uint32_t addr)
{
	uint16_t temp = 0;
	Canvas_Image_Start_address(addr); // Define read sdram start address
	Goto_Pixel_XY(X0, Y0);			  // Set coordinates
	LCD_CmdWrite(0x04);
	LCD_DataRead();

	temp = LCD_DataRead();
	temp = (LCD_DataRead() << 8) + temp;
	return temp;
}

//------------------------------------- line segment -----------------------------------------
void LT768_DrawLine(
	uint16_t X1 		// X1 coordinate
	,
	uint16_t Y1 		// Y1 coordinate
	,
	uint16_t X2 		// X2 coordinate
	,
	uint16_t Y2 		// Y2 coordinate
	,
	uint32_t LineColor 	// Line color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(LineColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(LineColor);
	Line_Start_XY(X1, Y1);
	Line_End_XY(X2, Y2);
	Start_Line();
	Check_2D_Busy();
}

void LT768_DrawLine_Width(
	uint16_t X1 		// X1 coordinate
	,
	uint16_t Y1 		// Y1 coordinate
	,
	uint16_t X2 		// X2 coordinate
	,
	uint16_t Y2 		// Y2 coordinate
	,
	uint32_t LineColor 	// Line color
	,
	uint16_t Width 		// Line width
)
{
	uint16_t i = 0;
	int16_t x = 0, y = 0;
	double temp = 0;
	x = X2 - X1;
	y = Y2 - Y1;
	if (x == 0)
		temp = 2;
	else
		temp = -((double)y / (double)x);
	if (temp >= -1 && temp <= 1)
	{
		while (Width--)
		{
			LT768_DrawLine(X1, Y1 + i, X2, Y2 + i, LineColor);
			i++;
		}
	}

	else
	{
		while (Width--)
		{
			LT768_DrawLine(X1 + i, Y1, X2 + i, Y2, LineColor);
			i++;
		}
	}
}

//------------------------------------- circle -----------------------------------------
void LT768_DrawCircle(
	uint16_t XCenter 		// Center X position
	,
	uint16_t YCenter 		// Center Y position
	,
	uint16_t R 				// Radius
	,
	uint32_t CircleColor 	// Line color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(CircleColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(CircleColor);
	Circle_Center_XY(XCenter, YCenter);
	Circle_Radius_R(R);
	Start_Circle_or_Ellipse();
	Check_2D_Busy();
}

void LT768_DrawCircle_Fill(
	uint16_t XCenter 			// Center X position
	,
	uint16_t YCenter 			// Center Y position
	,
	uint16_t R 					// Radius
	,
	uint32_t ForegroundColor 	// Background color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(ForegroundColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(ForegroundColor);
	Circle_Center_XY(XCenter, YCenter);
	Circle_Radius_R(R);
	Start_Circle_or_Ellipse_Fill();
	Check_2D_Busy();
}

void LT768_DrawCircle_Width(
	uint16_t XCenter 			// Center X position
	,
	uint16_t YCenter 			// Center Y position
	,	
	uint16_t R 					// Radius
	,	
	uint32_t CircleColor 		// Line color
	,
	uint32_t ForegroundColor 	// Background color
	,
	uint16_t Width 				// Line width
)
{
	LT768_DrawCircle_Fill(XCenter, YCenter, R + Width, CircleColor);
	LT768_DrawCircle_Fill(XCenter, YCenter, R, ForegroundColor);
}

//------------------------------------- Ellipse -----------------------------------------
void LT768_DrawEllipse
(
	uint16_t XCenter 		// X position of ellipse center
	,
	uint16_t YCenter 		// Y position of ellipse center
	,
	uint16_t X_R 			// Wide radius
	,
	uint16_t Y_R 			// major radius
	,
	uint32_t EllipseColor 	// Line width
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(EllipseColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(EllipseColor);
	Ellipse_Center_XY(XCenter, YCenter);
	Ellipse_Radius_RxRy(X_R, Y_R);
	Start_Circle_or_Ellipse();
	Check_2D_Busy();
}

void LT768_DrawEllipse_Fill(
	uint16_t XCenter 			// X position of ellipse center
	,
	uint16_t YCenter 			// Y position of ellipse center
	,
	uint16_t X_R 				// Wide radius
	,
	uint16_t Y_R 				// major radius
	,
	uint32_t ForegroundColor 	// Background color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(ForegroundColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(ForegroundColor);
	Ellipse_Center_XY(XCenter, YCenter);
	Ellipse_Radius_RxRy(X_R, Y_R);
	Start_Circle_or_Ellipse_Fill();
	Check_2D_Busy();
}

void LT768_DrawEllipse_Width(
	uint16_t XCenter 			// X position of ellipse center
	,
	uint16_t YCenter 			// Y position of ellipse center
	,
	uint16_t X_R 				// Wide radius
	,
	uint16_t Y_R 				// major radius
	,
	uint32_t EllipseColor 		// EllipseColor
	,
	uint32_t ForegroundColor 	// Background color
	,
	uint16_t Width 				// Line width
)
{
	LT768_DrawEllipse_Fill(XCenter, YCenter, X_R + Width, Y_R + Width, EllipseColor);
	LT768_DrawEllipse_Fill(XCenter, YCenter, X_R, Y_R, ForegroundColor);
}

//------------------------------------- Square -----------------------------------------
void LT768_DrawSquare(
	uint16_t X1 			// X1 position
	,	
	uint16_t Y1 			// Y1 position
	,
	uint16_t X2 			// X2 position
	,
	uint16_t Y2 			// Y2 position
	,
	uint32_t SquareColor 	// SquareColor
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(SquareColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(SquareColor);
	Square_Start_XY(X1, Y1);
	Square_End_XY(X2, Y2);
	Start_Square();
	Check_2D_Busy();
}

void LT768_DrawSquare_Fill(
	uint16_t X1 				// X1 position
	,	
	uint16_t Y1 				// Y1 position
	,
	uint16_t X2 				// X2 position
	,
	uint16_t Y2 				// Y2 position
	,
	uint32_t ForegroundColor 	// Background color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(ForegroundColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(ForegroundColor);
	Square_Start_XY(X1, Y1);
	Square_End_XY(X2, Y2);
	Start_Square_Fill();
	Check_2D_Busy();
}

void LT768_DrawSquare_Width(
	uint16_t X1 				// X1 position
	,	
	uint16_t Y1 				// Y1 position
	,
	uint16_t X2 				// X2 position
	,
	uint16_t Y2 				// Y2 position
	,
	uint32_t SquareColor 		// Line color
	,
	uint32_t ForegroundColor 	// Background color
	,
	uint16_t Width 				// Line width
)
{
	LT768_DrawSquare_Fill(X1 - Width, Y1 - Width, X2 + Width, Y2 + Width, SquareColor);
	LT768_DrawSquare_Fill(X1, Y1, X2, Y2, ForegroundColor);
}

//------------------------------------- Rounded rectangle -----------------------------------------
void LT768_DrawCircleSquare(
	uint16_t X1 				// X1 position
	,	
	uint16_t Y1 				// Y1 position
	,
	uint16_t X2 				// X2 position
	,
	uint16_t Y2 				// Y2 position
	,
	uint16_t X_R 				// Wide radius
	,
	uint16_t Y_R 				// major radius
	,
	uint32_t CircleSquareColor 	// Line color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(CircleSquareColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(CircleSquareColor);
	Square_Start_XY(X1, Y1);
	Square_End_XY(X2, Y2);
	Circle_Square_Radius_RxRy(X_R, Y_R);
	Start_Circle_Square();
	Check_2D_Busy();
}

void LT768_DrawCircleSquare_Fill(
	uint16_t X1 				// X1 position
	,	
	uint16_t Y1 				// Y1 position
	,
	uint16_t X2 				// X2 position
	,
	uint16_t Y2 				// Y2 position
	,
	uint16_t X_R 				// Wide radius
	,
	uint16_t Y_R 				// major radius
	,
	uint32_t ForegroundColor 	// Line color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(ForegroundColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(ForegroundColor);
	Square_Start_XY(X1, Y1);
	Square_End_XY(X2, Y2);
	Circle_Square_Radius_RxRy(X_R, Y_R);
	Start_Circle_Square_Fill();
	Check_2D_Busy();
}

void LT768_DrawCircleSquare_Width(
	uint16_t X1 				// X1 position
	,	
	uint16_t Y1 				// Y1 position
	,
	uint16_t X2 				// X2 position
	,
	uint16_t Y2 				// Y2 position
	,
	uint16_t X_R 				// Wide radius
	,
	uint16_t Y_R 				// major radius
	,
	uint32_t CircleSquareColor 	// Line color
	,
	uint32_t ForegroundColor 	// Line color
	,
	uint16_t Width 				// Width
)
{
	LT768_DrawCircleSquare_Fill(X1 - Width, Y1 - Width, X2 + Width, Y2 + Width, X_R, Y_R, CircleSquareColor);
	LT768_DrawCircleSquare_Fill(X1, Y1, X2, Y2, X_R, Y_R, ForegroundColor);
}

//------------------------------------- Triangle -----------------------------------------
void LT768_DrawTriangle(
	uint16_t X1 			// X1 position
	,
	uint16_t Y1 			// Y1 position
	,
	uint16_t X2 			// X2 position
	,
	uint16_t Y2 			// Y2 position
	,
	uint16_t X3 			// X3 position
	,
	uint16_t Y3 			// Y3 position
	,
	uint32_t TriangleColor 	// Line color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(TriangleColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(TriangleColor);
	Triangle_Point1_XY(X1, Y1); 
	Triangle_Point2_XY(X2, Y2);
	Triangle_Point3_XY(X3, Y3);
	Start_Triangle();
	Check_2D_Busy();
}

void LT768_DrawTriangle_Fill(
	uint16_t X1 				// X1 position
	,
	uint16_t Y1 				// Y1 position
	,	
	uint16_t X2 				// X2 position
	,
	uint16_t Y2 				// Y2 position
	,
	uint16_t X3 				// X3 position
	,
	uint16_t Y3 				// Y3 position
	,
	uint32_t ForegroundColor	// Line color
)
{
	
	if (TFT_bitcolor == 0)
		Foreground_color_65k(ForegroundColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(ForegroundColor);
	Triangle_Point1_XY(X1, Y1);
	Triangle_Point2_XY(X2, Y2);
	Triangle_Point3_XY(X3, Y3);
	Start_Triangle_Fill();
	Check_2D_Busy();
}

void LT768_DrawTriangle_Frame(
	uint16_t X1 				// X1 position
	,
	uint16_t Y1 				// Y1 position
	,	
	uint16_t X2 				// X2 position
	,
	uint16_t Y2 				// Y2 position
	,
	uint16_t X3 				// X3 position
	,
	uint16_t Y3 				// Y3 position
	,
	uint32_t TriangleColor		// Line color
	,
	uint32_t ForegroundColor 	// Background color
)
{
	LT768_DrawTriangle_Fill(X1, Y1, X2, Y2, X3, Y3, ForegroundColor);
	LT768_DrawTriangle(X1, Y1, X2, Y2, X3, Y3, TriangleColor);
}

//------------------------------------- Curve -----------------------------------------
void LT768_DrawLeftUpCurve(
	uint16_t XCenter 	// Curved center X position
	,
	uint16_t YCenter 	// Curved center Y position
	,
	uint16_t X_R 		// Wide radius
	,
	uint16_t Y_R 		// Major radius
	,
	uint32_t CurveColor // Line color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(CurveColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(CurveColor);
	Ellipse_Center_XY(XCenter, YCenter);
	Ellipse_Radius_RxRy(X_R, Y_R);
	Start_Left_Up_Curve();
	Check_2D_Busy();
}

void LT768_DrawLeftDownCurve(
	uint16_t XCenter 	// Curved center X position
	,
	uint16_t YCenter 	// Curved center Y position
	,
	uint16_t X_R 		// Wide radius
	,
	uint16_t Y_R 		// Major radius
	,
	uint32_t CurveColor // Line color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(CurveColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(CurveColor);
	Ellipse_Center_XY(XCenter, YCenter);
	Ellipse_Radius_RxRy(X_R, Y_R);
	Start_Left_Down_Curve();
	Check_2D_Busy();
}

void LT768_DrawRightUpCurve(
	uint16_t XCenter 	// Curved center X position
	,
	uint16_t YCenter 	// Curved center Y position
	,
	uint16_t X_R 		// Wide radius
	,
	uint16_t Y_R 		// Major radius
	,
	uint32_t CurveColor // Line color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(CurveColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(CurveColor);
	Ellipse_Center_XY(XCenter, YCenter);
	Ellipse_Radius_RxRy(X_R, Y_R);
	Start_Right_Up_Curve();
	Check_2D_Busy();
}

void LT768_DrawRightDownCurve(
	uint16_t XCenter 	// Curved center X position
	,
	uint16_t YCenter 	// Curved center Y position
	,
	uint16_t X_R 		// Wide radius
	,
	uint16_t Y_R 		// Major radius
	,
	uint32_t CurveColor // Line color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(CurveColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(CurveColor);
	Ellipse_Center_XY(XCenter, YCenter);
	Ellipse_Radius_RxRy(X_R, Y_R);
	Start_Right_Down_Curve();
	Check_2D_Busy();
}

void LT768_SelectDrawCurve(
	uint16_t XCenter 	// Curved center X position
	,
	uint16_t YCenter 	// Curved center Y position
	,
	uint16_t X_R 		// Wide radius
	,
	uint16_t Y_R 		// Major radius
	,
	uint32_t CurveColor // Line color
	,
	uint16_t Dir 		// Direction
)
{
	switch (Dir)
	{
	case 0:
		LT768_DrawLeftDownCurve(XCenter, YCenter, X_R, Y_R, CurveColor);
		break;
	case 1:
		LT768_DrawLeftUpCurve(XCenter, YCenter, X_R, Y_R, CurveColor);
		break;
	case 2:
		LT768_DrawRightUpCurve(XCenter, YCenter, X_R, Y_R, CurveColor);
		break;
	case 3:
		LT768_DrawRightDownCurve(XCenter, YCenter, X_R, Y_R, CurveColor);
		break;
	default:
		break;
	}
}

//------------------------------------- 1/4 solid ellipse -----------------------------------------
void LT768_DrawLeftUpCurve_Fill(
	uint16_t XCenter 			// Curved center X position
	,
	uint16_t YCenter 			// Curved center Y position
	,
	uint16_t X_R 				// Wide radius
	,
	uint16_t Y_R 				// Major radius
	,
	uint32_t ForegroundColor 	// Background color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(ForegroundColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(ForegroundColor);
	Ellipse_Center_XY(XCenter, YCenter);
	Ellipse_Radius_RxRy(X_R, Y_R);
	Start_Left_Up_Curve_Fill();
	Check_2D_Busy();
}

void LT768_DrawLeftDownCurve_Fill(
	uint16_t XCenter 			// Curved center X position
	,
	uint16_t YCenter 			// Curved center Y position
	,
	uint16_t X_R 				// Wide radius
	,
	uint16_t Y_R 				// Major radius
	,
	uint32_t ForegroundColor 	// Background color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(ForegroundColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(ForegroundColor);
	Ellipse_Center_XY(XCenter, YCenter);
	Ellipse_Radius_RxRy(X_R, Y_R);
	Start_Left_Down_Curve_Fill();
	Check_2D_Busy();
}

void LT768_DrawRightUpCurve_Fill(
	uint16_t XCenter 			// Curved center X position
	,
	uint16_t YCenter 			// Curved center Y position
	,
	uint16_t X_R 				// Wide radius
	,
	uint16_t Y_R 				// Major radius
	,
	uint32_t ForegroundColor 	// Background color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(ForegroundColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(ForegroundColor);
	Ellipse_Center_XY(XCenter, YCenter);
	Ellipse_Radius_RxRy(X_R, Y_R);
	Start_Right_Up_Curve_Fill();
	Check_2D_Busy();
}

void LT768_DrawRightDownCurve_Fill(
	uint16_t XCenter 			// Curved center X position
	,
	uint16_t YCenter 			// Curved center Y position
	,
	uint16_t X_R 				// Wide radius
	,
	uint16_t Y_R 				// Major radius
	,
	uint32_t ForegroundColor 	// Background color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(ForegroundColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(ForegroundColor);
	Ellipse_Center_XY(XCenter, YCenter);
	Ellipse_Radius_RxRy(X_R, Y_R);
	Start_Right_Down_Curve_Fill();
	Check_2D_Busy();
}

void LT768_SelectDrawCurve_Fill(
	uint16_t XCenter 	// Curved center X position
	,
	uint16_t YCenter 	// Curved center Y position
	,
	uint16_t X_R 		// Wide radius
	,
	uint16_t Y_R 		// Major radius
	,
	uint32_t CurveColor // Line color
	,
	uint16_t Dir 		// Direction
)
{
	switch (Dir)
	{
	case 0:
		LT768_DrawLeftDownCurve_Fill(XCenter, YCenter, X_R, Y_R, CurveColor);
		break;
	case 1:
		LT768_DrawLeftUpCurve_Fill(XCenter, YCenter, X_R, Y_R, CurveColor);
		break;
	case 2:
		LT768_DrawRightUpCurve_Fill(XCenter, YCenter, X_R, Y_R, CurveColor);
		break;
	case 3:
		LT768_DrawRightDownCurve_Fill(XCenter, YCenter, X_R, Y_R, CurveColor);
		break;
	default:
		break;
	}
}

//------------------------------------- Quadrilateral -----------------------------------------

void LT768_DrawQuadrilateral(
	uint16_t X1 				// X1 position
	,
	uint16_t Y1 				// Y1 position
	,
	uint16_t X2 				// X2 position
	,
	uint16_t Y2 				// Y2 position
	,
	uint16_t X3 				// X3 position
	,
	uint16_t Y3	 				// Y3 position
	,
	uint16_t X4 				// X4 position
	,
	uint16_t Y4 				// Y4 position
	,
	uint32_t ForegroundColor 	// Line color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(ForegroundColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(ForegroundColor);
	Triangle_Point1_XY(X1, Y1);
	Triangle_Point2_XY(X2, Y2);
	Triangle_Point3_XY(X3, Y3);
	Ellipse_Radius_RxRy(X4, Y4);

	LCD_CmdWrite(0x67);
	LCD_DataWrite(0x8d);
	Check_Busy_Draw();

	Check_2D_Busy();
}

void LT768_DrawQuadrilateral_Fill(
	uint16_t X1 				// X1 position
	,
	uint16_t Y1 				// Y1 position
	,
	uint16_t X2 				// X2 position
	,
	uint16_t Y2 				// Y2 position
	,
	uint16_t X3 				// X3 position
	,
	uint16_t Y3	 				// Y3 position
	,
	uint16_t X4 				// X4 position
	,
	uint16_t Y4 				// Y4 position
	,
	uint32_t ForegroundColor 	// Line color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(ForegroundColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(ForegroundColor);
	Triangle_Point1_XY(X1, Y1);
	Triangle_Point2_XY(X2, Y2);
	Triangle_Point3_XY(X3, Y3);
	Ellipse_Radius_RxRy(X4, Y4);

	LCD_CmdWrite(0x67);
	LCD_DataWrite(0xa7);
	Check_Busy_Draw();

	Check_2D_Busy();
}

//------------------------------------- Pentagon -----------------------------------------

void LT768_DrawPentagon(
	uint16_t X1 				// X1 position
	,
	uint16_t Y1 				// Y1 position
	,
	uint16_t X2 				// X2 position
	,
	uint16_t Y2 				// Y2 position
	,
	uint16_t X3 				// X3 position
	,
	uint16_t Y3	 				// Y3 position
	,
	uint16_t X4 				// X4 position
	,
	uint16_t Y4 				// Y4 position
	,
	uint16_t X5 				// X5 position
	,
	uint16_t Y5 				// Y5 position
	,
	uint32_t ForegroundColor 	// Line color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(ForegroundColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(ForegroundColor);
	Triangle_Point1_XY(X1, Y1);
	Triangle_Point2_XY(X2, Y2);
	Triangle_Point3_XY(X3, Y3);
	Ellipse_Radius_RxRy(X4, Y4);
	Ellipse_Center_XY(X5, Y5);

	LCD_CmdWrite(0x67);
	LCD_DataWrite(0x8F);
	Check_Busy_Draw();

	Check_2D_Busy();
}

void LT768_DrawPentagon_Fill(
	uint16_t X1 				// X1 position
	,
	uint16_t Y1 				// Y1 position
	,
	uint16_t X2 				// X2 position
	,
	uint16_t Y2 				// Y2 position
	,
	uint16_t X3 				// X3 position
	,
	uint16_t Y3	 				// Y3 position
	,
	uint16_t X4 				// X4 position
	,
	uint16_t Y4 				// Y4 position
	,
	uint16_t X5 				// X5 position
	,
	uint16_t Y5 				// Y5 position
	,
	uint32_t ForegroundColor 	// Line color
)
{
	if (TFT_bitcolor == 0)
		Foreground_color_65k(ForegroundColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(ForegroundColor);
	Triangle_Point1_XY(X1, Y1);
	Triangle_Point2_XY(X2, Y2);
	Triangle_Point3_XY(X3, Y3);
	Ellipse_Radius_RxRy(X4, Y4);
	Ellipse_Center_XY(X5, Y5);

	LCD_CmdWrite(0x67);
	LCD_DataWrite(0xa9);
	Check_Busy_Draw();

	Check_2D_Busy();
}

//------------------------------------- Cylinder -----------------------------------------
uint8_t LT768_DrawCylinder(
	uint16_t XCenter 			// Elliptical center X position
	,
	uint16_t YCenter 			// Elliptical center Y position
	,
	uint16_t X_R 				// Wide radius
	,
	uint16_t Y_R 				// Major radius
	,
	uint16_t H 					// Height
	,
	uint32_t CylinderColor 		// Line color
	,
	uint32_t ForegroundColor 	// Background color
	,
	uint8_t Width)
{
	uint8_t cut = 0;

	if (YCenter < H)
		return 1;

	LT768_DrawEllipse_Width(XCenter, YCenter, X_R, Y_R, CylinderColor, ForegroundColor, Width);

	LT768_DrawSquare_Fill(XCenter - X_R, YCenter - H, XCenter + X_R, YCenter, ForegroundColor);

	LT768_DrawEllipse_Width(XCenter, YCenter - H, X_R, Y_R, CylinderColor, ForegroundColor, Width);

	if (Width >= 1)
	{
		cut = 1;
		LT768_DrawSquare_Fill(XCenter + X_R + cut, YCenter - H, XCenter + X_R + Width, YCenter, CylinderColor);
		LT768_DrawSquare_Fill(XCenter - X_R - Width, YCenter - H, XCenter - X_R - cut, YCenter, CylinderColor);
	}
	return 0;
}

//------------------------------------- Quadrangular -----------------------------------------
void LT768_DrawQuadrangular(
	uint16_t X1, 
	uint16_t Y1, 
	uint16_t X2, 
	uint16_t Y2, 
	uint16_t X3, 
	uint16_t Y3, 
	uint16_t X4, 
	uint16_t Y4, 
	uint16_t X5, 
	uint16_t Y5, 
	uint16_t X6, 
	uint16_t Y6, 
	uint32_t QuadrangularColor, // Line color
	uint32_t ForegroundColor // Background color
)
{
	LT768_DrawSquare_Fill(X1, Y1, X5, Y5, ForegroundColor);
	LT768_DrawSquare(X1, Y1, X5, Y5, QuadrangularColor);

	LT768_DrawQuadrilateral_Fill(X1, Y1, X2, Y2, X3, Y3, X4, Y4, ForegroundColor);
	LT768_DrawQuadrilateral(X1, Y1, X2, Y2, X3, Y3, X4, Y4, QuadrangularColor);

	LT768_DrawQuadrilateral_Fill(X3, Y3, X4, Y4, X5, Y5, X6, Y6, ForegroundColor);
	LT768_DrawQuadrilateral(X3, Y3, X4, Y4, X5, Y5, X6, Y6, QuadrangularColor);
}

void LT768_DrawQuadrangular1(
	uint16_t X1 
	,
	uint16_t Y1 
	,
	uint16_t X2 
	,
	uint16_t Y2 
	,
	uint16_t W 		
	,
	uint16_t H 				
	,
	uint32_t QuadrangularColor 
	,
	uint32_t ForegroundColor 
	,
	uint8_t Width
)
{
	LT768_DrawSquare_Fill(X1, Y1, X1 + W, Y1 + H, ForegroundColor);
	LT768_DrawQuadrilateral_Fill(X1, Y1, X2, Y2, X2 + W, Y2, X1 + W, Y1, ForegroundColor);
	LT768_DrawQuadrilateral_Fill(X1 + W, Y1, X2 + W, Y2, X2 + W, Y2 + H, X1 + W, Y1 + H, ForegroundColor);

	LT768_DrawSquare(X1, Y1, X1 + W, Y1 + H, QuadrangularColor);
	LT768_DrawQuadrilateral(X1, Y1, X2, Y2, X2 + W, Y2, X1 + W, Y1, QuadrangularColor);
	LT768_DrawQuadrilateral(X1 + W, Y1, X2 + W, Y2, X2 + W, Y2 + H, X1 + W, Y1 + H, QuadrangularColor);
}



//--------------------------------------------------------------------------------------------------------------------------------------------

void LT768_Color_Bar_ON(void)
{
	Color_Bar_ON();
}

void LT768_Color_Bar_OFF(void)
{
	Color_Bar_OFF();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void LT768_DMA_24bit_Linear(
	uint8_t SCS 			// SPI   : SCS£º0       SCS£º1
	,
	uint8_t Clk 			// SPI clock division parameter : SPI Clock = System Clock /{(Clk+1)*2}
	,
	uint32_t flash_addr 	// Start address of flash reading data
	,
	uint32_t memory_addr 	// Start address of transmission to SDRAM
	,
	uint32_t data_num 		// Amount of data transferred
)
{
	
	Enable_SFlash_SPI(); // Enable SPI
	if (SCS == 0)
		Select_SFI_0(); // cs0
	if (SCS == 1)
		Select_SFI_1(); // cs1

	Memory_Linear_Mode();
	Select_SFI_DMA_Mode(); // Set DMA mode of SPI

	SPI_Clock_Period(Clk);							// SPI rate
	SFI_DMA_Destination_Start_Address(memory_addr); // Start address of memory
	SFI_DMA_Transfer_Number(data_num);				// Number of DMA transfers
	SFI_DMA_Source_Start_Address(flash_addr);		// Flash address
	Check_Busy_SFI_DMA();
	Start_SFI_DMA();
	Check_Busy_SFI_DMA();
	Memory_XY_Mode();
}

void LT768_DMA_32bit_Linear(
	uint8_t SCS 			// SPI   : SCS£º0       SCS£º1
	,
	uint8_t Clk 			// SPI clock division parameter : SPI Clock = System Clock /{(Clk+1)*2}
	,
	uint32_t flash_addr 	// Start address of flash reading data
	,
	uint32_t memory_addr 	// Start address of transmission to SDRAM
	,
	uint32_t data_num 		// Amount of data transferred
)
{
	Enable_SFlash_SPI(); 	// Enable SPI
	if (SCS == 0)
		Select_SFI_0(); 	// CS0
	if (SCS == 1)
		Select_SFI_1(); 	// CS1

	Memory_Linear_Mode();
	Select_SFI_DMA_Mode(); 	// Set DMA mode of SPI
	Select_SFI_32bit_Address();

	SPI_Clock_Period(Clk);							// SPI rate
	SFI_DMA_Destination_Start_Address(memory_addr); // Start address of memory
	SFI_DMA_Transfer_Number(data_num);				// Number of DMA transfers
	SFI_DMA_Source_Start_Address(flash_addr);		// Flash address
	Check_Busy_SFI_DMA();
	Start_SFI_DMA();
	Check_Busy_SFI_DMA();
	Memory_XY_Mode();
}

void LT768_DMA_32bit_Block(
	uint8_t SCS 	// SPI   : SCS£º0       SCS£º1
	,
	uint8_t Clk 	// SPI clock division parameter : SPI Clock = System Clock /{(Clk+1)*2}
	,
	uint16_t X1 	// Location of memory X1
	,
	uint16_t Y1 	// Location of memory Y1
	,
	uint16_t X_W 	// Width of DMA transmission data
	,
	uint16_t Y_H 	// Height of DMA transmission data
	,
	uint16_t P_W 	// Width of picture
	,
	uint32_t Addr 	// Flash address
)
{

	Enable_SFlash_SPI();
	if (SCS == 0)
		Select_SFI_0();
	if (SCS == 1)
		Select_SFI_1();

	Select_SFI_DMA_Mode();
	SPI_Clock_Period(Clk);

	Select_SFI_32bit_Address();

	Goto_Pixel_XY(X1, Y1);
	SFI_DMA_Destination_Upper_Left_Corner(X1, Y1);
	SFI_DMA_Transfer_Width_Height(X_W, Y_H);
	SFI_DMA_Source_Width(P_W);
	SFI_DMA_Source_Start_Address(Addr);

	Start_SFI_DMA();
	Check_Busy_SFI_DMA();
	Select_SFI_24bit_Address();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
/* First address of data cache */
void LT768_Select_Internal_Font_Init(
	uint8_t Size	// Set font size  16£º16*16     24:24*24    32:32*32
	,
	uint8_t XxN 	// Magnification factor of font width:1~4
	,
	uint8_t YxN 	// Height magnification of font£º1~4
	,
	uint8_t ChromaKey // 0£ºFont background color transparent    1£ºYou can set the background color of the font
	,
	uint8_t Alignment // 0£ºFont misalignment      1£ºFont Justification
)
{
	if (Size == 16)
		Font_Select_8x16_16x16();
	if (Size == 24)
		Font_Select_12x24_24x24();
	if (Size == 32)
		Font_Select_16x32_32x32();

	//(*)
	if (XxN == 1)
		Font_Width_X1();
	if (XxN == 2)
		Font_Width_X2();
	if (XxN == 3)
		Font_Width_X3();
	if (XxN == 4)
		Font_Width_X4();

	//(*)
	if (YxN == 1)
		Font_Height_X1();
	if (YxN == 2)
		Font_Height_X2();
	if (YxN == 3)
		Font_Height_X3();
	if (YxN == 4)
		Font_Height_X4();

	//(*)
	if (ChromaKey == 0)
		Font_Background_select_Color();
	if (ChromaKey == 1)
		Font_Background_select_Transparency();

	//(*)
	if (Alignment == 0)
		Disable_Font_Alignment();
	if (Alignment == 1)
		Enable_Font_Alignment();
}

/* Show internal integrated fonts */
void LT768_Print_Internal_Font_String(
	uint16_t x 					// The x position where the font starts to display
	,
	uint16_t y 					// The y position where the font starts to display
	,
	uint32_t FontColor 			// Font color
	,
	uint32_t BackGroundColor 	// The background color of the font£¨When transparent, setting this value is invalid£©
	,
	char *c 					// First address of data buffer
)
{
	Text_Mode();
	CGROM_Select_Internal_CGROM();

	if (TFT_bitcolor == 0)
		Foreground_color_65k(FontColor);
	else if (TFT_bitcolor == 1)
		Foreground_color_16M(FontColor);

	if (TFT_bitcolor == 0)
		Background_color_65k(BackGroundColor);
	else if (TFT_bitcolor == 1)
		Background_color_16M(BackGroundColor);
	Goto_Text_XY(x, y);
	Show_String(c);
}

//-----------------------------------------------------------------------------------------------------------------------------

void LT768_PIP_Init(
	uint8_t On_Off 		// 0: Disable PIP 1: Enable PIP 2: Keep the original state
	,
	uint8_t Select_PIP 	// 1: Use PIP1 2: Use PIP2
	,
	uint32_t PAddr 		// Start address of PIP
	,
	uint16_t XP 		// The X coordinate of the PIP window must be divided by 4
	,
	uint16_t YP 		// The Y coordinate of the PIP window must be divided by 4
	,
	uint32_t ImageWidth // Width of underlay
	,
	uint16_t X_Dis 		// Display the X coordinate of the window
	,
	uint16_t Y_Dis 		// Display the Y coordinate of the window
	,
	uint16_t X_W 		// The width of the display window must be divided by 4
	,
	uint16_t Y_H 		// The length of the display window must be divided by 4
)
{
	if (Select_PIP == 1)
	{
		if (TFT_bitcolor == 0)
			Select_PIP1_Window_16bpp();
		else if (TFT_bitcolor == 1)
			Select_PIP1_Window_24bpp();
		Select_PIP1_Parameter();
	}
	if (Select_PIP == 2)
	{
		if (TFT_bitcolor == 0)
			Select_PIP1_Window_16bpp();
		else if (TFT_bitcolor == 1)
			Select_PIP1_Window_24bpp();
		Select_PIP2_Parameter();
	}

	PIP_Display_Start_XY(X_Dis, Y_Dis);
	PIP_Image_Start_Address(PAddr);
	PIP_Image_Width(ImageWidth);
	PIP_Window_Image_Start_XY(XP, YP);
	PIP_Window_Width_Height(X_W, Y_H);

	if (On_Off == 0)
	{
		if (Select_PIP == 1)
			Disable_PIP1();
		if (Select_PIP == 2)
			Disable_PIP2();
	}

	if (On_Off == 1)
	{
		if (Select_PIP == 1)
			Enable_PIP1();
		if (Select_PIP == 2)
			Enable_PIP2();
	}
}

void LT768_Set_DisWindowPos(
	uint8_t On_Off 		// 0: Disable PIP 1: Enable PIP 2: Keep the original state
	,
	uint8_t Select_PIP 	// 1: Use PIP1 2: Use PIP2
	,
	uint16_t X_Dis 		// Display the X coordinate of the window
	,
	uint16_t Y_Dis 		// Display the Y coordinate of the window
)
{
	if (Select_PIP == 1)
		Select_PIP1_Parameter();
	if (Select_PIP == 2)
		Select_PIP2_Parameter();

	if (On_Off == 0)
	{
		if (Select_PIP == 1)
			Disable_PIP1();
		if (Select_PIP == 2)
			Disable_PIP2();
	}

	if (On_Off == 1)
	{
		if (Select_PIP == 1)
			Enable_PIP1();
		if (Select_PIP == 2)
			Enable_PIP2();
	}

	PIP_Display_Start_XY(X_Dis, Y_Dis);
}

//-----------------------------------------------------------------------------------------------------------------------------

void BTE_Solid_Fill(
	uint32_t Des_Addr 	// Filled destination address
	,
	uint16_t Des_W 		// Destination address picture width
	,
	uint16_t XDes 		// X coordinate
	,
	uint16_t YDes 		// Y coordinate
	,
	uint16_t color 		// Fill color
	,
	uint16_t X_W 		// Length of fill
	,
	uint16_t Y_H 		// Width of fill
)
{
	if (TFT_bitcolor == 0)
	{
		BTE_Destination_Color_16bpp();
		Foreground_color_65k(color);
	}
	else if (TFT_bitcolor == 1)
	{
		BTE_Destination_Color_24bpp();
		Foreground_color_16M(color);
	}

	BTE_Destination_Memory_Start_Address(Des_Addr);

	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);
	BTE_Window_Size(X_W, Y_H);

	BTE_Operation_Code(0x0c);
	BTE_Enable();
	Check_BTE_Busy();
}

/*  BTE memory replication combined with raster operation */
void LT768_BTE_Memory_Copy(
	uint32_t S0_Addr 	// Memory start address of SO image
	,
	uint16_t S0_W 		// S0 Width of image
	,
	uint16_t XS0 		// S0 Top left X coordinate of image
	,
	uint16_t YS0 		// S0 Upper left Y coordinate of image
	,
	uint32_t S1_Addr 	// Memory start address of S1 image
	,
	uint16_t S1_W 		// S1 Width of image
	,
	uint16_t XS1 		// Top left X coordinate of S1 image
	,
	uint16_t YS1 		// The upper left Y coordinate of S1 image
	,
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint32_t ROP_Code 	// Raster operation mode
	/*ROP_Code :
	   0000b		0(Blackness)
	   0001b		~S0!E~S1 or ~(S0+S1)
	   0010b		~S0!ES1
	   0011b		~S0
	   0100b		S0!E~S1
	   0101b		~S1
	   0110b		S0^S1
	   0111b		~S0 + ~S1 or ~(S0 + S1)
	   1000b		S0!ES1
	   1001b		~(S0^S1)
	   1010b		S1
	   1011b		~S0+S1
	   1100b		S0
	   1101b		S0+~S1
	   1110b		S0+S1
	   1111b		1(whiteness)*/
	,
	uint16_t X_W // The width of the active window
	,
	uint16_t Y_H // The length of the active window
)
{

	if (TFT_bitcolor == 0)
	{

		BTE_S0_Color_16bpp();
		BTE_S1_Color_16bpp();
		BTE_Destination_Color_16bpp();
	}
	else if (TFT_bitcolor == 1)
	{
		BTE_S0_Color_24bpp();
		BTE_S1_Color_24bpp();
		BTE_Destination_Color_24bpp();
	}

	BTE_S0_Memory_Start_Address(S0_Addr);
	BTE_S0_Image_Width(S0_W);
	BTE_S0_Window_Start_XY(XS0, YS0);

	BTE_S1_Memory_Start_Address(S1_Addr);
	BTE_S1_Image_Width(S1_W);
	BTE_S1_Window_Start_XY(XS1, YS1);

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_ROP_Code(ROP_Code);
	BTE_Operation_Code(0x02); // BTE Operation: Memory copy (move) with ROP.
	BTE_Window_Size(X_W, Y_H);
	BTE_Enable();
	Check_BTE_Busy();
}

void LT768_BTE_Memory_Copy_1(
	uint32_t S0_Addr 	// Memory start address of SO image
	,
	uint16_t S0_W 		// S0 Width of image
	,
	uint16_t XS0 		// S0 Top left X coordinate of image
	,
	uint16_t YS0 		// S0 Upper left Y coordinate of image
	,
	uint32_t S1_Addr 	// Memory start address of S1 image
	,
	uint16_t S1_W 		// S1 Width of image
	,
	uint16_t XS1 		// Top left X coordinate of S1 image
	,
	uint16_t YS1 		// The upper left Y coordinate of S1 image
	,
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint32_t ROP_Code 	// Raster operation mode
	/*ROP_Code :
	   0000b		0(Blackness)
	   0001b		~S0!E~S1 or ~(S0+S1)
	   0010b		~S0!ES1
	   0011b		~S0
	   0100b		S0!E~S1
	   0101b		~S1
	   0110b		S0^S1
	   0111b		~S0 + ~S1 or ~(S0 + S1)
	   1000b		S0!ES1
	   1001b		~(S0^S1)
	   1010b		S1
	   1011b		~S0+S1
	   1100b		S0
	   1101b		S0+~S1
	   1110b		S0+S1
	   1111b		1(whiteness)*/
	,
	uint16_t X_W // The width of the active window
	,
	uint16_t Y_H // The length of the active window
	,
	uint8_t Pic_flag
)
{

	if (TFT_bitcolor == 0 || Pic_flag == 2)
	{

		BTE_S0_Color_16bpp();
		BTE_S1_Color_16bpp();
		BTE_Destination_Color_16bpp();
	}
	else if (TFT_bitcolor == 1)
	{
		BTE_S0_Color_24bpp();
		BTE_S1_Color_24bpp();
		BTE_Destination_Color_24bpp();
	}
	
//	BTE_S0_Color_16bpp();
//	BTE_S1_Color_16bpp();
//	BTE_Destination_Color_16bpp();

	BTE_S0_Memory_Start_Address(S0_Addr);
	BTE_S0_Image_Width(S0_W);
	BTE_S0_Window_Start_XY(XS0, YS0);

	BTE_S1_Memory_Start_Address(S1_Addr);
	BTE_S1_Image_Width(S1_W);
	BTE_S1_Window_Start_XY(XS1, YS1);

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_ROP_Code(ROP_Code);
	BTE_Operation_Code(0x02); // BTE Operation: Memory copy (move) with ROP.
	BTE_Window_Size(X_W, Y_H);
	BTE_Enable();
	Check_BTE_Busy();
}

/*  Memory copy with Chroma Key (excluding ROP) */
void LT768_BTE_Memory_Copy_Chroma_key(
	uint32_t S0_Addr 	// Memory start address of SO image
	,
	uint16_t S0_W 		// S0 Width of image
	,
	uint16_t XS0 		// S0 Top left X coordinate of image
	,
	uint16_t YS0 		// S0 Upper left Y coordinate of image
	,
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint32_t Background_color // Transparent color
	,
	uint16_t X_W 		// The width of the active window
	,
	uint16_t Y_H 		// The length of the active window
)
{
	if (TFT_bitcolor == 0)
	{
		Background_color_65k(Background_color);
		BTE_S0_Color_16bpp();
		BTE_Destination_Color_16bpp();
	}
	else if (TFT_bitcolor == 1)
	{
		Background_color_16M(Background_color);
		BTE_S0_Color_24bpp();
		BTE_Destination_Color_24bpp();
	}

	BTE_S0_Memory_Start_Address(S0_Addr);
	BTE_S0_Image_Width(S0_W);
	BTE_S0_Window_Start_XY(XS0, YS0);

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_Operation_Code(0x05); // BTE Operation: Memory copy (move) with chroma keying (w/o ROP)
	BTE_Window_Size(X_W, Y_H);
	BTE_Enable();
	Check_BTE_Busy();
}

void LT768_BTE_Pattern_Fill(
	uint8_t P_8x8_or_16x16 // 0 : use 8x8 Icon , 1 : use 16x16 Icon.
	,
	uint32_t S0_Addr 	// Memory start address of SO image
	,
	uint16_t S0_W 		// S0 Width of image
	,
	uint16_t XS0 		// S0 Top left X coordinate of image
	,
	uint16_t YS0 		// S0 Upper left Y coordinate of image
	,
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint32_t ROP_Code 	// Raster operation mode
	/*ROP_Code :
	   0000b		0(Blackness)
	   0001b		~S0!E~S1 or ~(S0+S1)
	   0010b		~S0!ES1
	   0011b		~S0
	   0100b		S0!E~S1
	   0101b		~S1
	   0110b		S0^S1
	   0111b		~S0 + ~S1 or ~(S0 + S1)
	   1000b		S0!ES1
	   1001b		~(S0^S1)
	   1010b		S1
	   1011b		~S0+S1
	   1100b		S0
	   1101b		S0+~S1
	   1110b		S0+S1
	   1111b		1(whiteness)*/
	,
	uint16_t X_W 		// The width of the active window
	,
	uint16_t Y_H 		// The length of the active window
)
{
	if (P_8x8_or_16x16 == 0)
	{
		Pattern_Format_8X8();
	}
	if (P_8x8_or_16x16 == 1)
	{
		Pattern_Format_16X16();
	}

	if (TFT_bitcolor == 0)
	{
		BTE_S0_Color_16bpp();
		BTE_Destination_Color_16bpp();
	}
	else if (TFT_bitcolor == 1)
	{
		BTE_S0_Color_24bpp();
		BTE_Destination_Color_24bpp();
	}

	BTE_S0_Memory_Start_Address(S0_Addr);
	BTE_S0_Image_Width(S0_W);
	BTE_S0_Window_Start_XY(XS0, YS0);

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_ROP_Code(ROP_Code);
	BTE_Operation_Code(0x06); // BTE Operation: Pattern Fill with ROP.
	BTE_Window_Size(X_W, Y_H);
	BTE_Enable();
	Check_BTE_Busy();
}

void LT768_BTE_Pattern_Fill_With_Chroma_key(
	uint8_t P_8x8_or_16x16 // 0 : use 8x8 Icon , 1 : use 16x16 Icon.
	,
	uint32_t S0_Addr 	// Memory start address of SO image
	,
	uint16_t S0_W 		// S0 Width of image
	,
	uint16_t XS0 		// S0 Top left X coordinate of image
	,
	uint16_t YS0 		// S0 Upper left Y coordinate of image
	,
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint32_t ROP_Code 	// Raster operation mode
	/*ROP_Code :
	   0000b		0(Blackness)
	   0001b		~S0!E~S1 or ~(S0+S1)
	   0010b		~S0!ES1
	   0011b		~S0
	   0100b		S0!E~S1
	   0101b		~S1
	   0110b		S0^S1
	   0111b		~S0 + ~S1 or ~(S0 + S1)
	   1000b		S0!ES1
	   1001b		~(S0^S1)
	   1010b		S1
	   1011b		~S0+S1
	   1100b		S0
	   1101b		S0+~S1
	   1110b		S0+S1
	   1111b		1(whiteness)*/
	,
	uint32_t Background_color // Transparent color
	,
	uint16_t X_W 		// The width of the active window
	,
	uint16_t Y_H 		// The length of the active window
)
{

	if (P_8x8_or_16x16 == 0)
	{
		Pattern_Format_8X8();
	}
	if (P_8x8_or_16x16 == 1)
	{
		Pattern_Format_16X16();
	}

	if (TFT_bitcolor == 0)
	{
		BTE_S0_Color_16bpp();
		BTE_Destination_Color_16bpp();
		Background_color_65k(Background_color);
	}
	else if (TFT_bitcolor == 1)
	{
		BTE_S0_Color_24bpp();
		BTE_Destination_Color_24bpp();
		Background_color_16M(Background_color);
	}

	BTE_S0_Memory_Start_Address(S0_Addr);
	BTE_S0_Image_Width(S0_W);
	BTE_S0_Window_Start_XY(XS0, YS0);

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_ROP_Code(ROP_Code);
	BTE_Operation_Code(0x07); // BTE Operation: Pattern Fill with Chroma key.
	BTE_Window_Size(X_W, Y_H);
	BTE_Enable();
	Check_BTE_Busy();
}

void LT768_BTE_MCU_Write_MCU_16bit(
	uint32_t S1_Addr 	// Memory start address of S1 image
	,
	uint16_t S1_W 		// S1 Width of image
	,
	uint16_t XS1 		// Top left X coordinate of S1 image
	,
	uint16_t YS1 		// The upper left Y coordinate of S1 image
	,
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint32_t ROP_Code 	// Raster operation mode
	/*ROP_Code :
	   0000b		0(Blackness)
	   0001b		~S0!E~S1 or ~(S0+S1)
	   0010b		~S0!ES1
	   0011b		~S0
	   0100b		S0!E~S1
	   0101b		~S1
	   0110b		S0^S1
	   0111b		~S0 + ~S1 or ~(S0 + S1)
	   1000b		S0!ES1
	   1001b		~(S0^S1)
	   1010b		S1
	   1011b		~S0+S1
	   1100b		S0
	   1101b		S0+~S1
	   1110b		S0+S1
	   1111b		1(whiteness)*/
	,
	uint16_t X_W 		// The width of the active window
	,
	uint16_t Y_H 		// The length of the active window
	,
	const uint16_t *data // Data header address of S0
)
{
	uint16_t i, j;

	if (TFT_bitcolor == 0)
	{
		BTE_S1_Color_16bpp();
		BTE_Destination_Color_16bpp();
		BTE_S0_Color_16bpp();
	}
	else if (TFT_bitcolor == 1)
	{
		BTE_S1_Color_24bpp();
		BTE_Destination_Color_24bpp();
		BTE_S0_Color_24bpp();
	}

	BTE_S1_Memory_Start_Address(S1_Addr);
	BTE_S1_Image_Width(S1_W);
	BTE_S1_Window_Start_XY(XS1, YS1);

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_Window_Size(X_W, Y_H);
	BTE_ROP_Code(ROP_Code);
	BTE_Operation_Code(0x00); // BTE Operation: MPU Write with ROP.
	BTE_Enable();

	LCD_CmdWrite(0x04); // Memory Data Read/Write Port

	// MCU_16bit_ColorDepth_16bpp
	for (i = 0; i < Y_H; i++)
	{
		for (j = 0; j < (X_W); j++)
		{
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite_Pixel((*data));
			data++;
		}
	}
	Check_Mem_WR_FIFO_Empty();
	Check_BTE_Busy();
}

void LT768_BTE_MCU_Write_Chroma_key_MCU_16bit(
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint32_t Background_color // Transparent color
	,
	uint16_t X_W 		// The width of the active window
	,
	uint16_t Y_H 		// The length of the active window
	,
	const uint16_t *data // Data receiving address of S0
)
{
	uint32_t i, j;

	if (TFT_bitcolor == 0)
	{
		Background_color_65k(Background_color);
		BTE_Destination_Color_16bpp();
		BTE_S0_Color_16bpp();
	}
	else if (TFT_bitcolor == 1)
	{
		Background_color_16M(Background_color);
		BTE_Destination_Color_24bpp();
		BTE_S0_Color_24bpp();
	}

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_Window_Size(X_W, Y_H);
	BTE_Operation_Code(0x04); // BTE Operation: MPU Write with chroma keying (w/o ROP)
	BTE_Enable();

	LCD_CmdWrite(0x04); // Memory Data Read/Write Port

	// MCU_16bit_ColorDepth_16bpp
	for (i = 0; i < Y_H; i++)
	{
		for (j = 0; j < (X_W); j++)
		{
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite_Pixel((*data));
			data++;
		}
	}
	Check_Mem_WR_FIFO_Empty();
	Check_BTE_Busy();
}

/* MPU writing with extended color */
void LT768_BTE_MCU_Write_ColorExpansion_MCU_16bit(
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint16_t X_W 		// The width of the active window
	,
	uint16_t Y_H 		// The length of the active window
	,
	uint32_t Foreground_color // Foreground colorS
	/*Foreground_color : The source (1bit map picture) map data 1 translate to Foreground color by color expansion*/
	,
	uint32_t Background_color // Background color
	/*Background_color : The source (1bit map picture) map data 0 translate to Background color by color expansion*/
	,
	const uint16_t *data // First address of data cache
)
{

	uint16_t i, j;
	RGB_16b_16bpp();

	if (TFT_bitcolor == 0)
	{
		Foreground_color_65k(Foreground_color);
		Foreground_color_65k(Background_color);
		BTE_Destination_Color_16bpp();
	}
	else if (TFT_bitcolor == 1)
	{
		Foreground_color_16M(Foreground_color);
		Foreground_color_16M(Background_color);
		BTE_Destination_Color_24bpp();
	}

	BTE_ROP_Code(15);

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_Window_Size(X_W, Y_H);
	BTE_Operation_Code(0x8); // BTE Operation: MPU Write with Color Expansion (w/o ROP)
	BTE_Enable();

	LCD_CmdWrite(0x04); // Memory Data Read/Write Port
	for (i = 0; i < Y_H; i++)
	{
		for (j = 0; j < X_W / 16; j++)
		{
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite_Pixel(*data);
			data++;
		}
	}
	Check_Mem_WR_FIFO_Empty();
	Check_BTE_Busy();
}

/* MPU writing with extended color and Chroma key */
void LT768_BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_16bit(
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint16_t X_W 		// The width of the active window
	,
	uint16_t Y_H 		// The length of the active window
	,
	uint32_t Foreground_color // Foreground colorS
	/*Foreground_color : The source (1bit map picture) map data 1 translate to Foreground color by color expansion*/
	,
	const uint16_t *data // First address of data cache
)
{
	uint16_t i, j;

	if (TFT_bitcolor == 0)
	{
		RGB_16b_16bpp();
		Foreground_color_65k(Foreground_color);
		BTE_Destination_Color_16bpp();
	}
	else if (TFT_bitcolor == 1)
	{
		RGB_16b_24bpp_mode1();
		Foreground_color_16M(Foreground_color);
		BTE_Destination_Color_24bpp();
	}

	BTE_ROP_Code(15);

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_Window_Size(X_W, Y_H);
	BTE_Operation_Code(0x9); // BTE Operation: MPU Write with Color Expansion and chroma keying (w/o ROP)
	BTE_Enable();

	LCD_CmdWrite(0x04); // Memory Data Read/Write Port
	for (i = 0; i < Y_H; i++)
	{
		for (j = 0; j < X_W / 16; j++)
		{
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite_Pixel(*data);
			data++;
		}
	}
	Check_Mem_WR_FIFO_Empty();
	Check_BTE_Busy();
}

/* Memory replication with transparency */
void BTE_Alpha_Blending(
	uint32_t S0_Addr 	// Memory start address of SO image
	,
	uint16_t S0_W 		// S0 Width of image
	,
	uint16_t XS0 		// S0 Top left X coordinate of image
	,
	uint16_t YS0 		// S0 Upper left Y coordinate of image
	,
	uint32_t S1_Addr 	// Memory start address of S1 image
	,
	uint16_t S1_W 		// S1 Width of image
	,
	uint16_t XS1 		// Top left X coordinate of S1 image
	,
	uint16_t YS1 		// The upper left Y coordinate of S1 image
	,
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint16_t X_W 		// The width of the active window
	,
	uint16_t Y_H 		// The length of the active window
	,
	uint8_t alpha 		// Transparency level (level 32)
)
{
	if (TFT_bitcolor == 0)
	{
		BTE_S0_Color_16bpp();
		BTE_S1_Color_16bpp();
		BTE_Destination_Color_16bpp();
	}
	else if (TFT_bitcolor == 1)
	{
		BTE_S0_Color_24bpp();
		BTE_S1_Color_24bpp();
		BTE_Destination_Color_24bpp();
	}
	BTE_S0_Memory_Start_Address(S0_Addr);
	BTE_S0_Image_Width(S0_W);
	BTE_S0_Window_Start_XY(XS0, YS0);

	BTE_S1_Memory_Start_Address(S1_Addr);
	BTE_S1_Image_Width(S1_W);
	BTE_S1_Window_Start_XY(XS1, YS1);

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_Window_Size(X_W, Y_H);
	BTE_Operation_Code(0x0A); // BTE Operation: Memory write with opacity (w/o ROP)
	BTE_Alpha_Blending_Effect(alpha);
	BTE_Enable();
	Check_BTE_Busy();
}

//----------------------------------------------------------------------------------------------------------------------------------

void LT768_PWM0_Init(
	uint8_t on_off 			// 0: Disable PWM0    1: Enable PWM0
	,
	uint8_t Clock_Divided 	// PWM clock division  Value range 0~3(1,1/2,1/4,1/8)
	,
	uint8_t Prescalar 		// clock division     Value range 1~256
	,
	uint16_t Count_Buffer 	// PWM output period
	,
	uint16_t Compare_Buffer // Duty cycle
)
{
	Select_PWM0();
	Set_PWM_Prescaler_1_to_256(Prescalar);

	if (Clock_Divided == 0)
		Select_PWM0_Clock_Divided_By_1();
	if (Clock_Divided == 1)
		Select_PWM0_Clock_Divided_By_2();
	if (Clock_Divided == 2)
		Select_PWM0_Clock_Divided_By_4();
	if (Clock_Divided == 3)
		Select_PWM0_Clock_Divided_By_8();

	Set_Timer0_Count_Buffer(Count_Buffer);
	Set_Timer0_Compare_Buffer(Compare_Buffer);

	if (on_off == 1)
		Start_PWM0();
	if (on_off == 0)
		Stop_PWM0();
}

void LT768_PWM0_Duty(uint16_t Compare_Buffer)
{
	Set_Timer0_Compare_Buffer(Compare_Buffer);
}

void LT768_PWM1_Init(
	uint8_t on_off 			// 0: Disable PWM0    1: Enable PWM0
	,
	uint8_t Clock_Divided 	// PWM clock division  Value range 0~3(1,1/2,1/4,1/8)
	,
	uint8_t Prescalar 		// clock division     Value range 1~256
	,
	uint16_t Count_Buffer 	// PWM output period
	,
	uint16_t Compare_Buffer // Duty cycle
)
{
	Select_PWM1();
	Set_PWM_Prescaler_1_to_256(Prescalar);

	if (Clock_Divided == 0)
		Select_PWM1_Clock_Divided_By_1();
	if (Clock_Divided == 1)
		Select_PWM1_Clock_Divided_By_2();
	if (Clock_Divided == 2)
		Select_PWM1_Clock_Divided_By_4();
	if (Clock_Divided == 3)
		Select_PWM1_Clock_Divided_By_8();

	Set_Timer1_Count_Buffer(Count_Buffer);
	Set_Timer1_Compare_Buffer(Compare_Buffer);
	if (on_off == 1)
		Start_PWM1();
	if (on_off == 0)
		Stop_PWM1();
}

void LT768_PWM1_Duty(uint16_t Compare_Buffer)
{
	Set_Timer1_Compare_Buffer(Compare_Buffer);
}

//----------------------------------------------------------------------------------------------------------------------------------

// LT768 enters standby mode
void LT768_Standby(void)
{
	Power_Saving_Standby_Mode();
	Check_Power_is_Saving();
}
// Wake up from standby mode
void LT768_Wkup_Standby(void)
{
	Power_Normal_Mode();
	Check_Power_is_Normal();
}

void LT768_BTE_Memory_Copy_ColorExpansion_8
(
	uint32_t S0_Addr 	// Memory start address of SO image
	,
	uint16_t YS0 		// S0 Upper left Y coordinate of image
	,
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint16_t X_W 		// The width of the active window
	,
	uint16_t Y_H 		// The length of the active window
	,
	uint32_t Foreground_color // Foreground color
	,
	uint32_t Background_color // Background color
)
{
	Foreground_color_256(Foreground_color);
	Background_color_256(Background_color);
	BTE_ROP_Code(7);

	BTE_S0_Color_8bpp();
	BTE_S0_Memory_Start_Address(S0_Addr);
	BTE_S0_Image_Width(Des_W);
	BTE_S0_Window_Start_XY(YS0, 0);

	if (TFT_bitcolor == 0)
	{
		BTE_Destination_Color_16bpp();
	}
	else if (TFT_bitcolor == 1)
	{
		BTE_Destination_Color_24bpp();
	}

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_Operation_Code(0x0e); // BTE Operation: Memory copy (move) with chroma keying (w/o ROP)  0e
	BTE_Window_Size(X_W, Y_H);
	BTE_Enable();
	Check_BTE_Busy();
}

void LT768_BTE_Memory_Copy_ColorExpansion_Chroma_key_8
(
	uint32_t S0_Addr 	// Memory start address of SO image
	,
	uint16_t YS0 		// S0 Upper left Y coordinate of image
	,
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint16_t X_W 		// The width of the active window
	,
	uint16_t Y_H 		// The length of the active window
	,
	uint32_t Foreground_color // Foreground color
)
{
	Foreground_color_256(Foreground_color);
	Background_color_256(0);
	BTE_ROP_Code(7);

	BTE_S0_Color_8bpp();
	BTE_S0_Memory_Start_Address(S0_Addr);
	BTE_S0_Image_Width(Des_W);
	BTE_S0_Window_Start_XY(YS0, 0);

	if (TFT_bitcolor == 0)
	{
		BTE_Destination_Color_16bpp();
	}
	else if (TFT_bitcolor == 1)
	{
		BTE_Destination_Color_24bpp();
	}

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_Operation_Code(0x0f); // BTE Operation: Memory copy (move) with chroma keying (w/o ROP)  0e
	BTE_Window_Size(X_W, Y_H);
	BTE_Enable();
	Check_BTE_Busy();
}

void LT768_BTE_Memory_Copy_ColorExpansion_Chroma_key_8_2021(
	uint32_t S0_Addr 	// Memory start address of SO image
	,
	uint16_t XS0 		// S0 Upper left X coordinate of image
	,
	uint16_t YS0 		// S0 Upper left Y coordinate of image
	,
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint16_t X_W 		// The width of the active window
	,
	uint16_t Y_H 		// The length of the active window
	,
	uint32_t Foreground_color // Foreground color
)
{
	Foreground_color_256(Foreground_color);
	Background_color_256(0);
	BTE_ROP_Code(7);

	BTE_S0_Color_8bpp();
	BTE_S0_Memory_Start_Address(S0_Addr);
	BTE_S0_Image_Width(Des_W);
	BTE_S0_Window_Start_XY(XS0, YS0);

	if (TFT_bitcolor == 0)
	{
		BTE_Destination_Color_16bpp();
	}
	else if (TFT_bitcolor == 1)
	{
		BTE_Destination_Color_24bpp();
	}
	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_Operation_Code(0x0f); // BTE Operation: Memory copy (move) with chroma keying (w/o ROP)  0e
	BTE_Window_Size(X_W, Y_H);
	BTE_Enable();
	Check_BTE_Busy();
}

// LT768 enters pause mode
void LT768_Suspend(void)
{
	LT768_SDRAM_initail(10);
	Power_Saving_Suspend_Mode();
	Check_Power_is_Saving();
}
// Wake up from pause mode
void LT768_Wkup_Suspend(void)
{
	Power_Normal_Mode();
	Check_Power_is_Normal();
	LT768_SDRAM_initail(MCLK);
}

// LT768 enters sleep mode
void LT768_SleepMode(void)
{
	Power_Saving_Sleep_Mode();
	Check_Power_is_Saving();
}
// Wake up from sleep mode
void LT768_Wkup_Sleep(void)
{
	Power_Normal_Mode();
	Check_Power_is_Normal();
}

void LT768_SPI_Init(uint8_t div, uint8_t cs)
{
	if (cs == 0)
	{
		Select_SFI_0();
		Select_nSS_drive_on_xnsfcs0();
	}
	else
	{
		Select_SFI_1();
		Select_nSS_drive_on_xnsfcs1();
	}
	SPI_Clock_Period(div);
	Enable_SFlash_SPI();
}

//----------------------------------Custom font----------------------------------

void LT768_DMA_24bit_Block_Or_Line_16bpp
(
	uint8_t SCS // SPI   : SCS£º0       SCS£º1
	,
	uint8_t Clk // SPI clock division parameter : SPI Clock = System Clock /{(Clk+1)*2}
	,
	uint16_t X1 // Location transferred to memory X1
	,
	uint16_t Y1 // Location transferred to memory Y1
	,
	uint16_t X_W // Width of DMA transmission data
	,
	uint16_t Y_H // Height of DMA transmission data
	,
	uint16_t P_W // Width of picture
	,
	uint32_t Addr // Flash address
	,
	uint32_t lay0 // Address of layer
	,
	uint16_t canvas_w // Layer width
)
{
	if (UI_rotate == 1 || UI_rotate == 3)
		Addr += UI_rotate_addr;

	if (Flash_type == 0) 		// Norflash
	{
		if(TFT_bitcolor == 1 && pic_flag == 2) Memory_16bpp_Mode();
		Enable_SFlash_SPI(); 	// Enable SPI
		if (SCS == 0)
			Select_SFI_0(); 	// CS0
		if (SCS == 1)
			Select_SFI_1(); 	// CS1

		Select_SFI_DMA_Mode(); 	// Set DMA mode of SPI
		SPI_Clock_Period(Clk); 	// Set the frequency division coefficient of SPI

		if (W25Q256)
			Select_SFI_32bit_Address();

		Goto_Pixel_XY(X1, Y1);						   // Set the location of memory in graphics mode
		SFI_DMA_Destination_Upper_Left_Corner(X1, Y1); // Destination of DMA transmission (location of memory)
		SFI_DMA_Transfer_Width_Height(X_W, Y_H);	   // Set the width and height of block data
		SFI_DMA_Source_Width(P_W);					   // Set the width of the source data
		SFI_DMA_Source_Start_Address(Addr);			   // Set the address of source data in Flash

		Start_SFI_DMA();	  // Start DMA transmission
		Check_Busy_SFI_DMA(); // Detect whether DMA transmission is completed
		if(TFT_bitcolor == 1 && pic_flag == 2) Memory_24bpp_Mode();
	}
	else // NandFlash
	{
		uint16_t page, endpage, remainder, P_W4 = 0;
		uint16_t X_W4 = 0;
		
		
		
		
#if DualFlash
		if (Addr >= 2048*64*(1024-20))
		{
			Addr -= 2048*64*(1024-20);
			LT768_SPI_Init(1, 0);
			SCS = 0;
		}
		else 
		{
			LT768_SPI_Init(1, 1);
			SCS = 1;
		}
#endif		
		remainder = Addr % 2048; // The starting position of the picture on the page
		page = (Addr / 2048);	 // Page number of the starting address of the picture (page address)
		if (P_W % 4 != 0)
		{
			P_W4 = (P_W / 4) * 4 + 4;
		}
		else
		{
			P_W4 = P_W;
		}
		if (TFT_bitcolor == 0)
			endpage = ((Addr + P_W4 * Y_H * 2) / 2048); // Page number of the picture termination address (page address)
		else if (TFT_bitcolor == 1)
			endpage = ((Addr + P_W4 * Y_H * 3) / 2048); // Page number of the picture termination address (page address)
		LT768_SPI_Init(1, SCS);
		W25N01GV_ContinuousRead_Mode();
		Write_Page(page);
		LT768_DMA_24bit_Linear(SCS, Clk, 0, LAY_BUFF4 - remainder, ((endpage - page + 1) * 2048)); // Transfer all data of the page where the picture is on to layer 4
		Canvas_Image_Start_address(lay0);
		Canvas_image_width(canvas_w);

		if (X_W % 4 != 0)
		{
			X_W4 = (X_W / 4) * 4 + 4;
		}
		else
		{
			X_W4 = X_W;
		}
		
		LT768_BTE_Memory_Copy_1(LAY_BUFF4, P_W4, 0, 0,
									  LAY_BUFF4, P_W4, 0, 0,
									  lay0, canvas_w, X1, Y1,
									  0x0c, P_W, Y_H, pic_flag);
		/*
		if (TFT_bitcolor == 1)
		{
			if (pic_flag == 0 || pic_flag == 1)
			{
				LT768_BTE_Memory_Copy_1(LAY_BUFF4, P_W4, 0, 0,
									  LAY_BUFF4, P_W4, 0, 0,
									  lay0, canvas_w, X1, Y1,
									  0x0c, P_W, Y_H, pic_flag);
			}
			else if (pic_flag == 2)
			{
				LT768_BTE_Memory_Copy_16bit(LAY_BUFF4, P_W4, 0, 0,
											LAY_BUFF4, P_W4, 0, 0,
											lay0, canvas_w, X1, Y1,
											0x0c, P_W, Y_H);
			}
		}
		else if (TFT_bitcolor == 0)
		{
			LT768_BTE_Memory_Copy(LAY_BUFF4, P_W4, 0, 0,
								  LAY_BUFF4, P_W4, 0, 0,
								  lay0, canvas_w, X1, Y1,
								  0x0c, P_W, Y_H);
		}
		*/
		/*The macro definition part is temporarily reserved*/
		//		#if TFT_bitcolor
		//		if(pic_flag == 0 || pic_flag == 1)
		//		{
		//			LT768_BTE_Memory_Copy(LAY_BUFF4,P_W4,0,0,
		//															LAY_BUFF4,P_W4,0,0,
		//															lay0,canvas_w,X1,Y1,
		//															0x0c,X_W4,Y_H);
		//		}
		//		else if(pic_flag == 2 )
		//		{
		//			LT768_BTE_Memory_Copy_16bit(LAY_BUFF4,P_W4,0,0,
		//															LAY_BUFF4,P_W4,0,0,
		//															lay0,canvas_w,X1,Y1,
		//															0x0c,X_W4,Y_H);
		//		}
		//		#endif
		//
		//		#if !TFT_bitcolor
		//		LT768_BTE_Memory_Copy(LAY_BUFF4,P_W4,0,0,
		//															LAY_BUFF4,P_W4,0,0,
		//															lay0,canvas_w,X1,Y1,
		//															0x0c,X_W4,Y_H);
		//		#endif
	}
}

/*  BTE memory replication combined with raster operation */
void LT768_BTE_Memory_Copy_8(
	uint32_t S0_Addr 	// Memory start address of SO image
	,
	uint16_t S0_W 		// S0 Width of image
	,
	uint16_t XS0 		// S0 Top left X coordinate of image
	,
	uint16_t YS0 		// S0 Upper left Y coordinate of image
	,
	uint32_t S1_Addr 	// Memory start address of S1 image
	,
	uint16_t S1_W 		// S1 Width of image
	,
	uint16_t XS1 		// Top left X coordinate of S1 image
	,
	uint16_t YS1 		// The upper left Y coordinate of S1 image
	,
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint32_t ROP_Code 	// Raster operation mode
	/*ROP_Code :
	   0000b		0(Blackness)
	   0001b		~S0!E~S1 or ~(S0+S1)
	   0010b		~S0!ES1
	   0011b		~S0
	   0100b		S0!E~S1
	   0101b		~S1
	   0110b		S0^S1
	   0111b		~S0 + ~S1 or ~(S0 + S1)
	   1000b		S0!ES1
	   1001b		~(S0^S1)
	   1010b		S1
	   1011b		~S0+S1
	   1100b		S0
	   1101b		S0+~S1
	   1110b		S0+S1
	   1111b		1(whiteness)*/
	,
	uint16_t X_W // The width of the active window
	,
	uint16_t Y_H // The length of the active window
)
{
	BTE_S0_Color_8bpp();
	BTE_S0_Memory_Start_Address(S0_Addr);
	BTE_S0_Image_Width(S0_W);
	BTE_S0_Window_Start_XY(XS0, YS0);

	BTE_S1_Color_8bpp();
	BTE_S1_Memory_Start_Address(S1_Addr);
	BTE_S1_Image_Width(S1_W);
	BTE_S1_Window_Start_XY(XS1, YS1);

	BTE_Destination_Color_8bpp();
	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_ROP_Code(ROP_Code);
	BTE_Operation_Code(0x02); // BTE Operation: Memory copy (move) with ROP.
	BTE_Window_Size(X_W, Y_H);
	BTE_Enable();
	Check_BTE_Busy();
}


void BTE_Pixel_16bpp_Alpha_Blending // 16-bit png image display function
(
	uint32_t S0_Addr 	// Memory start address of SO image
	,
	uint16_t S0_W 		// S0 Width of image
	,
	uint16_t XS0 		// S0 Top left X coordinate of image
	,
	uint16_t YS0 		// S0 Upper left Y coordinate of image
	,
	uint32_t S1_Addr 	// Memory start address of S1 image
	,
	uint16_t S1_W 		// S1 Width of image
	,
	uint16_t XS1 		// Top left X coordinate of S1 image
	,
	uint16_t YS1 		// The upper left Y coordinate of S1 image
	,
	uint32_t Des_Addr 	// Memory start address of destination image
	,
	uint16_t Des_W 		// Width of destination image
	,
	uint16_t XDes 		// Top left X coordinate of destination image
	,
	uint16_t YDes 		// Top left Y coordinate of destination image
	,
	uint16_t X_W 		// The width of the active window
	,
	uint16_t Y_H 		// The length of the active window
)
{

	if (TFT_bitcolor == 0)
	{
		BTE_S0_Color_16bpp();
		BTE_S1_Color_16bit_Alpha();
		BTE_Destination_Color_16bpp();
	}
	else if (TFT_bitcolor == 1)
	{
		BTE_S0_Color_24bpp();
		BTE_S1_Color_16bit_Alpha();
		BTE_Destination_Color_24bpp();
	}
	BTE_S0_Memory_Start_Address(S0_Addr);
	BTE_S0_Image_Width(S0_W);
	BTE_S0_Window_Start_XY(XS0, YS0);

	BTE_S1_Memory_Start_Address(S1_Addr);
	BTE_S1_Image_Width(S1_W);
	BTE_S1_Window_Start_XY(XS1, YS1);
	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes, YDes);

	BTE_Window_Size(X_W, Y_H);
	BTE_Operation_Code(0x0A); // BTE Operation: Memory write with opacity (w/o ROP)
	BTE_Enable();
	Check_BTE_Busy();
}


u8 cSetb[8] = {cSetb0, cSetb1, cSetb2, cSetb3, cSetb4, cSetb5, cSetb6, cSetb7};
u8 cSetb_2bit[4] = {0xc0, 0x30, 0x0c, 0x03};
u8 cSetb_4bit[2] = {0xF0, 0x0F};
u8 fdata[8 * 128 * 2];
void LT768_Draw_16bit
(
	uint8_t SCS 		// SPI   : SCS£º0       SCS£º1
	,
	uint8_t Clk 		// SPI clock division parameter : SPI Clock = System Clock /{(Clk+1)*2}
	,
	uint16_t X1 		// Location transferred to memory X1
	,
	uint16_t Y1 		// Location transferred to memory Y1
	,
	uint16_t X_W	 	// Width of DMA transmission data
	,
	uint16_t Y_H 		// Height of DMA transmission data
	,
	uint16_t P_W 		// Width of picture
	,
	uint16_t pixel_format // Gray scale
	,
	uint32_t F_Color 	// Font color
	,
	uint32_t Addr 		// Flash address
	,
	uint32_t lay0 		// Address of layer
	,
	uint16_t canvas_w 	// Layer width
)
{
	uint16_t a, b = 0, c, d = 0, i, x1, y1;
	uint16_t color_temp;
	uint16_t temp;
	uint16_t r, g;


	Flash_Read_UI(fdata, Addr, X_W * Y_H);

	if (TFT_bitcolor == 0)
	{
		if (pixel_format > 0)
		{
			r = (F_Color >> 12) & 0x0f;
			g = (F_Color >> 7) & 0x0f;
			b = (F_Color >> 1) & 0x0f;
			F_Color = (((r << 8) + (g << 4) + b)) & 0x0FFF;
		}
	}
	else if (TFT_bitcolor == 1)
	{
		if (pixel_format > 0)
		{
			r = (F_Color >> 20) & 0x0f;
			g = (F_Color >> 12) & 0x0f;
			b = (F_Color >> 4) & 0x0f;
			F_Color = (((r << 8) + (g << 4) + b)) & 0x0FFF;
		}
	}
	if (F_Color == 0)
		F_Color += 1;

	Graphic_Mode();
	Canvas_Image_Start_address(lay0);
	Canvas_image_width(canvas_w);
	Active_Window_XY(X1, Y1);
	if (Screen_DIR == 0 || Screen_DIR == 2)
		Active_Window_WH(P_W, Y_H);
	else if (Screen_DIR == 1 || Screen_DIR == 3)
		Active_Window_WH(Y_H, P_W);

	Goto_Pixel_XY(X1, Y1);

	LCD_CmdWrite(0x04);
	SS_RESET;
	SPI2_ReadWriteByte(0x80);

	if (Screen_DIR == 0) // 0 degrees
	{
		if (pixel_format == 0)
		{
			for (a = 0; a < Y_H; a++)
			{
				for (c = 0; c < X_W; c++)
				{
					for (b = 0; b < 8; b++)
					{
						if (fdata[X_W * a + c] & cSetb[7 - b])
						{
							SPI2_WriteByte(F_Color);
							SPI2_WriteByte(F_Color >> 8);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(F_Color >> 16);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(0);
						}
					}
				}
			}
		}
		else if (pixel_format == 1)
		{
			for (a = 0; a < Y_H; a++)
			{
				for (c = 0; c < X_W; c++)
				{
					for (b = 0; b < 4; b++)
					{
						temp = (fdata[X_W * a + c] & cSetb_2bit[b]);
						if (b == 0)
							temp = temp >> 6;
						else if (b == 1)
							temp = temp >> 4;
						else if (b == 2)
							temp = temp >> 2;
						if (temp > 0)
						{
							if (temp == 1)
								color_temp = F_Color | (0x04 << 12);
							else if (temp == 2)
								color_temp = F_Color | (0x08 << 12);
							else if (temp == 3)
								color_temp = F_Color | (0x0F << 12);
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
		else if (pixel_format == 2)
		{
			for (a = 0; a < Y_H; a++)
			{
				for (c = 0; c < X_W; c++)
				{
					for (b = 0; b < 2; b++)
					{
						temp = (fdata[X_W * a + c] & cSetb_4bit[b]);
						if (b == 0)
							temp = temp >> 4;
						if (temp > 0)
						{
							color_temp = F_Color | (temp << 12);
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
	}
	else if (Screen_DIR == 1) // 90 degrees
	{
		if (pixel_format == 0)
		{
			for (c = 0; c < X_W; c++)
			{
				for (b = 0; b < 8; b++)
				{
					for (a = 0; a < Y_H; a++)
					{
						if (fdata[X_W * (Y_H - 1 - a) + c] & cSetb[7 - b])
						{
							SPI2_WriteByte(F_Color);
							SPI2_WriteByte(F_Color >> 8);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(F_Color >> 16);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(0);
						}
					}
				}
			}
		}
		else if (pixel_format == 1)
		{
			for (c = 0; c < X_W; c++)
			{
				for (b = 0; b < 4; b++)
				{
					for (a = 0; a < Y_H; a++)
					{
						temp = (fdata[X_W * (Y_H - 1 - a) + c] & cSetb_2bit[b]);

						if (b == 0)
							temp = temp >> 6;
						else if (b == 1)
							temp = temp >> 4;
						else if (b == 2)
							temp = temp >> 2;
						if (temp > 0)
						{
							switch (temp)
							{
							case 1:
								color_temp = F_Color | (0x04 << 12);
								break;
							case 2:
								color_temp = F_Color | (0x08 << 12);
								break;
							case 3:
								color_temp = F_Color | (0x0F << 12);
								break;
							default:
								break;
							}
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
		else if (pixel_format == 2)
		{
			for (c = 0; c < X_W; c++)
			{
				for (b = 0; b < 2; b++)
				{
					for (a = 0; a < Y_H; a++)
					{
						temp = (fdata[X_W * (Y_H - 1 - a) + c] & cSetb_4bit[b]);

						if (b == 0)
							temp = temp >> 4;
						if (temp > 0)
						{
							color_temp = F_Color | (temp << 12);
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
	}
	else if (Screen_DIR == 2) // 180 degrees
	{
		if (pixel_format == 0)
		{
			for (a = 0; a < Y_H; a++)
			{
				for (c = 0; c < X_W; c++)
				{
					for (b = 0; b < 8; b++)
					{
						if (fdata[X_W * (Y_H - 1 - a) + (X_W - 1 - c)] & cSetb[b])
						{
							SPI2_WriteByte(F_Color);
							SPI2_WriteByte(F_Color >> 8);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(F_Color >> 16);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(0);
						}
					}
				}
			}
		}
		else if (pixel_format == 1)
		{
			for (a = 0; a < Y_H; a++)
			{
				for (c = 0; c < X_W; c++)
				{
					for (b = 4; b > 0; b--)
					{
						temp = fdata[X_W * (Y_H - 1 - a) + (X_W - 1 - c)] & cSetb_2bit[b - 1];
						if (b == 1)
							temp = temp >> 6;
						else if (b == 2)
							temp = temp >> 4;
						else if (b == 3)
							temp = temp >> 2;
						if (temp > 0)
						{
							switch (temp)
							{
							case 1:
								color_temp = F_Color | (0x04 << 12);
								break;
							case 2:
								color_temp = F_Color | (0x08 << 12);
								break;
							case 3:
								color_temp = F_Color | (0x0F << 12);
								break;
							default:
								break;
							}
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
		else if (pixel_format == 2)
		{
			for (a = 0; a < Y_H; a++)
			{
				for (c = 0; c < X_W; c++)
				{
					for (b = 2; b > 0; b--)
					{
						temp = fdata[X_W * (Y_H - 1 - a) + (X_W - 1 - c)] & cSetb_4bit[b - 1];
						if (b == 1)
							temp = temp >> 4;
						if (temp > 0)
						{
							color_temp = F_Color | (temp << 12);
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
	}
	else if (Screen_DIR == 3) // 270 degrees
	{
		if (pixel_format == 0)
		{
			for (c = 0; c < X_W; c++)
			{
				for (b = 0; b < 8; b++)
				{
					for (a = 0; a < Y_H; a++)
					{
						if (fdata[X_W * a + (X_W - 1 - c)] & cSetb[b])
						{
							SPI2_WriteByte(F_Color);
							SPI2_WriteByte(F_Color >> 8);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(F_Color >> 16);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(0);
						}
					}
				}
			}
		}
		else if (pixel_format == 1)
		{
			for (c = 0; c < X_W; c++)
			{
				for (b = 4; b > 0; b--)
				{
					for (a = 0; a < Y_H; a++)
					{
						temp = fdata[X_W * a + (X_W - 1 - c)] & cSetb_2bit[b - 1];
						if (b == 1)
							temp = temp >> 6;
						else if (b == 2)
							temp = temp >> 4;
						else if (b == 3)
							temp = temp >> 2;
						if (temp > 0)
						{
							switch (temp)
							{
							case 1:
								color_temp = F_Color | (0x04 << 12);
								break;
							case 2:
								color_temp = F_Color | (0x08 << 12);
								break;
							case 3:
								color_temp = F_Color | (0x0F << 12);
								break;
							default:
								break;
							}
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
		else if (pixel_format == 2)
		{
			for (c = 0; c < X_W; c++)
			{
				for (b = 2; b > 0; b--)
				{
					for (a = 0; a < Y_H; a++)
					{
						temp = fdata[X_W * a + (X_W - 1 - c)] & cSetb_4bit[b - 1];
						if (b == 1)
							temp = temp >> 4;
						if (temp > 0)
						{
							color_temp = F_Color | (temp << 12);
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
	}
	SS_SET;
}

void LT768_Draw_16bit_2
(
	uint8_t SCS 		// SPI   : SCS£º0       SCS£º1
	,
	uint8_t Clk 		// SPI clock division parameter : SPI Clock = System Clock /{(Clk+1)*2}
	,
	uint16_t X1 		// Location transferred to memory X1
	,
	uint16_t Y1 		// Location transferred to memory Y1
	,
	uint16_t X_W	 	// Width of DMA transmission data
	,
	uint16_t Y_H 		// Height of DMA transmission data
	,
	uint16_t P_W 		// Width of picture
	,
	uint16_t pixel_format // Gray scale
	,
	uint32_t F_Color 	// Font color
	,
	uint32_t B_Color 	// Background color
	,
	uint8_t transparent // Show background se
	,
	uint32_t Addr 		// Flash address
	,
	uint32_t lay0 		// Address of layer
	,
	uint16_t canvas_w 	// Layer width
)
{
	uint16_t a, b, c, d, i, x1, y1;
	uint16_t color_temp, bcolor_temp;
	uint16_t temp;
	uint16_t r, g;

	Flash_Read_UI(fdata, Addr, X_W * Y_H);

	if (TFT_bitcolor == 0)
	{
		if (pixel_format > 0)
		{
			r = (F_Color >> 12) & 0x0f;
			g = (F_Color >> 7) & 0x0f;
			b = (F_Color >> 1) & 0x0f;
			F_Color = (((r << 8) + (g << 4) + b)) & 0x0FFF;
		}
	}
	else if (TFT_bitcolor == 1)
	{
		if (pixel_format > 0)
		{
			r = (F_Color >> 20) & 0x0f;
			g = (F_Color >> 12) & 0x0f;
			b = (F_Color >> 4) & 0x0f;
			F_Color = (((r << 8) + (g << 4) + b)) & 0x0FFF;
		}
	}
	if (F_Color == 0)
		F_Color += 1;

	if (B_Color == 0)
		B_Color += 1;

	if (transparent == 0)
		B_Color = 0;

	Graphic_Mode();
	Canvas_Image_Start_address(lay0);
	Canvas_image_width(canvas_w);
	Active_Window_XY(X1, Y1);
	if (Screen_DIR == 0 || Screen_DIR == 2)
		Active_Window_WH(P_W, Y_H);
	else if (Screen_DIR == 1 || Screen_DIR == 3)
		Active_Window_WH(Y_H, P_W);

	Goto_Pixel_XY(X1, Y1);
	LCD_CmdWrite(0x04);
	SS_RESET;
	SPI2_ReadWriteByte(0x80);

	if (Screen_DIR == 0) // 0 degrees
	{
		if (pixel_format == 0)
		{
			for (a = 0; a < Y_H; a++)
			{
				for (c = 0; c < X_W; c++)
				{
					for (b = 0; b < 8; b++)
					{
						if (fdata[X_W * a + c] & cSetb[7 - b])
						{
							SPI2_WriteByte(F_Color);
							SPI2_WriteByte(F_Color >> 8);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(F_Color >> 16);
						}
						else
						{
							SPI2_WriteByte(B_Color);
							SPI2_WriteByte(B_Color >> 8);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(B_Color >> 16);
						}
					}
				}
			}
		}
		else if (pixel_format == 1)
		{
			for (a = 0; a < Y_H; a++)
			{
				for (c = 0; c < X_W; c++)
				{
					for (b = 0; b < 4; b++)
					{
						temp = (fdata[X_W * a + c] & cSetb_2bit[b]);
						if (b == 0)
							temp = temp >> 6;
						else if (b == 1)
							temp = temp >> 4;
						else if (b == 2)
							temp = temp >> 2;
						if (temp > 0)
						{
							if (temp == 1)
								color_temp = F_Color | (0x04 << 12);
							else if (temp == 2)
								color_temp = F_Color | (0x08 << 12);
							else if (temp == 3)
								color_temp = F_Color | (0x0F << 12);
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
		else if (pixel_format == 2)
		{
			for (a = 0; a < Y_H; a++)
			{
				for (c = 0; c < X_W; c++)
				{
					for (b = 0; b < 2; b++)
					{
						temp = (fdata[X_W * a + c] & cSetb_4bit[b]);
						if (b == 0)
							temp = temp >> 4;
						if (temp > 0)
						{
							color_temp = F_Color | (temp << 12);
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
	}
	else if (Screen_DIR == 1) // 90 degrees
	{
		if (pixel_format == 0)
		{
			for (c = 0; c < X_W; c++)
			{
				for (b = 0; b < 8; b++)
				{
					for (a = 0; a < Y_H; a++)
					{
						if (fdata[X_W * (Y_H - 1 - a) + c] & cSetb[7 - b])
						{
							SPI2_WriteByte(F_Color);
							SPI2_WriteByte(F_Color >> 8);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(F_Color >> 16);
						}
						else
						{
							SPI2_WriteByte(B_Color);
							SPI2_WriteByte(B_Color >> 8);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(B_Color >> 16);
						}
					}
				}
			}
		}
		else if (pixel_format == 1)
		{
			for (c = 0; c < X_W; c++)
			{
				for (b = 0; b < 4; b++)
				{
					for (a = 0; a < Y_H; a++)
					{
						temp = (fdata[X_W * (Y_H - 1 - a) + c] & cSetb_2bit[b]);

						if (b == 0)
							temp = temp >> 6;
						else if (b == 1)
							temp = temp >> 4;
						else if (b == 2)
							temp = temp >> 2;
						if (temp > 0)
						{
							switch (temp)
							{
							case 1:
								color_temp = F_Color | (0x04 << 12);
								break;
							case 2:
								color_temp = F_Color | (0x08 << 12);
								break;
							case 3:
								color_temp = F_Color | (0x0F << 12);
								break;
							default:
								break;
							}
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
		else if (pixel_format == 2)
		{
			for (c = 0; c < X_W; c++)
			{
				for (b = 0; b < 2; b++)
				{
					for (a = 0; a < Y_H; a++)
					{
						temp = (fdata[X_W * (Y_H - 1 - a) + c] & cSetb_4bit[b]);

						if (b == 0)
							temp = temp >> 4;
						if (temp > 0)
						{
							color_temp = F_Color | (temp << 12);
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
	}
	else if (Screen_DIR == 2) // 180 degrees
	{
		if (pixel_format == 0)
		{
			for (a = 0; a < Y_H; a++)
			{
				for (c = 0; c < X_W; c++)
				{
					for (b = 0; b < 8; b++)
					{
						if (fdata[X_W * (Y_H - 1 - a) + (X_W - 1 - c)] & cSetb[b])
						{
							SPI2_WriteByte(F_Color);
							SPI2_WriteByte(F_Color >> 8);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(F_Color >> 16);
						}
						else
						{
							SPI2_WriteByte(B_Color);
							SPI2_WriteByte(B_Color >> 8);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(B_Color >> 16);
						}
					}
				}
			}
		}
		else if (pixel_format == 1)
		{
			for (a = 0; a < Y_H; a++)
			{
				for (c = 0; c < X_W; c++)
				{
					for (b = 4; b > 0; b--)
					{
						temp = fdata[X_W * (Y_H - 1 - a) + (X_W - 1 - c)] & cSetb_2bit[b - 1];
						if (b == 1)
							temp = temp >> 6;
						else if (b == 2)
							temp = temp >> 4;
						else if (b == 3)
							temp = temp >> 2;
						if (temp > 0)
						{
							switch (temp)
							{
							case 1:
								color_temp = F_Color | (0x04 << 12);
								break;
							case 2:
								color_temp = F_Color | (0x08 << 12);
								break;
							case 3:
								color_temp = F_Color | (0x0F << 12);
								break;
							default:
								break;
							}
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
		else if (pixel_format == 2)
		{
			for (a = 0; a < Y_H; a++)
			{
				for (c = 0; c < X_W; c++)
				{
					for (b = 2; b > 0; b--)
					{
						temp = fdata[X_W * (Y_H - 1 - a) + (X_W - 1 - c)] & cSetb_4bit[b - 1];
						if (b == 1)
							temp = temp >> 4;
						if (temp > 0)
						{
							color_temp = F_Color | (temp << 12);
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
	}
	else if (Screen_DIR == 3) // 270 degrees
	{
		if (pixel_format == 0)
		{
			for (c = 0; c < X_W; c++)
			{
				for (b = 0; b < 8; b++)
				{
					for (a = 0; a < Y_H; a++)
					{
						if (fdata[X_W * a + (X_W - 1 - c)] & cSetb[b])
						{
							SPI2_WriteByte(F_Color);
							SPI2_WriteByte(F_Color >> 8);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(F_Color >> 16);
						}
						else
						{
							SPI2_WriteByte(B_Color);
							SPI2_WriteByte(B_Color >> 8);
							if (TFT_bitcolor == 1)
								SPI2_WriteByte(B_Color >> 16);
						}
					}
				}
			}
		}
		else if (pixel_format == 1)
		{
			for (c = 0; c < X_W; c++)
			{
				for (b = 4; b > 0; b--)
				{
					for (a = 0; a < Y_H; a++)
					{
						temp = fdata[X_W * a + (X_W - 1 - c)] & cSetb_2bit[b - 1];
						if (b == 1)
							temp = temp >> 6;
						else if (b == 2)
							temp = temp >> 4;
						else if (b == 3)
							temp = temp >> 2;
						if (temp > 0)
						{
							switch (temp)
							{
							case 1:
								color_temp = F_Color | (0x04 << 12);
								break;
							case 2:
								color_temp = F_Color | (0x08 << 12);
								break;
							case 3:
								color_temp = F_Color | (0x0F << 12);
								break;
							default:
								break;
							}
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
		else if (pixel_format == 2)
		{
			for (c = 0; c < X_W; c++)
			{
				for (b = 2; b > 0; b--)
				{
					for (a = 0; a < Y_H; a++)
					{
						temp = fdata[X_W * a + (X_W - 1 - c)] & cSetb_4bit[b - 1];
						if (b == 1)
							temp = temp >> 4;
						if (temp > 0)
						{
							color_temp = F_Color | (temp << 12);
							SPI2_WriteByte(color_temp);
							SPI2_WriteByte(color_temp >> 8);
						}
						else
						{
							SPI2_WriteByte(0);
							SPI2_WriteByte(0);
						}
					}
				}
			}
		}
	}

	SS_SET;
}
