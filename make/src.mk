
OUT_DIR += $(SRC_DIR) \
$(SRC_DIR)/custom_zcl \
$(SRC_DIR)/common

OBJS += \
$(OUT_PATH)$(SRC_DIR)/adc.o \
$(OUT_PATH)$(SRC_DIR)/app_i2c.o \
$(OUT_PATH)$(SRC_DIR)/app_ui.o \
$(OUT_PATH)$(SRC_DIR)/device.o \
$(OUT_PATH)$(SRC_DIR)/lcd_03mmc.o \
$(OUT_PATH)$(SRC_DIR)/lcd_cgdk2.o \
$(OUT_PATH)$(SRC_DIR)/lcd_mho_c122.o \
$(OUT_PATH)$(SRC_DIR)/sensorEpCfg.o \
$(OUT_PATH)$(SRC_DIR)/sensors.o \
$(OUT_PATH)$(SRC_DIR)/zb_appCb.o \
$(OUT_PATH)$(SRC_DIR)/custom_zcl/zcl_relative_humidity.o \
$(OUT_PATH)$(SRC_DIR)/custom_zcl/zcl_thermostat_ui_cfg.o \
$(OUT_PATH)$(SRC_DIR)/zcl_sensorCb.o \
$(OUT_PATH)$(SRC_DIR)/reporting.o \
$(OUT_PATH)$(SRC_DIR)/drv_hw.o \
$(OUT_PATH)$(SRC_DIR)/drv_flash.o \
$(OUT_PATH)$(SRC_DIR)/common/main.o \
$(OUT_PATH)$(SRC_DIR)/common/factory_reset.o \
$(OUT_PATH)$(SRC_DIR)/common/firmwareEncryptChk.o \
$(OUT_PATH)$(SRC_DIR)/common/module_test.o \
$(OUT_PATH)$(SRC_DIR)/cstartup_8258.o \
$(OUT_PATH)$(SRC_DIR)/ext_ota.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)$(SRC_DIR)/%.o: $(PROJECT_PATH)$(SRC_DIR)/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"
	
$(OUT_PATH)$(SRC_DIR)/%.o: $(PROJECT_PATH)$(SRC_DIR)/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(ASM_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"	