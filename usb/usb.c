#include "usb_type.h"
#include "usb_defs.h"
#include "usb_pre.h"
#include "usb_extr.h"
#include "usb_std.h"
#include "reg_defs.h"

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
#define USB_HS_EP4_ATTRIBUTE            bUSB_EP_TYPE_INTERRUPT	// bUSB_EP_TYPE_BULK
#define USB_HS_EP4_MAX_PACKET_SIZE      bUSB_EP_MAX_PKT_SIZE_64
#define USB_HS_EP4_INTERVAL             0x01	// 0x00

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

u16_t u8HSConfigDescriptor01[] = {
	m2BYTE(USB_CONFIG_DESC_LEN, USB_CONFIG_DESC_TYPE),
	USB_TOTAL_DESC_LEN,
	m2BYTE(USB_INTERFACE_NUM, USB_CONFIG_NUM),
	m2BYTE(USB_STRING_INDEX, USB_ATTRIBUTE),
	m2BYTE(USB_MAX_POWER, USB_INTERFACE_DESC_LEN),
	m2BYTE(USB_INTERFACE_DESC_TYPE, USB_INTERFACE_INDEX_NUM),
	m2BYTE(USB_INTERFACE_ALT_SETTING, USB_INTERFACE_EP_NUM),
	m2BYTE(USB_INTERFACE_CLASS, USB_INTERFACE_SUB_CLASS),
	m2BYTE(USB_INTERFACE_PROTOCOL, USB_INTERFACE_STRING_INDEX),
	m2BYTE(USB_EP_DESC_LEN, USB_EP_DESC_TYPE),	// EP 1
	m2BYTE(USB_HS_EP1_ADDRESS, USB_HS_EP1_ATTRIBUTE),
	USB_HS_EP1_MAX_PACKET_SIZE,
	m2BYTE(USB_HS_EP1_INTERVAL, USB_EP_DESC_LEN),	// EP 2
	m2BYTE(USB_EP_DESC_TYPE, USB_HS_EP2_ADDRESS),
	m2BYTE(USB_HS_EP2_ATTRIBUTE, USB_HS_EP2_MAX_PACKET_SIZE),
	m2BYTE(mHIGH_BYTE(USB_HS_EP2_MAX_PACKET_SIZE), USB_HS_EP2_INTERVAL),
	m2BYTE(USB_EP_DESC_LEN, USB_EP_DESC_TYPE),	// EP 3
	m2BYTE(USB_HS_EP3_ADDRESS, USB_HS_EP3_ATTRIBUTE),
	USB_HS_EP3_MAX_PACKET_SIZE,
	m2BYTE(USB_HS_EP3_INTERVAL, USB_EP_DESC_LEN),	// EP 4
	m2BYTE(USB_EP_DESC_TYPE, USB_HS_EP4_ADDRESS),
	m2BYTE(USB_HS_EP4_ATTRIBUTE, USB_HS_EP4_MAX_PACKET_SIZE),
	m2BYTE(mHIGH_BYTE(USB_HS_EP4_MAX_PACKET_SIZE), USB_HS_EP4_INTERVAL)
};

u16_t u8FSConfigDescriptor01[] = {
	m2BYTE(USB_CONFIG_DESC_LEN, USB_CONFIG_DESC_TYPE),
	USB_TOTAL_DESC_LEN,
	m2BYTE(USB_INTERFACE_NUM, USB_CONFIG_NUM),
	m2BYTE(USB_STRING_INDEX, USB_ATTRIBUTE),
	m2BYTE(USB_MAX_POWER, USB_INTERFACE_DESC_LEN),
	m2BYTE(USB_INTERFACE_DESC_TYPE, USB_INTERFACE_INDEX_NUM),
	m2BYTE(USB_INTERFACE_ALT_SETTING, USB_INTERFACE_EP_NUM),
	m2BYTE(USB_INTERFACE_CLASS, USB_INTERFACE_SUB_CLASS),
	m2BYTE(USB_INTERFACE_PROTOCOL, USB_INTERFACE_STRING_INDEX),
	m2BYTE(USB_EP_DESC_LEN, USB_EP_DESC_TYPE),	// EP 1
	m2BYTE(USB_FS_EP1_ADDRESS, USB_FS_EP1_ATTRIBUTE),
	USB_FS_EP1_MAX_PACKET_SIZE,
	m2BYTE(USB_FS_EP1_INTERVAL, USB_EP_DESC_LEN),	// EP 2
	m2BYTE(USB_EP_DESC_TYPE, USB_FS_EP2_ADDRESS),
	m2BYTE(USB_FS_EP2_ATTRIBUTE, USB_FS_EP2_MAX_PACKET_SIZE),
	m2BYTE(mHIGH_BYTE(USB_FS_EP2_MAX_PACKET_SIZE), USB_FS_EP2_INTERVAL),
	m2BYTE(USB_EP_DESC_LEN, USB_EP_DESC_TYPE),	// EP 3
	m2BYTE(USB_FS_EP3_ADDRESS, USB_FS_EP3_ATTRIBUTE),
	USB_FS_EP3_MAX_PACKET_SIZE,
	m2BYTE(USB_FS_EP3_INTERVAL, USB_EP_DESC_LEN),	// EP 4
	m2BYTE(USB_EP_DESC_TYPE, USB_FS_EP4_ADDRESS),
	m2BYTE(USB_FS_EP4_ATTRIBUTE, USB_FS_EP4_MAX_PACKET_SIZE),
	m2BYTE(mHIGH_BYTE(USB_FS_EP4_MAX_PACKET_SIZE), USB_FS_EP4_INTERVAL)
};

