#ifndef USB_EXTR_H
#define USB_EXTR_H

#include "dt_defs.h"
#include "usb_type.h"

/* Variable for USB EP0 pipe (USB.c) */
extern u16_t       *pu8DescriptorEX;
extern u16_t       u16TxRxCounter;
extern u16_t       *u8ConfigDescriptorEX;
extern BOOLEAN     bUsbEP0HaltSt;
extern Action      eUsbCxFinishAction;
extern CommandType eUsbCxCommand;
extern BOOLEAN     UsbChirpFinish;
extern u16_t       u8UsbConfigValue;
extern u16_t       u8UsbInterfaceValue;
extern u16_t       u8UsbInterfaceAlternateSetting;
extern u16_t       *u8UsbDeviceDescriptor;
extern u16_t       *u8String00Descriptor;
extern u16_t       *u8String10Descriptor;
extern u16_t       *u8String20Descriptor;
extern u16_t       *u8String30Descriptor;
//extern u16_t       u8UsbCommandFlag;

//extern u16_t *u8DeviceDescriptorEX;
//extern u16_t *u8ConfigDescriptorEX;
//extern u16_t *u8OtherSpeedConfigDescriptorEX;
//extern u16_t u8DeviceQualifierDescriptorEX[];
//extern u16_t *u8HSDeviceDescriptor;
//extern u16_t *UsbDeviceDescriptor;

//extern u16_t u8HSConfigDescriptor01[];

#endif
