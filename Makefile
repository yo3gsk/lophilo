UPSTREAM=${HOME}/upstream
LOPHILO=${HOME}/lophilo

.PHONY: qemu linux

all: linux qemu

linux:
	cp -rsu ${UPSTREAM}/linux ${LOPHILO}

qemu:
	cp -rsu ${UPSTREAM}/qemu ${LOPHILO}
