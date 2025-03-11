# ZBdevices
ZigBee devices with BLE-OTA function on Telink chipset.

![image](https://github.com/user-attachments/assets/9fea94d4-8313-493a-a4e3-ae15f18a1c65)

Beta versions... 
(At the moment, the following devices have been tested with the ZBdevice firmware:<br>
Xiaomi LYWSD03MMC B 1.4, CGG1, CGG1N, LKTMZL02. The other options have not been tested yet...)

Прошивки имеют функции включения BLE для обновления по BLE OTA, а так же некоторые дополнительные BLE сервисы.

### Основное предназначение

Для начальной проверочной пробы работы с хабами Zigbee типа “Яндекс станция …” и подобными. Позволяет в случае неуспеха вернуться на BLE версию и прошить другую прошивку через BLE OTA. А так же для возможности настройки устройства по BLE перед спариванием с хабом.

### Поддерживаемые устройства

Смотри [таблицу](https://github.com/pvvx/pvvx.github.io?tab=readme-ov-file#id-numbers-of-alternative-firmware)

* Файлы прошивок находятся в директории [bin_zb](https://github.com/pvvx/ZigbeeTLc/tree/master/bin_zb)

### Включение BLE

Включение BLE происходит при сбросе устройства кнопкой (для Xiaomi LYWSD03MMC путем замыкания контактов “reset” и “gnd”) или потере Zigbee сети. При этом работа в Zigbee сохраняется.

Включить BLE на уже спаренном устройстве возможно путем подачи команды “отключения устройства”:

Кластер: Basic (Endpoint id: 1, Id: 0x0000, Type: in), Атрибут: device_enabled (id: 0x0012). 

Обратное включение или завершение BLE соединения отключит BLE.

Дополнительные BLE сервисы по настройке в BLE будет описаны и реализованы в TelinkMiFlasher позже. На сегодня в TelinkMiFlasher обеспечивается только BLE OTA. Функционал в Zigbee полность доступен и аналогичен ZigbeeTLc.

### В прошивке реализованы такие функции:

1. Изменение MAC/IEEE (только через BLE),
2. Изменение имени Zigbee устройства (в вариатнах BLE и Zigbee),
3. Изменение называния производителя (в вариатнах BLE и Zigbee),
4. Редакция всех пользовательских настроек Zigbee (в вариатнах BLE и Zigbee).

### Функция кнопки:

1. Короткое нажатие - Передача измерений (report),
2. Удержание 2 секунды - Переключение формата показаний температуры C/F,
3. Удержание 7 секунд - Сброс настроек Zigbee для нового спаривания,
4. Удержание 15 секунд - Сброс всех настроек и Zigbee в заводской вариант.
