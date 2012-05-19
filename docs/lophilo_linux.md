# Lophilo Linux

## Configuration

Based on the AT91SAMG45 configuration.

We do `make ARCH=arm at91sam9g45_defconfig` and then make modifications based on our differences.

Below configuration notes for each feature

## General Linux kernel 

* General setup
	* Kernel .config support CONFIG_IKCONFIG
	* /proc/config.gz CONFIG_IKCONFIG_PROC
	* DISABLE Initial RAM filesystem CONFIG_BLK_DEV_INITRD
	* DISABLE Optimize for size CONFIG_CC_OPTIMIZE_FOR_SIZE

* System type
	* Atmel AT91 System-on-Chip
		* AT91_EARLY_USART1

* Boot options
	* Default kernel command string: mem=128M console=ttyS1,115200o81 root=/dev/mmcblk1p1 rw
		* no initrd drive
		* mem=128M
		* root=/dev/mmcblk1p1: root device (MicroSD card, 2nd partition) 
		* console=ttyS1,115200o81
			* Speed (baud): 115200		
			* Data bits: 8
			* Stop bits: 1
			* Parity: Odd -> o
			* Flow control: none

* Enable the block layer
	* Advanced partition selection CONFIG_PARTITION_ADVANCED
		* CONFIG_MSDOS_PARTITION
	* I/O Schedulers -> No-op

* Filesystems
	* DISABLE CONFIG_EXT2_FS
	* CONFIG_EXT4_FS
	* CONFIG_MSDOS_FS (strangely, VFAT is enabled...)

Atmel specific (already enabled by default):

* Device drivers
	* Sound card support
		* CONFIG_SND_ATMEL_AC97C

* Device Drivers
	* Character devices
		* Serial drivers
			* CONFIG_SERIAL_ATMEL
			* CONFIG_SERIAL_ATMEL_CONSOLE
			* DISABLE CONFIG_SERIAL_ATMEL_PDC
			Note that break and error handling currently doesn't work properly when DMA is enabled. Make sure that ports where this matters don't use DMA.   
		* CONFIG_HW_RANDOM_ATMEL



## Wifi header

* cfg80211 /  firmware file rtlwifi/rtl8192cufw.bin
* CONFIG_CFG80211 should be enabled

## MicroSD card


## compile errors

/home/rngadam/lophilo/upstream/linux/arch/arm/include/asm/memory.h:169:0: warning: "PHYS_OFFSET" redefined [enabled by default]
/home/rngadam/lophilo/upstream/linux/arch/arm/include/asm/memory.h:148:0: note: this is the location of the previous definition

## Change back to DBGU

* Boot options
	* Default kernel command string: mem=128M console=ttyS0,115200o81 rw
		* no initrd drive
		* mem=128M
		* root device (MicroSD card, 2nd partition) 
		* console=ttyS0,115200o81
			* Speed (baud): 115200		
			* Data bits: 8
			* Stop bits: 1
			* Parity: Odd -> o
			* Flow control: none

## Review with Shyu's

