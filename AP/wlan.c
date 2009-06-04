/************************************************************************/
/*  Copyright (c) 2007  Atheros Communications, Inc.                    */
/*                                                                      */
/*  Module Name : wlan.c                                                */
/*                                                                      */
/*  Abstract                                                            */
/*      This module contains PCI handle functions.                      */
/*                                                                      */
/*  ROUTINES                                                            */
/*                                                                      */
/*      zfWlanIsr                                                       */
/*                                                                      */
/*  NOTES                                                               */
/*      None                                                            */
/*                                                                      */
/************************************************************************/
#include "sys_defs.h"
#include "dt_defs.h"
#include "desc.h"
#include "gv_extr.h"
#include "reg_defs.h"
#include "api_extr.h"
#include "ap.h"
#include "uart_extr.h"
#include "usb_defs.h"

void zfPauseUsbDma(void);

#if ZM_BOARD_TYPE == ZM_BOARD_TYPE_FB50

#define zm_hw_get_mt_mcs(buf, mt, mcs) {mt = buf[2] & 0x3; \
        mcs = (buf[3]>>2) & 0x3f;}

#define zm_hw_set_mt_mcs(buf, newMt, newMcs, sg) {buf[2] &= 0xfffc; \
        buf[3] &= 0x7e03; \
        buf[2] |= newMt; \
        buf[3] |= (newMcs<<2); \
		buf[3] |= ((sg & 0x1)<<15);}

#endif

#if ZM_BOARD_TYPE == ZM_BOARD_TYPE_MIMO2

#define zm_hw_get_mt_mcs(buf, mt, mcs) {mt = buf[2] & 0x3; \
        mcs = buf[3] & 0x3f;}

#define zm_hw_set_mt_mcs(buf, newMt, newMcs) {buf[2] &= 0xfffc; \
        buf[3] &= 0xffc0; \
        buf[2] |= newMt; \
        buf[3] |= newMcs;}

#endif

extern struct zsDmaDesc* zfExchangeDesc(struct zsDmaDesc* desc);

/************************************************************************/
/*                                                                      */
/*    FUNCTION DESCRIPTION                  zfWlanIsr                   */
/*      WLAN DMA handle.                                                */
/*                                                                      */
/*    ROUTINES CALLED                                                   */
/*      zfCmdHandler                                                    */
/*                                                                      */
/*    INPUTS                                                            */
/*      void                                                            */
/*                                                                      */
/*    OUTPUTS                                                           */
/*      void                                                            */
/*                                                                      */
/*    AUTHOR                                                            */
/*      Stephen Chen   Atheros Communications, Inc.             2005.10 */
/*                                                                      */
/************************************************************************/
extern void zfDumpPacket(struct zsDmaDesc* desc);

#define ZM_STATUS_TX_COMP       0x00
#define ZM_STATUS_RETRY_COMP    0x01
#define ZM_STATUS_TX_FAILED     0x02

/*    0 : CCK 1M */
/*    1 : CCK 2M */
/*    2 : CCK 5.5M */
/*    3 : CCK 11M */

/*    b : OFDM 6M */
/*    f : OFDM 9M */
/*    a : OFDM 12M */
/*    e : OFDM 18M */
/*    9 : OFDM 24M */
/*    d : OFDM 36M */
/*    8 : OFDM 48M */
/*    c : OFDM 54M */
#define ZM_NUM_OF_MT        4
#define ZM_NUM_OF_MCS       16

#define ZM_MT_CCK           0
#define ZM_MT_LOFDM         1
#define ZM_MT_HT            2
#define ZM_MT_DLOFDM        3

#define ZM_MCS_NONE         0xff

#define ZM_MCS_CCK_1M       0
#define ZM_MCS_CCK_2M       1
#define ZM_MCS_CCK_5M       2
#define ZM_MCS_CCK_11M      3

#define ZM_MCS_OFDM_6M      0x10b
#define ZM_MCS_OFDM_9M      0x10f
#define ZM_MCS_OFDM_12M     0x10a
#define ZM_MCS_OFDM_18M     0x10e
#define ZM_MCS_OFDM_24M     0x109
#define ZM_MCS_OFDM_36M     0x10d
#define ZM_MCS_OFDM_48M     0x108
#define ZM_MCS_OFDM_54M     0x10c

#define ZM_MCS_HT_MCS0      0x200
#define ZM_MCS_HT_MCS1      0x201
#define ZM_MCS_HT_MCS2      0x202
#define ZM_MCS_HT_MCS3      0x203
#define ZM_MCS_HT_MCS4      0x204
#define ZM_MCS_HT_MCS5      0x205
#define ZM_MCS_HT_MCS6      0x206
#define ZM_MCS_HT_MCS7      0x207
#define ZM_MCS_HT_MCS8      0x208
#define ZM_MCS_HT_MCS9      0x209
#define ZM_MCS_HT_MCS10     0x20a
#define ZM_MCS_HT_MCS11     0x20b
#define ZM_MCS_HT_MCS12     0x20c
#define ZM_MCS_HT_MCS13     0x20d
#define ZM_MCS_HT_MCS14     0x20e
#define ZM_MCS_HT_MCS15     0x20f

