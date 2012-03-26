export LOPHILO_TARGET_ARCH=arm
export PLATFORM_FLAGS="-mlittle-endian -mno-thumb-interwork -mabi=aapcs-linux -mfloat-abi=soft -march=armv5te -mtune=arm926ej-s -msoft-float -Uarm"
export CROSS_COMPILE="/opt/codesourcery/arm-2011.09/bin/arm-none-linux-gnueabi-"

. `dirname $0`/../common.sh  

export ASFLAGS="-mlittle-endian -mno-thumb-interwork -mfloat-abi=soft -march=armv5te -mcpu=arm926ej-s"
