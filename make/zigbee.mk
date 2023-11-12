
OUT_DIR += \
/zigbee/af \
/zigbee/aps \
/zigbee/bdb \
/zigbee/common \
/zigbee/gp \
/zigbee/mac \
/zigbee/ss \
/zigbee/ota \
/zigbee/zdo \
/zigbee/zcl \
/zigbee/wwah \
/zigbee/zcl/closures \
/zigbee/zcl/commissioning \
/zigbee/zcl/general \
/zigbee/zcl/hvac \
/zigbee/zcl/light_color_control \
/zigbee/zcl/measument_sensing \
/zigbee/zcl/ota_upgrading \
/zigbee/zcl/security_safety \
/zigbee/zcl/smart_energy \
/zigbee/zcl/zll_commissioning \
/zigbee/zcl/zcl_wwah


OBJS += \
$(OUT_PATH)/zigbee/bdb/bdb.o \
$(OUT_PATH)/zigbee/aps/aps_group.o \
$(OUT_PATH)/zigbee/mac/mac_phy.o \
$(OUT_PATH)/zigbee/mac/mac_pib.o \
$(OUT_PATH)/zigbee/zdo/zdp.o \
$(OUT_PATH)/zigbee/zcl/zcl.o \
$(OUT_PATH)/zigbee/zcl/zcl_nv.o \
$(OUT_PATH)/zigbee/zcl/zcl_reporting.o \
$(OUT_PATH)/zigbee/zcl/smart_energy/zcl_metering.o \
$(OUT_PATH)/zigbee/zcl/smart_energy/zcl_metering_attr.o \
$(OUT_PATH)/zigbee/zcl/zll_commissioning/zcl_toucklink_security.o \
$(OUT_PATH)/zigbee/zcl/zll_commissioning/zcl_zllTouchLinkDiscovery.o \
$(OUT_PATH)/zigbee/zcl/zll_commissioning/zcl_zllTouchLinkJoinOrStart.o \
$(OUT_PATH)/zigbee/zcl/zll_commissioning/zcl_zll_commissioning.o \
$(OUT_PATH)/zigbee/zcl/commissioning/zcl_commissioning.o \
$(OUT_PATH)/zigbee/zcl/commissioning/zcl_commissioning_attr.o \
$(OUT_PATH)/zigbee/zcl/hvac/zcl_thermostat.o \
$(OUT_PATH)/zigbee/zcl/measument_sensing/zcl_electrical_measurement.o \
$(OUT_PATH)/zigbee/zcl/measument_sensing/zcl_electrical_measurement_attr.o \
$(OUT_PATH)/zigbee/zcl/measument_sensing/zcl_illuminance_measurement.o \
$(OUT_PATH)/zigbee/zcl/measument_sensing/zcl_illuminance_measurement_attr.o \
$(OUT_PATH)/zigbee/zcl/measument_sensing/zcl_occupancy_sensing.o \
$(OUT_PATH)/zigbee/zcl/measument_sensing/zcl_occupancy_sensing_attr.o \
$(OUT_PATH)/zigbee/zcl/measument_sensing/zcl_temperature_measurement.o \
$(OUT_PATH)/zigbee/zcl/measument_sensing/zcl_temperature_measurement_attr.o \
$(OUT_PATH)/zigbee/zcl/light_color_control/zcl_light_colorCtrl.o \
$(OUT_PATH)/zigbee/zcl/light_color_control/zcl_light_colorCtrl_attr.o \
$(OUT_PATH)/zigbee/zcl/closures/zcl_door_lock.o \
$(OUT_PATH)/zigbee/zcl/closures/zcl_door_lock_attr.o \
$(OUT_PATH)/zigbee/zcl/closures/zcl_window_covering.o \
$(OUT_PATH)/zigbee/zcl/closures/zcl_window_covering_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_alarm.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_alarm_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_basic.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_basic_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_binary_input.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_binary_input_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_binary_output.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_binary_output_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_devTemperatureCfg.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_devTemperatureCfg_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_diagnostics.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_diagnostics_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_greenPower.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_greenPower_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_group.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_group_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_identify.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_identify_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_level.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_level_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_multistate_input.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_multistate_input_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_multistate_output.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_multistate_output_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_onoff.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_onoff_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_pollCtrl.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_pollCtrl_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_powerCfg.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_powerCfg_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_scene.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_scene_attr.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_time.o \
$(OUT_PATH)/zigbee/zcl/general/zcl_time_attr.o \
$(OUT_PATH)/zigbee/zcl/security_safety/zcl_ias_ace.o \
$(OUT_PATH)/zigbee/zcl/security_safety/zcl_ias_wd.o \
$(OUT_PATH)/zigbee/zcl/security_safety/zcl_ias_wd_attr.o \
$(OUT_PATH)/zigbee/zcl/security_safety/zcl_ias_zone.o \
$(OUT_PATH)/zigbee/zcl/security_safety/zcl_ias_zone_attr.o \
$(OUT_PATH)/zigbee/zcl/zcl_wwah/zcl_wwah.o \
$(OUT_PATH)/zigbee/zcl/zcl_wwah/zcl_wwah_attr.o \
$(OUT_PATH)/zigbee/zcl/ota_upgrading/zcl_ota.o \
$(OUT_PATH)/zigbee/zcl/ota_upgrading/zcl_ota_attr.o \
$(OUT_PATH)/zigbee/common/zb_config.o \
$(OUT_PATH)/zigbee/af/zb_af.o \
$(OUT_PATH)/zigbee/wwah/wwah.o \
$(OUT_PATH)/zigbee/wwah/wwahEpCfg.o \
$(OUT_PATH)/zigbee/gp/gp.o \
$(OUT_PATH)/zigbee/gp/gpEpCfg.o \
$(OUT_PATH)/zigbee/gp/gp_proxyTab.o \
$(OUT_PATH)/zigbee/ss/ss_nv.o \
$(OUT_PATH)/zigbee/ota/ota.o \
$(OUT_PATH)/zigbee/ota/otaEpCfg.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/zigbee/%.o: $(SDK_PATH)/zigbee/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"
