
MMCU = atmega2560
UPLOAD_BITRATE = 115200
COM = /dev/ttyACM0

BUILD_DIR = ./build
SRC_DIR   = ./src

CFLAGS = -g -mmcu=$(MMCU) -Os -Wl,-Map,$(SKETCH_MAP) -o $(SKETCH_ELF)
CC = avr-gcc