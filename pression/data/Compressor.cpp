
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

#include "Compressor.h"

#include <stdexcept>

namespace pression
{
namespace data
{
const Compressor::Results& Compressor::compress( const uint8_t* data,
                                                 size_t size )
{
#ifdef PRESSION_USE_OPENMP
    const size_t chunkSize = getChunkSize();
    size_t nChunks = size / chunkSize;

    if( nChunks * chunkSize < size )
        ++nChunks;
    compressed.resize( nChunks );

#pragma omp parallel for
    for( size_t i = 0; i < nChunks; ++i )
    {
        const size_t start = i * chunkSize;
        const size_t end = std::min( (i+1) * chunkSize, size );
        const size_t nBytes = end - start;

        compressed[i].reserve( getCompressBound( nBytes ));
        compress( data + start, nBytes, compressed[i] );
    }
#else
    compressed.resize( 1 );
    compressed[0].reserve( getCompressBound( size ));
    compress( data, size, compressed[0] );
#endif
    return compressed;
}

void Compressor::decompress( const Results& input, uint8_t* data,
                                 size_t size )
{
    if( input.empty( ))
        return;
    if( input.size() == 1 ) // compressor did not have OpenMP
    {
        decompress( input[0], data, size );
        return;
    }

    const size_t chunkSize = getChunkSize();
    if( size/chunkSize != input.size() && size/chunkSize + 1 != input.size() )
    {
        LBTHROW(
            std::runtime_error( "Chunk size of " + std::to_string( chunkSize ) +
                                " not consistent with " +
                                std::to_string( input.size( )) +
                                " input chunks for " + std::to_string( size ) +
                                " bytes" ));
    }

#pragma omp parallel for
    for( size_t i = 0; i < input.size(); ++i )
    {
        const size_t start = i * chunkSize;
        const size_t end = std::min( (i+1) * chunkSize, size );
        const size_t nBytes = end - start;

        decompress( input[i], data + start, nBytes );
    }
}
}
}
