// requires zigbee2mqtt v1.34+
// external converter for ZigbeeTLc v0.1.2.1+ by pvvx
// https://github.com/pvvx/ZigbeeTLc
// based on external converter for devbis-Firmware
// https://raw.githubusercontent.com/devbis/z03mmc/master/converters/lywsd03mmc.js

const fz = require("zigbee-herdsman-converters/converters/fromZigbee");
const tz = require("zigbee-herdsman-converters/converters/toZigbee");

const herdsman = require('zigbee-herdsman');
const { binary, numeric, quirkAddEndpointCluster } = require('zigbee-herdsman-converters/lib/modernExtend');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const constants = require('zigbee-herdsman-converters/lib/constants');
const ota = require('zigbee-herdsman-converters/lib/ota');
const e = exposes.presets;
const ea = exposes.access;

const pvvx = {
    comfortDisplay: binary({
        name: 'comfort_display',
        valueOn: ['show', 0],
        valueOff: ['hide', 1],
        cluster: 'hvacUserInterfaceCfg',
        attribute: {ID: 0x0002, type: herdsman.Zcl.DataType.enum8},
        description: 'Whether to show a comfort indicator on the device screen.',
    }),
    tempCalibration: numeric({
            name: 'temperature_calibration',
            unit: '°C',
            cluster: 'hvacUserInterfaceCfg',
            attribute: {ID: 0x0100, type: herdsman.Zcl.DataType.int16},
            valueMin: -50.0,
            valueMax: 50.0,
            valueStep: 0.01,
            scale: 10,
            description: 'Temperature calibration, in 0.01° steps, default 0%.',
        }),
    humidityCalibration: numeric({
            name: 'humidity_calibration',
            unit: '%',
            cluster: 'hvacUserInterfaceCfg',
            attribute: {ID: 0x0101, type: herdsman.Zcl.DataType.int16},
            valueMin: -50.0,
            valueMax: 50.0,
            valueStep: 0.01,
            scale: 10,
            description: 'Humidity calibration, in 0.01% steps, default 0%.',
        }),
    comfortTemperatureMin: numeric({
            name: 'comfort_temperature_min',
            unit: '°C',
            cluster: 'hvacUserInterfaceCfg',
            attribute: {ID: 0x0102, type: herdsman.Zcl.DataType.int16},
            valueMin: -50.0,
            valueMax: 120.0,
            valueStep: 0.01,
            scale: 10,
            description: 'Comfort parameters/Temperature minimum, in 0.01°C steps, default 20.00°C.',
        }),
    comfortTemperatureMax: numeric({
            name: 'comfort_temperature_max',
            unit: '°C',
            cluster: 'hvacUserInterfaceCfg',
            attribute: {ID: 0x0103, type: herdsman.Zcl.DataType.int16},
            valueMin: -50.0,
            valueMax: 120.0,
            valueStep: 0.01,
            scale: 10,
            description: 'Comfort parameters/Temperature maximum, in 0.01°C steps, default 25.00°C.',
        }),
    comfortHumidityMin: numeric({
            name: 'comfort_humidity_min',
            unit: '%',
            cluster: 'hvacUserInterfaceCfg',
            attribute: {ID: 0x0104, type: herdsman.Zcl.DataType.uint16},
            valueMin: 0,
            valueMax: 9999,
            scale: 10,
            description: 'Comfort parameters/Humidity minimum, in 1% steps, default 40.00%',
        }),
    comfortHumidityMax: numeric({
            name: 'comfort_humidity_max',
            unit: '%',
            cluster: 'hvacUserInterfaceCfg',
            attribute: {ID: 0x0105, type: herdsman.Zcl.DataType.uint16},
            valueMin: 0,
            valueMax: 9999,
            scale: 10,
            description: 'Comfort parameters/Humidity maximum, in 1% steps, default 60.00%.',
    }),
    display: binary({
        name: 'display',
        valueOn: ['on', 0],
        valueOff: ['off', 1],
        cluster: 'hvacUserInterfaceCfg',
        attribute: {ID: 0x0106, type: herdsman.Zcl.DataType.enum8},
        description: 'Whether to enable the device display.',
    }),
    measurementInterval: numeric({
        name: 'measurement_interval',
        unit: 's',
        cluster: 'hvacUserInterfaceCfg',
        attribute: {ID: 0x0107, type: herdsman.Zcl.DataType.uint8},
        valueMin: 3,
        valueMax: 255,
        description: 'Measurement interval, default 10 seconds.',
    }),
};

