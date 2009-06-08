/*
 * Copyright (c) 2000-2005  ZyDAS Technology Corporation
 * Copyright	2009	Johannes Berg <johannes@sipsolutions.net>
 *
 *
 * PHY and RF functions
 */

#include "dt_defs.h"
#include "gv_extr.h"
#include "uart_extr.h"
#include "reg_defs.h"

extern void zfRecycleTxQ(u8_t qNum);
extern void zfRecycleRxQ(void);

#define zfwReadReg(addr) (*(volatile u32_t*)(addr))
#define zfwWriteReg(addr, val) (*(volatile u32_t*)(addr) = val)

//#define reg_read(addr) zfwReadReg(addr+0x1bc000)
#define reg_write(addr, val) zfwWriteReg(addr+0x1bc000, val)

void zfHwSaveChannel(u32_t frequency);
void zfNotifySetChannel(void);

void zfSetChannelStart(void);
void zfSetChannelEnd(void);

void zfDelayUs(u32_t usec);

void zfHwTurnOffdyn(u32_t delta_slope_coeff_exp,
		    u32_t delta_slope_coeff_man,
		    u32_t delta_slope_coeff_exp_shgi,
		    u32_t delta_slope_coeff_man_shgi);
void zfProgramADDAC(void);
void zfHwSwapChain0andChain2(void);
void zfHwConfigureMask(void);
void zfHwHTEnable(void);
void zfActiveBB(void);
u16_t zfHwNoiseCalibration(void);
u16_t zfAGCCalibration(u32_t finiteLoopCount);

extern u8_t zg5GhzFreq;

u32_t reg_read_PHY(u32_t addr)
{
	u8_t i = 0;
	u32_t readVal, tmpVal;

	readVal = zfwReadReg(addr);
	while (i < 10) {
		i++;
		tmpVal = zfwReadReg(addr);
		if (tmpVal == readVal)
			break;
		else
			readVal = tmpVal;
	}
	return readVal;
}

#define reg_read(addr) reg_read_PHY(addr+0x1bc000)

u32_t zgGetClockCounter(void)
{
	u32_t t;

	t = ZM_CLOCK_HIGH_REG;
	t = (t << 16) + ZM_CLOCK_LOW_REG;
	return t;
}

void zfDelayMs(u32_t msec)
{
	u32_t t1, t2, dt;

	t1 = zgGetClockCounter();
	while (1) {
		t2 = zgGetClockCounter();
		dt = ((u32_t) ((u32_t) t2 - (u32_t) t1)) /
		    ZM_CLOCK_PER_MICORSECOND / 1000;
		if (dt >= msec) {
			return;
		}
	}
}

void wait_on_time(u32_t msec)
{
	zfDelayMs(msec);
}

void zfWaitTxCtlSetClear(void)
{
	/* Tx Q0~4 bit9~bit4 : Number of Q control setting stored in buffer. */
	while (*(volatile u32_t *)(0x1c3b88) & 0x3f0) {
		zfUartSendStrAndHex((u8_t *) "Q0:  ",
				    *(volatile u32_t *)(0x1c3b88));
	}
	while (*(volatile u32_t *)(0x1c3b8c) & 0x3f0) {
		zfUartSendStrAndHex((u8_t *) "Q1:  ",
				    *(volatile u32_t *)(0x1c3b8c));
	}
	while (*(volatile u32_t *)(0x1c3b90) & 0x3f0) {
		zfUartSendStrAndHex((u8_t *) "Q2:  ",
				    *(volatile u32_t *)(0x1c3b90));
	}
	while (*(volatile u32_t *)(0x1c3b94) & 0x3f0) {
		zfUartSendStrAndHex((u8_t *) "Q3:  ",
				    *(volatile u32_t *)(0x1c3b94));
	}
	while (*(volatile u32_t *)(0x1c3b98) & 0x3f0) {
		zfUartSendStrAndHex((u8_t *) "Q4:  ",
				    *(volatile u32_t *)(0x1c3b98));
	}
}

