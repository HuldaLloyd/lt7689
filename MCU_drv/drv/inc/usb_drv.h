// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : wdt_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __USB_DRV_H__
#define __USB_DRV_H__

#include "type.h"
#include "usb_reg.h"

//#define USB_CCID
//#define USB_MS
//#define USB_HID 0
//#define USB_CDC 



#define USB_MODE_1D1		0
#define USB_MODE_2D0		1   //USB2.0芯片需要外接12M晶振

/*USB variables*/
/* Global USB Register */
extern USBCFIFO_Reg* gUSBC_fifoReg;
extern USBCCommonReg* gUSBC_ComReg;
extern USBCIndexedReg* gUSBC_IdxReg;

//usb the max packet size
extern unsigned short gTxMaxPacket;
extern unsigned short gRxMaxPacket;

//USB version(1--USB2.0;0--USB1.1)
extern unsigned char g_usbVer;

//Suspend Mode Enable Flag(0-disable;1-enable)
extern unsigned char g_suspendMode;

//Save USB Address
extern volatile unsigned char g_USBNewAddr;

//indicate which command is processed currently
extern volatile unsigned char g_bulkPhase;


extern UINT8 g_dev_std_req[];
extern UINT8 g_uchUSBStatus;

//global message queue for USB loop tasks.
extern volatile UINT8 g_msgflags;
extern UINT8 g_msgflags1;


//CCID using
extern const UINT8 SmartCard_Reader[];

extern const UINT8 DATA_RATE[];

extern const UINT8 USB_CCID_Device_Descriptor[];

extern const UINT8 USB_CCID_Device_Qualifier_Descriptor[];

extern const UINT8 USB_CCID_Configuration_Descriptor [];

extern const UINT8 USB_CCID_StrDescProduct[];

//CDC using
extern const UINT8 USB_cdc_Device_Descriptor[];
//Device Qualifier Descriptor
extern const unsigned char USB_cdc_Device_Qualifier_Descriptor[];

extern const UINT8 USB_cdc_Configuration_Descriptor[] ;
extern const unsigned char USB_cdc_String_Descriptor[];

//USB Language ID
extern const UINT8 UsbLanguageID[] ;

//Manufacturer String
extern const UINT8 UsbStrDescManufacturer[];

//Product String
extern const UINT8 UsbStrDescProduct[];

//Serial Number
extern const UINT8 UsbStrDescSerialNumber[] ;

//standard Device Descriptor
extern const UINT8 USB_Device_Descriptor[18];
//Device Qualifier Descriptor
extern const UINT8 USB_Device_Qualifier_Descriptor[10];
//configuration descriptor
extern const UINT8 USB_Configuration_Descriptor[];
//standard string Descriptor,serial number
extern const UINT8 USB_String_Descriptor[];
//MBR const data
extern const UINT8 MBR[];
//DBR const data
extern const UINT8 DBR[];


//Save USB Address
extern	volatile UINT8 g_USBAddrFlag;

//indicate which sub command is processed currently
extern UINT8 g_subCmd;

//CBW packet
extern UINT32 g_cbwPacket_l[];
extern UINT8* g_cbwPacket;
	
//CSW packet
extern UINT8 g_cswPacket[];

//USB data buffer1
extern UINT32 g_databuf;
extern UINT32 g_userdatabuf;

//indicate the start sector in reading device
extern UINT32 g_wrLBA;

//indicate the length in Once tranfer from PC to device
extern UINT32 g_transLens;

//indicate the transfer stage of USB1.1
extern UINT8 g_TxRxCounter;

//indicate the mode of reading device
extern UINT8 g_readMode; 
extern UINT32 RWDataAddr;
extern UINT32 RWDataLength;

//ChipTest Status
extern UINT16 g_Chiptest_Status;	//1-Selftest is running;0-Selftest finished
extern UINT16 g_Chiptest_Result;	//0-Selftest pass;Other-Selftest fail
extern UINT16 g_Chiptest_FailtoDo;	//0-After Selftest fail,system halt,Other-After fail,system go-on
extern UINT16 g_Chiptest_Enable;	//0-ChipTest Enable,Other-ChipTest Disable
//ChipTest Parameters
//ChipTest Parameters
extern UINT16 g_trng_mode;
extern UINT16 g_sm1_protect;
extern UINT16 g_sm4_protect;
extern UINT16 g_rsa_protect;

