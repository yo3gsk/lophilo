export KBUILD_OUTPUT=${HOME}/lophilo/i686
export KERNEL_VERSION=`uname -r`
export SOURCE=${HOME}/upstream/linux

config:
	cp /boot/config-${KERNEL_VERSION} .config	
	yes "" | make -C ${SOURCE} oldconfig >conf.new

kernel: ./boot/output

./boot/output: .config Makefile
	time make -C ${SOURCE} -j8

clean:
	make -C ${SOURCE} clean
