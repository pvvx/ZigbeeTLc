# ZBdevices
ZigBee devices with BLE-OTA function on Telink chipset.

![image](https://github.com/user-attachments/assets/9fea94d4-8313-493a-a4e3-ae15f18a1c65)

Прошивки имеют функции включения BLE для обновления по BLE OTA, а так же некоторые дополнительные BLE сервисы.

В BLE режиме настройки и OTA производятся в [TelinkMiFlasher](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html).

Функционал в Zigbee полность доступен и аналогичен ZigbeeTLc.

### Основное предназначение

This is a "trial" firmware.
Created to be able to test the operation with Zigbee, but having the ability to use BLE and Zigbee OTA for those whose Zigbee coordinator does not allow Zigbee OTA.
Which is relevant for closed gateways of different brands.
If everything works correctly with your Zigbee coordinator, then it is advisable to make Zigbee or BLE OTA on a clean Zigbee firmware.

Это "пробная" прошивка. Создана для возможности протестировать работу с Zigbee, но имеющая возможность использовать BLE и Zigbee OTA для тех, чей координатор Zigbee не позволяет использовать Zigbee OTA.
Что актуально для закрытых шлюзов разных марок.
Если с вашим координатором Zigbee все работает корректно, то желательно сделать Zigbee или BLE OTA на чистой прошивке Zigbee.

### Поддерживаемые устройства

Смотри [таблицу](https://github.com/pvvx/pvvx.github.io?tab=readme-ov-file#id-numbers-of-alternative-firmware)

* Файлы прошивок находятся в директории [bin_zb](https://github.com/pvvx/ZigbeeTLc/tree/master/bin_zb)

### Включение BLE

Включение BLE происходит автоматически при сбросе устройства кнопкой (для Xiaomi LYWSD03MMC путем замыкания контактов “reset” и “gnd”) или потере связи с Zigbee сетью.

Включить BLE на уже зарегистрированном в Zigbee устройстве возможно путем подачи команды “отключения устройства”:

Кластер: Basic (Endpoint id: 1, Id: 0x0000, Type: in), Атрибут: device_enabled (id: 0x0012). 

Обратное включение device_enabled или завершение BLE соединения отключит BLE.


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
