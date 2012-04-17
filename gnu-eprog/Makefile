all:
	
export LOPHILO_TARGET_ARCH=arm
include ../Makefile.common
export PATH:=${TOOLCHAIN_PATH}:${PATH}

all:
	as -o add.o add.s
	ld -Ttext=0x0 -o add.elf add.o
	nm add.elf
	objcopy -O binary add.elf add.bin
