
# Copyright (c) 2012-2015 Stefan Eilemann <eile@eyescale.ch>

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Pression)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)

include(GitExternal)

set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION_ABI 1)

set(GITTARGETS_RELEASE_BRANCH minor)
set(DPUT_HOST "ppa:eilemann/equalizer-dev")

set(PRESSION_DESCRIPTION "Compressor, decompressor, uploader and downloader plugins")
set(COMMON_PROJECT_DOMAIN ch.eyescale)

include(Common)

list(APPEND CPPCHECK_EXTRA_ARGS -DLB_GCC_4_1_OR_LATER=1)

set(PROJECT_INCLUDE_NAME pression)
include(FindPackages)

if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

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

include(DoxygenRule) # must be after all targets
include(PressionCPack)
include(PackageConfig)

set(CTEST_CUSTOM_WARNING_EXCEPTION ${CTEST_CUSTOM_WARNING_EXCEPTION}
  # http://public.kitware.com/Bug/view.php?id=10179
  "ld: warning: duplicate dylib")
include(CTest)
