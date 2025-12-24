#include "rtp.h"
#include "i2c_drv.h"
#include "eflash_drv.h"
#include "uart_drv.h"

#include "iomacros.h"
u8 Rtp_Cor=0;
void LT_TpDelay(void)
{
	u8 t = 1;
	while(t--)
	{
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
	}

}

#define     TCLK_RESET                   I2C_WriteGpioData(I2C_SCL,0)	
#define     TCLK_SET                   	 I2C_WriteGpioData(I2C_SCL,1)
#define     TDIN_RESET                   I2C_WriteGpioData(I2C_SDA,0)
#define     TDIN_SET                     I2C_WriteGpioData(I2C_SDA,1)
#define     TCS_RESET                    EPORT_WriteGpioData(EPORT_PIN5,0)
#define     TCS_SET                 	 EPORT_WriteGpioData(EPORT_PIN5,1)

#define CMD_RDX 0XD0
#define CMD_RDY 0X90





//SPI写数据
//向触摸屏IC写入1byte数据
//num:要写入的数据
void TP_Write_Byte(u8 num)
{
	u8 count=0;
	for(count=0;count<8;count++)
	{
		if(num&0x80) TDIN_SET;
		else         TDIN_RESET;
		num<<=1;
		TCLK_RESET;
		Delay_us(8);
		TCLK_SET;
		Delay_us(8);
	}
}

//SPI读数据
//从触摸屏IC读取adc值
//CMD:指令
//返回值:读到的数据
//u16 TP_Read_AD(u8 CMD)
INT16 TP_Read_AD(CMD)
{
	u8 count=0; 	  
	u16 Num=0; 
	TCLK_RESET;												//先拉低时钟 	 
	TDIN_RESET; 												//拉低数据线
	
	TCS_RESET; 												//选中触摸屏IC
	
	TP_Write_Byte(CMD);	
	
	Delay_us(8);														//ADS7846的转换时间最长为6us
	TCLK_RESET; 	
    Delay_us(8);	
	TCLK_SET;	
	Delay_us(8);	   
	TCLK_RESET; 	
	Delay_us(8);	
	
	for(count=0;count<16;count++)						//读出16位数据,只有高12位有效 
	{ 				  
		Num<<=1; 	 
		TCLK_RESET;											//下降沿有效  	    	   
		Delay_us(8);
 		TCLK_SET;
		Delay_us(8);

 		if(EPORT_ReadGpioData(EPORT_PIN3))  Num++; 	
		//printf("eport=%d\r\n",EPORT_ReadGpioData(EPORT_PIN3));
	}  	
	Num>>=4;   	


	
	//只有高12位有效.
	TCS_SET;														//释放片选		
	return Num;   
}

//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值
//xy:指令（CMD_RDX/CMD_RDY）
//返回值:读到的数据
#define READ_TIMES 8	  								//读取次数
#define LOST_VAL 2	  									//丢弃值
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);
	for(i=0;i<READ_TIMES-1; i++)				 //排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])								 //升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;
}

//读取x,y坐标
//x,y:读取到的坐标值
u8 TP_Read_XY(u16 *x,u16 *y)
{
	*x=TP_Read_XOY(CMD_RDX);
	*y=TP_Read_XOY(CMD_RDY);
//	printf("%d %d\r\n",*x,*y);
	if(*x>4095||*y>4095)	return 0;			 //读数失败
	
//	printf("%d %d\r\n",*x,*y);
	
	return 1;
}

//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.
//该函数能大大提高准确度
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
#define ERR_RANGE 50 									 //误差范围
u8 TP_Read_XY2(u16 *x,u16 *y)
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;
	flag=TP_Read_XY(&x1,&y1);
	if(flag==0)return(0);
	flag=TP_Read_XY(&x2,&y2);
	if(flag==0)return(0);
	if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
	&&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
	{
		*x=(x1+x2)/2;
		*y=(y1+y2)/2;
		return 1;
	}
	else
	{
		return 0;
	}
}


