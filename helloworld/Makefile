CROSS_COMPILE=/opt/codesourcery/arm-2011.09/bin/arm-none-linux-gnueabi-

ia32:
	/opt/codesourcery/ia32-2011.09/bin/i686-elf-gcc -static helloworld.c -o helloworld

arm:
	${CROSS_COMPILE}gcc -mlittle-endian -Wall -mno-thumb-interwork -mabi=aapcs-linux -mfloat-abi=soft -march=armv5te -mtune=arm926ej-s -msoft-float -Uarm -static helloworld.c -o helloworld

install:
	cp helloworld /exports/squeeze/home/rngadam/

objdump:
	${CROSS_COMPILE}objdump -x helloworld

test:
	@echo "make sure that the string below is v5!"
	readelf -A helloworld