u16_t zfInitRf(u32_t frequency,
	       u32_t delta_slope_coeff_exp,
	       u32_t delta_slope_coeff_man,
	       u32_t delta_slope_coeff_exp_shgi,
	       u32_t delta_slope_coeff_man_shgi, u32_t finiteLoopCount)
{
	//zfUartSendStr((u8_t*)"zfInitRf\n");
	u16_t ret;

	/* stroe frequency */
	zfHwSaveChannel(frequency);

	zfHwTurnOffdyn(delta_slope_coeff_exp,
		       delta_slope_coeff_man,
		       delta_slope_coeff_exp_shgi, delta_slope_coeff_man_shgi);

	/* Real Chip */
	zfProgramADDAC();
	zfHwSwapChain0andChain2();

	zfHwConfigureMask();
	zfHwHTEnable();
	zfActiveBB();
	ret = zfAGCCalibration(finiteLoopCount);

	zfSetChannelEnd();

	//zfUartSendStr((u8_t*)"zfInitRf done\n");
	return ret;
}

void zfHwSaveChannel(u32_t frequency)
{
	if (frequency < 2484000) {
		/* Set WLAN mode to 802.11bg */
		zg5GhzFreq = 0;
	} else {
		/* Set WLAN mode to 802.11a */
		zg5GhzFreq = 1;
	}

	/* store frequency */
	zgFrequency = frequency;
}

void zfNotifySetChannel(void)
{
	zfSetChannelStart();
}

u16_t zfSetChannel(u32_t frequency,
		   u32_t delta_slope_coeff_exp,
		   u32_t delta_slope_coeff_man,
		   u32_t delta_slope_coeff_exp_shgi,
		   u32_t delta_slope_coeff_man_shgi, u32_t finiteLoopCount)
{
	u16_t ret;

	zfHwSaveChannel(frequency);

	zfHwTurnOffdyn(delta_slope_coeff_exp,
		       delta_slope_coeff_man,
		       delta_slope_coeff_exp_shgi, delta_slope_coeff_man_shgi);

	ret = zfAGCCalibration(finiteLoopCount);

	zfSetChannelEnd();
	return ret;
}

void zfSetChannelStart(void)
{
#if 1
	/* Manipulate CCA threshold to stop transmission */
	*(volatile u32_t *)(0x1c5968) = 0x300;
	/* Enable Virtual CCA */
	*(volatile u32_t *)(0x1c3b40) = 0xf8000;
#else
	/* TS/TC select */
	*(volatile u32_t *)(0x1c3b3c) |= 0x1f0000;
#endif

}

void zfSetChannelEnd(void)
{
#if 1
	/* Manipulate CCA threshold to resume transmission */
	*(volatile u32_t *)(0x1c5968) = 0x000;
	/* Disable Virtual CCA */
	*(volatile u32_t *)(0x1c3b40) = 0x0000;
#else
	/* TS/TC select */
	*(volatile u32_t *)(0x1c3b3c) &= ~(u32_t) (0x1f0000);
#endif
}

void zfDKReSetStart(void)
{
	u32_t val;

	wait_on_time(10);

	zfWaitTxCtlSetClear();

	//Warm reset BB/ADDA
	*(volatile u32_t *)(0x1d4004) = 0x400;
	*(volatile u32_t *)(0x1d4004) = 0x0;

	wait_on_time(10);

	/* TX/RX must be stopped by now */
	val = *(volatile u32_t *)(0x1c3500);
	val |= 0x20;

	*(volatile u32_t *)(0x1c3500) = val;
}

void zfDKReSetEnd(void)
{
	/* Restart DMA */
	zfRecycleTxQ(0);
	zfRecycleRxQ();
}

/*
 * Update delta slope coeff man and exp
 */
