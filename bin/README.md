
v0.1.2.5 - beta!

* 1141-02tt-nnnn3001-xxxxz_vNN.zigbee - ZigbeeTlc firmware
* 1141-d3a3-01993001-xxxxz_vNN.zigbee - firmware for transferring Tuya to ZigbeeTlc
* 1286-0202-10993607-TH03Zz_xxxx.zigbee - firmware for transferring TH03 Tuya/Sonoff to ZigbeeTlc
* 1286-0203-10983001-ZG227Zz_xxxx.zigbee - firmware for transferring ZG227Z Tuya/Sonoff to ZigbeeTlc
* 1002-0203-57013001-ZYZTH02z_v0124.zigbee - firmware for transferring Tuya to ZigbeeTlc

* xxxxx_vNN.bin - firmware files for the programmer or BLE OTA

* devbis - firmware files for the option https://github.com/devbis

* indexNNNN.json - index file for ZigbeeTlc firmware

---

* 1141-02tt-nnnn3001-xxxxz_vNN.zigbee - прошивки ZigbeeTlc
* 1141-d3a3-01993001-xxxxz_vNN.zigbee - прошивки для перехода из Tuya в ZigbeeTlc
* 1286-0202-10993607-TH03Zz_xxxx.zigbee - прошивка для перехода TH03 из Tuya (Sonoff) в ZigbeeTlc
* 1286-0203-10983001-ZG227Zz_xxxx.zigbee - прошивки для перехода ZG227Z Tuya (Sonoff) в ZigbeeTlc
* 1002-0203-57013001-ZYZTH02z_v0124.zigbee - прошивки для перехода из Tuya в ZigbeeTlc
* xxxxx_vNN.bin - файлы прошивки для программатора или BLE OTA

* devbis - файлы прошивок для варианта https://github.com/devbis

* indexNNNN.json - файл индексов для прошивок ZigbeeTlc

---

| HW ID | Zigbee Image Type | Device | Note
|--|--|--|--|
|  0 | =0x020A | LYWSD03MMC_B14    | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
|  1 | 0x0201 | [MHO-C401(old)](https://pvvx.github.io/MHO_C401) | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
|  2 | 0x0202 | [CGG1-M(2020,2021)](https://pvvx.github.io/CGG1) | [BLE](https://github.com/pvvx/ATC_MiThermometer)  & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
|    | 0x0203 | [LYWSD03MMC_DEVBIS](https://github.com/devbis/z03mmc) | Default Telink Zigbee SDK and [old version from devbis](https://github.com/devbis/z03mmc) |
|  3 | =0x020A | LYWSD03MMC_B19	  | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
|  4 | =0x020A  | LYWSD03MMC_B16    | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) | 
|  5 | =0x020A | LYWSD03MMC_B17    | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
|  6 | 0x0206 | [CGDK2](https://pvvx.github.io/CGDK2) | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
|  7 | 0x0207  | [CGG1-M(2022)](https://pvvx.github.io/CGG1_2022) | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
|  8 | 0x0208 | [MHO-C401(2022)](https://pvvx.github.io/MHO_C401N) | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
|  9 | 0x0209 | [MJWSD05MMC(ch)](https://pvvx.github.io/MJWSD05MMC) | [BLE](https://github.com/pvvx/ATC_MiThermometer) |
| 10 | 0x020A | LYWSD03MMC_B15    | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
| 11 | 0x020B | [MHO-C122](https://pvvx.github.io/MHO_C122) | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
| 12 | 0x020C | [MJWSD05MMC(en)](https://pvvx.github.io/MJWSD05MMC) | [BLE](https://github.com/pvvx/ATC_MiThermometer) |
| 17 | 0x0211 | TS0201 [TS0201_TZ3000](https://pvvx.github.io/TS0201_TZ3000)  | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
| 22 | 0x0216 | [TH03Z](https://pvvx.github.io/TH03Z) | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
| 27 | 0x021B | [ZTH01(TS0601_TZE200)](https://pvvx.github.io/TS0601_TZE200_zth01) | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
| 28 | 0x021C | [ZTH02(TS0601_TZE200)](https://pvvx.github.io/TS0601_TZE200_zth02) | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
| 30 | 0x021E | [TH03(TS0201_TZ3000)](https://pvvx.github.io/TS0201_TZ3000_TH03) | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
| 31 | 0x021F | [LKTMZL02(TS0201_TZ3210)](https://pvvx.github.io/LKTMZL02) | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
| 33 | 0x0221 | [ZTH05(TS0601_TZE204)](https://pvvx.github.io/TS0601_TZE204) | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
| 37 | 0x0225 | [ZY-ZTH02](https://pvvx.github.io/ZY-ZTH02) TS0201_TZ3000_v1w2k9dd without LCD | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |
| 39 | 0x0227 | [ZG-227Z](https://pvvx.github.io/ZG-227Z) TS0601_TZE200_a8sdabtg | [BLE](https://github.com/pvvx/ATC_MiThermometer) & [ZigBeeTLc](https://github.com/pvvx/ZigbeeTLc) |	
