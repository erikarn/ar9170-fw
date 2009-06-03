#include "usb_type.h"

#define USB_DEVICE_DESC_TYPE            0x01
#define USB_DEVICE_DESC_LEN             0x12
#define USB_SPEC_VERSION                0x0200
#define USB_DEVICE_CLASS                0xFF
#define USB_DEVICE_SUB_CLASS            0xFF
#define USB_DEVICE_PROTOCOL             0xFF
#define USB_MAX_PKT_SIZE                0x40
#define USB_VENDOR_ID                   0x0ACE
#define USB_PRODUCT_ID                  0x1221
#define USB_DEVICE_BCD                  0x4890
#define USB_MANUFACTURER_INDEX          0x10
#define USB_PRODUCT_INDEX               0x20
#define USB_SERIAL_INDEX                0x00
#define USB_CONFIGURATION_NUM           0x01

#define USB_CONFIG_DESC_TYPE            0x02
#define USB_CONFIG_DESC_LEN             0x09
#define USB_TOTAL_DESC_LEN              0x002E
#define USB_INTERFACE_NUM               0x01
#define USB_CONFIG_NUM                  0x01
#define USB_STRING_INDEX                0x00
#define USB_ATTRIBUTE                   0x80
#define USB_MAX_POWER                   0xFA

#define USB_INTERFACE_DESC_TYPE         0x04
#define USB_INTERFACE_DESC_LEN          0x09
#define USB_INTERFACE_INDEX_NUM         0x00
#define USB_INTERFACE_ALT_SETTING       0x00
#define USB_INTERFACE_EP_NUM            0x04
#define USB_INTERFACE_CLASS             0xFF
#define USB_INTERFACE_SUB_CLASS         0x00
#define USB_INTERFACE_PROTOCOL          0x00
#define USB_INTERFACE_STRING_INDEX      0x00

#define USB_EP_DESC_TYPE                0x05
#define USB_EP_DESC_LEN                 0x07

/* USB Endpoint attribute */
#define bUSB_EP1_NUM                    0x01
#define bUSB_EP2_NUM                    0x02
#define bUSB_EP3_NUM                    0x03
#define bUSB_EP4_NUM                    0x04

#define bUSB_EP_DIRECTION_IN            0x80
#define bUSB_EP_DIRECTION_OUT           0x00

#define bUSB_EP_TYPE_CONTROL            0x00
#define bUSB_EP_TYPE_ISOCHRONOUS        0x01
#define bUSB_EP_TYPE_BULK               0x02
#define bUSB_EP_TYPE_INTERRUPT          0x03

#define bUSB_EP_MAX_PKT_SIZE_64         0x0040
#define bUSB_EP_MAX_PKT_SIZE_512        0x0200

/* High Speed Endpoint */
#define USB_HS_EP1_ADDRESS              (bUSB_EP_DIRECTION_OUT | bUSB_EP1_NUM)
#define USB_HS_EP1_ATTRIBUTE            bUSB_EP_TYPE_BULK
#define USB_HS_EP1_MAX_PACKET_SIZE      bUSB_EP_MAX_PKT_SIZE_512
#define USB_HS_EP1_INTERVAL             0x00

#define USB_HS_EP2_ADDRESS              (bUSB_EP_DIRECTION_IN | bUSB_EP2_NUM)
#define USB_HS_EP2_ATTRIBUTE            bUSB_EP_TYPE_BULK
#define USB_HS_EP2_MAX_PACKET_SIZE      bUSB_EP_MAX_PKT_SIZE_512
#define USB_HS_EP2_INTERVAL             0x00

#define USB_HS_EP3_ADDRESS              (bUSB_EP_DIRECTION_IN | bUSB_EP3_NUM)
#define USB_HS_EP3_ATTRIBUTE            bUSB_EP_TYPE_INTERRUPT
#define USB_HS_EP3_MAX_PACKET_SIZE      bUSB_EP_MAX_PKT_SIZE_64
#define USB_HS_EP3_INTERVAL             0x01

