/************************************************************************/
/*  Copyright (c) 2007  Atheros Communications, Inc.                    */
/*                                                                      */
/*  Module Name : uart.c                                                */
/*                                                                      */
/*  Abstract                                                            */
/*      This module contains Uart interface handle functions.           */
/*                                                                      */
/*  ROUTINES                                                            */
/*                                                                      */
/*      zfGetChar                                                       */
/*      zfPutChar                                                       */
/*      zfCuartOut                                                      */
/*      zfUartSendStr                                                   */
/*      zfItoa16                                                        */
/*      zfUartSendHex                                                   */
/*      zfUartSendHex8                                                  */
/*      zfUartSendStrAndHex                                             */
/*                                                                      */
/*  NOTES                                                               */
/*      None                                                            */
/*                                                                      */
/************************************************************************/
#include "dt_defs.h"
#include "reg_defs.h"
#include "sys_defs.h"

/************************************************************************/
/************************************************************************/
#if 0
u16_t zfGetChar(u32_t ms)
{
    u32_t i = 0;
    u32_t j = 0;

    while (1)
    {
        if ((ZM_HUART_LINE_STATUS_REG & ZM_RX_DATA_READY_BIT)
               == ZM_RX_DATA_READY_BIT)
        {
            return (u16_t)(ZM_HUART_RX_BUFFER_REG&0xff);
        }
        
        if (ms == 0)
        {
            return 0xffff;
        }
        
        if (i++ >= 200)
        {
            j++;
            i = 0;
        }
            
        if (j >= ms)
        {
            return 0xffff;
        }
        
    }
}
#endif


/************************************************************************/
/************************************************************************/
void zfPutChar(u8_t ch)
{
#if 0
    ZM_HUART_TX_HOLDING_REG = ch;

    while ((ZM_HUART_LINE_STATUS_REG & ZM_TX_FIFO_ALMOST_EMPTY_BIT) == 0)
    {
    }
#endif
    return;
}

/************************************************************************/
/************************************************************************/
void zfCuartOut(u8_t* strPtr, u32_t len)
{
    u32_t i;
    
    for (i=0; i< len; i++)
    {
        zfPutChar(strPtr[i]);
    }
}

/************************************************************************/
/************************************************************************/
void zfUartSendStr(u8_t* str)
{
    u32_t i = 0;
    
    while (str[i] != 0)
    {
        i++;
    }
    
    zfCuartOut(str, i);
}

/************************************************************************/
/************************************************************************/
void zfItoa16(u32_t hexValue, u8_t* str)
{
    s32_t i;
    u32_t nibble;
#if 0
    for (i=7; i>=0; i--)
    {
        nibble = (hexValue >> i*4) & 0xf;
        if (nibble < 10)
        {
            str[7-i] = nibble + '0';
        }
        else
        {
            str[7-i] = nibble - 10 + 'A';
        }
    }
#else
    //Remove variable shifting to reduce code size
    for (i=0; i<8; i++)
    {
        nibble = hexValue & 0xf;
        hexValue = hexValue >> 4;
        str[7-i] = nibble + ((nibble < 10)?'0':('A' - 10));
    }
#endif
    str[8] = 0;
    return;
}

/************************************************************************/
/************************************************************************/
int zfUartSendHex(u32_t hex)
{
#if OTUS_NO_SHRINK == 1

    u8_t str[20];

    zfItoa16(hex, str);
    zfUartSendStr(str);
#endif
    
    return 0;
}

int zfUartSendHex8(u32_t hex)
{
#if OTUS_NO_SHRINK == 1

    u8_t str[20];

    zfItoa16(hex, str);
    zfUartSendStr(str+6);

#endif
    
    return 0;
}

/************************************************************************/
/************************************************************************/
void zfUartSendStrAndHex(u8_t* str, u32_t val)
{
    u8_t val_str[20];

    zfItoa16(val, val_str);
    zfUartSendStr(str);
    zfUartSendStr(val_str);
    zfUartSendStr((u8_t*)"\n\r");
}
