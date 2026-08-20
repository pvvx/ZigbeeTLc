"""Telink TLSR825x based devices with custom firmware.

see https://github.com/pvvx/ZigbeeTLc
"""

from zigpy.quirks.v2 import QuirkBuilder
from zigpy.quirks.v2.homeassistant import UnitOfTemperature, UnitOfTime
from zha.units import PERCENTAGE, LIGHT_LUX
import zigpy.types as t
from zigpy.zcl import ClusterType
from zigpy.zcl.clusters.hvac import ScheduleProgrammingVisibility, TemperatureDisplayMode, UserInterface, Dehumidification
from zigpy.zcl.clusters.measurement import IlluminanceLevelSensing, LevelStatus
from zigpy.zcl.foundation import ZCLAttributeDef, ZCL_REPORTING_STATUS_ATTR

from zhaquirks.builder import (
	#BinarySensorDeviceClass,
	#EntityType,
	QuirkBuilder,
	ReportingConfig,
)
from zhaquirks import CustomCluster

class Display(t.enum8):
	"""Turn off the display."""
	Off = 0x01
	On = 0x00

class CustomUserInterfaceCluster(CustomCluster, UserInterface):
	"""Custom User Interface Cluster with smiley control."""

	class AttributeDefs(UserInterface.AttributeDefs):
		"""Attribute Definitions."""

		# display. 0 - display is off, 1 - display is on
		display = ZCLAttributeDef(
			id=0x0106,
			type=Display,
			access="rw",
			is_manufacturer_specific=True,
		)

		# comfort temperature min: A value in 0.01ºC to set minimum comfort temperature for happy face
		comfort_temperature_min = ZCLAttributeDef(
			id=0x0102,
			type=t.int16s,
			access="rw",
			is_manufacturer_specific=True,
		)

		# comfort temperature max: A value in 0.01ºC to set maximum comfort temperature for happy face
		comfort_temperature_max = ZCLAttributeDef(
			id=0x0103,
			type=t.int16s,
			access="rw",
			is_manufacturer_specific=True,
		)

		# comfort humidity min: A value in 0.01%RH to set minimum comfort humidity for happy face
		comfort_humidity_min = ZCLAttributeDef(
			id=0x0104,
			type=t.uint16_t,
			access="rw",
			is_manufacturer_specific=True,
		)

		# comfort humidity max: A value in 0.01%RH to set maximum comfort humidity for happy face
		comfort_humidity_max = ZCLAttributeDef(
			id=0x0105,
			type=t.uint16_t,
			access="rw",
			is_manufacturer_specific=True,
		)

		# A value in 0.01ºC offset to fix up incorrect values from sensor
		temperature_offset = ZCLAttributeDef(
			id=0x0100,
			type=t.int16s,
			access="rw",
			is_manufacturer_specific=True,
		)

		# A value in 0.01%RH offset to fix up incorrect values from sensor
		humidity_offset = ZCLAttributeDef(
			id=0x0101,
			type=t.int16s,
			access="rw",
			is_manufacturer_specific=True,
		)
		# Measurement interval, step 1 second, range: 3..255 seconds. Default 10 seconds.
		measurement_interval = ZCLAttributeDef(
			id=0x0107,
			type=t.uint8_t,
			access="rw",
			is_manufacturer_specific=True,
		)