#define USB_HS_EP4_ADDRESS              (bUSB_EP_DIRECTION_OUT | bUSB_EP4_NUM)
#define USB_HS_EP4_ATTRIBUTE            bUSB_EP_TYPE_INTERRUPT // bUSB_EP_TYPE_BULK
#define USB_HS_EP4_MAX_PACKET_SIZE      bUSB_EP_MAX_PKT_SIZE_64
#define USB_HS_EP4_INTERVAL             0x01 // 0x00

/* Full Speed Endpoint */
#define USB_FS_EP1_ADDRESS              (bUSB_EP_DIRECTION_OUT | bUSB_EP1_NUM)
#define USB_FS_EP1_ATTRIBUTE            bUSB_EP_TYPE_BULK
#define USB_FS_EP1_MAX_PACKET_SIZE      bUSB_EP_MAX_PKT_SIZE_64
#define USB_FS_EP1_INTERVAL             0x00

#define USB_FS_EP2_ADDRESS              (bUSB_EP_DIRECTION_IN | bUSB_EP2_NUM)
#define USB_FS_EP2_ATTRIBUTE            bUSB_EP_TYPE_BULK
#define USB_FS_EP2_MAX_PACKET_SIZE      bUSB_EP_MAX_PKT_SIZE_64
#define USB_FS_EP2_INTERVAL             0x00

#define USB_FS_EP3_ADDRESS              (bUSB_EP_DIRECTION_IN | bUSB_EP3_NUM)
#define USB_FS_EP3_ATTRIBUTE            bUSB_EP_TYPE_INTERRUPT
#define USB_FS_EP3_MAX_PACKET_SIZE      bUSB_EP_MAX_PKT_SIZE_64
#define USB_FS_EP3_INTERVAL             0x01

#define USB_FS_EP4_ADDRESS              (bUSB_EP_DIRECTION_OUT | bUSB_EP4_NUM)
#define USB_FS_EP4_ATTRIBUTE            bUSB_EP_TYPE_BULK
#define USB_FS_EP4_MAX_PACKET_SIZE      bUSB_EP_MAX_PKT_SIZE_64
#define USB_FS_EP4_INTERVAL             0x00

#define USB_DEVICE_DESC_ADDR            0x137A
#define USB_STRING00_DESC_ADDR          0x138C
#define USB_STRING10_DESC_ADDR          0x1390
#define USB_STRING20_DESC_ADDR          0x13B0
#define USB_STRING30_DESC_ADDR          0x13E0

#if 0
u16_t UsbDeviceDescriptor[] =
{
    m2BYTE(USB_DEVICE_DESC_LEN, USB_DEVICE_DESC_TYPE),
    USB_SPEC_VERSION,
    m2BYTE(USB_DEVICE_CLASS, USB_DEVICE_SUB_CLASS),
    m2BYTE(USB_DEVICE_PROTOCOL, USB_MAX_PKT_SIZE),
    USB_VENDOR_ID,
    USB_PRODUCT_ID,
    USB_DEVICE_BCD,
    m2BYTE(USB_MANUFACTURER_INDEX, USB_PRODUCT_INDEX),
    m2BYTE(USB_SERIAL_INDEX, USB_CONFIGURATION_NUM)
};
#endif

