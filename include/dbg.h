#ifndef __DBG
#define __DBG

void morse(const char *txt);
void usb_dbg(const char *txt);
void usb_dbg8(u8_t v);
void usb_dbg16(u16_t v);
void usb_dbg32(u32_t v);
void usb_dbg_hex(int buflen, const u8_t *buf);

#endif	/* __DBG */
