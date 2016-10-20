
# Copyright (c) 2012-2016 Stefan Eilemann <eile@eyescale.ch>

set(PRESSION_PUBLIC_HEADERS
  compressor.h
  compressorResult.h
  dataCompressor.h
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
  compressor/compressorZSTD.cpp
  compressor/compressorZSTD.h
  compressor/fastlz/fastlz.c
  compressor/fastlz/fastlz.h
  compressor/liblzf/lzf.h
  compressor/liblzf/lzf_c.c
  compressor/liblzf/lzf_d.c
  compressor/snappy/snappy.h
  compressor/snappy/snappy.cc
  compressor/snappy/snappy-sinksource.cc
  compressor/zstd/lib/common/entropy_common.c
  compressor/zstd/lib/common/fse_decompress.c
  compressor/zstd/lib/common/xxhash.c
  compressor/zstd/lib/common/zstd_common.c
  compressor/zstd/lib/compress/fse_compress.c
  compressor/zstd/lib/compress/huf_compress.c
  compressor/zstd/lib/compress/zbuff_compress.c
  compressor/zstd/lib/compress/zstd_compress.c
  compressor/zstd/lib/decompress/huf_decompress.c
  compressor/zstd/lib/decompress/zbuff_decompress.c
  compressor/zstd/lib/decompress/zstd_decompress.c
  compressor/zstd/lib/dictBuilder/divsufsort.c
  compressor/zstd/lib/dictBuilder/zdict.c
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

include_directories(compressor/zstd/lib compressor/zstd/lib/common)
set(PRESSION_LINK_LIBRARIES PUBLIC Lunchbox)

if(LibJpegTurbo_FOUND)
  list(APPEND PRESSION_LINK_LIBRARIES PRIVATE ${LibJpegTurbo_LIBRARIES})
  list(APPEND PRESSION_COMPRESSORS
    compressor/compressorTurboJPEG.h compressor/compressorTurboJPEG.cpp)
endif()


add_definitions(-DEQ_PLUGIN_BUILD -DHAVE_BYTESWAP_H)
if(NOT WIN32 AND NOT CMAKE_COMPILER_IS_XLCXX)
  add_definitions(-DHAVE_BUILTIN_CTZ -DHAVE_BUILTIN_EXPECT)
endif()

common_library(Pression)
