#ifndef __LEVETOP_h
#define __LEVETOP_h
#include "bsp.h"
#include "dac.h"

extern uint32_t ModBus_CMD_info_addr;
extern uint32_t ModBus_CMD_info_sum;
extern uint32_t addr_index[];
extern uint32_t UI_version;
extern uint8_t encryption_key;
/***********/

void LT_ReadParam(void);
void Address_Index(void);
void Get_ModBus_CMD_info(void);
uint8_t Get_Touch_info(uint16_t id);

uint8_t Get_date_variable_Id_Regular_update(uint16_t id);
uint8_t Get_date_variable_Id(uint16_t id);
uint8_t page_date_update(uint16_t id);

uint8_t Get_date_variable_Id_download(uint16_t id);
uint8_t Get_Touch_info_download(uint16_t id);

void DrawClockPointer(
    uint16_t H_x, uint16_t H_y, uint16_t H_angle, uint16_t H_lenL, uint16_t H_lenS, uint32_t H_color, uint8_t H_r,
    uint16_t M_x, uint16_t M_y, uint16_t M_angle, uint16_t M_lenL, uint16_t M_lenS, uint32_t M_color, uint8_t M_r,
    uint16_t S_x, uint16_t S_y, uint16_t S_angle, uint16_t S_lenL, uint16_t S_lenS, uint32_t S_color, uint8_t S_r,
    uint16_t P_x, uint16_t P_y, uint16_t P_w, uint16_t P_h, uint8_t num);
void lineSDFAABB_BUFF(float ax, float ay, float bx, float by, float r, UINT32 color, UINT16 canvas, uint8_t num);
#endif