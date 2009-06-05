#include "reg_defs.h"

/* XXX */
extern void zfGenUsbInterrupt(u8_t rspLen, u8_t rspType, u16_t rspSeq, u8_t *body);

#define M(n, c1, c2, c3, c4, c5) ((n) | ((c1)<<3) | ((c2)<<4) | ((c3)<<5) | ((c4)<<6) | ((c5)<<7))

void morse(const char *txt)
{
	static const unsigned char patterns['Z' - 'A' + 1] = {
		['A' - 'A'] = M(2, 0, 1, 0, 0, 0),
		['B' - 'A'] = M(4, 1, 0, 0, 0, 0),
		['C' - 'A'] = M(4, 1, 0, 1, 0, 0),
		['D' - 'A'] = M(3, 1, 0, 0, 0, 0),
		['E' - 'A'] = M(1, 0, 0, 0, 0, 0),
		['F' - 'A'] = M(4, 0, 0, 1, 0, 0),
		['G' - 'A'] = M(3, 1, 1, 0, 0, 0),
		['H' - 'A'] = M(4, 0, 0, 0, 0, 0),
		['I' - 'A'] = M(2, 0, 0, 0, 0, 0),
		['J' - 'A'] = M(4, 0, 1, 1, 1, 0),
		['K' - 'A'] = M(3, 1, 0, 1, 0, 0),
		['L' - 'A'] = M(4, 0, 1, 0, 0, 0),
		['M' - 'A'] = M(2, 1, 1, 0, 0, 0),
		['N' - 'A'] = M(2, 1, 0, 0, 0, 0),
		['O' - 'A'] = M(3, 1, 1, 1, 0, 0),
		['P' - 'A'] = M(4, 0, 1, 1, 0, 0),
		['Q' - 'A'] = M(4, 1, 1, 0, 1, 0),
		['R' - 'A'] = M(3, 0, 1, 0, 0, 0),
		['S' - 'A'] = M(3, 0, 0, 0, 0, 0),
		['T' - 'A'] = M(1, 1, 0, 0, 0, 0),
		['U' - 'A'] = M(3, 0, 0, 1, 0, 0),
		['V' - 'A'] = M(4, 0, 0, 0, 1, 0),
		['W' - 'A'] = M(3, 0, 1, 1, 0, 0),
		['X' - 'A'] = M(4, 1, 0, 0, 1, 0),
		['Y' - 'A'] = M(4, 1, 0, 1, 1, 0),
		['Z' - 'A'] = M(4, 1, 1, 0, 0, 0),
	};
	unsigned int i, k;
	const char *tmp = txt;
	unsigned int base = 200000;

	for (k = 0; k < base; k++)
		ZM_GPIO_PORT_DATA_REG = 0;

	while (*tmp) {
		for (k = 0; k < base; k++)
			ZM_GPIO_PORT_DATA_REG = 0;

		if (*tmp >= 'A' && *tmp <= 'Z') {
			int idx = *tmp - 'A';
			int dly;

			for (i = 0; i < (patterns[idx] & 7); i++) {
				if (patterns[idx] & ((1 << 3)<< i))
					dly = 3 * base;
				else
					dly = base;

				for (k = 0; k < dly; k++)
					ZM_GPIO_PORT_DATA_REG = 3;

				for (k = 0; k < base; k++)
					ZM_GPIO_PORT_DATA_REG = 0;
			}
		}
		tmp++;
	}

	for (k = 0; k < base; k++)
		ZM_GPIO_PORT_DATA_REG = 0;
}

void usb_dbg(const char *txt)
{
	const char *tmp = txt;
	while (*tmp)
		tmp++;
	zfGenUsbInterrupt((tmp - txt + 3) & ~0x3, 0xca, 0x80, (u8_t *)txt);
}

void usb_dbg8(u8_t v)
{
	zfGenUsbInterrupt(1, 0xcb, 0x80, (u8_t *)&v);
}

void usb_dbg16(u16_t v)
{
	zfGenUsbInterrupt(2, 0xcb, 0x80, (u8_t *)&v);
}

void usb_dbg32(u32_t v)
{
	zfGenUsbInterrupt(4, 0xcb, 0x80, (u8_t *)&v);
}

void usb_dbg_hex(int buflen, const u8_t *buf)
{
	zfGenUsbInterrupt((buflen + 3) & ~0x3, 0xcc, 0x80, (u8_t *)buf);
}
