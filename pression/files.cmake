
# Copyright (c) 2012-2014 Stefan Eilemann <eile@eyescale.ch>

set(PRESSION_PUBLIC_HEADERS
  ${COMMON_INCLUDES}
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
  compressor/compressorFastLZ.cpp
  compressor/compressorFastLZ.h
  compressor/compressorLZF.cpp
  compressor/compressorLZF.h
  compressor/compressorRLE.ipp
  compressor/compressorRLE10A2.cpp
  compressor/compressorRLE10A2.h
  compressor/compressorRLE4B.cpp
  compressor/compressorRLE4B.h
  compressor/compressorRLE4BU.cpp
  compressor/compressorRLE4BU.h
  compressor/compressorRLE4HF.cpp
  compressor/compressorRLE4HF.h
  compressor/compressorRLE565.cpp
  compressor/compressorRLE565.h
  compressor/compressorRLEB.cpp
  compressor/compressorRLEB.h
  compressor/compressorRLEYUV.cpp
  compressor/compressorRLEYUV.h
  compressor/compressorSnappy.cpp
  compressor/compressorSnappy.h
  compressor/fastlz/fastlz.c
  compressor/fastlz/fastlz.h
  compressor/liblzf/lzf.h
  compressor/liblzf/lzf_c.c
  compressor/liblzf/lzf_d.c
  compressor/snappy/snappy.h
  compressor/snappy/snappy.cc
  compressor/snappy/snappy-sinksource.cc
)

if(LibJpegTurbo_FOUND)
  list(APPEND PRESSION_LINK_LIBRARIES ${LibJpegTurbo_LIBRARIES})
  list(APPEND PRESSION_COMPRESSORS
    compressor/compressorTurboJPEG.h compressor/compressorTurboJPEG.cpp)
endif()

set(PRESSION_HEADERS
  compressorInfo.h
  log.h
  pluginInstance.h
)

set(PRESSION_SOURCES
  ${PRESSION_COMPRESSORS}
  ${COMMON_SOURCES}
  compressor.cpp
  decompressor.cpp
  downloader.cpp
  plugin.cpp
  pluginRegistry.cpp
  uploader.cpp
)
