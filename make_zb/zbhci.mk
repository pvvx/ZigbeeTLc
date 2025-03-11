
OUT_DIR += /zbhci/uart \
/zbhci/usbHID \
/zbhci/usbCDC \
/zbhci/usbPrint 

OBJS += \
$(OUT_PATH)/zbhci/zbhci.o \
$(OUT_PATH)/zbhci/zbhci_zclHandler.o \
$(OUT_PATH)/zbhci/zbhciCmdProcess.o \
$(OUT_PATH)/zbhci/uart/hci_uart.o \
$(OUT_PATH)/zbhci/usbCDC/zbhci_usb_cdc.o \
$(OUT_PATH)/zbhci/usbHID/zbhci_usb_hid.o \
$(OUT_PATH)/zbhci/usbPrint/hci_usbPrint.o 


# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/zbhci/%.o: $(SDK_PATH)/zbhci/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"
