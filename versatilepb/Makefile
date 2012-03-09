export TODAY:=$(shell date +%Y%m%d)
export VERSION:=$(shell make -s -C ${SOURCE} kernelversion)
export IMAGE=${HOME}/lophilo-bin/linux/versatilepb/zImage.arm.latest
export ARM_TEST=${HOME}/upstream/arm-test
export INITRD=${ARM_TEST}/arm_root.img
export QEMU=${HOME}/lophilo-bin/bin/qemu-system-arm
export DISK_NAME=squeeze1.home.qcow2
export QEMU_PARAMETERS:=-M versatilepb -nographic -s -no-reboot
export QEMU_LINUX:=-kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.236.10.94:/exports/armel-squeeze rw"
# randomly generated macaddr
export QEMU_NET:=-net nic,macaddr=52:54:00:12:34:56 -net tap,ifname=tap0
export QEMU_DISK:=-drive file=${DISK_NAME},cache=none,if=sd 

.PHONY: arm-test stage2 squeeze createdisk genmac

arm-test:
	sudo ${QEMU} ${QEMU_PARAMETERS} ${QEMU_LINUX} ${QEMU_NET}

stage2:
	sudo ${QEMU} ${QEMU_PARAMETERS} ${QEMU_NET} -kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.236.10.94:/exports/squeeze rw init=/bin/sh"

squeeze:
	sudo ${QEMU} ${QEMU_PARAMETERS} ${QEMU_NET} ${QEMU_DISK} -kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.236.10.94:/exports/squeeze rw"

createdisk:
	# http://kashyapc.wordpress.com/2011/09/24/creating-a-qcow2-virtual-machine/
	qemu-img create -f qcow2 -o preallocation=metadata ${DISK_NAME} 20G

genmac:
	# http://osxdaily.com/2010/11/10/random-mac-address-generator/
	# see /etc/qemu-ifup
	openssl rand -hex 6 | sed 's/\(..\)/\1:/g; s/.$$//'
