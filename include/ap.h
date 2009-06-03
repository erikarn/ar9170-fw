#ifndef _AP_FW_H
#define _AP_FW_H

extern void zfRecycleRTQ(void);

extern u32_t zfLengthConfirm(struct zsDmaDesc* desc);

/* driver */
extern void zfQuePutPacket(struct zsDmaQueue* q, struct zsDmaDesc* desc);
extern struct zsDmaDesc* zfQueGetPacket(struct zsDmaQueue* q);
extern u8_t zfCheckAGP(struct zsDmaDesc* desc);
extern u8_t zfCheckAGG(struct zsDmaDesc* desc);

/* hostif.c for AP */
extern void zfSendTUFromDnQToTxQ(u8_t bSinglePacket);

/* wlan.c */
void zfTxSchedulerTask(void);

/* USB_Boot.c */
extern void zfGenUsbInterrupt(u8_t rspLen, u8_t rspType, u16_t rspSeq, u8_t *body);
 
#define ZM_CMD_REBOOT		0x93

/* stream mode setting */
#define USB_TX_STREAM_MODE      0
#define USB_RX_STREAM_MODE      0

#define ZM_TX_MAX_HANG_NUM        4//2
#define ZM_TX_MAX_HANG_NUM_HAV   8//32

#define ZM_BB_RESET             0

/* Use Queue 1 to simulate the CABQ */
#define ZM_CABQ_SUPPORT         1

/* BAR fix with Marvell */
#define ZM_BAR_AUTO_BA				 0

#endif /* #ifndef _AP_FW_H */

