#include "dt_defs.h"
#include "usb_defs.h"
#include "usb_type.h"
//#include "usb_extr.h"
#include "desc.h"
#include "gv_extr.h"
#include "reg_defs.h"
#include "cmd_defs.h"
#include "uart_extr.h"
#include "api_extr.h"
#include "ap.h"

#if ZM_INT_USE_EP2 == 1
#include "desc.h"
void vUsb_Status_In(void);
#endif

void vUsbHandler(u8_t usb_interrupt_level1);

/* Declaration for global variable */
u16_t    *pu8DescriptorEX;
u16_t    u16TxRxCounter;
u16_t    *u8ConfigDescriptorEX;
u16_t    *TxFrameStart;
//u16_t    UsbTxFifoLen = 0, UsbTxFifoAdr = 0, WaitFreeBuf = 0;
u16_t    MaxPktSize;
u16_t    u8UsbConfigValue;
u16_t    u8UsbInterfaceValue;
u16_t    u8UsbInterfaceAlternateSetting;
//u16_t    u8UsbCommandFlag;
Action   eUsbCxFinishAction;
CommandType eUsbCxCommand;
u16_t       *u8UsbDeviceDescriptor;
u16_t       *u8String00Descriptor;
u16_t       *u8String10Descriptor;
u16_t       *u8String20Descriptor;
u16_t       *u8String30Descriptor;

BOOLEAN     UsbChirpFinish;
BOOLEAN     bUsbEP0HaltSt;

void zfWlanFwLoopBack(void);

void zfUsbIsr(void)
{
    u8_t usb_interrupt_level1;

    usb_interrupt_level1 = ZM_INTR_GROUP_REG;

    if(usb_interrupt_level1)
    {
//        zfUartSendStr((u8_t*)"B");
        vUsbHandler(usb_interrupt_level1);
    }

    if (zgIntrINQNum > 0)
        mUSB_STATUS_IN_INT_ENABLE();
}

void vUsb_Data_In(void)
{
    //zfUartSendStr((u8_t *)"vUsb_Data_In\r\n");
}

void vUsb_Reg_Out(void)
{
    u16_t usbfifolen;
    u16_t ii;
    volatile u32_t *regaddr = (volatile u32_t *) ZM_CMD_BUFFER;

    mUSB_REG_OUT_INT_DISABLE();

    usbfifolen = ZM_EP4_BYTE_COUNT_LOW_REG;

    if(usbfifolen % 4)
        usbfifolen = (usbfifolen >> 2) + 1;
    else
        usbfifolen = usbfifolen >> 2;

    for(ii = 0; ii < usbfifolen; ii++)
    {
        u32_t ep4_data = ZM_EP4_DATA_REG;
        *regaddr = ep4_data;
        regaddr++;
    }

    zfCmdHandler();
    mUSB_STATUS_IN_INT_ENABLE();
}

#if ZM_INT_USE_EP2 == 1
#define INT_IN_DESC_dataAddr   ZM_RSP_BUFFER + ZM_INT_USE_EP2_HEADER_SIZE
#endif

