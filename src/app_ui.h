/********************************************************************************************************
 * @file    app_ui.h
 *
 * @brief   This is the header file for app_ui
 *
 *******************************************************************************************************/

#ifndef _APP_UI_H_
#define _APP_UI_H_

/**********************************************************************
 * CONSTANT
 */

/**********************************************************************
 * TYPEDEFS
 */

/**********************************************************************
 * FUNCTIONS
 */
void light_blink_start(u8 times, u16 ledOnTime, u16 ledOffTime);
void light_blink_stop(void);

void light_init(void);
void light_on(void);
void light_off(void);

void keys_init(void);
void task_keys(void);

void read_sensor_start(u16 delayTime);

#endif	/* _APP_UI_H_ */
