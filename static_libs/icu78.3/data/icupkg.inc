GENCCODE_ASSEMBLY_TYPE=-a gcc
SO=so
SOBJ=so
A=a
LIBPREFIX=lib
LIB_EXT_ORDER=.78.3
COMPILE=/opt/ndk/android-ndk-r29/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android21-clang -ffunction-sections -fdata-sections -D_REENTRANT  -DU_HAVE_ELF_H=1 -DU_ENABLE_DYLOAD=0  -DU_ALL_IMPLEMENTATION -DU_ATTRIBUTE_DEPRECATED= -O3 -frtti -fexceptions -fPIC -std=c11   -c
LIBFLAGS=-I/root/as/static_libs/icu78.3/source/./common -I../common -DPIC -fPIC
GENLIB=/opt/ndk/android-ndk-r29/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android21-clang -O3 -frtti -fexceptions -fPIC -std=c11   -Wl,--gc-sections  -shared -Wl,-Bsymbolic
LDICUDTFLAGS=-nodefaultlibs -nostdlib
LD_SONAME=-Wl,-soname -Wl,
RPATH_FLAGS=
BIR_LDFLAGS=-Wl,-Bsymbolic
AR=/opt/ndk/android-ndk-r29/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar
ARFLAGS=r
RANLIB=/opt/ndk/android-ndk-r29/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib
INSTALL_CMD=/usr/bin/install -c