(
	QuirkBuilder("MiaoMiaoCe", "MHO-C401N-z")
	.applies_to("MiaoMiaoCe", "MHO-C401N-bz")
	.applies_to("MiaoMiaoCe", "MHO-C401-z")
	.applies_to("MiaoMiaoCe", "MHO-C122-z")
	.applies_to("MiaoMiaoCe", "MHO-C122-bz")
	.applies_to("Xiaomi", "LYWSD03MMC-z")
	.applies_to("Xiaomi", "LYWSD03MMC-bz")
	.applies_to("Xiaomi", "MJWSD06MMC-z")
	.applies_to("Sonoff", "TH03-z")
	.applies_to("Qingping", "CGDK2-z")
	.applies_to("Qingping", "CGDK2-bz")
	.applies_to("Qingping", "CGG1-z")
	.applies_to("Qingping", "CGG1N-z")
	.applies_to("Tuya", "TH05-z")
	.removes(CustomUserInterfaceCluster.cluster_id, cluster_type=ClusterType.Client)
	.adds(CustomUserInterfaceCluster)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.temperature_offset.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=-327.67,
		max_value=327.67,
		step=0.01,
		unit=UnitOfTemperature.CELSIUS,
		translation_key="temperature_offset",
		fallback_name="Temperature offset",
		multiplier=0.01,
		mode="box",
	)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.humidity_offset.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=-327.67,
		max_value=327.67,
		step=0.01,
		unit=PERCENTAGE,
		translation_key="humidity_offset",
		fallback_name="Humidity offset",
		multiplier=0.01,
		mode="box",
	)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.comfort_temperature_min.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=-327.67,
		max_value=327.67,
		step=0.01,
		unit=UnitOfTemperature.CELSIUS,
		translation_key="comfort_temperature_min",
		fallback_name="Comfort temperature min",
		multiplier=0.01,
		mode="box",
	)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.comfort_temperature_max.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=-327.67,
		max_value=327.67,
		step=0.01,
		unit=UnitOfTemperature.CELSIUS,
		translation_key="comfort_temperature_max",
		fallback_name="Comfort temperature max",
		multiplier=0.01,
		mode="box",
	)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.comfort_humidity_min.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=0,
		max_value=99,
		step=1,
		unit=PERCENTAGE,
		translation_key="comfort_humidity_min",
		fallback_name="Comfort humidity min",
		multiplier=0.01,
		mode="box",
	)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.comfort_humidity_max.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=0,
		max_value=99,
		step=1,
		unit=PERCENTAGE,
		translation_key="comfort_humidity_max",
		fallback_name="Comfort humidity max",
		multiplier=0.01,
		mode="box",
	)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.measurement_interval.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=3,
		max_value=255,
		unit=UnitOfTime.SECONDS,
		translation_key="measurement_interval",
		fallback_name="Measurement interval",
		mode="box",
	)
	.switch(
		CustomUserInterfaceCluster.AttributeDefs.display.name,
		CustomUserInterfaceCluster.cluster_id,
		off_value=1,
		on_value=0,
		translation_key="display_enabled",
		fallback_name="Display enabled",
	)
	.switch(
		CustomUserInterfaceCluster.AttributeDefs.schedule_programming_visibility.name,
		CustomUserInterfaceCluster.cluster_id,
		translation_key="show_smiley",
		fallback_name="Show smiley",
		off_value=ScheduleProgrammingVisibility.Disabled,
		on_value=ScheduleProgrammingVisibility.Enabled,
	)
	.switch(
		CustomUserInterfaceCluster.AttributeDefs.temperature_display_mode.name,
		CustomUserInterfaceCluster.cluster_id,
		translation_key="temperature_mode",
		fallback_name="Temperature C/F",
		off_value=TemperatureDisplayMode.Metric,
		on_value=TemperatureDisplayMode.Imperial,
	)
	.add_to_registry()
)
(
	QuirkBuilder("Sonoff", "ZG-303Z-z")
	.applies_to("Tuya", "CB3S-z")
	.applies_to("Tuya", "TS0201-z")
	.applies_to("Tuya", "TS0201-bz")
	.applies_to("Tuya", "TH03Z-z")
	.applies_to("Tuya", "TH03Z-bz")
	.applies_to("Tuya", "ZTH01-z")
	.applies_to("Tuya", "ZTH02-z")
	.applies_to("Tuya", "ZY-ZTH02-z")
	.applies_to("Tuya", "ZG-227Z-z")
	.applies_to("Tuya", "MC-z")
	.applies_to("ZBeacon", "TH01-z")
	.applies_to("ZBeacon", "TH01-2-z")
	.applies_to("Wing","TS0201-z")
	.removes(CustomUserInterfaceCluster.cluster_id, cluster_type=ClusterType.Client)
	.adds(CustomUserInterfaceCluster)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.temperature_offset.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=-327.67,
		max_value=327.67,
		step=0.01,
		unit=UnitOfTemperature.CELSIUS,
		translation_key="temperature_offset",
		fallback_name="Temperature offset",
		multiplier=0.01,
		mode="box",
	)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.humidity_offset.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=-327.67,
		max_value=327.67,
		step=0.01,
		unit=PERCENTAGE,
		translation_key="humidity_offset",
		fallback_name="Humidity offset",
		multiplier=0.01,
		mode="box",
	)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.measurement_interval.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=3,
		max_value=255,
		unit=UnitOfTime.SECONDS,
		translation_key="measurement_interval",
		fallback_name="Measurement interval",
		mode="box",
	)
	.add_to_registry()
)
(
	QuirkBuilder("Tuya", "LKTMZL02-z")
	.applies_to("Tuya", "ZY-ZTH01-z")
	.applies_to("Tuya", "TY0201-z")
	.removes(CustomUserInterfaceCluster.cluster_id, cluster_type=ClusterType.Client)
	.adds(CustomUserInterfaceCluster)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.temperature_offset.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=-327.67,
		max_value=327.67,
		step=0.01,
		unit=UnitOfTemperature.CELSIUS,
		translation_key="temperature_offset",
		fallback_name="Temperature offset",
		multiplier=0.01,
		mode="box",
	)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.humidity_offset.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=-327.67,
		max_value=327.67,
		step=0.01,
		unit=PERCENTAGE,
		translation_key="humidity_offset",
		fallback_name="Humidity offset",
		multiplier=0.01,
		mode="box",
	)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.measurement_interval.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=3,
		max_value=255,
		unit=UnitOfTime.SECONDS,
		translation_key="measurement_interval",
		fallback_name="Measurement interval",
		mode="box",
	)
	.switch(
		CustomUserInterfaceCluster.AttributeDefs.display.name,
		CustomUserInterfaceCluster.cluster_id,
		off_value=1,
		on_value=0,
		translation_key="display_enabled",
		fallback_name="Display enabled",
	)
	.switch(
		CustomUserInterfaceCluster.AttributeDefs.temperature_display_mode.name,
		CustomUserInterfaceCluster.cluster_id,
		translation_key="temperature_mode",
		fallback_name="Temperature C/F",
		off_value=TemperatureDisplayMode.Metric,
		on_value=TemperatureDisplayMode.Imperial,
	)
	.add_to_registry()
)


