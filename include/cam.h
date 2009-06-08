#ifndef __CAM
#define __CAM

#include "dt_defs.h"

#define MAX_USER		64
#define MAX_KEY_LENGTH		16      // no use
#define ENCRY_TYPE_START_ADDR	24
#define DEFAULT_ENCRY_TYPE	26
#define KEY_START_ADDR		27
#define STA_KEY_START_ADDR	155
#define COUNTER_START_ADDR      163
#define STA_COUNTER_START_ADDR	165

extern void zfDisableCamUser(u16_t userId);
extern void zfEnableCamUser(u16_t userId);
extern void zfWaitForCamReadReady(void);
extern void zfWaitForCamWriteReady(void);
extern void HW_CAM_Avail(void);
extern void HW_CAM_Write128(const u32_t address, const u32_t *data);
extern void HW_CAM_Read128(const u32_t address, u32_t *data);

#endif /* __CAM */