const definitions = [
    {
        fingerprint: [
            {modelID: 'LYWSD03MMC-z', manufacturerName: 'Xiaomi', model: 'LYWSD03MMC'},
            {modelID: 'LYWSD03MMC-bz', manufacturerName: 'Xiaomi', model: 'LYWSD03MMC'},
            {modelID: 'MHO-C122-z', manufacturerName: 'MiaoMiaoCe', model: 'MHO-C122'},
            {modelID: 'MHO-C122-bz', manufacturerName: 'MiaoMiaoCe', model: 'MHO-C122'},
            {modelID: 'MHO-C401N-z', manufacturerName: 'MiaoMiaoCe', model: 'MHO-C401N'},
            {modelID: 'MHO-C401N-bz', manufacturerName: 'MiaoMiaoCe', model: 'MHO-C401N'}
        ],
        model: 'LYWSD03MMC',
        vendor: 'Xiaomi',
        description: 'Temp & RH Monitor Lite (pvxx/ZigbeeTLc)',
        fromZigbee: [fz.temperature, fz.humidity, fz.battery],
        toZigbee: [tz.thermostat_temperature_display_mode],
        exposes: [
            e.battery(),
            e.temperature(),
            e.humidity(),
            e.enum('temperature_display_mode', ea.ALL, ['celsius', 'fahrenheit'])
                .withDescription('The temperature unit displayed on the screen')
        ],
        extend: [
            quirkAddEndpointCluster({
                endpointID: 1,
                outputClusters: [
                    'genOta',
                ],
                inputClusters: [
                    'genBasic',
                    'genPowerCfg',
                    'genIdentify',
                    'hvacUserInterfaceCfg',
                    'msTemperatureMeasurement',
                    'msRelativeHumidity',
                ],
            }),
            pvvx.tempCalibration,
            pvvx.humidityCalibration,
            pvvx.measurementInterval,
            pvvx.display,
            pvvx.comfortDisplay,
            pvvx.comfortTemperatureMin,
            pvvx.comfortTemperatureMax,
            pvvx.comfortHumidityMin,
            pvvx.comfortHumidityMax
        ],
        ota: ota.zigbeeOTA,
        configure: async (device, coordinatorEndpoint, logger) => {
            const endpoint = device.getEndpoint(1);
            const bindClusters = ['msTemperatureMeasurement', 'msRelativeHumidity', 'genPowerCfg', 'genPollCtrl'];
            await reporting.bind(endpoint, coordinatorEndpoint, bindClusters);
            await reporting.temperature(endpoint, {min: 10, max: constants.repInterval.MINUTES_5, change: 10});
            await reporting.humidity(endpoint, {min: 10, max: constants.repInterval.MINUTES_5, change: 50});
            await reporting.batteryPercentageRemaining(endpoint);
            device.powerSource = 'Battery';
            device.save();
        },
    },
    {
        fingerprint: [
            {modelID: 'CGDK2-z', manufacturerName: 'Qingping', model: 'CGDK2'},
            {modelID: 'CGDK2-bz', manufacturerName: 'Qingping', model: 'CGDK2'}
        ],
        model: 'CGDK2',
        vendor: 'Qingping',
        description: 'Temp & RH Monitor Lite (pvxx/ZigbeeTLc)',
        fromZigbee: [fz.temperature, fz.humidity, fz.battery],
        toZigbee: [tz.thermostat_temperature_display_mode],
        exposes: [
            e.battery(),
            e.temperature(),
            e.humidity(),
            e.enum('temperature_display_mode', ea.ALL, ['celsius', 'fahrenheit'])
                .withDescription('The temperature unit displayed on the screen')
        ],
        extend: [
            quirkAddEndpointCluster({
                endpointID: 1,
                outputClusters: [
                    'genOta',
                ],
                inputClusters: [
                    'genBasic',
                    'genPowerCfg',
                    'genIdentify',
                    'hvacUserInterfaceCfg',
                    'msTemperatureMeasurement',
                    'msRelativeHumidity',
                ],
            }),
            pvvx.tempCalibration,
            pvvx.humidityCalibration,
            pvvx.display,
            pvvx.measurementInterval
        ],
        ota: ota.zigbeeOTA,
        configure: async (device, coordinatorEndpoint, logger) => {
            const endpoint = device.getEndpoint(1);
            const bindClusters = ['msTemperatureMeasurement', 'msRelativeHumidity', 'genPowerCfg', 'genPollCtrl'];
            await reporting.bind(endpoint, coordinatorEndpoint, bindClusters);
            await reporting.temperature(endpoint, {min: 10, max: constants.repInterval.MINUTES_5, change: 10});
            await reporting.humidity(endpoint, {min: 10, max: constants.repInterval.MINUTES_5, change: 50});
            await reporting.batteryPercentageRemaining(endpoint);
            device.powerSource = 'Battery';
            device.save();
        },
    },
    {
        fingerprint: [
            {modelID: 'TS0201-z', manufacturerName: 'Tuya', model: 'TS0201'},
            {modelID: 'TS0201-bz', manufacturerName: 'Tuya', model: 'TS0201'},
            {modelID: 'TH03Z-z', manufacturerName: 'Tuya', model: 'TH03Z'},
            {modelID: 'TH03Z-bz', manufacturerName: 'Tuya', model: 'TH03Z'}
        ],
        model: 'WSD500A',
        vendor: 'Tuya',
        description: 'Temperature & Humidity Sensor (pvxx/ZigbeeTLc)',
        fromZigbee: [fz.temperature, fz.humidity, fz.battery],
        toZigbee: [],
        exposes: [ e.battery(), e.temperature(), e.humidity()],
        extend: [
            quirkAddEndpointCluster({
                endpointID: 1,
                outputClusters: [
                    'genOta',
                ],
                inputClusters: [
                    'genBasic',
                    'genPowerCfg',
                    'genIdentify',
                    'hvacUserInterfaceCfg',
                    'msTemperatureMeasurement',
                    'msRelativeHumidity',
                ],
            }),
            pvvx.tempCalibration,
            pvvx.humidityCalibration,
            pvvx.measurementInterval
        ],
        ota: ota.zigbeeOTA,
        configure: async (device, coordinatorEndpoint, logger) => {
            const endpoint = device.getEndpoint(1);
            const bindClusters = ['msTemperatureMeasurement', 'msRelativeHumidity', 'genPowerCfg', 'genPollCtrl'];
            await reporting.bind(endpoint, coordinatorEndpoint, bindClusters);
            await reporting.temperature(endpoint, {min: 10, max: constants.repInterval.MINUTES_5, change: 10});
            await reporting.humidity(endpoint, {min: 10, max: constants.repInterval.MINUTES_5, change: 50});
            await reporting.batteryPercentageRemaining(endpoint);
            device.powerSource = 'Battery';
            device.save();
        },
    },
];

module.exports = definitions;
