/************************************************************************/
/*  Copyright (c) 2007  Atheros Communications, Inc.                    */
/*                                                                      */
/*  Module Name : boot.c                                                */
/*                                                                      */
/*  Abstract                                                            */
/*      This module contains boot and MainLoop functions.               */
/*                                                                      */
/*  ROUTINES                                                            */
/*                                                                      */
/*      zfbooter                                                        */
/*      zfInit                                                          */
/*      zfMainLoop                                                      */
/*      zfIdleTask                                                      */
/*                                                                      */
/*  NOTES                                                               */
/*      None                                                            */
/*                                                                      */
/************************************************************************/
#include "dt_defs.h"
#include "uart_extr.h"
#include "sys_defs.h"
#include "reg_defs.h"
#include "gv_extr.h"
#include "api_extr.h"
#include "ap.h"
#include "usb_defs.h"

//#if ZM_TX_HANG_CHK == 1
#include "desc.h"
//#endif

/* Function prototypes */
void zfInit(void);
void zfMainLoop(void);
void zfIdleTask(void);
void zfFPGAInit(void);
void zfWorkAround_SwReset(void);

/* testing for OTUS phy access with Tx path */
void zfPhyTxPathTest(void);

void zfHwInit(void);

#define ZM_WATCH_DOG_TIMER		   0x100
#define _ZF_WATCH_DOG(time)		   ZM_WATCH_DOG_REG = time

extern void zfDelayMs(u32_t msec);

/************************************************************************/
/*                                                                      */
/*    FUNCTION DESCRIPTION                  zfbooter                    */
/*      ZD Firmware start point                                         */
/*                                                                      */
/*    ROUTINES CALLED                                                   */
/*      zfInit                                                          */
/*      zfMainLoop                                                      */
/*                                                                      */
/*    INPUTS                                                            */
/*      void                                                            */
/*                                                                      */
/*    OUTPUTS                                                           */
/*      void                                                            */
/*                                                                      */
/*    AUTHOR                                                            */
/*      Stephen Chen   Atheros Communications, Inc.         2005.10     */
/*                                                                      */
/************************************************************************/
#define FIRMWARE_VERSION_STRING     __DATE__" "__TIME__
void __attribute__((section(".boot"))) zfbooter(void)
{
    u32_t k;

#if ZM_CLOCK_25M == 1
    ZM_HUART_DIVISOR_LSB_REG = 0xc;
#else
    ZM_HUART_DIVISOR_LSB_REG = 0x14;  /* 40 MHz */
    ZM_HUART_REMAINDER_REG = 0xB38E;
#endif

	/* watchdog magic pattern check */
	if ( (ZM_WATCH_DOG_MAGIC_REG & 0xFFFF0000) == 0x12340000 )
	{
		/* watch dog warm start */
		ZM_WATCH_DOG_MAGIC_REG += 1;
		mUSB_REG_OUT_INT_ENABLE();
	}
	else if ( (ZM_WATCH_DOG_MAGIC_REG & 0xFFFF0000) == 0x98760000 )
	{
		/* suspend/resume */
	}
	/* write the magic pattern for watch dog */
	ZM_WATCH_DOG_MAGIC_REG &= 0xFFFF;
	ZM_WATCH_DOG_MAGIC_REG |= 0x12340000;

    zfHwInit();

    zfInit();

    /* Wait until UART Tx goes to IDLE */
    k = 0;
    while ((ZM_HUART_LINE_STATUS_REG & ZM_TRANSMITTER_EMPTY_BIT) == 0)
    {
        if (k++ >= 10000)
        {
            break;
        }
    }

    zfUartSendStr((u8_t*)"\n\rOTUS FW: SWAP");
    //zfUartSendStr((u8_t*)"               date: ");
    zfUartSendStr((u8_t*)FIRMWARE_VERSION_STRING);
    //zfUartSendStr((u8_t*)"\n\r           note:usb, cam user 64, otus, 96k, r60\n\r");
	zfUartSendStr((u8_t*)"\n\r");

    //zfUartSendStrAndHex((u8_t*)"#ZM_USB =                ", ZM_USB);
    //zfUartSendStrAndHex((u8_t*)"#ZM_CAM_USER_NUM =       ", ZM_CAM_USER_NUM);
    //zfUartSendStrAndHex((u8_t*)"#ZM_FM_LOOPBACK =        ", ZM_FM_LOOPBACK);
    //zfUartSendStrAndHex((u8_t*)"#ZM_CLOCK_25M =          ", ZM_CLOCK_25M);
    //zfUartSendStrAndHex((u8_t*)"#USB_SW_TX_STREAM_MODE = ", USB_SW_TX_STREAM_MODE);
    //zfUartSendStrAndHex((u8_t*)"#USB_TX_STREAM_MODE =    ", USB_TX_STREAM_MODE);
    //zfUartSendStrAndHex((u8_t*)"#USB_RX_STREAM_MODE =    ", USB_RX_STREAM_MODE);
    //zfUartSendStr((u8_t*)"\n\r");

    /* Flash wait state, for FA510 */
    //ZM_FLASH_WAIT_STATE_REG = 0x33;

    zfMainLoop();

    return;
}

