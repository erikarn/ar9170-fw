#ifndef _SYS_DEFS_H
#define _SYS_DEFS_H

/* AGG Reclaim */
/* 0: Disable
 * 1: Reclaim this packet if retry counter greater than 3
 * 2: Retransmit this packet to single packet mode
 */
#define ZM_AGGREGATION_RECLAIM  2

#define ZM_TX_HANG_CHK            1

/* 1 to include test code, 0 to exclude test code  */

/* DeBug mode base include: Uart input and output */
#define ZM_UART		        	1

#define ZM_CLOCK_25M            0	/* 0=>40MHz clock */
				    /* 1=>25MHz clock */

/* fix: interrupt in use EP2(data in) replace EP3
 *         0 : normal use EP3
 *         1 : fix use EP2
 */
#define ZM_INT_USE_EP2               1
#define ZM_INT_USE_EP2_HEADER_SIZE  12

/* BAR fix with Marvell */
#define ZM_TX_DELAY_DESC			 1

#define ZM_MHZ_CLOCK                    40
#define ZM_NANOSECOND_PER_CLOCK         (1000/ZM_MHZ_CLOCK)
#define ZM_CLOCK_PER_MICORSECOND        (ZM_MHZ_CLOCK)

#define ZM_TIMER_TIME_UNIT_IN_MICROSEC  (80/ZM_MHZ_CLOCK)	//Microsecond

#endif