Make sure MTD and NAND is disabled
CONFIG_DEFAULT_HOSTNAME: Assign default hostname to lophilo

 * Enable loadable module support
 	* Forced module loading and unloading, module versioning support

 * Block device
 	* Support for large (2TB+) block devices and files: CONFIG_LBDAF: r/w support for ext4 with huge_file support


  * IO schedulers: CFQ + deadline IO schedulers enabled (even with no-op configured)

  * AT91_EARLY_DBGU1: should be set to DBGU, not the serial ports as there is a bug that causes a kernel hang

  * Test "Suspend-to-RAM disables main oscillator": CONFIG_AT91_SLOW_CLOCK

  * Test CONFIG_HIGH_RES_TIMERS: High Resolution Timer Support: does this work?

  * CONFIG_OABI_COMPAT: Allow old ABI binaries to run with this kernel (EXPERIMENTAL): Shyu's wants to keep this out to keep the kernel small

  *  CONFIG_CMDLINE
   	* rootwait: SD card has some additional latency
   	* ttyS1: DBGU: ttyS0, UART0: ttyS1 (not connected at all, conflicting with ISI interface), UART1: ttyS2
	* rootfstype=ext4: faster boot without probing the filesystem
	* ip=dhcp: right off the bat

  * CONFIG_VFP: ARMv5 has a VFP unit (test comparison between with and without, good for stuff like OpenCV potentially?)
 
  * CONFIG_IP_PNP: good for NFS roots and configuring the IP before a root filesystem

  * Wireless 
    * Amateur Radio support: will be enabled later, Shyu will add HW for this
  	* CONFIG_LIB80211_DEBUG: for debugging
  	* CONFIG_MAC80211_MESH: Enable mac80211 mesh networking (pre-802.11s) support 
  		* fun feature, we should try it out with our Lophilo's, configuration tool from: http://o11s.org/
  	* CONFIG_MAC80211_LEDS: need some software-level configuration (get configuration tool)
  	* CONFIG_BT_HCIBTUSB: Bluetooth HCI USB driver. For USB Bluetooth dongle
  	* RFCOMM (serial communication), BNEP (networking over bluetooth, w/multicast), HIDP (devices)

  * Device drivers
  	* Misc Devices
  		* Altera FPGA firmware download module: disabled, looks interesting but specific to one hardware platform
  		* SCSI device support: needed for USB mass storage
  			* CONFIG_SCSI: ...version of the IOMEGA ZIP drive, USB storage devices, Fibre...
  			* CONFIG_BLK_DEV_SD: ...USB storage or the SCSI or parallel port version of...
  	* USB Network Adapters: all deselected, we have real ethernet on-board...
  	* Wireless LAN: 
  		* select anything USB
  		* IEEE 802.11 for Host AP (Prism2/2.5/3 and WEP/TKIP/CCMP): CONFIG_HOSTAP
  		* CONFIG_HOSTAP_FIRMWARE: ???
  		* Realtek RTL8192CU/RTL8188CU USB Wireless Network Adapter : our provided USB wifi dongle
  			* Additional debugging: just until release
  	* Input device support: 
  		* Touchscreens disabled: we don't have any hardware yet, don't want the interrupts and GPIO be used
  	* disable joysticks, keyboards
  	* disable  Polled input device skeleton
  	* Serial drivers
  		*  CONFIG_SERIAL_ATMEL_PDC: faster!
	* CONFIG_HW_RANDOM_ATMEL: selected but not activated (need to change the mach-at91)
	*  CONFIG_RAW_DRIVER: RAW driver (/dev/raw/rawN): could be useful for the FPGA driver
	* I2C and SPI support disabled: not relevant
	* CONFIG_GPIO_SYSFS: configure later for user access to GPIO pins
	* Watchdog Timer Support: AT91SAM9X / AT91CAP9 watchdog: watch out for spontaneous reboots!
		*  CONFIG_WATCHDOG_NOWAYOUT: set to yes, this reflects the hardware (never stops) - to be tested

	* disable all Graphics support for now (no LCD)
	* Sound card
		* CONFIG_SND_ATMEL_AC97C: we have an AC97 in our system!
	* CONFIG_USB_HID: all USB devices
	* USB
		* CONFIG_USB_DEVICEFS: deprecated so we remove it
		* both EHCI and OCHI
		* CONFIG_USB_TMC: can connect signal generators! to be tested!
		* CONFIG_USB_UAS: typical harddrive interface
		* CONFIG_USB_SERIAL_FTDI_SIO: very common dongle, support Arduino board
		* USB Prolific 2303 Single Port Serial Driver: very common dongle, support Arduino board
		* USB Gadget drivers
			* CONFIG_USB_AT91: Atmel AT91 USB Device Port
			* CONFIG_USB_ETH: Ethernet Gadget (with CDC Ethernet support): networking to host
	* MMC
		* CONFIG_MMC_DEBUG: for development, remove later
		* Atmel SD/MMC Driver: 
		* CONFIG_MMC_ATMELMCI_DMA: faster transfer
	* LED support
		*  CONFIG_LEDS_ATMEL_PWM: atmel led control
		*  CONFIG_LEDS_GPIO: our system has LEDs and are connected to GPIO
		* LED Timer, Heartbeat, backlight, GPIO, default on
	* Real-time clock
		* CONFIG_RTC_DRV_AT91RM9200: RTC: we have hardware driver
	* DMA
		* CONFIG_AT_HDMAC

	* CDROM/DVD: enabled with joliet and UDF options in case someone wants to connect a USB DVD driver
		* CONFIG_ISO9660_FS

	* DOS/FAT/NT: all enabled as we want to plug windows storage

	* Misc filesystems: all disabled

	* Native language support: for FAT filesystems. Add UTF-8

	* Kernel hacking: separate configuration

