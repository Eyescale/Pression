
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

Compressor::~Compressor()
{
    if( _in > 0 )
        LBDEBUG << _in << " -> " << _out << " ("
                << int( float(_out) / float(_in) * 100.f ) << "%) " <<std::endl;
}

const Compressor::Results& Compressor::compress( const uint8_t* data,
                                                 size_t size )
{
#ifdef PRESSION_USE_OPENMP
    const size_t chunkSize = getChunkSize();
    const size_t nChunks = (size + chunkSize - 1) / chunkSize;

    compressed.resize( nChunks );

#pragma omp parallel for
    for( size_t i = 0; i < nChunks; ++i )
    {
        const size_t start = i * chunkSize;
        const size_t end = std::min( (i+1) * chunkSize, size );
        const size_t nBytes = end - start;

        compressed[i].reserve( getCompressBound( nBytes ));
        compressChunk( data + start, nBytes, compressed[i] );
    }
#else
    compressed.resize( 1 );
    compressed[0].reserve( getCompressBound( size ));
    compressChunk( data, size, compressed[0] );
#endif
    _in += size;
    _out += getDataSize( compressed );
    return compressed;
}

void Compressor::decompress( const Results& result, uint8_t* data, size_t size )
{
    if( result.empty( ))
        return;

    std::vector< std::pair< const uint8_t*, size_t >> inputs( result.size( ));
    for( size_t i = 0; i < result.size(); ++i )
        inputs[i] = { result[i].getData(), result[i].getSize() };
    decompress( inputs, data, size );
}


void Compressor::decompress(
    const std::vector< std::pair< const uint8_t*, size_t >>& inputs,
    uint8_t* data, size_t size )
{
    if( inputs.empty( ))
        return;

    if( inputs.size() == 1 ) // compressor did not have OpenMP
    {
        decompressChunk( inputs[0].first, inputs[0].second, data, size );
        return;
    }

    const size_t chunkSize = getChunkSize();
    if( size/chunkSize != inputs.size() && size/chunkSize + 1 != inputs.size( ))
    {
        LBTHROW(
            std::runtime_error( "Chunk size of " + std::to_string( chunkSize ) +
                                " not consistent with " +
                                std::to_string( inputs.size( )) +
                                " input chunks for " + std::to_string( size ) +
                                " bytes" ));
    }

#pragma omp parallel for
    for( size_t i = 0; i < inputs.size(); ++i )
    {
        const size_t start = i * chunkSize;
        const size_t end = std::min( (i+1) * chunkSize, size );
        const size_t nBytes = end - start;

        decompressChunk( inputs[i].first, inputs[i].second,
                         data + start, nBytes );
    }
}
}
}
