export ARCH=arm
# FriendlyARM
export PREFIX=/home/rngadam/friendlyarm/opt/FriendlyARM/toolschain/4.4.3/bin/arm-none-linux-gnueabi-
# Ubuntu
#export PREFIX=arm-linux-gnueabi-
export PLATFORM_FLAGS="-msoft-float -D__GCC_FLOAT_NOT_NEEDED  -march=armv4t -mtune=arm920t"
export CXX=${PREFIX}g++
export CXXFLAGS="$PLATFORM_FLAGS --sysroot=/home/rngadam/friendlyarm/opt/FriendlyARM/toolschain/4.4.3/arm-none-linux-gnueabi/sys-root" 
export CFLAGS="$CXXFLAGS"
export CPPFLAGS="$CXXFLAGS"
export CC=${PREFIX}gcc
export LD=${PREFIX}ld
