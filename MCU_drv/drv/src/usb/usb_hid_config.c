// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : usb_hid_config.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "usb_reg.h"
#include "usb_const.h"
#include "usb_drv.h"

#define USB_VID  0xEEEF
#define USB_PID  0x2929

//standard Device Descriptor
const unsigned char USB_hid_Device_Descriptor[18] =
{
		0x12,                       //bLength: 0x12
		0x01,                       //bDescriptorType:
#if USB_MODE_1D1
		0x10, 0x01,             //bcdUSB: version 1.1
#elif USB_MODE_2D0
		0x00, 0x02,             //bcdUSB: version 2.0
#else
		0x10, 0x01,             //bcdUSB: version 1.1
#endif
		0x00,                       //bDeviceClass:
		0x00,                       //bDeviceSubClass:
		0x00,                       //bDeviceProtocol:
		0x40,                       //bMaxPacketSize0: 64
		0x48, 0x55,                 //idVendor:
		0x66, 0x66,                 //idProduct:
		0x00, 0x03,                 //bcdDevice:
		0x01,                       //iManufacturer:
		0x02,                       //iProduct:
		0x00,                       //iSerialNumber:
		0x01,                       //bNumConfigurations:
};

//Device Qualifier Descriptor
const unsigned char USB_hid_Device_Qualifier_Descriptor[10] =
{

	//Device Qualifier(0x0A bytes)
	0x0A,                   //bLength: 0x12 bytes
	0x06,                   //bDescriptorType: DEVICE
#if USB_MODE_1D1
	0x10, 0x01,             //bcdUSB: version 1.1
#elif USB_MODE_2D0
	0x00, 0x02,             //bcdUSB: version 2.0
#else
	0x10, 0x01,             //bcdUSB: version 1.1
#endif
	0x00,                   //bDeviceClass: independent interfaces
	0x00,                   //bDeviceSubClass: 0
	0x00,                   //bDeviceProtocol: class specific protocols NOT used on device basis
	0x40,                   //bMaxPacketSize0 for other speed
	0x01,                   //bNumConfigurations for other-speed configuration
	0x00,                   //Reserved,must be 0        
};

//configuration descriptor
const unsigned char USB_hid_Configuration_Descriptor [] =
{
	0x09,                         //bLength
	0x02,                         //bDescriptor
	0x29, 0x00,                   //wTotalLength
	0x01,                         //bNumInterface
	0x01,                         //bConfiguration Value
	0x00,                         //iConfiguration
	0xa0,                         //bmAttributes
	0x32,                         //bMaxPower   100mA
	//USB_hid
	0x09,                         //bLength:
	0x04,                         //bDescriptorType
	0x00,                         //bInterfaceNumber
	0x00,                         //bAlternateSetting
	0x02,                         //bNumEndpoints
	0x03,                         //bInterfaceClass:
	0x00,                         //bInterfaceSubClass
	0x00,                         //bInterfaceProtocol
	0x00,                         //iInterface
	//HID
	0x09,                         //bLength:
	0x21,                         //bDescriptorType:    HID
#if USB_MODE_1D1
	0x10, 0x01,                        //BCDHID:             HID1.1
#elif USB_MODE_2D0
	0x00, 0x02,                        //BCDHID:             HID2.0
#else
	0x10, 0x01,                        //BCDHID:             HID1.1
#endif
	0x00,                         //bCountryCode:
	0x01,                         //bNumDescriptor:
	0x22,                         //bDescriptorType:
#if USB_MODE_1D1
	0x34,                         //WDescriptorLength:
#elif USB_MODE_2D0
	0x36,                         //WDescriptorLength:
#else
	0x34,                         //WDescriptorLength:
#endif
	
	0x00,
																 //USB_hid
	0x07,                          //bLength:
	0x05,                          //bDescriptorType:
	0x80+INDEX_EP2,       //EndpointAddress
	0x03,                          //bmAttributes
#if USB_MODE_1D1
	0x40, 0x00,             //wMaxPacketSize: 64 bytes,for USB1.1
#elif USB_MODE_2D0
	0x00, 0x02,             //wMaxPacketSize: 64 bytes,for USB2.0
#else
	0x40, 0x00,             //wMaxPacketSize: 64 bytes,for USB1.1
#endif
	0x01,                          //bInterval

	0x07,                          //bLength:
	0x05,                          //bDescriptorType:
	INDEX_EP1,            //EndpointAddress
	0x03,                          //bmAttributes
#if USB_MODE_1D1
	0x40, 0x00,             //wMaxPacketSize: 64 bytes,for USB1.1
#elif USB_MODE_2D0
	0x00, 0x02,             //wMaxPacketSize: 64 bytes,for USB2.0
#else
	0x40, 0x00,             //wMaxPacketSize: 64 bytes,for USB1.1
#endif
	0x01,                          //bInterval
};

//standard string Descriptor,serial number
const unsigned char USB_hid_String_Descriptor[] =
{

				//Device (0x1a bytes)
		0x1a,                   //bLength: 0x1A bytes
		0x03,                   //bDescriptorType: STRING
				
		0x33,
		0x00,

		0x35,
		0x00,
		0x35,
		0x00,

		0x34,
		0x00,
		0x39,
		0x00,

		0x39,
		0x00,
		0x34,
		0x00,

		0x34,
		0x00,
		0x31,
		0x00,

		0x34,
		0x00,
		0x39,
		0x00,

		0x34,
		0x00

};

const UINT8 USB_hid_report_Descriptor2[] =
{
	 0x06, 0xA0,0xFF,		//全局，将用途页选择为?
	0x09, 0x01, 			//局部，将用途选择为01
	0xA1, 0x01,				//主条目，COLLECTION开集合，为应用集合
	0x09, 0x02,             //局部，将用途选择为02
	0xA1, 0x00,  			//主条目，COLLECTION开集合，为?
	0x06, 0xA1,0xFF, 		//全局，将用途页选择为?
		 
					 
	0x09, 0x03,			//局部，将用途选择为03
	0x09, 0x04,			//局部，将用途选择为04
	0x15, 0x80, 		//全局，逻辑值最小为80
	0x25, 0x7F,			//全局，逻辑值最大为7F
	0x35, 0x00,         //全局，物理值最小为00
	0x45, 0xFF,			//全局，物理值最大为FF
	0x75, 0x08,			//全局，每个数据域长度为8位
	
#if USB_MODE_1D1
	0x95, 0x40,         //全局，数据域的数量为64个
#elif USB_MODE_2D0
	0x96,0x00,0x02,			//全局，数据域的数量为512个
#else
	0x95, 0x40,         //全局，数据域的数量为64个
#endif

	0x81, 0x02,			//主条目，数据位输入
	0x09, 0x05,         //局部，将用途选择为05
	0x09, 0x06, 
	0x15, 0x80,              
	0x25, 0x7F,
	0x35, 0x00,              
	0x45, 0xFF,
	0x75, 0x08, 
  
#if USB_MODE_1D1
	0x95, 0x40,         //全局，数据域的数量为64个
#elif USB_MODE_2D0
	0x96,0x00,0x02,			//全局，数据域的数量为512个
#else
	0x95, 0x40,         //全局，数据域的数量为64个
#endif

	0x91, 0x02,         //主条目，数据位输出
	0xC0,               //END COLLECTION
	0xC0                //END COLLECTION
};
