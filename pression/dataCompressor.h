
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

#ifndef PRESSION_DATACOMPRESSOR_H
#define PRESSION_DATACOMPRESSOR_H

#include <pression/api.h>
#include <pression/types.h>

namespace pression
{

/** Base interface for lossless CPU compressors of binary data. */
class DataCompressor
{
public:
    /** @internal Needed by the lunchbox plugin classes. */
    typedef DataCompressor InterfaceT;

    /** @internal Needed by the lunchbox plugin classes. */
    typedef std::string InitDataT;

    virtual ~DataCompressor() {} //!< @internal

    typedef lunchbox::Bufferb Result; //!< Single result data buffer
    typedef std::vector< Result > Results; //!< Set of result chunks

    /**
     * Compress the given data and return the result.
     *
     * @param data pointer to data to compress
     * @param size number of bytes to compress
     *
     * @return the compressed data chunk(s)
     */
    virtual const Results& compress( const uint8_t* data, size_t size ) = 0;
    /**
     * Decompress the given data.
     *
     * @param input compressed data chunk(s) produced by compress()
     * @param data pointer to pre-allocated memory for the decompressed data
     * @param size decompressed data size
     */
    virtual void decompress( const Results& input, uint8_t* data,
                             size_t size ) = 0;
protected:
    DataCompressor() {}
    DataCompressor( const DataCompressor& ) = delete;
    DataCompressor( DataCompressor&& ) = delete;
    DataCompressor& operator = ( const DataCompressor& ) = delete;
    DataCompressor& operator = ( DataCompressor&& ) = delete;

    Results compressed;
};
}

#endif