void zfHwTurnOffdyn(u32_t delta_slope_coeff_exp,
		    u32_t delta_slope_coeff_man,
		    u32_t delta_slope_coeff_exp_shgi,
		    u32_t delta_slope_coeff_man_shgi)
{
	u32_t rddata;
	u32_t wrdata;

	rddata = reg_read(0x9814);
	wrdata = (rddata & 0x00001fff) | (delta_slope_coeff_man << 17) |
	    (delta_slope_coeff_exp << 13);
	reg_write(0x9814, wrdata);

	wrdata = (delta_slope_coeff_man_shgi << 4) | delta_slope_coeff_exp_shgi;
	reg_write(0x9800 + (116 << 2), wrdata);
}

void zfProgramADDAC()
{
	reg_write(0x9800 + (12 << 2), 0x1);	//# Select Internal ADDAC
	wait_on_time(10);

	reg_write(0x989c, 0x00000000);	//# 7-0
	reg_write(0x989c, 0x00000000);	//# 15-8
	reg_write(0x989c, 0x00000000);	//# 23-16
	reg_write(0x989c, 0x00000000);	//# 31-
	reg_write(0x989c, 0x00000000);	//# 39-
	reg_write(0x989c, 0x00000000);	//# 47-
	reg_write(0x989c, 0x00000000);	//# 55- [48]:doubles the xtalosc bias current
	reg_write(0x989c, 0x00000000);	//# 63-
	reg_write(0x989c, 0x00000000);	//# 71-
	reg_write(0x989c, 0x00000000);	//# 79-
	reg_write(0x989c, 0x00000000);	//# 87-
	reg_write(0x989c, 0x00000000);	//# 95-
	reg_write(0x989c, 0x00000000);	//# 103-
	reg_write(0x989c, 0x00000000);	//# 111-
	reg_write(0x989c, 0x00000000);	//# 119-
	reg_write(0x989c, 0x00000000);	//# 127-
	reg_write(0x989c, 0x00000000);	//# 135-
	reg_write(0x989c, 0x00000000);	//# 143-
	reg_write(0x989c, 0x00000000);	//# 151-
	reg_write(0x989c, 0x00000030);	//# 159- #[158:156]=xlnabufmode
	reg_write(0x989c, 0x00000004);	//# 167-  [162]:disable clkp_driver to fowl
	reg_write(0x989c, 0x00000000);	//# 175-
	reg_write(0x989c, 0x00000000);	//# 183-176
	reg_write(0x989c, 0x00000000);	//# 191-184
	reg_write(0x989c, 0x00000000);	//# 199-
	reg_write(0x989c, 0x00000000);	//# 207-
	reg_write(0x989c, 0x00000000);	//# 215-
	reg_write(0x989c, 0x00000000);	//# 223-
	reg_write(0x989c, 0x00000000);	//# 231-
	reg_write(0x98c4, 0x00000000);	//# 233-232

	wait_on_time(10);
	reg_write(0x9800 + (12 << 2), 0x0);	//# Select External Fowl
}

void zfHwSwapChain0andChain2()
{
	u32_t Q_inv;

	/* inverse */
	Q_inv = 0x1;
	reg_write(0x9800 + (666 << 2), Q_inv);

	/* Swap Chain 0 and Chain 2 */
	reg_write(0x9800 + (666 << 2), (Q_inv | (1 << 6)));
}

void zfHwConfigureMask()
{
	reg_write(0x99a4, 0x5);
	reg_write(0xa39c, 0x5);
}