const u16_t zc2Ghz40RetryRateTable[ZM_NUM_OF_MT][ZM_NUM_OF_MCS] =
    {
        {
   /* CCK          1M,            2M,          5.5M,           11M, */
          ZM_MCS_NONE, ZM_MCS_CCK_1M, ZM_MCS_CCK_2M, ZM_MCS_CCK_5M,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE
        },
        {
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
   /* L-OFDM          48M,             24M,             12M,            6M, */
          ZM_MCS_OFDM_36M, ZM_MCS_OFDM_18M,  ZM_MCS_CCK_11M, ZM_MCS_CCK_5M,
   /* L-OFDM          54M,             36M,             18M,            9M */
          ZM_MCS_OFDM_48M, ZM_MCS_OFDM_24M, ZM_MCS_OFDM_12M, ZM_MCS_CCK_5M
        },
        {
    /* HT            MCS0,            MCS1,            MCS2,            MCS3, */
           ZM_MCS_CCK_11M,  ZM_MCS_HT_MCS0,  ZM_MCS_HT_MCS1,  ZM_MCS_HT_MCS2,
    /* HT            MCS4,            MCS5,            MCS6,            MCS7, */
           ZM_MCS_HT_MCS3,  ZM_MCS_HT_MCS4,  ZM_MCS_HT_MCS5,  ZM_MCS_HT_MCS6,
    /* HT            MCS8,            MCS9,           MCS10,           MCS11, */
           ZM_MCS_HT_MCS0,  ZM_MCS_HT_MCS2,  ZM_MCS_HT_MCS3,  ZM_MCS_HT_MCS4,
    /* HT           MCS12,           MCS13,           MCS14,           MCS15 */
           ZM_MCS_HT_MCS11, ZM_MCS_HT_MCS12, ZM_MCS_HT_MCS13, ZM_MCS_HT_MCS14
        },
        {
    /* DL-OFDM */
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE
        }
    };

const u16_t zc2Ghz20RetryRateTable[ZM_NUM_OF_MT][ZM_NUM_OF_MCS] =
    {
        {
   /* CCK          1M,            2M,          5.5M,           11M, */
          ZM_MCS_NONE, ZM_MCS_CCK_1M, ZM_MCS_CCK_2M, ZM_MCS_CCK_5M,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE
        },
        {
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
   /* L-OFDM          48M,             24M,             12M,            6M, */
          ZM_MCS_OFDM_36M, ZM_MCS_OFDM_18M,  ZM_MCS_CCK_11M, ZM_MCS_CCK_5M,
   /* L-OFDM          54M,             36M,             18M,            9M */
          ZM_MCS_OFDM_48M, ZM_MCS_OFDM_24M, ZM_MCS_OFDM_12M, ZM_MCS_CCK_5M
        },
        {
    /* HT            MCS0,            MCS1,            MCS2,            MCS3, */
           ZM_MCS_CCK_11M,  ZM_MCS_CCK_11M,  ZM_MCS_HT_MCS1,  ZM_MCS_HT_MCS2,
    /* HT            MCS4,            MCS5,            MCS6,            MCS7, */
           ZM_MCS_HT_MCS3,  ZM_MCS_HT_MCS4,  ZM_MCS_HT_MCS5,  ZM_MCS_HT_MCS6,
    /* HT            MCS8,            MCS9,           MCS10,           MCS11, */
           ZM_MCS_HT_MCS0,  ZM_MCS_HT_MCS2,  ZM_MCS_HT_MCS3,  ZM_MCS_HT_MCS4,
    /* HT           MCS12,           MCS13,           MCS14,           MCS15 */
           ZM_MCS_HT_MCS11, ZM_MCS_HT_MCS12, ZM_MCS_HT_MCS13, ZM_MCS_HT_MCS14
        },
        {
    /* DL-OFDM */
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE
        }
    };

const u16_t zc5Ghz40RetryRateTable[ZM_NUM_OF_MT][ZM_NUM_OF_MCS] =
    {
        {
   /* CCK          1M,            2M,          5.5M,           11M, */
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE
        },
        {
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
   /* L-OFDM          48M,             24M,             12M,            6M, */
          ZM_MCS_OFDM_36M, ZM_MCS_OFDM_18M, ZM_MCS_OFDM_9M,    ZM_MCS_NONE,
   /* L-OFDM          54M,             36M,             18M,            9M */
          ZM_MCS_OFDM_48M, ZM_MCS_OFDM_24M, ZM_MCS_OFDM_12M,ZM_MCS_OFDM_6M
        },
        {
    /* HT            MCS0,            MCS1,            MCS2,            MCS3, */
           ZM_MCS_OFDM_9M,  ZM_MCS_HT_MCS0,  ZM_MCS_HT_MCS1,  ZM_MCS_HT_MCS2,
    /* HT            MCS4,            MCS5,            MCS6,            MCS7, */
           ZM_MCS_HT_MCS3,  ZM_MCS_HT_MCS4,  ZM_MCS_HT_MCS5,  ZM_MCS_HT_MCS6,
    /* HT            MCS8,            MCS9,           MCS10,           MCS11, */
           ZM_MCS_HT_MCS0,  ZM_MCS_HT_MCS2,  ZM_MCS_HT_MCS3, ZM_MCS_HT_MCS4,
    /* HT           MCS12,           MCS13,           MCS14,           MCS15 */
          ZM_MCS_HT_MCS11, ZM_MCS_HT_MCS12, ZM_MCS_HT_MCS13, ZM_MCS_HT_MCS14
        },
        {
    /* DL-OFDM */
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE
        }
    };

const u16_t zc5Ghz20RetryRateTable[ZM_NUM_OF_MT][ZM_NUM_OF_MCS] =
    {
        {
   /* CCK          1M,            2M,          5.5M,           11M, */
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE
        },
        {
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
          ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
   /* L-OFDM          48M,             24M,             12M,            6M, */
          ZM_MCS_OFDM_36M, ZM_MCS_OFDM_18M, ZM_MCS_OFDM_9M,    ZM_MCS_NONE,
   /* L-OFDM          54M,             36M,             18M,            9M */
          ZM_MCS_OFDM_48M, ZM_MCS_OFDM_24M, ZM_MCS_OFDM_12M,ZM_MCS_OFDM_6M
        },
        {
    /* HT            MCS0,            MCS1,            MCS2,            MCS3, */
           ZM_MCS_OFDM_9M,  ZM_MCS_OFDM_9M,  ZM_MCS_HT_MCS1,  ZM_MCS_HT_MCS2,
    /* HT            MCS4,            MCS5,            MCS6,            MCS7, */
           ZM_MCS_HT_MCS3,  ZM_MCS_HT_MCS4,  ZM_MCS_HT_MCS5,  ZM_MCS_HT_MCS6,
    /* HT            MCS8,            MCS9,           MCS10,           MCS11, */
           ZM_MCS_HT_MCS0,  ZM_MCS_HT_MCS2,  ZM_MCS_HT_MCS3, ZM_MCS_HT_MCS4,
    /* HT           MCS12,           MCS13,           MCS14,           MCS15 */
          ZM_MCS_HT_MCS11, ZM_MCS_HT_MCS12, ZM_MCS_HT_MCS13, ZM_MCS_HT_MCS14
        },
        {
    /* DL-OFDM */
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE,
         ZM_MCS_NONE, ZM_MCS_NONE,   ZM_MCS_NONE,   ZM_MCS_NONE
        }
    };

