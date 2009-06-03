#ifndef _WLAN_DEFS_H
#define _WLAN_DEFS_H

#include "dt_defs.h"

#define ZM_DOWN_DESC_NUM            8
#define ZM_TX_DESC_NUM              8

#define ZM_DECS_DATA_SIZE           256

/* STATUS */
#define ZM_OWN_BITS_MASK            0xFFFC
#define ZM_OWN_SW                   0x0000
#define ZM_OWN_DMA                  0x0001
#define ZM_OWN_SE                   0x0002

/* CONTROL */
#define ZM_LS_BIT                   0x0002
#define ZM_FS_BIT                   0x0004

struct zsDescriptor
{
    u16_t status;
    u16_t ctrl;
    u16_t dataSize;
    u16_t totalLen;
    u32_t lastAddr;
    u32_t dataAddr;
    u32_t nextAddr;
};

#endif