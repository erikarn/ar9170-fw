/*
 * Copyright (c) 2000-2005  ZyDAS Technology Corporation
 * Copyright	2009	Johannes Berg <johannes@sipsolutions.net>
 *
 * Code to handle commands from the host driver.
 */
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
#include "cam.h"

extern void zfWorkAround_SwReset(void);
extern void zfResetUSBFIFO(void);
extern void zfTurnOffPower(void);
extern void zfJumpToBootCode(void);

struct zsSetKeyCmdStruct {
	u16_t user;
	u16_t keyId;
	u16_t type;
	u8_t macAddr[6];
	u32_t key[4];
};

void zfCmdHandler(void)
{
	u16_t i, ret;
	u8_t len, type;
	u32_t frequency;
	u32_t addr, val;
	u16_t row;
	u16_t wordId, byteId, nibbleId;
	u32_t data[4];
	u8_t *rspBuf;
	struct zsSetKeyCmdStruct *setKeyCmd;

	len = *(volatile u8_t *)ZM_CMD_BUFFER;
	type = *(volatile u8_t *)(ZM_CMD_BUFFER + 1);

	rspBuf = zfGetFreeIntrINQTailBuf();

	/* For command response, we have to let it pass the upper layer */
	if (!rspBuf) {
		zfUartSendStrAndHex((u8_t *) "rspBuf == NULL, zgIntrINQNum=", zgIntrINQNum);

		/* Overwrite the latest buffer */
		rspBuf = (u8_t *) zgIntrINQ[zgIntrINQTail];
	}

	*(volatile u32_t *)rspBuf = *(volatile u32_t *)ZM_CMD_BUFFER;

	switch (type) {
	case ZM_CMD_ECHO:
		for (i = 0; i < len; i++)
			*(volatile u8_t *)(rspBuf + 4 + i) = *(volatile u8_t *)(ZM_CMD_BUFFER + 4 + i);

		zgBlockTx = 0;
		break;
	case ZM_CMD_RREG:
		for (i = 0; i < len; i += 4) {
			addr = *(volatile u32_t *)(ZM_CMD_BUFFER + 4 + i);
			*(volatile u32_t *)(rspBuf + 4 + i) = *(volatile u32_t *)addr;

			//zfUartSendStrAndHex((u8_t*)"r  addr=", addr);
			//zfUartSendStrAndHex((u8_t*)"   value=", *(volatile u32_t*)addr);
		}
		*(volatile u8_t *)(rspBuf) = len;
		break;
	case ZM_CMD_WREG:
		*(volatile u8_t *)(rspBuf) = 0;
		for (i = 0; i < len; i += 8) {
			addr = *(volatile u32_t *)(ZM_CMD_BUFFER + 4 + i);
			val = *(volatile u32_t *)(ZM_CMD_BUFFER + 8 + i);
			*(volatile u32_t *)addr = val;
		}
		break;
	case ZM_CMD_RF_INIT: {
		u32_t delta_slope_coeff_exp = *(volatile u32_t *)(ZM_CMD_BUFFER + 16);
		u32_t delta_slope_coeff_man = *(volatile u32_t *)(ZM_CMD_BUFFER + 20);
		u32_t delta_slope_coeff_exp_shgi = *(volatile u32_t *)(ZM_CMD_BUFFER + 24);
		u32_t delta_slope_coeff_man_shgi = *(volatile u32_t *)(ZM_CMD_BUFFER + 28);

		u32_t finiteLoopCount = *(volatile u32_t *)(ZM_CMD_BUFFER + 32);

		frequency = *(volatile u32_t *)(ZM_CMD_BUFFER + 4);
		zgDYNAMIC_HT2040_EN = (u8_t) (*(volatile u32_t *)(ZM_CMD_BUFFER + 8));
		zgHT_ENABLE = (u8_t) ((*(volatile u32_t *)(ZM_CMD_BUFFER + 12)) & 0x1);
		zgExtOffset = (u8_t) ((*(volatile u32_t *)(ZM_CMD_BUFFER + 12)) >> 2);

		ret = zfInitRf(frequency,
			       delta_slope_coeff_exp,
			       delta_slope_coeff_man,
			       delta_slope_coeff_exp_shgi, delta_slope_coeff_man_shgi, finiteLoopCount);

		*(volatile u8_t *)(rspBuf) = 28;
		*(volatile u32_t *)(rspBuf + 4) = ret;
		*(volatile u32_t *)(rspBuf + 8) = *(volatile u32_t *)(0x1bc000 + 0x9800 + (25 << 2));
		*(volatile u32_t *)(rspBuf + 12) = *(volatile u32_t *)(0x1bc000 + 0xa800 + (25 << 2));
		*(volatile u32_t *)(rspBuf + 16) = *(volatile u32_t *)(0x1bc000 + 0xb800 + (25 << 2));
		*(volatile u32_t *)(rspBuf + 20) = *(volatile u32_t *)(0x1bc000 + 0x9800 + (111 << 2));
		*(volatile u32_t *)(rspBuf + 24) = *(volatile u32_t *)(0x1bc000 + 0xa800 + (111 << 2));
		*(volatile u32_t *)(rspBuf + 28) = *(volatile u32_t *)(0x1bc000 + 0xb800 + (111 << 2));
		break;
		}
	case ZM_CMD_FREQUENCY: {
		u32_t delta_slope_coeff_exp = *(volatile u32_t *)(ZM_CMD_BUFFER + 16);
		u32_t delta_slope_coeff_man = *(volatile u32_t *)(ZM_CMD_BUFFER + 20);
		u32_t delta_slope_coeff_exp_shgi = *(volatile u32_t *)(ZM_CMD_BUFFER + 24);
		u32_t delta_slope_coeff_man_shgi = *(volatile u32_t *)(ZM_CMD_BUFFER + 28);

		u32_t finiteLoopCount = *(volatile u32_t *)(ZM_CMD_BUFFER + 32);

		frequency = *(volatile u32_t *)(ZM_CMD_BUFFER + 4);
		zgDYNAMIC_HT2040_EN = (u8_t) (*(volatile u32_t *)(ZM_CMD_BUFFER + 8));
		zgHT_ENABLE = (u8_t) ((*(volatile u32_t *)(ZM_CMD_BUFFER + 12)) & 0x1);
		zgExtOffset = (u8_t) ((*(volatile u32_t *)(ZM_CMD_BUFFER + 12)) >> 2);
		//zfUartSendStrAndHex((u8_t*)"delta_slope_coeff_exp=", delta_slope_coeff_exp);
		//zfUartSendStrAndHex((u8_t*)"delta_slope_coeff_man=", delta_slope_coeff_man);
		//zfUartSendStrAndHex((u8_t*)"delta_slope_coeff_exp_shgi=", delta_slope_coeff_exp_shgi);
		//zfUartSendStrAndHex((u8_t*)"delta_slope_coeff_man_shgi=", delta_slope_coeff_man_shgi);

		ret = zfSetChannel(frequency,
				   delta_slope_coeff_exp,
				   delta_slope_coeff_man,
				   delta_slope_coeff_exp_shgi, delta_slope_coeff_man_shgi, finiteLoopCount);

		*(volatile u8_t *)(rspBuf) = 28;
		*(volatile u32_t *)(rspBuf + 4) = ret;
		*(volatile u32_t *)(rspBuf + 8) = *(volatile u32_t *)(0x1bc000 + 0x9800 + (25 << 2));
		*(volatile u32_t *)(rspBuf + 12) = *(volatile u32_t *)(0x1bc000 + 0xa800 + (25 << 2));
		*(volatile u32_t *)(rspBuf + 16) = *(volatile u32_t *)(0x1bc000 + 0xb800 + (25 << 2));
		*(volatile u32_t *)(rspBuf + 20) = *(volatile u32_t *)(0x1bc000 + 0x9800 + (111 << 2));
		*(volatile u32_t *)(rspBuf + 24) = *(volatile u32_t *)(0x1bc000 + 0xa800 + (111 << 2));
		*(volatile u32_t *)(rspBuf + 28) = *(volatile u32_t *)(0x1bc000 + 0xb800 + (111 << 2));
		break;
		}
	case ZM_CMD_FREQ_STRAT:
		zfNotifySetChannel();
		*(volatile u8_t *)(rspBuf) = 0;
		break;
	case ZM_CMD_EKEY:
		setKeyCmd = (struct zsSetKeyCmdStruct *)(ZM_CMD_BUFFER + 4);

		/* for 64 to 16 user space */
		if (setKeyCmd->user > (MAX_USER + 3))
			setKeyCmd->user = MAX_USER;

		if ((setKeyCmd->user > (MAX_USER + 3)) || (setKeyCmd->keyId > 1)) {
			*(volatile u8_t *)(rspBuf) = 1;
			//zfUartSendStr((u8_t*)"Invalid user or key ID!");
		} else {
			*(volatile u8_t *)(rspBuf) = 1;

			/* Disable Key */
			zfDisableCamUser(setKeyCmd->user);

			/* Set encrypt type */
			if (setKeyCmd->user <= (MAX_USER + 3)) {
				if (setKeyCmd->user >= MAX_USER) {
					/* default */
					row = DEFAULT_ENCRY_TYPE;
					wordId = 0;
					nibbleId = (setKeyCmd->user - MAX_USER) & 0x7;
				} else {
					row = ENCRY_TYPE_START_ADDR + (setKeyCmd->user >> 5);
					wordId = (setKeyCmd->user >> 3) & 0x3;
					nibbleId = setKeyCmd->user & 0x7;
				}

				HW_CAM_Read128(row, data);
				data[wordId] &= (~(0xf << ((u32_t) nibbleId * 4)));
				data[wordId] |= (setKeyCmd->type << ((u32_t) nibbleId * 4));
				HW_CAM_Write128(row, data);
			}

			/* Set MAC address */
			if (setKeyCmd->user < MAX_USER) {
				wordId = (setKeyCmd->user >> 2) & 0x3;
				byteId = setKeyCmd->user & 0x3;
				row = (setKeyCmd->user >> 4) * 6;

				for (i = 0; i < 6; i++) {
					HW_CAM_Read128(row + i, data);
					data[wordId] &= (~(0xff << ((u32_t) byteId * 8)));
					data[wordId] |= (setKeyCmd->macAddr[i] << ((u32_t) byteId * 8));
					HW_CAM_Write128(row + i, data);
				}
			}

			/* Set key */
			row = KEY_START_ADDR + (setKeyCmd->user * 2) + setKeyCmd->keyId;

			for (i = 0; i < 4; i++)
				data[i] = setKeyCmd->key[i];

			HW_CAM_Write128(row, data);

			/* Enable Key */
			zfEnableCamUser(setKeyCmd->user);
		}
		break;
	case ZM_CMD_DKEY:
		*(volatile u8_t *)(rspBuf) = 0;
		val = *(volatile u32_t *)(ZM_CMD_BUFFER + 4);
		val = val & 0xffff;

		//zfUartSendStrAndHex((u8_t*)"ZM_CMD_DKEY: user=", val);

		/* Disable Key */
		zfDisableCamUser(val);
		break;
	case ZM_CMD_TALLY:
		/* Update Tally */
		zgTally.RxTotalCount += zm_wl_rx_count_reg;
		zgTally.RxCRC32 += zm_wl_rx_crc32_reg;
		zgTally.RxCRC16 += zm_wl_rx_crc16_reg;

		zgRxOverRunWorkAround += zm_wl_rx_overrun_reg;
		zgTally.RxOverRun += zgRxOverRunWorkAround;

		zgTally.RxTimeOut += zm_wl_rx_timeout_reg;
		zgTally.RxUniError += zm_wl_rx_err_uni_reg;
		zgTally.RxMulError += zm_wl_rx_err_mul_reg;

		zgTally.RxMPDU += *(volatile u32_t *)0x1c3ca0;
		zgTally.RxDropMPDU += *(volatile u32_t *)0x1c3ca4;
		zgTally.RxDelMPDU += *(volatile u32_t *)0x1c3ca8;

		zgTally.RxPhyMiscError += *(volatile u32_t *)0x1c3cac;
		zgTally.RxPhyXRError += *(volatile u32_t *)0x1c3cb0;
		zgTally.RxPhyOFDMError += *(volatile u32_t *)0x1c3cb4;
		zgTally.RxPhyCCKError += *(volatile u32_t *)0x1c3cb8;
		zgTally.RxPhyHTError += *(volatile u32_t *)0x1c3cbc;
		zgTally.RxPhyTotalCount += *(volatile u32_t *)0x1c3cc0;

		zgTally.TxTotalCount += zm_wl_tx_count_reg;
		zgTally.TxUnderRun += zm_wl_tx_underrun_reg;
		zgTally.TxRetryCount += zm_wl_tx_retry_count_reg;

		zgTally.TxAmpduCnt += zm_wl_ampdu_count_reg >> 16;
		zgTally.TxMpduCnt += zm_wl_mpdu_count_reg >> 16;

		*(volatile u8_t *)(rspBuf) = 56;
		*(volatile u32_t *)(rspBuf + 4) = zgTally.TxUnderRun;
		*(volatile u32_t *)(rspBuf + 8) = zgTally.RxTotalCount;
		*(volatile u32_t *)(rspBuf + 12) = zgTally.RxCRC32;
		*(volatile u32_t *)(rspBuf + 16) = zgTally.RxCRC16;
		*(volatile u32_t *)(rspBuf + 20) = zgTally.RxUniError;
		*(volatile u32_t *)(rspBuf + 24) = zgTally.RxOverRun;
		*(volatile u32_t *)(rspBuf + 28) = zgTally.RxMulError;
		*(volatile u32_t *)(rspBuf + 32) = zgTally.TxRetryCount;
		*(volatile u32_t *)(rspBuf + 36) = zgTally.TxTotalCount;
		*(volatile u32_t *)(rspBuf + 40) = zgTally.RxTimeOut;
		*(volatile u32_t *)(rspBuf + 44) = zgTally.AggTxCnt;
		*(volatile u32_t *)(rspBuf + 48) = zgTally.BAFailCnt;
		*(volatile u32_t *)(rspBuf + 52) = zgTally.TxAmpduCnt;
		*(volatile u32_t *)(rspBuf + 56) = zgTally.TxMpduCnt;

		zgTally.TxUnderRun = 0;
		zgTally.RxTotalCount = 0;
		zgTally.RxCRC32 = 0;
		zgTally.RxCRC16 = 0;
		zgTally.RxUniError = 0;
		zgTally.RxOverRun = 0;
		zgTally.RxMulError = 0;
		zgTally.TxRetryCount = 0;
		zgTally.TxTotalCount = 0;
		zgTally.RxTimeOut = 0;

		zgTally.AggTxCnt = 0;
		zgTally.BAFailCnt = 0;
		zgTally.TxAmpduCnt = 0;
		zgTally.TxMpduCnt = 0;
		break;
	case ZM_CMD_TALLY_APD:
		*(volatile u8_t *)(rspBuf) = 36;

		*(volatile u32_t *)(rspBuf + 4) = zgTally.RxMPDU;
		*(volatile u32_t *)(rspBuf + 8) = zgTally.RxDropMPDU;
		*(volatile u32_t *)(rspBuf + 12) = zgTally.RxDelMPDU;

		*(volatile u32_t *)(rspBuf + 16) = zgTally.RxPhyMiscError;
		*(volatile u32_t *)(rspBuf + 20) = zgTally.RxPhyXRError;
		*(volatile u32_t *)(rspBuf + 24) = zgTally.RxPhyOFDMError;
		*(volatile u32_t *)(rspBuf + 28) = zgTally.RxPhyCCKError;
		*(volatile u32_t *)(rspBuf + 32) = zgTally.RxPhyHTError;
		*(volatile u32_t *)(rspBuf + 36) = zgTally.RxPhyTotalCount;

		zgTally.RxMPDU = 0;
		zgTally.RxDropMPDU = 0;
		zgTally.RxDelMPDU = 0;

		zgTally.RxPhyMiscError = 0;
		zgTally.RxPhyXRError = 0;
		zgTally.RxPhyOFDMError = 0;
		zgTally.RxPhyCCKError = 0;
		zgTally.RxPhyHTError = 0;
		zgTally.RxPhyTotalCount = 0;
		break;
	case ZM_CMD_CONFIG:
		*(volatile u8_t *)(rspBuf) = 0;
		zgRxMaxSize = *(volatile u32_t *)(ZM_CMD_BUFFER + 4);
		zgDontRetransmit = (u8_t) (*(volatile u32_t *)(ZM_CMD_BUFFER + 8));
		break;
	case ZM_CMD_SWRST:
		zfWorkAround_SwReset();
		*(volatile u8_t *)(rspBuf) = 0;
		break;
	case ZM_CMD_FW_RETRY:
		zgEnableFwRetry = (u8_t) (*(volatile u32_t *)(ZM_CMD_BUFFER + 4));
		*(volatile u8_t *)(rspBuf) = 0;
		break;
	case ZM_CMD_REBOOT:
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
		break;
	case ZM_CMD_WREEPROM: {
		/* TODO: modify ZM_CMD_WREEPROM to multiple write */
		u16_t ii, iii;

		for (ii = 0; ii < len; ii += 8) {

			addr = *(volatile u32_t *)(ZM_CMD_BUFFER + 4 + ii);
			val = *(volatile u32_t *)(ZM_CMD_BUFFER + 8 + ii);
			//        addr = *(volatile u32_t*)(ZM_CMD_BUFFER+4);
			//        val = *(volatile u32_t*)(ZM_CMD_BUFFER+8);
			//zfUartSendStr((u8_t *) "EEPROM Write, addr=");
			//zfUartSendHex(addr);
			//zfUartSendStr((u8_t *) " val=");
			//zfUartSendHex(val);
			//zfUartSendStr((u8_t *) "\n");

			for (iii = 0; iii < 4; iii++) {
				*((volatile u8_t *)(addr + iii)) = val;
				val = val >> 8;
			}
		}
		}
		break;
	case ZM_CMD_FW_DL_INIT: {
		u32_t val;

		/* Software Reset, before we use new downloaded SPI firmware */
		val = *(volatile u32_t *)(0x1c3500);
		val |= 0x20;

		*(volatile u32_t *)(0x1c3500) = val;

		/* Clsoe Watch dog control(_ZF_WATCH_DOG) */
		// disable Watch dog
		ZM_WATCH_DOG_REG = 0xFFFF;

		u8Watchdog_Enable = 0;

		*(volatile u8_t *)(rspBuf) = len;
		}
		break;
	default:
		break;
	}
}
