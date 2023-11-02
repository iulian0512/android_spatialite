#!/bin/bash
SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
export BUILD_UTILS_SH="$SCRIPT_DIR/build_utils.sh"
source "$BUILD_UTILS_SH" || {
  echo "cannot load buildutils"
  exit 1
}

[ -z "$NDK_HOME" ] && exit_with_message "Error: NDK_HOME environment variable is not set."
[ -z "$PROJECT_ROOT" ] && exit_with_message "Error: NDK_HOME environment variable is not set."

#and toolchain file path
export AND_TOOLCHAIN_FILE="$SCRIPT_DIR/andtoolchain.sh"

#android compiler flags
export AND_CFLAGS="-frtti -fexceptions -fPIC"
export AND_CXXFLAGS="-frtti -fexceptions -fPIC"

#general output dirs
APROJ_MAIN_DIR="$PROJECT_ROOT/app/src/main"
export JNI_DIR="$APROJ_MAIN_DIR/jni"

# Run the build scripts in a subshell to avoid env variables pollution

# Directory for GEOS
GEOS_DIR="geos-3.12.0"
#GEOS_INSTALL_DIR needed for rttopo
export GEOS_INSTALL_DIR="$SCRIPT_DIR/$GEOS_DIR/build"

# Directory for proj
PROJ_DIR="proj-9.2.1"
export PROJ_INSTALL_DIR="$SCRIPT_DIR/$PROJ_DIR/build"

# Directory for iconv
ICONV_DIR="libiconv-1.17"
export ICONV_INSTALL_DIR="$SCRIPT_DIR/$ICONV_DIR/build"

# Directory for topo
RTTOPO_DIR="librttopo-1.1.0"
export RTTOPO_INSTALL_DIR="$SCRIPT_DIR/$RTTOPO_DIR/build"

# sqlite
SQLITE_DIR="sqlite-amalgamation-3430200"
#env var needed by proj
export SQLITE_INSTALL_DIR="$SCRIPT_DIR/$SQLITE_DIR"

#icu
ICU_DIR="icu73.2"
#env var needed by sqlite3
export ICU_INSTALL_DIR="$SCRIPT_DIR/$ICU_DIR/build"

# Directory for spatialite
SPATIALITE_DIR="libspatialite-5.1.0"

export API=21
# Array of architectures
archs=("armeabi-v7a" "arm64-v8a" "x86_64")

scripts=(
  "$ICU_DIR/./build.sh"
  "$SQLITE_DIR/./build.sh"
  "$PROJ_DIR/./build.sh"
  "$GEOS_DIR/./build.sh"
  "$ICONV_DIR/./build.sh"
  "$RTTOPO_DIR/./build.sh"
  "$SPATIALITE_DIR/./build.sh"
)

# Loop over each architecture
for arch in "${archs[@]}"; do
  echo "Processing architecture: $arch"

  # Needed by most scripts
  export TARGET_ARCH_ABI="$arch"
  export JNI_LIB_ABI_DIR="$APROJ_MAIN_DIR/jniLibs/$TARGET_ARCH_ABI"
  mkdir -p "$JNI_LIB_ABI_DIR"

  # Loop over each subscript
  for script in "${scripts[@]}"; do
    ($script)
    exit_status=$?
    if [ $exit_status -ne 0 ]; then
      echo "Script '$script' failed with exit status $exit_status, exiting."
      exit 1
    fi
  done
done

