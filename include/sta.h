#ifndef _STA_H
#define _STA_H

 /* pci.c on STA */
extern void zfLinkTest(void);

extern u32_t reg_read_PHY(u32_t addr);

#define ZM_CMD_REBOOT		0x94

/* stream mode setting
 *  hpfwu_2k.c             None
 *  hpfwu.c                    Rx
 *  hpfwu_txstream.c      Tx / Rx
 *
 *  2k     hpfwu_2k.c
 *  xp     hpfwu_txstream.c
 *  vista  hpfwu_txstream.c
 *  linux  hpfwu.c
 */
#define USB_TX_STREAM_MODE	1
#define USB_RX_STREAM_MODE	1

#define ZM_TX_MAX_HANG_NUM	2
#define ZM_TX_MAX_HANG_NUM_HAV	32

#define ZM_BB_RESET		1

/* BAR fix with Marvell */
#define ZM_BAR_AUTO_BA		1
extern struct zsDmaDesc* zgBADesc;
extern u8_t zgBAAvailable;
extern u8_t zgBARGotState;
extern u32_t zgBARTimeOut;


#endif /* #ifndef _STA_H */