void vUsb_Status_In(void)
{
    u16_t count;
    u16_t remainder;
    volatile u32_t *regaddr;
    u16_t RegBufLen;

#if ZM_INT_USE_EP2 == 1

	if (zgIntrINAvailable)
	{
	    zgIntrINAvailable = 0;
	}
	else
	{
	    //zfUartSendStr((u8_t*)"RspNR");
		return;
	}

#endif

    mUSB_STATUS_IN_INT_DISABLE();

    regaddr = (volatile u32_t *) zfGetIntrINQHeadBuf();

    if (regaddr != NULL)
        RegBufLen = *(volatile u8_t*)(regaddr) + 4;
    else
        zfUartSendStrAndHex((u8_t *) "IntrQ empty, zgIntrINQNum=", zgIntrINQNum);

#if ZM_INT_USE_EP2 == 1
    /* fix: interrupt in use EP2(data in) replace EP3
     *         0 : normal use EP3
     *         1 : fix use EP2
     */

	/* boundary check */
	//if (RegBufLen > (128-INT_IN_DESC_REV))
	if ( RegBufLen > ZM_INTRQ_BUF_SIZE )
	{
	    zfUartSendStr((u8_t*)"ErLen");
	}

	for(count = 0; count < (RegBufLen >> 2); count++)
	{
		*(volatile u32_t*)(INT_IN_DESC_dataAddr + (count*4)) = *regaddr;
		regaddr++;
	}
	zgIntDesc->ctrl = (ZM_LS_BIT | ZM_FS_BIT);
	zgIntDesc->dataSize = RegBufLen + ZM_INT_USE_EP2_HEADER_SIZE;
	zgIntDesc->totalLen = RegBufLen + ZM_INT_USE_EP2_HEADER_SIZE;

	/* Put to UpQ */
    zfDmaPutPacket(&zgUpQ, zgIntDesc);

    /* Trigger PTA UP DMA */
    ZM_PTA_UP_DMA_TRIGGER_REG = 1;

//    zfUartSendStr((u8_t*)"I");

#else
    /* INT use EP3 */
    for(count = 0; count < (RegBufLen / 4); count++)
    {
    	ZM_EP3_DATA_REG = *regaddr;
    	regaddr++;
    }

    remainder = RegBufLen % 4;

    if (remainder)
    {
        switch(remainder)
        {
        case 3:
            ZM_CBUS_FIFO_SIZE_REG = 0x7;
            break;
        case 2:
            ZM_CBUS_FIFO_SIZE_REG = 0x3;
            break;
        case 1:
            ZM_CBUS_FIFO_SIZE_REG = 0x01;
            break;
        }

        ZM_EP3_DATA_REG = *regaddr;
    }

    // Restore CBus FIFO size to word size
    ZM_CBUS_FIFO_SIZE_REG = 0xf;

    mUSB_EP3_XFER_DONE();
#endif /* #if ZM_INT_USE_EP2 == 1 */
    mUSB_REG_OUT_INT_ENABLE();
}

#if 0
void ReadIntReg(U_16 int_mask)
{
    RegAddr = &rHMAC_INT_ACK;
    HMacIntSts[0] = *RegAddr;
    RegAddr += 2;
    HMacIntSts[1] = *RegAddr;
//    rHMAC_INT_MASK = 0;                     // Disable HMAC INT
//    *RegAddr = 0;                           // Disable HMAC INT
    *RegAddr = int_mask;
    RegAddr -= 2;
//    rHMAC_INT_ACK = HMacIntSts[0];          // Clear Int.
//    *RegAddr = HMacIntSts[0];               // Clear Int.
    mSET_REG1(rHMAC_INT_ACK, HMacIntSts[0]);// Clear Int.
}

void IsrHMac(void)
{
    ReadIntReg(0);

    #if fINDEPEND_REG_RW
    mUSB_REG_OUT_INT(DISABLE);
    #endif
    mUSB_STATUS_IN_INT(ENABLE);

}
#endif

void zfGenUsbInterrupt(u8_t rspLen, u8_t rspType, u16_t rspSeq, u8_t *body)
{
    u8_t len, type;
    u16_t i;
    u8_t *u8RspBuf;
    u16_t *u16RspBuf;

    u8RspBuf = (u8_t *)zfGetFreeIntrINQTailBuf();

    /* If there is not enough buffer */
    if ((u8RspBuf == NULL) || (zgIntrINQNum >= (ZM_INTRQ_NUM - 2)))
    {
        zfUartSendStr((u8_t *) "Not enough INQ Buf\r\n");
    //    zfUartSendStrAndHex((u8_t *) "zgIntrINQHead=", zgIntrINQHead);
    //    zfUartSendStrAndHex((u8_t *) "zgIntrINQHead=", zgIntrINQTail);
    //    zfUartSendStrAndHex((u8_t *) "zgIntrINQHead=", zgIntrINQNum);
        *(volatile u32_t*)(0x117704) += 1;
        return;
    }

    u16RspBuf = (u16_t *)u8RspBuf;

    *u8RspBuf = rspLen;
    *(u8RspBuf+1) = rspType;
    *(u16RspBuf+1) = rspSeq;

    //zfUartSendStrAndHex((u8_t*)"rspLen=", *u8RspBuf);
    //zfUartSendStrAndHex((u8_t*)"rspType=", *(u8RspBuf+1));

    for(i = 0; i < rspLen; i++)
    {
        *(volatile u8_t*)(u8RspBuf+4+i) = body[i];
    }

    mUSB_STATUS_IN_INT_ENABLE();
}

void zfGenerateBAFrame(u16_t seq, u32_t bitmap, u16_t cnt)
{
    u16_t buffer[30];
    u16_t *ptr = (u16_t *) buffer;

    /* Pack the BA information */
    *ptr++ = seq;
    *ptr++ = bitmap;
    *ptr++ = bitmap >> 16;
    *ptr++ = cnt;

    zfGenUsbInterrupt(8, 0xC4, 0x82, (u8_t *)buffer);
}

