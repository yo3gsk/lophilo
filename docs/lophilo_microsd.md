# Preparing MicroSD card manually for boot

## Partitioning 

see : https://code.google.com/p/beagleboard/wiki/LinuxBootDiskFormat

Insert MicroSD card into computer, 

	Disk /dev/sdb: 7964 MB, 7964983296 bytes
	91 heads, 18 sectors/track, 9497 cylinders, total 15556608 sectors
	Units = sectors of 1 * 512 = 512 bytes
	Sector size (logical/physical): 512 bytes / 512 bytes
	I/O size (minimum/optimal): 512 bytes / 512 bytes
	Disk identifier: 0x00000000

	   Device Boot      Start         End      Blocks   Id  System
	/dev/sdb1            8192    15556607     7774208    b  W95 FAT32

the card has:  7964983296 bytes

Create new FAT32 partition:

	d 1 n p 1 <enter> +1G 

Set partition type to W95 FAT32

	t 1 b

Make it bootable:

	a 1

Create new ext4 partition

	n p 2 <enter> <enter>

Output should be:

	   Device Boot      Start         End      Blocks   Id  System
	/dev/sdb1   *        2048     2099199     1048576    b  W95 FAT32
	/dev/sdb2         2099200    15556607     6728704   83  Linux

Write out with `w`

## Filesystem creation

format for FAT32:

	sudo mkfs.msdos -F 32 /dev/sdb1 -n LOPHILOFAT32

format for EXT4:

	sudo mkfs.ext4 -L LOPHILOEXT4 /dev/sdb2

## disabling automated time checking...

output from creation:

	This filesystem will be automatically checked every 36 mounts or 180 days, whichever comes first.  Use tune2fs -c or -i to override.

we probably don't want this in embedded systems as we'd rather be explicit about the checking. 

	sudo tune2fs -c 0 -i 0 /dev/sdb2

## mounting

Make our life easier using pmount

	sudo apt-get install usbmount

	$ pmount /dev/sdb1 lophilofat32
	$ pmount /dev/sdb2 lophiloext4

Should now be available on:

	 /media/lophilofat32 
	 /media/lophiloext4

## setting up zImage and filesystem

see Lophilo/linux/Makefile.lophilo:setup-microsd

This target can be run from Lophilo/lophilo/linux submodule as:

	makel setup-microsd