# ccache configuration; comment out to disable
export CCACHE_TEMPDIR=/tmp
export CCACHE_PREFIX=distcc
export CCACHE_DIR=/gdata/ccachevol
export CCACHE_SLOPPINESS="time_macros file_macro include_file_mtime"
export HOME=/home/rngadam

if [ -z "$CCACHE_DIR" ]; then
	export CXX="${CROSS_COMPILE}g++"
	export CC="${CROSS_COMPILE}gcc"
	echo "# CS compiler, $LOPHILO_TARGET_ARCH, !ccache, !distcc"
else
	export CXX="ccache ${CROSS_COMPILE}g++"
	export CC="ccache ${CROSS_COMPILE}gcc"
	echo "# CS compiler, $LOPHILO_TARGET_ARCH, ccache, distcc"
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
export AS=${CROSS_COMPILE}as
export OBJDUMP=${CROSS_COMPILE}objdump
