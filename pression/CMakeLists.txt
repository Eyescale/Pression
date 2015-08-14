
# Copyright (c) 2012-2014 Stefan Eilemann <eile@eyescale.ch>

include(files.cmake)

list(APPEND PRESSION_LINK_LIBRARIES PUBLIC Lunchbox)

add_definitions(-DEQ_PLUGIN_BUILD -DHAVE_BYTESWAP_H)
if(NOT WIN32 AND NOT CMAKE_COMPILER_IS_XLCXX)
  add_definitions(-DHAVE_BUILTIN_CTZ -DHAVE_BUILTIN_EXPECT)
endif()

common_library(Pression)

source_group(\\ FILES CMakeLists.txt)
source_group(pression FILES ${PRESSION_PUBLIC_HEADERS}
  ${PRESSION_HEADERS} ${PRESSION_SOURCES})