#if 0
void zfGenerateBAFailCntFrame(void)
{
    u8_t ii;
    u8_t len = 0;
    u16_t buffer[30];
    u16_t *ptr = (u16_t *) buffer;

    for (ii = 0; ii < sizeof(zgBARate)/sizeof(zgBARate[0]); ii++)
    {
        if (zgBARate[ii] != 0xffff)
        {
            *ptr++ = zgBAFailCnt[ii];
            *ptr++ = zgBARate[ii];

            zgBaErrorCount -= zgBAFailCnt[ii];
            len += 4;
        }

        zgBARate[ii] = 0xffff;
        zgBAFailCnt[ii] = 0;
    }


    if (len > 0)
    {
        zfGenUsbInterrupt(len, 0xC5, 0x80, (u8_t *)buffer);
    }
}
#endif

/***********************************************************************/
//      zfResetUSBFIFO()
//      Description:
//          1. Reset all the USB FIFO used for WLAN
//      input: none
//      output: none
/***********************************************************************/
void zfResetUSBFIFO(void)
{
    u32_t val;

    val = *(volatile u32_t*)(0x1c3500);
    val |= 0x20;

    *(volatile u32_t*)(0x1c3500) = val;

    /* Reset USB FIFO */
    *((volatile u32_t *) (0x1d4004)) = 0x05;
    *((volatile u32_t *) (0x1d4004)) = 0;
}

/***********************************************************************/
//      zfTurnOffPower()
//      Description:
//          1. Function to turn off ADDA/RF power, PLL
//      input: none
//      output: none
/***********************************************************************/
void zfTurnOffPower(void)
{
    *(volatile u32_t *) 0x1c581c = 0;
    *(volatile u32_t *) 0x1c582c = 0xa000a000;
    *(volatile u32_t *) 0x1d0104 = 0;
    *(volatile u32_t *) 0x1d0100 = 0xf;
    *(volatile u32_t *) 0x1d4000 = 0x40021;
    *(volatile u32_t *) 0x1d4004 = 0;
    *(volatile u32_t *) 0x1d4008 = 0x601; //0x601; // 0; modified
    *(volatile u32_t *) 0x1d4014 = 0x5163; //0x502b;
    *(volatile u32_t *) 0x1c5830 = 1;
    *(volatile u32_t *) 0x1c589c = 0;  // 7-0
    *(volatile u32_t *) 0x1c589c = 0;  // 15-8
    *(volatile u32_t *) 0x1c589c = 0;  // 23-16
    *(volatile u32_t *) 0x1c589c = 0;  // 31-
    *(volatile u32_t *) 0x1c589c = 0;  // 39-
    *(volatile u32_t *) 0x1c589c = 0;  // 47-
    *(volatile u32_t *) 0x1c589c = 0;  // 55-
    *(volatile u32_t *) 0x1c589c = 0xf8;  // 63-
    *(volatile u32_t *) 0x1c589c = 0x27; // 0x24;  // 71-   modified
    *(volatile u32_t *) 0x1c589c = 0xf9;  // 79-
    *(volatile u32_t *) 0x1c589c = 0x90;  // 87-
    *(volatile u32_t *) 0x1c589c = 0x04;  // 95-
    *(volatile u32_t *) 0x1c589c = 0x48;  // 103-
    *(volatile u32_t *) 0x1c589c = 0x19; // 0;  // 111-    modified
    *(volatile u32_t *) 0x1c589c = 0;  // 119-
    *(volatile u32_t *) 0x1c589c = 0;  // 127-
    *(volatile u32_t *) 0x1c589c = 0;  // 135-
    *(volatile u32_t *) 0x1c589c = 0;  // 143-
    *(volatile u32_t *) 0x1c589c = 0;  // 151-
    *(volatile u32_t *) 0x1c589c = 0x70;  // 159-
    *(volatile u32_t *) 0x1c589c = 0x0c;  // 167-
    *(volatile u32_t *) 0x1c589c = 0;  // 175-
    *(volatile u32_t *) 0x1c589c = 0;  // 183-176
    *(volatile u32_t *) 0x1c589c = 0;  // 191-184
    *(volatile u32_t *) 0x1c589c = 0;  // 199-
    *(volatile u32_t *) 0x1c589c = 0;  // 207-
    *(volatile u32_t *) 0x1c589c = 0;  // 215-
    *(volatile u32_t *) 0x1c589c = 0;  // 223-
    *(volatile u32_t *) 0x1c589c = 0;  // 231-
    *(volatile u32_t *) 0x1c58c4 = 0;  // 233-232
    *(volatile u32_t *) 0x1c5830 = 0;
//    *(volatile u32_t *) 0x1d4014 = 0x5028;
}

