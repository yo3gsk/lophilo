export KBUILD_OUTPUT=${HOME}/lophilo/versatilepb
export SOURCE=${HOME}/upstream/linux
export SOURCE_IMAGE=${HOME}/upstream/arm-test/zImage.integrator
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-
export IMAGE=${KBUILD_OUTPUT}/arch/arm/boot/zImage
#export IMAGE=${KBUILD_OUTPUT}/zImage.last
export ARM_TEST=${HOME}/upstream/arm-test
export INITRD=${ARM_TEST}/arm_root.img
export QEMU=${HOME}/upstream/qemu/arm-softmmu/qemu-system-arm
#export QEMU_PARAMETERS=-M versatilepb -trace events=./events -kernel ${IMAGE} -initrd ${INITRD} -nographic -append "console=ttyAMA0 init=/bin/busybox"
export QEMU_PARAMETERS=-M versatilepb -nographic 
#export QEMU_LINUX=-kernel ${IMAGE} -append "console=ttyAMA0 root=/dev/nfs rw nfsroot=10.0.0.13:/exports/busybox nfsrootdebug ip=dhcp"
#tap not working with virtualbox??
#export QEMU_NET=-net nic -net tap,ifname=tap0
#export QEMU_LINUX=-kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp netmask=255.255.255.0 root=/dev/nfs nfsroot=10.0.2.15:/exports/busybox,udp"
export QEMU_LINUX=-kernel ${IMAGE} -append "console=ttyAMA0 ip=dhcp root=/dev/nfs nfsroot=10.0.2.2:/exports/busybox,udp init=/bin/busybox"
export QEMU_NET=
export TODAY:=$(shell date +%Y%m%d)
export VERSION:=$(shell make -s -C ${SOURCE} kernelversion)

.PHONY: ccache-stats clean config menuconfig test backup

.config: ${SOURCE_IMAGE}
	${SOURCE}/scripts/extract-ikconfig ${SOURCE_IMAGE} > .config

config: .config
	yes "" | make -C ${SOURCE} oldconfig > conf.new

menuconfig:
	time make -C ${SOURCE} menuconfig

backup:
	mkdir -p ${HOME}/lophilo-bin/linux/versatilepb/
	cp ${IMAGE} ${HOME}/lophilo-bin/linux/versatilepb/zImage.${ARCH}.${VERSION}.${TODAY}

kernel: ${IMAGE}

${IMAGE}: .config
	time make -C ${SOURCE} zImage

version:
	make -C ${SOURCE} kernelversion

test:
	sudo ${QEMU} ${QEMU_PARAMETERS} ${QEMU_LINUX} ${QEMU_NET}

clean:
	make -C ${SOURCE} clean
