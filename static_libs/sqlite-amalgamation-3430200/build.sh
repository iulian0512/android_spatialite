#!/bin/bash
source "$BUILD_UTILS_SH" || { echo "cannot load buildutils"; exit 1; }
source "$AND_TOOLCHAIN_FILE" ||  exit_with_message "cannot load andtoolchain";

[ -z "$ICU_INSTALL_DIR" ] && exit_with_message "Error: ICU_INSTALL_DIR environment variable is not set."
[ -z "$TARGET_ARCH_ABI" ] && exit_with_message "Error: TARGET_ARCH_ABI environment variable is not set."

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
cd "$SCRIPT_DIR" || exit_with_message "cannot cd to $SCRIPT_DIR";

SQLITE_FLAGS="-DNDEBUG=1 \
-DHAVE_USLEEP=1 \
-DSQLITE_HAVE_ISNAN \
-DSQLITE_DEFAULT_JOURNAL_SIZE_LIMIT=1048576 \
-DSQLITE_THREADSAFE=2 \
-DSQLITE_TEMP_STORE=3 \
-DSQLITE_POWERSAFE_OVERWRITE=1 \
-DSQLITE_DEFAULT_FILE_FORMAT=4 \
-DSQLITE_DEFAULT_AUTOVACUUM=1 \
-DSQLITE_ENABLE_MEMORY_MANAGEMENT=1 \
-DSQLITE_ENABLE_FTS3 \
-DSQLITE_ENABLE_FTS4 \
-DSQLITE_ENABLE_JSON1 \
-DSQLITE_OMIT_BUILTIN_TEST \
-DSQLITE_OMIT_COMPILEOPTION_DIAGS \
-DSQLITE_DEFAULT_FILE_PERMISSIONS=0600 \
-DSQLITE_ENABLE_RTREE \
-DSQLITE_ENABLE_ICU \
-DSQLITE_ENABLE_LOAD_EXTENSION" \
ICU_INCLUDE_DIR="$ICU_INSTALL_DIR/android/$TARGET_ARCH_ABI/include"
OUT_LIB_FILENAME="libsqlite3.a"
$CC -c $CFLAGS $SQLITE_FLAGS sqlite3.c "-I$ICU_INCLUDE_DIR" -o "$OUT_LIB_FILENAME" &&
{ cp -f -v "$OUT_LIB_FILENAME" "$JNI_LIB_ABI_DIR/" || exit_with_message "cannot cp lib" &&
cp -f -v ./*.h "$JNI_DIR/sqlite3/include" || exit_with_message "cannot cp includes"; }


