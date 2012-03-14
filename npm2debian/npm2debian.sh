#!/bin/bash
ARM_TOOLCHAIN=${HOME}/lophilo/codesourcery/arm926ej-s

export EMAIL="rngadam@lophilo.com"
export DEBFULLNAME="Ricky Ng-Adam"
export PATH=${ARM_TOOLCHAIN_PATH}:${PATH} 
DEB_PREFIX=lophilo-npm- 

EXIST=`ls ${DEB_PREFIX}$1*/`
if [ -z "$EXIST" ]; then 
	./bin/npm2debian -m "Ricky Ng-Adam" -e "rngadam@lophilo.com" -p ${DEB_PREFIX} --debian-build=2 $1
	if [ ! "$?" -eq 0 ]; then
		echo "Error fetching package"
		exit
	fi
fi
EXIST=`ls ${DEB_PREFIX}$1*.deb`
if [ -z "$EXIST" ]; then 
	# dpkg-buildpackage -d to ignore dependencies...
	pushd .
	cd ${DEB_PREFIX}$1*/
	dpkg-buildpackage -aarmel -b -k37FC6E55 -d -rfakeroot
	if [ ! "$?" -eq 0 ]; then
		echo "Error dpkg-buildpackage"
		exit
	fi
	popd
fi
sudo reprepro --basedir /var/www/repos/apt/debian includedeb squeeze `ls ${DEB_PREFIX}$1*.deb`
