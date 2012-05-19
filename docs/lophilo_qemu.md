# Lophilo QEMU

QEMU is a machine emulator the converts the target machine CPU instructions to the local machine enabling running machines from a wide variety of architecture including ARM.

Lophilo QEMU is an attempt at supporting the AT91SAM9M10 MPU and the Lophilo hardware specific extensions to permit developing and testing software without the actual hardware platform.

## Source 

The QEMU fork is available at: git@github.com:Lophilo/qemu_lophilo.git

* Makefile.lophilo: Lophilo specific build instructions
* hw/lophilo.c: machine descriptor
* hw/at91*: the drivers we use

## Usage

Once built, you can run by:

./arm-softmmu/qemu-system-arm -M ?

sudo ./arm-softmmu/qemu-system-arm -M lophilo -kernel /media/lophilofat32/zImage -gdb

run -M at91sam9263ek -s -option-rom option-rom.lophilo -pflash spiflash.lophilo

## Debugging

We'll use a special gdb version that supports ARM.

Start QEMU with option -gdb

run -M at91sam9263ek -s

gdb ./arm-softmmu/qemu-system-arm
(gdb) run -M at91sam9263ek -s

In another shell:

	../../codesourcery/arm926ej-s/gdb 
	target remote :1234

## With lophilo

 ./arm-softmmu/qemu-system-arm -M lophilo -kernel /media/lophilofat32/zImage -s -nographic 

## Current state

work on hw/at91sam9263.c; it compiles, but the hw/pflash_atmel is hopelessly broken and it looks like spi_flash.c (which is what we would use in our system) is also broken.

Now that it compiles, probably need to copy to hw/lophilo.c and make changes there to reflect our real system; perhaps load directly the kernel to proper location in SDRAM and skip loading from SPI flash or SDCard for now.