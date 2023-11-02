GENCCODE_ASSEMBLY_TYPE=-a gcc
SO=so
SOBJ=so
A=a
LIBPREFIX=lib
LIB_EXT_ORDER=.73.2
COMPILE=/home/iulian/Android/Sdk/ndk/23.1.7779620/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android21-clang -ffunction-sections -fdata-sections -D_REENTRANT  -DU_HAVE_ELF_H=1 -DU_ENABLE_DYLOAD=0 -DU_HAVE_STRTOD_L=0 -DU_HAVE_STRING_VIEW=1  -DU_ATTRIBUTE_DEPRECATED= -O3 -frtti -fexceptions -fPIC   -c
LIBFLAGS=-I/home/iulian/github/android_spatialite/static_libs/icu73.2/build/android/x86_64/include -DPIC -fPIC
GENLIB=/home/iulian/Android/Sdk/ndk/23.1.7779620/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android21-clang -O3 -frtti -fexceptions -fPIC   -Wl,--gc-sections  -shared -Wl,-Bsymbolic
LDICUDTFLAGS=-nodefaultlibs -nostdlib
LD_SONAME=-Wl,-soname -Wl,
RPATH_FLAGS=
BIR_LDFLAGS=-Wl,-Bsymbolic
AR=/home/iulian/Android/Sdk/ndk/23.1.7779620/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar
ARFLAGS=r
RANLIB=/home/iulian/Android/Sdk/ndk/23.1.7779620/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib
INSTALL_CMD=/usr/bin/install -c
