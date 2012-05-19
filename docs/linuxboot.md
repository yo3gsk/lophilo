# Linux booting Linux

## Goal

Use a minimal Linux kernel with minimal functionality executing directly from the SPI flash to:

* Check the MicroSD card partitioning and file system
	* If empty MicroSD card
		* Configure network stack (DHCP) and fetch using HTTP
		* Download FPGA and kernel image from Lophilo.com
		* Partition into FAT32 (32MB) and EXT4 (whatever is left)
* In parallel
	* load FPGA image from FAT32 into FPGA
	* load from MicroSD card FAT32 partiion
* kexec MicroSD card kernel

## Benefits 

* Single set of drivers between the boot firmware and the kernel
* Switch to single compiler (GNU GCC) instead of having to port from Keil down the line
* Can support complex behavior
* well-supported, constantly improving codebase
* leverage existing drivers and debugging facilities
* Support a single OS (or pseudo-OS)
* Take advantage of relatively powerful CPU (4MB Flash storage, 128MB SDRAM)
* Best and fastest TCP/IP stack (decades ahead of uIP or lwip)

## Requirements 

* Fast
	* <1s kernel startup up to our usercode
	* <1s network initialization (DHCP)
	* <1s load image into memory 
* Minimal memory usage
* Fastest network initialization possible
* No operating system image or userspace applications (single custom "init")

## Functionality required

* Block device and filesystem support
	* FAT32 support
	* EXT4 support
	* MicroSD device card support
	* partitioning support fat32/ext4
* TCP/IP
	* DHCP kernel level autoconfiguration

## Features

### XIP

http://elinux.org/Kernel_XIP

Execute-in-Place (Wikipedia entry) is a method of executing code directly from long-term storage, instead of first loading it into RAM. When the kernel is executed in place, the bootloader does not have to: 

* read the kernel from flash or
* decompress the kernel and
* write the kernel to RAM.

### kexec ###

kexec: execute user kernel with pre-initialized hardware

### Thumb-2 instruction set

20% size reduction achievable here:

* http://www.cnx-software.com/2011/04/22/compile-with-arm-thumb2-reduce-memory-footprint-and-improve-performance/
* http://www.digipedia.pl/usenet/thread/11185/42927/

## Minimal linux kernel ##

Disable everything else then:

### Kernel options ###

CONFIG_CC_OPTIMIZE_FOR_SIZE
Enabling this option will pass "-Os" instead of "-O2" to gcc  resulting in a smaller kernel. 

CONFIG_EMBEDDED
This option should be enabled if compiling the kernel for an embedded system so certain expert options are available.

CONFIG_MMU
Select if you want MMU-based virtualised addressing space  support by paged memory management. If unsure, say 'Y'.

ARM system type: Atmel AT91

CONFIG_ARM_THUMB
The Thumb instruction set is a compressed form of the standard ARM instruction set resulting in smaller binaries at the expense of slightly less efficient code.

CONFIG_UACCESS_WITH_MEMCPY
Implement faster copy_to_user and clear_user methods for CPU  cores where a 8-word STM instruction give significantly higher memory write throughput than a sequence of individual 32bit stores.

### Boot options ###

CONFIG_XIP_KERNEL
Execute-In-Place allows the kernel to run from non-volatile storage

CONFIG_KEXEC
kexec is a system call that implements the ability to shutdown your current kernel, and to start another kernel.

### Enable the block layer ###

CONFIG_BLOCK

Floating point emulation > CONFIG_FPE_FASTFPE

to include the FAST floating point emulator in the kernel.

CONFIG_MSDOS_FS
This allows you to mount MSDOS partitions of your hard drive

CONFIG_VFAT_FS
This option provides support for normal Windows file systems with long filenames.

CONFIG_MSDOS_PARTITION

### Networking ###

CONFIG_INET
These are the protocols used on the Internet and on most local Ethernets.

CONFIG_IP_PNP
enables automatic configuration of IP addresses of devices and of the routing table during kernel boot

	CONFIG_IP_PNP_DHCP
	you want the IP address of your computer to be discovered automatically at boot time using the DHCP protocol 

CONFIG_ATMEL_TCLIB
Select this if you want a library to allocate the Timer/Counter blocks found on many Atmel processors.

CONFIG_ATMEL_TCB_CLKSRC
Select this to get a high precision clocksource based on a TC block with a 5+ MHz base clock rate.

### network device support ###

CONFIG_DM9000
Support for DM9000 chipset.

CONFIG_DAVICOM_PHY
Currently supports dm9161e and dm9131

CONFIG_DMADEVICES > CONFIG_AT_HDMAC
Support the Atmel AHB DMA controller.


## explicitly disabled  ##
explicitly disabled in the minimal kernel, probably needed in full version

