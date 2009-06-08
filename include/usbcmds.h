#ifndef __USBCMDS
#define __USBCMDS
#include "compiler.h"

#define ZM_CMD_ECHO		0x80
#define ZM_CMD_TALLY		0x81
#define ZM_CMD_TALLY_APD	0x82
#define ZM_CMD_CONFIG		0x83
#define ZM_CMD_RREG		0x00
#define ZM_CMD_WREG		0x01
#define ZM_CMD_RMEM		0x02
#define ZM_CMD_WMEM		0x03
#define ZM_CMD_BITAND		0x04
#define ZM_CMD_BITOR		0x05
#define ZM_CMD_EKEY		0x28
#define ZM_CMD_DKEY		0x29
#define ZM_CMD_FREQUENCY	0x30
#define ZM_CMD_RF_INIT		0x31
#define ZM_CMD_FREQ_STRAT	0x33
#define ZM_CMD_SWRST		0x90
#define ZM_CMD_DKRST		0x91
#define ZM_CMD_FW_RETRY		0x92
#define ZM_CMD_REBOOT		0x94
#define ZM_CMD_FDC		0xA0
#define ZM_CMD_WREEPROM		0xB0
#define ZM_CMD_FW_DL_INIT	0xB5

struct ar9170_set_key_cmd {
	u16_t	user;
	u16_t	keyId;
	u16_t	type;
	u8_t	macAddr[6];
	u32_t	key[4];
};

struct ar9170_disable_key_cmd {
	u16_t	idx;
	u16_t	padding;
};

struct ar9170_u32_list {
	u32_t	vals[0];
};

struct ar9170_reg_list {
	u32_t	*regs[0];
};

struct ar9170_write_reg {
	struct {
		u32_t	*addr;
		u32_t	val;
	} regs[0];
};

struct ar9170_rf_init {
	u32_t	freq;
	u8_t	dynht2040;
	u8_t	padding1[3];
	u8_t	ht_settings; /* 0x1: ht enable, 0xfc: ext offset */
	u8_t	padding2[3];
	u32_t	delta_slope_coeff_exp;
	u32_t	delta_slope_coeff_man;
	u32_t	delta_slope_coeff_exp_shgi;
	u32_t	delta_slope_coeff_man_shgi;
	u32_t	finiteLoopCount;
};

struct ar9170_rf_init_result {
	u32_t	ret;
	u32_t	regs[6];
};

struct ar9170_tally {
	u32_t	tx_underrun,
		rx_total_cnt,
		rx_crc32,
		rx_crc16,
		rx_unicast_error,
		rx_overrun,
		rx_mul_error,
		tx_retry_cnt,
		tx_total_cnt,
		rx_timeout,
		agg_tx_cnt,
		ba_fail_cnt,
		tx_ampdu_cnt,
		tx_mpdu_cnt;
};

struct ar9170_tally_apd {
	u32_t	rx_mpdu,
		rx_drop_mpdu,
		rx_del_mpdu,
		rx_phy_misc_error,
		rx_phy_xr_error,
		rx_phy_ofdm_error,
		rx_phy_cck_error,
		rx_phy_ht_error,
		rx_phy_total_cnt;
};

struct ar9170_config {
	u32_t	max_rx_size;
	u8_t	dont_retransmit;
	u8_t	padding[3];
};

struct ar9170_fw_retry {
	u8_t	fw_retry;
	u8_t	padding[3];
};

struct ar9170_cmd {
	u8_t	len;
	u8_t	cmd;
	u16_t	padding;
	union {
		struct ar9170_set_key_cmd	setkey;
		struct ar9170_disable_key_cmd	disablekey;
		struct ar9170_u32_list		echo;
		struct ar9170_reg_list		rreg;
		struct ar9170_u32_list		rreg_res;
		struct ar9170_write_reg		wreg;
		struct ar9170_rf_init		rf_init;
		struct ar9170_rf_init_result	rf_init_res;
		struct ar9170_tally		tally;
		struct ar9170_tally_apd		tally_apd;
		struct ar9170_config		cfg;
		struct ar9170_fw_retry		fw_retry;
	};
};

static inline void __check(void) {
	BUILD_BUG_ON(sizeof(struct ar9170_set_key_cmd) != 28);
	BUILD_BUG_ON(sizeof(struct ar9170_rf_init_result) != 28);
	BUILD_BUG_ON(sizeof(struct ar9170_tally) != 56);
	BUILD_BUG_ON(sizeof(struct ar9170_tally_apd) != 36);
}

#endif
