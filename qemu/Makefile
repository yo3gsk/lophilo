TMPDIR := /tmp
PATH := ${HOME}/lophilo/masquerade:${PATH}
export PATH

SOURCE=${HOME}/upstream/qemu
.PHONY: qemu-arm clean install compiler clear-cache

compiler:
	@echo ${PATH}
	which cc
	
qemu-arm:
	$(sh cd $SOURCE ; ./configure --target-list=arm-softmmu --enable-trace-backend=simple --prefix=$HOME/lophilo-bin)
	time make -C $(SOURCE) -j4

install:
	make -C $(SOURCE) install

clean:
	make -C $(SOURCE) clean

clear-cache:
	ccache-clear
