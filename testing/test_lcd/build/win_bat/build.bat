set com_port=.\COM8

del *.elf *.hex *.lsp *.o *.map


call compile.bat

avrdude -V -v -v -v -p%mmcu% -cwiring -P\\%com_port% -b115200 -D -U flash:w:%output_dir%/main.hex:i 

pause