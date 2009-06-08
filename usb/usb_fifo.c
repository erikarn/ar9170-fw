#include "usb_defs.h"
#include "usb_type.h"
#include "usb_pre.h"
#include "usb_extr.h"
#include "usb_std.h"
#include "uart_extr.h"

void mUsbEPMap(u8_t EPn, u8_t MAP)
{
	u8_t *reg = (u8_t *) (ZM_FUSB_BASE + 0x30 + (EPn - 1));
	*reg = MAP;
}

void mUsbFIFOMap(u8_t FIFOn, u8_t MAP)
{
	u8_t *reg = (u8_t *) (ZM_FUSB_BASE + 0x80 + FIFOn);
	*reg = MAP;
}

void mUsbFIFOConfig(u8_t FIFOn, u8_t cfg)
{
	u8_t *reg = (u8_t *) (ZM_FUSB_BASE + 0x90 + FIFOn);
	*reg = cfg;
}

void mUsbEPMxPtSzHigh(u8_t EPn, u8_t dir, u16_t size)
{
	u8_t *reg = (u8_t *) (ZM_FUSB_BASE + ZM_EP_IN_MAX_SIZE_HIGH_OFFSET + ((((dir) * 0x20) + EPn) << 1));
	*reg = (size >> 8) & 0xf;
}

void mUsbEPMxPtSzLow(u8_t EPn, u8_t dir, u16_t size)
{
	u8_t *reg = (u8_t *) (ZM_FUSB_BASE + ZM_EP_IN_MAX_SIZE_LOW_OFFSET + ((((dir) * 0x20) + EPn) << 1));
	*reg = (size & 0xff);
}

void mUsbEPinHighBandSet(u8_t EPn, u8_t dir, u16_t size)
{
	u8_t *reg = (u8_t *) (ZM_FUSB_BASE + ZM_EP_IN_MAX_SIZE_HIGH_OFFSET + (EPn << 1));

	*reg &= ~(BIT6 | BIT5);

	switch (dir) {
	case DIRECTION_IN:
		*reg |= (((u8_t) ((size) >> 11) + 1) << 5);
		break;
	case DIRECTION_OUT:
	default:
		break;
	}
}

