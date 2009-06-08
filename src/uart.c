/*
 * Copyright (c) 2000-2005  ZyDAS Technology Corporation
 * Copyright	2009	Johannes Berg
 *
 * UART debug interface functions.
 */
#include "dt_defs.h"
#include "reg_defs.h"
#include "sys_defs.h"
#include "uart_extr.h"

void zfPutChar(u8_t ch)
{
	ZM_HUART_TX_HOLDING_REG = ch;

	while ((ZM_HUART_LINE_STATUS_REG & ZM_TX_FIFO_ALMOST_EMPTY_BIT) == 0) {
	}
	return;
}

void zfCuartOut(u8_t * strPtr, u32_t len)
{
	u32_t i;

	for (i = 0; i < len; i++) {
		zfPutChar(strPtr[i]);
	}
}

void zfUartSendStr(u8_t * str)
{
	u32_t i = 0;

	while (str[i] != 0) {
		i++;
	}

	zfCuartOut(str, i);
}

void zfItoa16(u32_t hexValue, u8_t * str)
{
	s32_t i;
	u32_t nibble;
	for (i = 0; i < 8; i++) {
		nibble = hexValue & 0xf;
		hexValue = hexValue >> 4;
		str[7 - i] = nibble + ((nibble < 10) ? '0' : ('A' - 10));
	}
	str[8] = 0;
}

void zfUartSendHex(u32_t hex)
{
	u8_t str[20];

	zfItoa16(hex, str);
	zfUartSendStr(str);
}

void zfUartSendHex8(u32_t hex)
{
	u8_t str[20];

	zfItoa16(hex, str);
	zfUartSendStr(str + 6);
}

void zfUartSendStrAndHex(u8_t * str, u32_t val)
{
	u8_t val_str[20];

	zfItoa16(val, val_str);
	zfUartSendStr(str);
	zfUartSendStr(val_str);
	zfUartSendStr((u8_t *) "\n");
}
