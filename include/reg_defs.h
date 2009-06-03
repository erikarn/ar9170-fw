#ifndef _REG_DEFS_H
#define _REG_DEFS_H

#include "dt_defs.h"

/* Definition of Share Memory Area */
#if 0
/* 64k */
#define ZM_CMD_BUFFER       0x10F000
#define ZM_RSP_BUFFER       0x10F100
#define ZM_BEACON_BUFFER    0x10F200
//#define ZM_TALLY_BUFFER     0x10F400
//#define ZM_INFO_BUFFER      0x10F500
#else
/* 96k */
#define ZM_BA_BUFFER        0x117780 //Reserve 128
#define ZM_CMD_BUFFER       0x117800 //Reserve 128
#define ZM_RSP_BUFFER       0x117880 //Reserve 128
#endif


/************************* Timer *************************/
/* Definition of interrupt register base address */
#define ZM_TIMER_REGISTER_BASE          0x1C1000

/* Definitions of interrupt register offset */
#define ZM_WATCH_DOG_OFFSET             0x00
#define ZM_TIMER0_OFFSET                0x10
#define ZM_TIMER1_OFFSET                0x14
#define ZM_TIMER2_OFFSET                0x18
#define ZM_TIMER3_OFFSET                0x1C
#define ZM_TIMER4_OFFSET                0x20
#define ZM_TIMER_CONTROL_OFFSET         0x24
#define ZM_TIMER_INTERRUPT_OFFSET       0x28
#define ZM_TICK_TIMER_OFFSET            0x30
#define ZM_CLOCK_LOW_OFFSET             0x40
#define ZM_CLOCK_HIGH_OFFSET            0x44


/* Definitions of interrupt registers */

#define ZM_WATCH_DOG_MAGIC_REG \
    (*((volatile u32_t*)(0x1D4020)))

#define ZM_WATCH_DOG_REG \
    (*((volatile u32_t*)(ZM_TIMER_REGISTER_BASE+ZM_WATCH_DOG_OFFSET)))
    
#define ZM_TIMER_CONTROL_REG \
    (*((volatile u32_t*)(ZM_TIMER_REGISTER_BASE+ZM_TIMER_CONTROL_OFFSET)))

#define ZM_TIMER0_REG \
    (*((volatile u32_t*)(ZM_TIMER_REGISTER_BASE+ZM_TIMER0_OFFSET)))

#define ZM_TIMER1_REG \
    (*((volatile u32_t*)(ZM_TIMER_REGISTER_BASE+ZM_TIMER1_OFFSET)))

#define ZM_TIMER2_REG \
    (*((volatile u32_t*)(ZM_TIMER_REGISTER_BASE+ZM_TIMER2_OFFSET)))

#define ZM_TIMER3_REG \
    (*((volatile u32_t*)(ZM_TIMER_REGISTER_BASE+ZM_TIMER3_OFFSET)))

#define ZM_TIMER4_REG \
    (*((volatile u32_t*)(ZM_TIMER_REGISTER_BASE+ZM_TIMER4_OFFSET)))
    
#define ZM_TICK_TIMER_REG \
    (*((volatile u32_t*)(ZM_TIMER_REGISTER_BASE+ZM_TICK_TIMER_OFFSET)))

#define ZM_TIMER_INTERRUPT_REG \
    (*((volatile u32_t*)(ZM_TIMER_REGISTER_BASE+ZM_TIMER_INTERRUPT_OFFSET)))
    
#define ZM_CLOCK_LOW_REG \
    (*((volatile u32_t*)(ZM_TIMER_REGISTER_BASE+ZM_CLOCK_LOW_OFFSET)))
    
#define ZM_CLOCK_HIGH_REG \
    (*((volatile u32_t*)(ZM_TIMER_REGISTER_BASE+ZM_CLOCK_HIGH_OFFSET)))


/* ZM_TIMER_CONTROL_REG */
#define ZM_DISABLE_CLOCK_BIT             0x100

/* ZM_TIMER_INTERRUPT_REG */
#define ZM_TIMER0_INT_BIT               0x001
#define ZM_TIMER1_INT_BIT               0x002
#define ZM_TIMER2_INT_BIT               0x004
#define ZM_TIMER3_INT_BIT               0x008
#define ZM_TIMER4_INT_BIT               0x010
#define ZM_TICK_TIMER_INT_BIT           0x100