void zfHandleCfgBcnInt(void)
{
	u32_t bcnCount;

#if ZM_CABQ_SUPPORT == 1
    if (zgStartVCCA == 1)
    {
        /* Start Virtual CCA for pending all frames in Queue 0 */
	    *(volatile u32_t*)(0x1c3b40) = 0xe8000;

        //zgStartVCCA = 0;

        /* Trigger Tx Queue 1 */
        zm_wl_dma_trigger_reg = 0x02;
    }
#endif

	bcnCount =  *((volatile u32_t*)(ZM_WL_REGISTER_BASE+0xd9c));
    zfGenUsbInterrupt(4, 0xC2, 0x80, (u8_t*)(&bcnCount));

    return;
}

void zfHandlePreTbttInt(void)
{
    zfGenUsbInterrupt(0, 0xC0, 0x80, 0);

    return;
}

void zfHandleAtimInt(void)
{
    zfGenUsbInterrupt(0, 0xC3, 0x80, 0);

    return;
}

#if ZM_BAR_AUTO_BA == 1
#define ZM_LED_FOR_MARVELL_BAR  0
#endif

void zfHandleRxInterrupt(void)
{
    struct zsDmaDesc* desc;

    /* If zgRxQ own bits changed, get packet from zgRxQ and put to  */
    /* zgUpQ. Repeat until all Rx packet were reclaimed             */
    while ((zgRxQ.head != zgRxQ.terminator) && ((zgRxQ.head->status
            & ZM_OWN_BITS_MASK) != ZM_OWN_BITS_HW))
    {
        desc = zfDmaGetPacket(&zgRxQ);

        if (zgBlockRx == 1)
        {
            zfDmaReclaimPacket(&zgRxQ, desc);
            continue;
        }

#if ZM_BAR_AUTO_BA == 1
		{
			u8_t* buf = (u8_t*)desc->dataAddr;
			if (buf[12] == 0x84)
			{
				/* Got a BAR */

				u8_t ia;

				if (zgBARGotState == 1)
				{
					/* state 2: send BA for the BAR, and recover ack func */

					if (zgBAAvailable)
					{
#if ZM_LED_FOR_MARVELL_BAR == 1
						/* LED  */
						*(volatile u32_t*)(0x1d0104) = 1;
#endif

						zgBADesc->lastAddr = zgBADesc;
						zgBADesc->ctrl = (ZM_LS_BIT | ZM_FS_BIT);

						zgBAAvailable = 0;


						/* Do Wlan sw reset, and put BA into TxQ0 head */
						{
                            u32_t val;
                            u32_t agg_wait_counter;
                            u32_t agg_density;
							u32_t bcnStartAddr;
							u32_t rctlReg, rcthReg;
							u32_t camModeReg;
							u32_t ackPowerReg;
							u32_t rtsctsTPCReg;
							u32_t rxBBReg;
                            u16_t ii;
							//u8_t  chState;

                            /* Save aggregation parameters */
                            agg_wait_counter = *(volatile u32_t*)(0x1c3b9c);
                            agg_density = *(volatile u32_t*)(0x1c3ba0);
							bcnStartAddr = *(volatile u32_t*)(0x1c3d98);
                            rctlReg = zm_cam_roll_call_tablel_reg;
                            rcthReg = zm_cam_roll_call_tableh_reg;
							camModeReg = zm_cam_mode_reg;
							ackPowerReg = *(volatile u32_t*)(0x1c3694);
							rtsctsTPCReg = *(volatile u32_t*)(0x1c3bb4);
							rxBBReg = *(volatile u32_t*)(0x1c5960); /* 0x1c8960 write only */

                            /* TX/RX must be stopped by now */
                            val = *(volatile u32_t*)(0x1c3500);
                            val |= 0x20;

                            *(volatile u32_t*)(0x1c3500) = val;

                            for (ii = 0; ii < 64;ii++)
                            {
                            }

                            /* Restore aggregation parameters */
                            *(volatile u32_t*)(0x1c3b9c) = agg_wait_counter;
                            *(volatile  u32_t*)(0x1c3ba0) = agg_density;
							*(volatile u32_t*)(0x1c3d98) = bcnStartAddr;
                           	zm_cam_roll_call_tablel_reg = rctlReg;
                            zm_cam_roll_call_tableh_reg = rcthReg;
							zm_cam_mode_reg = camModeReg;
							*(volatile u32_t*)(0x1c3694) = ackPowerReg;
							*(volatile u32_t*)(0x1c3bb4) = rtsctsTPCReg;
							*(volatile u32_t*)(0x1c8960) = rxBBReg;

							zgBADesc->nextAddr = zgTxQ[0].head;
							zgBADesc->status |= ZM_OWN_BITS_HW;

							zgTxQ[0].head = zgBADesc;

							/* rewrite TxQ0 start addr */
							zm_wl_txq0_dma_addr_reg = (u32_t)zgBADesc;
							zm_wl_dma_trigger_reg = ZM_TXQ0_TRIG_BIT;

                           /* Restart RxQ */
                            zfRecycleRxQ();

                            /* Trigger Rx DMA */
                            zm_wl_dma_trigger_reg = 0x100;

							*(volatile u32_t*)(0x117730) += 1;
						}

#if ZM_LED_FOR_MARVELL_BAR == 1
						/* LED  */
						*(volatile u32_t*)(0x1d0104) = 0;
#endif

    					//zfUartSendStr((u8_t*)"ba");
					}

					/* Recover Ack func */
					*(volatile u32_t*)(0x1c36f8) |= 0x1;
					zgBARGotState = 2;
				}
				else if (zgBARGotState == 0)
				{
					/* state 1: got a BAR, stop (From DS/ To DS) ACK            */
					/* prepare BA for BAR */

					/* BAR  | PLCP | FC | SA | DA | ID | SN | FCS | Rx status | */
					/* BAR     12  + 4  + 6  + 6  + 2  + 2  + 4   + 24          */

     				/* BA   | CTL  | FC | SA | DA | ID | SN | Bit map | FCS |   */
					/* BA      8   + 4  + 6  + 6  + 2  + 2  + 8        + 4      */
					//u8_t A1[6];
					//u8_t A2[6];
					//u16_t sn;

					zgBARGotState = 1;
					zgBARTimeOut = 0;

					/* Stop Ack ToDS=0 and FromDS=0 */
					*(volatile u32_t*)(0x1c36f8) &= ~((u32_t)0x1);


					/* start SN */
					*(volatile u8_t*)(ZM_BA_BUFFER + 26) = buf[30];
					*(volatile u8_t*)(ZM_BA_BUFFER + 27) = buf[31];

					for (ia=0; ia<6; ia++)
					{
						*(volatile u8_t*)(ZM_BA_BUFFER + ia + 12) = buf[ia + 22];
						*(volatile u8_t*)(ZM_BA_BUFFER + ia + 18) = buf[ia + 16];
					}

					*(volatile u8_t*)(ZM_BA_BUFFER + 8) = 0x94;		/* BA */
					*(volatile u8_t*)(ZM_BA_BUFFER + 9) = 0x0;
					*(volatile u8_t*)(ZM_BA_BUFFER + 24) = 0x05;	/* Ack policy */
				}
 //           	zfUartSendStr((u8_t*)" bar ");

 				if (zgBARGotState != 1)
				{
                	/* Discard BAR */
             		zfDmaReclaimPacket(&zgRxQ, desc);

            		/* Trigger WLAN RX DMA */
            		zm_wl_dma_trigger_reg = ZM_RXQ_TRIG_BIT;
					continue;
				}

			}/* end if handle BAR */
		}

#endif
        /* TODO : if len < 5+14+8 or len > zgRxMaxSize, discard rx frame */
        if (desc->totalLen < zgRxMaxSize)
        {
            zfDmaPutPacket(&zgUpQ, desc);

            /* Trigger PTA UP DMA */
            ZM_PTA_UP_DMA_TRIGGER_REG = 1;

            //zfUartSendStr((u8_t*)"r");

			//if (zgPacketDumpFlag&0x8)
            //    zfDumpPacket(desc);
        } /* if (desc->totalLen < zgRxMaxSize) */
        else
        {
            zfDmaReclaimPacket(&zgRxQ, desc);

            /* Trigger WLAN RX DMA */
            zm_wl_dma_trigger_reg = ZM_RXQ_TRIG_BIT;

            zfUartSendStr((u8_t*)" R ");
        }
    }
}

