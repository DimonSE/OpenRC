
include configurate.mk

COMPILE_FILES = $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/hardware/*.cpp)

SKETCH_HEX  = $(BUILD_DIR)/main.hex
SKETCH_MAP  = $(BUILD_DIR)/main.map
SKETCH_ELF  = $(BUILD_DIR)/main.elf
SKETCH_LIST = $(BUILD_DIR)/main.lst

include targets.mk