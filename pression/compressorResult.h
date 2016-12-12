
/* Copyright (c) 2013-2016, Stefan.Eilemann@epfl.ch
 *
 * This file is part of Pression <https://github.com/Eyescale/Pression>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
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

#ifndef PRESSION_COMPRESSORRESULT_H
#define PRESSION_COMPRESSORRESULT_H

#include <pression/types.h>
#include <pression/plugins/compressorTypes.h> // EQ_COMPRESSOR_INVALID
#include <lunchbox/array.h> // used inline as CompressorChunk

namespace pression
{
/**
 * A structure to hold the results from one compress operation.
 *
 * Valid as long as the associated Compressor is valid and has not been used
 * again.
 */
struct CompressorResult
{
    CompressorResult() : compressor( EQ_COMPRESSOR_INVALID ) {}
    CompressorResult( const unsigned n, const CompressorChunks& c )
        : compressor( n ), chunks( c ) {}

    /** @return the aggregate size of all chunks @version 1.9.1 */
    uint64_t getSize() const
    {
        uint64_t size = 0;
        for( const CompressorChunk& chunk : chunks )
            size += chunk.getNumBytes();
        return size;
    }

    /** @return true if the result contains compressed data. @version 1.9.1 */
    bool isCompressed() const
    {
        return compressor != EQ_COMPRESSOR_INVALID &&
               compressor != EQ_COMPRESSOR_NONE &&
               compressor != EQ_COMPRESSOR_AUTO;
    }

    unsigned compressor;
    CompressorChunks chunks;
};
}
#endif  // PRESSION_COMPRESSORRESULT_H
