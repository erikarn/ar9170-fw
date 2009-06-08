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
} __packed;

struct ar9170_cmd {
	u8_t	len;
	u8_t	cmd;
	u16_t	padding;
	union {
		struct ar9170_set_key_cmd setkey;
	} __packed;
} __packed;

#endif
