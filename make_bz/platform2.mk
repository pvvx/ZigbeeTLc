
OUT_DIR += /platform \
/platform/boot \
/platform/boot/8258 \
/platform/services/b85m \
/platform/tc32 \
/platform/chip_8258 \
/platform/chip_8258/flash

OBJS += \
$(OUT_PATH)/platform/boot/link_cfg.o \
$(OUT_PATH)/platform/services/b85m/irq_handler.o \
$(OUT_PATH)/platform/tc32/div_mod.o 

#$(OUT_PATH)/platform/chip_8258/flash.o \
#$(OUT_PATH)/platform/chip_8258/flash/flash_mid13325e.o

#$(OUT_PATH)/platform/boot/8258/cstartup_8258.o \
#$(OUT_PATH)/platform/chip_8258/adc.o \


# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/platform/%.o: $(SDK_PATH)/platform/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

$(OUT_PATH)/platform/%.o: $(SDK_PATH)/platform/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(ASM_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"