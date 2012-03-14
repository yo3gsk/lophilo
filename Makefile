UPSTREAM=${HOME}/upstream
LOPHILO=${HOME}/lophilo

.PHONY: qemu linux all nodejs upstream openssl v8

all: 
	@echo "mount disks with `make aufs`"

aufs: linux nodejs openssl v8

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

v8:
	mkdir -p ${LOPHILO}/v8-aufs
	sudo mount -t aufs -o br=${LOPHILO}/v8:${UPSTREAM}/v8 none ${LOPHILO}/v8-aufs