void zfHwHTEnable()
{
#if 0				/* compatibility for UB83 */
	/* Temp setting for bringup */
	//reg_write(0x9804, 0x0);         //# Legacy

	if (zgHT_ENABLE == 1) {
		if (zgDYNAMIC_HT2040_EN == 1) {
			if (zgExtOffset == 1) {
				reg_write(0x9804, 0x3d4);	//3d4 for real

			} else {
				reg_write(0x9804, 0x3c4);	//3c4 for real
			}
			//# Dyn HT2040.Refer to Reg 1.
			//#[3]:single length (4us) 1st HT long training symbol; use Walsh spatial spreading for 2 chains 2 streams TX
			//#[c]:allow short GI for HT40 packets; enable HT detection.
			//#[4]:enable 20/40 MHz channel detection.
		} else {
			reg_write(0x9804, 0x340);
			//# Static HT20
			//#[3]:single length (4us) 1st HT long training symbol; use Walsh spatial spreading for 2 chains 2 streams TX
			//#[4]:Otus don't allow short GI for HT20 packets yet; enable HT detection.
			//#[0]:disable 20/40 MHz channel detection.
		}
	} else {

		reg_write(0x9804, 0x0);
		//# Legacy;# Direct Mapping for each chain.
		//#Be modified by Oligo to add dynanic for legacy.

		if (zgDYNAMIC_HT2040_EN == 1) {
			reg_write(0x9804, 0x4);	//# Dyn Legacy .Refer to reg 1.
		} else {
			reg_write(0x9804, 0x0);	//# Static Legacy
		}
	}

	wait_on_time(10);
#endif				/* end of compatibility for UB83 */
}

void zfActiveBB()
{
	reg_write(0x981c, 0x00000001);	// # Activate BB
	wait_on_time(10);
	/* force ADC alive */
//    reg_write(0x99a4, 0x00000001);
//    wait_on_time(10);
//    reg_write(0x99a4, 0x00000002);
//    wait_on_time(10);
//    reg_write(0x99a4, 0x00000005);
//    wait_on_time(10);

	/* turn on ADC */
//    reg_write(0x982c, 0x2000);
}

#define ZM_DBG_TMP_AGC 0

