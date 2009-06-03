#ifndef _DT_DEFS_H
#define _DT_DEFS_H

#include "sys_defs.h"

/* Basic data type */
#define u32_t   unsigned long
#define s32_t   signed long
#define u16_t   unsigned short
#define s16_t   signed short
#define u8_t    unsigned char
#define s8_t    signed char

#define TRUE    (0==0)
#define FALSE   (0!=0)

#define BOOLEAN u8_t

#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80
/* Is this needed ??? */
#define NULL    0

struct DB_TALLY_STRUCT
{
    /* Tally */
    u32_t  RxTotalCount;
    u32_t  RxCRC32;
    u32_t  RxCRC16;
    u32_t  RxOverRun;
    u32_t  RxTimeOut;
	u32_t  RxUniError;
	u32_t  RxMulError;

    u32_t  RxMPDU;           //CA0;
	u32_t  RxDropMPDU;       //CA4;
	u32_t  RxDelMPDU;        //CA8;

	u32_t  RxPhyMiscError;   //CAC;
	u32_t  RxPhyXRError;     //CB0;
	u32_t  RxPhyOFDMError;   //CB4;
	u32_t  RxPhyCCKError;    //CB8;
	u32_t  RxPhyHTError;     //CBC;
	u32_t  RxPhyTotalCount;  //CC0;

    u32_t  TxTotalCount;
    u32_t  TxUnderRun;
    u32_t  TxRetryCount;    

	u32_t  BAFailCnt;
	u32_t  AggTxCnt;
	u32_t  TxAmpduCnt;
	u32_t  TxMpduCnt;
};

#endif

