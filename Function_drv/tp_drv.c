/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     tp_drv.c
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief
 *************************************************************************/

#include "tp_drv.h"
#include "i2c_drv.h"
#include "ctp.h"
#include "eflash_drv.h"
#include "iomacros.h"
#include "cst3xx.h"


uint8_t ft5216flag = 0;
uint8_t gt911flag = 0;
uint8_t	CST3240flag = 0;

uint16_t X_Output_Max;
uint16_t Y_Output_Max;
/*****************Resistance touch screen************************/
void RTP_io_Init(void)
{
	I2C_ConfigGpio(I2C_SCL, GPIO_OUTPUT);		// CTP-SCL
	I2C_ConfigGpio(I2C_SDA, GPIO_OUTPUT);		// CTP-SDA
	EPORT_ConfigGpio(EPORT_PIN2, GPIO_INPUT);	// CTP-SDO
	
	//SPI_ConfigGpio(SPI3, SPI_MISO, GPIO_OUTPUT);	// CTP-RST //PIN12
	SPI_ConfigGpio(SPI3, SPI_SS, GPIO_OUTPUT);	// CTP-RST //PIN27
	EPORT_ConfigGpio(EPORT_PIN4, GPIO_INPUT);	// CTP-INT1
	// EPORT_ConfigGpio(EPORT_PIN5,GPIO_OUTPUT);//For debug	
}

#if 0

#define flh_sAddr 0x0807EE00
// Save calibration parameters
void LT_TpSaveAdjdata(void)
{
	gTpInfo.flag = 0x5a;
	EFLASH_Init(g_sys_clk / 1000);
	EFLASH_SetWritePermission();

	EFLASH_PageErase(flh_sAddr);	// Erase Eflash

	EFLASH_WordProg(flh_sAddr, gTpInfo.flag);
	EFLASH_WordProg(flh_sAddr + 20, gTpxfac.s[0]);
	EFLASH_WordProg(flh_sAddr + 24, gTpxfac.s[1]);
	EFLASH_WordProg(flh_sAddr + 28, gTpxfac.s[2]);
	EFLASH_WordProg(flh_sAddr + 32, gTpxfac.s[3]);

	EFLASH_WordProg(flh_sAddr + 36, gTpxoff.s[0]);
	EFLASH_WordProg(flh_sAddr + 40, gTpxoff.s[1]);
	EFLASH_WordProg(flh_sAddr + 44, gTpxoff.s[2]);
	EFLASH_WordProg(flh_sAddr + 48, gTpxoff.s[3]);

	EFLASH_WordProg(flh_sAddr + 52, gTpyfac.s[0]);
	EFLASH_WordProg(flh_sAddr + 56, gTpyfac.s[1]);
	EFLASH_WordProg(flh_sAddr + 60, gTpyfac.s[2]);
	EFLASH_WordProg(flh_sAddr + 64, gTpyfac.s[3]);

	EFLASH_WordProg(flh_sAddr + 68, gTpyoff.s[0]);
	EFLASH_WordProg(flh_sAddr + 72, gTpyoff.s[1]);
	EFLASH_WordProg(flh_sAddr + 76, gTpyoff.s[2]);
	EFLASH_WordProg(flh_sAddr + 80, gTpyoff.s[3]);
}


