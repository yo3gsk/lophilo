#CROSS_COMPILE=arm-linux-gnueabi-
CROSS_COMPILE=${HOME}/arm-2011.09/bin/arm-none-linux-gnueabi-
all:
	${CROSS_COMPILE}gcc -mlittle-endian -Wall -mno-thumb-interwork -mabi=aapcs-linux -mfloat-abi=soft -march=armv5te -mtune=arm926ej-s -msoft-float -Uarm -static helloworld.c -o helloworld

install:
	cp helloworld /exports/squeeze/home/rngadam/

objdump:
	${CROSS_COMPILE}objdump -x helloworld

test:
	@echo "make sure that the string below is v5!"
	readelf -A helloworld
