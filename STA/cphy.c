/************************************************************************/
/*  Copyright (c) 2000-2005  ZyDAS Technology Corporation               */
/*                                                                      */
/*  Module Name : cphy.c                                                */
/*                                                                      */
/*  Abstract                                                            */
/*      This module contains PHY and RF relative functions.             */
/*                                                                      */
/*  NOTES                                                               */
/*      None                                                            */
/*                                                                      */
/************************************************************************/

#include "dt_defs.h"
#include "gv_extr.h"
#include "uart_extr.h"
#include "reg_defs.h"

#include "sta.h"

extern void zfRecycleTxQ(u8_t qNum);
extern void zfRecycleRxQ(void);

#if ZM_BOARD_TYPE == ZM_BOARD_TYPE_FB50

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
void zfHwCalibration(void);
u16_t zfHwNoiseCalibration(void);
u16_t zfAGCCalibration(u32_t finiteLoopCount);

extern u8_t zg5GhzFreq;

//======================================================================================
u32_t reg_read_PHY(u32_t addr)
{
	u8_t i =0;
	u32_t readVal, tmpVal;

	readVal = zfwReadReg(addr);
	while (i<10)
	{
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

//======================================================================================
void zfDelayMs(u32_t msec);

void wait_on_time(u32_t msec)
{
    zfDelayMs(msec);
}

void zfWaitTxCtlSetClear(void)
{
	/* Tx Q0~4 bit9~bit4 : Number of Q control setting stored in buffer. */
	while(*(volatile u32_t*)(0x1c3b88) & 0x3f0 )
	{
		zfUartSendStrAndHex((u8_t*)"Q0:  ", *(volatile u32_t*)(0x1c3b88));
	}
	while(*(volatile u32_t*)(0x1c3b8c) & 0x3f0 )
	{
		zfUartSendStrAndHex((u8_t*)"Q1:  ", *(volatile u32_t*)(0x1c3b8c));
	}
	while(*(volatile u32_t*)(0x1c3b90) & 0x3f0 )
	{
		zfUartSendStrAndHex((u8_t*)"Q2:  ", *(volatile u32_t*)(0x1c3b90));
	}
	while(*(volatile u32_t*)(0x1c3b94) & 0x3f0 )
	{
		zfUartSendStrAndHex((u8_t*)"Q3:  ", *(volatile u32_t*)(0x1c3b94));
	}
	while(*(volatile u32_t*)(0x1c3b98) & 0x3f0 )
	{
		zfUartSendStrAndHex((u8_t*)"Q4:  ", *(volatile u32_t*)(0x1c3b98));
	}
}


u16_t zfInitRf(u32_t frequency,
              u32_t delta_slope_coeff_exp,
              u32_t delta_slope_coeff_man,
              u32_t delta_slope_coeff_exp_shgi,
              u32_t delta_slope_coeff_man_shgi,
			  u32_t finiteLoopCount)
{
    //zfUartSendStr((u8_t*)"zfInitRf\n\r");
	u16_t ret;

    /* stroe frequency */
    zfHwSaveChannel(frequency);

	zfHwTurnOffdyn(delta_slope_coeff_exp,
                   delta_slope_coeff_man,
                   delta_slope_coeff_exp_shgi,
                   delta_slope_coeff_man_shgi);


#if ZM_FPGA == 0
    /* Real Chip */
    zfProgramADDAC();
	zfHwSwapChain0andChain2();
#endif

    zfHwConfigureMask();
    zfHwHTEnable();
	zfActiveBB();
    #if 0
        zfHwCalibration();
    ret = zfHwNoiseCalibration();
    #else
	ret = zfAGCCalibration(finiteLoopCount);
    #endif

	zfSetChannelEnd();

    //zfUartSendStr((u8_t*)"zfInitRf done\n\r");
	return ret;
}

//=============================================================================
void zfHwSaveChannel(u32_t frequency)
{
	if (frequency < 2484000)
	{
		/* Set WLAN mode to 802.11bg */
        zg5GhzFreq = 0;
	}
	else
	{
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
                    u32_t delta_slope_coeff_man_shgi,
					u32_t finiteLoopCount)
{
	u16_t ret;

    zfHwSaveChannel(frequency);

	zfHwTurnOffdyn(delta_slope_coeff_exp,
                   delta_slope_coeff_man,
                   delta_slope_coeff_exp_shgi,
                   delta_slope_coeff_man_shgi);
    //zfHwConfigureMask();
    //zfHwHTEnable();
    #if 0
    //zfHwCalibration();
        ret = zfHwNoiseCalibration();
    #else
	ret = zfAGCCalibration(finiteLoopCount);
    #endif

	zfSetChannelEnd();
	return ret;
}

void zfSetChannelStart(void)
{
	//u32_t val;
	//wait_on_time(10);

	/* TX/RX must be stopped by now */
    //val = *(volatile u32_t*)(0x1c3500);
    //val |= 0x20;

    //*(volatile u32_t*)(0x1c3500) = val;
  #if 1
	/* Manipulate CCA threshold to stop transmission */
	*(volatile u32_t*)(0x1c5968) = 0x300;
	/* Enable Virtual CCA */
	*(volatile u32_t*)(0x1c3b40) = 0xf8000;

	/* DM controller state */
	//while(*(volatile u32_t*)(0x1c3684) & 0xf000)
	//{
	//	zfUartSendStrAndHex((u8_t*)"TXDM:  ", *(volatile u32_t*)(0x1c3684));
  	//}
  #else
    /* TS/TC select */
    *(volatile u32_t*)(0x1c3b3c) |= 0x1f0000;

    /* DM controller state */
    while(*(volatile u32_t*)(0x1c3684) & 0xf000)
    {
	    zfUartSendStrAndHex((u8_t*)"TXDM:  ", *(volatile u32_t*)(0x1c3684));
    }
  #endif

}

void zfSetChannelEnd(void)
{
    /* Restart DMA */
    //zfRecycleTxQ(0);
    //zfRecycleRxQ();

    /* Trigger Rx DMA */
    //zm_wl_dma_trigger_reg = 0x100;

  #if 1
	/* Manipulate CCA threshold to resume transmission */
	*(volatile u32_t*)(0x1c5968) = 0x000;
	/* Disable Virtual CCA */
	*(volatile u32_t*)(0x1c3b40) = 0x0000;
  #else
  /* TS/TC select */
  *(volatile u32_t*)(0x1c3b3c) &= ~(u32_t)(0x1f0000);
  #endif
}

void zfDKReSetStart(void)
{
	u32_t val;


	wait_on_time(10);

	zfWaitTxCtlSetClear();

    //Warm reset BB/ADDA
	*(volatile u32_t*)(0x1d4004) = 0x400;
	*(volatile u32_t*)(0x1d4004) = 0x0;

    wait_on_time(10);

	/* TX/RX must be stopped by now */
    val = *(volatile u32_t*)(0x1c3500);
    val |= 0x20;

    *(volatile u32_t*)(0x1c3500) = val;
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

    //zfUartSendStr((u8_t*)"zfHwTurnOffdyn\n\r");
#if 0
    if (zgHwConfigure.DYNAMIC_HT2040_EN)
	{
        delta_slope_coeff_exp = 5;
        delta_slope_coeff_man = 23476;

        delta_slope_coeff_exp_shgi = 5;
        delta_slope_coeff_man_shgi = 21128;
	}
	else
	{
        delta_slope_coeff_exp = 4;
        delta_slope_coeff_man = 23476;

        delta_slope_coeff_exp_shgi = 4;
        delta_slope_coeff_man_shgi = 21128;
	}
#endif
    rddata = reg_read(0x9814);
    wrdata = (rddata & 0x00001fff) | (delta_slope_coeff_man << 17) |
                                       (delta_slope_coeff_exp << 13);
    reg_write(0x9814, wrdata);
    //zfUartSendStrAndHex((u8_t*)"Delta Coeff Exp  ", delta_slope_coeff_exp);
    //zfUartSendStrAndHex((u8_t*)"Delta Coeff Man  ", delta_slope_coeff_man);
    //zfUartSendStrAndHex((u8_t*)"wrdata  ", wrdata);

    wrdata = (delta_slope_coeff_man_shgi << 4) | delta_slope_coeff_exp_shgi;
    reg_write(0x9800+(116<<2), wrdata);
    //zfUartSendStrAndHex((u8_t*)"Delta Coeff Exp Short GI  ", delta_slope_coeff_exp_shgi);
    //zfUartSendStrAndHex((u8_t*)"Delta Coeff Man Short GI  ", delta_slope_coeff_man_shgi);
    //zfUartSendStrAndHex((u8_t*)"wrdata  ", wrdata);
}


//======================================================================================
void zfProgramADDAC()
{
   reg_write(0x9800+(12<<2), 0x1);     //# Select Internal ADDAC
   wait_on_time(10);

   reg_write(0x989c,  0x00000000);     //# 7-0
   reg_write(0x989c,  0x00000000);     //# 15-8
   reg_write(0x989c,  0x00000000);     //# 23-16
   reg_write(0x989c,  0x00000000);     //# 31-
   reg_write(0x989c,  0x00000000);     //# 39-
   reg_write(0x989c,  0x00000000);     //# 47-
   reg_write(0x989c,  0x00000000);     //# 55- [48]:doubles the xtalosc bias current
   reg_write(0x989c,  0x00000000);     //# 63-
   reg_write(0x989c,  0x00000000);     //# 71-
   reg_write(0x989c,  0x00000000);     //# 79-
   reg_write(0x989c,  0x00000000);     //# 87-
   reg_write(0x989c,  0x00000000);     //# 95-
   reg_write(0x989c,  0x00000000);     //# 103-
   reg_write(0x989c,  0x00000000);     //# 111-
   reg_write(0x989c,  0x00000000);     //# 119-
   reg_write(0x989c,  0x00000000);     //# 127-
   reg_write(0x989c,  0x00000000);     //# 135-
   reg_write(0x989c,  0x00000000);     //# 143-
   reg_write(0x989c,  0x00000000);     //# 151-
   reg_write(0x989c,  0x00000030);     //# 159- #[158:156]=xlnabufmode
   reg_write(0x989c,  0x00000004);     //# 167-  [162]:disable clkp_driver to fowl
   reg_write(0x989c,  0x00000000);     //# 175-
   reg_write(0x989c,  0x00000000);     //# 183-176
   reg_write(0x989c,  0x00000000);     //# 191-184
   reg_write(0x989c,  0x00000000);     //# 199-
   reg_write(0x989c,  0x00000000);     //# 207-
   reg_write(0x989c,  0x00000000);     //# 215-
   reg_write(0x989c,  0x00000000);     //# 223-
   reg_write(0x989c,  0x00000000);     //# 231-
   reg_write(0x98c4,  0x00000000);     //# 233-232

   wait_on_time(10);
   reg_write(0x9800+(12<<2), 0x0);     //# Select External Fowl
}

//======================================================================================
void zfHwSwapChain0andChain2()
{
	u32_t Q_inv;

	/* inverse */
	Q_inv=0x1;
	reg_write(0x9800+(666<<2), Q_inv);

	/* Swap Chain 0 and Chain 2 */
//	if ($FPGA == 0x0) {
//   		if ($RX_CHAIN >= 0x1) {
//      		if ($SWAP_EN == 0x1) {
         		reg_write(0x9800+(666<<2), (Q_inv | (1 << 6)));
//      		}
//   		}
//	}
}

//======================================================================================
void zfHwConfigureMask()
{
#if ZM_FPGA == 1
    //zfUartSendStr((u8_t*)"zfHwConfigureMask\n\r");

    //zgHwConfigure.FPGA == 0x1)
    //zgHwConfigure.FPGA_CHAIN == 0x1)
    reg_write(0x99a4, 0x3);
    reg_write(0xa39c, 0x3);
#else
    reg_write(0x99a4, 0x5);
    reg_write(0xa39c, 0x5);
#endif
}


//======================================================================================
void zfHwHTEnable()
{
#if 0 /* compatibility for UB83 */
#if ZM_FPGA == 1
    //zfUartSendStr((u8_t*)"zfHwHTEnable\n\r");

    reg_write(0x9804, 0x3c4);     //# Dyn HT2040
    //reg_write(0x9804, 0x340);    //# Static HT20
    //reg_write(0x9804, 0x0);         //# Legacy
#else
	/* Temp setting for bringup */
	//reg_write(0x9804, 0x0);         //# Legacy


    if (zgHT_ENABLE == 1)
	{
        if (zgDYNAMIC_HT2040_EN == 1)
		{
			if (zgExtOffset == 1) {
            	reg_write(0x9804, 0x3d4); //3d4 for real

			}
			else {
				reg_write(0x9804, 0x3c4);   //3c4 for real
			}
			//# Dyn HT2040.Refer to Reg 1.
            //#[3]:single length (4us) 1st HT long training symbol; use Walsh spatial spreading for 2 chains 2 streams TX
            //#[c]:allow short GI for HT40 packets; enable HT detection.
            //#[4]:enable 20/40 MHz channel detection.
        }
        else
	    {
            reg_write(0x9804, 0x340);
		    //# Static HT20
            //#[3]:single length (4us) 1st HT long training symbol; use Walsh spatial spreading for 2 chains 2 streams TX
            //#[4]:Otus don't allow short GI for HT20 packets yet; enable HT detection.
            //#[0]:disable 20/40 MHz channel detection.
        }
    }
    else
	{

        reg_write(0x9804, 0x0);
		//# Legacy;# Direct Mapping for each chain.
        //#Be modified by Oligo to add dynanic for legacy.

        if (zgDYNAMIC_HT2040_EN == 1)
		{
            reg_write(0x9804, 0x4);     //# Dyn Legacy .Refer to reg 1.
        }
        else
		{
            reg_write(0x9804, 0x0);    //# Static Legacy
        }
	}

#endif

    wait_on_time(10);
#endif	/* end of compatibility for UB83 */
}

//======================================================================================
void zfActiveBB()
{
    reg_write(0x981c, 0x00000001);               // # Activate BB
    wait_on_time(10);
#if ZM_FPGA == 0
	/* force ADC alive */
//    reg_write(0x99a4, 0x00000001);
//    wait_on_time(10);
//    reg_write(0x99a4, 0x00000002);
//    wait_on_time(10);
//    reg_write(0x99a4, 0x00000005);
//    wait_on_time(10);

	/* turn on ADC */
//    reg_write(0x982c, 0x2000);
#endif
}


//======================================================================================
/* routine */
#if 0
void zfHwCalibration()
{
    u32_t rddata;
    u32_t wrdata;
    u32_t done;
	u32_t loop;

    //zfUartSendStr((u8_t*)"zfHwCalibration\n\r");

    rddata = reg_read(0x9860);
    wrdata = rddata | 0x1;
    reg_write(0x9860, wrdata);

	loop = 0;
    done = reg_read(0x9860) & 0x1;
    while (done != 0x0 && loop<100)
	{
        rddata = reg_read(0x9860);
        done = rddata & 0x1;
        wait_on_time(10);
		loop++;
    }

    if (loop>=100)
        zfUartSendStr((u8_t*)"HwCalibration  fail\n\r");

    wait_on_time(10);
}
#endif

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

    //zfUartSendStr((u8_t*)"zfAGCCalibration\n\r");
#if ZM_DBG_TMP_AGC == 1
	saveCR24 = reg_read(0x9800 + (24<<2));
	reg_write(0x9800 + (24<<2), (saveCR24 | 0x20000));
	//saveCR25 = reg_read(0x9800 + (25<<2));
	//reg_write(0xc800 + (25<<2), (saveCR25 & 0xfffffe00));
#endif
    rddata = reg_read(0x9860);
    wrdata = rddata | 0x3;
    reg_write(0x9860, wrdata);

    loop = 0;
    done = reg_read(0x9860) & 0x3;
	/* sitesurvey : 100 ms / current connected 200 ms */
    while (done != 0x0 && loop<finiteLoopCount)
    {
        rddata = reg_read(0x9860);
        done = rddata & 0x3;
        //wait_on_time(10);
		zfDelayUs(100);
		loop++;
    }

    ret = done;
    if (loop>=finiteLoopCount)
    {
        zfUartSendStr((u8_t*)"AGC+Noise Calibration fail on time\n\r");
    }

#if ZM_DBG_TMP_AGC == 1
	reg_write(0x9800 + (24<<2), saveCR24);
#endif

	/* Handle NF procedure result */
	if ((ret & 0x1) == 0x1)
	{
		/* AGC Calibration fail */
	}
	else if ((ret & 0x2) == 0x2)
	{
		/* Noise Calibration fail */
	}
	else if ((ret & 0x3) == 0)
	{
#if ZM_DBG_TMP_AGC == 1
		saveCR25_0 = reg_read(0x9800+(25<<2)) & 0xfffffe00;
		nf_0 = (saveCR25_0 >> 19) & 0x1ff;

		saveCR25_1 = reg_read(0xa800+(25<<2)) & 0xfffffe00;
		nf_1 = (saveCR25_1 >> 19) & 0x1ff;

		saveCR25_2 = reg_read(0xb800+(25<<2)) & 0xfffffe00;
		nf_2 = (saveCR25_2 >> 19) & 0x1ff;

        if ((nf_0 & 0x100) != 0x0)
        {
            nf = 0 - ((nf_0 ^ 0x1ff) + 1);
        }
        else
        {
            nf = nf_0;
        }

		/* HT40 BW */
		if (zgDYNAMIC_HT2040_EN == 1)
		{
			saveCR111_ext_0 = reg_read(0x9800+(111<<2)) & 0xfffffe00;
			nf_ext_0 = (saveCR111_ext_0 >> 23) & 0x1ff;

			saveCR111_ext_1 = reg_read(0xa800+(111<<2)) & 0xfffffe00;
			nf_ext_1 = (saveCR111_ext_1 >> 23) & 0x1ff;

			saveCR111_ext_2 = reg_read(0xb800+(111<<2)) & 0xfffffe00;
			nf_ext_2 = (saveCR111_ext_2 >> 23) & 0x1ff;

        	if ((nf_ext_0 & 0x100) != 0x0)
        	{
            	nf_ext = 0 - ((nf_ext_0 ^ 0x1ff) + 1);
        	}
        	else
        	{
            	nf_ext = nf_ext_0;
        	}
		}

		if ((nf > -111) && (nf < -62) )
		{
			/* HT40 BW */
			if (zgDYNAMIC_HT2040_EN == 1)
			{
				if ( (nf_ext > -111) && (nf_ext < -62) )
				{
					ret |= 0x8;
					reg_write(0x9800+(111<<2),(saveCR111_ext_0 | ((nf_ext_0<<1)&0x1ff)) );
					reg_write(0xa800+(111<<2),(saveCR111_ext_1 | ((nf_ext_1<<1)&0x1ff)) );
					reg_write(0xb800+(111<<2),(saveCR111_ext_2 | ((nf_ext_2<<1)&0x1ff)) );
				}
				else
				{
					ret |= 0x4;
					goto ZM_DONE;
				}
			}

			/* Good NF */
			reg_write(0x9800+(25<<2),(saveCR25_0 | ((nf_0<<1)&0x1ff)) );
			reg_write(0xa800+(25<<2),(saveCR25_1 | ((nf_1<<1)&0x1ff)) );
			reg_write(0xb800+(25<<2),(saveCR25_2 | ((nf_2<<1)&0x1ff)) );


			//saveCR24 = reg_read(0x9800+(24<<2));
			saveCR24_tmp = saveCR24 & 0xffff7fff;

			reg_write(0x9800+(24<<2), saveCR24_tmp);
			reg_write(0x9800+(24<<2), saveCR24_tmp | 0x2);


        	done = reg_read(0x9860) & 0x2;
			loop = 0;
        	while (done != 0x0  && loop<finiteLoopCount)
        	{
            	done = reg_read(0x9860) & 0x2;
            	zfDelayUs(100);
				loop++;
        	}
        	reg_write(0x9800+(24<<2), saveCR24);
		}
		else
		{
			ret |= 0x4;
		}
#endif
	}
#if ZM_DBG_TMP_AGC == 1
ZM_DONE:
#endif
    return ret;
}

//======================================================================================
u32_t zfabs(int input)
{
    if(input>=0)
	    return input;
    else
	    return (~input)+1;
}

#if 0
u16_t zfHwNoiseCalibration()
{
    int hainans_in_sync;
    int rddata;
    int wrdata;
    int done;
    int noisefloor;
    int noisefloor0;
    int noisefloor1;
    int noisefloor2;
    int nf;
    int nf_diff;

	//int loop_1 = 0;
	int loop_2 = 0;

    //zfUartSendStr((u8_t*)"zfHwNoiseCalibration\n\r");

#if ZM_FPGA == 1
    if (1) //(zgHwConfigure.FPGA == 0x1)
    {
        hainans_in_sync = 0;
        while (hainans_in_sync == 0 && loop_1<10)
        {
            rddata = reg_read(0x9860);
	        //zfUartSendStrAndHex((u8_t*)"1:0x9860  ", rddata);
            wrdata = rddata | 0x2;
            reg_write(0x9860, wrdata);
            rddata = reg_read(0x9860);
	        //zfUartSendStrAndHex((u8_t*)"2:0x9860  ", rddata);

	        loop_2 = 0;
            done = reg_read(0x9860) & 0x2;
            while (done != 0x0 && loop_2<100)
            {
                done = reg_read(0x9860) & 0x2;
                //$tmp = reg_read(0x9860);
                //printf "0x9860 0x%08x\n", $tmp;
                wait_on_time(100);
				loop_2++;
            }
	        //zfUartSendStr((u8_t*)":Noise Calibration done !!!!!\n\r");

            if (0)//(zgHwConfigure.FPGA_CHAIN == 0x0)
            {
                rddata = reg_read(0x9800+(25<<2));
                nf = (rddata >> 19) & 0x1ff;
                if ((nf & 0x100) != 0x0)
                {
                    noisefloor = 0 - ((nf ^ 0x1ff) + 1);
                }
                else
                {
                    noisefloor = nf;
                }
		        //zfUartSendStrAndHex((u8_t*)"Noise Floor for Chain0 = -", (~noisefloor)+1);

                if (1) //(zgHwConfigure.DYNAMIC_HT2040_EN == 1)
                {
                    rddata = reg_read(0x9800+(111<<2));
                    nf = (rddata >> 23) & 0x1ff;
                    if ((nf & 0x100) != 0x0)
                    {
                        noisefloor = 0 - ((nf ^ 0x1ff) + 1);
                    }
                    else
                    {
                        noisefloor = nf;
                    }
                //    zfUartSendStrAndHex((u8_t*)"Noise Floor for Chain0 (ext chan)= -", (~noisefloor)+1);
                }
                nf_diff = 0;
            } /* if (zgHwConfigure.FPGA_CHAIN == 0x0) */
            else
            {
                rddata = reg_read(0x9800+(25<<2));
                nf = (rddata >> 19) & 0x1ff;
                if ((nf & 0x100) != 0x0)
                {
                    noisefloor0 = 0 - ((nf ^ 0x1ff) + 1);
                }
                else
                {
                    noisefloor0 = nf;
                }
                //zfUartSendStrAndHex((u8_t*)":Noise Floor for Chain0 = -", (~noisefloor0)+1);

                rddata = reg_read(0xa800+(25<<2));
                nf = (rddata >> 19) & 0x1ff;
                if ((nf & 0x100) != 0x0)
                {
                    noisefloor1 = 0 - ((nf ^ 0x1ff) + 1);
                }
                else
                {
                    noisefloor1 = nf;
                }
                //zfUartSendStrAndHex((u8_t*)":Noise Floor for Chain1 = -", (~noisefloor1)+1);

                if (1) //(zgHwConfigure.DYNAMIC_HT2040_EN == 1)
                {
                    rddata = reg_read(0x9800+(111<<2));
                    nf = (rddata >> 23) & 0x1ff;
                    if ((nf & 0x100) != 0x0)
                    {
                        noisefloor0 = 0 - ((nf ^ 0x1ff) + 1);
                    }
                    else
                    {
                        noisefloor0 = nf;
                    }
                    //zfUartSendStrAndHex((u8_t*)":Noise Floor for Chain0 (ext chan)= -", (~noisefloor0)+1);

                    rddata = reg_read(0xa800+(111<<2));
                    nf = (rddata >> 23) & 0x1ff;
                    if ((nf & 0x100) != 0x0)
                    {
                        noisefloor1 = 0 - ((nf ^ 0x1ff) + 1);
                    }
                    else
                    {
                        noisefloor1 = nf;
                    }
                    //zfUartSendStrAndHex((u8_t*)":Noise Floor for Chain1 (ext chan)= -", (~noisefloor1)+1);
                }
                nf_diff = (noisefloor0 - noisefloor1);
            }/* if (zgHwConfigure.FPGA_CHAIN != 0x0) */

            if (1) //(zgHwConfigure.FPGA == 0x1)
            {
                if (zfabs(nf_diff) < 2)
                {
                    hainans_in_sync = 1;
                }
                else
                {
                    reg_write(0xd47c, 0xe6);
                    wait_on_time(10);
					loop_1++;
                }
            }
        }/* end while */
    }/* if (zgHwConfigure.FPGA == 0x1) */
    else
#endif /* end #if ZM_FPGA == 1 */
    {
        rddata = reg_read(0x9860);
        //printf " 1:0x9860 0x%08x\n", $rddata;
        wrdata = rddata | 0x2;
        reg_write(0x9860, wrdata);
        //rddata = reg_read(0x9860);
        //printf " 2:0x9860 0x%08x\n", $rddata;

	    loop_2 = 0;
        done = reg_read(0x9860) & 0x2;
        while (done != 0x0 && loop_2<1000)
        {
            done = reg_read(0x9860) & 0x2;
            //$tmp = reg_read(0x9860);
            //printf "0x9860 0x%08x\n", $tmp;
            //wait_on_time(1);
			zfDelayUs(100);
			loop_2++;
        }
        //zfUartSendStr((u8_t*)"Noise Calibration done !!!!!\n\r");

#if 0
/* only for hw status check- display information */
        if (0)//(zgHwConfigure.RX_CHAIN == 0x0)
        {
            rddata = reg_read(0x9800+(25<<2));
            nf = (rddata >> 19) & 0x1ff;
            if ((nf & 0x100) != 0x0)
            {
                noisefloor = 0 - ((nf ^ 0x1ff) + 1);
            }
            else
            {
                noisefloor = nf;
            }
            //zfUartSendStrAndHex((u8_t*)"Noise Floor for Chain0 = -", (~noisefloor)+1);

            if (1) //(zgHwConfigure.DYNAMIC_HT2040_EN == 1)
            {
                rddata = reg_read(0x9800+(111<<2));
                nf = (rddata >> 23) & 0x1ff;
                if ((nf & 0x100) != 0x0)
                {
                    noisefloor = 0 - ((nf ^ 0x1ff) + 1);
                }
                else
                {
                    noisefloor = nf;
                }
                //zfUartSendStrAndHex((u8_t*)"Noise Floor for Chain0 (ext chan)= -", (~noisefloor)+1);
            }
        } /* if (zgHwConfigure.RX_CHAIN == 0x0)  */
        else
        {
            rddata = reg_read(0x9800+(25<<2));
            nf = (rddata >> 19) & 0x1ff;
            if ((nf & 0x100) != 0x0)
            {
                noisefloor0 = 0 - ((nf ^ 0x1ff) + 1);
            }
            else
            {
                noisefloor0 = nf;
            }
            //zfUartSendStrAndHex((u8_t*)"Noise Floor for Chain0 = -", (~noisefloor0)+1);

//            rddata = reg_read(0xa800+(25<<2));
//            nf = (rddata >> 19) & 0x1ff;
//            if ((nf & 0x100) != 0x0)
//            {
//                noisefloor1 = 0 - ((nf ^ 0x1ff) + 1);
//            }
//            else
//            {
//                noisefloor1 = nf;
//            }
            //zfUartSendStrAndHex((u8_t*)"Noise Floor for Chain1 = -", (~noisefloor1)+1);

            rddata = reg_read(0xb800+(25<<2));
            nf = (rddata >> 19) & 0x1ff;
            if ((nf & 0x100) != 0x0)
            {
                noisefloor2 = 0 - ((nf ^ 0x1ff) + 1);
            }
            else
            {
                noisefloor2 = nf;
            }
            //zfUartSendStrAndHex((u8_t*)"Noise Floor for Chain2 = -", (~noisefloor2)+1);

            if (zgDYNAMIC_HT2040_EN == 1)
            {
                rddata = reg_read(0x9800+(111<<2));
                nf = (rddata >> 23) & 0x1ff;
                if ((nf & 0x100) != 0x0)
                {
                    noisefloor = 0 - ((nf ^ 0x1ff) + 1);
                }
                else
                {
                    noisefloor = nf;
                }
                //zfUartSendStrAndHex((u8_t*)"Noise Floor for Chain0 (ext chan)= -", (~noisefloor)+1);

//                rddata = reg_read(0xa800+(111<<2));
//                nf = (rddata >> 23) & 0x1ff;
//                if ((nf & 0x100) != 0x0)
//                {
//                    noisefloor = 0 - ((nf ^ 0x1ff) + 1);
//                }
//                else
//                {
//                    noisefloor = nf;
//                }
                //zfUartSendStrAndHex((u8_t*)"Noise Floor for Chain1 (ext chan)= -", (~noisefloor)+1);

                rddata = reg_read(0xb800+(111<<2));
                nf = (rddata >> 23) & 0x1ff;
                if ((nf & 0x100) != 0x0)
                {
                    noisefloor = 0 - ((nf ^ 0x1ff) + 1);
                }
                else
                {
                    noisefloor = nf;
                }
                //zfUartSendStrAndHex((u8_t*)"Noise Floor for Chain2 (ext chan)= -", (~noisefloor)+1);
            }/* if (zgHwConfigure.DYNAMIC_HT2040_EN == 1) */
        }
#endif /* information check */
    }

	//if (loop_1>=10 || loop_2>=100)
	if (loop_2>=1000)
	{
        zfUartSendStr((u8_t*)"Noise Calibration fail!!\n\r");
		*(volatile u32_t*)(0x117700) = *(volatile u32_t*)(0x117700) + 1;
		return 1;
	}
	return 0;
}
#endif


/************************************************************************/
/************************************************************************/
u32_t zgGetClockCounter(void)
{
    u32_t t;

    t = ZM_CLOCK_HIGH_REG;
    t = (t<<16)+ZM_CLOCK_LOW_REG;
    return t;
}

/************************************************************************/
/************************************************************************/
void zfDelayMs(u32_t msec)
{
    u32_t t1, t2, dt;

    t1 = zgGetClockCounter();
    while (1)
    {
        t2 = zgGetClockCounter();
        dt = ((u32_t)((u32_t)t2 - (u32_t)t1)) / ZM_CLOCK_PER_MICORSECOND / 1000;
        if (dt >= msec)
        {
            return;
        }
    }
}

void zfDelayUs(u32_t usec)
{
    u32_t t1, t2, dt;

    t1 = zgGetClockCounter();
    while (1)
    {
        t2 = zgGetClockCounter();
        dt = ((u32_t)((u32_t)t2 - (u32_t)t1)) / ZM_CLOCK_PER_MICORSECOND;
        if (dt >= usec)
        {
            return;
        }
    }
}


/* init for OTUS FPGA init */
void zfFPGAInit()
{
    /* DCM */
    *(volatile u32_t*)0x1d4004 = 0x2;
    *(volatile u32_t*)0x1d4004 = 0x0;
	zfDelayMs(3);

    /* WARM, Cold */
    *(volatile u32_t*)0x1d4004 = 0x600;
    *(volatile u32_t*)0x1d4004 = 0x0;
	zfDelayMs(3);
}


void zfHwInit(void)
{
#if ZM_FPGA == 1
    /* FPGA init */
    zfFPGAInit();

	/* OTUS */
	*(volatile u32_t*)0x1c947c = 0xe6;  /* PLL 24 MHz */
	zfDelayMs(10);

	/* FPGA init */
    zfFPGAInit();
#endif
    return;
}

#endif //#if ZM_BOARD_TYPE == ZM_BOARD_TYPE_FB50

/***************************************************************************/
#if ZM_BOARD_TYPE == ZM_BOARD_TYPE_MIMO2


#define zfwReadReg(addr) (*(volatile u32_t*)(0x1c3000+(addr<<2)))
#define zfwWriteReg(addr, val) (*(volatile u32_t*)(0x1c3000+(addr<<2)) = val)
extern u8_t zg5GhzFreq;

const u32_t zgAiroha2230Tbl[] =
{
    //0x03f790, 0x033331, 0x00000d,  //;Null
    0x03f790, 0x033331, 0x00000d,  //;Ch 1
    0x03f790, 0x0b3331, 0x00000d,  //;Ch 2
    0x03e790, 0x033331, 0x00000d,  //;Ch 3
    0x03e790, 0x0b3331, 0x00000d,  //;Ch 4
    0x03f7a0, 0x033331, 0x00000d,  //;Ch 5
    0x03f7a0, 0x0b3331, 0x00000d,  //;Ch 6
    0x03e7a0, 0x033331, 0x00000d,  //;Ch 7
    0x03e7a0, 0x0b3331, 0x00000d,  //;Ch 8
    0x03f7b0, 0x033331, 0x00000d,  //;Ch 9
    0x03f7b0, 0x0b3331, 0x00000d,  //;Ch 10
    0x03e7b0, 0x033331, 0x00000d,  //;Ch 11
    0x03e7b0, 0x0b3331, 0x00000d,  //;Ch 12
    0x03f7c0, 0x033331, 0x00000d,  //;Ch 13
    0x03e7c0, 0x066661, 0x00000d   //;Ch 14
};

const u32_t AL7230TB[] = {
    0x202790, 0x133331,   //;Null
    0x202790, 0x133331,   //;Ch 1
    0x202790, 0x1b3331,   //;Ch 2
    0x202790, 0x033331,   //;Ch 3
    0x202790, 0x0b3331,   //;Ch 4
    0x2027a0, 0x133331,   //;Ch 5
    0x2027a0, 0x1b3331,   //;Ch 6
    0x2027a0, 0x033331,   //;Ch 7
    0x2027a0, 0x0b3331,   //;Ch 8
    0x2027b0, 0x133331,   //;Ch 9
    0x2027b0, 0x1b3331,   //;Ch 10
    0x2027b0, 0x033331,   //;Ch 11
    0x2027b0, 0x0b3331,   //;Ch 12
    0x2027c0, 0x133331,   //;Ch 13
    0x2027c0, 0x066661    //;Ch 14
};

const u32_t AL7230TB_a[] = {
    0x2ff560, 0x0aaab1, 0x451fe2, 0x7fd784,   //;Null
    0x2ff540, 0x000001, 0x451fe2, 0x6fd784,   //;CH 8 , 5040MHz
    0x2ff540, 0x0aaab1, 0x451fe2, 0x7fd784,   //;CH 12, 5060MHz
    0x2ff550, 0x155551, 0x451fe2, 0x7fd784,   //;CH 16, 5080MHz
    0x2ff560, 0x055551, 0x451fe2, 0x7fd784,   //;CH 34, 5170MHz
    0x2ff560, 0x0aaab1, 0x451fe2, 0x7fd784,   //;Ch 36, 5180MHz
    0x2ff570, 0x100001, 0x451fe2, 0x7fd784,   //;Ch 38, 5190MHz
    0x2ff570, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 40, 5200MHz
    0x2ff570, 0x1aaab1, 0x451fe2, 0x7fd784,   //;Ch 42, 5210MHz
    0x2ff570, 0x000001, 0x451fe2, 0x6fd784,   //;Ch 44, 5220MHz, current support
    0x2ff570, 0x055551, 0x451fe2, 0x7fd784,   //;Ch 46, 5230MHz
    0x2ff570, 0x0aaab1, 0x451fe2, 0x7fd784,   //;Ch 48, 5240MHz
    0x2ff580, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 52, 5260MHz
    0x2ff580, 0x000001, 0x451fe2, 0x6fd784,   //;Ch 56, 5280MHz, current support
    0x2ff580, 0x0aaab1, 0x451fe2, 0x7fd784,   //;Ch 60, 5300MHz
    0x2ff590, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 64, 5320MHz
    0x2ff590, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 68, 5320MHz,dummy
    0x2ff590, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 72, 5320MHz,dummy
    0x2ff590, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 76, 5320MHz,dummy
    0x2ff590, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 80, 5320MHz,dummy
    0x2ff590, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 84, 5320MHz,dummy
    0x2ff590, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 88, 5320MHz,dummy
    0x2ff590, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 92, 5320MHz,dummy
    0x2ff590, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 96, 5320MHz,dummy
    0x2ff5c0, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 100, 5500MHz
    0x2ff5c0, 0x000001, 0x451fe2, 0x6fd784,   //;Ch 104, 5520MHz
    0x2ff5c0, 0x0aaab1, 0x451fe2, 0x7fd784,   //;Ch 108, 5540MHz
    0x2ff5d0, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 112, 5560MHz
    0x2ff5d0, 0x000001, 0x451fe2, 0x6fd784,   //;Ch 116, 5580MHz
    0x2ff5d0, 0x0aaab1, 0x451fe2, 0x7fd784,   //;Ch 120, 5600MHz
    0x2ff5e0, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 124, 5620MHz
    0x2ff5e0, 0x000001, 0x451fe2, 0x6fd784,   //;Ch 128, 5640MHz
    0x2ff5e0, 0x0aaab1, 0x451fe2, 0x7fd784,   //;Ch 132, 5660MHz
    0x2ff5f0, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 136, 5680MHz
    0x2ff5f0, 0x000001, 0x451fe2, 0x6fd784,   //;Ch 140, 5700MHz
    0x2ff5f0, 0x000001, 0x451fe2, 0x6fd784,   //;Ch 144, 5700MHz, dummy
    0x2ff600, 0x180001, 0x451fe2, 0x7fd784,   //;Ch 149, 5745MHz
    0x2ff600, 0x02aab1, 0x451fe2, 0x7fd784,   //;Ch 153, 5765MHz
    0x2ff600, 0x0d5551, 0x451fe2, 0x7fd784,   //;Ch 157, 5785MHz
    0x2ff610, 0x180001, 0x451fe2, 0x7fd784,   //;Ch 161, 5805MHz
    0x2ff610, 0x02aab1, 0x451fe2, 0x7fd784,   //;Ch 165, 5825MHz
    0x2ff610, 0x02aab1, 0x451fe2, 0x7fd784,   //;Ch 168, 5825MHz,dummy
    0x2ff610, 0x02aab1, 0x451fe2, 0x7fd784,   //;Ch 172, 5825MHz,dummy
    0x2ff610, 0x02aab1, 0x451fe2, 0x7fd784,   //;Ch 176, 5825MHz,dummy
    0x2ff610, 0x02aab1, 0x451fe2, 0x7fd784,   //;Ch 180, 5825MHz,dummy
    0x2ff520, 0x000001, 0x451fe2, 0x7fd784,   //;Ch 184, 4920MHz
    0x2ff520, 0x0aaab1, 0x451fe2, 0x7fd784,   //;Ch 188, 4940MHz
    0x2ff530, 0x155551, 0x451fe2, 0x7fd784,   //;Ch 192, 4960MHz
    0x2ff530, 0x000001, 0x451fe2, 0x7fd784    //;Ch 196, 4980MHz
};


void zfHwConfigBit(u32_t bit, u8_t set)
{
    u32_t temp;

    temp = zfwReadReg(203);
    //temp = zfwReadReg(203);

    if (set)
        zfwWriteReg(203, (temp | bit));
    else
        zfwWriteReg(203, (temp&(~bit)));
}

void zfHwSetIFSynthesizer(u32_t inputValue)
{
    u32_t serialBitCount = 24;
    volatile u32_t i;

    inputValue = inputValue << (31 - serialBitCount);

    /* Enable serial interface */
    zfHwConfigBit(2, 0);

    {
        /* Prepare clock */
        zfHwConfigBit(4, 0);

        while (serialBitCount)
        {
            inputValue = inputValue << 1;

            /* Generate data */
            if (inputValue & 0x80000000)
            {
                zfHwConfigBit(8, 1);
            }
            else
            {
                zfHwConfigBit(8, 0);
            }

            /* Generate clock */
            zfHwConfigBit(4, 1);
            zfHwConfigBit(4, 0);

            serialBitCount--;
       }
    }

    /* Disable serial interface */
    zfHwConfigBit(2, 1);

    for (i=0; i<40; i++){}

    return;
}

void zfInitRf(void)
{
	volatile u32_t i;
#if 0
    /* 2230 */
    /* Set to channel 1 */
    zfHwSetIFSynthesizer(zgAiroha2230Tbl[10*3]);
	zfHwSetIFSynthesizer(zgAiroha2230Tbl[10*3+1]);
	zfHwSetIFSynthesizer(zgAiroha2230Tbl[10*3+2]);

	zfHwSetIFSynthesizer(0x0b3331);
	zfHwSetIFSynthesizer(0x03b812);
	zfHwSetIFSynthesizer(0x00fff3);
	zfHwSetIFSynthesizer(0x0005a4);  //To improve AL2230 yield, improve phase noise, 4713
	zfHwSetIFSynthesizer(0x044dc5);  //To improve AL2230 yield, improve phase noise, 4713
	zfHwSetIFSynthesizer(0x0805b6);  //Reg6 update for MP versio
	zfHwSetIFSynthesizer(0x0146C7); // Modified by jxiao for Bor-Chin on 2004/08/02
	zfHwSetIFSynthesizer(0x000688);
	zfHwSetIFSynthesizer(0x0403b9);   //External control TX power (CR31)
	zfHwSetIFSynthesizer(0x00dbba);  //RegA update for MP version
	zfHwSetIFSynthesizer(0x00099b);  //RegB update for MP version
	zfHwSetIFSynthesizer(0x0bdffc);  //RegC update for MP version
	zfHwSetIFSynthesizer(0x00000d);  //RegD update for MP version
	zfHwSetIFSynthesizer(0x00580f); //improve phase noise and remove phase calibration,4713

	zfHwSetIFSynthesizer(0x000d80f); //To improve AL2230 yield, 4713
	//zm_delay1us(100);    //delay timing to be verify
	//udelay(100);
	for (i=0; i<100000; i++){}


	zfHwSetIFSynthesizer(0x00780f);  //To improve AL2230 yield, 4713
	//zm_delay1us(100);
	//udelay(100);
	for (i=0; i<100000; i++){}

	zfHwSetIFSynthesizer(0x00580f); //To improve AL2230 yield,4713
	for (i=0; i<100000; i++){}
#else
    /* 7230 */
    /* Init default set channel 11 */
    //zfwWriteReg(251, 0x2f);
	zfHwSetIFSynthesizer(AL7230TB[11*2]);
	zfHwSetIFSynthesizer(AL7230TB[11*2+1]);
	zfHwSetIFSynthesizer(0x841ff2);
	zfHwSetIFSynthesizer(0x3fdfa3);
	zfHwSetIFSynthesizer(0x7fd784);
	zfHwSetIFSynthesizer(0x802b55);
	zfHwSetIFSynthesizer(0x56af36);
	zfHwSetIFSynthesizer(0xce0207);
	zfHwSetIFSynthesizer(0x6ebc98);
	zfHwSetIFSynthesizer(0x221bb9);
	zfHwSetIFSynthesizer(0xe0000a);
	zfHwSetIFSynthesizer(0x08031b);
	zfHwSetIFSynthesizer(0x00093c);
	zfHwSetIFSynthesizer(0xfffffd);
	zfHwSetIFSynthesizer(0x00000e);
	zfHwSetIFSynthesizer(0x1aba8f);

	//zfwWriteReg(251, 0x3f); //PLL_ON
	//zfwWriteReg(128, 0x14);
	//zfwWriteReg(129, 0x12);
	//zfwWriteReg(130, 0x10);
	for (i=0; i<100000; i++){}

	zfHwSetIFSynthesizer(0x9aba8f);
	for (i=0; i<100000; i++){}

	zfHwSetIFSynthesizer(0x3aba8f);
	for (i=0; i<100000; i++){}

	zfHwSetIFSynthesizer(0x1aba8f);
	for (i=0; i<100000; i++){}
#endif
}


void zfSetChannel(u32_t frequency,
                    u32_t delta_slope_coeff_exp,
                    u32_t delta_slope_coeff_man,
                    u32_t delta_slope_coeff_exp_shgi,
                    u32_t delta_slope_coeff_man_shgi)
{
    u8_t channel;
    u8_t channel_temp;

    if(frequency <= 2484000)
    {
        if(frequency == 2484000)
            channel = 14;
        else
            channel = ((frequency)/1000 - 2412)/5 + 1;

        /* Set WLAN mode to 802.11bg */
        zg5GhzFreq = 0;
    }
    else
    {
        if(frequency < 5000000)
            channel = ((frequency)/1000 - 4000)/5 ;
        else
            channel = ((frequency)/1000 - 5000)/5 ;

        /* Set WLAN mode to 802.11a */
        zg5GhzFreq = 1;
    }
    zfUartSendStrAndHex((u8_t*)"HwSetChannel: set channel=", channel);


#if 0
    /* Set RF Chip and channel */
    zfHwSetIFSynthesizer(zgAiroha2230Tbl[(channel-1)*3]);
    zfHwSetIFSynthesizer(zgAiroha2230Tbl[(channel-1)*3+1]);
    zfHwSetIFSynthesizer(zgAiroha2230Tbl[(channel-1)*3+2]);
#else
	/* 7230 */
    if(frequency <= 2484000)
    {
        //zfwWriteReg(251, 0x2f); //PLL_OFF
    	zfHwSetIFSynthesizer(0x841ff2);
    	zfHwSetIFSynthesizer(0x3fdfa3);
    	zfHwSetIFSynthesizer(0x7fd784);
    	zfHwSetIFSynthesizer(0x802b45);
    	zfHwSetIFSynthesizer(0x56aff6);
    	zfHwSetIFSynthesizer(0x860e07);
    	zfHwSetIFSynthesizer(0x6efc98);
    	zfHwSetIFSynthesizer(0x221bb9);
    	zfHwSetIFSynthesizer(0xe0000a);
    	zfHwSetIFSynthesizer(0x080b1b);
    	zfHwSetIFSynthesizer(0x00093c);
    	zfHwSetIFSynthesizer(0xffffdd);
    	zfHwSetIFSynthesizer(0x00000e);
    	zfHwSetIFSynthesizer(0x1aba8f);

        //zfwWriteReg(122, 0xe0);
    	//zfwWriteReg(128, 0x14);
    	//zfwWriteReg(129, 0x12);
    	//zfwWriteReg(130, 0x10);

    	zfHwSetIFSynthesizer(AL7230TB[channel*2]);
    	zfHwSetIFSynthesizer(AL7230TB[channel*2+1]);

        //zfwWriteReg(251, 0x3f); //PLL_ON
    }
    else
    {
        //zfwWriteReg(251, 0x2f); //PLL_OFF

        zfHwSetIFSynthesizer(0x451fe2);
    	zfHwSetIFSynthesizer(0x5fdfa3);
    	//zfHwSetIFSynthesizer(0x6fd784);
    	zfHwSetIFSynthesizer(0x851b45);
    	zfHwSetIFSynthesizer(0x56aff6);
    	zfHwSetIFSynthesizer(0x860207);
    	zfHwSetIFSynthesizer(0x6efc98);
    	zfHwSetIFSynthesizer(0x221bb9);
    	zfHwSetIFSynthesizer(0xe0600a);
    	zfHwSetIFSynthesizer(0x080b1b);
    	zfHwSetIFSynthesizer(0x00143c);
    	zfHwSetIFSynthesizer(0xffffdd);
    	zfHwSetIFSynthesizer(0x00000e);
    	zfHwSetIFSynthesizer(0x1abacf);

        //zfwWriteReg(122, 0xfc);
    	//zfwWriteReg(128, 0x14);
    	//zfwWriteReg(129, 0x10);
    	//zfwWriteReg(130, 0x10);

    	if((34 <= channel) && (channel <= 48))
    	{
            channel_temp=(channel/2)-13;
        	zfHwSetIFSynthesizer(AL7230TB_a[channel_temp*4]);
        	zfHwSetIFSynthesizer(AL7230TB_a[channel_temp*4+1]);
        	//zfHwSetIFSynthesizer(AL7230TB_a[channel_temp*4+2]);
        	zfHwSetIFSynthesizer(AL7230TB_a[channel_temp*4+3]);
        }
        else
        {
                channel_temp=(channel/4)-1;
                zfHwSetIFSynthesizer(AL7230TB_a[channel_temp*4]);
        	    zfHwSetIFSynthesizer(AL7230TB_a[channel_temp*4+1]);
        	    //zfHwSetIFSynthesizer(AL7230TB_a[channel_temp*4+2]);
        	    zfHwSetIFSynthesizer(AL7230TB_a[channel_temp*4+3]);
    	}

        //zfwWriteReg(251, 0x3f); //PLL_ON
    }
#endif
}

void zfHwInit(void)
{
    return;
}


#endif //#if ZM_BOARD_TYPE == ZM_BOARD_TYPE_MIMO2