def lxLevelStatus_converter(value: int) -> str:
	actions = {
		0: "Average", # "On"
		1: "Low",   # "Below"
		2: "High", # "Above"
		255: "Unknown",
	}
	return actions.get(value)

class CustomIlluminanceLevelSensing(CustomCluster, IlluminanceLevelSensing):

	#def _update_attribute(self, attrid, value):
	#    if attrid == self.AttributeDefs.illuminance_target_level.id:
	#        #value = pow(10, ((value - 1) / 10000))
	#        #value = self.lux_calibration(value)
	#        value = 10000 * math.log10(value) + 1
	#        value = round(value)
	#    super()._update_attribute(attrid, value)

	class AttributeDefs(IlluminanceLevelSensing.AttributeDefs):
		#level_status: Final = ZCLAttributeDef(
		#    id=0x0000,
		#    type=LevelStatus,
		#    access="rp",
		#    mandatory=True
		#)
		meas_interval = ZCLAttributeDef(
		    id=0x5000,
		    type=t.uint8_t,
		    access="rw",
		    is_manufacturer_specific=True,
		)
		lx_zero = ZCLAttributeDef( # : Final
			id=0x5001,
			type=t.uint16_t,
			access="rw",
			is_manufacturer_specific=True,
		)
		lx_coef = ZCLAttributeDef(
			id=0x5002,
			type=t.uint16_t,
			access="rw",
			is_manufacturer_specific=True,
		)
		lx_ligth_level = ZCLAttributeDef(
			id=0x5003,
			type=t.uint24_t,
			access="rw",
			is_manufacturer_specific=True,
		)
		#reporting_status = ZCL_REPORTING_STATUS_ATTR