void zfGenerateTxCompEvent(u16_t* buf, u16_t status)
{
    u16_t info[8];

    info[0] = buf[6]; // macAddr
    info[1] = buf[7];
    info[2] = buf[8];
    info[3] = buf[2]; // phyCtrl
    info[4] = buf[3];
    info[5] = buf[1]; // macCtrl
    info[6] = status;
    info[7] = 0;      /* dummy for 4 byte alignment */

    zfGenUsbInterrupt(16, 0xC1, 0x81, (u8_t *)info);
}

void zfGenerateBaEvent(void)
{
    u16_t info[8];

    info[0] = zgRCBaMacAddr[0]; // macAddr
    info[1] = zgRCBaMacAddr[1];
    info[2] = zgRCBaMacAddr[2];
    info[3] = zgRCBaPhyCtrl[0]; // phyCtrl
    info[4] = zgRCBaPhyCtrl[1];
    info[5] = zgRCBaMacCtrl ;   // macCtrl
    info[6] = zgRCBaMPDUCnt;
    info[7] = zgRCBaErrorCnt;

    zfGenUsbInterrupt(16, 0xC5, 0x80, (u8_t *)info);
}


#if 0
//u8_t genBAFrame = 0;
//u8_t offsetBitMap;
//u16_t BAStartSeq;
//u32_t BABitmap;
//u16_t MPDUCnt;

void zfBAFailCntIntr(u16_t* buf)
{

    u16_t ii;
    u16_t mt, mcs, sg;
    u16_t rate;

    zgBaErrorCount++;

    zm_hw_get_mt_mcs(buf, mt, mcs);
    sg = (u16_t)(buf[3]>>15 & 0x1);

    if (sg == 1)
    {
        rate = (u16_t)(0x8000 | mcs);
    }
    else
    {
        rate = mcs;
    }

    /* Get Tx Rate */
    for (ii = 0; ii < sizeof(zgBARate)/sizeof(zgBARate[0]); ii++)
    {
        if (zgBARate[ii] == rate)
        {
            break;
        }
        else
        {
            if (zgBARate[ii] == 0xffff)
            {
                zgBARate[ii] = rate;
                break;
            }
        }
    }

    zgBAFailCnt[ii]++;

    if (zgBaStsCurrRate != rate && zgBaStsCurrRate != -1)
    {
        zgBaStsCurrRate = rate;
        zgGenBAIntr = 1;
    }
}
#endif

/*
 * Update AMPDU tx info for ba event to Host rate control.
 * If aggregation and grand, then update tx info for ba indicate event
 * if detect aggregation push bit, send BA info to host, and release grand.
 */
