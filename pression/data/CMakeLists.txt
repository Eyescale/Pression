
# Copyright (c) 2016 Stefan.Eilemann@epfl.ch

set(PRESSIONDATA_PUBLIC_HEADERS
  Compressor.h
  CompressorInfo.h
  Registry.h
  types.h
)

set(PRESSIONDATA_COMPRESSORS
  CompressorFastLZ.cpp
  CompressorFastLZ.h
  CompressorLZF.cpp
  CompressorLZF.h
  CompressorRLE.cpp
  CompressorRLE.h
  CompressorSnappy.cpp
  CompressorSnappy.h
  CompressorZSTD.cpp
  CompressorZSTD.h
  fastlz/fastlz.c
  fastlz/fastlz.h
  liblzf/lzf.h
  liblzf/lzf_c.c
  liblzf/lzf_d.c
  snappy/snappy.h
  snappy/snappy.cc
  snappy/snappy-sinksource.cc
  zstd/lib/common/entropy_common.c
  zstd/lib/common/fse_decompress.c
  zstd/lib/common/xxhash.c
  zstd/lib/common/zstd_common.c
  zstd/lib/compress/fse_compress.c
  zstd/lib/compress/huf_compress.c
  zstd/lib/compress/zbuff_compress.c
  zstd/lib/compress/zstd_compress.c
  zstd/lib/decompress/huf_decompress.c
  zstd/lib/decompress/zbuff_decompress.c
  zstd/lib/decompress/zstd_decompress.c
  zstd/lib/dictBuilder/divsufsort.c
  zstd/lib/dictBuilder/zdict.c
)

set(PRESSIONDATA_SOURCES
  ${PRESSIONDATA_COMPRESSORS}
  Compressor.cpp
  Registry.cpp
)

include_directories(zstd/lib zstd/lib/common)
set(PRESSIONDATA_LINK_LIBRARIES PUBLIC Lunchbox)

add_definitions( -DHAVE_BYTESWAP_H)
if(NOT WIN32 AND NOT CMAKE_COMPILER_IS_XLCXX)
  add_definitions(-DHAVE_BUILTIN_CTZ -DHAVE_BUILTIN_EXPECT)
endif()

set(PRESSIONDATA_INCLUDE_NAME pression/data)
set(PRESSIONDATA_NAMESPACE pressiondata)
common_library(PressionData)