//显示校正点
void LT_TpDrawTouchPoint(u16 x,u16 y,u32 color)
{
	LT768_DrawLine(x-12,y,x+13,y,color); //横线
	LT768_DrawLine(x,y-12,x,y+13,color); //竖线
	LT768_DrawCircle(x,y,6,color);       //画中心圈
}
#define flh_sAddr 0x0807EE00
//保存校准参数
void LT_TpSaveAdjdata(void)
{
	u16 i=0;
	
	gTpInfo.flag=0x5a;
	EFLASH_Init(g_sys_clk/1000);
	EFLASH_SetWritePermission();
	
	for(i=0;i<256;i++)	//擦除后256Bytes 的FLASH
	{
		EFLASH_PageErase(flh_sAddr+i);
	}

	EFLASH_WordProg(flh_sAddr,   gTpInfo.flag);
	EFLASH_WordProg(flh_sAddr+20,gTpxfac.s[0]);
	EFLASH_WordProg(flh_sAddr+24,gTpxfac.s[1]);
	EFLASH_WordProg(flh_sAddr+28,gTpxfac.s[2]);
	EFLASH_WordProg(flh_sAddr+32,gTpxfac.s[3]);
	
	EFLASH_WordProg(flh_sAddr+36,gTpxoff.s[0]);
	EFLASH_WordProg(flh_sAddr+40,gTpxoff.s[1]);
	EFLASH_WordProg(flh_sAddr+44,gTpxoff.s[2]);
	EFLASH_WordProg(flh_sAddr+48,gTpxoff.s[3]);
	
	EFLASH_WordProg(flh_sAddr+52,gTpyfac.s[0]);
	EFLASH_WordProg(flh_sAddr+56,gTpyfac.s[1]);
	EFLASH_WordProg(flh_sAddr+60,gTpyfac.s[2]);
	EFLASH_WordProg(flh_sAddr+64,gTpyfac.s[3]);
	
	EFLASH_WordProg(flh_sAddr+68,gTpyoff.s[0]);
	EFLASH_WordProg(flh_sAddr+72,gTpyoff.s[1]);
	EFLASH_WordProg(flh_sAddr+76,gTpyoff.s[2]);
	EFLASH_WordProg(flh_sAddr+80,gTpyoff.s[3]);


}

//读取校准参数
u8 LT_TpGetAdjdata(void)
{
	gTpxfac.s[0]=IO_READ32(flh_sAddr+20);
	gTpxfac.s[1]=IO_READ32(flh_sAddr+24);
	gTpxfac.s[2]=IO_READ32(flh_sAddr+28);
	gTpxfac.s[3]=IO_READ32(flh_sAddr+32);

	gTpxoff.s[0]=IO_READ32(flh_sAddr+36);
	gTpxoff.s[1]=IO_READ32(flh_sAddr+40);
	gTpxoff.s[2]=IO_READ32(flh_sAddr+44);
	gTpxoff.s[3]=IO_READ32(flh_sAddr+48);

	gTpyfac.s[0]=IO_READ32(flh_sAddr+52);
	gTpyfac.s[1]=IO_READ32(flh_sAddr+56);
	gTpyfac.s[2]=IO_READ32(flh_sAddr+60);
	gTpyfac.s[3]=IO_READ32(flh_sAddr+64);

	gTpyoff.s[0]=IO_READ32(flh_sAddr+68);
	gTpyoff.s[1]=IO_READ32(flh_sAddr+72);
	gTpyoff.s[2]=IO_READ32(flh_sAddr+76);
	gTpyoff.s[3]=IO_READ32(flh_sAddr+80);

	if(IO_READ32(flh_sAddr)==0x5a)
	{
		gTpxfac.s[0]=IO_READ32(flh_sAddr+20);
		gTpxfac.s[1]=IO_READ32(flh_sAddr+24);
		gTpxfac.s[2]=IO_READ32(flh_sAddr+28);
		gTpxfac.s[3]=IO_READ32(flh_sAddr+32);

		gTpxoff.s[0]=IO_READ32(flh_sAddr+36);
		gTpxoff.s[1]=IO_READ32(flh_sAddr+40);
		gTpxoff.s[2]=IO_READ32(flh_sAddr+44);
		gTpxoff.s[3]=IO_READ32(flh_sAddr+48);

		gTpyfac.s[0]=IO_READ32(flh_sAddr+52);
		gTpyfac.s[1]=IO_READ32(flh_sAddr+56);
		gTpyfac.s[2]=IO_READ32(flh_sAddr+60);
		gTpyfac.s[3]=IO_READ32(flh_sAddr+64);

		gTpyoff.s[0]=IO_READ32(flh_sAddr+68);
		gTpyoff.s[1]=IO_READ32(flh_sAddr+72);
		gTpyoff.s[2]=IO_READ32(flh_sAddr+76);
		gTpyoff.s[3]=IO_READ32(flh_sAddr+80);

		gTpInfo.xfac=gTpxfac.xfac;
		gTpInfo.xoff=gTpxoff.xoff;
		gTpInfo.yfac=gTpyfac.yfac;
		gTpInfo.yoff=gTpyoff.yoff;

#if DBG_EN		
		printf("GETgTpInfo.xfac=%f\r\n",gTpInfo.xfac);
		printf("GETgTpInfo.xoff=%f\r\n",gTpInfo.xoff);
		printf("GETgTpInfo.yfac=%f\r\n",gTpInfo.yfac);
		printf("GETgTpInfo.yoff=%f\r\n",gTpInfo.yoff);
#endif
		
		
		return 1;
	}
	else										return 0;
}




