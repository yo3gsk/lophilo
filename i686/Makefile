export KBUILD_OUTPUT=${HOME}/lophilo/i686
export KERNEL_VERSION=`uname -r`
export SOURCE=${HOME}/upstream/linux

.PHONY: ccache-stats

config: /boot/config-${KERNEL_VERSION}
	cp /boot/config-${KERNEL_VERSION} .config	
	yes "" | make -C ${SOURCE} oldconfig >conf.new

kernel: vmlinux

vmlinux: .config
	time make -C ${SOURCE} -j8

clean:
	make -C ${SOURCE} clean

ccache-stats:
	../masquerade/ccache-stats
