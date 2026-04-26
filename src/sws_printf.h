/****************************************************************
 * @file    sws_printf.h
 *
 * @brief   This is the header file for SWS printf() Version 2.0
 *
 * @author	 pvvx
 *
 ***************************************************************/
#ifndef _INC_SWS_PRINTF_H
#define _INC_SWS_PRINTF_H

//#define GPIO_PRINTF_MODE	1
//#define SWS_PRINTF_MODE	1

#define USE_PRINTF (SWS_PRINTF_MODE || GPIO_PRINTF_MODE)

#if USE_PRINTF

#if SWS_PRINTF_MODE
/* The maximum transmit buffer size is 254 bytes.
   Size reduction is possible. */
#define	SWS_BUF_MAX_LEN	253

/* SDI Off or Closed due to timeout, Wait for the next opening */
#define	SWS_BUF_CLOSED	255

/* Waiting timeout - 22 ms (proportional to 230400 UART baud rate)
  It is advisable to set the COM connection speed
  in TlsrPgm.py to 500000 or 921600 Baud.*/
#define SWS_WAITING_TIMEOUT	(30 * CLOCK_16M_SYS_TIMER_CLK_1MS)

/* SWS send buffer */
typedef struct {
	/* id */
	unsigned char id; // = 0x55 -> test TLSRPGM
	/* if len = 255 -> SWS transmission is disabled,
	   Timeout in SWS transmission, Wait for the next opening
	   if len = 0 -> The buffer is ready to be filled
	   if len = 1..MAX_SWS_BUF_LEN ->
	   Number of characters for translation via SWS - data buffer busy */
	volatile unsigned char len;
	/* Character send buffer */
	unsigned char data[SWS_BUF_MAX_LEN];
	/* Number of characters in the buffer */
	unsigned char cur_len;
} sws_buffer_t;

/* SWS send buffer */
// sws_buffer_t sws_buffer; or fixed address:
#define psws_buffer ((sws_buffer_t *)(0x847F00))

/* Checking and waiting waiting for the transfer to end (buffer to be ready to fill) */
int sws_ready(void);
/* Waiting for the transfer to end. (Use before sleep) */
void sws_buffer_flush(void);


#else // if GPIO_PRINTF_MODE

#define GPIO_BAUDRATE	1000000 //1M

#define sws_ready()
#define sws_buffer_flush()

#endif // SWS_PRINTF_MODE / GPIO_PRINTF_MODE

void sws_init(void);

/* Write a string to the send buffer */
void sws_puts(char *s);
/* Write a character to the send buffer */
void sws_putchar(unsigned char c);
/* printf */
int sws_printf(const char *format, ...);

void sws_print_hex_dump(void * pdata, int len);

#else // USE_PRINTF

#define sws_init()
#define sws_buffer_flush()
#define sws_ready()
#define sws_puts(...)
#define sws_printf(...)
#define sws_print_hex_dump(...)

#endif // USE_PRINTF

//#define printf sws_printf

#endif //_INC_SWS_PRINTF_H

