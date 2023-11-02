#!/bin/bash
source "$BUILD_UTILS_SH" || { echo "cannot load buildutils"; exit 1; }
source "$AND_TOOLCHAIN_FILE" ||  exit_with_message "cannot load andtoolchain";

[ -z "$TARGET_ARCH_ABI" ] && exit_with_message "Error: TARGET_ARCH_ABI environment variable is not set."

#GEOS
[ -z "$GEOS_INSTALL_DIR" ] && exit_with_message "Error: GEOS_INSTALL_DIR environment variable is not set."
GEOS_INCLUDE_DIR="$GEOS_INSTALL_DIR/include"
GEOS_LIB_DIR="$GEOS_INSTALL_DIR/lib"
GEOS_BIN_DIR="$GEOS_INSTALL_DIR/bin"

#iconv
[ -z "$ICONV_INSTALL_DIR" ] && exit_with_message "Error: ICONV_INSTALL_DIR environment variable is not set."
ICONV_INCLUDE_DIR="$ICONV_INSTALL_DIR/include"
ICONV_LIB_DIR="$ICONV_INSTALL_DIR/lib"
ICONV_BIN_DIR="$ICONV_INSTALL_DIR/bin"

#rttopo
[ -z "$RTTOPO_INSTALL_DIR" ] && exit_with_message "Error: RTTOPO_INSTALL_DIR environment variable is not set."
RTTOPO_INCLUDE_DIR="$RTTOPO_INSTALL_DIR/include"
RTTOPO_LIB_DIR="$RTTOPO_INSTALL_DIR/lib"

#proj
[ -z "$PROJ_INSTALL_DIR" ] && exit_with_message "Error: PROJ_INSTALL_DIR environment variable is not set."
PROJ_INCLUDE_DIR="$PROJ_INSTALL_DIR/include"
PROJ_LIB_DIR="$PROJ_INSTALL_DIR/lib"

#sqlite
[ -z "$SQLITE_INSTALL_DIR" ] && exit_with_message "Error: SQLITE_INSTALL_DIR environment variable is not set."
SQLITE_INCLUDE_DIR=$SQLITE_INSTALL_DIR
SQLITE_LIB_DIR=$SQLITE_INSTALL_DIR

#icu
[ -z "$ICU_INSTALL_DIR" ] && exit_with_message "Error: ICU_INSTALL_DIR environment variable is not set."
ICU_INSTALL_DIR+="/android/$TARGET_ARCH_ABI"
ICU_INCLUDE_DIR="$ICU_INSTALL_DIR/include"
ICU_LIB_DIR="$ICU_INSTALL_DIR/lib"

#merge Include dirs
INCLUDE_DIRS=($GEOS_INCLUDE_DIR $ICONV_INCLUDE_DIR $RTTOPO_INCLUDE_DIR $PROJ_INCLUDE_DIR $SQLITE_INCLUDE_DIR $ICU_INCLUDE_DIR)
INCLUDE_DIRS=$(printf " -I%s" "${INCLUDE_DIRS[@]}")

#merge Lib dirs
LIB_DIRS=($GEOS_LIB_DIR $ICONV_LIB_DIR $RTTOPO_LIB_DIR $PROJ_LIB_DIR $SQLITE_LIB_DIR $ICU_LIB_DIR)
LIB_DIRS=$(printf " -L%s" "${LIB_DIRS[@]}")

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
BUILD_DIR="$SCRIPT_DIR/build"

cd "$SCRIPT_DIR" ||  exit_with_message "cannot cd to $SCRIPT_DIR";
rm -rf "$BUILD_DIR" || exit_with_message "cannot cleanup build dir";
mkdir -pv "$BUILD_DIR"  || exit_with_message "cannot make dirs";

LDFLAGS="$LIB_DIRS -lgeos -liconv -lrttopo -lproj -lsqlite3 -licudata -licui18n -licuuc -lstdc++ -lm" \
CFLAGS="$INCLUDE_DIRS $CFLAGS" \
./configure \
    --prefix="$BUILD_DIR" \
    --host=arm-64-linux \
    --disable-freexl \
    --disable-minizip \
    --disable-gcov \
    --disable-examples \
    --disable-libxml2 \
    --disable-examples \
    --disable-shared \
    --with-geosconfig="$GEOS_BIN_DIR/geos-config" \
    && make clean && make && make install &&
{ cp -f -v "$BUILD_DIR/lib/"*.a "$JNI_LIB_ABI_DIR" || exit_with_message "cannot cp lib" &&
cp -f -v -R "$BUILD_DIR"/include "$JNI_DIR/spatialite" || exit_with_message "cannot cp include"; }

