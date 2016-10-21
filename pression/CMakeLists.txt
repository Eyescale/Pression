
# Copyright (c) 2012-2016 Stefan Eilemann <eile@eyescale.ch>

set(PRESSION_PUBLIC_HEADERS
  compressor.h
  compressorResult.h
  decompressor.h
  downloader.h
  log.h
  plugin.h
  pluginRegistry.h
  pluginVisitor.h
  plugins/compressor.h
  plugins/compressorTokens.h
  plugins/compressorTypes.h
  types.h
  uploader.h
)

set(PRESSION_COMPRESSORS
  compressor/compressor.cpp
  compressor/compressor.h
  compressor/compressorRLE.ipp
  compressor/compressorRLE10A2.cpp
  compressor/compressorRLE10A2.h
  compressor/compressorRLE4B.cpp
  compressor/compressorRLE4B.h
  compressor/compressorRLE4HF.cpp
  compressor/compressorRLE4HF.h
  compressor/compressorRLE565.cpp
  compressor/compressorRLE565.h
  compressor/compressorRLEYUV.cpp
  compressor/compressorRLEYUV.h
)

set(PRESSION_HEADERS
  compressorInfo.h
  log.h
  pluginInstance.h
)

set(PRESSION_SOURCES
  ${PRESSION_COMPRESSORS}
  compressor.cpp
  decompressor.cpp
  downloader.cpp
  plugin.cpp
  pluginRegistry.cpp
  uploader.cpp
)

set(PRESSION_LINK_LIBRARIES PUBLIC Lunchbox)

add_definitions(-DEQ_PLUGIN_BUILD -DHAVE_BYTESWAP_H)
if(NOT WIN32 AND NOT CMAKE_COMPILER_IS_XLCXX)
  add_definitions(-DHAVE_BUILTIN_CTZ -DHAVE_BUILTIN_EXPECT)
endif()

common_library(Pression)
add_subdirectory(data)
