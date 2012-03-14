#!/bin/sh
ARM_TOOLCHAIN=${HOME}/lophilo/codesourcery/arm926ej-s

export EMAIL="rngadam@lophilo.com"
export DEBFULLNAME="Ricky Ng-Adam"
export PATH=${ARM_TOOLCHAIN_PATH}:${PATH} 

exist=`ls npm-$1*`
if [ -z "$exist" ]; then 
	./bin/npm2debian --versioned $1
	if [ ! "$?" -eq 0 ]; then
		echo "Error fetching package"
		exit
	fi
fi
# dpkg-buildpackage -d to ignore dependencies...
cd npm-$1*/
dpkg-buildpackage -aarmel -b -k37FC6E55 -d -rfakeroot
if [ ! "$?" -eq 0 ]; then
	echo "Error dpkg-buildpackage"
	exit
fi