//扫描TP
//返回：1->触摸  0->无触摸
u8 LT_TpScan(void)
{
	
	if(EPORT_ReadGpioData(EPORT_PIN4)==0 )//&& Rtp_temp==1)
	{
		//DelayMS(1);
		//TP_Read_XY2(&gTpInfo.px,&gTpInfo.py);
		
		if(	TP_Read_XY2(&gTpInfo.px,&gTpInfo.py))
		{
			gTpInfo.x[0] = gTpInfo.xfac * gTpInfo.px + gTpInfo.xoff;
			gTpInfo.y[0] = gTpInfo.yfac * gTpInfo.py + gTpInfo.yoff;
		}
		if(gTpInfo.x[0]>LCD_XSIZE_TFT)	gTpInfo.x[0] = LCD_XSIZE_TFT;
		if(gTpInfo.y[0]>LCD_YSIZE_TFT)	gTpInfo.y[0] = LCD_YSIZE_TFT;
		if((gTpInfo.x[0]==0 || gTpInfo.x[0]==LCD_XSIZE_TFT) && Rtp_Cor==0)  return 0;
		if(First_press==0)
		{
			First_press=1;
			First_pressX= gTpInfo.x[0];
			First_pressY= gTpInfo.y[0];		
    	//printf("First_pressX=%d    First_pressY=%d\r\n",gTpInfo.x[0],gTpInfo.y[0]);			
		}
		//printf("x=%d    y=%d\r\n",gTpInfo.x[0],gTpInfo.y[0]);

		if(gTpInfo.sta == 0)	gTpInfo.sta = 1;
		return 1;
 	}	
	else 
	{   
		First_press=0;
		gTpInfo.sta = 0;
		//		printf("x=%d    y=%d\r\n",gTpInfo.x[0],gTpInfo.y[0]);
		//	printf("gTpInfo.sta=%d \r\n",gTpInfo.sta);
		return 0;
	}

}


//提示字符串
u8* const TP_REMIND_MSG_TBL=(u8 *)"Please correct the resistance screen";


