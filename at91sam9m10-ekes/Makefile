export LOPHILO_TARGET_ARCH=arm include ../Makefile.common

export TODAY:=$(shell date +%Y%m%d)
#export VERSION:=$(shell make -s -C ${SOURCE} kernelversion)
export IMAGE=../linux-sam9m10/arch/arm/boot/uImage
export ARM_TEST=${HOME}/upstream/arm-test
export INITRD=${ARM_TEST}/arm_root.img
#export QEMU=../qemu/arm-softmmu/qemu-system-arm
export QEMU=${HOME}/upstream/qemu_at91sam/arm-softmmu/qemu-system-arm
export QEMU_IMG=../qemu/qemu-img
export DISK_NAME=squeeze1.home.qcow2
#export QEMU_PARAMETERS:=-M lophilo -nographic -s -no-reboot
#export QEMU_PARAMETERS:=-M at91pes -nographic -s -no-reboot -serial stdio
export QEMU_PARAMETERS:=-M at91sam9263ek -nographic -s -S -no-reboot -serial stdio
export QEMU_AT91BOOT:=-boot n -option-rom ./flash.bin -pflash ./flash.bin
export QEMU_LINUX:=-kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.236.10.94:/exports/armel-squeeze rw"
# randomly generated macaddr
export QEMU_NET:=-net nic,macaddr=72:e3:fe:c1:ce:21 -net tap,ifname=tap0
export QEMU_DISK:=-drive file=${DISK_NAME},cache=none,if=sd 
export AT91BOOTSTRAP:=../Bootstrap-v1.16/board/at91sam9m10ekes/dataflash/dataflash_at91sam9m10ekes.bin

.PHONY: arm-test stage2 squeeze createdisk genmac setup

setup:
	sudo openvpn --dev tap0 --mktun
	sudo brctl addbr br0

arm-test:
	sudo ${QEMU} ${QEMU_PARAMETERS} ${QEMU_LINUX} ${QEMU_NET}

bootstrap:
	sudo ${QEMU} ${QEMU_PARAMETERS} 

adder:
	sudo ${QEMU} ${QEMU_PARAMETERS} -pflash ./flash.bin -boot n -option-rom ./option.rom

at91bootstrap: bootstrap.bin
	# we cheat and start with the bootstrap in ROM...
	sudo ${QEMU} ${QEMU_PARAMETERS} -pflash bootstrap.bin -boot n -option-rom  bootstrap.bin

	#../boot-rom/test.bin

stage2:
	sudo ${QEMU} ${QEMU_PARAMETERS} ${QEMU_NET} -kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.236.10.94:/exports/squeeze rw init=/bin/sh"

squeeze:
	sudo ${QEMU} ${QEMU_PARAMETERS} ${QEMU_NET} ${QEMU_DISK} -kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.236.10.94:/exports/squeeze rw"

createdisk:
	# http://kashyapc.wordpress.com/2011/09/24/creating-a-qcow2-virtual-machine/
	${QEMU_IMG}create -f qcow2 -o preallocation=metadata ${DISK_NAME} 20G

genmac:
	# http://osxdaily.com/2010/11/10/random-mac-address-generator/
	# see /etc/qemu-ifup
	openssl rand -hex 6 | sed 's/\(..\)/\1:/g; s/.$$//'

flash:
	-rm flash.bin
	# minimum size 4MB?
	dd if=/dev/zero of=flash.bin count=4096 bs=1024
	dd if=../gnu-eprog/add.bin of=flash.bin conv=notrunc

bootstrap.bin: ${AT91BOOTSTRAP}
	-rm $@
	# exact size of 4MB expected
	dd if=/dev/zero of=$@ count=4096 bs=1024
	dd if=${AT91BOOTSTRAP} of=$@ conv=notrunc

gdb:
	# use display/i $pc
	 /opt/codesourcery/arm-2011.09/bin/arm-none-eabi-gdb -x gdbremote.cmd