(
	QuirkBuilder("Sonoff", "ZG-204ZL-z")
	.replaces(CustomIlluminanceLevelSensing, endpoint_id=1)
	.sensor(
		IlluminanceLevelSensing.AttributeDefs.level_status.name,
		IlluminanceLevelSensing.cluster_id,
		attribute_converter=lxLevelStatus_converter,
		translation_key="level_status",
		fallback_name="Light level",
		reporting_config=ReportingConfig(
			min_interval=0,
			max_interval=3600,
			reportable_change=1,
		),
	)
	.number(
		CustomIlluminanceLevelSensing.AttributeDefs.lx_ligth_level.name,
		CustomIlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=3576000,
		step=1,
		translation_key="lx_ligth_level",
		unit=LIGHT_LUX,
		fallback_name="Low Light Level",
		mode="box",
	)
	.number(
		CustomIlluminanceLevelSensing.AttributeDefs.lx_zero.name,
		CustomIlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=65535,
		step=1,
		translation_key="lx_sensor_offset",
		fallback_name="Illuminance offset",
		unit=LIGHT_LUX,
		mode="box",
	)
	.number(
		CustomIlluminanceLevelSensing.AttributeDefs.lx_coef.name,
		CustomIlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=65535,
		step=1,
		translation_key="lx_sensor_offset",
		fallback_name="Illuminance maximum",
		unit=LIGHT_LUX,
		mode="box",
	)
	.number(
		IlluminanceLevelSensing.AttributeDefs.illuminance_target_level.name,
		IlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=65535,
		step=1,
		translation_key="illuminance_target_level",
		#unit=LIGHT_LUX,
		fallback_name="zlx target",
		mode="box",
	)
	.number(
		CustomIlluminanceLevelSensing.AttributeDefs.meas_interval.name,
		CustomIlluminanceLevelSensing.cluster_id,
		min_value=3,
		max_value=255,
		unit=UnitOfTime.SECONDS,
		translation_key="measurement_interval",
		fallback_name="Measurement interval",
		mode="box",
	)
	.add_to_registry()
)
(
	QuirkBuilder("Sonoff", "ZG-204ZV-z")
	.removes(CustomUserInterfaceCluster.cluster_id, cluster_type=ClusterType.Client)
	.adds(CustomUserInterfaceCluster)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.temperature_offset.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=-327.67,
		max_value=327.67,
		step=0.01,
		unit=UnitOfTemperature.CELSIUS,
		translation_key="temperature_offset",
		fallback_name="Temperature offset",
		multiplier=0.01,
		mode="box",
	)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.humidity_offset.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=-327.67,
		max_value=327.67,
		step=0.01,
		unit=PERCENTAGE,
		translation_key="humidity_offset",
		fallback_name="Humidity offset",
		multiplier=0.01,
		mode="box",
	)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.measurement_interval.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=3,
		max_value=255,
		unit=UnitOfTime.SECONDS,
		translation_key="measurement_interval",
		fallback_name="Measurement interval",
		mode="box",
	)
	.replaces(CustomIlluminanceLevelSensing, endpoint_id=1)
	.sensor(
		IlluminanceLevelSensing.AttributeDefs.level_status.name,
		IlluminanceLevelSensing.cluster_id,
		attribute_converter=lxLevelStatus_converter,
		#attribute_converter=lambda x: x == lxLevelStatus.Below,
		translation_key="level_status",
		fallback_name="Light level",
		reporting_config=ReportingConfig(
			min_interval=0,
			max_interval=3600,
			reportable_change=1,
		),
	)
	.number(
		CustomIlluminanceLevelSensing.AttributeDefs.lx_ligth_level.name,
		CustomIlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=3576000,
		step=1,
		translation_key="lx_ligth_level",
		unit=LIGHT_LUX,
		fallback_name="Low Light Level",
		mode="box",
	)
	.number(
		CustomIlluminanceLevelSensing.AttributeDefs.lx_zero.name,
		CustomIlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=65535,
		step=1,
		translation_key="lx_sensor_offset",
		fallback_name="Illuminance offset",
		unit=LIGHT_LUX,
		mode="box",
	)
	.number(
		CustomIlluminanceLevelSensing.AttributeDefs.lx_coef.name,
		CustomIlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=65535,
		step=1,
		translation_key="lx_sensor_offset",
		fallback_name="Illuminance maximum",
		unit=LIGHT_LUX,
		mode="box",
	)
	.number(
		IlluminanceLevelSensing.AttributeDefs.illuminance_target_level.name,
		IlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=65535,
		step=1,
		translation_key="illuminance_target_level",
		#unit=LIGHT_LUX,
		fallback_name="zlx target",
		mode="box",
	)
	.add_to_registry()
)
(
	QuirkBuilder("HOBEIAN", "ZG-223Z-z")
	.removes(CustomUserInterfaceCluster.cluster_id, cluster_type=ClusterType.Client)
	.adds(CustomUserInterfaceCluster)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.temperature_offset.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=-327.67,
		max_value=327.67,
		step=0.01,
		unit=UnitOfTemperature.CELSIUS,
		translation_key="temperature_offset",
		fallback_name="Temperature offset",
		multiplier=0.01,
		mode="box",
	)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.humidity_offset.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=-327.67,
		max_value=327.67,
		step=0.01,
		unit=PERCENTAGE,
		translation_key="humidity_offset",
		fallback_name="Humidity offset",
		multiplier=0.01,
		mode="box",
	)
	.number(
		CustomUserInterfaceCluster.AttributeDefs.measurement_interval.name,
		CustomUserInterfaceCluster.cluster_id,
		min_value=3,
		max_value=255,
		unit=UnitOfTime.SECONDS,
		translation_key="measurement_interval",
		fallback_name="Measurement interval",
		mode="box",
	)
	.replaces(CustomIlluminanceLevelSensing, endpoint_id=1)
	.sensor(
		IlluminanceLevelSensing.AttributeDefs.level_status.name,
		IlluminanceLevelSensing.cluster_id,
		attribute_converter=lxLevelStatus_converter,
		#attribute_converter=lambda x: x == lxLevelStatus.Below,
		translation_key="level_status",
		fallback_name="Light level",
		reporting_config=ReportingConfig(
			min_interval=0,
			max_interval=3600,
			reportable_change=1,
		),
	)
	.number(
		CustomIlluminanceLevelSensing.AttributeDefs.lx_ligth_level.name,
		CustomIlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=3576000,
		step=1,
		translation_key="lx_ligth_level",
		fallback_name="Low Light Level",
		unit=LIGHT_LUX,
		mode="box",
	)
	.number(
		CustomIlluminanceLevelSensing.AttributeDefs.lx_zero.name,
		CustomIlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=65535,
		step=1,
		translation_key="lx_sensor_offset",
		fallback_name="Illuminance offset",
		unit=LIGHT_LUX,
		mode="box",
	)
	.number(
		CustomIlluminanceLevelSensing.AttributeDefs.lx_coef.name,
		CustomIlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=65535,
		step=1,
		translation_key="lx_sensor_offset",
		fallback_name="Illuminance maximum",
		unit=LIGHT_LUX,
		mode="box",
	)
	.number(
		IlluminanceLevelSensing.AttributeDefs.illuminance_target_level.name,
		IlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=65535,
		step=1,
		translation_key="illuminance_target_level",
		fallback_name="zlx target",
		mode="box",
	)
	.number(
		Dehumidification.AttributeDefs.rh_dehumidification_setpoint.name,
		Dehumidification.cluster_id,
		min_value=0,
		max_value=100,
		step=1,
		unit=PERCENTAGE,
		translation_key="rh_dehumidification_setpoint",
		fallback_name="Rain detection level",
		mode="box",
	)
	.number(
		Dehumidification.AttributeDefs.dehumidification_hysteresis.name,
		Dehumidification.cluster_id,
		min_value=0,
		max_value=100,
		step=1,
		unit=PERCENTAGE,
		translation_key="dehumidification_hysteresis",
		fallback_name="Rain detection level hysteresis",
		mode="box",
	)
	.switch(
		Dehumidification.AttributeDefs.dehumidification_lockout.name,
		Dehumidification.cluster_id,
		off_value=0,
		on_value=1,
		translation_key="dehumidification_lockout",
		fallback_name="Remote control for rain",
	)
	#.switch(
	#	Dehumidification.AttributeDefs.relative_humidity_display.name,
	#	Dehumidification.cluster_id,
	#	off_value=0,
	#	on_value=1,
	#	translation_key="relative_humidity_display",
	#	fallback_name="Remote control of lighting",
	#)
	.sensor(
		Dehumidification.AttributeDefs.dehumidification_cooling.name,
		Dehumidification.cluster_id,
		translation_key="dehumidification_cooling",
		fallback_name="Rain detected",
		unit=PERCENTAGE,
		reporting_config=ReportingConfig(
			min_interval=0,
			max_interval=3600,
			reportable_change=1,
		),
	)
	.add_to_registry()
)
