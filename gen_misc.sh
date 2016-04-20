#!/bin/bash

:<<!
******NOTICE******
MUST set SDK_PATH & BIN_PATH firstly!!!
example:
export SDK_PATH=/medi
export BIN_PATH=~/esp8266_bin
!

export SDK_PATH=
export BIN_PATH=

# set xtensa path
export PATH=

echo "gen_misc.sh version 20150911"
echo ""

if [ $SDK_PATH ]; then
    echo "SDK_PATH:"
    echo "$SDK_PATH"
    echo ""
else
    echo "ERROR: Please export SDK_PATH in gen_misc.sh firstly, exit!!!"
    exit
fi

if [ $BIN_PATH ]; then
    echo "BIN_PATH:"
    echo "$BIN_PATH"
    echo ""
else
    echo "ERROR: Please export BIN_PATH in gen_misc.sh firstly, exit!!!"
    exit
fi


echo ""
boot=new
app=0
echo "generate bin: eagle.flash.bin+eagle.irom0text.bin"
echo ""
spi_speed=40
echo "spi speed: $spi_speed MHz"
echo ""
spi_mode=QIO
echo "spi mode: $spi_mode"
echo ""
spi_size_map=0
echo "spi size: 512KB"
echo "spi ota map:  256KB + 256KB"
echo ""

echo "start..."
echo ""

make clean

make BOOT=$boot APP=$app SPI_SPEED=$spi_speed SPI_MODE=$spi_mode SPI_SIZE_MAP=$spi_size_map
