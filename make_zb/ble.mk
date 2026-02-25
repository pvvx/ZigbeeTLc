
OUT_DIR += /stack/ble

OBJS += $(OUT_PATH)/stack/ble/ble_rf_pa_ctrl.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/stack/ble/%.o: $(SDK_PATH)/stack/ble/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

