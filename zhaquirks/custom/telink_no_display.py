"""Telink TLSR825x based devices with custom firmware.

see https://github.com/pvvx/ZigbeeTLc
"""

from zigpy.quirks.v2 import QuirkBuilder
from zigpy.quirks.v2.homeassistant import PERCENTAGE, UnitOfTemperature, UnitOfTime
import zigpy.types as t
from zigpy.zcl import ClusterType
from zigpy.zcl.clusters.hvac import ScheduleProgrammingVisibility, UserInterface
from zigpy.zcl.foundation import ZCLAttributeDef

from zhaquirks import CustomCluster


class CustomUserInterface2Cluster(CustomCluster, UserInterface):
    """Custom User Interface Cluster with smiley control."""

    class AttributeDefs(UserInterface.AttributeDefs):
        """Attribute Definitions."""

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
    QuirkBuilder("Sonoff", "ZG-303Z-z")
    .applies_to("Tuya", "CB3S-z")
    .applies_to("Tuya", "TS0201-z")
    .applies_to("Tuya", "TH03Z-z")
    .applies_to("Tuya", "ZTH01-z")
    .applies_to("Tuya", "ZTH02-z")
    .applies_to("Tuya", "ZY-ZTH02-z")
    .applies_to("Tuya", "ZG-227Z-z")
    .applies_to("Tuya", "MC-z")
    .applies_to("ZBeacon", "TH01-z")
    .removes(CustomUserInterface2Cluster.cluster_id, cluster_type=ClusterType.Client)
    .adds(CustomUserInterface2Cluster)
    .number(
        CustomUserInterface2Cluster.AttributeDefs.temperature_offset.name,
        CustomUserInterface2Cluster.cluster_id,
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
        CustomUserInterface2Cluster.AttributeDefs.humidity_offset.name,
        CustomUserInterface2Cluster.cluster_id,
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
        CustomUserInterface2Cluster.AttributeDefs.measurement_interval.name,
        CustomUserInterface2Cluster.cluster_id,
        min_value=3,
        max_value=255,
        unit=UnitOfTime.SECONDS,
        translation_key="measurement_interval",
        fallback_name="Measurement interval",
        mode="box",
    )
    .add_to_registry()
)
