export CCACHE_TEMPDIR=/tmp
export CCACHE_PREFIX=distcc
export CCACHE_DIR=/gdata/ccachevol
export CCACHE_SLOPPINESS="time_macros file_macro include_file_mtime"
export PREFIX=/usr/bin/

#export CXX="ccache ${PREFIX}g++"
export CXX="ccache ${PREFIX}clang -Qunused-arguments"
export CXXFLAGS="$PLATFORM_FLAGS" 
export CFLAGS="$CXXFLAGS"
export CPPFLAGS="$CXXFLAGS"
#export CC="ccache ${PREFIX}gcc"
export CC="ccache ${PREFIX}clang -mcpu=i686 -Qunused-arguments"
export LD=${PREFIX}ld
export STRIP=${PREFIX}strip
