
#ifndef _rtp_h
#define _rtp_h

#include "LT768_Lib.h"

#include <math.h>
#include <stdlib.h>
#include "delay.h"
#include "data.h"

u8 LT_TpInit(void);
u8 LT_TpScan(void);
u8 LT_TpAdjust(void);
void LT_TpSaveAdjdata(void);
u8 LT_TpGetAdjdata(void);
u8 TP_Read_XY2(u16 *x,u16 *y) ;



typedef unsigned char	U8;
typedef unsigned short int   	U16;
typedef unsigned int 	U32;

typedef char			S8;
typedef int 			S16;
typedef long 			S32;

void LT_TpScan1(void);
void NS2009_IIC_Start(void) ;
void NS2009_IIC_Stop (void);
void NS2009_IIC_RepeatedStart(void) ;
void NS2009_IIC_OneClk(void);
void NS2009_IIC_SendByte(U8 sData);
U8  NS2009_IIC_ChkAck(void);
U8 NS2009_IIC_ReadByteACK(void);
U8 NS2009_IIC_ReadByteNCK(void) ;
void NS2009_IICWrite(U8 Command);
S8 NS2009_IICRead(U8 Command,U8 *readdata,S8 nread);
U16 Noise_Filter(U16* datatemp);
S8 read_data(U16 *coord);
S8 Init_NS2009(void);
#endif