/////////////////////////////////////////////////////
//      vUsbFIFO_EPxCfg_HS(void)
//      Description:
//          1. Configure the FIFO and EPx map
//      input: none
//      output: none
/////////////////////////////////////////////////////
void vUsbFIFO_EPxCfg_HS(void)
{
	int i;

	//zfUartSendStr((u8_t *) "vUsbFIFO_EPxCfg_HS\n");

	switch (u8UsbConfigValue) {
#if (HS_CONFIGURATION_NUMBER >= 1)
		// Configuration 0X01
	case 0X01:
		switch (u8UsbInterfaceValue) {
#if (HS_C1_INTERFACE_NUMBER >= 1)
			// Interface 0
		case 0:
			switch (u8UsbInterfaceAlternateSetting) {
#if (HS_C1_I0_ALT_NUMBER >= 1)
				// AlternateSetting 0
			case 0:
#if (HS_C1_I0_A0_EP_NUMBER >= 1)
				//EP0X01
				mUsbEPMap(EP1, HS_C1_I0_A0_EP1_MAP);
				mUsbFIFOMap(HS_C1_I0_A0_EP1_FIFO_START, HS_C1_I0_A0_EP1_FIFO_MAP);
				mUsbFIFOConfig(HS_C1_I0_A0_EP1_FIFO_START, HS_C1_I0_A0_EP1_FIFO_CONFIG);

				for (i = HS_C1_I0_A0_EP1_FIFO_START + 1;
				     i < HS_C1_I0_A0_EP1_FIFO_START + HS_C1_I0_A0_EP1_FIFO_NO; i++) {
					mUsbFIFOConfig(i, (HS_C1_I0_A0_EP1_FIFO_CONFIG & (~BIT7)));
				}

				mUsbEPMxPtSzHigh(EP1, HS_C1_I0_A0_EP1_DIRECTION, (HS_C1_I0_A0_EP1_MAX_PACKET & 0x7ff));
				mUsbEPMxPtSzLow(EP1, HS_C1_I0_A0_EP1_DIRECTION, (HS_C1_I0_A0_EP1_MAX_PACKET & 0x7ff));
				mUsbEPinHighBandSet(EP1, HS_C1_I0_A0_EP1_DIRECTION, HS_C1_I0_A0_EP1_MAX_PACKET);

				//zfUartSendStr((u8_t *) "Initialize EP1\n");
#endif
#if (HS_C1_I0_A0_EP_NUMBER >= 2)
				//EP0X02
				mUsbEPMap(EP2, HS_C1_I0_A0_EP2_MAP);
				mUsbFIFOMap(HS_C1_I0_A0_EP2_FIFO_START, HS_C1_I0_A0_EP2_FIFO_MAP);
				mUsbFIFOConfig(HS_C1_I0_A0_EP2_FIFO_START, HS_C1_I0_A0_EP2_FIFO_CONFIG);

				for (i = HS_C1_I0_A0_EP2_FIFO_START + 1;
				     i < HS_C1_I0_A0_EP2_FIFO_START + HS_C1_I0_A0_EP2_FIFO_NO; i++) {
					mUsbFIFOConfig(i, (HS_C1_I0_A0_EP2_FIFO_CONFIG & (~BIT7)));
				}

				mUsbEPMxPtSzHigh(EP2, HS_C1_I0_A0_EP2_DIRECTION, (HS_C1_I0_A0_EP2_MAX_PACKET & 0x7ff));
				mUsbEPMxPtSzLow(EP2, HS_C1_I0_A0_EP2_DIRECTION, (HS_C1_I0_A0_EP2_MAX_PACKET & 0x7ff));
				mUsbEPinHighBandSet(EP2, HS_C1_I0_A0_EP2_DIRECTION, HS_C1_I0_A0_EP2_MAX_PACKET);

				//zfUartSendStr((u8_t *) "Initialize EP2\n");
#endif
#if (HS_C1_I0_A0_EP_NUMBER >= 3)
				//EP0X03
				mUsbEPMap(EP3, HS_C1_I0_A0_EP3_MAP);
				mUsbFIFOMap(HS_C1_I0_A0_EP3_FIFO_START, HS_C1_I0_A0_EP3_FIFO_MAP);
				mUsbFIFOConfig(HS_C1_I0_A0_EP3_FIFO_START, HS_C1_I0_A0_EP3_FIFO_CONFIG);

				for (i = HS_C1_I0_A0_EP3_FIFO_START + 1;
				     i < HS_C1_I0_A0_EP3_FIFO_START + HS_C1_I0_A0_EP3_FIFO_NO; i++) {
					mUsbFIFOConfig(i, (HS_C1_I0_A0_EP3_FIFO_CONFIG & (~BIT7)));
				}

				mUsbEPMxPtSzHigh(EP3, HS_C1_I0_A0_EP3_DIRECTION, (HS_C1_I0_A0_EP3_MAX_PACKET & 0x7ff));
				mUsbEPMxPtSzLow(EP3, HS_C1_I0_A0_EP3_DIRECTION, (HS_C1_I0_A0_EP3_MAX_PACKET & 0x7ff));
				mUsbEPinHighBandSet(EP3, HS_C1_I0_A0_EP3_DIRECTION, HS_C1_I0_A0_EP3_MAX_PACKET);

				//zfUartSendStr((u8_t *) "Initialize EP3\n");
#endif
#if (HS_C1_I0_A0_EP_NUMBER >= 4) || fFLASH_DISK
				//EP0X04
				mUsbEPMap(EP4, HS_C1_I0_A0_EP4_MAP);
				mUsbFIFOMap(HS_C1_I0_A0_EP4_FIFO_START, HS_C1_I0_A0_EP4_FIFO_MAP);
				mUsbFIFOConfig(HS_C1_I0_A0_EP4_FIFO_START, HS_C1_I0_A0_EP4_FIFO_CONFIG);

				for (i = HS_C1_I0_A0_EP4_FIFO_START + 1;
				     i < HS_C1_I0_A0_EP4_FIFO_START + HS_C1_I0_A0_EP4_FIFO_NO; i++) {
					mUsbFIFOConfig(i, (HS_C1_I0_A0_EP4_FIFO_CONFIG & (~BIT7)));
				}

				mUsbEPMxPtSzHigh(EP4, HS_C1_I0_A0_EP4_DIRECTION, (HS_C1_I0_A0_EP4_MAX_PACKET & 0x7ff));
				mUsbEPMxPtSzLow(EP4, HS_C1_I0_A0_EP4_DIRECTION, (HS_C1_I0_A0_EP4_MAX_PACKET & 0x7ff));
				mUsbEPinHighBandSet(EP4, HS_C1_I0_A0_EP4_DIRECTION, HS_C1_I0_A0_EP4_MAX_PACKET);

				//zfUartSendStr((u8_t *) "Initialize EP4\n");
#endif
#if (HS_C1_I0_A0_EP_NUMBER >= 5)
				//EP0X05
				mUsbEPMap(EP5, HS_C1_I0_A0_EP5_MAP);
				mUsbFIFOMap(HS_C1_I0_A0_EP5_FIFO_START, HS_C1_I0_A0_EP5_FIFO_MAP);
				mUsbFIFOConfig(HS_C1_I0_A0_EP5_FIFO_START, HS_C1_I0_A0_EP5_FIFO_CONFIG);

				for (i = HS_C1_I0_A0_EP5_FIFO_START + 1;
				     i < HS_C1_I0_A0_EP5_FIFO_START + HS_C1_I0_A0_EP5_FIFO_NO; i++) {
					mUsbFIFOConfig(i, (HS_C1_I0_A0_EP5_FIFO_CONFIG & (~BIT7)));
				}

				mUsbEPMxPtSzHigh(EP5, HS_C1_I0_A0_EP5_DIRECTION, (HS_C1_I0_A0_EP5_MAX_PACKET & 0x7ff));
				mUsbEPMxPtSzLow(EP5, HS_C1_I0_A0_EP5_DIRECTION, (HS_C1_I0_A0_EP5_MAX_PACKET & 0x7ff));
				mUsbEPinHighBandSet(EP5, HS_C1_I0_A0_EP5_DIRECTION, HS_C1_I0_A0_EP5_MAX_PACKET);
#endif
				break;
#endif
			default:
				break;
			}
			break;
#endif
		default:
			break;
		}
		break;
#endif
	default:
		break;
	}
	//mCHECK_STACK();
}

