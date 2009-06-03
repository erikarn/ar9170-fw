#ifndef USB_STD_H
#define USB_STD_H

#define mDEV_REQ_REQ_DIR()      (ControlCmd.Direction)
#define mDEV_REQ_REQ_TYPE()     (ControlCmd.Type)
#define mDEV_REQ_REQ_RECI()     (ControlCmd.Object)
#define mDEV_REQ_REQ()          (ControlCmd.Request)
#define mDEV_REQ_VALUE()        (ControlCmd.Value)
#define mDEV_REQ_VALUE_LOW()    (mLOW_BYTE(mDEV_REQ_VALUE()))
#define mDEV_REQ_VALUE_HIGH()   (mHIGH_BYTE(mDEV_REQ_VALUE()))
#define mDEV_REQ_INDEX()        (ControlCmd.Index)
#define mDEV_REQ_LENGTH()       (ControlCmd.Length)

/******************** USB Protocol Definition *************************/
/* Standard Request Code (Table 9-4) */
#define USB_GET_STATUS          0
#define USB_CLEAR_FEATURE       1
#define USB_SET_FEATURE         3
#define USB_SET_ADDRESS         5
#define USB_GET_DESCRIPTOR      6
#define USB_SET_DESCRIPTOR      7
#define USB_GET_CONFIGURATION   8
#define USB_SET_CONFIGURATION   9
#define USB_GET_INTERFACE       10
#define USB_SET_INTERFACE       11
#define USB_SYNCH_FRAME         12

/* Descriptor Type (Table 9-5) */
#define USB_DESC_TYPE_DEVICE    1
#define USB_DESC_TYPE_CONFIG    2
#define USB_DESC_TYPE_STRING    3
#define USB_DESC_TYPE_INTERFACE 4
#define USB_DESC_TYPE_ENDPOINT  5

/* Endpoint Attribute (Table 9-10) */
#define USB_EP_ATTR_CTRL        0
#define USB_EP_ATTR_ISOCH       1
#define USB_EP_ATTR_BULK        2
#define USB_EP_ATTR_INTRPT      3

/*********************** for USB 2.0 **********************************/
// Table 9-5. Descriptor Types
#define DT_DEVICE                       1
#define DT_CONFIGURATION                2
#define DT_STRING                       3
#define DT_INTERFACE                    4
#define DT_ENDPOINT                     5
#define DT_DEVICE_QUALIFIER             6
#define DT_OTHER_SPEED_CONFIGURATION    7
#define DT_INTERFACE_POWER              8

/**********************************************************************/
// Values for bmAttributes Field in USB_CONFIGURATION_DESCRIPTOR
#define USB_BUS_POWERED         0x80
#define USB_SELF_POWERED        0x40
#define USB_REMOTE_WAKEUP       0x20

#define cUSB_REQTYPE_DIR_POS    7
#define cUSB_REQTYPE_DIR_LEN    1
#define cUSB_REQTYPE_TYPE_POS   5
#define cUSB_REQTYPE_TYPE_LEN   2
#define cUSB_REQTYPE_RX_POS     0
#define cUSB_REQTYPE_RX_LEN     5

/* for USB State */
#define cUSB_DEFAULT_STATE      0
#define cUSB_ADDRESS_STATE      1
#define cUSB_CONFIG_STATE       2

/* for Data transfer direction */
#define bmUSB_HOST_DIR          7     /* Bit 7 */
#define cUSB_DIR_HOST_OUT       0
#define cUSB_DIR_HOST_IN        1

/* for Type */
#define cUSB_REQTYPE_STD        0
#define cUSB_REQTYPE_CLASS      1
#define cUSB_REQTYPE_VENDOR     2

/* for Recipient */
#define cUSB_REQTYPE_DEVICE     0
#define cUSB_REQTYPE_INTERFACE  1
#define cUSB_REQTYPE_ENDPOINT   2
#define cUSB_REQTYPE_OTHER      3

/* for Descriptor Type */
#define cUSB_DESTYPE_DEVICE     1
#define cUSB_DESTYPE_CONFIG     2
#define cUSB_DESTYPE_STRING     3
#define cUSB_DESTYPE_INTERFACE  4
#define cUSB_DESTYPE_ENDPOINT   5
#define cUSB_DESTYPE_END        cUSB_DESTYPE_ENDPOINT   // for range check

/* for Feature selector */
#define cUSB_FEATSEL_RMWAKEUP   0
#define cUSB_FEATSEL_EPHALT     1
#define cUSB_FEATSEL_END        cUSB_FEATSEL_EPHALT     // for range check

#define bmREQ_RECI              0
#define bmwREQ_RECI             5           // mMASKS(bmwREQ_RECI, bmREQ_RECI)
#define bmREQ_TYPE              5
#define bmwREQ_TYPE             2           // mMASKS(bmwREQ_TYPE, bmREQ_TYPE)
#define bmREQ_DIR               7
#define bmwREQ_DIR              1

#endif

