/************************************************************************/
/*  Copyright (c) 2007  Atheros Communications, Inc.                    */
/*                                                                      */
/*  Module Name : pci.c                                                 */
/*                                                                      */
/*  Abstract                                                            */
/*      This module contains host interface handle functions.           */
/*                                                                      */
/*  ROUTINES                                                            */
/*                                                                      */
/*      zfHostIfIsr                                                     */
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


/* This is a workaround routine. */
void zfSetDnQAGP_w()
{
    u16_t* buf;
    struct zsDmaDesc *descPtr, *preDescPtr;

    descPtr = zgDnQ.head;

    while ( (descPtr != zgDnQ.terminator) && (descPtr != zgDnQCurrentDescPtr))
    {
        preDescPtr = descPtr;
        descPtr = descPtr->lastAddr->nextAddr;
    }

    *(volatile u32_t*)(0x117708) += 1;

    buf = (u16_t*)preDescPtr->dataAddr;
    /* AGP : MAC ctrl Bit_14 */
    buf[1] |= 0x4000;
}
/************************************************************************/
/*                                                                      */
/*    FUNCTION DESCRIPTION                  zfHostDlTask                */
/*      Process Down Link DMA descriptor chain.                         */
/*                                                                      */
/*    ROUTINES CALLED                                                   */
/*      zfHostIfIsr                                                     */
/*                                                                      */
/*    INPUTS                                                            */
/*      void                                                            */
/*                                                                      */
/*    OUTPUTS                                                           */
/*      void                                                            */
/*                                                                      */
/*    AUTHOR                                                            */
/*      Stephen Chen   Atheros Communications, Inc.             2007.12 */
/*                                                                      */
/************************************************************************/
//! \brief Process Down Link DMA descriptor chain.
//! \author Wade Sung
//! \date 2007.12
void zfHostDlTask(void)
{
    /* zgDnQ own bits changed */
    while ( (zgDnQCurrentDescPtr != zgDnQ.terminator) &&
           ((zgDnQCurrentDescPtr->status & ZM_OWN_BITS_MASK) != ZM_OWN_BITS_HW) )
    {
        /* Length inconsistent check part1 : avoid lose the packet with AGP */
		if ( zfLengthConfirm(zgDnQCurrentDescPtr) )
        {
            /* TODO : pre-detect : protect the packet with AGP */
        }



        if (zfCheckAGG(zgDnQCurrentDescPtr))
		{/* aggregation packet */

            /* setup a completed AMPDU */
		    if (zfCheckAGP(zgDnQCurrentDescPtr))
            {
                zgDnQTUCount++;
            }


            /* Update advance DnQ current Desc pointer */
            zgDnQCurrentDescPtr = zgDnQCurrentDescPtr->lastAddr->nextAddr;
		}
		else
        {/* handle single/multi/board cast */
            if (zgDnQCurrentDescPtr == zgDnQ.head)
            {
                /* put TxQ immediately */

                /* Update advance DnQ current Desc pointer */
                zgDnQCurrentDescPtr = zgDnQCurrentDescPtr->lastAddr->nextAddr;
				zfSendTUFromDnQToTxQ(1);
            }
            else
			{
				/* waiting for zgDnQTUCount become zero */
				/* workaround : if zgDnQTUCount is zero, then firmware would cause tx hang. */
				if (zgDnQTUCount == 0)
				{
					/* set push bit for prev packets force build a TU */
					zgDnQTUCount++;
					zfSetDnQAGP_w();
				}
			    break;
			}
        }

    }/* end of while */

}

//! \brief Process Down Link DMA descriptor chain.
//! \author Wade Sung
//! \date 2007.12
void zfSendTUFromDnQToTxQ(u8_t bSinglePacket)
{
    struct zsDmaDesc* desc;

    /* zgDnQ own bits changed */
    while ((zgDnQ.head != zgDnQ.terminator) &&
           ((zgDnQ.head->status & ZM_OWN_BITS_MASK) != ZM_OWN_BITS_HW) &&
           (zgDnQ.head != zgDnQCurrentDescPtr))
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

            if (zfCheckAGG(desc))
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
      		  #if ZM_CABQ_SUPPORT == 1
        		txqNum = ((buf[3] >> 12) & 0x3);
        		if (txqNum > 4)
      		  #endif
        		{
           			txqNum = 0;
        		}

                zfDmaPutPacket(&zgTxQ[txqNum], desc);
                #if ZM_CABQ_SUPPORT == 1
                    if (txqNum == 0)
                    {
                #endif
                /* Trigger WLAN TX DMA */
                zm_wl_dma_trigger_reg = (1 << txqNum);
                #if ZM_CABQ_SUPPORT == 1
                    }
                    else if (txqNum == 1)
                    {
                        zgStartVCCA = 1;
                        //zfUartSendStr((u8_t *) "CabQ\r\n");
                        //zm_wl_dma_trigger_reg = (1 << txqNum);
                    }
                #endif

            #if OTUS_NO_SHRINK == 1
                zfPutTraceBuffer((u32_t)desc | 0x02000000);
            #endif
                ch = '0' + txqNum;
                //zfPutChar(ch);

    		}

			if (zfCheckAGP(desc) || bSinglePacket)
			{
				#if 0
                if (zfCheckAGP(desc))
                    zfUartSendStr((u8_t*)"p");
                else
                    zfUartSendStr((u8_t*)"s");
			    #endif
				/*
				 * When catch last packet of one AMPDU ==> complete one TU
				 * or one single packet ==> complete this routine.
				 */
				break;
			}
			#if 0
			else
			{
                zfUartSendStr((u8_t*)"d");
			}
			#endif
        }
      #endif /* #if ZM_FM_LOOPBACK == 1 */
    }/* while */
}


/************************************************************************/
/*                                                                      */
/*    FUNCTION DESCRIPTION                  zfHostIfIsr                 */
/*      PTA interrupt and command handle.                               */
/*                                                                      */
/*    ROUTINES CALLED                                                   */
/*      zfMainLoop                                                      */
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
void zfHostIfIsr(void)
{
    struct zsDmaDesc* desc;
    u16_t intr;

    intr = (u16_t)ZM_PTA_INT_FLAG_REG;

    if ((intr & (ZM_PTA_DOWN_INT_BIT|ZM_PTA_UP_INT_BIT))!=0)
    {
#if 1 /* softap */

		zfHostDlTask();

#else /* softap */
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
                #if ZM_CABQ_SUPPORT == 1
                    txqNum = ((buf[3] >> 12) & 0x3);
                    if (txqNum > 4)
                #endif
                    {
                        txqNum = 0;
                    }

                    zfDmaPutPacket(&zgTxQ[txqNum], desc);

                #if ZM_CABQ_SUPPORT == 1
                    if (txqNum == 0)
                    {
                #endif
                    /* Trigger WLAN TX DMA */
                    zm_wl_dma_trigger_reg = (1 << txqNum);
                #if ZM_CABQ_SUPPORT == 1
                    }
                    else if (txqNum == 1)
                    {
                        zgStartVCCA = 1;
                        //zfUartSendStr((u8_t *) "CabQ\r\n");
                        //zm_wl_dma_trigger_reg = (1 << txqNum);
                    }
                #endif
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
          #endif /* #if ZM_FM_LOOPBACK == 1 */
        }
#endif /* softap */

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
