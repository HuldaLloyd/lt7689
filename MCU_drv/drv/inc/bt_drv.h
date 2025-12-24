/*
 * bt_drv.h
 *
 *  Created on: 2018年9月4日
 *      Author: sjzhang
 */

#ifndef __BT_DRV_H__
#define __BT_DRV_H__

#include "type.h"

//初始化模式
#define BT_NORMAL_MODE 		0
#define BT_LOWPOWER_MODE 	1

extern unsigned int g_bt_flash_base_addr;		//蓝牙数据存储地址，用户可自行配置
extern volatile unsigned char g_bt_print_info;	//0=不打印调试信息，1=打印调试信息，须在BT初始化前配置，默认关闭
extern volatile unsigned char g_bt_init_flag;	//初始化状态标识，取值BT_INIT_STATE

typedef enum{
	BT_UnInit,
	BT_PatchDownload,
	BT_PatchDownload_Complete,
	BT_WritePin,
	BT_WritePin_Complete,
	BT_SetPiscan,
	BT_SetPiscan_Complete
}BT_INIT_STATE;


/************************** BT CMD API************************************/
void bt_init(unsigned char mode);
void bt_event_handler(unsigned char *buff, int len);
void bt_cmd_clear_patch(void);
void bt_cmd_dl_patchcfg(unsigned char patch_sel);
void bt_cmd_wakeup(void);
void bt_cmd_read_sw_version(void);
void bt_cmd_erase_paired_device(unsigned char device_type);
void bt_cmd_read_bt_status(void);
void bt_cmd_set_iocap(unsigned char val);
void bt_cmd_baud_set(unsigned int val);
//val: 0=disable 1=enable
void bt_cmd_flowctl_set(unsigned char val);
void bt_cmd_bt_reset(void);
void bt_cmd_set_logmask(void);
void bt_cmd_ble_set_sc(unsigned char sc, unsigned char mitm);
void bt_cmd_read_le_addr(void);
void bt_cmd_read_ble_remote_name(void);
void bt_cmd_gatt_send(unsigned char* buff, unsigned int len);
//adv_mode: 0=close, 1=open
void bt_cmd_ble_set_adv_mode(unsigned char adv_mode);
void bt_cmd_ble_adv_enable(unsigned char adv_enable);
void bt_cmd_ble_write_device_name(unsigned char* buff, unsigned int len);
//type: 0=public addr, 1=static random addr
void bt_cmd_write_le_addr(unsigned char type, char* addr);
void bt_cmd_ble_set_adv_inerval(unsigned short adv_interval);
void bt_cmd_ble_write_adv_data(void);
void bt_cmd_ble_write_scan_rsp_data(void);
void bt_cmd_ble_disconnect(void);
void bt_cmd_bleconn_upd(unsigned short interval_min, unsigned short interval_max, unsigned short latency, unsigned short timeout);
void bt_cmd_ble_read_rssi(void);
void bt_cmd_ble_set_dis(unsigned char type, unsigned char* val, unsigned short len);
void bt_cmd_gatttptx_test(unsigned int datalen);
void bt_cmd_write_pin(void);
void bt_cmd_legacy_write_device_name(unsigned char *buff, unsigned int len);
void bt_cmd_legacy_set_radio_mode(unsigned char radio_mode);
void bt_cmd_legacy_set_page_scan_para(unsigned char type, unsigned short interval, unsigned short window, unsigned short timeout);
void bt_cmd_legacy_set_inq_scan_para(unsigned char type, unsigned short interval, unsigned short window);
void bt_cmd_spp_connect(char *addr);
void bt_cmd_legacy_read_rssi(void);
void bt_cmd_send_spp_data(unsigned char* buff, unsigned int len);
void bt_cmd_spptptx_test(unsigned int datalen);
void bt_cmd_legacy_read_remote_device_name(void);
void bt_cmd_spp_disconnect(void);
void bt_cmd_legacy_set_sdprec(unsigned char* data, unsigned int len);
void bt_cmd_legacy_set_did(unsigned short vendor_id, unsigned short vendor_id_src, unsigned short product_id, unsigned short product_ver);
//ertm: 0=disable, 1=enable
void bt_cmd_legacy_set_ertm(unsigned char ertm);


#endif /* __BT_DRV_H__ */