//触摸屏校准代码
//得到四个校准参数
u8 LT_TpAdjust(void)
{
	u8 cnt = 0;
	u16 outTime = 0;
	u16 d1,d2;
	u32 temp1,temp2;
	u16 posTemp[2][4] = {0};
	float fac;
	u8 flag=1;
	U16 buff_coord[2];

	Display_ON(); 
	Delay_ms(100);
	LT768_PWM1_Init(1,0,5,800,450);	
	
	Canvas_Image_Start_address(0);
	Canvas_image_width(LCD_XSIZE_TFT);
	LT768_DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,White);
	LT768_Select_Internal_Font_Init(32,1,1,0,0);
	LT768_Print_Internal_Font_String(40,40,Red,White,(char*)TP_REMIND_MSG_TBL);

	LT_TpDrawTouchPoint(20,20,Red);

	while(1)
	{
	// LT_TpScan();
	//	LT_TpScan1();
	//	read_data(buff_coord);
    //	printf("SDO=%X %d\r\n",EPORT->EPPDR,EPORT_ReadGpioData(EPORT_PIN4));
    //	Delay_ms(100);
    //	Delay_ms(100);
		
   if(EPORT_ReadGpioData(EPORT_PIN4)==0)
   { 
		if(flag==1)
		{
			   if(read_data(buff_coord)==1 )
			   {
				   gTpInfo.sta=1;
			   }
		}
		   flag=0;
   }
   else flag=1;
 
		if(gTpInfo.sta==1)
		{
		
			outTime = 0;
			gTpInfo.sta = 2;

			posTemp[0][cnt] = gTpInfo.px;
			posTemp[1][cnt] = gTpInfo.py;
			cnt++;
			switch(cnt)
			{
				case 1:	LT768_DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,White);
								      LT_TpDrawTouchPoint(LCD_XSIZE_TFT-20,20,Red);
								break;

				case 2: LT768_DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,White);
								LT_TpDrawTouchPoint(20,LCD_YSIZE_TFT-20,Red);
								break;

				case 3: LT768_DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,White);
								LT_TpDrawTouchPoint(LCD_XSIZE_TFT-20,LCD_YSIZE_TFT-20,Red);
								break;

				case 4: //计算上下两条边长的比例
								temp1=abs(posTemp[0][0]-posTemp[0][1]);
								temp2=abs(posTemp[1][0]-posTemp[1][1]);
								temp1*=temp1;
								temp2*=temp2;
								d1=sqrt(temp1+temp2);

				                temp1=abs(posTemp[0][2]-posTemp[0][3]);
								temp2=abs(posTemp[1][2]-posTemp[1][3]);
								temp1*=temp1;
								temp2*=temp2;
								d2=sqrt(temp1+temp2);								
								fac = (float)d1/d2;
								
								if(fac<0.95f||fac>1.05f||d1==0||d2==0)
								{
									cnt=0;
									LT_TpDrawTouchPoint(LCD_XSIZE_TFT-20,LCD_YSIZE_TFT-20,White);
									LT_TpDrawTouchPoint(20,20,Red);
									LT768_Print_Internal_Font_String(200,180,Red,White,"Check Failure,Please recheck!");
									continue;
								}

								//计算左右两条边长的比例
								temp1=abs(posTemp[0][0]-posTemp[0][2]);
								temp2=abs(posTemp[1][0]-posTemp[1][2]);
								temp1*=temp1;
								temp2*=temp2;
								d1=sqrt(temp1+temp2);

				                temp1=abs(posTemp[0][1]-posTemp[0][3]);
								temp2=abs(posTemp[1][1]-posTemp[1][3]);
								temp1*=temp1;
								temp2*=temp2;
								d2=sqrt(temp1+temp2);

								if(fac<0.95f||fac>1.05f||d1==0||d2==0)
								{
									cnt=0;
									LT_TpDrawTouchPoint(LCD_XSIZE_TFT-20,LCD_YSIZE_TFT-20,White);
									LT_TpDrawTouchPoint(20,20,Red);
									LT768_Print_Internal_Font_String(200,180,Red,White,"Check Failure,Please recheck!");
									continue;
								}

								//计算对边长度的比例
								temp1=abs(posTemp[0][0]-posTemp[0][3]);
								temp2=abs(posTemp[1][0]-posTemp[1][3]);
								temp1*=temp1;
								temp2*=temp2;
								d1=sqrt(temp1+temp2);

				                temp1=abs(posTemp[0][1]-posTemp[0][2]);
								temp2=abs(posTemp[1][1]-posTemp[1][2]);
								temp1*=temp1;
								temp2*=temp2;
								d2=sqrt(temp1+temp2);

								fac = (float)d1/d2;
								
								if(fac<0.95f||fac>1.05f||d1==0||d2==0)    //不合格
								{
									cnt=0;
									LT_TpDrawTouchPoint(LCD_XSIZE_TFT-20,LCD_YSIZE_TFT-20,White);
									LT_TpDrawTouchPoint(20,20,Red);
									LT768_Print_Internal_Font_String(200,180,Red,White,"Check Failure,Please recheck!");
									continue;
								}

								//上下两边比例、左右两边比例、对角边的比例，都在范围内，则4个校正点符合要求
								gTpInfo.xfac=(float)(LCD_XSIZE_TFT-40)/(posTemp[0][1]-posTemp[0][0]);
								gTpInfo.xoff=(LCD_XSIZE_TFT-gTpInfo.xfac*(posTemp[0][1]+posTemp[0][0]))/2;
								gTpInfo.yfac=(float)(LCD_YSIZE_TFT-40)/(posTemp[1][2]-posTemp[1][0]);
								gTpInfo.yoff=(LCD_YSIZE_TFT-gTpInfo.yfac*(posTemp[1][2]+posTemp[1][0]))/2;
								
								gTpxfac.xfac=gTpInfo.xfac;
								gTpxoff.xoff=gTpInfo.xoff;
								gTpyfac.yfac=gTpInfo.yfac;
								gTpyoff.yoff=gTpInfo.yoff;
								LT768_DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,White);
								LT768_Print_Internal_Font_String(200,180,Red,White,"Touch Screen Adjust OK!");	//校正成功
								Delay_ms(1000);
								
	
#if DBG_EN				
								printf("xfac=%f \r\n",gTpInfo.xfac);
								printf("xoff=%f \r\n",gTpInfo.xoff);
								printf("yfac=%f \r\n",gTpInfo.yfac);
								printf("yoff=%f \r\n",gTpInfo.yoff);
#endif			   
								
								
								//保存4个参数
								LT_TpSaveAdjdata();
								//LT_TpGetAdjdata();
								return 1;
			}
			gTpInfo.sta = 3;
		}
		Delay_ms(10);
		outTime++;
		if(outTime>4000)
		{
			//获取之前保存的4个参数
			LT_TpGetAdjdata();
			LT768_DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,White);
			LT768_Print_Internal_Font_String(200,180,Red,White,"Touch Screen Adjust Failure!");				//校正失败
			return 1;
		}
	}
}


u8 LT_TpInit(void)
{
	I2C_ConfigGpio(I2C_SCL,GPIO_OUTPUT);//CTP-SCL             clk
	I2C_ConfigGpio(I2C_SDA,GPIO_OUTPUT);//CTP-SDA             TDIN
	EPORT_ConfigGpio(EPORT_PIN3,GPIO_INPUT);//CTP-SDO   	   DOUT
	EPORT_ConfigGpio(EPORT_PIN5,GPIO_OUTPUT);//CTP-RST         CS
	EPORT_ConfigGpio(EPORT_PIN4,GPIO_INPUT);//CTP-INT1     int
		
	
	Delay_ms(10);

//	while(1)
//	{
//		if(EPORT_ReadGpioData(EPORT_PIN4)==0)
//		{
//				printf("x=%d     y=%d\r\n",TP_Read_XOY(CMD_RDX),TP_Read_XOY(CMD_RDY));
//				EPORT_WriteGpioData(EPORT_PIN15,0);
//		}
//	}

	if(LT_TpGetAdjdata()!=1)
	{
		Display_ON(); 
		Delay_ms(100);
	    LT768_PWM1_Init(1,0,5,800,450);	
		Rtp_Cor=1;
		LT_TpAdjust();
	}
	Rtp_Cor=0;
	TP_Read_XY2(&gTpInfo.px,&gTpInfo.py);
		
	gTpInfo.scan = LT_TpScan;
	return 1;
}













