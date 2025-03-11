# Custom firmware for Zigbee 3.0 IoT devices on the TLSR825x chip

Based on an example from  [Telink's Zigbee SDK](http://wiki.telink-semi.cn/wiki/chip-series/TLSR825x-Series/).

Currently supported devices: [LYWSD03MMC](https://pvvx.github.io/ATC_MiThermometer/),
 [CGG1](https://pvvx.github.io/CGG1/),
 [CGG1N](https://pvvx.github.io/CGG1_2022),
 [CGDK2](https://pvvx.github.io/CGDK2/),
 [MHO-C122](https://pvvx.github.io/MHO_C122),
 [MHO-C401(old)](https://pvvx.github.io/MHO_C401),
 [MHO-C401N](https://pvvx.github.io/MHO_C401N),
 [TS0201_TZ3000](https://pvvx.github.io/TS0201_TZ3000),
 [TH03Z](https://pvvx.github.io/TH03Z/),
 [TH03](https://pvvx.github.io/TS0201_TZ3000_TH03),
 [LKTMZL02](https://pvvx.github.io/LKTMZL02),
 [ZG-227Z](https://pvvx.github.io/ZG-227Z),
 [ZY-ZTH02](https://pvvx.github.io/ZY-ZTH02),
 [ZY-ZTH02Pro](https://pvvx.github.io/ZY-ZTH02Pro),
 [ZTH01](https://pvvx.github.io/TS0601_TZE200_zth01/).

* Work with modification:
 [ZTH02](https://pvvx.github.io/TS0601_TZE200_zth02/),
 [ZTH05](https://pvvx.github.io/TS0601_TZE204).

Additional alternative firmware options for some devices with TLSR825x can be found at [doctor64/tuyaZigbee](https://github.com/doctor64/tuyaZigbee)

Please direct questions regarding use and configuration in [ZHA](https://www.home-assistant.io/integrations/zha/), [Zigbee2MQTT](https://www.zigbee2mqtt.io/), [Home Assistant](https://www.home-assistant.io/) to the appropriate resources.

* Due to high consumption, limited transmit power of +2 dBm is used. A moderately discharged CR2032 battery cannot withstand higher power.
* [Average consumption](https://github.com/pvvx/ZigbeeTLc/issues/37#issuecomment-1937844181) for Xiaomi LYWSD03MMC HW: B1.4 - ~15 uA.

## To flash the Zigbee firmware, use a Google Chrome, Microsoft Edge or Opera Browser.*

1. Go to the [Over-the-air Webupdater Page TelinkMiFlasher.html](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html)
2. If using Android, Windows, Linux: Ensure you enabled "experimental web platform features". Therefore copy the according link (i.e. chrome://flags/#enable-experimental-web-platform-features for Chrome), open a new browser tab, paste the copied URL. Now sten the _Experimental Web Platform features_ flag to _Enabled_. Then restart the browser.
3. In the Telink Flasher Page: Press `Connect:`. The browser should open a popup with visible Bluetooth devices. Choose the according target device (i.e. LYWSD03MMC) to pair.
4. After connection is established a _Do Acivation_ button appears. Press this button to start the decryption key process.
5. Now you can press the Zigbee Firmware button to directly flash the `Zigbee Firmware`:<br>Alternatively you can choose a specific firmware binary (i.e. the original firmware) via the file chooser. Example: in the file name field paste "https://github.com/devbis/z03mmc/releases/download/1.0.6/1141-0203-10063001-z03mmc.zigbee". If the firmware file is larger than 128 kilobytes, then you will have to download BLE version 4.6 or later. BLE versions below 4.6 do not support OTA downloads with fw sizes larger than 128 kilobytes. Xiaomi firmwares, excluding MJWSD05MMC, also do not support OTA downloads with fw sizes larger than 128 kilobytes.
6. Press Start Flashing. Wait for the firmware to finish.
7. The device should now show up in your Zigbee bridge (If joining is enabled, of course). If this does not happen, reinsert the battery and/or short-circuit the RESET and GND pins on the LYWSD03MMC board, and on sensors with a button, press the button and hold it for 7..8 seconds.

## Action of the button

Action of the button (or contact "reset" to "gnd" for LYWSD03MMC):

* Short press - send temperature, humidity and battery data.
* Hold for 2 seconds - Switches temperature between Celsius and Fahrenheit. The change will immediately appear on the screen.
* Hold for 7 seconds - reset the binding and restart. The screen will briefly display “---” (for devices using a screen with fast refresh capabilities, not E-Ink). After restarting, the thermometer will wait forever for registration. Jerking the battery won't change anything.

* After flashing from BLE, the thermometer is already in standby mode for registration in the Zigbee network.

## Additional settings

_For firmware version 0.1.2.1_

### ClusterID: 0x0000 (Basic)
Attr: 0x0005, CHAR_STR (id: 0x42), ModelIdentifer (Device Name): 3..23 Chars.

### ClusterID: 0x0204 (Thermostat User Interface Configuration)
Attr: 0x0000, ENUM8 (id:0x30). 0 – Celsius, 1- Fahrenheit. Default 0.<br>
Attr: 0x0002, ENUM8 (id:0x30). 0 - Show smiley, 1 - Don't show smiley. Default 0.<br>
Attr: 0x0100, INT16 (id:0x29), Temperature offset, in 0.01° steps, range: -32767 (-327.67°)..32767(+327.67°). Default 0.<br>
Attr: 0x0101, INT16 (id:0x29), Humidity offset, in 0.01% steps, range: -32767 (-327.67%)..32767(+327.67%). Default 0.<br>
Attr: 0x0102, INT16 (id:0x29), Comfort temperature minimum, in 0.01° steps, range -32767..+32767 (-327.67° ..+327.67°). Default 2000 (20.00°C).<br>
Attr: 0x0103, INT16 (id:0x29), Comfort temperature maximum, in 0.01° steps, range -32767..+32767 (-327.67° ..+327.67°). Default 2500 (25.00°C).<br>
Attr: 0x0104, UINT16 (id:0x21), Comfort humidity minimum, in 1% steps, range 0..9999 (0..99.99%). Default 4000 (40.00%).<br>
Attr: 0x0105, UINT16 (id:0x21), Comfort humidity maximum, in 1% steps, range 0..9999 (0..99.99%). Default 6000 (60.00%).<br>
Attr: 0x0106, ENUM8 (id:0x30), Turn off the display. 1 - Display Off. Default 0 - Display On.<br>
Attr: 0x0107, UINT8 (id:0x20), Measurement interval, range: 3..255 seconds. Default 10 seconds.

•	Not on all HW variants, turning off the display leads to a significant reduction in consumption. Better results are obtained by increasing the measurement interval.

### Default Report Settings

Power configuration (ClusterID: 0x0001), Battery Voltage (Attr: 0x0020). Min interval 360 sec, Max interval 3600 sec, Tolerance 0.<br>
Power configuration (ClusterID: 0x0001), Battery Percentage Remaining (Attr: 0x0021). Min interval 360 sec, Max interval 3600 sec, Tolerance 0.<br>
Temperature Measurement (ClusterID: 0x0402), Measured Value (Attr: 0x0000). Min interval 30 sec, Max interval 180 sec, Tolerance 10 (0.1°).<br>
Relative Humidity Measurement (ClusterID: 0x0405), Measured Value (Attr: 0x0000). Min interval 30 sec, Max interval 180 sec, Tolerance 50 (0.5%).

## Z2M

Use [External convertors](https://github.com/pvvx/ZigbeeTLc/tree/master/z2m) (for now for version 0.1.2.0)

## Zigbee OTA

[OTA update in ZHA](https://github.com/pvvx/ZigbeeTLc/issues/7)

Before flashing the firmware, check the compliance of the “Manufacturer Code” and “Image Type” between the OTA file and those issued by the Zigbee device.

Sample: "1141-020a-01233001-Z03MMC.zigbee" 

| Manufacturer Code | Image Type | File Version | Stack Version | Name | Ext OTA |
| -- | -- | -- | -- | -- | -- |
| 1141 | 020a | 0123 | 3001 | Z03MMC | zigbee |
| 0x1141 - Telink | 0x02 - TLSR825x, 0x0a - Xiaomi LYWSD03MMC | App release 0.1, App build 2.3 | Zigbee v3.0, Release 0.1 |  Z03MMC | OTA |

|Image Type| Device, note |
| -- | -- |
| 0x0201 | MHO-C401 (old version) |
| 0x0202 | CGG1 (old version) |
| 0x0203 | LYWSD03MMC ver https://github.com/devbis/z03mmc |
| 0x0204 | WATERMETER ver https://github.com/slacky1965/watermeter_zed |
| 0x0206 | CGDK2 |
| 0x0207 | CGG1 (new version) |
| 0x0208 | MHO-C401 (new version) |
| 0x0209 | MJWSD05MMC |
| 0x020A | LYWSD03MMC ver https://github.com/pvvx/ZigbeeTLc |
| 0x020B | MHO-C122 |
| 0x0211 | TS0201-TZ3000 |
| 0x0216 | TH03Z |
| 0x021b | ZTH01 |
| 0x021c | ZTH02 |
| 0x021e | TH03 |
| 0x021f | LKTMZL02 |
| 0x0221 | ZTH05 |
| 0x0225 | ZY-ZTH02 |
| 0x0226 | ZY-ZTH01 |
| 0x0227 | ZG227 |

* To restore to BLE, use the Zigbee OTA file with the same number device from: https://github.com/pvvx/ATC_MiThermometer/tree/master/zigbee_ota

* Zigbee OTA for 128 KB firmware takes 677 seconds with an average consumption of about 1 mA.

* [Full list of supported devices](https://pvvx.github.io/).


## Display decimal places for temperature, humidity and battery charge in ZHA.

[ZHA patch for more precision display of data for all Zigbee devices.](https://github.com/pvvx/ZigbeeTLc/issues/6)

## Indication on LCD screen

A solid "BT" icon indicates a connection loss or the thermometer is not registered with the Zigbee network.

The flashing "BT" icon is called by the "identify" command.

[ZHA: Switch temperature between Celsius and Fahrenheit, Show smiley, Comfort parameters](https://github.com/pvvx/ZigbeeTLc/issues/28)

## Setting offsets for temperature and humidity, turn off the display, measurement interval.

[ZHA: Setting offsets for temperature and humidity, turn off the display, measurement interval](https://github.com/pvvx/ZigbeeTLc/issues/30)

## Change the device name

[ZHA: Change the device name](https://github.com/pvvx/ZigbeeTLc/issues/84)

## Current additions

1. Added project assembly using 'make' (Windows/linux) and the ability to import 'Existing Project' into "[Telink IoT Studio](http://wiki.telink-semi.cn/wiki/IDE-and-Tools/Telink_IoT_Studio/ )".

2. Default intervals for temperature and humidity reporting are set to 30-180 (min-max) seconds, reportableChanged to 0.1C and 0.5%.

3. Consumption has been optimized when the coordinator is disconnected or connection is lost. When performing _rejoin_, if the connection is broken, the thermometer consumes quite a lot. Monitor the work of your network coordinator - Zigbee does not like (almost cannot tolerate) coordinator outages.

4. Consumption has been optimized when polling the reset button or contact.

5. Additional adjustments to consumption consumption and correction of the report transmission functions algorithm.

6. Correspondence of transmitted and displayed measurements (correct rounding).

7. Reducing firmware size: removing unnecessary code from SDK, reorganization of battery voltage measurement (ADC), ...

8. Averaging battery level measurements (eliminates clutter on charts)

9. Switch temperature between Celsius and Fahrenheit

10. Adding button actions (ver 0.1.1.1)

11. Added Poll Control Cluster (ver 0.1.1.2)

12. Ver 0.1.1.4: According to the "ZigBee Cluster Library Specification" the "Thermostat Temperature Conversion" values have been corrected. Added On/Off smiley.  (ClusterID: 0x0204 (Thermostat User Interface Configuration), Attr: 0x0000 (TemperatureDisplayMode), ENUM8 = 0 in C, = 1 in F, Attr: 0x0002 (ScheduleProgrammingVisibility), ENUM8 = 0 show smiley On,  = 1 show smiley Off)

13. Ver 0.1.1.5: Description of the HVAC cluster ([for ZHA](https://github.com/pvvx/ZigbeeTLc/issues/28))

13. Ver 0.1.1.6: Setting offsets for temperature and humidity.

14. Ver 0.1.1.7: Add Comfort parameters

15. Ver 0.1.1.8: Easy version. Options disabled: "Groups", "TouchLink", "Find and Bind".

16. Ver 0.1.1.9: Fix configuration saving error

17. Ver 0.1.2.0: At the [request](https://github.com/pvvx/ZigbeeTLc/pull/50#issuecomment-1925906616) of @devbis, for differences in z2m, the device names have been renamed. "-z" will be added to the name Zigbee devices, "-bz" will be added to the [BLE and Zigbee](https://github.com/pvvx/BZdevice) devices, "-zb" to the [Zigbee2BLE](https://github.com/pvvx/Zigbee2BLE) devices.

18. Ver 0.1.2.1: Disabled POLL_CTRL - some programs set a [short polling period](https://github.com/home-assistant/core/blob/dev/homeassistant/components/zha/core/cluster_handlers/general.py#L603) without taking into account the Zigbee 3.0 specification, which increases consumption. For temperature and humidity offsets, as well as COMFORT parameters, the resolution has been changed to 0.01 units. Added parameters: display off, sensor measurement interval. The ability to change the device name is enabled - works with all programs that support ZigBee 3.0 without rewriting configurations. Down with identifying a device by name in Z2M! Users should be able to change the names of Zigbee devices!

19. Ver 0.1.2.2: Offset correction for SHTv3 and SHT4x sensors.

20. Ver 0.1.2.3: Added the ability to change the "manufacturer name". Changing the structure of saving the model and the manufacturer name. Corrected manufacturer name "MiaoMiaoCe" for MHO-C122 and MHO-C401N.

21. Added firmware for [TH03](https://pvvx.github.io/TS0201_TZ3000_TH03).

22. Added firmware for [LKTMZL02](https://pvvx.github.io/LKTMZL02).

23. Added firmware for [MHO-C401(old)](https://pvvx.github.io/MHO_C401).

24. Ver 0.1.2.4. Added firmware for [ZY-ZTH02](https://pvvx.github.io/ZY-ZTH02), [ZG-227Z](https://pvvx.github.io/ZG-227Z). Prevent ZHA from incorrectly setting LongPoll to a short interval contrary to Zigbee 3.0 standards.

25. Ver 0.1.2.5 Alpha. Completely reworked version on SDK v3.7.1.2. Reduced battery consumption for thermometers with SHTC3 sensor. Changes to storage area for user settings. Additions to button functions. Multiple fixes. Adding new devices. ...

## Make

To build under Linux, run:

```
git clone https://github.com/pvvx/ZigbeeTLc
make install
make
```

Out log:
```
===================================================================
 Section|          Description| Start (hex)|   End (hex)|Used space
-------------------------------------------------------------------
 ramcode|   Resident Code SRAM|           0|        1360|    4960
    text|           Code Flash|        1360|       1E028|  117960
  rodata| Read Only Data Flash|           0|           0|       0
 cusdata|          Custom SRAM|      844FD8|      844FD8|       0
      nc|   Wasteful Area SRAM|      841360|      841400|     160
   ictag|     Cache Table SRAM|      841400|      841500|     256
  icdata|      Cache Data SRAM|      841500|      841D00|    2048
    data|       Init Data SRAM|      841D00|      841E94|     404
     bss|        BSS Data SRAM|      841EA0|      844FD8|   12600
 irq_stk|        BSS Data SRAM|      841EA0|      8420A0|     512
    cbss| Custom BSS Data SRAM|      844FD8|      844FD8|       0
   stack|       CPU Stack SRAM|      844FD8|      850000|   45096
   flash|       Bin Size Flash|           0|       1E1BC|  123324
-------------------------------------------------------------------
Start Load SRAM : 0 (ICtag: 0x0)
Total Used SRAM : 20440 from 65536
Total Free SRAM : 160 + stack[45096] = 45256
```

Under Window, after downloading the project, run “make install”:
```
git clone https://github.com/pvvx/ZigbeeTLc
make install
```
and import the project into 'Telink IoT Studio' as described in 'Telink_IoT_Studio_User_Guide.pdf', chapter "6 Import and build projects".

On Windows, it is possible to enable multi-threaded assembly to build the entire project from scratch in 2..3 seconds (make -j -> 3 seconds on 12 cores/24 threads).

---


The start of the project with Zigbee on TLSR825x was inspired by the following original works:

* https://github.com/slacky1965/watermeter_zed

* https://github.com/devbis/z03mmc

---

* [BLE firmware](https://github.com/pvvx/ATC_MiThermometer)

---
