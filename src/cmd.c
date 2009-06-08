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
#include "usbcmds.h"
#include "api_extr.h"
#include "intrq_extr.h"
#include "sta.h"
#include "cam.h"

extern void zfWorkAround_SwReset(void);
extern void zfResetUSBFIFO(void);
extern void zfTurnOffPower(void);
extern void zfJumpToBootCode(void);

void zfCmdHandler(void)
{
	u16_t i, j, ret;
	u32_t val;
	u16_t row;
	u16_t wordId, byteId, nibbleId;
	u32_t data[4];
	struct ar9170_cmd *cmd = (void *)ZM_CMD_BUFFER;
	struct ar9170_cmd *resp;

	resp = (void *)zfGetFreeIntrINQTailBuf();

	/* For command response, we have to let it pass the upper layer */
	if (!resp) {
		/* Overwrite the latest buffer */
		resp = (void *)zgIntrINQ[zgIntrINQTail];
	}

	resp->cmd = cmd->cmd;
	resp->len = cmd->len;
	resp->padding = cmd->padding;

	switch (cmd->cmd) {
	case ZM_CMD_ECHO:
		for (i = 0; i < cmd->len / 4; i++)
			resp->echo.vals[i] = cmd->echo.vals[i];

		zgBlockTx = 0;
		break;
	case ZM_CMD_RREG:
		for (i = 0; i < cmd->len / 4; i++)
			resp->rreg_res.vals[i] = *cmd->rreg.regs[i];
		resp->len = cmd->len;
		break;
	case ZM_CMD_WREG:
		resp->len = 0;
		for (i = 0; i < cmd->len / 8; i ++)
			*cmd->wreg.regs[i].addr = cmd->wreg.regs[i].val;
		break;
	case ZM_CMD_RF_INIT:
		zgDYNAMIC_HT2040_EN = cmd->rf_init.dynht2040;
		zgHT_ENABLE = cmd->rf_init.ht_settings & 1;
		zgExtOffset = cmd->rf_init.ht_settings >> 2;

		ret = zfInitRf(cmd->rf_init.freq,
			       cmd->rf_init.delta_slope_coeff_exp,
			       cmd->rf_init.delta_slope_coeff_man,
			       cmd->rf_init.delta_slope_coeff_exp_shgi,
			       cmd->rf_init.delta_slope_coeff_man_shgi,
			       cmd->rf_init.finiteLoopCount);

		resp->len = sizeof(struct ar9170_rf_init_result);
		resp->rf_init_res.ret = ret;
		resp->rf_init_res.regs[0] = *(volatile u32_t *)(0x1bc000 + 0x9800 + (25 << 2));
		resp->rf_init_res.regs[1] = *(volatile u32_t *)(0x1bc000 + 0xa800 + (25 << 2));
		resp->rf_init_res.regs[2] = *(volatile u32_t *)(0x1bc000 + 0xb800 + (25 << 2));
		resp->rf_init_res.regs[3] = *(volatile u32_t *)(0x1bc000 + 0x9800 + (111 << 2));
		resp->rf_init_res.regs[4] = *(volatile u32_t *)(0x1bc000 + 0xa800 + (111 << 2));
		resp->rf_init_res.regs[5] = *(volatile u32_t *)(0x1bc000 + 0xb800 + (111 << 2));
		break;
	case ZM_CMD_FREQUENCY:
		zgDYNAMIC_HT2040_EN = cmd->rf_init.dynht2040;
		zgHT_ENABLE = cmd->rf_init.ht_settings & 1;
		zgExtOffset = cmd->rf_init.ht_settings >> 2;

		ret = zfSetChannel(cmd->rf_init.freq,
				   cmd->rf_init.delta_slope_coeff_exp,
				   cmd->rf_init.delta_slope_coeff_man,
				   cmd->rf_init.delta_slope_coeff_exp_shgi,
				   cmd->rf_init.delta_slope_coeff_man_shgi,
				   cmd->rf_init.finiteLoopCount);

		resp->len = sizeof(struct ar9170_rf_init_result);
		resp->rf_init_res.ret = ret;
		resp->rf_init_res.regs[0] = *(volatile u32_t *)(0x1bc000 + 0x9800 + (25 << 2));
		resp->rf_init_res.regs[1] = *(volatile u32_t *)(0x1bc000 + 0xa800 + (25 << 2));
		resp->rf_init_res.regs[2] = *(volatile u32_t *)(0x1bc000 + 0xb800 + (25 << 2));
		resp->rf_init_res.regs[3] = *(volatile u32_t *)(0x1bc000 + 0x9800 + (111 << 2));
		resp->rf_init_res.regs[4] = *(volatile u32_t *)(0x1bc000 + 0xa800 + (111 << 2));
		resp->rf_init_res.regs[5] = *(volatile u32_t *)(0x1bc000 + 0xb800 + (111 << 2));
		break;
	case ZM_CMD_FREQ_STRAT:
		zfNotifySetChannel();
		resp->len = 0;
		break;
	case ZM_CMD_EKEY:
		/* for 64 to 16 user space */
		if (cmd->setkey.user > (MAX_USER + 3))
			cmd->setkey.user = MAX_USER;

		if ((cmd->setkey.user > (MAX_USER + 3)) || (cmd->setkey.keyId > 1)) {
			resp->len = 1;
		} else {
			resp->len = 1;

			/* Disable Key */
			zfDisableCamUser(cmd->setkey.user);

			/* Set encrypt type */
			if (cmd->setkey.user <= (MAX_USER + 3)) {
				if (cmd->setkey.user >= MAX_USER) {
					/* default */
					row = DEFAULT_ENCRY_TYPE;
					wordId = 0;
					nibbleId = (cmd->setkey.user - MAX_USER) & 0x7;
				} else {
					row = ENCRY_TYPE_START_ADDR + (cmd->setkey.user >> 5);
					wordId = (cmd->setkey.user >> 3) & 0x3;
					nibbleId = cmd->setkey.user & 0x7;
				}

				HW_CAM_Read128(row, data);
				data[wordId] &= (~(0xf << ((u32_t) nibbleId * 4)));
				data[wordId] |= (cmd->setkey.type << ((u32_t) nibbleId * 4));
				HW_CAM_Write128(row, data);
			}

			/* Set MAC address */
			if (cmd->setkey.user < MAX_USER) {
				wordId = (cmd->setkey.user >> 2) & 0x3;
				byteId = cmd->setkey.user & 0x3;
				row = (cmd->setkey.user >> 4) * 6;

				for (i = 0; i < 6; i++) {
					HW_CAM_Read128(row + i, data);
					data[wordId] &= (~(0xff << ((u32_t) byteId * 8)));
					data[wordId] |= (cmd->setkey.macAddr[i] << ((u32_t) byteId * 8));
					HW_CAM_Write128(row + i, data);
				}
			}

			/* Set key */
			row = KEY_START_ADDR + (cmd->setkey.user * 2) + cmd->setkey.keyId;

			for (i = 0; i < 4; i++)
				data[i] = cmd->setkey.key[i];

			HW_CAM_Write128(row, data);

			/* Enable Key */
			zfEnableCamUser(cmd->setkey.user);
		}
		break;
	case ZM_CMD_DKEY:
		/* Disable Key */
		resp->len = 0;
		zfDisableCamUser(cmd->disablekey.idx);
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

		resp->len = sizeof(struct ar9170_tally);
		resp->tally.tx_underrun = zgTally.TxUnderRun;
		resp->tally.rx_total_cnt = zgTally.RxTotalCount;
		resp->tally.rx_crc32 = zgTally.RxCRC32;
		resp->tally.rx_crc16 = zgTally.RxCRC16;
		resp->tally.rx_unicast_error = zgTally.RxUniError;
		resp->tally.rx_overrun = zgTally.RxOverRun;
		resp->tally.rx_mul_error = zgTally.RxMulError;
		resp->tally.tx_retry_cnt = zgTally.TxRetryCount;
		resp->tally.tx_total_cnt = zgTally.TxTotalCount;
		resp->tally.rx_timeout = zgTally.RxTimeOut;
		resp->tally.agg_tx_cnt = zgTally.AggTxCnt;
		resp->tally.ba_fail_cnt = zgTally.BAFailCnt;
		resp->tally.tx_ampdu_cnt = zgTally.TxAmpduCnt;
		resp->tally.tx_mpdu_cnt = zgTally.TxMpduCnt;

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
		resp->len = sizeof(struct ar9170_tally_apd);
		resp->tally_apd.rx_mpdu = zgTally.RxMPDU;
		resp->tally_apd.rx_drop_mpdu = zgTally.RxDropMPDU;
		resp->tally_apd.rx_del_mpdu = zgTally.RxDelMPDU;
		resp->tally_apd.rx_phy_misc_error = zgTally.RxPhyMiscError;
		resp->tally_apd.rx_phy_xr_error = zgTally.RxPhyXRError;
		resp->tally_apd.rx_phy_ofdm_error = zgTally.RxPhyOFDMError;
		resp->tally_apd.rx_phy_cck_error = zgTally.RxPhyCCKError;
		resp->tally_apd.rx_phy_ht_error = zgTally.RxPhyHTError;
		resp->tally_apd.rx_phy_total_cnt = zgTally.RxPhyTotalCount;

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
		resp->len = 0;
		zgRxMaxSize = cmd->cfg.max_rx_size;
		zgDontRetransmit = cmd->cfg.dont_retransmit;
		break;
	case ZM_CMD_SWRST:
		zfWorkAround_SwReset();
		resp->len = 0;
		break;
	case ZM_CMD_FW_RETRY:
		resp->len = 0;
		zgEnableFwRetry = cmd->fw_retry.fw_retry;
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
		for (i = 0; i < cmd->len / 8; i++) {
			val = cmd->wreg.regs[i].val;

			for (j = 0; j < 4; j++) {
				*((u8_t *)cmd->wreg.regs[i].addr + j) = val;
				val = val >> 8;
			}
		}
		}
		break;
	case ZM_CMD_FW_DL_INIT:
		/* Software Reset, before we use new downloaded SPI firmware */
		val = *(volatile u32_t *)(0x1c3500);
		val |= 0x20;

		*(volatile u32_t *)(0x1c3500) = val;

		/* Clsoe Watch dog control(_ZF_WATCH_DOG) */
		// disable Watch dog
		ZM_WATCH_DOG_REG = 0xFFFF;

		u8Watchdog_Enable = 0;

		resp->len = cmd->len;
		break;
	default:
		break;
	}
}
