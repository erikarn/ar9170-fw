/************************************************************************/
/*                                                                      */
/*  FILE DESCRIPTION                                                    */
/*                                                                      */
/*  This file contains console debug command table structure and        */
/*  relative defination.                                                */
/*                                                                      */
/*  ROUTINES                                                            */
/*                                                                      */
/*      None                                                            */
/*                                                                      */
/*  NOTES                                                               */
/*                                                                      */
/*      None                                                            */
/*                                                                      */
/************************************************************************/
#ifndef _DB_DEFS_H
#define _DB_DEFS_H

#define ZM_DEBUGGER_VERSION_STR     "ZB1016 Debugger v0.01\n"
#define ZM_COMMAND_LIST_STR         "===Command Listing===\n"

#define DB_MAX_COMMAND_LENGTH       40   /* Maximum command line length */
#define COMMAND_BUFFER_SIZE         6    /* Maximum command buffer size */

struct DB_COMMAND_STRUCT
{
    char* cmd_str;                         /* Command string */
    char* help_str;                        /* Help description */
    int (*cmd_func)(char *, char*, char*, char*);  /* Command function */
};

#define zm_uart_send(x, y) zfCuartOut(((u8_t*)x), (y))

#endif