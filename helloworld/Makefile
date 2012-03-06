all:
#	arm-linux-gnueabi-gcc -static -march=armv5te -mtune=arm926ej-s -msoft-float -mfloat-abi=soft -static helloworld.c -o helloworld
	#arm-linux-gnueabi-gcc -mlittle-endian -Wall -mabi=apcs-gnu -mno-thumb-interwork -march=armv5te -msoft-float -Uarm -static helloworld.c -o helloworld
	arm-linux-gnueabi-gcc -mlittle-endian -Wall -mno-thumb-interwork -mabi=aapcs-linux -mfloat-abi=soft -march=armv5te -msoft-float -Uarm -static helloworld.c -o helloworld
	cp helloworld /exports/busybox

objdump:
	arm-linux-gnueabi-objdump -x helloworld

elf:
	readelf -h helloworld | grep Flags
