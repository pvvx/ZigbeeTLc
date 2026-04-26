/*
 * app_utils.c
 *
 *  Created on: 25 апр. 2026 г.
 *      Author: pvvx
 */


const char tab_hex_ascii[] = { "0123456789ABCDEF" };

unsigned char * str_bin2hex(unsigned char *d, unsigned char *s, int len) {
	while(len--) {
		*d++ = tab_hex_ascii[(*s >> 4) & 0xf];
		*d++ = tab_hex_ascii[(*s++ >> 0) & 0xf];
	}
	return d;
}
