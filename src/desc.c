/*
 *
 * Copyright (c) 2000-2005  ZyDAS Technology Corporation
 * Copyright	2009	Johannes Berg <johannes@sipsolutions.net>
 *
 * DMA descriptor handling functions
 */
#include "sys_defs.h"
#include "dt_defs.h"
#include "desc.h"
#include "string.h"
#include "gv_extr.h"
#include "reg_defs.h"
#include "uart_extr.h"
#include "compiler.h"
#include "api_extr.h"

static void __noinline copy_zsDmaDesc(struct zsDmaDesc *dst,
				      const struct zsDmaDesc *src)
{
	dst->status = src->status;
	dst->ctrl = src->ctrl;
	dst->dataSize = src->dataSize;
	dst->totalLen = src->totalLen;
	dst->lastAddr = src->lastAddr;
	dst->dataAddr = src->dataAddr;
	dst->nextAddr = src->nextAddr;
}

/*
 *  - Init zgUpQ, zgDnQ, zgTxQ, zgRxQ.
 *  - Setup descriptors and data buffer address.
 *  - Ring descriptors zgRxQ and zgDnQ by zfDmaReclaimPacket().
 *
 * NOTE: LastAddr tempary point (same) to nextAddr after initialize.
 *	 Because LastAddr is don't care in function zfDmaReclaimPacket().
 */
void zfDmaInitDescriptor(void)
{
	u16_t i;
	struct zsDmaDesc *desc[ZM_TERMINATOR_NUMBER];

	/* Init descriptors for terminators */
	for (i = 0; i < ZM_TERMINATOR_NUMBER; i++) {
		desc[i] = (struct zsDmaDesc *)(ZM_DESCRIPTOR_BASE + (i * ZM_DESCRIPTOR_SIZE));
		desc[i]->status = ZM_OWN_BITS_SW;
		desc[i]->ctrl = 0;
		desc[i]->dataSize = 0;
		desc[i]->totalLen = 0;
		desc[i]->lastAddr = 0;
		desc[i]->dataAddr = 0;
		desc[i]->nextAddr = 0;
	}

	/* Assigned terminators to DMA queues */
	zgUpQ.head = zgUpQ.terminator = desc[0];
	zgDnQ.head = zgDnQ.terminator = desc[1];
	zgTxQ[0].head = zgTxQ[0].terminator = desc[2];
	zgTxQ[1].head = zgTxQ[1].terminator = desc[3];
	zgTxQ[2].head = zgTxQ[2].terminator = desc[4];
	zgTxQ[3].head = zgTxQ[3].terminator = desc[5];
	zgTxQ[4].head = zgTxQ[4].terminator = desc[6];
	zgRxQ.head = zgRxQ.terminator = desc[7];
#if ZM_INT_USE_EP2 == 1
	zgIntDesc = desc[8];
#endif

#if ZM_BAR_AUTO_BA == 1
#if ZM_INT_USE_EP2 == 1
	zgBADesc = desc[9];
#else
	zgBADesc = desc[8];
#endif
#endif

	/* Init descriptors and memory blocks */
	for (i = 0; i < ZM_BLOCK_NUMBER; i++) {
		desc[0] = (struct zsDmaDesc *)((ZM_DESCRIPTOR_BASE +
						(ZM_TERMINATOR_NUMBER * ZM_DESCRIPTOR_SIZE))
					       + (i * ZM_DESCRIPTOR_SIZE));
		desc[0]->status = ZM_OWN_BITS_SW;
		desc[0]->ctrl = 0;
		desc[0]->dataSize = ZM_BLOCK_SIZE;
		desc[0]->totalLen = 0;
		desc[0]->lastAddr = desc[0];
		desc[0]->dataAddr = ZM_BLOCK_BUFFER_BASE + (i * ZM_BLOCK_SIZE);
		desc[0]->nextAddr = 0;

#if 0
		if (i < ZM_RX_BLOCK_NUMBER) {
			zfDmaReclaimPacket(&zgRxQ, desc[0]);
		} else {
			zfDmaReclaimPacket(&zgDnQ, desc[0]);
		}
#else
		if (i < ZM_TX_BLOCK_NUMBER) {
			zfDmaReclaimPacket(&zgDnQ, desc[0]);
		} else {
			zfDmaReclaimPacket(&zgRxQ, desc[0]);
		}
#endif

	}

	/* Set DMA address registers */
	ZM_PTA_DN_DMA_ADDRH_REG = (u32_t) zgDnQ.head >> 16;
	ZM_PTA_DN_DMA_ADDRL_REG = (u32_t) zgDnQ.head & 0xffff;
	ZM_PTA_UP_DMA_ADDRH_REG = (u32_t) zgUpQ.head >> 16;
	ZM_PTA_UP_DMA_ADDRL_REG = (u32_t) zgUpQ.head & 0xffff;

	zm_wl_txq0_dma_addr_reg = (u32_t) zgTxQ[0].head;

#if 1
	zm_wl_txq1_dma_addr_reg = (u32_t) zgTxQ[1].head;

	zm_wl_txq2_dma_addr_reg = (u32_t) zgTxQ[2].head;

	zm_wl_txq3_dma_addr_reg = (u32_t) zgTxQ[3].head;

	zm_wl_txq4_dma_addr_reg = (u32_t) zgTxQ[4].head;
#endif

	zm_wl_rx_dma_addr_reg = (u32_t) zgRxQ.head;

#if ZM_TX_DELAY_DESC == 1

	zgTxDelayDescIdx = 0;

	for (i = 0; i < ZM_TX_DELAY_DESC_NUM; i++) {
		zgTxDelayDesc[i] = (struct zsDmaDesc *)(ZM_TX_DELAY_DESC_BASE +
							(i * ZM_DESCRIPTOR_SIZE));
		zgTxDelayDesc[i]->status = ZM_OWN_BITS_SW;
		zgTxDelayDesc[i]->ctrl = 0;
		zgTxDelayDesc[i]->dataSize = i;
		zgTxDelayDesc[i]->totalLen = 0;
		zgTxDelayDesc[i]->lastAddr = zgTxDelayDesc[i];
		zgTxDelayDesc[i]->dataAddr = 0;
		zgTxDelayDesc[i]->nextAddr = 0;
	}
#endif

#if ZM_INT_USE_EP2 == 1
	{
		zgIntDesc->status = ZM_OWN_BITS_SW;
		zgIntDesc->ctrl = (ZM_LS_BIT | ZM_FS_BIT);
		zgIntDesc->dataSize = ZM_BLOCK_SIZE;
		zgIntDesc->totalLen = 0;
		zgIntDesc->lastAddr = zgIntDesc;
		zgIntDesc->dataAddr = ZM_RSP_BUFFER;
		zgIntDesc->nextAddr = 0;

		/* Fill private id for rsp */
		for (i = 0; i < (ZM_INT_USE_EP2_HEADER_SIZE >> 2); i++) {
			*(volatile u32_t *)(ZM_RSP_BUFFER + (i * 4)) =
			    0xffffffff;
		}
	}
#endif

#if ZM_BAR_AUTO_BA == 1
	zgBADesc->status = ZM_OWN_BITS_SW;
	zgBADesc->ctrl = (ZM_LS_BIT | ZM_FS_BIT);
	//zgBADesc->dataSize = ZM_BLOCK_SIZE;
	//zgBADesc->totalLen = 0;
	zgBADesc->lastAddr = zgBADesc;
	zgBADesc->dataAddr = ZM_BA_BUFFER;
	zgBADesc->nextAddr = 0;

	zgBADesc->dataSize = 36;
	zgBADesc->totalLen = 36;

	/* Zero out */
	for (i = 0; i < 128; i += 4) {
		*(volatile u32_t *)(ZM_BA_BUFFER + i) = 0;
	}

	*(volatile u32_t *)(ZM_BA_BUFFER + 0) = 0x00040020;
	*(volatile u32_t *)(ZM_BA_BUFFER + 4) = 0x0026c401;
#endif
}

