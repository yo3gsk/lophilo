UPSTREAM=${HOME}/upstream
LOPHILO=${HOME}/lophilo

.PHONY: qemu linux all nodejs upstream openssl

all: linux qemu

linux:
	# make-kpkg packages symlinks... and has no option
	# to build out of tree
	#cp -rsu ${UPSTREAM}/linux ${LOPHILO}
	mkdir -p ${LOPHILO}/linux-aufs
	sudo mount -t aufs -o br=${LOPHILO}/linux:${UPSTREAM}/linux none ${LOPHILO}/linux-aufs

nodejs:
	# node.js tries to write to the config files...
	mkdir -p ${LOPHILO}/node-aufs
	sudo mount -t aufs -o br=${LOPHILO}/node:${UPSTREAM}/node none ${LOPHILO}/node-aufs

qemu:
	cp -rsu ${UPSTREAM}/qemu ${LOPHILO}

upstream:
	mkdir -p ${UPSTREAM}
	sudo mount -t nfs cnshaqs10:/home/rngadam/upstream ${UPSTREAM}

openssl:
	mkdir -p ${LOPHILO}/openssl-aufs
	sudo mount -t aufs -o br=${LOPHILO}/openssl:${UPSTREAM}/openssl none ${LOPHILO}/openssl-aufs
