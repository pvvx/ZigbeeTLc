# Custom firmware for Zigbee 3.0 IoT devices on the TLSR825x chip

Under construction...

Currently supported devices: LYWSD03MMC, CGDK2, MHO-C122

In developing: MHO-C401, MJWSD05MMC, CGG1, TS0201_TZ3000, TS0202_TZ3000


To flash the Zigbee firmware, use a Google Chrome, Microsoft Edge or Opera Browser.
	1. Go to the [Over-the-air Webupdater Page TelinkMiFlasher.html](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html)
	2. If using Android, Windows, Linux: Ensure you enabled "experimental web platform features". Therefore copy the according link (i.e. chrome://flags/#enable-experimental-web-platform-features for Chrome), open a new browser tab, paste the copied URL. Now sten the _Experimental Web Platform features_ flag to _Enabled_. Then restart the browser.
	3. In the Telink Flasher Page: Press `Connect:`. The browser should open a popup with visible Bluetooth devices. Choose the according target device (i.e. LYWSD03MMC) to pair.
	4. After connection is established a _Do Acivation_ button appears. Press this button to start the decryption key process.
	5. Now you can press the Zigbee Firmware button to directly flash the `Zigbee Firmware`:<br>Alternatively you can choose a specific firmware binary (i.e. the original firmware) via the file chooser.
	7. Press Start Flashing. Wait for the firmware to finish.
	8. The device should now show up in your Zigbee bridge (If joining is enabled, of course). If this does not happen, reinsert the battery and/or short-circuit the RESET and GND pins on the LYWSD03MMC board, and on sensors with a button, press the button and hold it for 3 seconds.

A brief description of the installed Zigbee firmware version is in this [repository](https://github.com/pvvx/z03mmc).
In the future, you can update [Zigbee LYWSD03MMC firmware to the version from devbis] (https://github.com/devbis/z03mmc).

---

Attention: Without registration in the Zigbee network, the thermometer does not work. Registration of the thermometer in the Zigbee network is carried out by closing the “reset” contacts to ground on the LYWSD03MMC board, and on sensors with a button - by pressing the button and holding it for 3 seconds or when turning on the power. To save energy, the registration time is limited by turning off the thermometer after 45 seconds.

Внимание: Без регистрации в сети Zigbee термометр не работает. Регистрация термометра в сети Zigbee осуществляется замыканием контактов «сброса» на массу на плате LYWSD03MMC, а на датчиках с кнопкой — нажатием кнопки и удерживанием ее в течение 3 секунд или при включении питания. В целях экономии электроэнергии время регистрации ограничено отключением термометра через 45 секунд.

Уровень готовности прошивок - Beta version. Требуются дополнительные тесты.

**Indication on LCD screen:**

A solid "BT" icon indicates a connection loss or the thermometer is not registered with the Zigbee network.

The flashing "BT" icon is called by the identification command.

The “---” indication indicates that the thermometer’s registration time on the network has expired. To resume registration of the thermometer on the network, you must re-enable registration.

**Индикация на LCD экране:**

Длительно отображаемый значок “BT” обозначает потерю связи или отсутствие регистрации термометра в сети Zigbee.

Мигающий значок “BT” вызывается по команде identify.

Индикация “---“ говорит о том, что время регистрации термометра в сети вышло. Для возобновления регистрации термометра в сети требуется повторить регистрацию.


**Текущие дополнения:**

1. Добавлена сборка проекта с помощью ‘make’ (Windows/linux) и возможность импорта 'Existing Project' в "Telink IoT Studio".

2. Заданы интервалы по умолчанию для отчетов по температуре и влажности в 20-120 (мин-мах) секунд, reportableChange в 0.1C и 1%. 

3. Оптимизировано потребление при отключении координатора или потере связи. Выполняя _rejoin_, при нарушении связи термометр потребляет достатчно много. Следите за работой вашего координатора сети - Zigbee не любит (практически не переносит) отключений координатора.

4. Оптимизировано потребление при опросе контакта "reset".

Итоговое среднее потребление LYWSD03MC B1.4 при измерении от источника 3.3V от 14 до 26 мкА в зависимости от динамики изменений температуры и влажности. (Для сравнения: Вариант с BLE потребляет 14 мкА при default настройках и постоянной передаче всех измеренных значений каждые 10 секунд. Дальность связи в режиме BLE LongRange составляет до 1 км по прямой, для Zigbee - сотни метров.)

---

Для сборки под Linux выполнить:

```
git clone https://github.com/pvvx/z03mmc
make install
make
```

Out log:
```
Warning: Undefined symbol 'ss_apsmeSwitchKeyReq'!
Warning: Undefined symbol 'ss_apsmeTransportKeyReq'!
Warning: Undefined symbol 'tl_zbNwkBeaconPayloadUpdate'!
===================================================================
 Section|          Description| Start (hex)|   End (hex)|Used space
-------------------------------------------------------------------
 ramcode|   Resident Code SRAM|           0|        1510|    5392
    text|           Code Flash|        1510|       1F304|  122356
  rodata| Read Only Data Flash|           0|           0|       0
 cusdata|          Custom SRAM|      8452EC|      8452EC|       0
      nc|   Wasteful Area SRAM|      841510|      841600|     240
   ictag|     Cache Table SRAM|      841600|      841700|     256
  icdata|      Cache Data SRAM|      841700|      841F00|    2048
    data|       Init Data SRAM|      841F00|      8420C8|     456
     bss|        BSS Data SRAM|      8420D0|      8452EC|   12828
 irq_stk|        BSS Data SRAM|      8420D0|      8422D0|     512
    cbss| Custom BSS Data SRAM|      8452EC|      8452EC|       0
   stack|       CPU Stack SRAM|      8452EC|      850000|   44308
   flash|       Bin Size Flash|           0|       1F4CC|  128204
-------------------------------------------------------------------
Start Load SRAM : 0 (ICtag: 0x0)
Total Used SRAM : 21228 from 65536
Total Free SRAM : 240 + stack[44308] = 44548
```

Под Window, после импорта в ‘Telink IoT Studio’, выполнить “make install” или в папке проекта:
```
git clone https://github.com/devbis/tl_zigbee_sdk.git -b 3.6.8.5 --depth 1 ./SDK
```
И включить многопоточную сборку для сборки всего проекта с нуля за 2..3 сек (make -j -> 3 сек на 12-ти ядрах/24 потока).

---
