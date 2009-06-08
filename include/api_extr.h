#ifndef _DESC_EXTR_H
#define _DESC_EXTR_H

/* desc.c */
extern void zfDmaInitDescriptor(void);
extern struct zsDmaDesc* zfDmaGetPacket(struct zsDmaQueue* q);
extern void zfDmaReclaimPacket(struct zsDmaQueue* q, struct zsDmaDesc* desc);
extern void zfDmaPutPacket(struct zsDmaQueue* q, struct zsDmaDesc* desc);
extern void zfRecycleTxQ(u8_t qNum);
extern void zfRecycleRxQ(void);

/* cmd.c */
extern void zfCmdHandler(void);
extern void zfSwReset(void);

/* db.c */
#if ZM_UART
	extern void zfUartInit(void);
	extern void zfDebugTask(void);
#endif

/* pci.c on STA / hostif.c for AP */
extern void zfHostIfIsr(void);

/* usb.c */
extern void zfPtaModeInit(void);

/* wlan.c */
void zfWlanIsr(void);

/* cphy.c */
//void zfInitPhy(u32_t tableNo);
//void zfInitRf(void);
u16_t zfInitRf(u32_t frequency,
              u32_t delta_slope_coeff_exp,
              u32_t delta_slope_coeff_man,
              u32_t delta_slope_coeff_exp_shgi,
              u32_t delta_slope_coeff_man_shgi,
			  u32_t finiteLoopCount);
u16_t zfSetChannel(u32_t frequency,
                    u32_t delta_slope_coeff_exp,
                    u32_t delta_slope_coeff_man,
                    u32_t delta_slope_coeff_exp_shgi,
	              	u32_t delta_slope_coeff_man_shgi,
				  	u32_t finiteLoopCount);
extern void zfNotifySetChannel(void);

#endif /* #ifndef _DESC_EXTR_H */