void vUsbFIFO_EPxCfg_FS(void)
{
	int i;

	//zfUartSendStr((u8_t *) "vUsbFIFO_EPxCfg_FS\n");

	switch (u8UsbConfigValue) {
#if (FS_CONFIGURATION_NUMBER >= 1)
		// Configuration 0X01
	case 0X01:
		switch (u8UsbInterfaceValue) {
#if (FS_C1_INTERFACE_NUMBER >= 1)
			// Interface 0
		case 0:
			switch (u8UsbInterfaceAlternateSetting) {
#if (FS_C1_I0_ALT_NUMBER >= 1)
				// AlternateSetting 0
			case 0:
#if (FS_C1_I0_A0_EP_NUMBER >= 1)
				//EP0X01
				mUsbEPMap(EP1, FS_C1_I0_A0_EP1_MAP);
				mUsbFIFOMap(FS_C1_I0_A0_EP1_FIFO_START, FS_C1_I0_A0_EP1_FIFO_MAP);
				mUsbFIFOConfig(FS_C1_I0_A0_EP1_FIFO_START, FS_C1_I0_A0_EP1_FIFO_CONFIG);

				for (i = FS_C1_I0_A0_EP1_FIFO_START + 1;
				     i < FS_C1_I0_A0_EP1_FIFO_START + FS_C1_I0_A0_EP1_FIFO_NO; i++) {
					mUsbFIFOConfig(i, (FS_C1_I0_A0_EP1_FIFO_CONFIG & (~BIT7)));
				}

				mUsbEPMxPtSzHigh(EP1, FS_C1_I0_A0_EP1_DIRECTION, (FS_C1_I0_A0_EP1_MAX_PACKET & 0x7ff));
				mUsbEPMxPtSzLow(EP1, FS_C1_I0_A0_EP1_DIRECTION, (FS_C1_I0_A0_EP1_MAX_PACKET & 0x7ff));
				//``.JWEI 2003/04/29
				mUsbEPinHighBandSet(EP1, FS_C1_I0_A0_EP1_DIRECTION, FS_C1_I0_A0_EP1_MAX_PACKET);

				//zfUartSendStr((u8_t *) "Initialize EP1\n");
#endif
#if (FS_C1_I0_A0_EP_NUMBER >= 2)
				//EP0X02
				mUsbEPMap(EP2, FS_C1_I0_A0_EP2_MAP);
				mUsbFIFOMap(FS_C1_I0_A0_EP2_FIFO_START, FS_C1_I0_A0_EP2_FIFO_MAP);
				mUsbFIFOConfig(FS_C1_I0_A0_EP2_FIFO_START, FS_C1_I0_A0_EP2_FIFO_CONFIG);

				for (i = FS_C1_I0_A0_EP2_FIFO_START + 1;
				     i < FS_C1_I0_A0_EP2_FIFO_START + FS_C1_I0_A0_EP2_FIFO_NO; i++) {
					mUsbFIFOConfig(i, (FS_C1_I0_A0_EP2_FIFO_CONFIG & (~BIT7)));
				}

				mUsbEPMxPtSzHigh(EP2, FS_C1_I0_A0_EP2_DIRECTION, (FS_C1_I0_A0_EP2_MAX_PACKET & 0x7ff));
				mUsbEPMxPtSzLow(EP2, FS_C1_I0_A0_EP2_DIRECTION, (FS_C1_I0_A0_EP2_MAX_PACKET & 0x7ff));
				mUsbEPinHighBandSet(EP2, FS_C1_I0_A0_EP2_DIRECTION, FS_C1_I0_A0_EP2_MAX_PACKET);

				//zfUartSendStr((u8_t *) "Initialize EP2\n");
#endif
#if (FS_C1_I0_A0_EP_NUMBER >= 3)
				//EP0X03
				mUsbEPMap(EP3, FS_C1_I0_A0_EP3_MAP);
				mUsbFIFOMap(FS_C1_I0_A0_EP3_FIFO_START, FS_C1_I0_A0_EP3_FIFO_MAP);
				mUsbFIFOConfig(FS_C1_I0_A0_EP3_FIFO_START, FS_C1_I0_A0_EP3_FIFO_CONFIG);

				for (i = FS_C1_I0_A0_EP3_FIFO_START + 1;
				     i < FS_C1_I0_A0_EP3_FIFO_START + FS_C1_I0_A0_EP3_FIFO_NO; i++) {
					mUsbFIFOConfig(i, (FS_C1_I0_A0_EP3_FIFO_CONFIG & (~BIT7)));
				}

				mUsbEPMxPtSzHigh(EP3, FS_C1_I0_A0_EP3_DIRECTION, (FS_C1_I0_A0_EP3_MAX_PACKET & 0x7ff));
				mUsbEPMxPtSzLow(EP3, FS_C1_I0_A0_EP3_DIRECTION, (FS_C1_I0_A0_EP3_MAX_PACKET & 0x7ff));
				mUsbEPinHighBandSet(EP3, FS_C1_I0_A0_EP3_DIRECTION, FS_C1_I0_A0_EP3_MAX_PACKET);

				//zfUartSendStr((u8_t *) "Initialize EP3\n");
#endif
#if (FS_C1_I0_A0_EP_NUMBER >= 4) || fFLASH_DISK
				//EP0X04
				mUsbEPMap(EP4, FS_C1_I0_A0_EP4_MAP);
				mUsbFIFOMap(FS_C1_I0_A0_EP4_FIFO_START, FS_C1_I0_A0_EP4_FIFO_MAP);
				mUsbFIFOConfig(FS_C1_I0_A0_EP4_FIFO_START, FS_C1_I0_A0_EP4_FIFO_CONFIG);

				for (i = FS_C1_I0_A0_EP4_FIFO_START + 1;
				     i < FS_C1_I0_A0_EP4_FIFO_START + FS_C1_I0_A0_EP4_FIFO_NO; i++) {
					mUsbFIFOConfig(i, (FS_C1_I0_A0_EP4_FIFO_CONFIG & (~BIT7)));
				}

				mUsbEPMxPtSzHigh(EP4, FS_C1_I0_A0_EP4_DIRECTION, (FS_C1_I0_A0_EP4_MAX_PACKET & 0x7ff));
				mUsbEPMxPtSzLow(EP4, FS_C1_I0_A0_EP4_DIRECTION, (FS_C1_I0_A0_EP4_MAX_PACKET & 0x7ff));
				mUsbEPinHighBandSet(EP4, FS_C1_I0_A0_EP4_DIRECTION, FS_C1_I0_A0_EP4_MAX_PACKET);

				//zfUartSendStr((u8_t *) "Initialize EP4\n");
#endif
#if (FS_C1_I0_A0_EP_NUMBER >= 5)
				//EP0X05
				mUsbEPMap(EP5, FS_C1_I0_A0_EP5_MAP);
				mUsbFIFOMap(FS_C1_I0_A0_EP5_FIFO_START, FS_C1_I0_A0_EP5_FIFO_MAP);
				mUsbFIFOConfig(FS_C1_I0_A0_EP5_FIFO_START, FS_C1_I0_A0_EP5_FIFO_CONFIG);

				for (i = FS_C1_I0_A0_EP5_FIFO_START + 1;
				     i < FS_C1_I0_A0_EP5_FIFO_START + FS_C1_I0_A0_EP5_FIFO_NO; i++) {
					mUsbFIFOConfig(i, (FS_C1_I0_A0_EP5_FIFO_CONFIG & (~BIT7)));
				}

				mUsbEPMxPtSzHigh(EP5, FS_C1_I0_A0_EP5_DIRECTION, (FS_C1_I0_A0_EP5_MAX_PACKET & 0x7ff));
				mUsbEPMxPtSzLow(EP5, FS_C1_I0_A0_EP5_DIRECTION, (FS_C1_I0_A0_EP5_MAX_PACKET & 0x7ff));
				mUsbEPinHighBandSet(EP5, FS_C1_I0_A0_EP5_DIRECTION, FS_C1_I0_A0_EP5_MAX_PACKET);
#endif
				break;
#endif
			default:
				break;
			}
			break;
#endif
		default:
			break;
		}
		break;
#endif
	default:
		break;
	}
	//mCHECK_STACK();
}
