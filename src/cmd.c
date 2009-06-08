/************************************************************************/
/*  Copyright (c) 2000-2005  ZyDAS Technology Corporation               */
/*                                                                      */
/*  Module Name : cmd.c                                                 */
/*                                                                      */
/*  Abstract                                                            */
/*      This module contains host Commands handle functions.            */
/*                                                                      */
/*  ROUTINES                                                            */
/*                                                                      */
/*      zfCmdHandler                                                    */
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
#include "uart_extr.h"
#include "cmd_defs.h"
#include "api_extr.h"
#include "intrq_extr.h"

#include "sta.h"

void HW_CAM_Read128(u32_t address, u32_t *data);
void zfDisableCamUser(u16_t userId);
void zfEnableCamUser(u16_t userId);
void HW_CAM_Write128(u32_t address, u32_t *data);
void zfSwReset(void);

extern void zfDKReSetStart(void);
extern void zfDKReSetEnd(void);
extern void zfWorkAround_SwReset(void);
extern void zfResetUSBFIFO(void);
extern void zfTurnOffPower(void);
extern void zfJumpToBootCode(void);


#define zfwWriteReg(addr, val) (*(volatile u32_t*)(0x1c3000+(addr<<2)) = val)

#if ZM_CAM_USER_NUM == 64
#define MAX_USER		64
#define MAX_KEY_LENGTH		16      // no use
#define ENCRY_TYPE_START_ADDR	24
#define DEFAULT_ENCRY_TYPE	26
#define KEY_START_ADDR		27
#define STA_KEY_START_ADDR	155
#define COUNTER_START_ADDR      163
#define STA_COUNTER_START_ADDR	165
#else
#define MAX_USER		16
#define MAX_KEY_LENGTH		16      // no use
#define ENCRY_TYPE_START_ADDR	6
#define DEFAULT_ENCRY_TYPE	7
#define KEY_START_ADDR		8
#define STA_KEY_START_ADDR	40
#define COUNTER_START_ADDR      48
#define STA_COUNTER_START_ADDR	49
#endif

struct zsSetKeyCmdStruct
{
    u16_t user;
    u16_t keyId;
    u16_t type;
    u8_t  macAddr[6];
    u32_t key[4];
};