void zfUpdateTxAggInfo(struct zsDmaDesc* desc)
{
	if (zfCheckAGG(desc))
	{
        zgRCBaMPDUCnt++;

        if (zgRCBaGrand)
	    {
		    u16_t *buf = (u16_t*)desc->dataAddr;

            zgRCBaMacAddr[0] = buf[6];
            zgRCBaMacAddr[1] = buf[7];
            zgRCBaMacAddr[2] = buf[8];
		    zgRCBaPhyCtrl[0] = buf[2];
		    zgRCBaPhyCtrl[1] = buf[3];
		    zgRCBaMacCtrl    = buf[1];

		    /* We have got the tx information of the first packet in one AMPDU */
		    zgRCBaGrand = 0;
	    }

		if (zfCheckAGP(desc))
		{
			zfGenerateBaEvent();

			/* reset count */
			zgRCBaMPDUCnt = 0;
            zgRCBaErrorCnt = 0;
			/* release grand for update tx information */
			zgRCBaGrand = 1;
		}
	}
}

/* Set AGP of the last packet in RTQ */
void zfSetRTQAGP(void)
{
    if (zgRTQ.head)
    {
        u16_t* buf;
        buf = (u16_t*)zgRTQ.terminator->dataAddr;
        /* AGP : MAC ctrl Bit_14 */
        buf[1] |= 0x4000;
    }
}

#define ZM_AGGREGATION_BAFAIL_RETRY_NUM  2
void zfBAFailHandler(struct zsDmaQueue* q, u16_t q_num, struct zsDmaDesc* desc)
{
    u16_t* buf;

    //zfPutTraceBuffer((u32_t)desc | 0x20000000);
    buf = (u16_t*)desc->dataAddr;

    /* Turn on retry bit */
    buf[4] |= 0x0800;

#if 1 /* Do BA Fail handle */
    /* Increase zgBAFailCnt counter */
    zgTally.BAFailCnt++;
//    zgBaErrorCount++;

    if(zgDontRetransmit)
    {
        //Reclaim this packet
        zfDmaReclaimPacket(&zgDnQ, desc);
        //zfPutTraceBuffer((u32_t)desc | 0x08000000);

        /* Trigger PTA DN DMA */
        ZM_PTA_DN_DMA_TRIGGER_REG = 1;
    }
    else
    {

#if  ZM_AGGREGATION_RECLAIM != 0
        if ((desc->status & 0xf00) == (ZM_AGGREGATION_BAFAIL_RETRY_NUM<<8) )
        {
#if ZM_AGGREGATION_RECLAIM == 1
            //Reclaim this packet
            zfDmaReclaimPacket(&zgDnQ, desc);
            //zfPutTraceBuffer((u32_t)desc | 0x08000000);

            /* Trigger PTA DN DMA */
            ZM_PTA_DN_DMA_TRIGGER_REG = 1;

#elif ZM_AGGREGATION_RECLAIM == 2
            /* Change control setting to single packet */

            //Clear TxFail|BaFail control bit
            desc->ctrl &= 0xfffc;

            //Set OWN bits
            desc->status |= 0x1;

            /* Clear AGG bit & protection bits & AGG push bit */
            buf[1] &= 0xbfdc;
            //*(volatile u32_t*)(0x1c3b28) = 0x33333;

            /* Turn off retry bit */
            buf[4] &= 0xf7ff;

            /* Put to TxQ */
            zfDmaPutPacket(q, desc);

//			zfUartSendStrAndHex((u8_t*)"give agg = ", (u32_t)desc);

            //zfPutTraceBuffer((u32_t)desc | 0x02000000);

            /* Trigger WLAN TX DMA */
            zm_wl_dma_trigger_reg = (1<<q_num);
#endif
        }
        else
        {
            u8_t retryCnt = (u8_t)(desc->status >> 8);
            retryCnt++;
            desc->status = (u16_t)(retryCnt << 8);
#endif /* ZM_AGGREGATION_RECLAIM != 0 */

            /* Count BAFail by rate */
//            if (retryCnt == 1)
//			{
//                zfBAFailCntIntr(buf);
//			}

            //Clear TxFail|BaFail control bit
            desc->ctrl &= 0xfffc;

            /* pending into RTQ */
            zfQuePutPacket(&zgRTQ, desc);

            /* softap */
            /* last one in an AMPDU */
            if (zfCheckAGP(desc))
            {
                zgRTQTUCount++;
            }

#if ZM_AGGREGATION_RECLAIM != 0
        }
#endif
    }/* end of if(zgDontRetransmit) */
#else /* reclaim it, do nothing for BA Fail */
    zfDmaReclaimPacket(&zgDnQ, desc);
    //zfPutTraceBuffer((u32_t)desc | 0x08000000);

    /* Trigger PTA DN DMA */
    ZM_PTA_DN_DMA_TRIGGER_REG = 1;
#endif /* End of Do BA Fail handle */

//    zfUartSendStrAndHex((u8_t*)"BA seq=", buf[15]);
}


