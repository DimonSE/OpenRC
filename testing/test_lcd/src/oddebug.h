/* Name: oddebug.h
 * Project: AVR library
 * Author: Christian Starkjohann
 * Creation Date: 2005-01-16
 * Tabsize: 4
 * Copyright: (c) 2005 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 */

#ifndef __oddebug_h_included__
#define __oddebug_h_included__

/*
General Description:
This module implements a function for debug logs on the serial line of the
AVR microcontroller. Debugging can be configured with the define
'DEBUG_LEVEL'. If this macro is not defined or defined to 0, all debugging
calls are no-ops. If it is 1, DBG1 logs will appear, but not DBG2. If it is
2, DBG1 and DBG2 logs will be printed.

A debug log consists of a label ('prefix') to indicate which debug log created
the output and a memory block to dump in hex ('data' and 'len').
*/


#ifndef F_CPU
#define  F_CPU   16000000    /* 16 MHz */
#endif

#ifndef uchar
#define  uchar unsigned char
#endif

#if DEBUG_LEVEL > 0 && !(defined TXEN || defined TXEN0) /* no UART in device */
#   warning "Debugging disabled because device has no UART"
#   undef   DEBUG_LEVEL
#endif

#ifndef DEBUG_LEVEL
#define  DEBUG_LEVEL 0
#endif

/* ------------------------------------------------------------------------- */

#if DEBUG_LEVEL > 0
#define  DBG1(prefix, data, len) odDebug(prefix, data, len)
#else
#define  DBG1(prefix, data, len)
#endif

#if DEBUG_LEVEL > 1
#define  DBG2(prefix, data, len) odDebug(prefix, data, len)
#else
#define  DBG2(prefix, data, len)
#endif

/* ------------------------------------------------------------------------- */

#if DEBUG_LEVEL > 0

/* Try to find our control registers; ATMEL likes to rename these */

#if defined UBRR
#define  ODDBG_UBRR  UBRR
#elif defined UBRRL
#define  ODDBG_UBRR  UBRRL
#elif defined UBRR0
#define  ODDBG_UBRR  UBRR0
#elif defined UBRR0L
#define  ODDBG_UBRR  UBRR0L
#endif

#if defined UCR
#define  ODDBG_UCR   UCR
#elif defined UCSRB
#define  ODDBG_UCR   UCSRB
#elif defined UCSR0B
#define  ODDBG_UCR   UCSR0B
#endif

#if defined TXEN
#define  ODDBG_TXEN  TXEN
#else
#define  ODDBG_TXEN  TXEN0
#endif

#if defined USR
#define  ODDBG_USR   USR
#elif defined UCSRA
#define  ODDBG_USR   UCSRA
#elif defined UCSR0A
#define  ODDBG_USR   UCSR0A
#endif

#if defined UDRE
#define  ODDBG_UDRE  UDRE
#else
#define  ODDBG_UDRE  UDRE0
#endif

#if defined UDR
#define  ODDBG_UDR   UDR
#elif defined UDR0
#define  ODDBG_UDR   UDR0
#endif

void uartPutc(char c)
{
    while(!(ODDBG_USR & (1 << ODDBG_UDRE)));    /* wait for data register empty */
    ODDBG_UDR = c;
}

uchar hexAscii(uchar h)
{
    h &= 0xf;
    if(h >= 10)
        h += 'a' - (uchar)10 - '0';
    h += '0';
    return h;
}

void printHex(uchar c)
{
    uartPutc(hexAscii(c >> 4));
    uartPutc(hexAscii(c));
}

void odDebug(uchar prefix, const char* data, uchar len)
{
    printHex(prefix);
    uartPutc(':');
    while(len--){
        uartPutc(' ');
        uartPutc(*data++);
    }
    uartPutc('\r');
    uartPutc('\n');
}

static inline void  odDebugInit(void)
{
    ODDBG_UCR |= (1<<ODDBG_TXEN);
    ODDBG_UBRR = F_CPU / (9600 * 16L) - 1;
}
#else
#define odDebugInit()
#endif

/* ------------------------------------------------------------------------- */

#endif /* __oddebug_h_included__ */