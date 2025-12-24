/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     tp_drv.h
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief
 *************************************************************************/

#ifndef TP_DRV_H_
#define TP_DRV_H_

#include "data.h"

extern uint8_t ft5216flag;
extern uint8_t gt911flag;
extern uint8_t	CST3240flag;

extern uint16_t X_Output_Max;
extern uint16_t Y_Output_Max;
/*****Resistance touch screen*****/
/*****Resistance screen pin configuration******/
#define TCLK_RESET I2C_WriteGpioData(I2C_SCL, 0)
#define TCLK_SET I2C_WriteGpioData(I2C_SCL, 1)
#define TDIN_RESET I2C_WriteGpioData(I2C_SDA, 0)
#define TDIN_SET I2C_WriteGpioData(I2C_SDA, 1)
#define TCS_RESET SPI_WriteGpioData(SPI3, SPI_MISO, 0)
#define TCS_SET SPI_WriteGpioData(SPI3, SPI_MISO, 1)
#define GET_SDO_Bit EPORT_ReadGpioData(EPORT_PIN2)
#define GET_RTP_INT_Bit EPORT_ReadGpioData(EPORT_PIN4)
// #define TCS_RESET EPORT_WriteGpioData(EPORT_PIN5, 0)
// #define TCS_SET EPORT_WriteGpioData(EPORT_PIN5, 1)

extern void RTP_io_Init(void);
extern void LT_TpSaveAdjdata(void);
extern uint8_t LT_TpGetAdjdata(void);

/****Capacitive touch screen*****/
/****Capacitor screen pin configuration*****/

#define SDA_High I2C_WriteGpioData(I2C_SDA, 1)
#define SDA_Low I2C_WriteGpioData(I2C_SDA, 0)
#define SCL_High I2C_WriteGpioData(I2C_SCL, 1)
#define SCL_Low I2C_WriteGpioData(I2C_SCL, 0)
#define GetSDABit I2C_ReadGpioData(I2C_SDA)

//#define RST_High SPI_WriteGpioData(SPI3, SPI_MISO, 1)
//#define RST_Low SPI_WriteGpioData(SPI3, SPI_MISO, 0)

#define RST_High SPI_WriteGpioData(SPI3, SPI_SS, 1)
#define RST_Low SPI_WriteGpioData(SPI3, SPI_SS, 0)


#define INT_High EPORT_WriteGpioData(EPORT_PIN4, 1)
#define INT_Low EPORT_WriteGpioData(EPORT_PIN4, 0)

#define GET_INT_Bit EPORT_ReadGpioData(EPORT_PIN4)

extern void CTP_io_Init(void);
extern void I2C_stop(void);
extern uint8_t CT_IIC_Wait_Ack(void);
extern void I2C_start(void);
extern uint8_t I2C_write_byte(const uint8_t s);
extern uint8_t I2C_read_byte(uint8_t ack);

#endif