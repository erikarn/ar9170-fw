#include "dt_defs.h"
#include "uart_extr.h"
#include "sys_defs.h"
#include "reg_defs.h"
#include "gv_extr.h"
#include "api_extr.h"
#include "ap.h"

//#define ZM_MONITOR_BUFFER_SIZE      64 /*Must be 2^N */

//u32_t zgPreIdleCount;
//u32_t zgAvgDelta;
//u8_t zgMonitorBuffer[ZM_MONITOR_BUFFER_SIZE];
//u16_t zgMonitorPointer;

extern int db_int_to_ascii(unsigned long int_num, char* num_str);

void zfInitTimer0(void)
{
    /* Init average delta */
    //zgAvgDelta = 1;
    
    //zgMonitorPointer = 0;

    /* Init timer */
    /* Set timer-0 to periodic mode */
    ZM_TIMER_CONTROL_REG = 0x1;
    
    /* Set time-0 = 50ms */
    ZM_TIMER0_REG = (50000-1);
	/* OTUS FB50 */
    //ZM_TIMER0_REG = (200000-1);
    
    /* Clear Timer-0 interrupt flag */
    ZM_TIMER_INTERRUPT_REG = 0x1;
}

#if OTUS_NO_SHRINK == 1

void zfMonitorTask(void)
{
    //u32_t delta;
    //u32_t idleCount;

    /* Check timer interrupt flag */
    if (ZM_TIMER_INTERRUPT_REG == 1)
    {
        /* Clear Timer-0 interrupt flag */
        ZM_TIMER_INTERRUPT_REG = 0x1;
        
    //    idleCount = zgIdleCount;
    //    delta = idleCount - zgPreIdleCount;
    //    zgPreIdleCount = idleCount;
        
    //    zgMonitorBuffer[zgMonitorPointer] = (delta < zgAvgDelta)
    //                ? (((zgAvgDelta - delta)*100) / zgAvgDelta) : 0;    

    //    if (zgAvgDelta < delta)
    //    {
    //        zgAvgDelta = delta;
    //    }

    //    zgMonitorPointer++;
    //    zgMonitorPointer &= (ZM_MONITOR_BUFFER_SIZE-1);

		/* Update Tally */
		zgTally.RxTotalCount += zm_wl_rx_count_reg;
	    zgTally.RxCRC32      += zm_wl_rx_crc32_reg;
	    zgTally.RxCRC16      += zm_wl_rx_crc16_reg;
	    zgTally.RxOverRun    += zm_wl_rx_overrun_reg;
	    zgTally.RxTimeOut    += zm_wl_rx_timeout_reg;
	    zgTally.RxUniError   += zm_wl_rx_err_uni_reg;
	    zgTally.RxMulError   += zm_wl_rx_err_mul_reg;

        zgTally.RxMPDU                += *(volatile u32_t*)0x1c3ca0;
        zgTally.RxDropMPDU            += *(volatile u32_t*)0x1c3ca4;
        zgTally.RxDelMPDU             += *(volatile u32_t*)0x1c3ca8;

        zgTally.RxPhyMiscError        += *(volatile u32_t*)0x1c3cac;
        zgTally.RxPhyXRError          += *(volatile u32_t*)0x1c3cb0;
        zgTally.RxPhyOFDMError        += *(volatile u32_t*)0x1c3cb4;
        zgTally.RxPhyCCKError         += *(volatile u32_t*)0x1c3cb8;
        zgTally.RxPhyHTError          += *(volatile u32_t*)0x1c3cbc;
        zgTally.RxPhyTotalCount       += *(volatile u32_t*)0x1c3cc0;
   
	    zgTally.TxTotalCount += zm_wl_tx_count_reg;
	    zgTally.TxUnderRun   += zm_wl_tx_underrun_reg;
	    zgTally.TxRetryCount += zm_wl_tx_retry_count_reg;

        zgTally.TxAmpduCnt += zm_wl_ampdu_count_reg >> 16;
        zgTally.TxMpduCnt += zm_wl_mpdu_count_reg >> 16;
    }
}


void zfPrintCpuUtilization(void)
{
    u32_t i;
    u16_t originalPosition;
    u16_t position;
    char str[20];
    u32_t j;
    u8_t ch;
    u16_t temp;

    originalPosition = (zgMonitorPointer-1) & (ZM_MONITOR_BUFFER_SIZE-1);
    position = originalPosition;

#if 0
    for (i=0; i<(ZM_MONITOR_BUFFER_SIZE-1); i++)
    {
        db_int_to_ascii(zgMonitorBuffer[position--], str);
        position &= (ZM_MONITOR_BUFFER_SIZE-1);

        zfUartSendStr((u8_t*)str);
        zfUartSendStr((u8_t*)"% ");
    }
    
    //zfSendHex(avgDelta);
#endif
    
    //zfSleepMilliSecond(10);
    zfUartSendStr((u8_t*)"\n\r");

    for (j=100; j>0; j-=10)
    {
        if (j == 10)
        {
            str[0] = '0';
            str[1] = '0';
            str[2] = 0;
        }
        else
        {
            db_int_to_ascii(j-10, str);
        }
        zfUartSendStr((u8_t*)str);
        zfUartSendStr((u8_t*)"% ");
        
        position = originalPosition;
        for (i=0; i<(ZM_MONITOR_BUFFER_SIZE-2); i++)
        {
#if 1
            if ((zgMonitorBuffer[position]) > (j-10))
            {
                temp = zgMonitorBuffer[position] + 10 - j;
                if (temp > 10)
                {
                    temp = 10;
                }
                ch = '0' + temp;
                zfPutChar(ch);
            }
            else
            {
                zfUartSendStr((u8_t*)" ");
            }
#endif
            position--;
            position &= (ZM_MONITOR_BUFFER_SIZE-1);
        }
        zfUartSendStr((u8_t*)"\n\r");
        //zfSleepMilliSecond(10);
    }
    
    return;
}

#endif