u16_t u8DeviceQualifierDescriptorEX[] = {
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000
};

u16_t u8OtherSpeedConfigDescriptorEX[] = {
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

/*
 * stream mode setting
 *
 * Needs to be RX stream/TX packet for ar9170.
 */
#define USB_TX_STREAM_MODE	0
#define USB_RX_STREAM_MODE	1

//extern u16_t u8HSConfigDescriptor01[];

extern u16_t *u8ConfigDescriptorEX;
extern void zfUartSendStr(u8_t * str);
extern void vUsbFIFO_EPxCfg_HS(void);
extern void vUsbFIFO_EPxCfg_FS(void);
extern void zfUartSendStrAndHex(u8_t * str, u32_t val);

SetupPacket ControlCmd;

void vUsbEP0TxData(void);
void vUsbEP0RxData(void);

/* Flag definition */
#define fCHK_LOADCODE              0

#define fBUS_POWER                 1
u16_t UsbStatus[3];

void zfPtaModeInit(void)
{
	/* Set PTA mode to USB */
	ZM_PTA_DMA_MODE_CTRL_REG &= ~BIT1;

	/* Do a software reset to PTA component */
	ZM_PTA_DMA_MODE_CTRL_REG |= BIT0;
	ZM_PTA_DMA_MODE_CTRL_REG &= ~BIT0;

	if (mUsbHighSpeedST())	// First judge HS or FS??
	{
		// Set into 512 byte mode */
		ZM_SOC_USB_MODE_CTRL_REG |= BIT2;
	}
#if USB_RX_STREAM_MODE == 1
	/* Set mode into stream mode */
	ZM_SOC_USB_MODE_CTRL_REG &= ~BIT3;
#else
	/* Set mode into packet mode */
	ZM_SOC_USB_MODE_CTRL_REG |= BIT3;
#endif

#if USB_TX_STREAM_MODE == 1
	ZM_SOC_USB_MODE_CTRL_REG |= BIT6;
#endif

	/* Set maximum transfer size to 8K */
	ZM_SOC_USB_MODE_CTRL_REG &= 0xcf;
	ZM_SOC_USB_MODE_CTRL_REG |= 0x10;

#ifdef AHB_LOOPBACK
	/* Disable up stream and down stream */
	ZM_SOC_USB_MODE_CTRL_REG &= ~(BIT0 | BIT1);
	//zfUartSendStr((u8_t *) "Disable up stream and down stream\n");
#else
	/* Enable up stream and down stream */
	ZM_SOC_USB_MODE_CTRL_REG |= (BIT0 | BIT1);
	//ZM_SOC_USB_MODE_CTRL_REG |= BIT0;
	//zfUartSendStr((u8_t *) "Enable up stream and down stream\n");
#endif

	/* Set the up stream mode maximum aggregate number */
	ZM_SOC_USB_MAX_AGGREGATE_REG = 0x02;

	/* Set the up stream mode timeout value */
	ZM_SOC_USB_TIME_CTRL_REG = 0x100;
}

void zfUsbInit(void)
{
// init variables
//    u16TxRxCounter = 0;
//    eUsbCxCommand = CMD_VOID;
//    u8UsbConfigValue = 0;
	u8UsbInterfaceValue = 0;
	u8UsbInterfaceAlternateSetting = 0;
	bUsbEP0HaltSt = FALSE;
//    u8UsbCommandFlag = 0;
//    eUsbCxFinishAction = ACT_IDLE;
//    #if fLESS_SPEC_CODE
	UsbStatus[0] = !fBUS_POWER;
//    #endif

// init hardware
//    vFUSB200Init();
//    ZM_PHY_TEST_SELECT_REG = 0;             // Plug In

	zfPtaModeInit();
	//ZM_CBUS_CTRL_REG = 0x02;
	//ZM_CBUS_CTRL_REG = 0x01;

	//mUsbApWrEnd();
	//mUsbApRdEnd();
}

static void mUsbEPinRsTgSet(u8_t u8ep)
{
	u8_t *reg =
	    (u8_t *) (ZM_FUSB_BASE + ZM_EP_IN_MAX_SIZE_HIGH_OFFSET +
		      (u8ep << 1));
	*reg |= BIT4;
}

static void mUsbEPinRsTgClr(u8_t u8ep)
{
	u8_t *reg =
	    (u8_t *) (ZM_FUSB_BASE + ZM_EP_IN_MAX_SIZE_HIGH_OFFSET +
		      (u8ep << 1));
	*reg &= ~BIT4;
}

static void mUsbEPoutRsTgSet(u8_t u8ep)
{
	u8_t *reg =
	    (u8_t *) (ZM_FUSB_BASE + ZM_EP_OUT_MAX_SIZE_HIGH_OFFSET +
		      (u8ep << 1));
	*reg |= BIT4;
}

static void mUsbEPoutRsTgClr(u8_t u8ep)
{
	u8_t *reg =
	    (u8_t *) (ZM_FUSB_BASE + ZM_EP_OUT_MAX_SIZE_HIGH_OFFSET +
		      (u8ep << 1));
	*reg &= ~BIT4;
}

/***********************************************************************/
//      vUsb_ep0tx()
//      Description:
//          1. Transmit data to EP0 FIFO.
//      input: none
//      output: none
/***********************************************************************/
void vUsb_ep0tx(void)
{
	switch (eUsbCxCommand) {
	case CMD_GET_DESCRIPTOR:
		vUsbEP0TxData();
		break;
	default:
		/* Mark endpoint STALL */
		ZM_CX_CONFIG_STATUS_REG = 0x40;
		break;
	}

	eUsbCxFinishAction = ACT_IDLE;
}

/***********************************************************************/
//      vUsb_ep0rx()
//      Description:
//          1. Receive data from EP0 FIFO.
//      input: none
//      output: none
/***********************************************************************/
void vUsb_ep0rx(void)
{
	void vUsbEP0RxData(void);

	switch (eUsbCxCommand) {
	case CMD_SET_DESCRIPTOR:
		vUsbEP0RxData();
		break;
	default:
		//mUsbEP0StallSet();
		break;
	}

	eUsbCxFinishAction = ACT_IDLE;
}

/***********************************************************************/
//      vUsb_ep0end()
//      Description:
//          1. End this transfer.
//      input: none
//      output: none
/***********************************************************************/
void vUsb_ep0end(void)
{
	eUsbCxCommand = CMD_VOID;
	ZM_CX_CONFIG_STATUS_REG = 0x01;
	//mUsbEP0DoneSet();                               // Return EP0_Done flag
}

/***********************************************************************/
//      vUsb_ep0fail()
//      Description:
//          1. Stall this transfer.
//      input: none
//      output: none
/***********************************************************************/
void vUsb_ep0fail(void)
{
	ZM_CX_CONFIG_STATUS_REG = 0x04;
	//mUsbEP0StallSet();                              // Return EP0_Stall
}

/***********************************************************************/
//      vUsb_rst()
//      Description:
//          1. Change descriptor table (High or Full speed).
//      input: none
//      output: none
/***********************************************************************/
void vUsb_rst(void)
{
	//mUsbIntBusRstClr();
	zfUartSendStr((u8_t *) "vUsb_rst\n");
	ZM_INTR_SOURCE_7_REG &= ~0x02;

	UsbChirpFinish = FALSE;
}

/***********************************************************************/
//      vUsb_suspend()
//      Description:
//          1. .
//      input: none
//      output: none
/***********************************************************************/
void vUsb_suspend(void)
{
	/* write watchdog magic pattern for suspend  */
	ZM_WATCH_DOG_MAGIC_REG &= 0xFFFF;
	ZM_WATCH_DOG_MAGIC_REG |= 0x98760000;

	/* Disable watchdog */
	ZM_WATCH_DOG_REG = 0xFFFF;

// uP must do-over everything it should handle
// and do before into the suspend mode
	//mUsbIntSuspClr();                       // Go Suspend status
	zfUartSendStr((u8_t *) "vUsb_suspend\n");
	ZM_INTR_SOURCE_7_REG &= ~0x04;

//    mSHOW_MSG(" Sp");
//    mSHOW_CHAR1('p');
}

/***********************************************************************/
//      vUsb_resm()
//      Description:
//          1. Change descriptor table (High or Full speed).
//      input: none
//      output: none
/***********************************************************************/
void vUsb_resm(void)
{
// uP must do-over everything it should handle
// and do before into the suspend mode

//    mUsbIntResmClr();                       // uP must wakeup immediately
	zfUartSendStr((u8_t *) "vUsb_resm\n");
	ZM_INTR_SOURCE_7_REG &= ~0x08;
//    mSHOW_MSG(" Rm");
//    mSHOW_CHAR1('m');
}

void vUsb_Data_Out0Byte(void)
{
	ZM_INTR_SOURCE_7_REG &= ~BIT7;
}

void vUsb_Data_In0Byte(void)
{
	ZM_INTR_SOURCE_7_REG &= ~BIT6;
}

/***********************************************************************/
//      bGet_status()
//      Description:
//          1. Send 2 bytes status to host.
//      input: none
//      output: TRUE or FALSE (BOOLEAN)
/***********************************************************************/
static BOOLEAN bGet_status(void)
{
	u8_t RecipientStatusLow;

	RecipientStatusLow = UsbStatus[mDEV_REQ_REQ_RECI() & 0x0F];

	ZM_CBUS_FIFO_SIZE_REG = 0x3;
	ZM_EP0_DATA_REG = RecipientStatusLow;

	// Restore CBus FIFO size to word size
	ZM_CBUS_FIFO_SIZE_REG = 0xf;

	eUsbCxFinishAction = ACT_DONE;
	return TRUE;
}

/***********************************************************************/
//      bClear_feature()
//      Description:
//          1. Send 2 bytes status to host.
//      input: none
//      output: TRUE or FALSE (BOOLEAN)
/***********************************************************************/
static BOOLEAN bClear_feature(void)
{
	if (mDEV_REQ_VALUE() > cUSB_FEATSEL_END)
		return FALSE;

	UsbStatus[2] = 0;
	eUsbCxFinishAction = ACT_DONE;

	return TRUE;
}

/***********************************************************************/
//      bSet_feature()
//      Description:
//          1. Send 2 bytes status to host.
//      input: none
//      output: TRUE or FALSE (BOOLEAN)
/***********************************************************************/
static BOOLEAN bSet_feature(void)
{

	switch (mDEV_REQ_VALUE())	// FeatureSelector
	{
	case 0:		// ENDPOINT_HALE
		eUsbCxFinishAction = ACT_DONE;
		break;

	case 1:		// Device Remote Wakeup
		// Set "Device_Remote_Wakeup", Turn on the"RMWKUP" bit in Mode Register
		mUsbRmWkupSet();
		eUsbCxFinishAction = ACT_DONE;
		break;

	default:
		return FALSE;
	}
	if (eUsbCxFinishAction == ACT_DONE)
		UsbStatus[2] = 1;

	return TRUE;
}

/***********************************************************************/
//      bSet_address()
//      Description:
//          1. Set addr to FUSB200 register.
//      input: none
//      output: TRUE or FALSE (BOOLEAN)
/***********************************************************************/
static BOOLEAN bSet_address(void)
{
	if (mDEV_REQ_VALUE() >= 0x0100)
		return FALSE;
	else {
		//zfUartSendStrAndHex((u8_t *) "USB_SET_ADDRESS=", mDEV_REQ_VALUE());
		ZM_DEVICE_ADDRESS_REG = mDEV_REQ_VALUE();
		eUsbCxFinishAction = ACT_DONE;
		return TRUE;
	}
}

/***********************************************************************/
//      bGet_descriptor()
//      Description:
//          1. Point to the start location of the correct descriptor.
//          2. set the transfer length
//      input: none
//      output: TRUE or FALSE (BOOLEAN)
/***********************************************************************/
static BOOLEAN bGet_descriptor(void)
{
// Change Descriptor type
	switch (mDEV_REQ_VALUE_HIGH()) {
	case 1:		// device descriptor
		//pu8DescriptorEX = u8DeviceDescriptorEX;
		//u16TxRxCounter = mTABLE_LEN(u8DeviceDescriptorEX[0]);
		//zfUartSendStr((u8_t *)"Device Descriptor\n");
		pu8DescriptorEX = u8UsbDeviceDescriptor;
		u16TxRxCounter = mTABLE_LEN(u8UsbDeviceDescriptor[0]);
		break;

	case 2:		// configuration descriptor
		// It includes Configuration, Interface and Endpoint Table
		//zfUartSendStr((u8_t *)"Configuration Descriptor\n");
		switch (mDEV_REQ_VALUE_LOW()) {
		case 0x00:	// configuration no: 0
			pu8DescriptorEX = u8ConfigDescriptorEX;
			u16TxRxCounter = u8ConfigDescriptorEX[1];;
			//u16TxRxCounter = u8ConfigDescriptorEX[mTABLE_IDX(2)];
			break;
		default:
			return FALSE;
		}
		break;

	case 3:		// string descriptor
		// DescriptorIndex = low_byte of wValue
		//zfUartSendStr((u8_t *)"String Descriptor\n");
		switch (mDEV_REQ_VALUE_LOW()) {
		case 0x00:
			//zfUartSendStr((u8_t *)"Index 0x00\n");
			pu8DescriptorEX = u8String00Descriptor;
			//u16TxRxCounter = 4;
			break;

		case 0x10:
			//zfUartSendStr((u8_t *)"Index 0x10\n");
			pu8DescriptorEX = u8String10Descriptor;
			//u16TxRxCounter = 12;
			break;

		case 0x20:
			//zfUartSendStr((u8_t *)"Index 0x20\n");
			pu8DescriptorEX = u8String20Descriptor;
			//u16TxRxCounter = 24;
			break;

		case 0x30:
			pu8DescriptorEX = u8String30Descriptor;
			break;

		default:
			return FALSE;
		}
		u16TxRxCounter = mTABLE_LEN(pu8DescriptorEX[0]);
		break;

	case 6:		// Device_Qualifier descritor
		//zfUartSendStr((u8_t *) "Device_Qualifier Descriptor\n");
		pu8DescriptorEX = u8DeviceQualifierDescriptorEX;
		u16TxRxCounter = mTABLE_LEN(u8DeviceQualifierDescriptorEX[0]);
		//u16TxRxCounter = mTABLE_LEN(u8DeviceQualifierDescriptorEX[0]);
		break;

	case 7:		// Other_Speed_Configuration
		//zfUartSendStr((u8_t *)"Other_Speed Descriptor\n");
		// It includes Configuration, Interface and Endpoint Table
		pu8DescriptorEX = u8OtherSpeedConfigDescriptorEX;
		u16TxRxCounter = u8OtherSpeedConfigDescriptorEX[1];
		//u16TxRxCounter = u8OtherSpeedConfigDescriptorEX[mTABLE_IDX(2)];
		break;

	default:
		//zfUartSendStrAndHex((u8_t *) "Descriptor error=", mDEV_REQ_VALUE_HIGH());
		return FALSE;
	}

	if (u16TxRxCounter > mDEV_REQ_LENGTH())
		u16TxRxCounter = mDEV_REQ_LENGTH();

	vUsbEP0TxData();
	return TRUE;
}

/***********************************************************************/
//      bGet_configuration()
//      Description:
//          1. Send 1 bytes configuration value to host.
//      input: none
//      output: none
/***********************************************************************/
static BOOLEAN bGet_configuration(void)
{
	ZM_CBUS_FIFO_SIZE_REG = 0x1;
	ZM_EP0_DATA_REG = u8UsbConfigValue;

	// Restore CBus FIFO size to word size
	ZM_CBUS_FIFO_SIZE_REG = 0xf;

	eUsbCxFinishAction = ACT_DONE;

	return TRUE;
}

/***********************************************************************/
//      bSet_configuration()
//      Description:
//          1. Get 1 bytes configuration value from host.
//          2-1. if (value == 0) then device return to address state
//          2-2. if (value match descriptor table)
//                  then config success & Clear all EP toggle bit
//          2-3  else stall this command
//      input: none
//      output: TRUE or FALSE
/***********************************************************************/
static BOOLEAN bSet_configuration(void)
{
	void vUsbClrEPx(void);

	//zfUartSendStr((u8_t *) "bSet_configuration\n");

	if (mLOW_BYTE(mDEV_REQ_VALUE()) == 0) {
		u8UsbConfigValue = 0;
		//mUsbCfgClr();
		ZM_DEVICE_ADDRESS_REG &= ~BIT7;
	} else {
		if (mUsbHighSpeedST())	// First judge HS or FS??
		{
			if (mLOW_BYTE(mDEV_REQ_VALUE()) >
			    HS_CONFIGURATION_NUMBER)
				return FALSE;

			u8UsbConfigValue = mLOW_BYTE(mDEV_REQ_VALUE());
			vUsbFIFO_EPxCfg_HS();

			// Set into 512 byte mode */
			//ZM_SOC_USB_MODE_CTRL_REG |= BIT2;
			//    mUsbSOFMaskHS();
		} else {
			if (mLOW_BYTE(mDEV_REQ_VALUE()) >
			    FS_CONFIGURATION_NUMBER)
				return FALSE;

			u8UsbConfigValue = mLOW_BYTE(mDEV_REQ_VALUE());
			vUsbFIFO_EPxCfg_FS();

			// Set into 64 byte mode */
			//ZM_SOC_USB_MODE_CTRL_REG &= ~BIT2;
			//    mUsbSOFMaskFS();
		}
		//mUsbCfgSet();
		ZM_DEVICE_ADDRESS_REG |= BIT7;
		vUsbClrEPx();

		mUsbGlobIntEnable();
		mUSB_REG_OUT_INT_ENABLE();
		//zfUartSendStrAndHex("u8UsbConfigValue = ", u8UsbConfigValue);
	}

	eUsbCxFinishAction = ACT_DONE;
	return TRUE;
}

/***********************************************************************/
//      bGet_interface()
//      Description:
//          Getting interface
//      input: none
//      output: TRUE or FALSE
/***********************************************************************/
static BOOLEAN bGet_interface(void)
{
	if (mUsbCfgST() == 0)
		return FALSE;

	// If there exists many interfaces, Interface0,1,2,...N,
	// You must check & select the specific one
	switch (u8UsbConfigValue) {
#if (HS_CONFIGURATION_NUMBER >= 1)
		// Configuration 1
	case 1:
		if (mDEV_REQ_INDEX() > HS_C1_INTERFACE_NUMBER)
			return FALSE;
		break;
#endif
#if (HS_CONFIGURATION_NUMBER >= 2)
		// Configuration 2
	case 2:
		if (mDEV_REQ_INDEX2() > HS_C2_INTERFACE_NUMBER)
			return FALSE;
		break;
#endif
	default:
		return FALSE;
	}

	ZM_CBUS_FIFO_SIZE_REG = 0x3;
	ZM_EP0_DATA_REG = u8UsbInterfaceAlternateSetting;

	// Restore CBus FIFO size to word size
	ZM_CBUS_FIFO_SIZE_REG = 0xf;
	u16TxRxCounter = sizeof(u8UsbInterfaceAlternateSetting);
	eUsbCxFinishAction = ACT_DONE;
	return TRUE;
}

/***********************************************************************/
//      bSet_interface()
//      Description:
//          1-1. If (the device stays in Configured state)
//                  &(command match the alternate setting)
//                      then change the interface
//          1-2. else stall it
//      input: none
//      output: TRUE or FALSE
/***********************************************************************/
static BOOLEAN bSet_interface(void)
{
	void vUsbClrEPx(void);

	if (mUsbCfgST()) {

		// If there exists many interfaces, Interface0,1,2,...N,
		// You must check & select the specific one
		switch (mDEV_REQ_INDEX()) {
		case 0:	// Interface0

			if (mLOW_BYTE(mDEV_REQ_VALUE()) ==
			    mLOW_BYTE(u8ConfigDescriptorEX[mTABLE_IDX(12)]))
			{
				u8UsbInterfaceValue = (u8_t) mDEV_REQ_INDEX();
				u8UsbInterfaceAlternateSetting =
				    mLOW_BYTE(mDEV_REQ_VALUE());
				if (mUsbHighSpeedST())	// First judge HS or FS??
				{
					vUsbFIFO_EPxCfg_HS();

					// Set into 512 byte mode */
					//ZM_SOC_USB_MODE_CTRL_REG |= BIT2;
				} else {
					//    u8UsbConfigValue = mLOW_BYTE(mDEV_REQ_VALUE());
					vUsbFIFO_EPxCfg_FS();

					// Set into 64 byte mode */
					//ZM_SOC_USB_MODE_CTRL_REG &= ~BIT2;
				}
				vUsbClrEPx();
				eUsbCxFinishAction = ACT_DONE;

				mUsbGlobIntEnable();
				mUSB_REG_OUT_INT_ENABLE();
				return TRUE;
			}
			//    case 1: // Interface1
			//    case 2: // Interface2
			//    default:
			//        break;
		}
	}
	return FALSE;
}

/***********************************************************************/
//      bStandardCommand()
//      Description:
//          1. Process standard command.
//      input: none
//      output: TRUE or FALSE
/***********************************************************************/
static BOOLEAN bStandardCommand(void)
{
	switch (mDEV_REQ_REQ())	// by Standard Request codes
	{
	case USB_GET_STATUS:
		//zfUartSendStr((u8_t *) "USB_GET_STATUS\n");
		return (bGet_status());

	case USB_CLEAR_FEATURE:
		//zfUartSendStr((u8_t *) "USB_CLEAR_FEATURE\n");
		return (bClear_feature());

	case USB_SET_FEATURE:
		//zfUartSendStr((u8_t *) "USB_SET_FEATURE\n");
		return (bSet_feature());

	case USB_SET_ADDRESS:
		//zfUartSendStr((u8_t *) "USB_SET_ADDRESS\n");
		if (!bUsbEP0HaltSt)
			return (bSet_address());
		break;

	case USB_GET_DESCRIPTOR:
		//zfUartSendStr((u8_t *) "USB_GET_DESCRIPTOR\n");
		if (!bUsbEP0HaltSt)
			return (bGet_descriptor());
		break;

	case USB_GET_CONFIGURATION:
		//zfUartSendStr((u8_t *) "USB_GET_CONFIGURATION\n");
		if (!bUsbEP0HaltSt)
			return (bGet_configuration());
		break;

	case USB_SET_CONFIGURATION:
		//zfUartSendStr((u8_t *) "USB_SET_CONFIGURATION\n");
		if (!bUsbEP0HaltSt)
			return (bSet_configuration());
		break;

	case USB_GET_INTERFACE:
		//zfUartSendStr((u8_t *) "USB_GET_INTERFACE\n");
		if (!bUsbEP0HaltSt)
			return (bGet_interface());
		break;

	case USB_SET_INTERFACE:
		//zfUartSendStr((u8_t *) "USB_SET_INTERFACE\n");
		if (!bUsbEP0HaltSt)
			return (bSet_interface());
		break;
	}
	return FALSE;
}

/***********************************************************************/
//      vUsbEP0TxData()
//      Description:
//          1. Send data(max or short packet) to host.
//      input: none
//      output: none
/***********************************************************************/
void vUsbEP0TxData(void)
{
	u8_t u8temp;

	if (u16TxRxCounter < EP0MAXPACKETSIZE)
		u8temp = (u8_t) u16TxRxCounter;
	else
		u8temp = EP0MAXPACKETSIZE;

	u16TxRxCounter -= (u16_t) u8temp;

	{
		register u8_t u8count;
		u8_t remainder;
		//u32_t cx_status;

		//cx_status = ZM_CX_CONFIG_STATUS_REG;
		//zfUartSendStrAndHex((u8_t *)"1. cx_status = ", cx_status);

		for (u8count = 0; u8count < (u8temp / 4); u8count++) {
			u32_t ep0_data;
			u16_t ep0_low;
			u16_t ep0_high;

			// pack data into word size
			ep0_low = *pu8DescriptorEX++;
			ep0_high = *pu8DescriptorEX++;

			ep0_data = (ep0_high << 16) + ep0_low;

			//zfUartSendStrAndHex("ep0_data = ", ep0_data);
			// Write data into FIFO
			ZM_EP0_DATA_REG = ep0_data;
		}

		remainder = u8temp % 4;

		// Check whether there are data needed to be filled into the FIFO
		if (remainder == 3) {
			u32_t ep0_data;
			u16_t ep0_low;
			u16_t ep0_high;

			// pack data into word size
			ep0_low = *pu8DescriptorEX++;
			ep0_high = *pu8DescriptorEX++;

			ep0_data = (ep0_high << 16) + ep0_low;

			ZM_CBUS_FIFO_SIZE_REG = 0x7;
			ZM_EP0_DATA_REG = ep0_data;
		} else if (remainder == 2) {
			ZM_CBUS_FIFO_SIZE_REG = 0x3;
			ZM_EP0_DATA_REG = *pu8DescriptorEX;
		} else if (remainder == 1) {
			ZM_CBUS_FIFO_SIZE_REG = 0x01;
			ZM_EP0_DATA_REG = *pu8DescriptorEX;
		}
		// Restore CBus FIFO size to word size
		ZM_CBUS_FIFO_SIZE_REG = 0xf;
	}

	// end of the data stage
	if (u16TxRxCounter == 0) {
		eUsbCxCommand = CMD_VOID;
		eUsbCxFinishAction = ACT_DONE;
	}
}

/***********************************************************************/
//      vUsbEP0RxData()
//      Description:
//          1. Receive data(max or short packet) from host.
//      input: none
//      output: none
/***********************************************************************/
void vUsbEP0RxData(void)
{
	u8_t u8temp;

	if (u16TxRxCounter < EP0MAXPACKETSIZE)
		u8temp = (u8_t) u16TxRxCounter;
	else
		u8temp = EP0MAXPACKETSIZE;

	u16TxRxCounter -= (u16_t) u8temp;

	// Receive u8Temp bytes data
	{
		register u8_t u8count;
		u8_t *p = (u8_t *) pu8DescriptorEX;

		for (u8count = 0; u8count < ((u8temp + 3) >> 2); u8count++) {
			u32_t ep0_data;

			ep0_data = ZM_EP0_DATA_REG;

			//zfUartSendStrAndHex((u8_t*) "ep0_data=", ep0_data);
			//zfUartSendStrAndHex((u8_t*) "pu8DescriptorEX=", (u32_t) pu8DescriptorEX);

			//*pu8DescriptorEX++ = mGetByte0(ep0_data) + (mGetByte1(ep0_data) << 8);
			//*pu8DescriptorEX++ = mGetByte2(ep0_data) + (mGetByte3(ep0_data) << 8);

			*p++ = mGetByte0(ep0_data);
			*p++ = mGetByte1(ep0_data);
			*p++ = mGetByte2(ep0_data);
			*p++ = mGetByte3(ep0_data);
		}

		pu8DescriptorEX += (u8count << 1);
	}

	// end of the data stage
	if (u16TxRxCounter == 0) {
		eUsbCxCommand = CMD_VOID;
		eUsbCxFinishAction = ACT_DONE;
	}
}

void vUsbClrEPx(void)
{
	u8_t u8ep;

	// Clear All EPx Toggle Bit
	for (u8ep = 1; u8ep <= FUSB200_MAX_EP; u8ep++) {
		mUsbEPinRsTgSet(u8ep);
		mUsbEPinRsTgClr(u8ep);
	}
	for (u8ep = 1; u8ep <= FUSB200_MAX_EP; u8ep++) {
		mUsbEPoutRsTgSet(u8ep);
		mUsbEPoutRsTgClr(u8ep);
	}
}

/***********************************************************************/
//      vUsb_SetupDescriptor()
//      Description:
//          Setup the pointer to the descriptor in the SRAM and EEPROM
//
/***********************************************************************/
static void vUsb_SetupDescriptor(void)
{
	u8UsbDeviceDescriptor = (u16_t *) USB_DEVICE_DESC_ADDR;
	u8String00Descriptor = (u16_t *) USB_STRING00_DESC_ADDR;
	u8String10Descriptor = (u16_t *) USB_STRING10_DESC_ADDR;
	u8String20Descriptor = (u16_t *) USB_STRING20_DESC_ADDR;
	u8String30Descriptor = (u16_t *) USB_STRING30_DESC_ADDR;
}

/***********************************************************************/
//      vUsb_ep0setup()
//      Description:
//          1. Read 8-byte setup packet.
//          2. Decode command as Standard, Class, Vendor or NOT support command
//      input: none
//      output: none
/***********************************************************************/
void vUsb_ep0setup(void)
{
	register u8_t ii;
	volatile u32_t ep0_data;
	BOOLEAN bStandardCommand(void);
	BOOLEAN bClassCommand(void);
	void VendorCommand(void);

	if (UsbChirpFinish != TRUE) {
		UsbChirpFinish = TRUE;

		// Setup Descriptor pointer
		vUsb_SetupDescriptor();

		if (mUsbHighSpeedST())	// Judge HS or FS??
		{
			u8ConfigDescriptorEX = u8HSConfigDescriptor01;

			// copy Device Qualifierdescriptors (from rom to sram)
			for (ii = 1; ii < 4; ii++) {
				u8DeviceQualifierDescriptorEX[ii] =
				    u8UsbDeviceDescriptor[ii];
			}

			// Number of Other-speed Configurations
			// byte 9 Reserved for future use, must be zero
			u8DeviceQualifierDescriptorEX[4] =
			    (u8UsbDeviceDescriptor[8] >> 8) & 0x00ff;

			// copy Other Speed Descriptor
			for (ii = 1; ii < 23; ii++) {
				u8OtherSpeedConfigDescriptorEX[ii] =
				    u8FSConfigDescriptor01[ii];
			}
		} else {
			u8ConfigDescriptorEX = u8FSConfigDescriptor01;

			// copy Device Qualifierdescriptors (from rom to sram)
			for (ii = 1; ii < 4; ii++) {
				u8DeviceQualifierDescriptorEX[ii] =
				    u8UsbDeviceDescriptor[ii];
			}

			// Number of Other-speed Configurations
			// byte 9 Reserved for future use, must be zero
			u8DeviceQualifierDescriptorEX[4] =
			    (u8UsbDeviceDescriptor[8] >> 8) & 0x00ff;

			// copy Other Speed Descriptor
			for (ii = 1; ii < 23; ii++) {
				u8OtherSpeedConfigDescriptorEX[ii] =
				    u8HSConfigDescriptor01[ii];
			}
		}
		//Change bLength
		u8DeviceQualifierDescriptorEX[0] = 0x060A;
	}

	ep0_data = ZM_EP0_DATA_REG;

	ii = mGetByte0(ep0_data);

	ControlCmd.Direction = (u8_t) (ii & 0x80);	// xfer Direction(IN, OUT)
	ControlCmd.Type = (u8_t) (ii & 0x60);	// type(Standard, Class, Vendor)
	ControlCmd.Object = (u8_t) (ii & 0x03);	// Device, Interface, Endpoint

	ControlCmd.Request = mGetByte1(ep0_data);
	ControlCmd.Value = mGetByte2(ep0_data) + (mGetByte3(ep0_data) << 8);

	ep0_data = ZM_EP0_DATA_REG;
	ControlCmd.Index = mGetByte0(ep0_data) + (mGetByte1(ep0_data) << 8);
	ControlCmd.Length = mGetByte2(ep0_data) + (mGetByte3(ep0_data) << 8);

//  Command Decode
	if (mDEV_REQ_REQ_TYPE() == (cUSB_REQTYPE_STD << bmREQ_TYPE)) {	// standard command
		if (bStandardCommand() == FALSE) {
			eUsbCxFinishAction = ACT_STALL;
		}
	} else if (mDEV_REQ_REQ_TYPE() == (cUSB_REQTYPE_VENDOR << bmREQ_TYPE)) {	// vendor command
		VendorCommand();
	} else {
		// Invalid(bad) command, Return EP0_STALL flag
		eUsbCxFinishAction = ACT_STALL;
	}
}

void VendorCommand(void)
{
#define cUSB_REQ_DOWNLOAD          0x30
#define cUSB_REQ_DOWNLOAD_COMP     0x31
#define cUSB_REQ_BOOT              0x32
#define ZM_FIRMWARE_SPI_ADDR       0x114000	//SPI firmware base address

	void (*funcPtr) (void);

	switch (mDEV_REQ_REQ()) {
	case cUSB_REQ_DOWNLOAD:
		u16TxRxCounter = mDEV_REQ_LENGTH();
		pu8DescriptorEX = (u16_t *) (mDEV_REQ_VALUE() << 8);
		zfUartSendStr((u8_t *) "D");

		//zfUartSendStrAndHex((u8_t *) "u16TxRxCounter=", u16TxRxCounter);
		zfUartSendStrAndHex((u8_t *) "pu8DescriptorEX=",
				    (u32_t) pu8DescriptorEX);

		if ((((u32_t) pu8DescriptorEX) & 0x300000) == 0x200000) {
			//zfUartSendStr((u8_t *) "CMD_VOID");
			eUsbCxCommand = CMD_VOID;
		} else		//((pu8DescriptorEX &= 0x300000) == 0x100000)
		{
			//zfUartSendStr((u8_t *) "CMD_SET");
			eUsbCxCommand = CMD_SET_DESCRIPTOR;
		}
		break;

	case cUSB_REQ_DOWNLOAD_COMP:
		// u16FirmwareComplete = 1;
		if ((((u32_t) pu8DescriptorEX) & 0x300000) != 0x200000) {
			/* Set EP0 Done */
			ZM_CX_CONFIG_STATUS_REG = 0x01;

			/* Firmware download completed */
			zfUartSendStr((u8_t *) "G");
			/* goto firmware start address */
			funcPtr = (void *)(ZM_FIRMWARE_SPI_ADDR + 4);
			funcPtr();
			break;
		}
	}
}
