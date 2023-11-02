#!/bin/bash
{ source "$BUILD_UTILS_SH" || source "build_utils.sh"; } || { echo "cannot load buildutils"; exit 1; }

# Check if the first parameter is provided. If yes, set TARGET_ARCH_ABI to its value
if [ ! -z "$1" ]; then
  TARGET_ARCH_ABI="$1"
fi

# Now check if TARGET_ARCH_ABI is still empty
[ -z "$TARGET_ARCH_ABI" ] && exit_with_message "Error: TARGET_ARCH_ABI environment variable is not set."

# Check if the API environment variable is set. If not, set it to 21
if [ -z "$API" ]; then
  API=21
fi

# Print the API level for debugging (optional)
echo "API level is set to $API"

[ -z "$AND_CFLAGS" ] && exit_with_message "Error: AND_CFLAGS environment variable is not set."
[ -z "$AND_CXXFLAGS" ] && exit_with_message "Error: AND_CXXFLAGS environment variable is not set."


UNAME_S=$(uname -s | tr '[:upper:]' '[:lower:]');

# Set TARGET based on TARGET_ARCH_ABI
case $TARGET_ARCH_ABI in
  "armeabi-v7a")
    TARGET="armv7a-linux-androideabi"
    ;;
  "arm64-v8a")
    TARGET="aarch64-linux-android"
    ;;
  "x86")
    TARGET="i686-linux-android"
    ;;
  "x86_64")
    TARGET="x86_64-linux-android"
    ;;
  *)
    exit_with_message "Unsupported TARGET_ARCH_ABI: $TARGET_ARCH_ABI"
    ;;
esac


export TOOLCHAIN="$NDK_HOME/toolchains/llvm/prebuilt/$UNAME_S-x86_64"
export AR="$TOOLCHAIN/bin/llvm-ar"
export CC="$TOOLCHAIN/bin/$TARGET$API-clang"
export AS="$CC"
export CXX="$TOOLCHAIN/bin/$TARGET$API-clang++"
export LD="$TOOLCHAIN/bin/ld"
export RANLIB="$TOOLCHAIN/bin/llvm-ranlib"
export STRIP="$TOOLCHAIN/bin/llvm-strip"
export CFLAGS=$AND_CFLAGS
export CXXFLAGS=$AND_CXXFLAGS
alias readelf="$TOOLCHAIN/bin/llvm-readelf"
alias nm="$TOOLCHAIN/bin/llvm-nm"
