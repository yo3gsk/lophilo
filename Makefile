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

nodejs:
	#mkdir -p ${LOPHILO}/node-aufs
	#sudo mount -t aufs -o br=${LOPHILO}/node:${UPSTREAM}/node none ${LOPHILO}/node-aufs
	cp -rsu ${UPSTREAM}/node ${LOPHILO}

qemu:
	cp -rsu ${UPSTREAM}/qemu ${LOPHILO}

upstream:
	mkdir -p ${UPSTREAM}
	sudo mount -t nfs cnshaqs10:/home/rngadam/upstream ${UPSTREAM}
