#export IMAGE=${HOME}/lophilo-bin/linux/versatilepb/zImage.arm.3.3.0-rc1.20120306-1331024313
export IMAGE=${HOME}/lophilo-bin/linux/versatilepb/vmlinux.arm.3.3.0-rc1.20120307-1331113585
export ARM_TEST=${HOME}/upstream/arm-test
export INITRD=${ARM_TEST}/arm_root.img
export QEMU=${HOME}/lophilo-bin/bin/qemu-system-arm
export DISK_NAME=squeeze1.home.qcow2
#export QEMU_PARAMETERS=-M versatilepb -trace events=./events -kernel ${IMAGE} -initrd ${INITRD} -nographic -append "console=ttyAMA0 init=/bin/busybox"
export QEMU_PARAMETERS:=-M versatilepb -nographic -s -no-reboot
#export QEMU_LINUX=-kernel ${IMAGE} -append "console=ttyAMA0 root=/dev/nfs rw nfsroot=10.0.0.13:/exports/busybox nfsrootdebug ip=dhcp"
#tap not working with virtualbox??
#export QEMU_LINUX=-kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp netmask=255.255.255.0 root=/dev/nfs nfsroot=10.0.2.15:/exports/busybox,udp"
#export QEMU_LINUX=-kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.0.2.2:/exports/busybox,udp init=/bin/busybox"

#
# host: cnshaqs10 (with nfs server) and tap device configured
#
#export QEMU_LINUX=-kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.236.10.94:/exports/busybox,udp init=/bin/sh"
#export QEMU_LINUX=-kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.236.10.94:/exports/busybox,udp init=/sbin/init"
#export QEMU_LINUX=-kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.236.10.94:/exports/busybox rw"
#export QEMU_LINUX=-kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.236.10.94:/exports/busybox-arm init=/bin/sh rw"
#export QEMU_LINUX=-kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.236.10.94:/exports/mini2440/armel-squeeze rw"
export QEMU_LINUX:=-kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.236.10.94:/exports/armel-squeeze rw"
export QEMU_NET:=-net nic -net tap,ifname=tap0
export QEMU_DISK:=-drive file=${DISK_NAME},cache=none,if=sd 
# default qemu parameters: usermode linux and smc nic
#export QEMU_NET=
export TODAY:=$(shell date +%Y%m%d)
export VERSION:=$(shell make -s -C ${SOURCE} kernelversion)

.PHONY: run

run:
	sudo ${QEMU} ${QEMU_PARAMETERS} ${QEMU_LINUX} ${QEMU_NET}

stage2:
	sudo ${QEMU} ${QEMU_PARAMETERS} ${QEMU_NET} -kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.236.10.94:/exports/squeeze rw init=/bin/sh"

squeeze:
	sudo ${QEMU} ${QEMU_PARAMETERS} ${QEMU_NET} ${QEMU_DISK} -kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.236.10.94:/exports/squeeze rw"

createdisk:
	# http://kashyapc.wordpress.com/2011/09/24/creating-a-qcow2-virtual-machine/
	qemu-img create -f qcow2 -o preallocation=metadata ${DISK_NAME} 20G
