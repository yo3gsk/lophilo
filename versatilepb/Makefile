export KBUILD_OUTPUT=${HOME}/lophilo/versatilepb
export SOURCE=${HOME}/upstream/linux
export SOURCE_IMAGE=${HOME}/upstream/arm-test/zImage.integrator

.PHONY: ccache-stats clean config menuconfig

extract: ${SOURCE_IMAGE}
	${SOURCE}/scripts/extract-ikconfig ${SOURCE_IMAGE} > .config

config: extract
	yes "" | make -C ${SOURCE} oldconfig >conf.new

menuconfig:
	time make -C ${SOURCE} menuconfig

kernel: zImage

zImage: .config
	time make -C ${SOURCE} zImage -j8

clean:
	make -C ${SOURCE} clean

ccache-stats:
	../masquerade/ccache-stats