/*
 * Get a completed packet with # descriptors. Return the first
 * descriptor and pointer the head directly by lastAddr->nextAddr
 */
struct zsDmaDesc *zfDmaGetPacket(struct zsDmaQueue *q)
{
	struct zsDmaDesc *desc = NULL;

	//if (((q->head->status & ZM_OWN_BITS_MASK) == ZM_OWN_BITS_SW)
	//        || ((q->head->status & ZM_OWN_BITS_MASK) == ZM_OWN_BITS_SE))

	if ((((q->head->status & ZM_OWN_BITS_MASK) == ZM_OWN_BITS_SW)
	     && ((u32_t) q != (u32_t) & zgDnQ))
	    || (((q->head->status & ZM_OWN_BITS_MASK) == ZM_OWN_BITS_SE)
		&& ((u32_t) q == (u32_t) & zgDnQ)))
	{
		desc = q->head;

		q->head = desc->lastAddr->nextAddr;
	}
	return desc;
}

/*
 * Free descriptor.
 *
 * Exchange the terminator and the first descriptor of the packet
 * for hardware ascy...
 */
void zfDmaReclaimPacket(struct zsDmaQueue *q, struct zsDmaDesc *desc)
{
	struct zsDmaDesc *tmpDesc;
	struct zsDmaDesc tdesc;

	/* 1. Set OWN bit to 1 for all TDs to be added, clear ctrl and size */
	tmpDesc = desc;
	while (1) {
		tmpDesc->status = ZM_OWN_BITS_HW;
		tmpDesc->ctrl = 0;
		tmpDesc->totalLen = 0;
		tmpDesc->dataSize = ZM_BLOCK_SIZE;
		//tmpDesc->dataAddr &= (~((u32_t)64-1));
		//tmpDesc->totalLen = 0;

		/* TODO : Exception handle */

		if (desc->lastAddr == tmpDesc) {
			break;
		}
		tmpDesc = tmpDesc->nextAddr;
	}

	/* 3. Next address of Last TD to be added = first TD */
	desc->lastAddr->nextAddr = desc;

	/* 2. Copy first TD to be added to TTD */
	copy_zsDmaDesc(&tdesc, desc);

	/* 4. set first TD OWN bit to 0 */
	desc->status &= (~ZM_OWN_BITS_MASK);

	/* 5. Copy TTD to last TD */
	tdesc.status &= (~ZM_OWN_BITS_MASK);
	copy_zsDmaDesc((void *)q->terminator, (void *)&tdesc);
	q->terminator->status |= ZM_OWN_BITS_HW;

	/* Update terminator pointer */
	q->terminator = desc;
}

