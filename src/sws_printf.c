/*****************************************************************
 * @file     sws_printf.c
 *
 * @brief    This is the source file for SWS printf() Version 2.0
 *
 * @author	 pvvx
 *
 ****************************************************************/
#include "tl_common.h"
#include "app_utils.h"

#if USE_PRINTF

#if SWS_PRINTF_MODE

void sws_init(void) {
	psws_buffer->id = 0x55;
	psws_buffer->len = 0;
}

/* Checking and waiting waiting for the transfer to end (buffer to be ready to fill) */
int sws_ready(void) {
	unsigned int tt;
	sws_buffer_t *p = psws_buffer;
	p->id = 0x55;
	if (p->len == SWS_BUF_CLOSED)
		// Wait for the next opening
		return 0;
	tt = clock_time();
	while (p->len != 0) {
		// The transfer is not finished
		if (clock_time() - tt > SWS_WAITING_TIMEOUT) {
			// SDI Timeout, Closed due to timeout
			p->len = SWS_BUF_CLOSED;
			return 0;
		}
	}
	// The buffer has been transmitted or is ready to be filled.
	return 1;
}

/* Write a character to the send buffer */
void sws_putchar(unsigned char c) {
	sws_buffer_t *p = psws_buffer;
	if (sws_ready()) { // The buffer is waiting for new data?
		// Put in buffer
		p->data[p->cur_len] = c;
		p->cur_len++;
		// End of line or maximum transmit buffer size reached?
		if (c == '\n' || p->cur_len >= SWS_BUF_MAX_LEN) {
			// Set the length of the new data transfer
			p->len = p->cur_len;
			p->cur_len = 0;
		}
	}
}

/* Waiting for the transfer to end. (Use before sleep) */
void sws_buffer_flush(void) {
	sws_buffer_t *p = psws_buffer;
	// Checking and waiting waiting for the transfer to end (buffer to be ready to fill)
	if (sws_ready()) {
		// Buffer to be ready to fill.
		if(p->cur_len) { // All data transferred?
			// Set the length of the new data transfer
			p->len = p->cur_len;
			p->cur_len = 0;
			// Waiting for buffer to be transmitted to SWS
			sws_ready();
		}
	}
}

#else // GPIO_PRINTF_MODE

#ifndef DEBUG_INFO_TX_PIN
 #define	DEBUG_INFO_TX_PIN	GPIO_SWS
#endif

#ifndef GPIO_BAUDRATE
 #define	1000000 // 1MBit
#endif


void sws_init(void) {
	gpio_set_func(DEBUG_INFO_TX_PIN, AS_GPIO);
	gpio_set_output_en(DEBUG_INFO_TX_PIN, 1);
	gpio_setup_up_down_resistor(DEBUG_INFO_TX_PIN, PM_PIN_PULLUP_1M);
	gpio_write(DEBUG_INFO_TX_PIN, 1);
}

_attribute_ram_code_
void sws_putchar(unsigned char c)
{
	u8 j = 0;
	u32 t1 = 0, t2 = 0;

	u8 tmp_bit0 = reg_gpio_out(DEBUG_INFO_TX_PIN) & (~(DEBUG_INFO_TX_PIN & 0xff));
	u8 tmp_bit1 = reg_gpio_out(DEBUG_INFO_TX_PIN) | (DEBUG_INFO_TX_PIN & 0xff);
	u8 bit[10] = {0};

	bit[0] = tmp_bit0;
	bit[1] = (c & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[2] = ((c >> 1) & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[3] = ((c >> 2) & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[4] = ((c >> 3) & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[5] = ((c >> 4) & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[6] = ((c >> 5) & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[7] = ((c >> 6) & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[8] = ((c >> 7) & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[9] = tmp_bit1;
	u8 r = irq_disable(); // enable this may disturb time sequence, but if disable unrecognizable code will show
	t1 = clock_time();
	for(j = 0; j < 10; j++){
		t2 = t1;
		while(t1 - t2 < (16000000/GPIO_BAUDRATE)){
			t1 = clock_time();
		}

		reg_gpio_out(DEBUG_INFO_TX_PIN) = bit[j];       //send bit0
	}
	irq_restore(r);
}

#endif // SWS_PRINTF_MODE / GPIO_PRINTF_MODE

//------ sws_puts -----------
/* Write a string to the send buffer */
void sws_puts(char *s)
{
    while ((*s != '\0')) {
        sws_putchar(*s++);
    }
}

//------ printf ----------------------------------------------------

typedef char *va_list;

#define _INTSIZEOF(n)       ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))

#define va_start(ap, v)     (ap = (va_list)&v + _INTSIZEOF(v))
#define va_arg(ap, t)       (*(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))
#define va_end(ap)          (ap = (va_list)0)

#define DECIMAL_OUTPUT      10
#define OCTAL_OUTPUT        8
#define HEX_OUTPUT          16

//const char tab_hex_ascii[] = "0123456789ABCDEF"; // in app_utils.c

static void puti(unsigned int num, int base, int w)
{
	unsigned char buf[50];
    int cnt = 0;

    unsigned char *addr = &buf[49];
    *addr = '\0';

    do {
        *--addr = tab_hex_ascii[num % base];
        num /= base;
        cnt++;
    } while (num != 0 && cnt < 49);

    for ( ; w > cnt; --w) {
        *--addr = '0';
    }

    sws_puts((char *)addr);
}

/* sws print hex dump */
void sws_print_hex_dump(void * d, int len) {
	unsigned char *p = (unsigned char *)d;
	while(len--) {
		unsigned char c = *p++;
		sws_putchar(tab_hex_ascii[c >> 8]);
		sws_putchar(tab_hex_ascii[c & 0x0f]);
	}
}

/* printf */
int sws_printf(const char *format, ...)
{
	unsigned char span;
    unsigned long j;
    unsigned char *s;
    long m;
    int w;

    va_list arg_ptr;
#if SWS_PRINTF_MODE
    sws_buffer_t *p = psws_buffer;
	p->id = 0x55;
    if (p->len == SWS_BUF_CLOSED)
    	// Wait for the next opening
		return 0;
#endif
    va_start(arg_ptr, format);

    while ((span = *(format++))) {
        if (span != '%') {
            sws_putchar(span);
        } else {
            span = *(format++);

            w = 0;
            for ( ; span >= '0' && span <= '9'; span = *(format++)) {
                w *= 10;
                w += span - '0';
            }

            if (span == 'c') {
                j = va_arg(arg_ptr, int);//get value of char
                sws_putchar(j);
            } else if (span == 'd') {
                m = va_arg(arg_ptr, int);//get value of char
                if (m < 0) {
                    sws_putchar('-');
                    m = -m;
                }
                puti(m, DECIMAL_OUTPUT, w);
            } else if (span == 's') {
                s = va_arg(arg_ptr, unsigned char *);//get string value
                sws_puts((char *)s);
//            } else if (span == 'o') {
//                j = va_arg(arg_ptr, unsigned int);//get octal value
//                puti(j, OCTAL_OUTPUT, w);
            } else if (span == 'p') {
                j = va_arg(arg_ptr, unsigned int);//get octal value
                sws_print_hex_dump((void *)j, w);
            } else if (span == 'x') {
                j = va_arg(arg_ptr, unsigned int);//get hex value
                puti(j, HEX_OUTPUT, w);
            } else if (span == 0) {
                break;
            } else {
                sws_putchar(span);
            }
        }
    }

    va_end(arg_ptr);

    return 0;
}

#endif // USE_PRINTF
