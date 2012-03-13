# ccache configuration; comment out to disable
export CCACHE_TEMPDIR=/tmp
export CCACHE_CROSS_COMPILE=distcc
export CCACHE_DIR=/gdata/ccachevol
export CCACHE_SLOPPINESS="time_macros file_macro include_file_mtime"

export PLATFORM_FLAGS="-mlittle-endian -mno-thumb-interwork -mabi=aapcs-linux -mfloat-abi=soft -march=armv5te -mtune=arm926ej-s -msoft-float -Uarm"
export CROSS_COMPILE="/opt/codesourcery/arm-2011.09/bin/arm-none-linux-gnueabi-"

if [ -z "CCACHE_DIR" ]; then
	export CXX="${CROSS_COMPILE}g++"
	export CC="${CROSS_COMPILE}gcc"
else
	export CXX="ccache ${CROSS_COMPILE}g++"
	export CC="ccache ${CROSS_COMPILE}gcc"
fi

# compile flags for target
export CXXFLAGS="$PLATFORM_FLAGS" 
export CFLAGS="$PLATFORM_FLAGS"
export CPPFLAGS="$PLATFORM_FLAGS"

# other executables
export LD=${CROSS_COMPILE}ld
export STRIP=${CROSS_COMPILE}strip
export AR=${CROSS_COMPILE}ar
export RANLIB=${CROSS_COMPILE}ranlib
