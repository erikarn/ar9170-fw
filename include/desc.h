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

struct zsDmaDesc {
	volatile u16_t status;	// Descriptor status
	volatile u16_t ctrl;	// Descriptor control
	volatile u16_t dataSize;	// Data size
	volatile u16_t totalLen;	// Total length
	struct zsDmaDesc *lastAddr;	// Last address of this chain
	volatile u32_t dataAddr;	// Data buffer address
	struct zsDmaDesc *nextAddr;	// Next TD address
};

/* Tx5 Dn Rx Up Int */
#define ZM_TERMINATOR_NUMBER_B  8

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

#define ZM_TX_DELAY_DESC_NUM	16
#define ZM_TERMINATOR_NUMBER (8 + ZM_TERMINATOR_NUMBER_BAR + \
                                  ZM_TERMINATOR_NUMBER_INT + \
							      ZM_TX_DELAY_DESC_NUM)

#define ZM_BLOCK_SIZE           (256+64)
#define ZM_DESCRIPTOR_SIZE      (sizeof(struct zsDmaDesc))

#define ZM_FRAME_MEMORY_BASE    0x100000

#if 1
/* 64k */
//#define ZM_FRAME_MEMROY_SIZE    0xf000
/* 96k */
#define ZM_FRAME_MEMROY_SIZE    0x17000

#else
/* fake phy */
/* 128k / 96k */
#define ZM_FRAME_MEMROY_SIZE    (ZM_BLOCK_SIZE+ZM_DESCRIPTOR_SIZE)*(160+60) + \
                                (ZM_DESCRIPTOR_SIZE*ZM_TERMINATOR_NUMBER)+64
#endif

#define ZM_BLOCK_NUMBER         ((ZM_FRAME_MEMROY_SIZE-(ZM_DESCRIPTOR_SIZE* \
                                ZM_TERMINATOR_NUMBER)-64)/(ZM_BLOCK_SIZE \
                                +ZM_DESCRIPTOR_SIZE))
#define ZM_DESC_NUMBER          (ZM_BLOCK_NUMBER + ZM_TERMINATOR_NUMBER)

#define ZM_DESCRIPTOR_BASE		ZM_FRAME_MEMORY_BASE
#define ZM_BLOCK_BUFFER_BASE	(((((ZM_BLOCK_NUMBER+ZM_TERMINATOR_NUMBER) \
                                *ZM_DESCRIPTOR_SIZE) >> 6) << 6) + 0x40 \
                                + ZM_FRAME_MEMORY_BASE)

#define ZM_DOWN_BLOCK_RATIO     2
#define ZM_RX_BLOCK_RATIO       1
/* Tx 16*2 = 32 packets => 32*(5*320) */
#define ZM_TX_BLOCK_NUMBER      ZM_BLOCK_NUMBER * ZM_DOWN_BLOCK_RATIO / \
                                (ZM_RX_BLOCK_RATIO + ZM_DOWN_BLOCK_RATIO)
#define ZM_RX_BLOCK_NUMBER      ZM_BLOCK_NUMBER-ZM_TX_BLOCK_NUMBER

#define ZM_TX_DELAY_DESC_BASE	ZM_FRAME_MEMORY_BASE + ZM_DESCRIPTOR_SIZE*(ZM_TERMINATOR_NUMBER-ZM_TX_DELAY_DESC_NUM)

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

#endif				/* #ifndef _DESC_DEFS_H */
