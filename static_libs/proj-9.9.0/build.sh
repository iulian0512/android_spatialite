#!/bin/bash
source "$BUILD_UTILS_SH" || { echo "cannot load buildutils"; exit 1; }

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
BUILD_DIR="$SCRIPT_DIR/build"

rm -rf "$BUILD_DIR" || exit_with_message "cannot cleanup build dir";
mkdir -p "$BUILD_DIR" || exit_with_message "cannot make dirs";
cd "$BUILD_DIR" || exit_with_message "cannot cd to $BUILD_DIR";
echo "build dir is $BUILD_DIR";

cmake -DBUILD_APPS=OFF \
  -DBUILD_SHARED_LIBS=OFF \
  -DBUILD_TESTING=OFF -DENABLE_CURL=OFF \
  -DENABLE_TIFF=OFF \
  -DPROJ_DB_CACHE_DIR="$SCRIPT_DIR/data" \
  -DCMAKE_INSTALL_PREFIX="$BUILD_DIR" \
  -DCMAKE_SYSTEM_NAME=Android \
  -DCMAKE_SYSTEM_VERSION="$API" \
  -DCMAKE_ANDROID_ARCH_ABI="$TARGET_ARCH_ABI" \
  -DCMAKE_ANDROID_NDK="$NDK_HOME" \
  -DCMAKE_ANDROID_STL_TYPE=c++_static \
  -DCMAKE_C_FLAGS="$AND_CFLAGS" \
  -DCMAKE_CXX_FLAGS="$AND_CXXFLAGS" \
  -DSQLITE3_INCLUDE_DIR="$SQLITE_INSTALL_DIR" \
  -DSQLITE3_LIBRARY="$SQLITE_INSTALL_DIR/libsqlite3.a" \
  .. &&
cmake --build . && make install &&
{ cp -f -v "$BUILD_DIR/lib/"*.a "$JNI_LIB_ABI_DIR" || exit_with_message "cannot cp lib"
cp -f -v -R "$BUILD_DIR"/../include "$JNI_DIR/proj" || exit_with_message "cannot cp include"; }