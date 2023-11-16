# Custom firmware for Zigbee 3.0 IoT devices on the TLSR825x chip

Under construction...

Currently supported devices: LYWSD03MMC, CGDK2, MHO-C122, MHO-C401N, TS0201_TZ3000

In developing: MHO-C401(old), MJWSD05MMC, CGG1-M, TS0202_TZ3000

Уровень готовности прошивок - Beta version 0.1.0.4 Требуются дополнительные тесты.

Firmware readiness level - Beta versions 0.1.0.4. Additional tests are required.


## To flash the Zigbee firmware, use a Google Chrome, Microsoft Edge or Opera Browser.*

1. Go to the [Over-the-air Webupdater Page TelinkMiFlasher.html](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html)
2. If using Android, Windows, Linux: Ensure you enabled "experimental web platform features". Therefore copy the according link (i.e. chrome://flags/#enable-experimental-web-platform-features for Chrome), open a new browser tab, paste the copied URL. Now sten the _Experimental Web Platform features_ flag to _Enabled_. Then restart the browser.
3. In the Telink Flasher Page: Press `Connect:`. The browser should open a popup with visible Bluetooth devices. Choose the according target device (i.e. LYWSD03MMC) to pair.
4. After connection is established a _Do Acivation_ button appears. Press this button to start the decryption key process.
5. Now you can press the Zigbee Firmware button to directly flash the `Zigbee Firmware`:<br>Alternatively you can choose a specific firmware binary (i.e. the original firmware) via the file chooser.
7. Press Start Flashing. Wait for the firmware to finish.
8. The device should now show up in your Zigbee bridge (If joining is enabled, of course). If this does not happen, reinsert the battery and/or short-circuit the RESET and GND pins on the LYWSD03MMC board, and on sensors with a button, press the button and hold it for 3 seconds.

In the future, you can update [Zigbee LYWSD03MMC firmware to the version from devbis](https://github.com/devbis/z03mmc).

* BLE firmware for [TS0201_TZ3000](https://github.com/pvvx/BLE_THSensor)

---

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

4. Оптимизировано потребление при опросе контакта "reset" или кнопки.

5. Ещё оптимизировано потребление и исправлен report. 

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
Warning: Undefined symbol 'cmf'!
Warning: Undefined symbol 'ss_apsmeSwitchKeyReq'!
Warning: Undefined symbol 'ss_apsmeTransportKeyReq'!
Warning: Undefined symbol 'tl_zbNwkBeaconPayloadUpdate'!
===================================================================
 Section|          Description| Start (hex)|   End (hex)|Used space
-------------------------------------------------------------------
 ramcode|   Resident Code SRAM|           0|        1510|    5392
    text|           Code Flash|        1510|       1E844|  119604
  rodata| Read Only Data Flash|           0|           0|       0
 cusdata|          Custom SRAM|      8451DC|      8451DC|       0
      nc|   Wasteful Area SRAM|      841510|      841600|     240
   ictag|     Cache Table SRAM|      841600|      841700|     256
  icdata|      Cache Data SRAM|      841700|      841F00|    2048
    data|       Init Data SRAM|      841F00|      8420A0|     416
     bss|        BSS Data SRAM|      8420A0|      8451DC|   12604
 irq_stk|        BSS Data SRAM|      8420A0|      8422A0|     512
    cbss| Custom BSS Data SRAM|      8451DC|      8451DC|       0
   stack|       CPU Stack SRAM|      8451DC|      850000|   44580
   flash|       Bin Size Flash|           0|       1E9E4|  125412
-------------------------------------------------------------------
Start Load SRAM : 0 (ICtag: 0x0)
Total Used SRAM : 20956 from 65536
Total Free SRAM : 240 + stack[44580] = 44820
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