/*
 * Copyright (c) 2000-2005  ZyDAS Technology Corporation
 * Copyright	2009	Johannes Berg <johannes@sipsolutions.net>
 *
 */
#include "sys_defs.h"
#include "dt_defs.h"
#include "desc.h"
#include "gv_extr.h"
#include "reg_defs.h"
#include "api_extr.h"
#include "uart_extr.h"

extern u32_t zfLengthConfirm(struct zsDmaDesc *desc);

/* handle interrupts from DMA chip */
void zfHostIfIsr(void)
{
	struct zsDmaDesc *desc;
	u16_t intr;

	intr = ZM_PTA_INT_FLAG_REG;

	if ((intr & (ZM_PTA_DOWN_INT_BIT | ZM_PTA_UP_INT_BIT)) != 0) {
		/* zgDnQ own bits changed */
		while ((zgDnQ.head != zgDnQ.terminator) &&
		       ((zgDnQ.head->status & ZM_OWN_BITS_MASK) != ZM_OWN_BITS_HW)) {
			desc = zfDmaGetPacket(&zgDnQ);

			if (zgBlockTx == 1) {
				zfDmaReclaimPacket(&zgDnQ, desc);
			} else {
				u16_t *buf;
				u16_t txqNum;

				buf = (u16_t *) desc->dataAddr;

				/* if AGG bit = 1 */
				if ((buf[1] & 0x20) != 0) {
					zgTally.AggTxCnt++;
				}

				/* Length inconsistent: totalLen of desc and len of ctrlsetting */
				if (zfLengthConfirm(desc)) {
					zfDmaReclaimPacket(&zgDnQ, desc);
					ZM_PTA_DN_DMA_TRIGGER_REG = 1;
					continue;
				}
				txqNum = 0;

				zfDmaPutPacket(&zgTxQ[txqNum], desc);
				/* Trigger WLAN TX DMA */
				zm_wl_dma_trigger_reg = (1 << txqNum);
			}
		}

		/* zgUpQ own bits changed */
		while ((zgUpQ.head != zgUpQ.terminator) &&
		       ((zgUpQ.head->status & ZM_OWN_BITS_MASK) != ZM_OWN_BITS_HW)) {
			desc = zfDmaGetPacket(&zgUpQ);

#if ZM_INT_USE_EP2 == 1
			if (desc->dataAddr == ZM_RSP_BUFFER) {
				/* Rsp */
				zgIntrINAvailable = 1;
				zgIntDesc = desc;
			} else
#endif				/* ZM_INT_USE_EP2 == 1 */
			{

				zfDmaReclaimPacket(&zgRxQ, desc);
				/* Trigger WLAN RX DMA */
				zm_wl_dma_trigger_reg = ZM_RXQ_TRIG_BIT;
				//zfPutChar('u');

			}	/* INT use EP2 */
		}		/* end of while */
	}

	/* handle commands coming from chip -- cannot happen since we're USB?! */
	if ((intr & ZM_PTA_CMD_INT) == ZM_PTA_CMD_INT)
		zfCmdHandler();
}