/************************* Interrupt Controller *************************/
#define ZM_MAX_INT_SRC                     9

/* Definition of interrupt register base address */
#define ZM_INT_REGISTER_BASE               0x1D2000

/* Definitions of interrupt register offset */
#define ZM_INT_FLAG_OFFSET                 0x00 
#define ZM_INT_FIQ_MASK_OFFSET             0x04
#define ZM_INT_IRQ_MASK_OFFSET             0x08
#define ZM_INT_PRIORITY1_OFFSET            0x0C
#define ZM_INT_PRIORITY2_OFFSET            0x10
#define ZM_INT_PRIORITY3_OFFSET            0x14
#define ZM_EXT_INT_CONTROL_OFFSET          0x18
#define ZM_SW_INT_CONTROL_OFFSET           0x1C
#define ZM_INT_FIQ_ENCODE_OFFSET           0x20
#define ZM_INT_IRQ_ENCODE_OFFSET           0x24

/* Definitions of interrupt registers */
#define ZM_INT_FLAG_REG \
    (*((volatile u32_t*)(ZM_INT_REGISTER_BASE+ZM_INT_FLAG_OFFSET)))
    
#define ZM_INT_FIQ_MASK_REG \
    (*((volatile u32_t*)(ZM_INT_REGISTER_BASE+ZM_INT_FIQ_MASK_OFFSET)))

#define ZM_INT_IRQ_MASK_REG \
    (*((volatile u32_t*)(ZM_INT_REGISTER_BASE+ZM_INT_IRQ_MASK_OFFSET)))
    
#define ZM_INT_PRIORITY1_REG \
    (*((volatile u32_t*)(ZM_INT_REGISTER_BASE+ZM_INT_PRIORITY1_OFFSET)))
    
#define ZM_INT_PRIORITY2_REG \
    (*((volatile u32_t*)(ZM_INT_REGISTER_BASE+ZM_INT_PRIORITY2_OFFSET)))
    
#define ZM_INT_PRIORITY3_REG \
    (*((volatile u32_t*)(ZM_INT_REGISTER_BASE+ZM_INT_PRIORITY3_OFFSET)))
    
#define ZM_EXT_INT_CONTROL_REG \
    (*((volatile u32_t*)(ZM_INT_REGISTER_BASE+ZM_EXT_INT_CONTROL_OFFSET)))

#define ZM_SW_INT_CONTROL_REG \
    (*((volatile u32_t*)(ZM_INT_REGISTER_BASE+ZM_SW_INT_CONTROL_OFFSET)))
    
#define ZM_INT_FIQ_ENCODE_REG \
    (*((volatile u32_t*)(ZM_INT_REGISTER_BASE+ZM_INT_FIQ_ENCODE_OFFSET)))
    
#define ZM_INT_IRQ_ENCODE_REG \
    (*((volatile u32_t*)(ZM_INT_REGISTER_BASE+ZM_INT_IRQ_ENCODE_OFFSET)))

/* INT_FLAG_REG, INT_FIQ_MASK_REG and INT_IRQ_MASK_REG */
#define ZM_INT_WLAN_BIT                     0x001
#define ZM_INT_PTAB_BIT                     0x002
#define ZM_INT_SE_BIT                       0x004
#define ZM_INT_UART_BIT                     0x008
#define ZM_INT_TIMER_BIT                    0x010
#define ZM_INT_EXT_BIT                      0x020
#define ZM_INT_SW_BIT                       0x040
#define ZM_INT_USB_BIT                      0x080
#define ZM_INT_ETHERNET_BIT                 0x100

/* SW_INT_CONTROL_REG */
#define ZM_SW_INT_ENABLE_BIT                0x1


/************************* Random number generator *************************/
/* Definition of interrupt register base address */
#define ZM_RAND_REGISTER_BASE          0x1D0000

/* Definitions of interrupt register offset */
#define ZM_RAND_NUM_OFFSET              0x00
#define ZM_RAND_MODE_OFFSET             0x04

/* Definitions of interrupt registers */
#define ZM_RAND_NUM_REG \
    (*((volatile u32_t*)(ZM_RAND_REGISTER_BASE+ZM_RAND_NUM_OFFSET)))