u16_t u8HSConfigDescriptor01[] =
{
    m2BYTE(USB_CONFIG_DESC_LEN, USB_CONFIG_DESC_TYPE),
    USB_TOTAL_DESC_LEN,
    m2BYTE(USB_INTERFACE_NUM, USB_CONFIG_NUM),
    m2BYTE(USB_STRING_INDEX, USB_ATTRIBUTE),
    m2BYTE(USB_MAX_POWER, USB_INTERFACE_DESC_LEN),
    m2BYTE(USB_INTERFACE_DESC_TYPE, USB_INTERFACE_INDEX_NUM),
    m2BYTE(USB_INTERFACE_ALT_SETTING, USB_INTERFACE_EP_NUM),
    m2BYTE(USB_INTERFACE_CLASS, USB_INTERFACE_SUB_CLASS),
    m2BYTE(USB_INTERFACE_PROTOCOL, USB_INTERFACE_STRING_INDEX),
    m2BYTE(USB_EP_DESC_LEN, USB_EP_DESC_TYPE),                      // EP 1
    m2BYTE(USB_HS_EP1_ADDRESS, USB_HS_EP1_ATTRIBUTE),
    USB_HS_EP1_MAX_PACKET_SIZE, 
    m2BYTE(USB_HS_EP1_INTERVAL, USB_EP_DESC_LEN),                   // EP 2
    m2BYTE(USB_EP_DESC_TYPE, USB_HS_EP2_ADDRESS),
    m2BYTE(USB_HS_EP2_ATTRIBUTE, USB_HS_EP2_MAX_PACKET_SIZE),
    m2BYTE(mHIGH_BYTE(USB_HS_EP2_MAX_PACKET_SIZE), USB_HS_EP2_INTERVAL),
    m2BYTE(USB_EP_DESC_LEN, USB_EP_DESC_TYPE),                      // EP 3
    m2BYTE(USB_HS_EP3_ADDRESS, USB_HS_EP3_ATTRIBUTE),
    USB_HS_EP3_MAX_PACKET_SIZE,
    m2BYTE(USB_HS_EP3_INTERVAL, USB_EP_DESC_LEN),                   // EP 4
    m2BYTE(USB_EP_DESC_TYPE, USB_HS_EP4_ADDRESS),
    m2BYTE(USB_HS_EP4_ATTRIBUTE, USB_HS_EP4_MAX_PACKET_SIZE),
    m2BYTE(mHIGH_BYTE(USB_HS_EP4_MAX_PACKET_SIZE), USB_HS_EP4_INTERVAL)
};

u16_t u8FSConfigDescriptor01[] =
{
    m2BYTE(USB_CONFIG_DESC_LEN, USB_CONFIG_DESC_TYPE),
    USB_TOTAL_DESC_LEN,
    m2BYTE(USB_INTERFACE_NUM, USB_CONFIG_NUM),
    m2BYTE(USB_STRING_INDEX, USB_ATTRIBUTE),
    m2BYTE(USB_MAX_POWER, USB_INTERFACE_DESC_LEN),
    m2BYTE(USB_INTERFACE_DESC_TYPE, USB_INTERFACE_INDEX_NUM),
    m2BYTE(USB_INTERFACE_ALT_SETTING, USB_INTERFACE_EP_NUM),
    m2BYTE(USB_INTERFACE_CLASS, USB_INTERFACE_SUB_CLASS),
    m2BYTE(USB_INTERFACE_PROTOCOL, USB_INTERFACE_STRING_INDEX),
    m2BYTE(USB_EP_DESC_LEN, USB_EP_DESC_TYPE),                      // EP 1
    m2BYTE(USB_FS_EP1_ADDRESS, USB_FS_EP1_ATTRIBUTE),
    USB_FS_EP1_MAX_PACKET_SIZE,
    m2BYTE(USB_FS_EP1_INTERVAL, USB_EP_DESC_LEN),                   // EP 2
    m2BYTE(USB_EP_DESC_TYPE, USB_FS_EP2_ADDRESS),
    m2BYTE(USB_FS_EP2_ATTRIBUTE, USB_FS_EP2_MAX_PACKET_SIZE),
    m2BYTE(mHIGH_BYTE(USB_FS_EP2_MAX_PACKET_SIZE), USB_FS_EP2_INTERVAL),
    m2BYTE(USB_EP_DESC_LEN, USB_EP_DESC_TYPE),                      // EP 3
    m2BYTE(USB_FS_EP3_ADDRESS, USB_FS_EP3_ATTRIBUTE),
    USB_FS_EP3_MAX_PACKET_SIZE, 
    m2BYTE(USB_FS_EP3_INTERVAL, USB_EP_DESC_LEN),                   // EP 4
    m2BYTE(USB_EP_DESC_TYPE, USB_FS_EP4_ADDRESS),
    m2BYTE(USB_FS_EP4_ATTRIBUTE, USB_FS_EP4_MAX_PACKET_SIZE),
    m2BYTE(mHIGH_BYTE(USB_FS_EP4_MAX_PACKET_SIZE), USB_FS_EP4_INTERVAL)
};

u16_t u8DeviceQualifierDescriptorEX[] =
{
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000
};

u16_t u8OtherSpeedConfigDescriptorEX[] =
{
    0x0709,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000
};