## Kernel crash

at91_ohci at91_ohci: can't request overcurrent gpio 0
at91_ohci at91_ohci: AT91 OHCI
at91_ohci at91_ohci: new USB bus registered, assigned bus number 2
at91_ohci at91_ohci: irq 22, io mem 0x00700000
Unable to handle kernel NULL pointer dereference at virtual address 0000000c
pgd = c0004000
[0000000c] *pgd=00000000
Internal error: Oops: 5 [#1] ARM
Modules linked in:
CPU: 0    Not tainted  (3.4.0-rc6+ #10)
PC is at ohci_irq+0xc/0x1ac
LR is at usb_hcd_irq+0x34/0x44
pc : [<c02bd5dc>]    lr : [<c02a55b0>]    psr: 60000093
sp : c7821cf8  ip : c049f148  fp : 00000001
r10: c0610564  r9 : c0636fd1  r8 : 00000016
r7 : 00000080  r6 : 00000000  r5 : 00000001  r4 : c7920800
r3 : 00000001  r2 : 00000021  r1 : c02bd5d0  r0 : c7920800
Flags: nZCv  IRQs off  FIQs on  Mode SVC_32  ISA ARM  Segment kernel
Control: 0005317f  Table: 70004000  DAC: 00000017
Process swapper (pid: 1, stack limit = 0xc7820270)
Stack: (0xc7821cf8 to 0xc7822000)
1ce0:                                                       00000001 a0000093
1d00: 00000001 00000001 00000080 c02a55b0 c7924f20 c004ef44 00000020 c01bfeb8
1d20: 80000013 c0610564 00000016 00000000 c7821da4 00000003 c06379b0 60000013
1d40: 60000013 c004f0e8 c0610564 c0050fe0 c061946c c004e8c0 000000c0 c000ed0c
1d60: c001cde8 60000013 fefff000 c000e118 c060d798 c060d7e0 c060d7e0 00001965
1d80: c06379d6 c0637a05 c060d798 00000035 00000003 c06379b0 60000013 60000013
1da0: 00000000 c7821db8 c001cb6c c001cde8 60000013 ffffffff 6963686f 6463685f
1dc0: 6273753a 00000032 c06379b0 00000000 00000000 60000013 00000000 00000000
1de0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
1e00: 00000000 00000000 00000000 00000000 00000006 00000000 00000000 c7821e7c
1e20: c060c510 c0539608 00000000 c0627920 c060c510 00000011 00000000 c0475d68
1e40: 00000000 c7821e54 00000016 c01f0884 c055b73c c0539608 c053b8f0 c7849640
1e60: c7821e7c c7920800 c7920800 00000016 00700000 c01f08dc c7920800 c056f07c
1e80: c7821e84 c7821e90 c02a6a28 c056f07c 00000016 c056efc8 00700000 00000000
1ea0: c060ac9c c7920400 c060ad58 c7920800 c060c510 c7920a80 c8c00000 c0571020
1ec0: c060c508 c0474640 00000000 00000000 c784be08 00000000 20000013 c0642678
1ee0: 20000013 c060c510 00000000 c0628018 c05fcc14 c05d820c 00000000 c01f4c3c
1f00: c01f4c28 c01f3840 c060c510 c0628018 c060c544 00000000 c05eba24 c01f3a44
1f20: c0628018 c01f39b8 00000000 c01f21ec c78183cc c78439f0 c0628018 c061ce20
1f40: c7927e60 c01f30a4 c053b8f0 c05f81ec c0628018 c7820000 c06370a0 00000000
1f60: c05eba24 c05fcc14 c05d820c c01f3ee4 c01f4c60 c0642d94 c7820000 c06370a0
1f80: 00000000 c05eba24 c05d820c c05eba68 c05f81ec c0008784 00000006 00000006
1fa0: 00000000 c05f81e8 c05f81ec 00000006 c05f81cc c06370a0 000000ba c05d820c
1fc0: 00000000 c05d8864 00000006 00000006 c05d820c 00000000 00000000 c05d8784
1fe0: c000ee04 00000013 00000000 00000000 00000000 c000ee04 ffbfabbb bfbaddfe
[<c02bd5dc>] (ohci_irq+0xc/0x1ac) from [<c02a55b0>] (usb_hcd_irq+0x34/0x44)
[<c02a55b0>] (usb_hcd_irq+0x34/0x44) from [<c004ef44>] (handle_irq_event_percpu+0x50/0x1cc)
[<c004ef44>] (handle_irq_event_percpu+0x50/0x1cc) from [<c004f0e8>] (handle_irq_event+0x28/0x38)
[<c004f0e8>] (handle_irq_event+0x28/0x38) from [<c0050fe0>] (handle_level_irq+0x80/0xcc)
[<c0050fe0>] (handle_level_irq+0x80/0xcc) from [<c004e8c0>] (generic_handle_irq+0x28/0x3c)
[<c004e8c0>] (generic_handle_irq+0x28/0x3c) from [<c000ed0c>] (handle_IRQ+0x30/0x98)
[<c000ed0c>] (handle_IRQ+0x30/0x98) from [<c000e118>] (__irq_svc+0x38/0x60)
[<c000e118>] (__irq_svc+0x38/0x60) from [<c001cde8>] (vprintk+0x1dc/0x4e8)
[<c001cde8>] (vprintk+0x1dc/0x4e8) from [<c0475d68>] (printk+0x20/0x30)
[<c0475d68>] (printk+0x20/0x30) from [<c01f0884>] (__dev_printk+0x40/0x64)
[<c01f0884>] (__dev_printk+0x40/0x64) from [<c01f08dc>] (_dev_info+0x34/0x48)
[<c01f08dc>] (_dev_info+0x34/0x48) from [<c02a6a28>] (usb_add_hcd+0x404/0x62c)
[<c02a6a28>] (usb_add_hcd+0x404/0x62c) from [<c0474640>] (ohci_hcd_at91_drv_probe+0x3b4/0x4a4)
[<c0474640>] (ohci_hcd_at91_drv_probe+0x3b4/0x4a4) from [<c01f4c3c>] (platform_drv_probe+0x14/0x18)
[<c01f4c3c>] (platform_drv_probe+0x14/0x18) from [<c01f3840>] (driver_probe_device+0x78/0x1f0)
[<c01f3840>] (driver_probe_device+0x78/0x1f0) from [<c01f3a44>] (__driver_attach+0x8c/0x90)
[<c01f3a44>] (__driver_attach+0x8c/0x90) from [<c01f21ec>] (bus_for_each_dev+0x54/0x7c)
[<c01f21ec>] (bus_for_each_dev+0x54/0x7c) from [<c01f30a4>] (bus_add_driver+0x1b4/0x278)
[<c01f30a4>] (bus_add_driver+0x1b4/0x278) from [<c01f3ee4>] (driver_register+0x78/0x178)
[<c01f3ee4>] (driver_register+0x78/0x178) from [<c05eba68>] (ohci_hcd_mod_init+0x44/0x84)
[<c05eba68>] (ohci_hcd_mod_init+0x44/0x84) from [<c0008784>] (do_one_initcall+0x30/0x168)
[<c0008784>] (do_one_initcall+0x30/0x168) from [<c05d8864>] (kernel_init+0xe0/0x198)
[<c05d8864>] (kernel_init+0xe0/0x198) from [<c000ee04>] (kernel_thread_exit+0x0/0x8)
Code: c0570f00 e92d40f8 e59060d8 e1a04000 (e596300c)
---[ end trace 01f3805d32a11584 ]---
Kernel panic - not syncing: Fatal exception in interrupt






































