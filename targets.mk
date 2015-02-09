
help:
	@echo 'Help details:'
	@echo 'hex: compile hex file'
	@echo 'upload-by-usbasp: upload to device by usb-asp'
	@echo 'upload-by-bootloader: upload to Arduino Mega 2560 by bootloader'

hex: $(COMPILE_FILES)
	$(CC) $(CFLAGS) $^
	avr-objdump -h -S $(SKETCH_ELF) > $(SKETCH_LIST)
	avr-objcopy -j .text -j .data -O ihex $(SKETCH_ELF) $(SKETCH_HEX)

upload-by-usbasp: hex
	@echo 'TODO: Write this'

upload-by-bootloader: hex
	avrdude -V -v -v -p$(MMCU) -cwiring -P${COM} -b$(UPLOAD_BITRATE) -D -U flash:w:$(SKETCH_HEX):i 

.PHONY: clean

clean:
	rm -f $(BUILD_DIR)/*
