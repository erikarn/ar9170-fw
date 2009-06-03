#ifndef _GV_EXTR_H
#define _GV_EXTR_H

#include "desc.h"
#include "intrq_defs.h"

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

/* soft ap */
extern struct zsDmaDesc* zgDnQCurrentDescPtr;
extern struct zsDmaQueue zgRTQ;
extern u8_t zgDnQTUCount;
extern u8_t zgRTQTUCount;
extern u8_t zgPendingTUCount;

extern volatile u32_t zgRxMaxSize;


extern u8_t zg5GhzFreq;

/* PHY BB */
extern u8_t zgDYNAMIC_HT2040_EN;
extern u8_t zgHT_ENABLE;
extern u8_t zgExtOffset;

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
extern volatile u8_t zgIntrINQ[ZM_INTRQ_NUM][ZM_INTRQ_BUF_SIZE];

//extern volatile u32_t zgBaErrorCount;
//extern volatile u16_t zgBaStsCurrRate;
//extern volatile u8_t zgGenBAIntr;
//extern volatile u16_t zgBARate[4];
//extern volatile u16_t zgBAFailCnt[4];

extern volatile u8_t zgRCBaGrand;
extern volatile u16_t zgRCBaMacAddr[3];
extern volatile u16_t zgRCBaPhyCtrl[2];
extern volatile u16_t zgRCBaMacCtrl;
extern volatile u32_t zgRCBaMPDUCnt;
extern volatile u32_t zgRCBaErrorCnt;

#if ZM_INT_USE_EP2 == 1
/* EP3 replace by EP2 */
extern struct zsDmaDesc* zgIntDesc;
extern u8_t zgIntrINAvailable;
#endif

#if ZM_BAR_AUTO_BA == 1
extern struct zsDmaDesc* zgBADesc;
extern u8_t zgBAAvailable;
extern u8_t zgBARGotState;
extern u32_t zgBARTimeOut;
#endif

#if ZM_TX_DELAY_DESC == 1
extern struct zsDmaDesc* zgTxDelayDesc[ZM_TX_DELAY_DESC_NUM];
extern u32_t zgTxDelayDescIdx;
extern u32_t zgLastDelayDesc;
#endif
extern u32_t zgNextStartDesc;

#if ZM_TX_HANG_CHK == 1
extern u32_t zgLastTxQHead;
extern u16_t zgTxQSameCnt;
#endif
#endif

//Paul
extern u8_t u8Watchdog_Enable;
extern u16_t zgBlockTx;
extern u16_t zgBlockRx;
extern u16_t zgTxCompleted;
extern u16_t zgEnableFwRetry;

extern u32_t zgRTSRetryCnt;
extern u32_t zgGenBBResetCnt;
extern u32_t zgStartVCCA;
