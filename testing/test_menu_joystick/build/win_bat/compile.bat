@echo off

set src_dir=../../../../src
set output_dir=./release

set mmcu=atmega2560


IF EXIST %output_dir% (
  rd /s /q "%output_dir%"
)
md "%output_dir%"

@echo on

@echo --------------------------Compite--------------------------

avr-gcc -g -Os -mmcu=%mmcu% -c %src_dir%/Crc.cpp -o %output_dir%/Crc.o
avr-gcc -g -Os -mmcu=%mmcu% -c %src_dir%/Variables.cpp -o %output_dir%/Variables.o
avr-gcc -g -Os -mmcu=%mmcu% -c %src_dir%/hardware/TFT_ST7735.cpp -o %output_dir%/TFT_ST7735.o
avr-gcc -g -Os -mmcu=%mmcu% -c %src_dir%/Graphic.cpp -o %output_dir%/Graphic.o
avr-gcc -g -Os -mmcu=%mmcu% -c %src_dir%/../testing/test_menu_joystick/src/main.cpp -o %output_dir%/main.o


@echo --------------------------Linking--------------------------

avr-gcc -g -mmcu=%mmcu% -Wl,-Map,%output_dir%/main.map -o %output_dir%/main.elf %output_dir%/Crc.o %output_dir%/Variables.o %output_dir%/TFT_ST7735.o %output_dir%/Graphic.o %output_dir%/main.o

avr-objdump -h -S %output_dir%/main.elf > %output_dir%/main.lst

avr-objcopy -j .text -j .data -O ihex %output_dir%/main.elf %output_dir%/main.hex


pause