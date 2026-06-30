/*
 * board_ewn8258_water.h
 *
 * Custom EWN-8258FAT1BA/TLSR8258 water leak sensor board.
 */
#ifndef _BOARD_EWN8258_WATER_H_
#define _BOARD_EWN8258_WATER_H_

#include "version_cfg.h"

#if (BOARD == BOARD_EWN8258_WATER)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_LED)

#define WATER_LEAK_SENSOR       1
#define WATER_PERIODIC_REPORT_SEC 3600
#define ZCL_IAS_ZONE_SUPPORT    1

#define BLE_MODEL_STR           "SNZB-05P"
#define BLE_MAN_STR             "SONOFF"

#define ZCL_BASIC_MFG_NAME      {6,'S','O','N','O','F','F'}
#define ZCL_BASIC_MODEL_ID      {8,'S','N','Z','B','-','0','5','P'}

// Battery & RF power.
#define USE_BATTERY             BATTERY_CR2032

// No periodic environment sensor; wake mainly on water/button.
// Keep within the common ZigbeeTLc reporting/poll interval limit.
#define READ_SENSOR_TIMER_SEC       240
#define READ_SENSOR_TIMER_MAX_SEC   240

// Display.
#define USE_DISPLAY             0

// Button: PC1, active low, one side to GND.
#define BUTTON1                 GPIO_PC1
#define BUTTON1_ON              0
#define PC1_FUNC                AS_GPIO
#define PC1_OUTPUT_ENABLE       0
#define PC1_INPUT_ENABLE        1
#define PULL_WAKEUP_SRC_PC1     PM_PIN_PULLUP_10K

// Water input: PB6, active high. External 3M + capacitor pull it low when dry.
#define GPIO_WATER              GPIO_PB6
#define WATER_ON                1
#define PB6_FUNC                AS_GPIO
#define PB6_OUTPUT_ENABLE       0
#define PB6_INPUT_ENABLE        1
#define PULL_WAKEUP_SRC_PB6     PM_PIN_UP_DOWN_FLOAT

// LED: PC4 drives an NPN base, active high.
#define USE_BLINK_LED           1
#define LED_ON                  1
#define LED_OFF                 0
#define GPIO_LED                GPIO_PC4
#define PC4_FUNC                AS_GPIO
#define PC4_OUTPUT_ENABLE       1
#define PC4_INPUT_ENABLE        0
#define PC4_DATA_OUT            LED_OFF

// No I2C sensor on this board.
#define USE_I2C_DRV             I2C_DRV_NONE
#define USE_SENSOR_CHT8305      0
#define USE_SENSOR_CHT8215      0
#define USE_SENSOR_AHT20_30     0
#define USE_SENSOR_SHT4X        0
#define USE_SENSOR_SHTC3        0
#define USE_SENSOR_SHT30        0

// VBAT ADC path used by pvvx TLSR825x boards.
#define SHL_ADC_VBAT            C5P
#define GPIO_VBAT               GPIO_PC5
#define PC5_INPUT_ENABLE        0
#define PC5_DATA_OUT            1
#define PC5_OUTPUT_ENABLE       1
#define PC5_FUNC                AS_GPIO

#if ZBHCI_UART
    #error please configurate uart PIN!!!!!!
#endif

#if UART_PRINTF_MODE
    #define DEBUG_INFO_TX_PIN   GPIO_SWS
#endif

#define init_sensor()

#endif // BOARD == BOARD_EWN8258_WATER
#endif // _BOARD_EWN8258_WATER_H_