/************************************************************************/
/*                                                                      */
/*    FUNCTION DESCRIPTION                  zfInit                      */
/*      Init                                                            */
/*                                                                      */
/*    ROUTINES CALLED                                                   */
/*      zfDmaInitDescriptor                                             */
/*                                                                      */
/*    INPUTS                                                            */
/*      void                                                            */
/*                                                                      */
/*    OUTPUTS                                                           */
/*      void                                                            */
/*                                                                      */
/*    AUTHOR                                                            */
/*      Stephen Chen   Atheros Communications, Inc.             2005.10 */
/*                                                                      */
/************************************************************************/
void zfInit(void)
{
    u16_t ii;

    //zfUartSendStr((u8_t*)"\n\rZD1221 zfInit\n\r");

    /* Uart init */
#if ZM_UART
    //zfUartInit();
#endif


    /* CPU Utilization */
    zfInitTimer0();

    /* USB init */
#if ZM_USB
    zfUsbInit();
#endif

    //PCI only
    //ZM_PTA_MEM_CTRL_REG = 0xE000 | (0x10f000>>11); /* size=1K, enable=1 */

#if ZM_DRV_INIT_USB_MODE == 0
    zfDmaInitDescriptor();
#endif

    /* clear all interrupt */
    zm_wl_int_ctrl_reg = 0xffff;

    zm_wl_after_pnp_reg |= 1;
    zm_wl_hash_tablel_reg = 0xffffffff;
    zm_wl_hash_tableh_reg = 0xffffffff;

    /* Trigger DMA */
#if ZM_DRV_INIT_USB_MODE == 0
    ZM_PTA_DN_DMA_TRIGGER_REG = 1;
#endif
    /* Trigger WLAN RX DMA */
    //zm_wl_dma_trigger_reg |= ZM_RXQ_TRIG_BIT;

#if ZM_USB == 0
    /* Firmware Ready */
    ZM_PTA_AHB_INT_FLAG_REG = 0x8000;
#else
    /* Firmware Ready */
    //ZM_CBUS_FIFO_SIZE_REG = 0xf;
    //ZM_EP0_DATA_REG = 0x12345678;
    //ZM_CX_CONFIG_STATUS_REG = 0x01;
#endif

    /* initialize */
    zgRxMaxSize = 8192;

	zgBAFailRetryCount = 0;

	/* disable all packet dump */
	zgPacketDumpFlag = 0;

	/* MDK mode use */
	zgDontRetransmit = 0;

#if ZM_INT_USE_EP2 == 1
	/* EP2 desc for Interrupt */
	zgIntrINAvailable = 1;
#endif

    /* softap RC */
    zgRCBaGrand = 1;
#if 0
    zgBaStsCurrRate = -1;

    for (ii = 0; ii < sizeof(zgBARate)/sizeof(zgBARate[0]); ii++)
    {
        zgBARate[ii] = 0xffff;
    }
#endif

#if ZM_BAR_AUTO_BA == 1
	zgBAAvailable = 1;
	zgBARGotState = 0;
	zgBARTimeOut  = 0;
#endif

	/* Marvell debug */
	for (ii=0; ii<0x80; ii+=4)
	{
		*(volatile u32_t*)(0x117700 + ii) = 0;
	}

    zgBlockTx = 1;
    zgBlockRx = 1;

	/* Init watch dog control flag */
	u8Watchdog_Enable = 1;
}