#define ZM_RAND_MODE_REG \
    (*((volatile u32_t*)(ZM_RAND_REGISTER_BASE+ZM_RAND_MODE_OFFSET)))
   

/* ZM_RAND_MODE_REG */
#define ZM_RAND_MANUAL_BIT              0x000
#define ZM_RAND_FREE_BIT                0x001


/************************* GPIO/LED *************************/
/* Base address */
#define ZM_GPIO_BASE_ADDRESS                0x1D0100

/* Register offset */
#define ZM_GPIO_PORT_TYPE_OFFSET            0x0
#define ZM_GPIO_PORT_DATA_OFFSET            0x4

/* Register */
#define ZM_GPIO_PORT_TYPE_REG \
    (*((volatile u32_t*)(ZM_GPIO_BASE_ADDRESS+ZM_GPIO_PORT_TYPE_OFFSET)))

#define ZM_GPIO_PORT_DATA_REG \
    (*((volatile u32_t*)(ZM_GPIO_BASE_ADDRESS+ZM_GPIO_PORT_DATA_OFFSET)))

    
/*ZM_GPIO_PORT_DATA_REG */
#define ZM_GPIO0_BIT                        0X0001
#define ZM_GPIO1_BIT                        0X0002
#define ZM_GPIO2_BIT                        0X0004
#define ZM_GPIO3_BIT                        0X0008
#define ZM_GPIO4_BIT                        0X0010
#define ZM_GPIO5_BIT                        0X0020
#define ZM_GPIO6_BIT                        0X0040
#define ZM_GPIO7_BIT                        0X0080
#define ZM_LED0_BIT                         0x010000
#define ZM_LED1_BIT                         0x020000
#define ZM_LED2_BIT                         0x040000
#define ZM_LED3_BIT                         0x080000
#define ZM_LED4_BIT                         0x100000


/************************* High Speed UART *************************/
/* Definition of interrupt register base address */
#define ZM_HUART_REGISTER_BASE              0x1C0000

/* Definitions of interrupt register offset */
#define ZM_HUART_RX_BUFFER_OFFSET           0x00 
#define ZM_HUART_TX_HOLDING_OFFSET          0x04
#define ZM_HUART_FIFO_CONTROL_OFFSET        0x10
#define ZM_HUART_LINE_CONTROL_OFFSET        0x14
#define ZM_HUART_MODEM_CONTROL_OFFSET       0x18
#define ZM_HUART_LINE_STATUS_OFFSET         0x1C
#define ZM_HUART_MODEM_STATUS_OFFSET        0x20
#define ZM_HUART_SCRATCH_OFFSET	            0x24
#define ZM_HUART_DIVISOR_LSB_OFFSET	        0x28
#define ZM_HUART_DIVISOR_MSB_OFFSET	        0x2C
#define ZM_HUART_WORD_RX_BUFFER_OFFSET      0x34 
#define ZM_HUART_WORD_TX_HOLDING_OFFSET     0x38
#define ZM_HUART_FIFO_COUNT_OFFSET	        0x3C
#define ZM_HUART_REMAINDER_OFFSET           0x4C

/* Definitions of interrupt registers */
#define ZM_HUART_RX_BUFFER_REG \
    (*((volatile u32_t*)(ZM_HUART_REGISTER_BASE+ZM_HUART_RX_BUFFER_OFFSET)))
    
#define ZM_HUART_TX_HOLDING_REG \
    (*((volatile u32_t*)(ZM_HUART_REGISTER_BASE+ZM_HUART_TX_HOLDING_OFFSET)))

#define ZM_HUART_FIFO_CONTROL_REG \
    (*((volatile u32_t*)(ZM_HUART_REGISTER_BASE+ZM_HUART_FIFO_CONTROL_OFFSET)))

#define ZM_HUART_LINE_CONTROL_REG \
    (*((volatile u32_t*)(ZM_HUART_REGISTER_BASE+ZM_HUART_LINE_CONTROL_OFFSET)))

#define ZM_HUART_MODEM_CONTROL_REG \
    (*((volatile u32_t*)(ZM_HUART_REGISTER_BASE+ZM_HUART_MODEM_CONTROL_OFFSET)))
    
#define ZM_HUART_LINE_STATUS_REG \
    (*((volatile u32_t*)(ZM_HUART_REGISTER_BASE+ZM_HUART_LINE_STATUS_OFFSET)))