/***************************************************************************************
****************************************************************************************
* FILE		:I2C.c
* Description	:NS2009 I2C programming
* Date                         : 08.07.2009
* Copyright (c) 2009 Nsiway Corp. All Rights Reserved.
* History:
 ****************************************************************************************
****************************************************************************************/
//移植：只要重新定义SCK,SDA引脚，把原来初始化触摸屏，读写触摸屏函数替换
//typedef unsigned char	U8;
//typedef unsigned short int   	U16;
//typedef unsigned int 	U32;

//typedef char			S8;
//typedef int 			S16;
//typedef long 			S32;

#if  _NFequalSeven_
#define NumberFilter	7
#else
#define NumberFilter	5
#endif

//extern void GPIO_InitIO(S8 direction, S8 port);
//extern void GPIO_WriteIO(S8 data, S8 port);
//extern S8   GPIO_ReadIO(S8 port);


//extern const S8 gpio_NS2009_sck_pin;
//extern const S8 gpio_NS2009_sda_pin;

//#define ACC_NS2009_SCK 	gpio_NS2009_sck_pin
//#define ACC_NS2009_SDA 	gpio_NS2009_sda_pin

#define SET_NS2009_I2C_CLK_OUTPUT	I2C_ConfigGpio(I2C_SCL,GPIO_OUTPUT);//CTP-SCL             clk
#define SET_NS2009_I2C_DATA_OUTPUT	I2C_ConfigGpio(I2C_SDA,GPIO_OUTPUT);//CTP-SDA             TDIN
#define SET_NS2009_I2C_DATA_INPUT	I2C_ConfigGpio(I2C_SDA,GPIO_INPUT);//CTP-SDA             TDIN
#define SET_NS2009_I2C_CLK_HIGH		I2C_WriteGpioData(I2C_SCL,1)
#define SET_NS2009_I2C_CLK_LOW		I2C_WriteGpioData(I2C_SCL,0)
#define SET_NS2009_I2C_DATA_HIGH	I2C_WriteGpioData(I2C_SDA,1)	
#define SET_NS2009_I2C_DATA_LOW		I2C_WriteGpioData(I2C_SDA,0)	
#define GET_NS2009_I2C_DATA_BIT		I2C_ReadGpioData(I2C_SDA)

#define NS2009xL_AddW 0x90          //NS2009 Address for Writing"1001 00 0 0"
#define NS2009xL_AddR 0x91          //NS2009 Address for Reading"1001 00 0 1"

//如果X Y接反了就需要交换寄存器的地址
#define ReadX		0xD0     
#define ReadY		0xC0     
#define ReadZ1		0xF0     
#define ReadZ2		0xE0  


//#define ReadX		    0xC0     
//#define ReadY		    0xD0     
//#define ReadZ1		0xE0     
//#define ReadZ2		0xF0  



#define Rx_Plate	    700	
#define R_Threshold 	10000

void NS2009_IIC_Start(void) 
{                        //I2C Start signal generation: Data pin falls down when clock is high
	//I2C_ConfigGpio(I2C_SCL,GPIO_OUTPUT);//CTP-SCL             clk//GPIO_ModeSetup(ACC_NS2009_SCK, 0x00);   //Set I2C CLK pin as GPIO
	SET_NS2009_I2C_CLK_OUTPUT;                     //Set I2C CLK pin as output
	//GPIO_ModeSetup(ACC_NS2009_SDA, 0x00);   //Set I2C DATA pin as GPIO
	SET_NS2009_I2C_DATA_OUTPUT;                //Set I2C DATA pin as output

	SET_NS2009_I2C_DATA_HIGH;	              //I2C DATA pin output high(1)
	SET_NS2009_I2C_CLK_HIGH;		      //I2C CLK pin output high(1)
	Delay_us(20);                                       //delay_us 20uS
	SET_NS2009_I2C_DATA_LOW;                //I2C DATA pin output low(0)
	Delay_us(10);                                      //delay_us 10uS
	SET_NS2009_I2C_CLK_LOW;                 //I2C CLK pin output low(0)
	Delay_us(10);                                    //delay_us 10uS
}