/************************************************************************/
/*                                                                      */
/*    FUNCTION DESCRIPTION                  zfCmdHandler                */
/*      Command handle.                                                 */
/*      ZM_CMD_ECHO, ZM_CMD_RREG, ZM_CMD_WREG                           */
/*                                                                      */
/*    ROUTINES CALLED                                                   */
/*                                                                      */
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
void zfCmdHandler(void)
{
    u16_t i;
    u8_t len, type;
    u32_t frequency;
    u32_t addr, val;
    u16_t row;
    u16_t wordId, byteId, nibbleId;
    u32_t data[4];
    u8_t *rspBuf;
    struct zsSetKeyCmdStruct* setKeyCmd;


    len = *(volatile u8_t*)ZM_CMD_BUFFER;
    type = *(volatile u8_t*)(ZM_CMD_BUFFER+1);

//    zfUartSendStr((u8_t*)"C");
    //zfUartSendStrAndHex((u8_t*)"len=", len);
    //zfUartSendStrAndHex((u8_t*)"type=", type);
    //zfUartSendStrAndHex((u8_t*)"seq=", *(volatile u16_t*)(ZM_CMD_BUFFER+2));
    //zfUartSendStrAndHex((u8_t*)"addr=", *(volatile u32_t*)(ZM_CMD_BUFFER+4));
    //zfUartSendStrAndHex((u8_t*)"value=", *(volatile u32_t*)(ZM_CMD_BUFFER+8));

    rspBuf = zfGetFreeIntrINQTailBuf();

    /* For command response, we have to let it pass the upper layer */
    if (rspBuf == NULL) {
        zfUartSendStrAndHex((u8_t *) "rspBuf == NULL, zgIntrINQNum=", zgIntrINQNum);

        /* Overwrite the latest buffer */
        rspBuf = (u8_t *) zgIntrINQ[zgIntrINQTail];
    }

    *(volatile u32_t*)rspBuf = *(volatile u32_t*)ZM_CMD_BUFFER;

    if (type == ZM_CMD_ECHO)
    {
        for (i=0; i<len; i++)
        {
            *(volatile u8_t*)(rspBuf+4+i) =
                *(volatile u8_t*)(ZM_CMD_BUFFER+4+i);
        }

        zgBlockTx = 0;
    }
    else if (type == ZM_CMD_RREG)
    {
        for (i=0; i<len; i+=4)
        {
            addr = *(volatile u32_t*)(ZM_CMD_BUFFER+4+i);
            *(volatile u32_t*)(rspBuf+4+i) = *(volatile u32_t*)addr;

            //zfUartSendStrAndHex((u8_t*)"r  addr=", addr);
            //zfUartSendStrAndHex((u8_t*)"   value=", *(volatile u32_t*)addr);
        }
        *(volatile u8_t*)(rspBuf) = len;
    }
    else if (type == ZM_CMD_WREG)
    {
        *(volatile u8_t*)(rspBuf) = 0;
        for (i=0; i<len; i+=8)
        {
            addr = *(volatile u32_t*)(ZM_CMD_BUFFER+4+i);
            val = *(volatile u32_t*)(ZM_CMD_BUFFER+8+i);
            *(volatile u32_t*)addr = val;

            //zfUartSendStrAndHex((u8_t*)"w  addr=", addr);
            //zfUartSendStrAndHex((u8_t*)"   value=", val);
        }
    }
#if 0
    else if (type == ZM_CMD_BITAND)
    {
        *(volatile u8_t*)(rspBuf) = 0;
        for (i=0; i<len; i+=8)
        {
            addr = *(volatile u32_t*)(ZM_CMD_BUFFER+4+i);
            val = *(volatile u32_t*)(ZM_CMD_BUFFER+8+i);
            *(volatile u32_t*)addr &= val;
        }
    }
    else if (type == ZM_CMD_BITOR)
    {
        *(volatile u8_t*)(rspBuf) = 0;
        for (i=0; i<len; i+=8)
        {
            addr = *(volatile u32_t*)(ZM_CMD_BUFFER+4+i);
            val = *(volatile u32_t*)(ZM_CMD_BUFFER+8+i);
            *(volatile u32_t*)addr |= val;
        }
    }
#endif
	else if (type == ZM_CMD_RF_INIT)
	{
        u16_t ret;
        u32_t delta_slope_coeff_exp = *(volatile u32_t*)(ZM_CMD_BUFFER+16);
        u32_t delta_slope_coeff_man = *(volatile u32_t*)(ZM_CMD_BUFFER+20);
        u32_t delta_slope_coeff_exp_shgi = *(volatile u32_t*)(ZM_CMD_BUFFER+24);
        u32_t delta_slope_coeff_man_shgi = *(volatile u32_t*)(ZM_CMD_BUFFER+28);

		u32_t finiteLoopCount = *(volatile u32_t*)(ZM_CMD_BUFFER+32);

        frequency 			= *(volatile u32_t*)(ZM_CMD_BUFFER+4);
		zgDYNAMIC_HT2040_EN = (u8_t)(*(volatile u32_t*)(ZM_CMD_BUFFER+8));
		zgHT_ENABLE			= (u8_t)((*(volatile u32_t*)(ZM_CMD_BUFFER+12)) & 0x1);
		zgExtOffset			= (u8_t)((*(volatile u32_t*)(ZM_CMD_BUFFER+12)) >> 2);
        //zfUartSendStrAndHex((u8_t*)"delta_slope_coeff_exp=", delta_slope_coeff_exp);
        //zfUartSendStrAndHex((u8_t*)"delta_slope_coeff_man=", delta_slope_coeff_man);
        //zfUartSendStrAndHex((u8_t*)"delta_slope_coeff_exp_shgi=", delta_slope_coeff_exp_shgi);
        //zfUartSendStrAndHex((u8_t*)"delta_slope_coeff_man_shgi=", delta_slope_coeff_man_shgi);

        ret = zfInitRf(frequency,
		         delta_slope_coeff_exp,
                 delta_slope_coeff_man,
                 delta_slope_coeff_exp_shgi,
                 delta_slope_coeff_man_shgi,
				 finiteLoopCount);

        *(volatile u8_t*)(rspBuf) = 28;
        *(volatile u32_t*)(rspBuf+4) = ret;
		*(volatile u32_t*)(rspBuf+8) =  *(volatile u32_t*)(0x1bc000+0x9800+(25<<2));
		*(volatile u32_t*)(rspBuf+12) = *(volatile u32_t*)(0x1bc000+0xa800+(25<<2));
		*(volatile u32_t*)(rspBuf+16) = *(volatile u32_t*)(0x1bc000+0xb800+(25<<2));
		*(volatile u32_t*)(rspBuf+20) = *(volatile u32_t*)(0x1bc000+0x9800+(111<<2));
		*(volatile u32_t*)(rspBuf+24) = *(volatile u32_t*)(0x1bc000+0xa800+(111<<2));
		*(volatile u32_t*)(rspBuf+28) = *(volatile u32_t*)(0x1bc000+0xb800+(111<<2));
	}
    else if (type == ZM_CMD_FREQUENCY)
    {
        u16_t ret;

        u32_t delta_slope_coeff_exp = *(volatile u32_t*)(ZM_CMD_BUFFER+16);
        u32_t delta_slope_coeff_man = *(volatile u32_t*)(ZM_CMD_BUFFER+20);
        u32_t delta_slope_coeff_exp_shgi = *(volatile u32_t*)(ZM_CMD_BUFFER+24);
        u32_t delta_slope_coeff_man_shgi = *(volatile u32_t*)(ZM_CMD_BUFFER+28);

		u32_t finiteLoopCount = *(volatile u32_t*)(ZM_CMD_BUFFER+32);

        frequency 			= *(volatile u32_t*)(ZM_CMD_BUFFER+4);
		zgDYNAMIC_HT2040_EN = (u8_t)(*(volatile u32_t*)(ZM_CMD_BUFFER+8));
		zgHT_ENABLE			= (u8_t)((*(volatile u32_t*)(ZM_CMD_BUFFER+12)) & 0x1);
		zgExtOffset			= (u8_t)((*(volatile u32_t*)(ZM_CMD_BUFFER+12)) >> 2);
        //zfUartSendStrAndHex((u8_t*)"delta_slope_coeff_exp=", delta_slope_coeff_exp);
        //zfUartSendStrAndHex((u8_t*)"delta_slope_coeff_man=", delta_slope_coeff_man);
        //zfUartSendStrAndHex((u8_t*)"delta_slope_coeff_exp_shgi=", delta_slope_coeff_exp_shgi);
        //zfUartSendStrAndHex((u8_t*)"delta_slope_coeff_man_shgi=", delta_slope_coeff_man_shgi);

        ret = zfSetChannel(frequency,
		             delta_slope_coeff_exp,
                     delta_slope_coeff_man,
                     delta_slope_coeff_exp_shgi,
                     delta_slope_coeff_man_shgi,
					 finiteLoopCount);

        *(volatile u8_t*)(rspBuf) = 28;
        *(volatile u32_t*)(rspBuf+4) = ret;
		*(volatile u32_t*)(rspBuf+8) =  *(volatile u32_t*)(0x1bc000+0x9800+(25<<2));
		*(volatile u32_t*)(rspBuf+12) = *(volatile u32_t*)(0x1bc000+0xa800+(25<<2));
		*(volatile u32_t*)(rspBuf+16) = *(volatile u32_t*)(0x1bc000+0xb800+(25<<2));
		*(volatile u32_t*)(rspBuf+20) = *(volatile u32_t*)(0x1bc000+0x9800+(111<<2));
		*(volatile u32_t*)(rspBuf+24) = *(volatile u32_t*)(0x1bc000+0xa800+(111<<2));
		*(volatile u32_t*)(rspBuf+28) = *(volatile u32_t*)(0x1bc000+0xb800+(111<<2));

    }
	else if (type == ZM_CMD_FREQ_STRAT)
	{
		zfNotifySetChannel();
        *(volatile u8_t*)(rspBuf) = 0;
	}
#if 1
    else if (type == ZM_CMD_EKEY)
    {
        setKeyCmd = (struct zsSetKeyCmdStruct*)(ZM_CMD_BUFFER+4);

        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_EKEY: user=", setKeyCmd->user);
        //zfUartSendStrAndHex((u8_t*)"keyID=", setKeyCmd->keyId);
        //zfUartSendStrAndHex((u8_t*)"EncType=", setKeyCmd->type);
        //zfUartSendStrAndHex((u8_t*)"mac[0]=", setKeyCmd->macAddr[0]);
        //zfUartSendStrAndHex((u8_t*)"mac[1]=", setKeyCmd->macAddr[1]);
        //zfUartSendStrAndHex((u8_t*)"mac[2]=", setKeyCmd->macAddr[2]);
        //zfUartSendStrAndHex((u8_t*)"mac[3]=", setKeyCmd->macAddr[3]);
        //zfUartSendStrAndHex((u8_t*)"mac[4]=", setKeyCmd->macAddr[4]);
        //zfUartSendStrAndHex((u8_t*)"mac[5]=", setKeyCmd->macAddr[5]);
        //zfUartSendStrAndHex((u8_t*)"key[0]=", setKeyCmd->key[0]);
        //zfUartSendStrAndHex((u8_t*)"key[1]=", setKeyCmd->key[1]);
        //zfUartSendStrAndHex((u8_t*)"key[2]=", setKeyCmd->key[2]);
        //zfUartSendStrAndHex((u8_t*)"key[3]=", setKeyCmd->key[3]);

        /* for 64 to 16 user space */
        if (setKeyCmd->user > (MAX_USER+3))
            setKeyCmd->user = MAX_USER;

        if ((setKeyCmd->user > (MAX_USER+3)) || (setKeyCmd->keyId > 1))
        {
            *(volatile u8_t*)(rspBuf) = 1;
            //zfUartSendStr((u8_t*)"Invalid user or key ID!");
        }
        else
        {
            *(volatile u8_t*)(rspBuf) = 1;

            /* Disable Key */
            zfDisableCamUser(setKeyCmd->user);

            /* Set encrypt type */
            if (setKeyCmd->user <= (MAX_USER+3))
            {
                if (setKeyCmd->user >= MAX_USER)
                {
                    /* default */
                    row = DEFAULT_ENCRY_TYPE;
                    wordId = 0;
                    nibbleId = (setKeyCmd->user-MAX_USER) & 0x7;
                }
                else
                {
                    row = ENCRY_TYPE_START_ADDR + (setKeyCmd->user>>5);
                    wordId = (setKeyCmd->user >> 3) & 0x3;
                    nibbleId = setKeyCmd->user & 0x7;
                }


                HW_CAM_Read128(row, data);
                data[wordId] &= (~(0xf<<((u32_t)nibbleId*4)));
                data[wordId] |= (setKeyCmd->type<<((u32_t)nibbleId*4));
                HW_CAM_Write128(row, data);
            }

            /* Set MAC address */
            if (setKeyCmd->user < MAX_USER)
            {
                wordId = (setKeyCmd->user >> 2) & 0x3;
                byteId = setKeyCmd->user & 0x3;
                row = (setKeyCmd->user >> 4) * 6;

                for (i=0; i<6; i++)
                {
                    HW_CAM_Read128(row+i, data);
                    data[wordId] &= (~(0xff<<((u32_t)byteId*8)));
                    data[wordId] |= (setKeyCmd->macAddr[i]<<((u32_t)byteId*8));
                    HW_CAM_Write128(row+i, data);
                }
            }

            /* Set key */
            row = KEY_START_ADDR + (setKeyCmd->user*2) + setKeyCmd->keyId;
#if 0
            /* tentitaive patch for CAM */
            /* Bug : The default keys are overlapped with user 62 and 63  */
            /* on FPGA revision before 2006.1.4                           */
            if (row >= 155)
            {
                row -= 4;
            }
#endif

            for (i=0; i<4; i++)
            {
                data[i] = setKeyCmd->key[i];
            }
            HW_CAM_Write128(row, data);

            /* Enable Key */
            zfEnableCamUser(setKeyCmd->user);
        }
    }
    else if (type == ZM_CMD_DKEY)
    {
        *(volatile u8_t*)(rspBuf) = 0;
        val = *(volatile u32_t*)(ZM_CMD_BUFFER+4);
        val = val & 0xffff;

        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_DKEY: user=", val);

        /* Disable Key */
        zfDisableCamUser(val);
    }
#endif
    else if (type == ZM_CMD_TALLY)
    {
		/* Update Tally */
		zgTally.RxTotalCount += zm_wl_rx_count_reg;
	    zgTally.RxCRC32      += zm_wl_rx_crc32_reg;
	    zgTally.RxCRC16      += zm_wl_rx_crc16_reg;

		zgRxOverRunWorkAround    += zm_wl_rx_overrun_reg;
	    zgTally.RxOverRun    	 += zgRxOverRunWorkAround;

	    zgTally.RxTimeOut    += zm_wl_rx_timeout_reg;
	    zgTally.RxUniError   += zm_wl_rx_err_uni_reg;
	    zgTally.RxMulError   += zm_wl_rx_err_mul_reg;

        zgTally.RxMPDU                += *(volatile u32_t*)0x1c3ca0;
        zgTally.RxDropMPDU            += *(volatile u32_t*)0x1c3ca4;
        zgTally.RxDelMPDU             += *(volatile u32_t*)0x1c3ca8;

        zgTally.RxPhyMiscError        += *(volatile u32_t*)0x1c3cac;
        zgTally.RxPhyXRError          += *(volatile u32_t*)0x1c3cb0;
        zgTally.RxPhyOFDMError        += *(volatile u32_t*)0x1c3cb4;
        zgTally.RxPhyCCKError         += *(volatile u32_t*)0x1c3cb8;
        zgTally.RxPhyHTError          += *(volatile u32_t*)0x1c3cbc;
        zgTally.RxPhyTotalCount       += *(volatile u32_t*)0x1c3cc0;

	    zgTally.TxTotalCount += zm_wl_tx_count_reg;
	    zgTally.TxUnderRun   += zm_wl_tx_underrun_reg;
	    zgTally.TxRetryCount += zm_wl_tx_retry_count_reg;

        zgTally.TxAmpduCnt += zm_wl_ampdu_count_reg >> 16;
        zgTally.TxMpduCnt += zm_wl_mpdu_count_reg >> 16;

        *(volatile u8_t*)(rspBuf) = 56;
        *(volatile u32_t*)(rspBuf+4)  = zgTally.TxUnderRun;
        *(volatile u32_t*)(rspBuf+8)  = zgTally.RxTotalCount;
        *(volatile u32_t*)(rspBuf+12) = zgTally.RxCRC32;
        *(volatile u32_t*)(rspBuf+16) = zgTally.RxCRC16;
        *(volatile u32_t*)(rspBuf+20) = zgTally.RxUniError;
        *(volatile u32_t*)(rspBuf+24) = zgTally.RxOverRun;
        *(volatile u32_t*)(rspBuf+28) = zgTally.RxMulError;
        *(volatile u32_t*)(rspBuf+32) = zgTally.TxRetryCount;
        *(volatile u32_t*)(rspBuf+36) = zgTally.TxTotalCount;
        *(volatile u32_t*)(rspBuf+40) = zgTally.RxTimeOut;
        *(volatile u32_t*)(rspBuf+44) = zgTally.AggTxCnt;
        *(volatile u32_t*)(rspBuf+48) = zgTally.BAFailCnt;
        *(volatile u32_t*)(rspBuf+52) = zgTally.TxAmpduCnt;
        *(volatile u32_t*)(rspBuf+56) = zgTally.TxMpduCnt;


        zgTally.TxUnderRun = 0;
        zgTally.RxTotalCount = 0;
        zgTally.RxCRC32 = 0;
        zgTally.RxCRC16 = 0;
        zgTally.RxUniError= 0;
        zgTally.RxOverRun = 0;
        zgTally.RxMulError = 0;
        zgTally.TxRetryCount = 0;
        zgTally.TxTotalCount = 0;
        zgTally.RxTimeOut = 0;

        zgTally.AggTxCnt = 0;
        zgTally.BAFailCnt = 0;
        zgTally.TxAmpduCnt = 0;
        zgTally.TxMpduCnt = 0;

        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: zm_wl_tx_underrun_reg   =", *(volatile u32_t*)(ZM_RSP_BUFFER+4));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: zm_wl_rx_count_reg      =", *(volatile u32_t*)(ZM_RSP_BUFFER+8));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: zm_wl_rx_crc32_reg      =", *(volatile u32_t*)(ZM_RSP_BUFFER+12));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: zm_wl_rx_crc16_reg      =", *(volatile u32_t*)(ZM_RSP_BUFFER+16));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: zm_wl_rx_err_uni_reg    =", *(volatile u32_t*)(ZM_RSP_BUFFER+20));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: zm_wl_rx_overrun_reg    =", *(volatile u32_t*)(ZM_RSP_BUFFER+24));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: zm_wl_rx_err_mul_reg    =", *(volatile u32_t*)(ZM_RSP_BUFFER+28));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: zm_wl_tx_retry_count_reg=", *(volatile u32_t*)(ZM_RSP_BUFFER+32));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: zm_wl_tx_count_reg      =", *(volatile u32_t*)(ZM_RSP_BUFFER+36));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: zm_wl_rx_timeout_reg    =", *(volatile u32_t*)(ZM_RSP_BUFFER+40));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: AggTxCnt   =", *(volatile u32_t*)(ZM_RSP_BUFFER+44));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: BAFailCnt  =", *(volatile u32_t*)(ZM_RSP_BUFFER+48));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: TxAmpduCnt =", *(volatile u32_t*)(ZM_RSP_BUFFER+52));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: TxMpduCnt  =", *(volatile u32_t*)(ZM_RSP_BUFFER+56));
    }
    else if (type == ZM_CMD_TALLY_APD)
    {
        *(volatile u8_t*)(rspBuf) = 36;

        *(volatile u32_t*)(rspBuf+4) = zgTally.RxMPDU;
        *(volatile u32_t*)(rspBuf+8) = zgTally.RxDropMPDU;
        *(volatile u32_t*)(rspBuf+12) = zgTally.RxDelMPDU;

        *(volatile u32_t*)(rspBuf+16) = zgTally.RxPhyMiscError;
        *(volatile u32_t*)(rspBuf+20) = zgTally.RxPhyXRError;
        *(volatile u32_t*)(rspBuf+24) = zgTally.RxPhyOFDMError;
        *(volatile u32_t*)(rspBuf+28) = zgTally.RxPhyCCKError;
        *(volatile u32_t*)(rspBuf+32) = zgTally.RxPhyHTError;
        *(volatile u32_t*)(rspBuf+36) = zgTally.RxPhyTotalCount;

        zgTally.RxMPDU = 0;
        zgTally.RxDropMPDU = 0;
        zgTally.RxDelMPDU = 0;

        zgTally.RxPhyMiscError = 0;
        zgTally.RxPhyXRError = 0;
        zgTally.RxPhyOFDMError = 0;
        zgTally.RxPhyCCKError = 0;
        zgTally.RxPhyHTError = 0;
        zgTally.RxPhyTotalCount = 0;

        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: RxMPDU         =", *(volatile u32_t*)(ZM_RSP_BUFFER+4));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: RxDropMPDU     =", *(volatile u32_t*)(ZM_RSP_BUFFER+8));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: RxDelMPDU      =", *(volatile u32_t*)(ZM_RSP_BUFFER+12));

        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: RxPhyMiscError =", *(volatile u32_t*)(ZM_RSP_BUFFER+16));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: RxPhyXRError   =", *(volatile u32_t*)(ZM_RSP_BUFFER+20));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: RxPhyOFDMError =", *(volatile u32_t*)(ZM_RSP_BUFFER+24));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: RxPhyCCKError  =", *(volatile u32_t*)(ZM_RSP_BUFFER+28));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: RxPhyHTError   =", *(volatile u32_t*)(ZM_RSP_BUFFER+32));
        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_TALLY: RxPhyTotalCount=", *(volatile u32_t*)(ZM_RSP_BUFFER+36));
    }
    else if (type == ZM_CMD_CONFIG)
    {
        *(volatile u8_t*)(rspBuf) = 0;
        zgRxMaxSize = *(volatile u32_t*)(ZM_CMD_BUFFER+4);
		zgDontRetransmit = (u8_t)(*(volatile u32_t*)(ZM_CMD_BUFFER+8));

        //zfUartSendStrAndHex((u8_t*)"ZM_CMD_CONFIG: set value=", zgRxMaxSize);
    }
    else if (type == ZM_CMD_SWRST)
    {
        //zfUartSendStr((u8_t *) "Software Reset\r\n");
        //zfSwReset();
        zfWorkAround_SwReset();
        *(volatile u8_t*)(rspBuf) = 0;
    }