#define ZM_HUART_MODEM_STATUS_REG \
    (*((volatile u32_t*)(ZM_HUART_REGISTER_BASE+ZM_HUART_MODEM_STATUS_OFFSET)))

#define ZM_HUART_SCRATCH_REG \
    (*((volatile u32_t*)(ZM_HUART_REGISTER_BASE+ZM_HUART_SCRATCH_OFFSET)))

#define ZM_HUART_DIVISOR_LSB_REG \
    (*((volatile u32_t*)(ZM_HUART_REGISTER_BASE+ZM_HUART_DIVISOR_LSB_OFFSET)))

#define ZM_HUART_DIVISOR_MSB_REG \
    (*((volatile u32_t*)(ZM_HUART_REGISTER_BASE+ZM_HUART_DIVISOR_MSB_OFFSET)))

#define ZM_HUART_WORD_RX_BUFFER_REG \
  (*((volatile u32_t*)(ZM_HUART_REGISTER_BASE+ZM_HUART_WORD_RX_BUFFER_OFFSET)))
    
#define ZM_HUART_WORD_TX_HOLDING_REG \
  (*((volatile u32_t*)(ZM_HUART_REGISTER_BASE+ZM_HUART_WORD_TX_HOLDING_OFFSET)))

#define ZM_HUART_FIFO_COUNT_REG \
    (*((volatile u32_t*)(ZM_HUART_REGISTER_BASE+ZM_HUART_FIFO_COUNT_OFFSET)))

#define ZM_HUART_REMAINDER_REG \
    (*((volatile u32_t*)(ZM_HUART_REGISTER_BASE+ZM_HUART_REMAINDER_OFFSET)))


/* HUART_FIFO_CONTROL_REG */
#define ZM_RESET_RX_FIFO_BIT            0x02
#define ZM_RESET_TX_FIFO_BIT            0x04

/* HUART_MODEM_CONTROL_REG */
#define ZM_DTR_BIT                      0x01
#define ZM_RTS_BIT                      0x02
#define ZM_INTERNAL_LOOP_BACK_BIT       0x10
#define ZM_AUTO_RTS_BIT                 0x20
#define ZM_AUTO_CTR_BIT                 0x40

/* HUART_LINE_STATUS_REG */
#define ZM_RX_DATA_READY_BIT            0x01
#define ZM_RX_BUFFER_OVERRUN_BIT        0x02
#define ZM_RX_BREAK_INDICATION_BIT      0x10
#define ZM_TX_FIFO_ALMOST_EMPTY_BIT     0x20
#define ZM_TRANSMITTER_EMPTY_BIT        0x40

/* HUART_MODEM_STATUS_REG */
#define ZM_CTS_CHANGE_BIT               0x01
#define ZM_DSR_CHANGE_BIT               0x02
#define ZM_DCD_CHANGE_BIT               0x08
#define ZM_CTS_COMPL_BIT                0x10
#define ZM_DSR_COMPL_BIT                0x20
#define ZM_DCD_COMPL_BIT                0x80


/************************* Memory Control *************************/
/* Definition of Memory Control  register base address */
#define ZM_MC_REGISTER_BASE             0x1D1000

#define ZM_FLASH_WAIT_STATE_OFFSET      0x000

#define ZM_SEEPROM_WP0_OFFSET           0x400
#define ZM_SEEPROM_WP1_OFFSET           0x404
#define ZM_SEEPROM_WP2_OFFSET           0x408

/* Definitions of WLAN MAC registers */
#define ZM_FLASH_WAIT_STATE_REG \
    (*((volatile u32_t*)(ZM_MC_REGISTER_BASE + ZM_FLASH_WAIT_STATE_OFFSET)))

#define ZM_SEEPROM_WP0_REG \
    (*((volatile u32_t*)(ZM_MC_REGISTER_BASE + ZM_SEEPROM_WP0_OFFSET)))
#define ZM_SEEPROM_WP1_REG \
    (*((volatile u32_t*)(ZM_MC_REGISTER_BASE + ZM_SEEPROM_WP1_OFFSET)))
#define ZM_SEEPROM_WP2_REG \
    (*((volatile u32_t*)(ZM_MC_REGISTER_BASE + ZM_SEEPROM_WP2_OFFSET)))

/************************* PCI to AHB Bridge *************************/
/* Definition of Memory Control  register base address */
#define ZM_PTA_REGISTER_BASE            0x1E2000