void NS2009_IIC_Stop (void) {                //I2C Stop signal generation: Data pin rises up when clock in is high
	Delay_us(10);                                       //delay_us 10uS
	SET_NS2009_I2C_CLK_HIGH;		    //I2C CLK pin output high(1)
	Delay_us(10);                                    //delay_us 10uS
	SET_NS2009_I2C_DATA_HIGH;	        //I2C DATA pin output high(1)
}

void NS2009_IIC_RepeatedStart(void) {        //I2C Repeat Start signal generation: Data pin falls down when clock is high
	Delay_us(20);                                                //delay_us 20uS
	Delay_us(20);                                                //delay_us 20uS
	SET_NS2009_I2C_DATA_HIGH;	                   //I2C DATA pin output high(1)
	Delay_us(10);                                             //delay_us 10uS
	SET_NS2009_I2C_CLK_HIGH;	                  //I2C CLK pin output high(1)
	Delay_us(20);                                          //delay_us 20uS
	Delay_us(20);                                       //delay_us 20uS
	SET_NS2009_I2C_DATA_LOW;              //I2C DATA pin output low(0)
	Delay_us(10);                                    //delay_us 10uS
	SET_NS2009_I2C_CLK_LOW;               //I2C CLK pin output low(0)
	Delay_us(10);                                 //delay_us 10uS	
}


void NS2009_IIC_OneClk(void) {               //I2C CLK pin output one clock: CLK pin rises up before falls down
	Delay_us(5);
	SET_NS2009_I2C_CLK_HIGH;		       //I2C CLK pin output high(1)		
	Delay_us(10);                                       //delay_us 10uS
	SET_NS2009_I2C_CLK_LOW;                //I2C CLK pin output low(0)
	Delay_us(5);	
}


void NS2009_IIC_SendByte(U8 sData) {         //I2C send one byte out
	S8 i;
	for (i=8; i>0; i--) 
{                                //Loop 8 times to send 8 bits

		if ((sData>>(i-1))&0x01) {                        //Judge output 1 or 0
			SET_NS2009_I2C_DATA_HIGH;  //I2C DATA pin output high(1) if output 1
		} else { 
			SET_NS2009_I2C_DATA_LOW;   //I2C DATA pin output low(0) if output 0
			}

		NS2009_IIC_OneClk();               //Output one clock pulse after data pin is ready
			
	}

}


U8  NS2009_IIC_ChkAck(void) {                  //Check I2C Acknowledgement signal
	SET_NS2009_I2C_DATA_INPUT;                     //Set I2C DATA pin as input
	Delay_us(5);                                           //delay_us 5uS
	SET_NS2009_I2C_CLK_HIGH;		    //I2C CLK pin output high(1)		
	Delay_us(5);                                      //delay_us 5uS again
	if (GET_NS2009_I2C_DATA_BIT) {             //Read I2C DATA pin
		Delay_us(5);                                   //delay_us 5uS
		SET_NS2009_I2C_CLK_LOW;            //I2C CLK pin output low(0)
		Delay_us(5);                                 //delay_us 5us again
		SET_NS2009_I2C_DATA_OUTPUT;        //Set I2C DATA pin as output
		SET_NS2009_I2C_DATA_LOW;           //I2C DATA pin output low(0)
		return 1;                                         //Return 1 if read 1 from I2C DATA pin
		} else {                                            //If I2C DATA pin is invalid for acknowledgement signal
		Delay_us(5);                                  //delay_us 5uS
		SET_NS2009_I2C_CLK_LOW;            //I2C CLK pin output low(0)
		Delay_us(5);                                   //delay_us 5uS again
		SET_NS2009_I2C_DATA_OUTPUT;        //Set I2C DATA pin as output
		SET_NS2009_I2C_DATA_LOW;           //I2C DATA pin output low(0)
		return 0;                                           //Return 0 if read 0 from I2C DATA pin
		}			
}

U8 NS2009_IIC_ReadByteACK(void) {                   //Read one byte and send an acknowledgement signal
	S8 i;
	U8 rdata;

	SET_NS2009_I2C_DATA_INPUT;                                  //Set I2C DATA pin as input
	rdata = 0;                                                    //Prepare to receive data
	for (i=8; i>0; i--) {                                       //Loop 8 times to receive 8 bits
		if (GET_NS2009_I2C_DATA_BIT) rdata |= (0x01<<(i-1));    //If read a 1, set to data bit
		NS2009_IIC_OneClk();
		}			                             //Output one clock pulse after a bit is read

	SET_NS2009_I2C_DATA_OUTPUT;                                 //Set I2C DATA pin as output
	SET_NS2009_I2C_DATA_LOW;                                    //I2C DATA pin output low(0): the acknowledgement signal
	NS2009_IIC_OneClk();                                        //Output one clock pulse after data pin is ready

	return rdata;                                                //Return eceived data
}


