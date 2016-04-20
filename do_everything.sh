./gen_misc.sh && sudo esptool.py -p /dev/ttyUSB0 write_flash 0x00000 ../BIN/eagle.flash.bin 0x20000 ../BIN/eagle.irom0text.bin --verify