#define ZM_PTA_CMD_OFFSET               0x000
#define ZM_PTA_PARAM1_OFFSET            0x004
#define ZM_PTA_PARAM2_OFFSET            0x008
#define ZM_PTA_PARAM3_OFFSET            0x00C
#define ZM_PTA_RSP_OFFSET               0x010
#define ZM_PTA_STATUS1_OFFSET           0x014
#define ZM_PTA_STATUS2_OFFSET           0x018
#define ZM_PTA_STATUS3_OFFSET           0x01C
#define ZM_PTA_AHB_INT_FLAG_OFFSET      0x020
#define ZM_PTA_AHB_INT_MASK_OFFSET      0x024
#define ZM_PTA_AHB_INT_ACK_OFFSET       0x028
#define ZM_PTA_AHB_SCRATCH1_OFFSET      0x030
#define ZM_PTA_AHB_SCRATCH2_OFFSET      0x034
#define ZM_PTA_AHB_SCRATCH3_OFFSET      0x038
#define ZM_PTA_AHB_SCRATCH4_OFFSET      0x03C

#define ZM_AHB_INT_FLAG_OFFSET          0x100
#define ZM_PTA_SHARE_MEM_CTRL_OFFSET    0x124

/* Definitions of PCI to AHB Bridge registers */
#define ZM_PTA_CMD_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_CMD_OFFSET)))

#define ZM_PTA_PARAM1_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_PARAM1_OFFSET)))
    
#define ZM_PTA_PARAM2_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_PARAM2_OFFSET)))

#define ZM_PTA_PARAM3_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_PARAM3_OFFSET)))

#define ZM_PTA_RSP_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_RSP_OFFSET)))

#define ZM_PTA_STATUS1_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_STATUS1_OFFSET)))
    
#define ZM_PTA_STATUS2_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_STATUS2_OFFSET)))

#define ZM_PTA_STATUS3_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_STATUS3_OFFSET)))

#define ZM_PTA_AHB_INT_FLAG_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_AHB_INT_FLAG_OFFSET)))
    
#define ZM_AHB_INT_FLAG_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_AHB_INT_FLAG_OFFSET)))

#define ZM_PTA_AHB_INT_MASK_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_AHB_INT_MASK_OFFSET)))
    
#define ZM_PTA_AHB_INT_ACK_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_AHB_INT_ACK_OFFSET)))

#define ZM_PTA_AHB_SCRATCH1_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_AHB_SCRATCH1_OFFSET)))
#define ZM_PTA_AHB_SCRATCH2_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_AHB_SCRATCH2_OFFSET)))
#define ZM_PTA_AHB_SCRATCH3_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_AHB_SCRATCH3_OFFSET)))
#define ZM_PTA_AHB_SCRATCH4_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_AHB_SCRATCH4_OFFSET)))

#define ZM_PTA_SHARE_MEM_CTRL_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE + ZM_PTA_SHARE_MEM_CTRL_OFFSET)))


/* ZM_PTA_INT_FLAG_REG */
#define ZM_PTA_TX_INT                   0x01
#define ZM_PTA_RX_INT                   0x02
#define ZM_PTA_CMD_INT                  0x04


/************************* PCI to AHB Bridge *************************/

#define ZM_PTA_INT_FLAG_OFFSET          0x100
#define ZM_PTA_INT_MASK_OFFSET          0x104
#define ZM_PTA_DN_DMA_ADDRL_OFFSET      0x108
#define ZM_PTA_DN_DMA_ADDRH_OFFSET      0x10C
#define ZM_PTA_UP_DMA_ADDRL_OFFSET      0x110
#define ZM_PTA_UP_DMA_ADDRH_OFFSET      0x114
#define ZM_PTA_DN_PEND_TIME_OFFSET      0x118
#define ZM_PTA_UP_PEND_TIME_OFFSET      0x11C
#define ZM_PTA_CONTROL_OFFSET           0x120
#define ZM_PTA_MEM_CTRL_OFFSET          0x124
#define ZM_PTA_MEM_ADDR_OFFSET          0x128
#define ZM_PTA_DN_DMA_TRIGGER_OFFSET    0x12C
#define ZM_PTA_UP_DMA_TRIGGER_OFFSET    0x130
#define ZM_PTA_DMA_STATUS_OFFSET        0x134
#define ZM_PTA_DN_CURR_ADDRL_OFFSET     0x138
#define ZM_PTA_DN_CURR_ADDRH_OFFSET     0x13C
#define ZM_PTA_UP_CURR_ADDRL_OFFSET     0x140
#define ZM_PTA_UP_CURR_ADDRH_OFFSET     0x144
#define ZM_PTA_DMA_MODE_CTRL_OFFSET     0x148