// Read calibration parameters
uint8_t LT_TpGetAdjdata(void)
{
	gTpxfac.s[0] = IO_READ32(flh_sAddr + 20);
	gTpxfac.s[1] = IO_READ32(flh_sAddr + 24);
	gTpxfac.s[2] = IO_READ32(flh_sAddr + 28);
	gTpxfac.s[3] = IO_READ32(flh_sAddr + 32);

	gTpxoff.s[0] = IO_READ32(flh_sAddr + 36);
	gTpxoff.s[1] = IO_READ32(flh_sAddr + 40);
	gTpxoff.s[2] = IO_READ32(flh_sAddr + 44);
	gTpxoff.s[3] = IO_READ32(flh_sAddr + 48);

	gTpyfac.s[0] = IO_READ32(flh_sAddr + 52);
	gTpyfac.s[1] = IO_READ32(flh_sAddr + 56);
	gTpyfac.s[2] = IO_READ32(flh_sAddr + 60);
	gTpyfac.s[3] = IO_READ32(flh_sAddr + 64);

	gTpyoff.s[0] = IO_READ32(flh_sAddr + 68);
	gTpyoff.s[1] = IO_READ32(flh_sAddr + 72);
	gTpyoff.s[2] = IO_READ32(flh_sAddr + 76);
	gTpyoff.s[3] = IO_READ32(flh_sAddr + 80);

	if (IO_READ32(flh_sAddr) == 0x5a)
	{
		gTpxfac.s[0] = IO_READ32(flh_sAddr + 20);
		gTpxfac.s[1] = IO_READ32(flh_sAddr + 24);
		gTpxfac.s[2] = IO_READ32(flh_sAddr + 28);
		gTpxfac.s[3] = IO_READ32(flh_sAddr + 32);

		gTpxoff.s[0] = IO_READ32(flh_sAddr + 36);
		gTpxoff.s[1] = IO_READ32(flh_sAddr + 40);
		gTpxoff.s[2] = IO_READ32(flh_sAddr + 44);
		gTpxoff.s[3] = IO_READ32(flh_sAddr + 48);

		gTpyfac.s[0] = IO_READ32(flh_sAddr + 52);
		gTpyfac.s[1] = IO_READ32(flh_sAddr + 56);
		gTpyfac.s[2] = IO_READ32(flh_sAddr + 60);
		gTpyfac.s[3] = IO_READ32(flh_sAddr + 64);

		gTpyoff.s[0] = IO_READ32(flh_sAddr + 68);
		gTpyoff.s[1] = IO_READ32(flh_sAddr + 72);
		gTpyoff.s[2] = IO_READ32(flh_sAddr + 76);
		gTpyoff.s[3] = IO_READ32(flh_sAddr + 80);

		gTpInfo.xfac = gTpxfac.xfac;
		gTpInfo.xoff = gTpxoff.xoff;
		gTpInfo.yfac = gTpyfac.yfac;
		gTpInfo.yoff = gTpyoff.yoff;

		//For debug
		// printf("%f",gTpInfo.xfac);
		// printf("%f",gTpInfo.xoff);
		// printf("%f",gTpInfo.yfac);
		// printf("%f",gTpInfo.yoff);

		return 1;
	}
	else
		return 0;
}
#endif
/******Capacitive touch screen*****/
/****Initialization function of IO port related to capacitive touch screen*******/
unsigned char Config_BUFF[186] =
	{
		0x4B, 0x00, 0x04, 0x58, 0x02, 0x01, 0x0D, 0x00, 0x01, 0x08,
		0x28, 0x05, 0x50, 0x32, 0x03, 0x05, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8C, 0x2E, 0x0E,
		0x17, 0x15, 0x31, 0x0D, 0x00, 0x00, 0x02, 0xBC, 0x03, 0x2D,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x0F, 0x41, 0x94, 0xC5, 0x02, 0x07, 0x00, 0x00, 0x04,
		0x9B, 0x11, 0x00, 0x77, 0x17, 0x00, 0x5C, 0x1F, 0x00, 0x48,
		0x2A, 0x00, 0x3B, 0x38, 0x00, 0x3B, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10,
		0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0F,
		0x10, 0x12, 0x13, 0x14, 0x16, 0x18, 0x1C, 0x1D, 0x1E, 0x1F,
		0x20, 0x21, 0x22, 0x24, 0x26, 0x28, 0x29, 0x2A, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0xBB, 0x01};