/***********************************************************************/
//      zfJumpToBootCode()
//      Description:
//          1. Function to return to boot code
//      input: none
//      output: none
/***********************************************************************/
void zfJumpToBootCode(void)
{
#define ZM_BOOTCODE_ADDR      0x00000008

    void (*funcPtr)(void);

    /* Jump to the boot code */
    funcPtr = (void*)(ZM_BOOTCODE_ADDR);
    funcPtr();

#undef ZM_BOOTCODE_ADDR
}

/***********************************************************************/
//      vUsbHandler()
//      Description:
//          1. Service all Usb events
//          2. ReEnable Usb interrupt.
//      input: none
//      output: none
/***********************************************************************/
void vUsbHandler(u8_t usb_interrupt_level1)
{
register u8_t usb_interrupt_level2;
//U_8 usb_interrupt_level2;
extern void vUsb_ep0setup(void);
extern void vUsb_ep0tx(void);
extern void vUsb_ep0rx(void);
extern void vUsb_ep0end(void);
extern void vUsb_ep0fail(void);
extern void vUsb_rst(void);
extern void vUsb_suspend(void);
extern void vUsb_resm(void);
extern void vUsb_sof(void);

    if (usb_interrupt_level1 & BIT5)            //Group Byte 5
    {
        vUsb_Data_In();
    }

    if (usb_interrupt_level1 & BIT4)
    {
        vUsb_Reg_Out();
    }

    if (usb_interrupt_level1 & BIT6)
    {
        vUsb_Status_In();
    }

    if (usb_interrupt_level1 & BIT0)            //Group Byte 0
    {
        usb_interrupt_level2 = ZM_INTR_SOURCE_0_REG;

        if (usb_interrupt_level2 & BIT1)
            vUsb_ep0tx();						// USB EP0 tx interrupt

        if (usb_interrupt_level2 & BIT2)
            vUsb_ep0rx();						// USB EP0 rx interrupt

        if (usb_interrupt_level2 & BIT7)
		{
            /* Clear the command abort interrupt */
            ZM_INTR_SOURCE_0_REG &= 0x7f;
		}

        if (usb_interrupt_level2 & BIT0)
        {
            vUsb_ep0setup();
        }
//TODO : What are these interrupts?
        else if (usb_interrupt_level2 & BIT3)
        {
            vUsb_ep0end();
        }
        if (usb_interrupt_level2 & BIT4)
            vUsb_ep0fail();

        if (eUsbCxFinishAction == ACT_STALL)
    	{
//            mUsbEP0StallSet();
            ZM_CX_CONFIG_STATUS_REG = 0x04;
    	}
    	else if (eUsbCxFinishAction == ACT_DONE)
    	{
//             mUsbEP0DoneSet();
            ZM_CX_CONFIG_STATUS_REG = 0x01;
    	}

        eUsbCxFinishAction = ACT_IDLE;
    }

    if (usb_interrupt_level1 & BIT7)            //Group Byte 7
    {
        usb_interrupt_level2 = ZM_INTR_SOURCE_7_REG;

        if (usb_interrupt_level2 & BIT7)
            vUsb_Data_Out0Byte();

        if (usb_interrupt_level2 & BIT6)
            vUsb_Data_In0Byte();

        if (usb_interrupt_level2 & BIT1)
            vUsb_rst();
        if (usb_interrupt_level2 & BIT2)
        {
            u32_t val;
            void (*funcPtr)(void);

            /* ACK USB suspend interrupt */
            vUsb_suspend();

            /* Reset USB FIFO */
            zfResetUSBFIFO();

            /* Turn off power */
            zfTurnOffPower();

            /* Set GO_TO_SUSPEND bit to USB main control register */
            ZM_MAIN_CTRL_REG = BIT3;
            //ZM_MAIN_CTRL_REG &= BIT3;

            // add by ygwei for work around USB PHY chirp sequence problem
            *((volatile u32_t *)0x10F100) = 0x12345678;

            /* Jump to boot code */
            zfJumpToBootCode();
        }
        if (usb_interrupt_level2 & BIT3)
            vUsb_resm();

    }
    //mSysUsbIntEnable();                     // ReEnable Usb interrupt.
}
