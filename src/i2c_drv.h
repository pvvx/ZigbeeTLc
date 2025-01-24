#ifndef _I2C_DRV_H_
#define _I2C_DRV_H_

#if USE_I2C_DRV

/* Universal I2C/SMBUS read-write transaction struct */
typedef struct _i2c_utr_t {
	unsigned char mode;  // bit0..6: The byte number of the record for the new START (bit7: =1 - generate STOP/START)
	unsigned char rdlen; // bit0..6: Number of bytes read (bit7: =0 - the last byte read generates NACK, =1 - ACK)
	unsigned char wrdata[1]; // Array, the first byte is the address, then the bytes to write to the bus: i2c_addr_wr, wr_byte1, wr_byte2, wr_byte3, ... wr_byte126
} i2c_utr_t;

void init_i2c(void);

/* scan_i2c_addr() return: address (=0 - NAK) */
unsigned char scan_i2c_addr(unsigned char address);
/* send_i2c_byte() return: NAK (=0 - send ok) */
int send_i2c_byte(unsigned char i2c_addr, unsigned char cmd);
/* send_i2c_word() return: NAK (=0 - send ok) */
int send_i2c_word(unsigned char i2c_addr, unsigned short w);
/* send_i2c_bytes() return: NAK (=0 - send ok) */
int send_i2c_bytes(unsigned char i2c_addr, unsigned char * dataBuf, size_t dataLen);
/* read_i2c_byte() return: data (=-1 - read error) */
int read_i2c_byte(unsigned char  i2c_addr);
/* read_i2c_bytes() return: NAK (=0 - read ok) */
int read_i2c_bytes(unsigned char i2c_addr, unsigned char * dataBuf, int dataLen);
/* read_i2cread_i2c_addr_bytes() return: NAK (=0 - read ok) */
int read_i2c_addr_bytes(unsigned char i2c_addr, unsigned char reg_addr, unsigned char * dataBuf, int dataLen);

#if USE_I2C_DRV == I2C_DRV_HARD // Hardware I2C
int I2CBusUtr(void * outdata, i2c_utr_t *tr, unsigned int wrlen);
//#else  // Soft I2C (USE_I2C_DRV == I2C_DRV_SOFT)
#endif

#endif // USE_I2C_DRV

#endif /* _I2C_DRV_H_ */