void CTP_io_Init(void)
{
    u8 temp[5]={0x00,0x00,0x00,0x00,0x00}; 
	uint8_t READ_BUFF[10],CTP_ADDR;
	uint8_t ack, i;
	
	unsigned int Product_ID;

	I2C_ConfigGpio(I2C_SDA, GPIO_OUTPUT);
	I2C_ConfigGpio(I2C_SCL, GPIO_OUTPUT);
	SPI_ConfigGpio(SPI3, SPI_MISO, GPIO_OUTPUT);//PIN12
	EPORT_ConfigGpio(EPORT_PIN4, GPIO_OUTPUT);

	INT_Low;	// INT = 0,For GT911 Addr = 0xBB
	RST_Low;	// RESET = 0;
	DelayMS(20);
	INT_High;	// INT  = 1;
	DelayMS(20);
	RST_High;
	DelayMS(10);
	INT_Low;
	DelayMS(60);
	EPORT_ConfigGpio(EPORT_PIN4, GPIO_INPUT);
	DelayMS(20);
	EPORT_Init(EPORT_PIN4, RISING_EDGE_INT);
	DelayMS(100);

#if 0
	I2C_start();
	I2C_write_byte(CST3240_CMD_WR);
	I2C_write_byte(0xD1);
	I2C_write_byte(0x01);
	I2C_write_byte(0x00);
	I2C_stop();

	I2C_start();
	I2C_write_byte(CST3240_CMD_WR);
	I2C_write_byte(0xD2);
	I2C_write_byte(0x04);
	I2C_stop();

	I2C_start(); 
	I2C_write_byte(CST3240_CMD_RD);

	for (i = 0; i < 4; i++)
	{
		READ_BUFF[i] = I2C_read_byte(0);
	}
	READ_BUFF[i] = I2C_read_byte(1);
	I2C_stop();
#endif

//	CST3XX_WR_Reg(0xD101,temp,4);
//	//CT_Write_Nbyte(CST3240_CMD_WR,0xD101,4,temp);
//	CST3XX_RD_Reg(CST_PID_REG,temp,4);

//	printf("temp[0]=0x%x,,temp[1]=0x%x,temp[2]=0x%x,temp[3]=0x%x\r\n",temp[0],temp[1],temp[2],temp[3]);
//	if((temp[3]==0x32)&&(temp[2]==0x40))//3240   //此处如果读取到ID正确，则返回
//	{
//		cst3240flag = 1;
//		return;
//	}
	
	//CT_Write_Nbyte(0x28,0x8047,186,Config_BUFF);
	for (i = 0; i < 128; i++)
	{
		I2C_start();
		ack = I2C_write_byte(i << 1);
		I2C_stop();
		if (!ack)
		{
//			printf("addr==0x%x, ack==ok  \r\n",i<<1);
			CTP_ADDR = (i << 1);
			if (CTP_ADDR == 0x28)
			{
				
				CT_Write_Nbyte(0x28,0x8047,186,Config_BUFF);
				gt911flag = 1;
				I2C_start();
				I2C_write_byte(0x28);
				I2C_write_byte(0x81);
				I2C_write_byte(0x46);
				I2C_stop();
				I2C_start();
				I2C_write_byte(0x29);
				for (i = 0; i < 4; i++)
				{
					READ_BUFF[i] = I2C_read_byte(0);
				}
				READ_BUFF[i] = I2C_read_byte(1);
				X_Output_Max = (READ_BUFF[1] << 8 | READ_BUFF[0]);
				Y_Output_Max = (READ_BUFF[3] << 8 | READ_BUFF[2]);
				
			}
			else if (CTP_ADDR == 0x70)
				ft5216flag = 1;
            else if (CTP_ADDR == 0xb4)
			{
				CST3240flag = 1;
			}
			break;
		}
		DelayUS(1);
	}
}
/*************Configure SDA pin as output********************/
void SDA_OUTPUT(void)
{
	I2C_ConfigGpio(I2C_SDA, GPIO_OUTPUT);
}
/**************Configure SDA pin as input*****************/
void SDA_INPUT(void)
{
	I2C_ConfigGpio(I2C_SDA, GPIO_INPUT);
}

void CT_DELAY_US(UINT32 val)
{
	for (int i = 0; i < val * 25; i++)
	{
		__NOP;
	}
}

/*************************/////////**************************/

