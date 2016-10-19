
# Copyright (c) 2012-2016 Stefan Eilemann <eile@eyescale.ch>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(Pression VERSION 2.0.0)
set(Pression_VERSION_ABI 3)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)

include(GitExternal)

set(DPUT_HOST "ppa:eilemann/equalizer-dev")

set(PRESSION_DESCRIPTION "Compressor, decompressor, uploader and downloader plugins")
set(PRESSION_MAINTAINER "Stefan Eilemann <eile@eyescale.ch>")
set(PRESSION_LICENSE LGPL)
set(COMMON_PROJECT_DOMAIN ch.eyescale)

include(Common)

set(PRESSION_INCLUDE_NAME pression)

common_find_package(LibJpegTurbo)
common_find_package(Lunchbox REQUIRED)
common_find_package(OpenMP)
common_find_package_post()

set(PRESSION_DEPENDENT_LIBRARIES Lunchbox)

set(PRESSION_DEFINITIONS ${PRESSION_DEFINITIONS}
  -DPRESSION_BUILD_DIR="${CMAKE_BINARY_DIR}/")
add_definitions(${PRESSION_DEFINITIONS})

if(APPLE)
  # WAR needed when CMAKE_OSX_SYSROOT is set to find /usr/include/turbojpeg.h
  include_directories(SYSTEM /usr/local/../include)
endif()

add_subdirectory(pression)
add_subdirectory(tests)
add_subdirectory(doc)

set(DOXYGEN_MAINPAGE_MD README.md)
set(DOXYGEN_EXTRA_INPUT ${PROJECT_SOURCE_DIR}/README.md)
include(DoxygenRule) # must be after all targets
include(CPackConfig)

set(CTEST_CUSTOM_WARNING_EXCEPTION ${CTEST_CUSTOM_WARNING_EXCEPTION}
  # http://public.kitware.com/Bug/view.php?id=10179
  "ld: warning: duplicate dylib")
include(CTest)