CONFIG_ATMEL_PWM
enables device driver support for the PWM channels

CONFIG_RTC_CLASS > CONFIG_RTC_DRV_AT91RM9200

CONFIG_ARM_PATCH_PHYS_VIRT
This can only be used with non-XIP MMU kernels where the base of physical memory is at a 16MB boundary.

CONFIG_COMPAT_BRK
On non-ancient distros (post-2000 ones) N is usually a safe choice.

CONFIG_AEBI
This option allows for the kernel to be compiled using the latest ARM ABI (aka EABI).  This is only useful if you are using a user space environment that is also compiled with EABI.

CONFIG_ATAGS_PROC
Should the atags used to boot the kernel be exported in an "atags" file in procfs. Useful with kexec.

CONFIG_IOSCHED_DEADLINE
The deadline I/O scheduler is simple and compact.

CONFIG_IOSCHED_CFQ
The CFQ I/O scheduler tries to distribute bandwidth equally

Default I/O scheduler: no/op

### MMC/MTD support is disabled for XIP ###

CONFIG_MMC
selects MultiMediaCard, Secure Digital and Secure  Digital I/O support. 

CONFIG_MMC_ATMELMCI_DMA
Say Y here to have the Atmel MCI driver use a DMA engine to do data transfers and thus increase the throughput and  reduce the CPU utilization. 

CONFIG_DMATEST
Simple DMA test client

## Networking support ##

CONFIG_MII
Most ethernet controllers have MII transceiver either as an external or internal device

### Filesystem ###

CONFIG_PROC_FS
This is a virtual file system providing information about the status of the system.

CONFIG_SYSFS
Designers of embedded systems may wish to say N here to conserve space.

### display ###

CONFIG_FB
The frame buffer device provides an abstraction for the graphics hardware.

CONFIG_BACKLIGHT_LCD_SUPPORT
Enable this to be able to choose the drivers for controlling the backlight and the LCD panel on some platforms

### weird options that get turned on... ###

CONFIG_SSB
Support for the Sonics Silicon Backplane bus.


## compile errors ##

### implicit declaration of function 'flush_dcache_page' ###

https://lkml.org/lkml/2012/4/10/375

None of mmc host drivers using linux/blkdev.h header really need anything from it. The slight exception is
atmel-mci, which uses blkdev.h only because it indirectly pulls asm/cacheflush.h

> include/linux/blkdev.h:1420:1: error: unknown type name 'bool'
> include/linux/blkdev.h: In function 'blk_needs_flush_plug':
> include/linux/blkdev.h:1422:9: error: 'false' undeclared (first use in this function)
> include/linux/blkdev.h:1422:9: note: each undeclared identifier is reported only once for each function it appears in
>  drivers/mmc/host/atmel-mci.c: In function 'atmci_read_data_pio':
>  drivers/mmc/host/atmel-mci.c:1568:5: error: implicit declaration of function 'flush_dcache_page' [-Werror=implicit-function-declaration]
>  cc1: some warnings being treated as errors

### PHYS_OFFSET ###

>  arch/arm/kernel/head.o: In function `stext':
>  (.head.text+0x34): undefined reference to `PHYS_OFFSET'
>  make[2]: *** [.tmp_vmlinux1] Error 1
>  make[1]: *** [sub-make] Error 2

see Lophilo/linux/patches

## Compilation ##

### Where to find the source ###

see: https://github.com/Lophilo/linux

This is also pulled in as a submodule to https://github.com/Lophilo/lophilo

makel is a tiny make wrapper in lophilo/bin/makel

### Build ###

Modify configuration:

makel minimal_menuconfig

Compile

makel minimal_kernel 

output is in : lophilo/obj/linux/arch/arm/boot/xipImage

arch/arm/boot/xipImage is ready (physical address: 0x00080000)

### Output ###

Kernel: 

$ ls -al ../../obj/linux/vmlinux
-rwxrwxr-x 1 rngadam rngadam 3524993 2012-05-08 12:35 ../../obj/linux/vmlinux
$ ls -alh ../../obj/linux/vmlinux
-rwxrwxr-x 1 rngadam rngadam 3.4M 2012-05-08 12:35 ../../obj/linux/vmlinux

the xipImage image size is configuration dependent:

$ ls -alh ../../obj/linux/arch/arm/boot/xipImage

By default, a 16MB image is generated; this should be changed to a 4MB or less image

make -f /home/rngadam/lophilo/upstream/linux/scripts/Makefile.build obj=arch/arm/boot MACHINE=arch/arm/mach-at91/ arch/arm/boot/xipImage
  ../../codesourcery/arm926ej-s/objcopy -O binary -R .comment -S  vmlinux arch/arm/boot/xipImage



