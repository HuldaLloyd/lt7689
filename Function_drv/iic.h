/************************************************************************
 * Copyright(c) 2023 Levetop Semiconductor Co.,Led. All rights reserved.
 * @file     iic.h
 * @author   UartTFT Application Team
 * @version  V1.0.0
 * @date     2023-02-24
 * @brief
 *************************************************************************/

#ifndef IIC_H_
#define IIC_H_

#include "sys.h"
#include "delay.h"
#include "i2c_drv.h"
#include "eport_drv.h"
#include "spi_drv.h"
extern uint8_t IIC_Buf[128];
extern uint32_t u32Tmp;
extern uint8_t tmpID, tmpEvent;
extern uint32_t u16Tmp, tmpX, tmpY;
extern uint16_t tpX[10], tpY[10];
extern uint32_t TPmaxX, TPmaxY;
extern uint8_t tpActive;
extern void i2c_sda_output(void);
extern void i2c_sda_input(void);
extern void i2c_set_scl(uint8_t index);
extern void i2c_set_sda(uint8_t index);
extern uint8_t i2c_get_sda(void);
extern void i2c_start(void);
extern void i2c_stop(void);
extern uint8_t i2c_write(uint8_t value);
extern uint8_t i2c_read(uint8_t ack);

#endif /* IIC_H_ */
