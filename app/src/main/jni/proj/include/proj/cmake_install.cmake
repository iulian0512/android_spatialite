# Install script for directory: /Users/ics/temp/spatialite5.1.0_build/proj-9.2.1/include/proj

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/ics/temp/spatialite5.1.0_build/proj-9.2.1/build")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Users/ics/Library/Android/sdk/ndk/23.1.7779620/toolchains/llvm/prebuilt/darwin-x86_64/bin/llvm-objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/proj" TYPE FILE FILES
    "/Users/ics/temp/spatialite5.1.0_build/proj-9.2.1/include/proj/util.hpp"
    "/Users/ics/temp/spatialite5.1.0_build/proj-9.2.1/include/proj/metadata.hpp"
    "/Users/ics/temp/spatialite5.1.0_build/proj-9.2.1/include/proj/common.hpp"
    "/Users/ics/temp/spatialite5.1.0_build/proj-9.2.1/include/proj/coordinates.hpp"
    "/Users/ics/temp/spatialite5.1.0_build/proj-9.2.1/include/proj/crs.hpp"
    "/Users/ics/temp/spatialite5.1.0_build/proj-9.2.1/include/proj/datum.hpp"
    "/Users/ics/temp/spatialite5.1.0_build/proj-9.2.1/include/proj/coordinatesystem.hpp"
    "/Users/ics/temp/spatialite5.1.0_build/proj-9.2.1/include/proj/coordinateoperation.hpp"
    "/Users/ics/temp/spatialite5.1.0_build/proj-9.2.1/include/proj/io.hpp"
    "/Users/ics/temp/spatialite5.1.0_build/proj-9.2.1/include/proj/nn.hpp"
    )
endif()

