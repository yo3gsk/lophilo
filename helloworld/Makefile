all:
	arm-linux-gnueabi-gcc -mlittle-endian -Wall -mno-thumb-interwork -mabi=aapcs-linux -mfloat-abi=soft -march=armv5te -mtune=arm926ej-s -msoft-float -Uarm -static helloworld.c -o helloworld

install:
	cp helloworld /exports/busybox

objdump:
	arm-linux-gnueabi-objdump -x helloworld

elf:
	readelf -A helloworld