//Bootloader Enable Flag
extern UINT16 g_BL_Enable;

//Bootloader Password Success Flag
extern UINT16 g_PWD_Result;	

//Bootloader Pwd fail Counter
extern UINT16 g_BL_FailCounter;

//Bootloader Pwd Max fail time
extern UINT16 g_BL_MaxFail;

extern const UINT8 USB_hid_Device_Descriptor[18];

//Device Qualifier Descriptor
extern const UINT8 USB_hid_Device_Qualifier_Descriptor[10];

//configuration descriptor
extern const UINT8 USB_hid_Configuration_Descriptor[];

//standard string Descriptor,serial number
extern const UINT8 USB_hid_String_Descriptor[];
extern const UINT8 USB_hid_report_Descriptor2[];

extern unsigned char usb_tx_int_flag ;

/*USB function*/
extern void USBDev_Polling(void);
extern void USB_Init(void);
extern void read_ep_buf(UINT8 *out_buf,UINT32 ep,UINT16 uiLens);
extern void write_ep_buf(UINT32 ep,UINT8 *out_buf,UINT16 uiLens);
extern void USBDev_EP0_SendStall(void);
extern void write_ep0_buf(UINT8 *in_buf,UINT16 uiLen);
extern void USBDev_TestMode(void);
extern void USBDev_Inquiry(void);
extern void USBDev_ModeSense(void);
extern void USBDev_RequestSense(void);
extern void USBDev_ReadCapacity(void);
extern void USBDev_Suspend(void);
extern void USBDev_BusReset(void);
extern void USBDev_Ep0Handler(void);
extern void ClearRx(void);
extern void SetTx(void);
extern void SetFIFO_Addr(UINT32 bufferAddr,UINT8 mode);
extern void USBDev_Packet_BulkIn(UINT8 *pDataBuf,UINT16 uiLength,UINT8 flag);
extern void USB_int_Service(void);
extern void USB_CDC_Init(void);
extern void usb_cdc_send(UINT8 usb_ep, UINT8 *buf, UINT16 len);
extern void usb_cdc_send_null(UINT8 usb_ep);
extern UINT16 usb_cdc_receive(UINT8 usb_ep, UINT8 *buf);
extern void USB_HID_Init(void);
extern UINT16 USB_HID_EP1_RX_ISR(UINT8* pData);
extern void USB_HID_EP1_TX_ISR(UINT8* pData,UINT16 len);
extern void USB_HID_EP_TX_ISR(unsigned char epID,unsigned char* pData,unsigned short len);
extern void USB_HID_BusReset(void);
extern void USB_HID_EP0_ISR(void);
extern UINT16 usb_receive(UINT8 *buf);
extern void usb_send(UINT8 *buf,UINT16 len);
extern void USB_HID_Ep0Handler(void);
extern void USBDev_EP1_TX_ISR(void);
extern void USBDev_EP0_ISR(void);
extern void USBDev_EP1_RX_ISR(void);
extern void UsbSetTXsize(UINT16 size,UINT8 mode);
extern void USBDev_Data_BulkOut(void);
extern void USBDev_Data_BulkIn(void);
extern void USB_MS_ISR(void);
extern void USB_HID_ISR(void);
extern void USB_CDC_ISR(void);
extern void USBC_IRQHandler(void);
extern void USB_HID_Polling(void);
extern void HID_SetFIFO_Addr(UINT32 bufferAddr,UINT8 mode);

extern void USB_CCID_Init(void);
extern UINT16 USB_CCID_receive(UINT8 usb_ep, UINT8 *buf);
extern void USB_CCID_send(UINT8 usb_ep, UINT8 *buf, UINT16 len);
extern void USB_CCID_ISR(void);

#endif /* __USB_DRV_H__ */
