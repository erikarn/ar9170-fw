/************************************************************************/
/*  Copyright (c) 2000-2004  ZyDAS Technology Corporation               */
/*  Copyright (c) 2007  Atheros Communications, Inc.			*/
/*                                                                      */
/*  Module Name : desc_def.h                                            */
/*                                                                      */
/*  Abstract                                                            */
/*      This module contains DMA descriptor related definitions.        */
/*                                                                      */
/*  NOTES                                                               */
/*      None                                                            */
/*                                                                      */
/************************************************************************/

#ifndef _DESC_DEFS_H
#define _DESC_DEFS_H

#include "compiler.h"

struct zsDmaDesc {
	volatile u16_t status;	// Descriptor status
	volatile u16_t ctrl;	// Descriptor control
	volatile u16_t dataSize;	// Data size
	volatile u16_t totalLen;	// Total length
	struct zsDmaDesc *lastAddr;	// Last address of this chain
	volatile u8_t *dataAddr;	// Data buffer address
	struct zsDmaDesc *nextAddr;	// Next TD address
};

/* 5x Tx, Dn, Rx, Up */
#define ZM_TERMINATOR_NUMBER_B		8

#if ZM_BAR_AUTO_BA == 1
#define ZM_TERMINATOR_NUMBER_BAR 	1
#else
#define ZM_TERMINATOR_NUMBER_BAR 	0
#endif

#if ZM_INT_USE_EP2 == 1
#define ZM_TERMINATOR_NUMBER_INT 	1
#else
#define ZM_TERMINATOR_NUMBER_INT 	0
#endif

#if ZM_TX_DELAY_DESC == 1
#define ZM_TX_DELAY_DESC_NUM		16
#else
#define ZM_TX_DELAY_DESC_NUM		0
#endif

#define ZM_TERMINATOR_NUMBER (ZM_TERMINATOR_NUMBER_B + \
			      ZM_TERMINATOR_NUMBER_BAR + \
			      ZM_TERMINATOR_NUMBER_INT)

#define ZM_BLOCK_SIZE           (256 + 64)

#define ZM_DESCRIPTOR_SIZE      (sizeof(struct zsDmaDesc))

/*
 * Memory layout in RAM:
 *
 * 0x100000			+--
 *				| terminator descriptors (zsDmaDesc)
 *				|  - TX (5x, to wifi)
 *				|  - Down (from USB host)
 *				|  - RX (from wifi)
 *				|  - Up (to USB host)
 *				|  - BAR (optional, auto BA (?))
 *				|  - USB Interrupt (optional, to USB host)
 *				|  - delay descriptors (optional, BA buffering (?))
 *				| total: ZM_TERMINATOR_NUMBER + ZM_TX_DELAY_DESC_NUM
 *				+--
 *				| block descriptors (zsDmaDesc)
 *				| (ZM_BLOCK_NUMBER)
 * ZM_BLOCK_BUFFER_BASE		+-- align to multiple of 64
 *				| block buffers (ZM_BLOCK_SIZE each)
 *				| (ZM_BLOCK_NUMBER)
 * 0x117000			+--
 */

#define ZM_FRAME_MEMORY_SIZE	0x17000

#define BLOCK_ALIGNMENT		64

#define NONBLOCK_DESCRIPTORS_SIZE	\
	ZM_DESCRIPTOR_SIZE * (ZM_TERMINATOR_NUMBER + ZM_TX_DELAY_DESC_NUM)

#define NONBLOCK_DESCRIPTORS_SIZE_ALIGNED	\
	ALIGN(NONBLOCK_DESCRIPTORS_SIZE, BLOCK_ALIGNMENT)

#define ZM_BLOCK_NUMBER	((ZM_FRAME_MEMORY_SIZE - NONBLOCK_DESCRIPTORS_SIZE_ALIGNED) / \
			 (ZM_BLOCK_SIZE + ZM_DESCRIPTOR_SIZE))

struct ar9170_data_block {
	u8_t	data[ZM_BLOCK_SIZE];
};

struct ar9170_dma_memory {
	struct zsDmaDesc		terminator[ZM_TERMINATOR_NUMBER];
	struct zsDmaDesc		delay[ZM_TX_DELAY_DESC_NUM];
	struct zsDmaDesc		block[ZM_BLOCK_NUMBER];
	struct ar9170_data_block	data[ZM_BLOCK_NUMBER] __attribute__((aligned(BLOCK_ALIGNMENT)));
};

extern struct ar9170_dma_memory dma_mem;

#define ZM_DOWN_BLOCK_RATIO     2
#define ZM_RX_BLOCK_RATIO       1
/* Tx 16*2 = 32 packets => 32*(5*320) */
#define ZM_TX_BLOCK_NUMBER      ZM_BLOCK_NUMBER * ZM_DOWN_BLOCK_RATIO / \
                                (ZM_RX_BLOCK_RATIO + ZM_DOWN_BLOCK_RATIO)
#define ZM_RX_BLOCK_NUMBER      ZM_BLOCK_NUMBER-ZM_TX_BLOCK_NUMBER

/* Erro code */
#define ZM_ERR_FS_BIT           1
#define ZM_ERR_LS_BIT           2
#define ZM_ERR_OWN_BITS         3
#define ZM_ERR_DATA_SIZE        4
#define ZM_ERR_TOTAL_LEN        5
#define ZM_ERR_DATA             6
#define ZM_ERR_SEQ              7
#define ZM_ERR_LEN              8

/* Status bits definitions */
/* Own bits definitions */
#define ZM_OWN_BITS_MASK        0x3
#define ZM_OWN_BITS_SW          0x0
#define ZM_OWN_BITS_HW          0x1
#define ZM_OWN_BITS_SE          0x2

/* Control bits definitions */
/* First segament bit */
#define ZM_LS_BIT               0x100
/* Last segament bit */
#define ZM_FS_BIT               0x200

struct zsDmaQueue {
	struct zsDmaDesc *head;
	struct zsDmaDesc *terminator;
};

extern struct zsDmaDesc *zfExchangeDesc(struct zsDmaDesc *desc);

#endif				/* #ifndef _DESC_DEFS_H */
