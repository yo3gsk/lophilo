# http://www.emdebian.org/release/squeeze.html

include ../Makefile.microsd

TARGET_DIR=../obj/squeeze
HOSTNAME=lophilo
DEFAULT_SERIAL_CONSOLE=ttyS1
DEFAULT_SERIAL_CONSOLE_MAJOR=4
DEFAULT_SERIAL_CONSOLE_MINOR=65
DEFAULT_SERIAL_CONSOLE_TARGET:=${TARGET_DIR}/dev/${DEFAULT_SERIAL_CONSOLE}
STAGE1_INDICATOR:=${TARGET_DIR}/debootstrap/debootstrap

.PHONY: setup

#
# initial image creation from the host system
#

${TARGET_DIR}:
	mkdir -p ${TARGET_DIR}

/usr/bin/debootstrap:
	sudo apt-get install debootstrap

${TARGET_DIR}/debootstrap/debootstrap: ${TARGET_DIR} /usr/bin/debootstrap
	sudo debootstrap --arch=armel --foreign squeeze ${TARGET_DIR} http://www.emdebian.org/grip/
	@echo "Modify /etc/exports to export ${TARGET_DIR} if you want to use NFSROOT"

#
# setup tasks
#

copy-to-microsd: setup /media/lophiloext4
	sudo rsync -avz ${TARGET_DIR}/ /media/lophiloext4/
	pumount ${LOPHILOEXT4DEV}

${TARGET_DIR}/firstboot.sh: firstboot.sh
	sudo cp firstboot.sh $@
	sudo chmod a+rx $@

setup: ${TARGET_DIR}/etc/fstab ${TARGET_DIR}/etc/hostname ${TARGET_DIR}/etc/securetty ${TARGET_DIR}/etc/inittab /etc/exports ${TARGET_DIR}/firstboot.sh
	@echo "Boot to init=/bin/sh using QEMU or the real device"
	@echo "run /debootstrap/debootstrap --second-stage"
	@echo "Please set root password on target system"

${TARGET_DIR}/etc/fstab: ${STAGE1_INDICATOR}
	@echo "Setting up /proc for $@"
	sudo sh -c "echo 'proc /proc proc none 0 0' >> $@"

${TARGET_DIR}/etc/hostname: ${TARGET_DIR}/bin/hostname ${STAGE1_INDICATOR}
	sudo sh -c "echo ${HOSTNAME} > $@"
	sudo mkdir -p ${TARGET_DIR}/usr/share/man/man1/

${TARGET_DIR}/dev/console: ${STAGE1_INDICATOR}
	sudo mknod $@ c 5 1

${TARGET_DIR}/etc/securetty: ${DEFAULT_SERIAL_CONSOLE_TARGET} ${STAGE1_INDICATOR}
	sudo sh -c "echo ${DEFAULT_SERIAL_CONSOLE} >> ${TARGET_DIR}/etc/securetty"

${TARGET_DIR}/etc/inittab: ${DEFAULT_SERIAL_CONSOLE_TARGET} ${STAGE1_INDICATOR}
	sudo sh -c "echo 'T0:123:respawn:/sbin/getty 115200 ${DEFAULT_SERIAL_CONSOLE}' >> $@"

${TARGET_DIR}/etc/apt/sources.list: ${STAGE1_INDICATOR}
	sudo cp sources.list ${TARGET_DIR}/etc/apt

${TARGET_DIR}/etc/rc.local: ${STAGE1_INDICATOR}
	sudo sh -c "echo ntpdate 2.asia.pool.ntp.org >> $@"

#
# sub-targets
#
${DEFAULT_SERIAL_CONSOLE_TARGET}: 
	sudo mknod ${TARGET_DIR}/dev/${DEFAULT_SERIAL_CONSOLE} c ${DEFAULT_SERIAL_CONSOLE_MAJOR} ${DEFAULT_SERIAL_CONSOLE_MINOR}

/etc/exports: ${TARGET_DIR}
	sudo service nfs-kernel-server restart
