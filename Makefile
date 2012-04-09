UPSTREAM=${HOME}/upstream
LOPHILO_DIR=.

.PHONY: qemu linux all nodejs upstream openssl v8 npm2debian zlib build-couchdb linux-sam9m10 setup

all: 
	@echo "mount disks with `make aufs`"

setup: /usr/bin/openvpn /usr/bin/screen /usr/bin/ccache /usr/bin/distcc /usr/lib/libncurses.a /usr/bin/dpkg-buildpackage /usr/bin/python /usr/bin/brctl

/usr/bin/brctl:
	sudo apt-get install bridge-utils

/usr/bin/openvpn:
	sudo apt-get install openvpn

/usr/bin/screen:
	sudo apt-get install screen dvtm vim nfs-server nfs-client git openssh-server etckeeper smartmontools

/usr/lib/libncurses.a: 
	sudo apt-get install zlib1g-dev libsdl-dev glib2.0-dev binutils-gold

/usr/bin/ccache:
	sudo apt-get install ccache
       
/usr/bin/distcc:
	sudo apt-get install distcc distcc-pump

/usr/bin/mkimage:
	sudo apt-get install libncurses5-dev kernel-package uboot-mkimage

/usr/bin/dpkg-buildpackage:
	sudo apt-get install devscripts dh-make debhelper fakeroot dpkg-dev

/usr/bin/python:
	sudo apt-get install python

/gdata/ccachevol:
	sudo mkdir -p $@
	sudo chown -R ${USER}:${USER} $@

codesourcery: /opt/codesourcery/arm-2011.09/bin/arm-none-linux-gnueabi-gcc
	# from our custom Lophilo repository...
	sudo apt-get install codesourcery-arm

aufs: linux nodejs openssl v8 npm2debian zlib qemu build-couchdb

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

linux-sam9m10:
	mkdir -p ${LOPHILO_DIR}/$@
	mkdir -p ${LOPHILO_DIR}/$@-aufs
	sudo mount -t aufs -o br=${LOPHILO_DIR}/$@:${UPSTREAM}/$@ none ${LOPHILO_DIR}/$@-aufs

build-couchdb:
	mkdir -p ${LOPHILO_DIR}/$@
	mkdir -p ${LOPHILO_DIR}/$@-aufs
	sudo mount -t aufs -o br=${LOPHILO_DIR}/$@:${UPSTREAM}/$@ none ${LOPHILO_DIR}/$@-aufs