void zfHandleTxCompInt(void)
{
    u16_t* buf;
    u16_t ii;
    u16_t mt, mcs, newMt, newMcs, sg;
    struct zsDmaQueue* q;
    struct zsDmaDesc* desc;
    u16_t shiftCnt;
    u32_t dmaState;
    u32_t ctlState;

#if ZM_CABQ_SUPPORT == 1
    u16_t qNum;
#endif

    for (ii=0; ii<=4; ii++)
        {
            q = (struct zsDmaQueue*)((u32_t)zgTxQ
                    + (ii*sizeof(struct zsDmaQueue)));
#if ZM_CABQ_SUPPORT == 1
        qNum = ii;
#endif

            //zfUartSendStrAndHex((u8_t*)"q=", (u32_t)q);
            /* If zgTxQ own bits changed, get packet from zgTxQ nad recilam */
            /* to zgDnQ. Repeat until all Tx completed packet were reclaimed. */

            while ((q->head != q->terminator)
                    && ((q->head->status & ZM_OWN_BITS_MASK)
                    != ZM_OWN_BITS_HW))
            {
//zfUartSendStrAndHex((u8_t *) "Q=", ii);

#if ZM_BAR_AUTO_BA == 1
				if (q->head->dataAddr == ZM_BA_BUFFER)
				{
					desc = zfDmaGetPacket(q);
					desc = zfExchangeDesc(desc);

					*(volatile u32_t*)(0x11770c) = (u32_t)desc;

					zgBADesc = desc;
					zgBAAvailable = 1;
					zgBARGotState = 0;
					//zfUartSendStrAndHex((u8_t*)"BADesc = ", (u32_t)zgBADesc);
					continue;
				}
#endif


                //if q=txq and desc->ctrl = txfail
				/* Tx retry fail (hw retry max set to 0)                               */
				/*   1. if first tx retry fail ==> do retransmit by the same rate.     */
				/*   2. else if get new tx rate and not ZM_MCS_NONE ==> do retransmit. */
				/*   3. else reclaim it.                                               */

                if ((q->head->ctrl & 0x1) != 0)
                {
					/* Bit15 of status : flag for simHw retry */
//                    u8_t simHwRetryCnt = (u8_t)(q->head->status >> 15);
                    u8_t simHwRetryCnt = (q->head->status >> 15);

                    buf = (u16_t*)q->head->dataAddr;
                    zm_hw_get_mt_mcs(buf, mt, mcs);
//                    sg = (u16_t)((buf[3]>>15) & 0x1);
                    sg = (buf[3]>>15) & 0x1;

                    if (zgEnableFwRetry == 1)
                    {
					    if ( simHwRetryCnt != 0 )
					    {
						    /* clear simHwRetry flag */
						    q->head->status &= 0x7fff;
					    }
					    else
					    {
						    /* Turn on simHwRetry flag */
						    q->head->status |= 0x8000;
					    }
                    }

                    if (mcs < 16)
                    {
                        /* get new rate for retransmit */
                        if (simHwRetryCnt != 0 || zgEnableFwRetry == 0)
                        {
                            //if ((buf[1] & 0x20) == 0) //AGG bit
                            {
                                if (zg5GhzFreq == 0)
                                {
                                    //2.4GHz
									if (buf[2] & 0x10)
									{
										//40M
										newMt = zc2Ghz40RetryRateTable[mt][mcs] >> 8;
                                    	newMcs = zc2Ghz40RetryRateTable[mt][mcs] & 0xff;
										if (sg == 1)
										{
											if (mcs != 15)
											{
											    newMcs = mcs;
											    sg = 0;
											}
											else
											{
												//newMcs = 14;
												//sg = 1;
											}
										}
									}
									else
									{
										//20M
										newMt = zc2Ghz20RetryRateTable[mt][mcs] >> 8;
										newMcs = zc2Ghz20RetryRateTable[mt][mcs] & 0xff;
									}
                                }
                                else
                                {
                                    //5GHz
									if (buf[2] & 0x10)
									{
										//40M
										newMt = zc5Ghz40RetryRateTable[mt][mcs] >> 8;
                                    	newMcs = zc5Ghz40RetryRateTable[mt][mcs] & 0xff;
										if (sg == 1)
										{
											newMcs = mcs;
											sg = 0;
										}
									}
									else
									{
										//20M
										newMt = zc5Ghz20RetryRateTable[mt][mcs] >> 8;
										newMcs = zc5Ghz20RetryRateTable[mt][mcs] & 0xff;
									}

                                }
                            }
                            //else
                            //{
                                /* use the same rate do retransmit A-MPDU as MPDU */
                            //    newMt = mt;
                            //    newMcs = mcs;

                                /* Clear AGG bit & protection bits */
                            //    buf[1] &= 0xffdc;
                                //*(volatile u32_t*)(0x1c3b28) = 0x33333;
                            //}

                            if ((newMt != 2) && ((buf[1] & 0x20) != 0))
                            {
                                /* Clear AGG bit & protection bits */
                                buf[1] &= 0xffdc;
							}
                        }
                        else
                        {
							/* use the same rate do retransmit => sim HwRetry */
							newMt = mt;
							newMcs = mcs;
						}
                    }
                    else
                    {
						/* Error condition */
                        newMcs = ZM_MCS_NONE;
                    }

                    //zfUartSendStrAndHex((u8_t*)"O_", mt+(mcs<<16));
                    //zfUartSendStrAndHex((u8_t*)"N_", newMt+(newMcs<<16));

					/* 1=>no retransmit */
					if (zgDontRetransmit)
					{
						newMcs = ZM_MCS_NONE;
					}

                    if (newMcs == ZM_MCS_NONE)
                    {
                        /* 1. Removed from TxQ */
                        desc = zfDmaGetPacket(q);
						desc = zfExchangeDesc(desc);
                        //zfPutTraceBuffer((u32_t)desc | 0x04000000);

                        /* 2. Generate retry interrupt */
                        buf = (u16_t*)desc->dataAddr;
                        //if ((buf[4] & 0x0800) == 0x0800)
                        if( (buf[4] & 0x48) == 0x8 )
                        {   // Prevent accumulating the null data tx failed
                            zfGenerateTxCompEvent(buf, ZM_STATUS_TX_FAILED);
                        }

                        /* 3. Put to DnQ */
                        zfDmaReclaimPacket(&zgDnQ, desc);
                        /* Trigger PTA DN DMA */
                        ZM_PTA_DN_DMA_TRIGGER_REG = 1;
                        //zfPutTraceBuffer((u32_t)desc | 0x08000000);

#if ZM_BB_RESET == 1
                        //if ((buf[1] & 0x20) != 0)
                        {
                            zgGenBBResetCnt++;
                            zgRTSRetryCnt++;

                            if (zgRTSRetryCnt == 5)
                            {
                                /* Generate USB Interrupt to tell driver to reset BB */
                                zfGenUsbInterrupt(0, 0xC9, 0x80, NULL);
                            }
                        }
#endif

                        //zfUartSendStr((u8_t*)"F");
                    }
                    else
                    {
                        /* Retransmit */
                        //1. Setup new MT and MCS
                        zm_hw_set_mt_mcs(buf, newMt, newMcs, sg);

                        //2. Set mac header retry bit
                        buf[4] |= 0x0800;

                        //3. Clear TxFail|BaFail control bit
                        q->head->ctrl &= 0xfffc;

                        //4. Set OWN bits
                        q->head->status |= 0x1;

                        //5. Set CWM to 20Mhz when NewMT is CCK or OFDM
                        if (newMt < 2)
                            buf[2] &= 0xffe7;

                        //zfUartSendStr((u8_t*)"Y");
                    }

					/* 5. Restart DMA */
                    //set dma start address
					/* check MAC DMA status */
					//if ( (((*(volatile u32_t*)(0x1c3d30) >> 12) & 0xf) == 0xA) &&
					//     (((*(volatile u32_t*)(0x1c3d3c) >> 12) & 0xf) == 0x8) )
                    dmaState = (*(volatile u32_t*)(0x1c3d30) >> 12);
                    ctlState = (*(volatile u32_t*)(0x1c3d3c) >> 12);
                    shiftCnt = ii;

                    while (shiftCnt != 0)
                    {
                        dmaState = dmaState >> 4;
                        ctlState = ctlState >> 4;
                        shiftCnt--;
                    }

                    if (((dmaState & 0xf) == 0xA) &&  ((ctlState & 0xf) == 0x8))
					{
                    	(*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_TXQ0_DMA_ADDR_OFFSET+(ii*8))))
       				        	= ((u32_t)q->head) | 1;
					}
					else
					{
						/* debug counter for retry fail handle but not match with Hw status */
						*(volatile u32_t*)(0x117760) += 1;
                    	(*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_TXQ0_DMA_ADDR_OFFSET+(ii*8))))
       				        	= ((u32_t)q->head);
					}
                    zm_wl_dma_trigger_reg = (1<<ii);
                    //zfPutTraceBuffer((u32_t)q->head | 0x0f000000);
				//	zfUartSendStrAndHex((u8_t*)"d-H = ", (u32_t)q->head);


                } //if ((zgTxQ->head->ctrl & 0x1) == 1)

