// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : USB_CCID_config.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "type.h"
#include "usb_const.h"

//½Ó´¥×Ö·û´®
const UINT8 SmartCard_Reader[34] =
{
	34,				// Num bytes of this descriptor
	DT_STRING,		// String descriptor
	'S',	0,
	'm',	0,
	'a',	0,
	'r',	0,
	't',	0,
	'C',	0,
	'a',	0,
	'r',	0,
	'd',	0,
	' ',	0,
	'R',	0,
	'e',	0,
	'a',	0,
	'd',	0,
	'e',	0,
	'r',	0
};

const UINT8 DATA_RATE[0xB0] =
{
	0x00,0x2a,0x00,0x00,
	0x00,0x38,0x00,0x00,
	0x09,0x3d,0x00,0x00,
	0x34,0x43,0x00,0x00,
	0x61,0x51,0x00,0x00,
	0x01,0x54,0x00,0x00,
	0x8d,0x5b,0x00,0x00,
	0xce,0x64,0x00,0x00,
	0x01,0x70,0x00,0x00,
	0x12,0x7a,0x00,0x00,
	0x02,0x7e,0x00,0x00,
	0x68,0x86,0x00,0x00,
	0x96,0x98,0x00,0x00,
	0xc2,0xa2,0x00,0x00,
	0x02,0xa8,0x00,0x00,
	0x1b,0xb7,0x00,0x00,
	0x73,0xcb,0x00,0x00,
	0x03,0xd2,0x00,0x00,
	0x03,0xe0,0x00,0x00,
	0x24,0xf4,0x00,0x00,
	0x04,0xfc,0x00,0x00,
	0xd1,0x0c,0x01,0x00,
	0x04,0x18,0x01,0x00,
	0x2d,0x31,0x01,0x00,
	0x85,0x45,0x01,0x00,
	0x05,0x50,0x01,0x00,
	0x36,0x6e,0x01,0x00,
	0xe6,0x96,0x01,0x00,
	0x06,0xa4,0x01,0x00,
	0x07,0xc0,0x01,0x00,
	0x48,0xe8,0x01,0x00,
	0x08,0xf8,0x01,0x00,
	0xa2,0x19,0x02,0x00,
	0x09,0x30,0x02,0x00,
	0x5a,0x62,0x02,0x00,
	0x0a,0x8b,0x02,0x00,
	0x0b,0xa0,0x02,0x00,
	0x0d,0x48,0x03,0x00,
	0x0e,0x80,0x03,0x00,
	0x90,0xd0,0x03,0x00,
	0x15,0x16,0x05,0x00,
	0x16,0x40,0x05,0x00,
	0x1d,0x00,0x07,0x00,
	0x20,0xa1,0x07,0x00,
};

//standard Device Descriptor
const UINT8 USB_CCID_Device_Descriptor[18] =
{
        //Device (0x12 bytes)
        0x12,                   //bLength: 0x12 bytes
        DT_DEVICE,              //bDescriptorType: DEVICE
        0x00, 0x02,             //bcdUSB: version 2.0
        0x00,                   //bDeviceClass: independent interfaces
        0x00,                   //bDeviceSubClass: 0
        0x00,                   //bDeviceProtocol: class specific protocols NOT used on device basis
        0x40,                   //bMaxPacketSize0: maximum packet size for endpoint zero
        0x25, 0x07,             //idVendor: vendor ID
        0x01, 0x10,             //idProduct: product ID //900515But org=503
        0x03, 0x01,             //bcdDevice: device release number in BCD
        0x01,                   //iManufacturer: index of string
        0x02,                   //iProduct: index of manufacturer string
        0x00,                   //iSerialNumber: index of product string
        0x01,                   //bNumConfigurations: 1 configuration
};

//Device Qualifier Descriptor
const UINT8 USB_CCID_Device_Qualifier_Descriptor[10] =
{
        //Device Qualifier(0x0A bytes)
        0x0A,                   //bLength: 0x12 bytes
        DT_DEVICE_QUALIFIER,    //bDescriptorType: DEVICE
        0x00, 0x02,             //bcdUSB: version 2.0
        0x00,                   //bDeviceClass: independent interfaces
        0x00,                   //bDeviceSubClass: 0
        0x00,                   //bDeviceProtocol: class specific protocols NOT used on device basis
        0x40,                   //bMaxPacketSize0 for other speed
        0x01,                   //bNumConfigurations for other-speed configuration
        0x00,                   //Reserved,must be 0
};

