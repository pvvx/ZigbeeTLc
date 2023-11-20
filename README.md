# Custom firmware for Zigbee 3.0 IoT devices on the TLSR825x chip

Currently supported devices: [LYWSD03MMC](https://pvvx.github.io/ATC_MiThermometer/), [CGDK2](https://pvvx.github.io/CGDK2/), [MHO-C122](https://pvvx.github.io/MHO_C122), [MHO-C401N](https://pvvx.github.io/MHO_C401N), [TS0201_TZ3000](https://pvvx.github.io/TS0201_TZ3000/)

In developing: [MHO-C401(old)](https://pvvx.github.io/MHO_C401), [MJWSD05MMC](https://pvvx.github.io/MJWSD05MMC), [CGG1-M](https://pvvx.github.io/CGG1), [TS0202_TZ3000](https://pvvx.github.io/TS0202_TZ3000)

Уровень готовности прошивок - Beta version 0.1.x.x. 
Стабильна от версии 0.1.1.0, но требуются дополнительные тесты.

Firmware readiness level - Beta versions 0.1.x.x.
Stable from version 0.1.1.0, but additional tests are required


## To flash the Zigbee firmware, use a Google Chrome, Microsoft Edge or Opera Browser.*

1. Go to the [Over-the-air Webupdater Page TelinkMiFlasher.html](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html)
2. If using Android, Windows, Linux: Ensure you enabled "experimental web platform features". Therefore copy the according link (i.e. chrome://flags/#enable-experimental-web-platform-features for Chrome), open a new browser tab, paste the copied URL. Now sten the _Experimental Web Platform features_ flag to _Enabled_. Then restart the browser.
3. In the Telink Flasher Page: Press `Connect:`. The browser should open a popup with visible Bluetooth devices. Choose the according target device (i.e. LYWSD03MMC) to pair.
4. After connection is established a _Do Acivation_ button appears. Press this button to start the decryption key process.
5. Now you can press the Zigbee Firmware button to directly flash the `Zigbee Firmware`:<br>Alternatively you can choose a specific firmware binary (i.e. the original firmware) via the file chooser.
7. Press Start Flashing. Wait for the firmware to finish.
8. The device should now show up in your Zigbee bridge (If joining is enabled, of course). If this does not happen, reinsert the battery and/or short-circuit the RESET and GND pins on the LYWSD03MMC board, and on sensors with a button, press the button and hold it for 3 seconds.

In the future, you can update [Zigbee LYWSD03MMC firmware to the version from devbis](https://github.com/devbis/z03mmc).

## Zigbee OTA

[OTA update in ZHA](https://github.com/pvvx/ZigbeeTLc/issues/7)

## Display decimal places for temperature, humidity and battery charge in ZHA.

[ZHA patch for more precision display of data for all Zigbee devices.](https://github.com/pvvx/ZigbeeTLc/issues/6)

## Indication on LCD screen

A solid "BT" icon indicates a connection loss or the thermometer is not registered with the Zigbee network.

The flashing "BT" icon is called by the identification command.


**Индикация на LCD экране:**

Длительно отображаемый значок “BT” обозначает потерю связи или отсутствие регистрации термометра в сети Zigbee.

Мигающий значок “BT” вызывается по команде identify.


## Current additions

1. Добавлена сборка проекта с помощью ‘make’ (Windows/linux) и возможность импорта 'Existing Project' в "[Telink IoT Studio](http://wiki.telink-semi.cn/wiki/IDE-and-Tools/Telink_IoT_Studio/)".

2. Заданы интервалы по умолчанию для отчетов по температуре и влажности в 20-120 (мин-мах) секунд, reportableChange в 0.1C и 1%.

3. Оптимизировано потребление при отключении координатора или потере связи. Выполняя _rejoin_, при нарушении связи термометр потребляет достатчно много. Следите за работой вашего координатора сети - Zigbee не любит (практически не переносит) отключений координатора.

4. Consumption has been optimized when polling the reset button or contact.

5. Additional adjustments to consumption consumption and correction of the report transmission functions algorithm.

6. Correspondence of transmitted and displayed measurements (correct rounding).

7. Reducing firmware size: removing unnecessary code from SDK, reorganization of battery voltage measurement (ADC), ...

8. Averaging battery level measurements (eliminates clutter on charts)

9. Switch temperature between Celsius and Fahrenheit (ClusterID: 0x0204, Attr: 0x0000, ENUM8 = 1 in C, = 2 in F)

Итоговое среднее потребление LYWSD03MC B1.4 при измерении от источника 3.3В от 14 до 26 мкА в зависимости от динамики изменений температуры и влажности. Это в 1.5 раза меньше [варинта fw](https://github.com/devbis/z03mmc) от @devbis.
(Для сравнения с BLE версией: Вариант с BLE потребляет 14 мкА при default настройках и постоянной передаче всех измеренных значений каждые 10 секунд. Дальность связи в режиме BLE LongRange составляет до 1 км по прямой, для Zigbee - сотни метров.)

Zigbee OTA для прошивки в 128 килобайт выполняется 677 секунд со средним потреблением около 1 мА. Это затраты энергии в 5.4 мА·ч от батареи.

---

Для сборки под Linux выполнить:

```
git clone https://github.com/pvvx/z03mmc
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

Под Window, после импорта в ‘Telink IoT Studio’, выполнить “make install” или в папке проекта:
```
git clone https://github.com/devbis/tl_zigbee_sdk.git -b 3.6.8.5 --depth 1 ./SDK
```
И включить многопоточную сборку для сборки всего проекта с нуля за 2..3 сек (make -j -> 3 сек на 12-ти ядрах/24 потока).

---


The start of the project with Zigbee on TLSR825x was inspired by the following original works:

* https://github.com/slacky1965/watermeter_zed

* https://github.com/devbis/z03mmc

---

* BLE firmware for [TS0201_TZ3000](https://github.com/pvvx/BLE_THSensor)

---