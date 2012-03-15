UPSTREAM=${HOME}/upstream
LOPHILO_DIR=.

.PHONY: qemu linux all nodejs upstream openssl v8 npm2debian zlib

all: 
	@echo "mount disks with `make aufs`"

aufs: linux nodejs openssl v8 npm2debian zlib qemu

linux:
	# make-kpkg packages symlinks... and has no option
	# to build out of tree
	#cp -rsu ${UPSTREAM}/linux ${LOPHILO_DIR}
	mkdir -p ${LOPHILO_DIR}/linux-aufs
	sudo mount -t aufs -o br=${LOPHILO_DIR}/linux:${UPSTREAM}/linux none ${LOPHILO_DIR}/linux-aufs

nodejs:
	# node.js tries to write to the config files...
	mkdir -p ${LOPHILO_DIR}/node-aufs
	sudo mount -t aufs -o br=${LOPHILO_DIR}/node:${UPSTREAM}/node none ${LOPHILO_DIR}/node-aufs

qemu:
	mkdir -p ${LOPHILO_DIR}/$@
	mkdir -p ${LOPHILO_DIR}/$@-aufs
	sudo mount -t aufs -o br=${LOPHILO_DIR}/$@:${UPSTREAM}/$@ none ${LOPHILO_DIR}/$@-aufs

upstream:
	mkdir -p ${UPSTREAM}
	sudo mount -t nfs cnshaqs10:/home/rngadam/upstream ${UPSTREAM}

openssl:
	mkdir -p ${LOPHILO_DIR}/openssl-aufs
	sudo mount -t aufs -o br=${LOPHILO_DIR}/openssl:${UPSTREAM}/openssl none ${LOPHILO_DIR}/openssl-aufs

v8:
	mkdir -p ${LOPHILO_DIR}/v8-aufs
	sudo mount -t aufs -o br=${LOPHILO_DIR}/v8:${UPSTREAM}/v8 none ${LOPHILO_DIR}/v8-aufs

npm2debian:
	mkdir -p ${LOPHILO_DIR}/$@
	mkdir -p ${LOPHILO_DIR}/$@-aufs
	sudo mount -t aufs -o br=${LOPHILO_DIR}/$@:${UPSTREAM}/$@ none ${LOPHILO_DIR}/$@-aufs

zlib:
	mkdir -p ${LOPHILO_DIR}/$@
	mkdir -p ${LOPHILO_DIR}/$@-aufs
	sudo mount -t aufs -o br=${LOPHILO_DIR}/$@:${UPSTREAM}/$@ none ${LOPHILO_DIR}/$@-aufs
