/************************************************************************/
/*  Copyright (c) 2000-2005  ZyDAS Technology Corporation               */
/*                                                                      */
/*  Module Name : pci.c                                                 */
/*                                                                      */
/*  Abstract                                                            */
/*      This module contains PCI handle functions.                      */
/*                                                                      */
/*  ROUTINES                                                            */
/*                                                                      */
/*      zfHostIfIsr                                                        */
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
#include "uart_extr.h"

/************************************************************************/
/*                                                                      */
/*    FUNCTION DESCRIPTION                  zfPciIsr                    */
/*      PTA interrupt and command handle.                               */
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
/*      Stephen Chen   ZyDAS Communication Corporation        2005.10   */
/*                                                                      */
/************************************************************************/
extern void zfDumpPacket(struct zsDmaDesc* desc);
extern u32_t zfLengthConfirm(struct zsDmaDesc* desc);

void zfHostIfIsr(void)
{
    struct zsDmaDesc* desc;
    u16_t intr;

    intr = ZM_PTA_INT_FLAG_REG;

    if ((intr & (ZM_PTA_DOWN_INT_BIT|ZM_PTA_UP_INT_BIT))!=0)
    {
        /* zgDnQ own bits changed */
        while ((zgDnQ.head != zgDnQ.terminator) && ((zgDnQ.head->status
                & ZM_OWN_BITS_MASK) != ZM_OWN_BITS_HW))
        {
#if ZM_FM_LOOPBACK == 1
            desc = zfDmaGetPacket(&zgDnQ);

            zfDmaPutPacket(&zgUpQ, desc);
            ZM_PTA_UP_DMA_TRIGGER_REG = 1;

			//zfUartSendStr((u8_t*)"lb ");
            //zfDmaReclaimPacket(&zgDnQ, desc);
            //ZM_PTA_DN_DMA_TRIGGER_REG = 1;
#else
            desc = zfDmaGetPacket(&zgDnQ);
#if OTUS_NO_SHRINK == 1
            zfPutTraceBuffer((u32_t)desc | 0x01000000);
#endif
#if 1

            if (zgBlockTx == 1)
            {
                 zfDmaReclaimPacket(&zgDnQ, desc);
            }
            else
            {
                u16_t* buf;
                u16_t txqNum;
                u8_t ch;

                buf = (u16_t*)desc->dataAddr;

                /* if AGG bit = 1 */
                if ((buf[1] & 0x20) != 0)
                {
                    zgTally.AggTxCnt++;
                }

				/* Length inconsistent: totalLen of desc and len of ctrlsetting */
#if 1
				if ( zfLengthConfirm(desc) )
				{
            		zfDmaReclaimPacket(&zgDnQ, desc);
            		ZM_PTA_DN_DMA_TRIGGER_REG = 1;
					continue;
				}
#else
				zfLengthConfirm(desc);
#endif


				{
                    /* TxQ number */
                    //txqNum = (buf[2] >> 13);
                    //if (txqNum > 4)
                    {
                        txqNum = 0;
                    }

                    zfDmaPutPacket(&zgTxQ[txqNum], desc);
                    /* Trigger WLAN TX DMA */
                    zm_wl_dma_trigger_reg = (1 << txqNum);
#if OTUS_NO_SHRINK == 1
                    zfPutTraceBuffer((u32_t)desc | 0x02000000);
#endif
                    ch = '0' + txqNum;
                    //zfPutChar(ch);
				}
            }
#else
            //zfDumpPacket(desc);
            zfDmaPutPacket(&zgTxQ[0], desc);
            /* Trigger WLAN TX DMA */
            zm_wl_dma_trigger_reg = ZM_TXQ0_TRIG_BIT;
            //zfUartSendStr((u8_t*)"d");
#endif

			//zfDumpPacket(desc);
#endif
        }

        /* zgUpQ own bits changed */
        while ((zgUpQ.head != zgUpQ.terminator) && ((zgUpQ.head->status
                & ZM_OWN_BITS_MASK) != ZM_OWN_BITS_HW))
        {
#if ZM_FM_LOOPBACK == 1
            desc = zfDmaGetPacket(&zgUpQ);

            zfDmaReclaimPacket(&zgDnQ, desc);
            ZM_PTA_DN_DMA_TRIGGER_REG = 1;
#else


            desc = zfDmaGetPacket(&zgUpQ);

#if ZM_INT_USE_EP2 == 1
		    if (desc->dataAddr == ZM_RSP_BUFFER)
			{
			    /* Rsp */
				zgIntrINAvailable = 1;
				zgIntDesc = desc;
				//zfUartSendStrAndHex((u8_t*)"IntDesc = ", (u32_t)zgIntDesc);
			}
			else
			{
#endif /* ZM_INT_USE_EP2 == 1 */

            zfDmaReclaimPacket(&zgRxQ, desc);
            /* Trigger WLAN RX DMA */
            zm_wl_dma_trigger_reg = ZM_RXQ_TRIG_BIT;
            //zfPutChar('u');

#if ZM_INT_USE_EP2 == 1
			}/* INT use EP2 */
#endif /* ZM_INT_USE_EP2 == 1 */

#endif /* ZM_FM_LOOPBACK == 1 */
        }/* end of while */
    }

    if ((intr & ZM_PTA_CMD_INT) == ZM_PTA_CMD_INT)
    {
        zfCmdHandler();
    }
}
