#!/bin/bash
source "$BUILD_UTILS_SH" || { echo "cannot load buildutils"; exit 1; }
#dirs
SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
BUILD_DIR="$SCRIPT_DIR/build"
ICU_SRC_DIR="$SCRIPT_DIR/source"

#cd to scriptdir
cd "$SCRIPT_DIR" ||  exit_with_message "cannot cd to $SCRIPT_DIR";

# Check for required environment variables
[ -z "$NDK_HOME" ] && exit_with_message "Error: NDK_HOME environment variable is not set."
[ -z "$TARGET_ARCH_ABI" ] && exit_with_message "Error: TARGET_ARCH_ABI environment variable is not set."
echo "NDK_HOME is defined as $NDK_HOME."
echo "TARGET_ARCH_ABI is defined as $TARGET_ARCH_ABI."

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
    exit_with_message "Unsupported TARGET_ARCH_ABI: '$TARGET_ARCH_ABI'"
    ;;
esac


# Initialize the OS variable
OS="Unknown"

# Determine the OS type
UNAME_S=$(uname -s)
if [ "$UNAME_S" == "Darwin" ]; then
    OS="MacOSX"
elif [ "$UNAME_S" == "Linux" ]; then
    OS="Linux"
fi

# Check for unsupported OS
if [[ "$OS" != "MacOSX" && "$OS" != "Linux" ]]; then
   exit_with_message "Unsupported operating system $OS"
fi

# Setup directories
ICU_INTERMEDIATE_OS_BUILD_DIR="$BUILD_DIR/intermediate_$UNAME_S"
ICU_AND_OUT_DIR="$BUILD_DIR/android"
ICU_AND_ARCH_BUILD_DIR="$ICU_AND_OUT_DIR/$TARGET_ARCH_ABI" 

# Handle 'clean' parameter
if [ "$1" == "clean" ]; then
  echo "Performing cleanup..."
  rm -rf "$ICU_INTERMEDIATE_OS_BUILD_DIR" || exit_with_message "Cannot delete $ICU_INTERMEDIATE_OS_BUILD_DIR"
  rm -rf "$ICU_AND_OUT_DIR" || exit_with_message "Cannot delete $ICU_AND_OUT_DIR"
  exit 0
fi

# Build the intermediate build if neded 
if [[ ! -d "$ICU_INTERMEDIATE_OS_BUILD_DIR" ]]; then
  echo "creating intermediate build in $ICU_INTERMEDIATE_OS_BUILD_DIR"
    mkdir -p "$ICU_INTERMEDIATE_OS_BUILD_DIR"
    cd "$ICU_INTERMEDIATE_OS_BUILD_DIR" || exit_with_message "cannot cd to $ICU_INTERMEDIATE_OS_BUILD_DIR"
    "${ICU_SRC_DIR}/./runConfigureICU" "$OS" \
        --prefix="$ICU_INTERMEDIATE_OS_BUILD_DIR" \
        --enable-static \
        --enable-shared=no \
        --enable-extras=no \
        --enable-strict=no \
        --enable-icuio=no \
        --enable-layout=no \
        --enable-layoutex=no \
        --enable-tools=yes \
        --enable-tests=no \
        --enable-samples=no \
        --enable-dyload=no &&
        make || exit_with_message "could not create intermediate build"

else 
  echo "skipping intermediate build because $ICU_INTERMEDIATE_OS_BUILD_DIR exists"
fi



#build android dedicated libs

# Cleanup first
rm -rf "$ICU_AND_ARCH_BUILD_DIR" || { exit_with_message "Cannot delete $$ICU_AND_ARCH_BUILD_DIR"; }
#Create arch dir 
mkdir -p "$ICU_AND_ARCH_BUILD_DIR" || cd "$ICU_AND_ARCH_BUILD_DIR" || exit_with_message "cannot cd to $ICU_AND_ARCH_BUILD_DIR";

source "$AND_TOOLCHAIN_FILE" || exit_with_message "cannot load andtoolchain";

#build
echo "building android icu $TARGET"
 "${ICU_SRC_DIR}/./runConfigureICU" Linux \
  --host="$TARGET" \
  --prefix="$ICU_AND_ARCH_BUILD_DIR" \
  --with-cross-build="$ICU_INTERMEDIATE_OS_BUILD_DIR" \
  --enable-static \
  --enable-shared=no \
  --enable-extras=no \
  --enable-strict=no \
  --enable-icuio=no \
  --enable-layout=no \
  --enable-layoutex=no \
  --enable-tools=no \
  --enable-tests=no \
  --enable-samples=no \
  --enable-dyload=no \
  --with-data-packaging=archive \
 && make clean && make && make install &&
{ cp -f -v "$ICU_AND_ARCH_BUILD_DIR/lib/"*.a "$JNI_LIB_ABI_DIR" || exit_with_message "cannot cp lib" &&
cp -f -v -R "$ICU_AND_ARCH_BUILD_DIR"/include "$JNI_DIR/icu" || exit_with_message "cannot cp include"; }