//产生IIC起始信号
void CT_IIC_Start(void)
{
	SDA_OUTPUT();     //sda线输出
	SDA_High;	  	  
	SCL_High;
	CT_DELAY_US(10);
 	SDA_Low;//START:when CLK is high,DATA change form high to low 
	CT_DELAY_US(10);
	SCL_Low;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void CT_IIC_Stop(void)
{ 
	SDA_OUTPUT();//sda线输出
	SCL_Low;
	SDA_Low;
	CT_DELAY_US(10);
	SCL_High;
	CT_DELAY_US(10);
	SDA_High;//STOP:when CLK is high DATA change form low to high 
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 CT_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_INPUT();      //SDA设置为输入  
	SDA_High;CT_DELAY_US(1);	   
	SCL_High;CT_DELAY_US(1);	 
	while(GetSDABit)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			CT_IIC_Stop();
			return 1;
		} 
	}
	SCL_Low;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void CT_IIC_Ack(void)
{
	SCL_Low;
	SDA_OUTPUT();
	SDA_Low;
	CT_DELAY_US(10);
	SCL_High;
	CT_DELAY_US(10);
	SCL_Low;
}
//不产生ACK应答		    
void CT_IIC_NAck(void)
{
	SCL_Low;
	SDA_OUTPUT();
	SDA_High;
	CT_DELAY_US(10);
	SCL_High;
	CT_DELAY_US(10);
	SCL_Low;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void CT_IIC_Send_Byte(u8 txd)
{                        
    u8 t,ch_data=txd;   
	SDA_OUTPUT(); 	    
    SCL_Low;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        if (ch_data & 0x80)
			SDA_High;
		else
			SDA_Low;
		
		CT_DELAY_US(1);
		SCL_High;
		CT_DELAY_US(1);
		SCL_Low;
		ch_data = ch_data << 1;
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 CT_IIC_Read_Byte(unsigned char ack)
{
	u8 i,receive=0;
 	SDA_INPUT();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        SCL_Low; 	    	   
		CT_DELAY_US(30);
		SCL_High;  
		receive<<=1;
		if(GetSDABit)receive++;   
	}	  				 
	if (!ack)CT_IIC_NAck();//发送nACK
	else CT_IIC_Ack(); //发送ACK   
 	return receive;
}

/*************************/////////**************************/


/*****************************I2C startup function***************************/
void I2C_start(void)
{
	SDA_OUTPUT();
	SDA_High;
	SCL_High;
	CT_DELAY_US(1);
	SDA_Low;
	CT_DELAY_US(1);
	SCL_Low;
	CT_DELAY_US(1);
}

/****CTI2C stop function*****/
void I2C_stop(void)
{
	SDA_OUTPUT();
	SCL_Low;
	SDA_Low;

	CT_DELAY_US(1);
	SCL_High;
	CT_DELAY_US(1);
	SDA_High;
	CT_DELAY_US(1);
}

/*****CTI2C writes the 1byte function, S is the content to be written*************/
uint8_t I2C_write_byte(const uint8_t s)
{
	uint8_t j, ch_data, i = 0;

	SDA_OUTPUT();
	ch_data = s;
	SCL_Low;
	for (i = 0; i < 8; i++)
	{
		if (ch_data & 0x80)
			SDA_High;
		else
			SDA_Low;
		CT_DELAY_US(1);
		SCL_High;
		CT_DELAY_US(1);
		SCL_Low;
		ch_data = ch_data << 1;
	}
	
	SDA_INPUT();	// Read ACK
	CT_DELAY_US(1);
	SCL_High;	

	j = 0;
	while (GetSDABit == 1)
	{
		CT_DELAY_US(1);
		j++;
		if (j > 10)
			break;
	}
	if (GetSDABit == 0)
	{
		SCL_Low;
		return 0;
	}
	else
	{
		SCL_Low;
		return 1;
	}
}

/*CTI2C reads the 1byte function, and the return value is the read data*/
uint8_t I2C_read_byte(uint8_t ack)
{
	uint8_t ch_data, iii = 0;

	SDA_INPUT();
	ch_data = 0x00;
	for (iii = 0; iii < 8; iii++)
	{
		ch_data = ch_data << 1;
		SCL_Low;
		SDA_High;
		CT_DELAY_US(1);
		SCL_High;
		CT_DELAY_US(1);
		if (GetSDABit == 1)
			ch_data = ch_data | 0x01;
	}
	SCL_Low;
	SDA_OUTPUT();
	SCL_Low;
	if (!ack)
	{
		SDA_Low;
		CT_DELAY_US(1);
		SCL_High;
		CT_DELAY_US(1);
		SCL_Low;
	}
	else
	{
		SDA_High;
		CT_DELAY_US(1);
		SCL_High;
		CT_DELAY_US(1);
		SCL_Low;
	}

	return ch_data;
}