/*
 * Put a complete packet into the tail of the Queue q.
 * Exchange the terminator and the first descriptor of the packet
 * for hardware ascy...
 */
void zfDmaPutPacket(struct zsDmaQueue *q, struct zsDmaDesc *desc)
{
	struct zsDmaDesc *tmpDesc;
	struct zsDmaDesc tdesc;

	/* 1. Set OWN bit to 1 for all TDs to be added */
	tmpDesc = desc;
	while (1) {
		tmpDesc->status =
		    ((tmpDesc->status & (~ZM_OWN_BITS_MASK)) | ZM_OWN_BITS_HW);
		/* TODO : Exception handle */

		if (desc->lastAddr == tmpDesc) {
			break;
		}
		tmpDesc = tmpDesc->nextAddr;
	}

	/* 3. Next address of Last TD to be added = first TD */
	desc->lastAddr->nextAddr = desc;

	/* If there is only one descriptor, update pointer of last descriptor */
	if (desc->lastAddr == desc) {
		desc->lastAddr = q->terminator;
	}

	/* 2. Copy first TD to be added to TTD */
	copy_zsDmaDesc(&tdesc, desc);

	/* 4. set first TD OWN bit to 0 */
	desc->status &= (~ZM_OWN_BITS_MASK);

	/* 5. Copy TTD to last TD */
	tdesc.status &= (~ZM_OWN_BITS_MASK);
	copy_zsDmaDesc((void *)q->terminator, (void *)&tdesc);
	q->terminator->status |= ZM_OWN_BITS_HW;

	/* Update terminator pointer */
	q->terminator = desc;
}

void zfRecycleRxQ(void)
{
	struct zsDmaDesc *desc;

	while ((zgRxQ.head != zgRxQ.terminator) && ((zgRxQ.head->status
						     & ZM_OWN_BITS_MASK) !=
						    ZM_OWN_BITS_HW)) {
		desc = zfDmaGetPacket(&zgRxQ);

		/* TODO : if len < 5+14+8 or len > zgRxMaxSize, discard rx frame */
		if (desc->totalLen < zgRxMaxSize) {
			zfDmaPutPacket(&zgUpQ, desc);
		} else {
			zfDmaReclaimPacket(&zgRxQ, desc);
		}
	}

	zm_wl_rx_dma_addr_reg = (u32_t) zgRxQ.head;
}

struct zsDmaDesc *zfExchangeDesc(struct zsDmaDesc *desc)
{
#if ZM_TX_DELAY_DESC == 1
	struct zsDmaDesc *tmpDesc;
	copy_zsDmaDesc((void *)zgTxDelayDesc[zgTxDelayDescIdx], (void *)desc);

	zgLastDelayDesc = (u32_t) desc;
	zgNextStartDesc = (u32_t) (desc->lastAddr->nextAddr);

	/* The packet with single desc */
	if (desc->lastAddr == desc) {
		zgTxDelayDesc[zgTxDelayDescIdx]->lastAddr =
		    zgTxDelayDesc[zgTxDelayDescIdx];
	}

	tmpDesc = zgTxDelayDesc[zgTxDelayDescIdx];
	zgTxDelayDesc[zgTxDelayDescIdx] = desc;

	zgTxDelayDescIdx = (zgTxDelayDescIdx + 1) & (ZM_TX_DELAY_DESC_NUM - 1);

	return tmpDesc;
#else
	zgNextStartDesc = (u32_t) (desc->lastAddr->nextAddr);
	return desc;
#endif
}

/* Length inconsistent: totalLen of desc and len of ctrlsetting */
/* Desc length        : D                                       */
/* CtrlSetting length : C                                       */
/*                         C(Nnrmal)+4 = D ~ D = C(WAPI)-12     */
u32_t zfLengthConfirm(struct zsDmaDesc * desc)
{
	u32_t ret = 0;

	u16_t desc_len = desc->totalLen;
	u16_t ctrl_len = (*(volatile u32_t *)(desc->dataAddr) & 0xffff);

	/* check length */
	//if ( desc_len != (ctrl_len+4) )
	if (desc_len > (ctrl_len + 4)) {
		if (!((ctrl_len + 4) & 0x1ff)) {
			*(volatile u32_t *)(0x117758) += 1;
		}

		ret = 1;
	}

	if (ret) {
		*(volatile u32_t *)(0x117754) += 1;
	}

	return ret;
}