U8 NS2009_IIC_ReadByteNCK(void) {                              //Read one byte but do not send acknowledgement signal
	S8 i;
	U8 rdata;
 
	SET_NS2009_I2C_DATA_INPUT;                                            //Set I2C DATA pin as input
	rdata = 0;                                                                         //Prepare to receive data
	for (i=8; i>0; i--) {                                                               //Loop 8 times to receive 8 bits
		if (GET_NS2009_I2C_DATA_BIT) rdata |= (0x01<<(i-1));    //If read a 1, set to data bit
		NS2009_IIC_OneClk();
		}			                                                               //Output one clock pulse after a bit is read

	SET_NS2009_I2C_DATA_OUTPUT;                                //Set I2C DATA pin as output
	SET_NS2009_I2C_DATA_HIGH;	                            //I2C DATA pin output high(1): no acknowledge
	NS2009_IIC_OneClk();                                       //Output one clock pulse after data pin is ready
	SET_NS2009_I2C_DATA_LOW;                                   //I2C DATA pin output low(0)

	return rdata;                                                //Return received data
}



// NS2009 Master Write
void NS2009_IICWrite(U8 Command) {              //Write one command to NS2009 via I2C
	//Start
	NS2009_IIC_Start();                           //Output a START signal

	// Device hardware address
	NS2009_IIC_SendByte(NS2009xL_AddW);          //Send one byte of NS2009 address for writing
	if (NS2009_IIC_ChkAck()) {                    //Check acknowledge signal
		NS2009_IIC_Stop();	               //Output a STOP signal
		return;                                //If acknowledgement signal is read as 1, then return to end
		}
                                                       //If acknowledgement signal is read as 0, then go to next step
	// Register address to read                         
	NS2009_IIC_SendByte(Command);                 //Send one byte of command in NS2009
	if (NS2009_IIC_ChkAck()) {                    //Check acknowledgement signal
		NS2009_IIC_Stop();                    //Output a STOP signal
		return;    	                       //If acknowledgement signal is read as 1, then return to end
		}
                                                       //If acknowledgement signal is read as 0, then go to next step
	// Stop	
	NS2009_IIC_Stop();	                       //Output a STOP signal	

}


// Master Read
S8 NS2009_IICRead(U8 Command,U8 *readdata,S8 nread){                //Read bytes from NS2009 via I2C

	S8 i;

	if(nread < 2)
		return 1;
		
	//Start
	NS2009_IIC_Start();                                                     //Output a START signal

	// Device hardware address
	NS2009_IIC_SendByte(NS2009xL_AddW);                                   //Send one byte NS2009 address for writing
	if (NS2009_IIC_ChkAck()) {                                              //Check acknowledge signal
		NS2009_IIC_Stop();	                                         //Output a STOP signal
		return 1;                                                        //If acknowledgement signal is read as 1, then return to end
		}
                                                                                 //If acknowledgement signal is read as 0, then go to next step
	// send command                         
	NS2009_IIC_SendByte(Command);                                           //Send one byte of Command
	if (NS2009_IIC_ChkAck()) {                                              //Check acknowledgement signal
		NS2009_IIC_Stop();                                              //Output a STOP signal
		return 1;    	                                                 //If acknowledgement signal is read as 1, then return to end
		}
                                                                                 //If acknowledgement signal is read as 0, then go to next step
	// Stop	
	NS2009_IIC_Stop();	                                                 //Output a STOP signal
		
	//Start
	NS2009_IIC_Start();                                                     //Output a START signal
                                                      
	// Device hardware address
	NS2009_IIC_SendByte(NS2009xL_AddR);                                    //Send a byte of NS2009 address for reading
	if (NS2009_IIC_ChkAck()) {                                              //Check acknowledge signal 
		NS2009_IIC_Stop();	                                         //Output a STOP signal	
		return 1;                                                        //If acknowledgement signal is read as 1, then return to end
	}
  
 	                                                                         //If acknowledgement signal is read as 0, then go to next step													
	for(i=0;i<(nread-1);i++){
		readdata[i] = NS2009_IIC_ReadByteACK();
	
	}
	
	readdata[nread-1] = NS2009_IIC_ReadByteNCK();                

                          
	// Stop	
	NS2009_IIC_Stop();	                                                //Output a STOP signal

	return 0;                                                               //Return received data
}                                                                               //Return received data
	

//Noise Filter
//Drop the max and min value,average the other 3 values.

