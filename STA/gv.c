/************************************************************************/
/*  Copyright (c) 2000-2005  ZyDAS Technology Corporation               */
/*                                                                      */
/*  Module Name : gv.c                                                  */
/*                                                                      */
/*  Abstract                                                            */
/*      This module contains global valible delcation.                  */
/*                                                                      */
/*  ROUTINES                                                            */
/*                                                                      */
/*  NOTES                                                               */
/*      None                                                            */
/*                                                                      */
/************************************************************************/
#include "dt_defs.h"
#include "desc.h"
#include "intrq_defs.h"

#include "sta.h"

u32_t zgIdleCount;
u8_t zgPacketDumpFlag;
u32_t zgAggMode;

/* Tally */
struct DB_TALLY_STRUCT zgTally;
u32_t zgRxOverRunWorkAround;

struct zsDmaQueue zgUpQ;
struct zsDmaQueue zgDnQ;
struct zsDmaQueue zgTxQ[5];
struct zsDmaQueue zgRxQ;
u8_t zgIntrINQ[ZM_INTRQ_NUM][ZM_INTRQ_BUF_SIZE];

u32_t zgRxMaxSize;

/* Frequency setting */
u32_t zgFrequency;

/* BA fail retry send count */
u32_t zgBAFailRetryCount;

u32_t zgUsbTxMode;

/* testing for OTUS phy access with Tx path */
u32_t zgPhyTxPathTestFlag;

/* MDK mode for no retransmit (rate) 1,  0=>retry by down tx rate */
u8_t zgDontRetransmit;

/* 2.4G 5G */
u8_t zg5GhzFreq = 0;

/* PHY BB */
u8_t zgDYNAMIC_HT2040_EN;
u8_t zgHT_ENABLE;
u8_t zgExtOffset;

u8_t zgIntrINQHead;
u8_t zgIntrINQTail;
u8_t zgIntrINQNum;

#if ZM_INT_USE_EP2 == 1
/* EP3 replace by EP2 */
struct zsDmaDesc* zgIntDesc;
u8_t zgIntrINAvailable;
#endif

#if ZM_BAR_AUTO_BA == 1
struct zsDmaDesc* zgBADesc;
u8_t zgBAAvailable;
u8_t zgBARGotState;
u32_t zgBARTimeOut;
#endif

#if ZM_TX_DELAY_DESC == 1
struct zsDmaDesc* zgTxDelayDesc[ZM_TX_DELAY_DESC_NUM];
u32_t zgTxDelayDescIdx;
u32_t zgLastDelayDesc;
#endif
u32_t zgNextStartDesc;

volatile u32_t zgBaErrorCount = 0;
u16_t zgBaStsCurrRate;
u8_t zgGenBAIntr;
u16_t zgBARate[4];
u16_t zgBAFailCnt[4];

#if ZM_TX_HANG_CHK == 1
u32_t zgLastTxQHead;
u16_t zgTxQSameCnt;
#endif

u8_t  u8Watchdog_Enable;
u16_t zgBlockTx;
u16_t zgTxCompleted;
u16_t zgEnableFwRetry;

#if ZM_BB_RESET == 1
u32_t zgRTSRetryCnt;
u32_t zgGenBBResetCnt;
#endif
