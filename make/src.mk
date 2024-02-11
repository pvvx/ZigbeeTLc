
OUT_DIR += $(SRC_DIR) \
$(SRC_DIR)/patch_sdk \
$(SRC_DIR)/custom_zcl

OBJS += \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/flash.o \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/flash_drv.o \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/adc_drv.o \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/random.o \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/i2c_drv.o \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/hw_drv.o \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/cstartup_8258.o \
$(OUT_PATH)$(SRC_DIR)/app_ui.o \
$(OUT_PATH)$(SRC_DIR)/device.o \
$(OUT_PATH)$(SRC_DIR)/lcd_03mmc.o \
$(OUT_PATH)$(SRC_DIR)/lcd_cgdk2.o \
$(OUT_PATH)$(SRC_DIR)/lcd_mho_c122.o \
$(OUT_PATH)$(SRC_DIR)/epd_mho_c401n.o \
$(OUT_PATH)$(SRC_DIR)/battery.o \
$(OUT_PATH)$(SRC_DIR)/sensors_shtxx.o \
$(OUT_PATH)$(SRC_DIR)/sensor_cht8305.o \
$(OUT_PATH)$(SRC_DIR)/sensorEpCfg.o \
$(OUT_PATH)$(SRC_DIR)/zb_appCb.o \
$(OUT_PATH)$(SRC_DIR)/custom_zcl/zcl_relative_humidity.o \
$(OUT_PATH)$(SRC_DIR)/custom_zcl/zcl_thermostat_ui_cfg.o \
$(OUT_PATH)$(SRC_DIR)/zcl_sensorCb.o \
$(OUT_PATH)$(SRC_DIR)/reporting.o \
$(OUT_PATH)$(SRC_DIR)/main.o \
$(OUT_PATH)$(SRC_DIR)/ext_ota.o

#$(OUT_PATH)$(SRC_DIR)/common/firmwareEncryptChk.o \
#$(OUT_PATH)$(SRC_DIR)/common/module_test.o \

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)$(SRC_DIR)/%.o: $(PROJECT_PATH)$(SRC_DIR)/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"
	
$(OUT_PATH)$(SRC_DIR)/%.o: $(PROJECT_PATH)$(SRC_DIR)/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(ASM_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"