#define ZM_PTA_INT_FLAG_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_INT_FLAG_OFFSET)))

#define ZM_PTA_INT_MASK_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_INT_MASK_OFFSET)))

#define ZM_PTA_DN_DMA_ADDRL_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_DN_DMA_ADDRL_OFFSET)))

#define ZM_PTA_DN_DMA_ADDRH_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_DN_DMA_ADDRH_OFFSET)))

#define ZM_PTA_UP_DMA_ADDRL_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_UP_DMA_ADDRL_OFFSET)))

#define ZM_PTA_UP_DMA_ADDRH_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_UP_DMA_ADDRH_OFFSET)))

#define ZM_PTA_DN_PEND_TIME_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_DN_PEND_TIME_OFFSET)))

#define ZM_PTA_UP_PEND_TIME_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_UP_PEND_TIME_OFFSET)))

#define ZM_PTA_CONTROL_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_CONTROL_OFFSET)))

#define ZM_PTA_MEM_CTRL_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_MEM_CTRL_OFFSET)))

#define ZM_PTA_MEM_ADDR_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_MEM_ADDR_OFFSET)))

#define ZM_PTA_DN_DMA_TRIGGER_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_DN_DMA_TRIGGER_OFFSET)))

#define ZM_PTA_UP_DMA_TRIGGER_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_UP_DMA_TRIGGER_OFFSET)))

#define ZM_PTA_DMA_STATUS_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_DMA_STATUS_OFFSET)))

#define ZM_PTA_DN_CURR_ADDRL_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_DN_CURR_ADDRL_OFFSET)))

#define ZM_PTA_DN_CURR_ADDRH_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_DN_CURR_ADDRH_OFFSET)))

#define ZM_PTA_UP_CURR_ADDRL_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_UP_CURR_ADDRL_OFFSET)))

#define ZM_PTA_UP_CURR_ADDRH_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_UP_CURR_ADDRH_OFFSET)))

#define ZM_PTA_DMA_MODE_CTRL_REG \
    (*((volatile u32_t*)(ZM_PTA_REGISTER_BASE+ZM_PTA_DMA_MODE_CTRL_OFFSET)))

/* ZM_PTA_INT_FLAG_REG */
#define ZM_PTA_DOWN_INT_BIT     0x1
#define ZM_PTA_UP_INT_BIT       0x2
#define ZM_PTA_CMD_INT_BIT      0x4

/* ZM_PTA_CONTROL_REG */
#define ZM_PTA_4_BEAT_BURST     0x00
#define ZM_PTA_8_BEAT_BURST     0x01
#define ZM_PTA_16_BEAT_BURST    0x02
#define ZM_PTA_LOOPBACK_MODE    0x10


/************************* WLAN Registers *************************/
#define ZM_WL_REGISTER_BASE             0x1c3000

/* Protocol Controller Module */
#define ZM_WL_POWER_STATE_CTRL_OFFSET   0x500
#define ZM_WL_INTERRUPT_CTRL_OFFSET     0x510
#define ZM_WL_ATIM_WINDOW_PERIOD_OFFSET 0x51c
#define ZM_WL_BEACON_PERIOD_OFFSET      0x520

#define ZM_WL_HASH_TABLE_L              0x624
#define ZM_WL_HASH_TABLE_H              0x628
#define ZM_WL_AFTER_PNP                 0x648
#define ZM_WL_MISC_OFFSET               0x680

#define ZM_WL_TX_UNDERRUN_COUNT         0x688

#define ZM_WL_RX_TOTAL_COUNT            0x6A0
#define ZM_WL_RX_CRC32_COUNT            0x6A4
#define ZM_WL_RX_CRC16_COUNT            0x6A8
#define ZM_WL_RX_ERR_UNI                0x6AC
#define ZM_WL_RX_OVERRUN_COUNT          0x6B0
#define ZM_WL_RX_ERR_MUL                0x6BC
#define ZM_WL_RX_TIMEOUT_COUNT          0x69c