#if 0
	else if (type == ZM_CMD_DKRST)
	{
		u8_t flag = (u8_t)(*(volatile u32_t*)(ZM_CMD_BUFFER+4));
		if (flag)
			zfDKReSetStart();
		else
			zfDKReSetEnd();
		*(volatile u8_t*)(rspBuf) = 0;
	}
#endif
    else if (type == ZM_CMD_FW_RETRY)
    {
        zgEnableFwRetry = (u8_t)(*(volatile u32_t*)(ZM_CMD_BUFFER+4));
		*(volatile u8_t*)(rspBuf) = 0;
    }
    else if (type == ZM_CMD_REBOOT)
    {
	/* write watchdog magic pattern for suspend  */
        ZM_WATCH_DOG_MAGIC_REG &= 0xFFFF;
        ZM_WATCH_DOG_MAGIC_REG |= 0x98760000;

        /* Disable watchdog */
        ZM_WATCH_DOG_REG = 0xFFFF;

        /* Reset USB FIFO */
        zfResetUSBFIFO();

        /* Turn off power */
        zfTurnOffPower();

        // add by ygwei for work around USB PHY chirp sequence problem
        *((volatile u32_t *)0x10F100) = 0x12345678;

        /* Jump to boot code */
        zfJumpToBootCode();
    }
    else if (type == ZM_CMD_WREEPROM)
    {
		//TODO: modify ZM_CMD_WREEPROM to multiple write
        u16_t ii,iii;

	    for (ii = 0; ii < len; ii+=8)
	    {

            addr = *(volatile u32_t*)(ZM_CMD_BUFFER+4+ii);
            val = *(volatile u32_t*)(ZM_CMD_BUFFER+8+ii);
	//        addr = *(volatile u32_t*)(ZM_CMD_BUFFER+4);
    //        val = *(volatile u32_t*)(ZM_CMD_BUFFER+8);
            //zfUartSendStr((u8_t *) "EEPROM Write, addr=");
            //zfUartSendHex(addr);
            //zfUartSendStr((u8_t *) " val=");
            //zfUartSendHex(val);
            //zfUartSendStr((u8_t *) "\r\n");

            for (iii = 0; iii < 4; iii++)
            {
                *((volatile u8_t *) (addr+iii)) = val;
                val = val >> 8;
            }
        }
    }
    else if (type == ZM_CMD_FW_DL_INIT)
    {
		u32_t val;

        //zfUartSendStr((u8_t*)"ZM_CMD_FW_DL_INIT");

        /* Software Reset, before we use new downloaded SPI firmware*/
		val = *(volatile u32_t*)(0x1c3500);
		val |= 0x20;

		*(volatile u32_t*)(0x1c3500) = val;

		/* Clsoe Watch dog control(_ZF_WATCH_DOG) */
		// disable Watch dog
		ZM_WATCH_DOG_REG = 0xFFFF;

		u8Watchdog_Enable = 0;

        *(volatile u8_t*)(rspBuf) = len;
    }
    else
    {
        zfUartSendStrAndHex((u8_t*)"Unknown command=",
                *(volatile u32_t*)ZM_CMD_BUFFER);
    }

}

