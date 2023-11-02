cmake_minimum_required(VERSION 3.22)
project(android_spatialite)

# Print the directory of the top-level CMakeLists.txt
message("Top-level source directory: ${CMAKE_SOURCE_DIR}")
# Print the directory of the currently processed CMakeLists.txt
message("Current source directory: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
include_directories("${CMAKE_SOURCE_DIR}/src/include")
add_library(android_spatialite SHARED ${SOURCES})

set(LIB_NAMES "spatialite" "sqlite3" "geos" "geos_c" "iconv" "rttopo" "proj" "icuuc" "icui18n" "icudata")
# Loop through the list to define and set properties for each library
foreach(LIB_NAME IN LISTS LIB_NAMES)
    set(LIB_PATH ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/lib${LIB_NAME}.a)
    if(EXISTS ${LIB_PATH})
        add_library(${LIB_NAME} STATIC IMPORTED)
        set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION ${LIB_PATH})
        set(INC_DIR "${CMAKE_SOURCE_DIR}/${LIB_NAME}/include")
        if(EXISTS ${INC_DIR})
            include_directories(${INC_DIR})
        endif()
    endif()
endforeach()




#loglib
find_library(log-lib log)
#zlib
find_package(ZLIB REQUIRED)

add_compile_options(-frtti -fexceptions -fPIC)
target_link_libraries(android_spatialite ${LIB_NAMES} log ZLIB::ZLIB)


