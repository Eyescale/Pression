
/* Copyright (c) 2016, Stefan.Eilemann@epfl.ch
 *
 * This file is part of Pression <https://github.com/Eyescale/Pression>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include <pression/data/api.h>
#include <pression/data/types.h>
#include <lunchbox/buffer.h> // used inline
#include <lunchbox/compiler.h> // LB_UNUSED
#include <lunchbox/debug.h> // LBUNIMPLEMENTED

namespace pression
{
namespace data
{
/** Base interface for lossless CPU compressors of binary data. */
class Compressor
{
public:
    virtual ~Compressor() {} //!< @internal

    typedef lunchbox::Bufferb Result; //!< Single result data buffer
    typedef std::vector< Result > Results; //!< Set of result chunks

    /**
     * Compress the given data and return the result.
     *
     * This default implementation will use getCompressBound(), getChunkSize()
     * and call the protected compress() method to parallelize the compression.
     *
     * @param data pointer to data to compress
     * @param size number of bytes to compress
     * @return the compressed data chunk(s)
     */
    PRESSIONDATA_API virtual const Results& compress( const uint8_t* data,
                                                      size_t size );
    /**
     * Decompress the given data.
     *
     * This default implementation will decompress the given input in parallel,
     * assuming getChunkSize() returns the same value as during the compress()
     * operation.
     *
     * @param input compressed data chunk(s) produced by compress()
     * @param data pointer to pre-allocated memory for the decompressed data
     * @param size decompressed data size
     * @throw std::runtime_error if chunksize does not match input
     */
    PRESSIONDATA_API virtual void decompress( const Results& input,
                                              uint8_t* data, size_t size );

    /** @return the result of the last compress() operation. */
    const Results& getCompressedData() const { return compressed; }

protected:
    Compressor() {}
    Compressor( const Compressor& ) = delete;
    Compressor( Compressor&& ) = delete;
    Compressor& operator = ( const Compressor& ) = delete;
    Compressor& operator = ( Compressor&& ) = delete;

    /** @return an upper bound of the compressed output for a given size. */
    virtual size_t getCompressBound( const size_t size ) const = 0;

    /** @return the optimal chunk size for this compressor */
    virtual size_t getChunkSize() const { return LB_64KB; }

    /**
     * Compress the given chunk.
     *
     * @param data pointer to data to compress
     * @param size number of bytes to compress
     * @param output pre-allocated output chunk
     */
    virtual void compress( const uint8_t* data LB_UNUSED, size_t size LB_UNUSED,
                           Result& output LB_UNUSED ) { LBUNIMPLEMENTED }
    /**
     * Decompress the given chunk.
     *
     * @param input compressed data chunk produced by compress()
     * @param data pointer to pre-allocated memory for the decompressed data
     * @param size decompressed data size
     */
    virtual void decompress( const Result& input LB_UNUSED,
                             uint8_t* data LB_UNUSED,
                             size_t size LB_UNUSED ) { LBUNIMPLEMENTED }

    Results compressed;
};

inline size_t getDataSize( const Compressor::Results& results )
{
    size_t size = 0;
    for( const auto& result : results )
        size += result.getSize();
    return size;
}
}
}
