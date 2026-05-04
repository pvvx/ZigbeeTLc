/**
 * @file sensor_xbr818.h
 * @author pvvx
 */

#ifndef _SENSOR_XBR818_H
#define _SENSOR_XBR818_H

#define DEF_OCCUPANCY_THRES	(250>>1)

int xbr818_init(void);
int xbr818_set_cfg(void);
//void xbr818_activate(void);
//int xbr818_write_regs(u8 raddr, u8 * pdata, int size);
//int xbr818_read_regs(u8 raddr, u8 *pdata, int size);
void xbr818_go_sleep(void);

#endif // _SENSOR_XBR818_H