void zfDisableCamUser(u16_t userId)
{
    if (userId >= MAX_USER)
	    return;

    if (userId <= 31)
    {
        zm_cam_roll_call_tablel_reg &= (~((u32_t)1<<userId));
    }
    else if (userId <= 63)
    {
        zm_cam_roll_call_tableh_reg &= (~((u32_t)1<<(userId-32)));
    }
    return;
}

void zfEnableCamUser(u16_t userId)
{
    if (userId >= MAX_USER)
	    return;

    if (userId <= 31)
    {
        zm_cam_roll_call_tablel_reg |= (((u32_t)1)<<userId);
    }
    else if (userId <= 63)
    {
        zm_cam_roll_call_tableh_reg |= (((u32_t)1)<<(userId-32));
    }
    return;
}


void zfWaitForCamReadReady(void)
{
    while (((*(volatile u32_t*)0x1c373c) & 0x40000000) == 0){}
    //zfUartSendStr((u8_t*)"R");
}

void zfWaitForCamWriteReady(void)
{
    while (((*(volatile u32_t*)0x1c373c) & 0x80000000) == 0){}
    //zfUartSendStr((u8_t*)"W");
}

void zfDelayLoop(u32_t n)
{
    u32_t temp;
    for (temp=0; temp<n;)
    {
        temp++;
    }
}

void HW_CAM_Avail(void)
{
    u32_t tmpValue;

    tmpValue = zm_cam_mode_reg;
    while(tmpValue & HOST_PEND)
    {
        //for(ii=0; ii<10000; ii++){}
        tmpValue = zm_cam_mode_reg;
    }

}


void HW_CAM_Write128(u32_t address, u32_t *data)
{
    HW_CAM_Avail();

    zm_cam_data0_reg = data[0];
    zm_cam_data1_reg = data[1];
    zm_cam_data2_reg = data[2];
    zm_cam_data3_reg = data[3];

    zm_cam_addr_reg = address | CAM_WRITE;

    zfWaitForCamWriteReady();
}



void HW_CAM_Read128(u32_t address, u32_t *data)
{

    HW_CAM_Avail();
    zm_cam_addr_reg = address;

    zfWaitForCamReadReady();
    HW_CAM_Avail();
    data[0] = zm_cam_data0_reg;
    data[1] = zm_cam_data1_reg;
    data[2] = zm_cam_data2_reg;
    data[3] = zm_cam_data3_reg;
}
