// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : usb_hid_config.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "usb_reg.h"
#include "usb_const.h"
#include "usb_drv.h"

#define EP_SIZE  0x40

#define USB_VID  0xEEEF
#define USB_PID  0x2929

// USB commands
#define GET_STATUS	0
#define CLR_FEATURE	1
#define SET_FEATURE	3
#define SET_ADDRESS	5
#define GET_DESC		6
#define SET_DESC		7
#define GET_CONFIG	8
#define SET_CONFIG	9
#define GET_INTF		10
#define SET_INTF		11
#define SYNC_FRAME	12

#define SET_REPORT	0x09
#define SET_IDLE		0x0A

#define mDEVICE				1
#define mCONFIGURATION   		2
#define mSTRING	        	 	3
#define mINTERFACE			4
#define mENDPOINT				5
#define mDEVICE_QUALIFIE		6
#define mOTHER_SPEED_CONFIGURATION  7
#define mINTERFACE_POWER	    	8
#define mREPORT                     0x22

typedef  struct USB_Setup
{
	UINT8 bmRequestType;
	UINT8 bRequest;
	UINT8 wValue_l;
	UINT8 wValue_h;
	UINT8 wIndex_l;
	UINT8 wIndex_h;
	UINT8 wLength_l;
	UINT8 wLength_h;
}usb_Setup;

//*******************************************************************
//       String 0
//*******************************************************************
const unsigned char HID_strDescriptor0[4] =
{
	0x04,		   //bLength;
	0x03,		   //bDescriptorType - STRING descriptor
	0x09,0x04	   //wLANDID0 - English (American)
};

//*******************************************************************
//       String 1
//*******************************************************************
const unsigned char HID_strDescriptor1[] =
{
	0x10,			//bLength; 11 bytes
	0x03,		    //bDescriptorType - STRING descriptor
	'L',0x00,
	'E',0x00,
	'V',0x00,
	'E',0x00,
	'T',0x00,
	'O',0x00,
	'P',0x00
};


//*******************************************************************
//      String 2
//*******************************************************************
const unsigned char HID_strDescriptor2[] =
{
	0x10,			//bLength;
	0x03,		    //bDescriptorType - STRING descriptor
	'U',0x00,
	'S',0x00,
	'B',0x00,
	'-',0x00,
	'H',0x00,
	'I',0x00,
	'D',0x00
};
//*******************************************************************
//      String 23
//*******************************************************************
const unsigned char HID_strDescriptor3[] =
{
	0x12,		//bLength;
	0x03,		//bDescriptorType - STRING descriptor
	'T',0x00,
	'E',0x00,
	'S',0x00,
	'T',0x00,
	'_',0x00,
	'2',0x00,
	'.',0x00,
	'0',0x00
};

const unsigned char HID_String_Descriptor4[] =
{
	0x0,		//bLength;
	0x03,		//bDescriptorType - STRING descriptor
};

const unsigned char* HID_strTable[4]=
{
	HID_strDescriptor0,
	HID_strDescriptor1,
	HID_strDescriptor2,
	HID_strDescriptor3
};

//standard Device Descriptor
const unsigned char USB_hid_Device_Descriptor[18] =
{	
	0x12,		      //blength
	0x01,		      //bDescriptor
#if USB_MODE_1D1
	0x10, 0x01,             //bcdUSB: version 1.1
#elif USB_MODE_2D0
	0x00, 0x02,             //bcdUSB: version 2.0
#else
	0x10, 0x01,             //bcdUSB: version 1.1
#endif
	0x00,		      //bDeviceClass
	0x00,		      //bDeviceSubClass
	0x00,		      //bDeviceProtocol
	EP_SIZE,		      //bMaxPacketSize0  = =32
	USB_VID&0xFF,(USB_VID>>8)&0xFF,	    //idVendor
	USB_PID&0xFF,(USB_PID>>8)&0xFF,	    //idProduct
	0x00,0x01,	    //bcdDevice - Version 1.00
	0x01,         //iManufacturer - Index to string Manufacturer descriptor
	0x02,	        //iProduct  - Index to string product descriptor
	0x03,		      //iSerialNumber - Index to string serial number
	0x01	        //bNumConfigurations - # of config. at current speed,
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

const UINT8 USB_hid_report_Descriptor2[] =
{
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)	
	0x09, 0x00, // USAGE (0)	
	0xa1, 0x01, // COLLECTION (Application)
	0x15, 0x00, //     LOGICAL_MINIMUM (0)
	0x25, 0xff, //     LOGICAL_MAXIMUM (255)
	0x19, 0x00, //     USAGE_MINIMUM (1)
	0x29, 0xff, //     USAGE_MAXIMUM (255)
	
	//0x95, 0x40, //     REPORT_COUNT (32)
#if USB_MODE_1D1
	0x95, 0x40,         //全局，数据域的数量为64个
#elif USB_MODE_2D0
	0x96,0x00,0x02,		//全局，数据域的数量为512个
#else
	0x95, 0x40,         //全局，数据域的数量为64个
#endif
	
	0x75, 0x08, //     REPORT_SIZE (8)
	0x81, 0x02, //     INPUT (Data,Var,Abs)
	0x19, 0x00, //     USAGE_MINIMUM (0)
	0x29, 0xff, //     USAGE_MAXIMUM (255)
	0x91, 0x02, //   OUTPUT (Data,Var,Abs)
	0xc0        // END_COLLECTION
};

