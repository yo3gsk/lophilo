export LOPHILO_TARGET_ARCH=arm
include ../Makefile.common
export PATH:=${TOOLCHAIN_PATH}:${PATH}

all: test.bin

config:
	which as
	echo ${PATH}

test.bin: test.c test.ld startup.s
	as -mcpu=arm926ej-s -g startup.s -o startup.o
	gcc -c -mcpu=arm926ej-s -g test.c -o test.o
	#ld -T test.ld test.o startup.o -o test.elf
	ld -Ttext=0x0 test.o -o test.elf
	objcopy -O binary test.elf test.bin

run:
	qemu-system-arm -M versatilepb -m 128M -nographic -kernel test.bin

debugging:
	qemu-system-arm -M versatilepb -m 128M -nographic -s -S -kernel test.bin

symbols:
	nm test.elf

debugger:
	arm-linux-gnueabi-gdb 

clean:
	rm -f test.elf test.bin *.o
