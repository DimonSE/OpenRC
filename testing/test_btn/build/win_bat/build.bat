
set avrdude_conf=C:/Arduino/hardware/tools/avr/etc/avrdude.conf
set com_port=.\COM3

del *.elf *.hex *.lsp *.o *.map


call compile.bat

avrdude -C%avrdude_conf% -V -v -v -p%mmcu% -cwiring -P\\%com_port% -b115200 -D -U flash:w:%output_dir%/main.hex:i 

pause