#if 1 /* BA fail handle block */
                /* BA Fail */
                else if ((q->head->ctrl & 0x2) != 0)
                {
                    /* Get from TxQ */
                    desc = zfDmaGetPacket(q);
                    desc = zfExchangeDesc(desc);

                    /* last one in an AMPDU */
                    if (zfCheckAGP(desc))
                    {
                        zgPendingTUCount--;
						zfPauseUsbDma();
                    }
                    zgRCBaErrorCnt++;
					zfUpdateTxAggInfo(desc);

                    zfBAFailHandler(q, ii, desc);

                } //else if ((q->head->ctrl & 0x2) != 0)
#endif /* BA fail handle block */
				else /* Tx Completed */
                {
					//if (*(volatile u32_t*)(0x1c3b28) != 0)
				    //{
				    //    *(volatile u32_t*)(0x1c3b28) = 0 ;
				    //}

#if ZM_BB_RESET == 1
                    zgRTSRetryCnt = 0;
#endif

                    desc = zfDmaGetPacket(q);
					desc = zfExchangeDesc(desc);
                    //zfPutTraceBuffer((u32_t)desc | 0x04000000);

                    /* 2. Generate retry interrupt */
                    buf = (u16_t*)desc->dataAddr;
#if 0
                    /* Check whether this is an aggregation frame */
                    if (buf[1] & 0x20)
                    {
                        if (genBAFrame == 0)
                        {
                            BAStartSeq = (buf[15] >> 4);
                            genBAFrame = 1;
                            offsetBitMap = 0;
                            BABitmap = 0;
                            MPDUCnt = 0;
                        }

                        /* If this is not a BA fail frame */
                        if ((desc->ctrl & 0x2) == 0)
                        {
                            BABitmap |= (1 << offsetBitMap);
                        }
                        else
                        {
                            /* Increase zgBAFailCnt counter */
                            zgTally.BAFailCnt++;
                        }

                        offsetBitMap++;
                        MPDUCnt++;

                        /* Check whether push bit is on */
                        if (buf[1] & 0x4000)
                        {
                        //    zfUartSendStrAndHex((u8_t *) "seq=", BAStartSeq);    Mark by ygwei
                        //    zfUartSendStrAndHex((u8_t *) "bitmap=", BABitmap);
                        //    zfUartSendStrAndHex((u8_t *) "len=", MPDUCnt);

                            /* Generate a BA frame to upper layer */
                            zfGenerateBAFrame(BAStartSeq, BABitmap, MPDUCnt);

                            /* Set genBAFrame zero */
                            genBAFrame = 0;
                        }
                    }
                    else
#endif
                    if (((buf[4] & 0x0800) == 0x0800) && ((buf[1] & 0x20) == 0) && ((buf[4] & 0x48) == 0x8) )
                    {   // Prevent accumulating the null data rtray count !
                        zfGenerateTxCompEvent(buf, ZM_STATUS_RETRY_COMP);
                    }
                    else if ((buf[1] & 0x8000) != 0)
                    {
						/* this is a probe packet */
                        zfGenerateTxCompEvent(buf, ZM_STATUS_TX_COMP);
                    }
					else if (!zfCheckAGG(desc))
					{
						/* single packet */
                        zfGenerateTxCompEvent(buf, ZM_STATUS_TX_COMP);
					}

					/* softap */
				    zfUpdateTxAggInfo(desc);

                    /* Last one in an AMPDU is completed,  */
                    /* setup BA Fail packets as completed TU in RTQ */
                    if (zfCheckAGG(desc) && zfCheckAGP(desc))
                    {
                        /* complete one TU */
                        zgPendingTUCount--;
						zfPauseUsbDma();

                        /* check if any BA Fail packets in RTQ waiting */
						if (zgRTQ.head && (!zfCheckAGP(zgRTQ.terminator)))
                        {
                            /* SetUp Complete TU : Set the last one packet of RTS with AGP */
                            zgRTQTUCount++;
                            zfSetRTQAGP();
                        }
                    }

                    zfDmaReclaimPacket(&zgDnQ, desc);

                    /* Trigger PTA DN DMA */
                    ZM_PTA_DN_DMA_TRIGGER_REG = 1;

                    //zfPutTraceBuffer((u32_t)desc | 0x08000000);

                    //zfUartSendStr((u8_t*)"t");

                    //zfUartSendStrAndHex((u8_t*)" desc=", (u32_t)desc);
                } /* if (getDescFlag == 1) */

