# http://www.emdebian.org/release/squeeze.html
TARGET=/exports/squeeze
HOSTNAME=squeeze1
.PHONY: stage1

stage1:
	debootstrap --arch=armel --foreign squeeze ${TARGET} http://www.emdebian.org/grip/
	@echo "Modify /etc/exports to export ${TARGET}"

setup: setupfs setupnet setupdev setupterm setupsources
	@echo "Boot to init=/bin/sh using QEMU"
	@echo "run /debootstrap/debootstrap --second-stage"
setupfs:
	echo "proc /proc proc none 0 0" >> ${TARGET}/etc/fstab
setupnet:
	echo "squeeze1" > ${TARGET}/etc/hostname
	mkdir -p ${TARGET}/usr/share/man/man1/
setupdev:
	mknod ${TARGET}/dev/console c 5 1
	mknod ${TARGET}/dev/ttyAMA0 c 204 64
	@echo "Set root password"

setupterm:
	echo "ttyAMA0" >> ${TARGET}/etc/securetty
	echo "T0:123:respawn:/sbin/getty 115200 ttyAMA0" >> ${TARGET}/etc/inittab

setupsources:
	cp sources.list ${TARGET}/etc/apt
	@echo "apt-get update"

packages:
	@echo "apt-get install build-essential git ntpdate openssh-server"

setuptime:
	@echo "edit /etc/rc.local and add `ntpdate 2.asia.pool.ntp.org`

nfs:
	sudo service nfs-kernel-server restart