/* ret: Bit0: AGC calibration   0=>finish  1=>unfinish */
/*      Bit1: Noise calibration 0=>finish  1=>unfinish */
/*      Bit2: Noise calibration finish, but NF value unexcepted => 1 */
u16_t zfAGCCalibration(u32_t finiteLoopCount)
{
	u32_t rddata;
	u32_t wrdata;
	u32_t done;
	u32_t loop;
	u16_t ret = 0;
#if ZM_DBG_TMP_AGC == 1
	u32_t saveCR24, saveCR24_tmp;
	u32_t saveCR25;
	u32_t saveCR25_0, saveCR25_1, saveCR25_2;
	u32_t saveCR111_ext_0, saveCR111_ext_1, saveCR111_ext_2;
	u16_t nf_0, nf_1, nf_2;
	u16_t nf_ext_0, nf_ext_1, nf_ext_2;
	s32_t nf, nf_ext;
#endif

	//zfUartSendStr((u8_t*)"zfAGCCalibration\n");
#if ZM_DBG_TMP_AGC == 1
	saveCR24 = reg_read(0x9800 + (24 << 2));
	reg_write(0x9800 + (24 << 2), (saveCR24 | 0x20000));
	//saveCR25 = reg_read(0x9800 + (25<<2));
	//reg_write(0xc800 + (25<<2), (saveCR25 & 0xfffffe00));
#endif
	rddata = reg_read(0x9860);
	wrdata = rddata | 0x3;
	reg_write(0x9860, wrdata);

	loop = 0;
	done = reg_read(0x9860) & 0x3;
	/* sitesurvey : 100 ms / current connected 200 ms */
	while (done != 0x0 && loop < finiteLoopCount) {
		rddata = reg_read(0x9860);
		done = rddata & 0x3;
		//wait_on_time(10);
		zfDelayUs(100);
		loop++;
	}

	ret = done;
	if (loop >= finiteLoopCount) {
		zfUartSendStr((u8_t *) "AGC+Noise Calibration fail on time\n");
	}
#if ZM_DBG_TMP_AGC == 1
	reg_write(0x9800 + (24 << 2), saveCR24);
#endif

	/* Handle NF procedure result */
	if ((ret & 0x1) == 0x1) {
		/* AGC Calibration fail */
	} else if ((ret & 0x2) == 0x2) {
		/* Noise Calibration fail */
	} else if ((ret & 0x3) == 0) {
#if ZM_DBG_TMP_AGC == 1
		saveCR25_0 = reg_read(0x9800 + (25 << 2)) & 0xfffffe00;
		nf_0 = (saveCR25_0 >> 19) & 0x1ff;

		saveCR25_1 = reg_read(0xa800 + (25 << 2)) & 0xfffffe00;
		nf_1 = (saveCR25_1 >> 19) & 0x1ff;

		saveCR25_2 = reg_read(0xb800 + (25 << 2)) & 0xfffffe00;
		nf_2 = (saveCR25_2 >> 19) & 0x1ff;

		if ((nf_0 & 0x100) != 0x0) {
			nf = 0 - ((nf_0 ^ 0x1ff) + 1);
		} else {
			nf = nf_0;
		}

		/* HT40 BW */
		if (zgDYNAMIC_HT2040_EN == 1) {
			saveCR111_ext_0 =
			    reg_read(0x9800 + (111 << 2)) & 0xfffffe00;
			nf_ext_0 = (saveCR111_ext_0 >> 23) & 0x1ff;

			saveCR111_ext_1 =
			    reg_read(0xa800 + (111 << 2)) & 0xfffffe00;
			nf_ext_1 = (saveCR111_ext_1 >> 23) & 0x1ff;

			saveCR111_ext_2 =
			    reg_read(0xb800 + (111 << 2)) & 0xfffffe00;
			nf_ext_2 = (saveCR111_ext_2 >> 23) & 0x1ff;

			if ((nf_ext_0 & 0x100) != 0x0) {
				nf_ext = 0 - ((nf_ext_0 ^ 0x1ff) + 1);
			} else {
				nf_ext = nf_ext_0;
			}
		}

		if ((nf > -111) && (nf < -62)) {
			/* HT40 BW */
			if (zgDYNAMIC_HT2040_EN == 1) {
				if ((nf_ext > -111) && (nf_ext < -62)) {
					ret |= 0x8;
					reg_write(0x9800 + (111 << 2),
						  (saveCR111_ext_0 |
						   ((nf_ext_0 << 1) & 0x1ff)));
					reg_write(0xa800 + (111 << 2),
						  (saveCR111_ext_1 |
						   ((nf_ext_1 << 1) & 0x1ff)));
					reg_write(0xb800 + (111 << 2),
						  (saveCR111_ext_2 |
						   ((nf_ext_2 << 1) & 0x1ff)));
				} else {
					ret |= 0x4;
					goto ZM_DONE;
				}
			}

			/* Good NF */
			reg_write(0x9800 + (25 << 2),
				  (saveCR25_0 | ((nf_0 << 1) & 0x1ff)));
			reg_write(0xa800 + (25 << 2),
				  (saveCR25_1 | ((nf_1 << 1) & 0x1ff)));
			reg_write(0xb800 + (25 << 2),
				  (saveCR25_2 | ((nf_2 << 1) & 0x1ff)));

			//saveCR24 = reg_read(0x9800+(24<<2));
			saveCR24_tmp = saveCR24 & 0xffff7fff;

			reg_write(0x9800 + (24 << 2), saveCR24_tmp);
			reg_write(0x9800 + (24 << 2), saveCR24_tmp | 0x2);

			done = reg_read(0x9860) & 0x2;
			loop = 0;
			while (done != 0x0 && loop < finiteLoopCount) {
				done = reg_read(0x9860) & 0x2;
				zfDelayUs(100);
				loop++;
			}
			reg_write(0x9800 + (24 << 2), saveCR24);
		} else {
			ret |= 0x4;
		}
#endif
	}
#if ZM_DBG_TMP_AGC == 1
      ZM_DONE:
#endif
	return ret;
}

u32_t zfabs(int input)
{
	if (input >= 0)
		return input;
	else
		return (~input) + 1;
}

void zfDelayUs(u32_t usec)
{
	u32_t t1, t2, dt;

	t1 = zgGetClockCounter();
	while (1) {
		t2 = zgGetClockCounter();
		dt = ((u32_t) ((u32_t) t2 - (u32_t) t1)) /
		    ZM_CLOCK_PER_MICORSECOND;
		if (dt >= usec) {
			return;
		}
	}
}
