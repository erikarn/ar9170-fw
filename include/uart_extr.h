#ifndef _CUART_DEFS_H
#define _CUART_DEFS_H

extern u16_t zfGetChar(u32_t ms);
extern void zfPutChar(u8_t ch);
extern void zfCuartOut(u8_t* strPtr, u32_t len);
extern void zfUartSendStr(u8_t* str);
extern int zfUartSendHex(u32_t hex);
extern int zfUartSendHex8(u32_t hex);
extern void zfItoa16(u32_t hexValue, u8_t* str);
extern void zfUartSendStrAndHex(u8_t* str, u32_t val);

#endif

