#ifndef _GV_EXTR_H
#define _GV_EXTR_H

#include "desc.h"

extern volatile u32_t zgIdleCount;
extern volatile u8_t zgPacketDumpFlag;
extern volatile u32_t zgAggMode;

/* Tally */
extern volatile struct DB_TALLY_STRUCT zgTally;

/* SendPacket setting */
#if 0
extern volatile u8_t   enableRxDump;
extern volatile u32_t  sendPacketLen;
extern volatile u32_t  sendPacketRate;
extern volatile u32_t  sendPacketNum;
extern volatile u32_t  sendPacketSeed;
extern volatile u32_t  sendPacketPattern;
#endif

extern struct zsDmaQueue zgUpQ;
extern struct zsDmaQueue zgDnQ;
extern struct zsDmaQueue zgTxQ[5];
extern struct zsDmaQueue zgRxQ;

extern volatile u32_t zgRxMaxSize;


extern u8_t zg5GhzFreq;

/* PHY BB */
extern u8_t zgDYNAMIC_HT2040_EN;
extern u8_t zgHT_ENABLE;
extern u8_t	zgExtOffset;

/* Frequency setting */
extern u32_t zgFrequency;

/* BA fail retry send count */
extern u32_t zgBAFailRetryCount;

extern u32_t zgUsbTxMode;

/* testing for OTUS phy access with Tx path */
extern u32_t zgPhyTxPathTestFlag;

/* MDK mode for no retransmit (rate) */
extern u8_t zgDontRetransmit;
extern volatile u8_t zgIntrINQHead;
extern volatile u8_t zgIntrINQTail;
extern volatile u8_t zgIntrINQNum;
extern volatile u8_t zgIntrINQ[4][60];
#endif