U16 Noise_Filter(U16* datatemp){
	S8 i;
	U16 swaptemp;

#if !defined(_NFequalSeven_)	
	for(i=0;i<4;i++){
		if(datatemp[i]>datatemp[i+1]){
			swaptemp = datatemp[i];
			datatemp[i] = datatemp[i+1];
			datatemp[i+1] = swaptemp;
		}
	}
	
	for(i=0;i<3;i++){	
		if(datatemp[i]<datatemp[i+1]){
			swaptemp = datatemp[i];
			datatemp[i] = datatemp[i+1];
			datatemp[i+1] = swaptemp;
		}
	}
#else
	for(i=0;i<6;i++){
		if(datatemp[i]>datatemp[i+1]){
			swaptemp = datatemp[i];
			datatemp[i] = datatemp[i+1];
			datatemp[i+1] = swaptemp;
		}
	}
	for(i=0;i<5;i++){
		if(datatemp[i]>datatemp[i+1]){
			swaptemp = datatemp[i];
			datatemp[i] = datatemp[i+1];
			datatemp[i+1] = swaptemp;
		}
	}
	for(i=0;i<4;i++){	
		if(datatemp[i]<datatemp[i+1]){
			swaptemp = datatemp[i];
			datatemp[i] = datatemp[i+1];
			datatemp[i+1] = swaptemp;
		}
	}
	
	for(i=0;i<3;i++){	
		if(datatemp[i]<datatemp[i+1]){
			swaptemp = datatemp[i];
			datatemp[i] = datatemp[i+1];
			datatemp[i+1] = swaptemp;
		}
	}
#endif
	
	swaptemp = 0;
	for(i=0;i<3;i++){	
		swaptemp = swaptemp + datatemp[i];
	}
	
	swaptemp = swaptemp/3;
	
	return swaptemp;
}


S8 Init_NS2009(void)
{
    U16 buff_coord[2];
	U8   i,icdata[2];
	
	NS2009_IICRead(ReadX,icdata,2);  //初始化NS2009 ；主要是使能中断
	gTpInfo.scan=LT_TpScan1;

	if(LT_TpGetAdjdata()!=1)
{
	Display_ON(); 
	Delay_ms(100);
	LT768_PWM1_Init(1,0,5,800,450);	
	LT_TpAdjust();
}
read_data(buff_coord);
}


S8 read_data(U16 *coord)
	{
	U16     x,y,z1,z2;
	U8      i,buf[4],icdata[4];
	U16    Tempx[NumberFilter],Tempy[NumberFilter];
	float rt,tempf = 4096;
	
	for(i=0;i<NumberFilter;i++){
		NS2009_IICRead(ReadX,icdata,2);
		Tempx[i] =(icdata[0]<<4 | icdata[1]>>4); 
		NS2009_IICRead(ReadY,icdata,2);
		Tempy[i] =(icdata[0]<<4 | icdata[1]>>4);
		 //printf("Tempx=%d    Tempy=%d\r\n",Tempx[i],Tempy[i]);
	}
	
	x = Noise_Filter(Tempx);
	y = Noise_Filter(Tempy);

#if DBG_EN
	 printf("Noise_Filter=x0=%d    Noise_Filter=y0=%d\r\n",y,x);
#endif
		
	NS2009_IICRead(ReadZ1,buf,2);
	z1 = buf[0]<<4 | buf[1]>>4;
		
	NS2009_IICRead(ReadZ2,buf,2);
	z2 = buf[0]<<4 | buf[1]>>4;

	rt = (((float)Rx_Plate*(float)x)/tempf)*((float)z2/(float)z1 - 1);
	
	if(x < 100 || rt > R_Threshold)              
	    return 0;
	   else
	   {
		coord[1] = abs(4096-x);
		coord[0] = abs(4096-y);
		gTpInfo.px=abs(4096-x);
		gTpInfo.py=abs(4096-y);   
		//   printf("gTpInfo.px =%d gTpInfo.py=%d \r\n",gTpInfo.px,gTpInfo.py);
		return 1;
	   }
 }


void LT_TpScan1(void)
{
	U16 buff_coord[2];
   if(EPORT_ReadGpioData(EPORT_PIN4)==0)
   {

	   if(read_data(buff_coord)==1)
	   {
		   
		gTpInfo.x[0] = gTpInfo.xfac * buff_coord[1] + gTpInfo.xoff;
        gTpInfo.y[0] = gTpInfo.yfac * buff_coord[0] + gTpInfo.yoff;	
			
		if(gTpInfo.x[0]>LCD_XSIZE_TFT)	gTpInfo.x[0] = LCD_XSIZE_TFT;
		if(gTpInfo.y[0]>LCD_YSIZE_TFT)	gTpInfo.y[0] = LCD_YSIZE_TFT;
		if(First_press==0)
		{
			First_press=1;
			First_pressX= gTpInfo.x[0];
			First_pressY= gTpInfo.y[0];			
		}
		gTpInfo.sta=1;

#if 1	

	    printf("buff_coord[0]=%d    buff_coord[1]=%d\r\n",buff_coord[0],buff_coord[1]);
	    printf("x=%d    y=%d\r\n",gTpInfo.x[0],gTpInfo.y[0]);
#endif
	}
	
   }
   else
   {

	 First_press=0;
	 gTpInfo.sta=0;  
   }
}

