#ifndef _CMD_DEFS_H
#define _CMD_DEFS_H

#define ZM_CMD_CODE         0x0F5A



#define ZM_CMD_ECHO         0x80
#define ZM_CMD_TALLY        0x81
#define ZM_CMD_TALLY_APD    0x82
#define ZM_CMD_CONFIG       0x83
#define ZM_CMD_RREG         0x00
#define ZM_CMD_WREG         0x01
#define ZM_CMD_RMEM         0x02
#define ZM_CMD_WMEM         0x03
#define ZM_CMD_BITAND       0x04
#define ZM_CMD_BITOR        0x05
#define ZM_CMD_EKEY         0x28
#define ZM_CMD_DKEY         0x29
#define ZM_CMD_FREQUENCY    0x30
#define ZM_CMD_RF_INIT      0x31
#define ZM_CMD_FREQ_STRAT   0x33
#define ZM_CMD_SWRST        0x90
#define ZM_CMD_DKRST        0x91
#define ZM_CMD_FW_RETRY     0x92
/*
 * ZM_CMD_REBOOT differs between STA and AP,
 * refer to sta.h and ap.h
 */ 
#define ZM_CMD_FDC          0xA0
#define ZM_CMD_WREEPROM     0xB0
#define ZM_CMD_FW_DL_INIT   0xB5
#endif
