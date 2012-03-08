UPSTREAM=${HOME}/upstream
LOPHILO=${HOME}/lophilo

.PHONY: qemu linux

all: linux qemu

linux:
	# make-kpkg packages symlinks... and has no option
	# to build out of tree
	#cp -rsu ${UPSTREAM}/linux ${LOPHILO}
	mkdir -p ${LOPHILO}/linux-aufs
	sudo mount -t aufs -o br=${LOPHILO}/linux:${UPSTREAM}/linux none ${LOPHILO}/linux-aufs

qemu:
	cp -rsu ${UPSTREAM}/qemu ${LOPHILO}

gcc:
	cp -rsu ${UPSTREAM}/gcc ${LOPHILO}
