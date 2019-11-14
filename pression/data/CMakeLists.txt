
# Copyright (c) 2016 Stefan.Eilemann@epfl.ch

set(PRESSIONDATA_PUBLIC_HEADERS
  Compressor.h
  CompressorFastLZ.h
  CompressorInfo.h
  CompressorLZF.h
  CompressorRLE.h
  CompressorSnappy.h
  CompressorZSTD.h
  Registry.h
  types.h
)

set(SNAPPY_SOURCES
  snappy/snappy.h
  snappy/snappy.cc
  snappy/snappy-sinksource.cc
)

set(LZF_SOURCES
  liblzf/lzf.h
  liblzf/lzf_c.c
  liblzf/lzf_d.c
)

set(ac_cv_have_stdint_h 1)
set(ac_cv_have_stddef_h 1)
set(ac_cv_have_sys_uio_h 0)
set(SNAPPY_MAJOR 1)
set(SNAPPY_MINOR 1)
set(SNAPPY_PATCHLEVEL 3)
configure_file(snappy/snappy-stubs-public.h.in
  ${PROJECT_BINARY_DIR}/include/snappy-stubs-public.h @ONLY)
if(MSVC)
  file(WRITE ${PROJECT_BINARY_DIR}/include/config.h
    "#pragma once\n\n"
    "#include <BaseTsd.h>\n"
    "typedef SSIZE_T ssize_t;\n\n")
endif()

set(PRESSIONDATA_COMPRESSORS
  CompressorFastLZ.cpp
  CompressorLZF.cpp
  CompressorRLE.cpp
  CompressorSnappy.cpp
  CompressorZSTD.cpp
  fastlz/fastlz.c
  fastlz/fastlz.h
  ${LZF_SOURCES}
  ${SNAPPY_SOURCES}
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

if(CMAKE_COMPILER_IS_GCC OR CMAKE_COMPILER_IS_CLANG)
  set_source_files_properties(${PRESSIONDATA_COMPRESSORS} PROPERTIES COMPILE_FLAGS
    "-Wno-implicit-fallthrough -Wno-unused-parameter -Wno-header-hygiene -Wno-sign-compare")
elseif(MSVC)
  set_source_files_properties(${SNAPPY_SOURCES} PROPERTIES COMPILE_FLAGS
    "/w")
endif()


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

if(MSVC)
  target_compile_definitions(PressionData PRIVATE HAVE_CONFIG_H) # for snappy
endif()