#define ZM_WL_TX_RETRY_COUNT            0x6CC
#define ZM_WL_TX_TOTAL_COUNT            0x6F4

#define ZM_CAM_MODE                     0x700
#define ZM_CAM_ROLL_CALL_TABLE_L        0x704
#define ZM_CAM_ROLL_CALL_TABLE_H        0x708
#define ZM_CAM_ADDR                     0x70c
#define ZM_CAM_DATA0                    0x720                 
#define ZM_CAM_DATA1                    0x724                 
#define ZM_CAM_DATA2                    0x728                 
#define ZM_CAM_DATA3                    0x72c 

#define ZM_CAM_DBG0                     0x730                
#define ZM_CAM_DBG1                     0x734                
#define ZM_CAM_DBG2                     0x738
#define ZM_CAM_TXKEY                    0x740  
#define ZM_CAM_RXKEY                    0x750

#define ZM_CAM_TX_ENC_TYPE              0x760
#define ZM_CAM_RX_ENC_TYPE              0x770
#define ZM_CAM_TX_SERACH_HIT            0x780
#define ZM_CAM_RX_SERACH_HIT            0x790                

#define ZM_AMPDU_COUNT_OFF              0xb88
#define ZM_MPDU_COUNT_OFF               0xb8c

#define zm_wl_power_state_ctrl_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_POWER_STATE_CTRL_OFFSET)))

#define zm_wl_int_ctrl_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_INTERRUPT_CTRL_OFFSET)))

#define zm_wl_atim_window_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_ATIM_WINDOW_PERIOD_OFFSET)))    

#define zm_wl_bcn_period_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_BEACON_PERIOD_OFFSET)))    

#define zm_wl_hash_tablel_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_HASH_TABLE_L))) 
#define zm_wl_hash_tableh_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_HASH_TABLE_H))) 
    
#define zm_wl_after_pnp_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_AFTER_PNP)))    

#define zm_wl_misc_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_MISC_OFFSET)))    

#define zm_wl_tx_underrun_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_TX_UNDERRUN_COUNT)))    

#define zm_wl_rx_count_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_RX_TOTAL_COUNT)))    
#define zm_wl_rx_crc32_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_RX_CRC32_COUNT)))    
#define zm_wl_rx_crc16_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_RX_CRC16_COUNT)))    
#define zm_wl_rx_err_uni_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_RX_ERR_UNI)))    
#define zm_wl_rx_overrun_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_RX_OVERRUN_COUNT)))    
#define zm_wl_rx_err_mul_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_RX_ERR_MUL)))    
#define zm_wl_rx_timeout_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_RX_TIMEOUT_COUNT)))    

#define zm_wl_tx_retry_count_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_TX_RETRY_COUNT)))    
#define zm_wl_tx_count_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_TX_TOTAL_COUNT)))    

#define zm_cam_mode_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_MODE)))    
#define zm_cam_roll_call_tablel_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_ROLL_CALL_TABLE_L)))    
#define zm_cam_roll_call_tableh_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_ROLL_CALL_TABLE_H)))    
#define zm_cam_addr_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_ADDR)))    
#define zm_cam_data0_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_DATA0)))    
#define zm_cam_data1_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_DATA1)))    
#define zm_cam_data2_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_DATA2)))    
#define zm_cam_data3_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_DATA3)))    

#define zm_cam_dbg0_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_DBG0)))    
#define zm_cam_dbg1_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_DBG1)))    
#define zm_cam_dbg2_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_DBG2)))    
#define zm_cam_txkey_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_TXKEY)))    
#define zm_cam_rxkey_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_RXKEY)))    

#define zm_cam_txenc_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_TX_ENC_TYPE)))
#define zm_cam_rxenc_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_RX_ENC_TYPE)))
#define zm_cam_txhit_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_TX_SERACH_HIT)))
#define zm_cam_rxhit_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_CAM_RX_SERACH_HIT)))    

#define zm_wl_ampdu_count_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_AMPDU_COUNT_OFF)))    
#define zm_wl_mpdu_count_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_MPDU_COUNT_OFF)))    
    