#if ZM_CABQ_SUPPORT == 1
                if (qNum == 1)
                {
                    //zfUartSendStrAndHex((u8_t*) "status=", q->head->status);

                    /* Check whether there is any pending frames in Queue 1 */

                    if (zgStartVCCA && (q->head == q->terminator))
                    {
                        //zfPutChar('E');

                        /* Disable Virtual CCA */
                        *(volatile u32_t*)(0x1c3b40) = 0x0000;
                        zgStartVCCA = 0;
                    }
                    //else
                    //{
                    //    zfPutChar('N');
                    //}
                }
#endif


            } /* while ((zgTxQ[ii].head != zgTxQ[ii].terminator)... */
        } /* for (ii=0; ii<3 ; ii++) */
}


void zfWlanIsr(void)
{
    u32_t intr;

    /* Read interrupt */
    intr = zm_wl_int_ctrl_reg;
    /* clear interrupt flag */
    zm_wl_int_ctrl_reg = intr;

    if ((intr & ZM_INT_CFG_BCN_BIT) != 0)
    {
        if (zgBlockRx == 1)
            return;

        zfHandleCfgBcnInt();
    }

    /* PreTBTT interrupt */
    if ((intr & ZM_INT_PRETBTT_BIT) != 0)
    {
        if (zgBlockRx == 1)
            return;

        zfHandlePreTbttInt();
    }

    /* ATIM interrupt */
    if ((intr & ZM_INT_ATIM_BIT) != 0)
    {
        zfUartSendStr((u8_t*)" ATIM ");
        zfHandleAtimInt();
    }



    if ((intr & (ZM_INT_TXC_BIT|ZM_INT_RETRY_FAIL_BIT)) != 0)
    {
        zfHandleTxCompInt();
    }

    /* Rx interrupt */
    if ((intr & ZM_INT_RXC_BIT) != 0)
    {
        zfHandleRxInterrupt();
    }
}

//! \brief Schedule transmission packets
//! \author Stephen Chen
//! \date 2007.12
void zfTxSchedulerTask(void)
{
    //If agPendingTUCount < 2
        //If agRtQTUCount > 0, get a TU from RtQ and put to TxQ, agPendingTUCount++
        //If agDnQTUCount > 0, get a TU from DnQ and put to TxQ, agPendingTUCount++

    struct zsDmaDesc* desc;

    /* first priority : RTQ */
#if 0
//	if (zgPendingTUCount<2)
//    {
//        if (zgRTQTUCount>0)
//        {
#endif
    while ((zgPendingTUCount<2) && (zgRTQTUCount>0))
    {
		    //zfUartSendStr((u8_t*)".");

            if (zgRTQ.head)
            {
                zgRTQTUCount--;
                zgPendingTUCount++;
				zfPauseUsbDma();

                while(!zfCheckAGP(zgRTQ.head))
                {
                    desc = zfQueGetPacket(&zgRTQ);

                    //Clear TxFail|BaFail control bit
                    desc->ctrl &= 0xfffc;

                    //Set OWN bits
                    desc->status |= 0x1;

                    /* Put to TxQ */
					zfDmaPutPacket(&zgTxQ[0], desc);

                    //zfUartSendStrAndHex((u8_t*)"dp = ", (u32_t)desc);
                    //zfUartSendStr((u8_t*)"t");

                    /* Trigger WLAN TX DMA */
                    zm_wl_dma_trigger_reg = 1;
                }
                /* Last one in TU */
                desc = zfQueGetPacket(&zgRTQ);

                //Clear TxFail|BaFail control bit
                desc->ctrl &= 0xfffc;

                //Set OWN bits
                desc->status |= 0x1;

                /* Put to TxQ */
				zfDmaPutPacket(&zgTxQ[0], desc);

                //zfUartSendStrAndHex((u8_t*)"dp = ", (u32_t)desc);
                //zfUartSendStr((u8_t*)"R");

                /* Trigger WLAN TX DMA */
                zm_wl_dma_trigger_reg = 1;
            }/* if (zgRTQ.head) */
      } /* while ((zgPendingTUCount<2) && (zgRTQTUCount>0)) */
#if 0
//        }/* if (zgRTQTUCount>0) */
//    }/* if (zgPendingTUCount<2) */
#endif

    /* second priority : DnQ */
    if (zgPendingTUCount<2)
    {
        if (zgDnQTUCount>0)
        {
            zgDnQTUCount--;
            zgPendingTUCount++;

            zfSendTUFromDnQToTxQ(0);

            //zfUartSendStr((u8_t*)"D");
			zfPauseUsbDma();
        }/* if (zgDnQTUCount>0) */
    }/* if (zgPendingTUCount<2) */
}


void zfPauseUsbDma(void)
{
	if (zgPendingTUCount >= 2)
	{
		if ((ZM_SOC_USB_MODE_CTRL_REG & BIT0) != 0)
		{
			ZM_SOC_USB_MODE_CTRL_REG &= ~(BIT0);
		}
	}
	else
	{
		if ((ZM_SOC_USB_MODE_CTRL_REG & BIT0) == 0)
		{
			ZM_SOC_USB_MODE_CTRL_REG |= BIT0;
		}
	}
}