const UINT8 USB_CCID_Configuration_Descriptor [] =
{
	0x09,					          //bLength
	DT_CONFIGURATION,		    //bDescriptor
	0x5D,0x00,				      //wTotalLength
	0x01,					          //bNumInterface
	0x01,				      	    //bConfiguration Value
	0x00,					          //iConfiguration
	0x80,					          //bmAttributes,BUS-POWER
	0x64,					          //bMaxPower,200mA

	//Interface 0, AlternateSetting 0 (0x09 bytes)
	0x09,                   //bLength: 0x09 bytes
	DT_INTERFACE,           //bDescriptorType: INTERFACE
	0x00,                   //bInterfaceNumber: interface 0
	0x00,                   //bAlternateSetting: alternate setting 0
	0x03,                   //bNumEndpoints: 2 endpoint + zero ep
	0x0B,                   //bInterfaceClass: vendor specific interface
	0x00,                   //bInterfaceSubClass: 06 SCSI Command Set
	0x00,                   //bInterfaceProtocol: class specific protocol NOT used on this interface
	0x04,                   //iInterface: index of string

	 //Smart Card Device Class Descriptor
	0x36,					          //bLength
	0x21,				          	//bDescriptorType
	0x10,0x01,              //bcdCCID: version 1.1
	0x00,					          //bMaxSlotIndex
	0x01,					          //bVoltageSupport  01->5.0  02->3.0  03->1.8
	0x02,0x00,0x00,0x00,    //dwProtocols  0001->T=0  0002->T=1  0003->T=0,T=1
	0xc0,0x12,0x00,0x00,    //dwDefaultClock
	0xc0,0x12,0x00,0x00,    //dwMaximumClock
	0x00,					          //bNumClockSupported
	0x67,0x32,0x00,0x00,    //dwDataRate  9600
	0xe7,0x4c,0x00,0x00,    //dwMaxDataRate
	0x00,					          //bNumDataRatesSupported
	0x00,0x01,0x00,0x00,    //dwMaxIFSD
	0x00,0x00,0x00,0x00,    //dwSynchProtocols
	0x00,0x00,0x00,0x00,    //dwMechanical
	0xB2,0x00,0x02,0x00,    //dwFeatures
	0x0f,0x01,0x00,0x00,    //dwMacCCIDMessageLength
	0x00,				          	//bClassGetResponse
	0x00,					          //bClassEnvelope
	0x00,0x00,              //wLcdLayout
	0x00,					          //bPINSupport
	0x01,					          //bMaxCCIDBusySlots

	//Endpoint 1 (0x07 bytes)
	0x07,                   //bLength: 0x07 bytes
	DT_ENDPOINT,            //bDescriptorType: ENDPOINT
	0X01,                   //bEndpointAddress: out endpoint 1
	TRANS_BULK,             //bmAttributes: BULK-ONLY
	0x00, 0x02,             //wMaxPacketSize: 64 bytes
	0x00,					          //bInterval: polling interval is 1 ms
	
	//Endpoint 1 (0x07 bytes)
	0x07,                   //bLength: 0x07 bytes
	DT_ENDPOINT,            //bDescriptorType: ENDPOINT
	0x81,                   //bEndpointAddress: IN endpoint 1
	TRANS_BULK,             //bmAttributes: BULK-ONLY(00:Control 01:isoch 10:bulk 11:intr
	0x00, 0x02,             //wMaxPacketSize: 64 bytes
	0x00,                   //bInterval: polling interval is 1 ms

	//Endpoint 2 (0x07 bytes)
	0x07,                   //bLength: 0x07 bytes
	DT_ENDPOINT,            //bDescriptorType: ENDPOINT
	0x82,                   //bEndpointAddress: IN endpoint 2
	TRANS_INT,              //bmAttributes: INT(00:Control 01:isoch 10:bulk 11:intr
	0x00, 0x02,             //wMaxPacketSize: 64 bytes, int max size is 1024
	0x32,                   //bInterval: polling interval is 1 ms
};

//Product String
const UINT8 USB_CCID_StrDescProduct[32] =
{
	32,				// Num bytes of this descriptor
	DT_STRING,				// String descriptor
	'U',	0,
	'S',	0,
	'B',	0,
	' ',	0,
	'C',	0,
	'C',	0,
	'I',	0,
	'D',	0,
	' ',	0,
	'D',	0,
	'E',	0,
	'V',	0,
	'I',	0,
	'C',	0,
	'E',	0
};

