const {deviceEndpoints, battery, identify,humidity, temperature} = require('zigbee-herdsman-converters/lib/modernExtend');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const ota = require('zigbee-herdsman-converters/lib/ota');


const definitions = [
  {
    zigbeeModel: ['TS0201-2ep'],
    model: 'TS0201-2ep',
    vendor: 'Tuya',
    description: 'Temp & RH Monitor ZigbeeTLc 2sens 2ep)',
    extend: [deviceEndpoints({"endpoints":{"1":1,"2":2}}), 
        battery(), identify(), 
        temperature({endpointNames:["1"],"description": "EC temperature","reporting": {min:10, max:360, change: 10} }),
        //temperature({"endpointNames":["1"],"name": "temperatureEC","description": "EC temperature"}),
        temperature({endpointNames:["2"], "description": " temperature retour","reporting": {min: 10, max: 360, change: 10} })
    ],
    meta: {"multiEndpoint":true},
    //ota: ota.zigbeeOTA
  },
  {
    zigbeeModel: ['TS0201-2TH'],
    model: 'TS0201-2TH',
    vendor: 'Tuya',
    description: 'Temp & RH Monitor Lite (HG/ZigbeeTLc2se  overload humi)',
    extend: [battery(), identify(), temperature(), humidity()],
    meta: {},
    ota: ota.zigbeeOTA,
    configure: async (device, coordinatorEndpoint, logger) => {
        const endpoint = device.getEndpoint(1);
        const bindClusters = ['msTemperatureMeasurement', 'msRelativeHumidity', 'genPowerCfg', 'genPollCtrl'];
        await reporting.bind(endpoint, coordinatorEndpoint, bindClusters);
        await reporting.temperature(endpoint, {min: 10, max: 360, change: 10});
        await reporting.humidity(endpoint, {min: 10, max: 360, change: 50});
        await reporting.batteryPercentageRemaining(endpoint);
        device.powerSource = 'Battery';
        device.save();
    }
  },
];

module.exports = definitions;