/* INTERRUPT Flag bits */
#define ZM_INT_TXC_BIT              0x0001
#define ZM_INT_RXC_BIT              0x0002
#define ZM_INT_RETRY_FAIL_BIT       0x0004
#define ZM_INT_WAKEUP_BIT           0x0008
#define ZM_INT_ATIM_BIT             0x0010
#define ZM_INT_DTIM_BIT             0x0020
#define ZM_INT_CFG_BCN_BIT          0x0040
#define ZM_INT_ABORT_BIT            0x0080
#define ZM_INT_QOS_BIT              0x0100
//#define ZM_INT_UART_BIT             0x0200
#define ZM_INT_KEY_GEN_BIT          0x0400
#define ZM_INT_DECRY_NOUSER_BIT     0x0800
#define ZM_INT_RADAR_BIT            0x1000
#define ZM_INT_QUIET_FRAME_BIT      0x2000
#define ZM_INT_PRETBTT_BIT          0x4000




/* BEACON specification bits */
#define ZM_AP_MODE_BIT              0x01000000
#define ZM_IBSS_MODE_BIT            0x02000000
#define ZM_POWER_MNT_BIT            0x04000000
#define ZM_STA_PS_BIT               0x08000000


#define ZM_WL_TXC_BIT               BIT_0
#define ZM_WL_RXC_BIT               BIT_1

/* CAM */
#define HOST_PEND                   0x80000000
#define CAM_WRITE                   0x80000000
#define MAC_LENGTH                  6
#define MIC_FINISH                  0x1



/* DMA Module */
#define ZM_WL_DMA_REG_BASE              0xD00
#define ZM_WL_TXQ0_DMA_ADDR_OFFSET      (0x000 + ZM_WL_DMA_REG_BASE)
#define ZM_WL_TXQ1_DMA_ADDR_OFFSET      (0x008 + ZM_WL_DMA_REG_BASE)
#define ZM_WL_TXQ2_DMA_ADDR_OFFSET      (0x010 + ZM_WL_DMA_REG_BASE)
#define ZM_WL_TXQ3_DMA_ADDR_OFFSET      (0x018 + ZM_WL_DMA_REG_BASE)
#define ZM_WL_TXQ4_DMA_ADDR_OFFSET      (0x020 + ZM_WL_DMA_REG_BASE)
#define ZM_WL_RX_DMA_ADDR_OFFSET        (0x028 + ZM_WL_DMA_REG_BASE)
#define ZM_WL_DMA_TRIGGER_OFFSET        (0x030 + ZM_WL_DMA_REG_BASE)

#define zm_wl_txq0_dma_addr_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_TXQ0_DMA_ADDR_OFFSET)))

#define zm_wl_txq1_dma_addr_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_TXQ1_DMA_ADDR_OFFSET)))

#define zm_wl_txq2_dma_addr_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_TXQ2_DMA_ADDR_OFFSET)))

#define zm_wl_txq3_dma_addr_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_TXQ3_DMA_ADDR_OFFSET)))

#define zm_wl_txq4_dma_addr_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_TXQ4_DMA_ADDR_OFFSET)))
    
#define zm_wl_rx_dma_addr_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_RX_DMA_ADDR_OFFSET)))

#define zm_wl_dma_trigger_reg \
    (*((volatile u32_t*)(ZM_WL_REGISTER_BASE+ZM_WL_DMA_TRIGGER_OFFSET)))


/* WLAN DMA Trigger bits */
#define ZM_TXQ0_TRIG_BIT            0x01
#define ZM_TXQ1_TRIG_BIT            0x02
#define ZM_TXQ2_TRIG_BIT            0x04
#define ZM_TXQ3_TRIG_BIT            0x08
#define ZM_TXQ4_TRIG_BIT            0x10
#define ZM_RXQ_TRIG_BIT             0x100

/* WLAN DMA TxC bits */
#define ZM_TXQ0_TXC_BIT             0x01
#define ZM_TXQ1_TXC_BIT             0x02
#define ZM_TXQ2_TXC_BIT             0x04
#define ZM_TXQ3_TXC_BIT             0x08

/* BEACON DATA STATUS bits */
#define ZM_BCN_READY_BIT            0x01
#define ZM_BCN_LOCK_BIT             0x02


/* Protocol Controller Module */
#define ZM_WL_PC_REG_BASE               0xE00//to be defined


#endif /* #ifndef _REG_DEFS_H */

