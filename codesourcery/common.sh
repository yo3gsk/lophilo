#export CCACHE_TEMPDIR=/tmp
#export CCACHE_CROSS_COMPILE=distcc
#export CCACHE_DIR=/gdata/ccachevol
#export CCACHE_SLOPPINESS="time_macros file_macro include_file_mtime"
#export CROSS_COMPILE=/usr/bin/
export PLATFORM_FLAGS="-mlittle-endian -mno-thumb-interwork -mabi=aapcs-linux -mfloat-abi=soft -march=armv5te -mtune=arm926ej-s -msoft-float -Uarm"
export CROSS_COMPILE="${HOME}/upstream/arm-2011.09/bin/arm-none-linux-gnueabi-"
#export CXX="ccache ${CROSS_COMPILE}g++"
export CXX="${CROSS_COMPILE}g++"
export CXXFLAGS="$PLATFORM_FLAGS" 
export CFLAGS="$CXXFLAGS"
export CPPFLAGS="$CXXFLAGS"
#export CC="ccache ${CROSS_COMPILE}gcc"
export CC="${CROSS_COMPILE}gcc"
export LD=${CROSS_COMPILE}ld
export STRIP=${CROSS_COMPILE}strip
export AR=${CROSS_COMPILE}ar