/************************************************************************/
/*                                                                      */
/*    FUNCTION DESCRIPTION                  zfMainLoop                  */
/*      Infinite while-loop                                             */
/*                                                                      */
/*    ROUTINES CALLED                                                   */
/*      zfDebugTask                                                     */
/*      zfTicktask                                                      */
/*      zfPciIsr / zfTestPci                op                          */
/*      zfWlanIsr                           op                          */
/*      zfIdleTask                                                      */
/*                                                                      */
/*    INPUTS                                                            */
/*      void                                                            */
/*                                                                      */
/*    OUTPUTS                                                           */
/*      void                                                            */
/*                                                                      */
/*    AUTHOR                                                            */
/*      Stephen Chen   Atheros Communications, Inc.             2005.10 */
/*                                                                      */
/************************************************************************/
void zfMainLoop(void)
{
	u32_t tickCount = 0;

    /* Main loop */
    while (1)
    {

		/* workaround - otherwise : don't use singed bit even unsinged delcare */
		if (tickCount < 0x7fffffff)
		{
			tickCount++;
		}

		//zfUartSendStr((u8_t*)".");
		if (u8Watchdog_Enable == 1)
		{
		    _ZF_WATCH_DOG(ZM_WATCH_DOG_TIMER);
		}

        /* CPU Utilization Monitor */
        //zfMonitorTask();

#if OTUS_NO_SHRINK == 1
        /* if UART Rx Interrupt, call Debug task */
        if ((ZM_HUART_LINE_STATUS_REG & ZM_RX_DATA_READY_BIT) != 0)
        {
            zfDebugTask();
        }
#endif

        /* if tick timer interrupt, call 100ms task */
        //TBD

        /* PCI ISRs */
        zfHostIfIsr();

        /* WLAN ISRs */
        zfWlanIsr();

        /* Schedule transmission packets */
        zfTxSchedulerTask();

        /* USB ISRs */
#if ZM_USB
        zfUsbIsr();
#endif

#if ZM_BAR_AUTO_BA == 1
		if (zgBARGotState == 1)
		{
			if (zgBARTimeOut > 0x5000)
			{
				zgBARGotState = 0;
				/* Recover Ack func */
				*(volatile u32_t*)(0x1c36f8) |= 0x1;
			}
			else
			{
				zgBARTimeOut++;
			}
		}
#endif

//    if ((ZM_TIMER_INTERRUPT_REG == 1) || (zgBaErrorCount > 200) || zgGenBAIntr == 1)
    if (ZM_TIMER_INTERRUPT_REG == 1)
    {

		//#if ZM_BAR_AUTO_BA == 1
		*(volatile u32_t*)(0x117714) = (u32_t)zgTxQ[0].head;
		*(volatile u32_t*)(0x117718) = (u32_t)zgTxQ[0].terminator;

		*(volatile u32_t*)(0x117744) = (u32_t)zgTxQ[1].head;
		*(volatile u32_t*)(0x117748) = (u32_t)zgTxQ[1].terminator;

		*(volatile u32_t*)(0x117734) = (u32_t)zgDnQ.head;
		*(volatile u32_t*)(0x117738) = (u32_t)zgDnQ.terminator;

		*(volatile u32_t*)(0x11771c) = (zgDnQTUCount << 16) | (zgRTQTUCount << 8) | (zgPendingTUCount);
		*(volatile u32_t*)(0x117720) = (u32_t)zgDnQCurrentDescPtr;
		*(volatile u32_t*)(0x117724) = (u32_t)zgRTQ.head;
		*(volatile u32_t*)(0x117728) = (u32_t)zgRTQ.terminator;


#if ZM_TX_DELAY_DESC == 1
#if OTUS_NO_SHRINK == 1
		{
			u8_t i;
			for (i=0; i<ZM_TX_DELAY_DESC_NUM; i++)
			{
				*(volatile u32_t*)(0x117600+i*4) = (u32_t)zgTxDelayDesc[i];

				/* error check */
				if ((zgTxDelayDesc[i]->status & ZM_OWN_BITS_MASK) == ZM_OWN_BITS_HW)
				{
					*(volatile u32_t*)(0x117740) += 1;
				}
			}
			*(volatile u32_t*)(0x117600) |= (zgTxDelayDescIdx<<24);
		}
#endif
#endif

		//#endif

#if ZM_TX_HANG_CHK == 1

        if (ZM_TIMER_INTERRUPT_REG == 1)
        {
			/* Workaround-PendingTUCount */
			if ( ((u32_t)zgTxQ[0].head == (u32_t)zgTxQ[0].terminator) && zgPendingTUCount)
			{
				zgPendingTUCount = 0;
				*(volatile u32_t*)(0x117770) += 1;
				zfPauseUsbDma();
			}

            if (((u32_t)zgTxQ[0].head != (u32_t)zgTxQ[0].terminator) && ((zgTxQ[0].head->status & 0xf) == ZM_OWN_BITS_HW))
            {
                if ((u32_t)zgTxQ[0].head != (u32_t)zgLastTxQHead)
                {
                    zgLastTxQHead = (u32_t)zgTxQ[0].head;
                    zgTxQSameCnt = 0;
                }
                else if ( zgTxCompleted == 0 )
                {
                    if (++zgTxQSameCnt >= ZM_TX_MAX_HANG_NUM)
                    {
                        u32_t Q0TxDmaState = (zm_wl_dma_trigger_reg >> 12) & 0xf;
    					u32_t CurrAddr = (*(volatile u32_t*)(0x1c3dc0) & 0xfffc);
    					struct zsDmaDesc *hwDesc;

						CurrAddr |= 0x100000;
    					hwDesc = (struct zsDmaDesc *)CurrAddr;

#if 1 /* shrink */
					    if (zgTxQSameCnt >= ZM_TX_MAX_HANG_NUM_HAV)
					    {

#if 1
							/* try-1 : good test result only do sw reset. */
							    *(volatile u32_t*)(0x11772c) += 1;
        	                    zgTxQSameCnt = 0;
						        zfWorkAround_SwReset();
#else
							/* try-2 : bad with watchdog */
                            if ( tickCount > 0x100000 )
							{
							    tickCount = 0;
							    *(volatile u32_t*)(0x11772c) += 1;
        	                    zgTxQSameCnt = 0;
						        zfWorkAround_SwReset();
							}
							else
							{
								ZM_WATCH_DOG_MAGIC_REG += 0x100;
								_ZF_WATCH_DOG(1);
								zfDelayMs(10);
							}
#endif

#else /* shrink */
                        if (Q0TxDmaState == 0x0A)
                        {
                            /* Workaround : guess - aggregation + TxUnderRun cause extra-tx retry fail */
							if ( ((*(volatile u32_t*)(0x1c3d3c) >> 12) & 0xf) == 0x8 )
							{
	                            zm_wl_txq0_dma_addr_reg = (u32_t)zgTxQ[0].head | 1;
    	                        zm_wl_dma_trigger_reg = 0x01;

        	                    zgTxQSameCnt = 0;

								*(volatile u32_t*)(0x11772c) += 1;
							}
                        }
						else if (Q0TxDmaState == 0x05)
						{
							/* Workaround : guess - CtrlSetting preload-num and DMA Tx num not match  */
							/* D30:5  D3c:2 */
							if ( (((*(volatile u32_t*)(0x1c3d3c) >> 12) & 0xf) == 0x2) &&
							     ((*(volatile u32_t*)(0x1c3b88)  & 0xffff) == 0)  &&
								 (zgNextStartDesc == (u32_t)zgTxQ[0].head) )
							{
                                /* Put descriptor to the start address */
                                zm_wl_txq0_dma_addr_reg = (u32_t)zgTxQ[0].head;
                                zm_wl_dma_trigger_reg = 0x01;

        	                    zgTxQSameCnt = 0;

								*(volatile u32_t*)(0x11773c) += 1;
							}
							else if ( (((*(volatile u32_t*)(0x1c3d3c) >> 12) & 0xf) == 0x5) &&
							          (zgTxQSameCnt >= (ZM_TX_MAX_HANG_NUM * 8)) )
							{
								/* SwReSet Need */
								/* D30/D3c = 5/5 */
								*(volatile u32_t*)(0x117744) += 1;
								if ( (*(volatile u32_t*)(0x1c3684) & 0xf000 ) == 0x4000 )
								{
									*(volatile u32_t*)(0x117764) += 1;
								}

        	                    zgTxQSameCnt = 0;
								zfWorkAround_SwReset();

							}

						}
						else if (zgTxQSameCnt >= ZM_TX_MAX_HANG_NUM_HAV)
						{
							/* SwReSet Need */
							/* D30/D3c = 3/3 */
							if ( (Q0TxDmaState == 0x03) && (((*(volatile u32_t*)(0x1c3d3c) >> 12) & 0xf) == 0x3))
							{
								*(volatile u32_t*)(0x117748) += 1;
								if ( (*(volatile u32_t*)(0x1c3684) & 0xf000 ) == 0x4000 )
								{
									*(volatile u32_t*)(0x117768) += 1;
								}
        	                    zgTxQSameCnt = 0;
								zfWorkAround_SwReset();
							}
							else
							{
								/* otherwise : do total reset --> watchdog */
								*(volatile u32_t*)(0x11776c) += 1;
								zgTxQSameCnt = 0;

								/* avoid "false alarm" */
								if (tickCount > 0x1ffffff)
								{
									*(volatile u32_t*)(0x11774c) += 1;
									zfWorkAround_SwReset();
									tickCount = 0;
							//	ZM_WATCH_DOG_MAGIC_REG += 0x100;
							//	_ZF_WATCH_DOG(1);
							//	zfDelayMs(10);
								}
							}
#endif /* shrink */

							/* WorkAround detect: DownQ error OwnBit */
#if OTUS_NO_SHRINK == 1
							{
								struct zsDmaDesc *descPtr;

								u32_t finitLoop = 0;


								/* DnQ != 0 */
								descPtr = zgDnQ.head;
								while ( descPtr != zgDnQ.terminator )
								{
									if ( (descPtr->status & ZM_OWN_BITS_MASK) == ZM_OWN_BITS_SW )
									{
										*(volatile u32_t*)(0x117710) = 1;
										//descPtr->status |= ZM_OWN_BITS_HW;

									}
									descPtr = descPtr->nextAddr;

									finitLoop++;
									if (finitLoop>2000)
									{
										*(volatile u32_t*)(0x117710) |= 0xf0000000;
										break;
									}
								}
							}
#endif


						}/* else if (zgTxQSameCnt >= ZM_TX_MAX_HANG_NUM_HAV) */
                    }
                }/* else if ( zgTxCompleted == 0 ) */
				else
				{
					zgTxCompleted = 0;
				}
            }
            else
            {
                /* Set zgTxQSameCnt 0 */
                zgTxQSameCnt = 0;
				zgTxCompleted = 0;
            }
        }
#endif


        /* Clear Timer-0 interrupt flag */
        ZM_TIMER_INTERRUPT_REG = 0x1;

#if 0
        if (zgBaErrorCount != 0)
		{
            //zfGenUsbInterrupt(4, 0xC5, 0x80, &zgBaErrorCount);
            //zgBaErrorCount = 0;
            zfGenerateBAFailCntFrame();
            zgGenBAIntr = 0;
		}
#endif
   }

        /* Call Idle Task */
        //zfIdleTask();

    }/* end of while(1) */
}


void zfWorkAround_SwReset(void)
{
   	u32_t val;
    u32_t agg_wait_counter;
    u32_t agg_density;
	u32_t bcnStartAddr;
	u32_t rctlReg, rcthReg;
    u32_t camModeReg;
    u32_t ackPowerReg;
    u32_t rtsctsTPCReg;
    u32_t rxBBReg;
    u16_t ii;
//	u8_t  chState;

    /* Save aggregation parameters */
    agg_wait_counter = *(volatile u32_t*)(0x1c3b9c);
    agg_density = *(volatile u32_t*)(0x1c3ba0);
	bcnStartAddr = *(volatile u32_t*)(0x1c3d98);
    rctlReg = zm_cam_roll_call_tablel_reg;
    rcthReg = zm_cam_roll_call_tableh_reg;
	camModeReg = zm_cam_mode_reg;
	ackPowerReg = *(volatile u32_t*)(0x1c3694);
	rtsctsTPCReg = *(volatile u32_t*)(0x1c3bb4);
	rxBBReg = *(volatile u32_t*)(0x1c5960); /* 0x1c8960 write only */

    /* TX/RX must be stopped by now */
    val = *(volatile u32_t*)(0x1c3500);
    val |= 0x20;

	/* Manipulate CCA threshold to stop transmission */
//	*(volatile u32_t*)(0x1c5968) = 0x300;

	/* check Rx state in 0(idle) 9(disable) */
//	chState = (0x1c3684 >> 16) & 0xf;
//	while( (chState != 0) && (chState != 9) )
//	{
//		chState = (0x1c3684 >> 16) & 0xf;
//	}

    *(volatile u32_t*)(0x1c3500) = val;

    for (ii = 0; ii < 64;ii++)
    {
    }

    /* Restore aggregation parameters */
    *(volatile u32_t*)(0x1c3b9c) = agg_wait_counter;
    *(volatile  u32_t*)(0x1c3ba0) = agg_density;
	*(volatile u32_t*)(0x1c3d98) = bcnStartAddr;
    zm_cam_roll_call_tablel_reg = rctlReg;
    zm_cam_roll_call_tableh_reg = rcthReg;
	zm_cam_mode_reg = camModeReg;
	*(volatile u32_t*)(0x1c3694) = ackPowerReg;
	*(volatile u32_t*)(0x1c3bb4) = rtsctsTPCReg;
	*(volatile u32_t*)(0x1c8960) = rxBBReg;

	/* Manipulate CCA threshold to resume transmission */
//	*(volatile u32_t*)(0x1c5968) = 0x000;

	/* Discard tx */
	zfRecycleRTQ();

	zfRecycleTxQ(0);
	zfRecycleTxQ(1);

	/* rewrite TxQ0 start addr */
	zm_wl_txq0_dma_addr_reg = (u32_t)zgTxQ[0].head;
	zm_wl_txq1_dma_addr_reg = (u32_t)zgTxQ[1].head;
	//zm_wl_dma_trigger_reg = ZM_TXQ0_TRIG_BIT;

    /* Restart RxQ */
    zfRecycleRxQ();

    /* Trigger Rx DMA */
    zm_wl_dma_trigger_reg = 0x100;
}

#if 0
/************************************************************************/
/*                                                                      */
/*    FUNCTION DESCRIPTION                  zfIdleTask                  */
/*      Idle counter                                                    */
/*                                                                      */
/*    ROUTINES CALLED                                                   */
/*                                                                      */
/*    INPUTS                                                            */
/*      void                                                            */
/*                                                                      */
/*    OUTPUTS                                                           */
/*      void                                                            */
/*                                                                      */
/*    AUTHOR                                                            */
/*      Stephen Chen   Atheros Communications, Inc.             2005.10 */
/*                                                                      */
/************************************************************************/
void zfIdleTask(void)
{
    zgIdleCount++;
}

#endif
