
MMCU = atmega2560
UPLOAD_BITRATE = 115200
COM = /dev/ttyACM0

BUILD_DIR = ./build
SRC_DIR   = ./src

CPP = $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/hardware/*.cpp)

SKETCH_HEX  = $(BUILD_DIR)/main.hex
SKETCH_MAP  = $(BUILD_DIR)/main.map
SKETCH_ELF  = $(BUILD_DIR)/main.elf
SKETCH_LIST = $(BUILD_DIR)/main.lst

CFLAGS = -g -mmcu=$(MMCU) -Os -Wl,-Map,$(SKETCH_MAP) -o $(SKETCH_ELF)
CC = avr-gcc

help:
	@echo 'Help details:'
	@echo 'hex: compile hex file'
	@echo 'flash: install hex file'
	@echo 'program: compile hex and install'

hex: $(CPP)
	$(CC) $(CFLAGS) $^
	avr-objdump -h -S $(SKETCH_ELF) > $(SKETCH_LIST)
	avr-objcopy -j .text -j .data -O ihex $(SKETCH_ELF) $(SKETCH_HEX)

upload-by-usbasp: hex

upload-by-bootloader: hex
	avrdude -V -v -v -p$(MMCU) -cwiring -P${COM} -b$(UPLOAD_BITRATE) -D -U flash:w:$(SKETCH_HEX):i 

.PHONY: clean

clean:
	rm -f $(BUILD_DIR)/main.*
