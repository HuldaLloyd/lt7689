// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : usbdev_scsi.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "usb_drv.h"
#include "module_select.h"

void USBC_IRQHandler(void)
{
//#ifdef USB_CCID
//	USB_CCID_ISR();
//#endif

//#ifdef USB_MS
//	USB_MS_ISR();
//	USBDev_Polling();
//#endif
	
#if (UARTBUS_OPTION == 3)
	USB_HID_ISR();
	USB_HID_Polling();
#endif
	
#if (UARTBUS_OPTION == 4)
	USB_CDC_ISR();
#endif
}