//configuration descriptor
const unsigned char USB_hid_Configuration_Descriptor [] =
{
	0x09,		    	//blength
	0x02,		    	//bDescriptor
	0x29,0x00,	    //wTotalLength - # of bytes including interface and endpoint descpt.
	//sizeof(Configuration_Descriptor)&0xFF,
	//(sizeof(Configuration_Descriptor)>>8)&0xFF,
	0x01,		    	//bNumInterfaces - one interface
	0x01,				//bConfigurationValue -
	0x00,				//iConfiguration - index to string descriptor
	0xC0,				//bmAttributes - bit 7- Compatibility with USB 1.0
		            		//                          bit 6 if 1 self powered else Bus powered
			    			//                          bit 5-remote wakeup
			   				//                          bit 4-0-reserved
	0x32,		    	//bMaxPower - 50mA

//      Interface Descriptor
	0x09,           //blength
	0x04,           //bDescriptorType - Interface descriptor
	0x00,           //bInterfaceNumber ?????????,?????,???0?
	0x00,           //bAlternateSetting  ???????????,?0?
	0x02,           //bNumEndpoints - 2 endpoints
	0x03,           //bInterfaceClass -  ???HID??
	0x00,           //bInterfaceSubClass
	0x00,           //bInterfaceProtocol
	0x00,          //iInterface

//  Report  Descriptor
	0x09,		//bLength????HID???????????????????9???
	0x21,		//bDescriptorType???HID???????0x21?
	
#if USB_MODE_1D1
	0x10, 0x01,                        //BCDHID:             HID1.1
#elif USB_MODE_2D0
	0x00, 0x02,                        //BCDHID:             HID2.0
#else
	0x10, 0x01,                        //BCDHID:             HID1.1
#endif

	0x21,		//bCountyCode????????????,???????,??0x21?
	0x01,		//bNumDescriptors????????????????????????
	0x22,		//bDescritporType???????????,??????,???0x22?
	//bDescriptorLength????????????????????????
	//0x1B,0x00,
	sizeof(USB_hid_report_Descriptor2)&0xFF,
    (sizeof(USB_hid_report_Descriptor2)>>8)&0xFF,

//      Endpoint IN Descriptor
	0x07,           //blength
	0x05,           //bDescriptorType - EndPoint
	0x82,           //bEndpointAddress
	0x03,           //bmAttributes
#if USB_MODE_1D1
	0x40,0x00,      //wMaxPacketSize
#elif USB_MODE_2D0
	0x00, 0x02,             //bcdUSB: version 2.0
#else
	0x40,0x00,      //wMaxPacketSize
#endif
	
	
	0x01,           //bInterval

//      Endpoint OUT Descriptor
	0x07,           //blength
	0x05,           //bDescriptorType - EndPoint
	0x01,           //bEndpointAddress
	0x03,           //bmAttributes
#if USB_MODE_1D1
	0x40,0x00,      //wMaxPacketSize
#elif USB_MODE_2D0
	0x00, 0x02,             //bcdUSB: version 2.0
#else
	0x40,0x00,      //wMaxPacketSize
#endif	
	0x01,           //bInterval
};


