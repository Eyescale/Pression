
/* Copyright (c) 2010-2016, Cedric Stalder <cedric.stalder@gmail.com>
 *                          Stefan Eilemann <eile@eyescale.ch>
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

#include "CompressorRLE.h"

#include <pression/data/Registry.h>
#include <lunchbox/buffer.h>
#include <limits>

namespace
{
const uint8_t _rleMarker = 0x42; // just a random number
}
#pragma clang diagnostic ignored "-Wunneeded-internal-declaration"
#include "../compressor/compressorRLE.ipp"

namespace pression
{
namespace data
{
namespace
{
const bool _initialized =
    Registry::getInstance().registerEngine< CompressorRLE >(
        { "pression::data::CompressorRLE", .97f, 1.f });

template< typename T >
inline void _compress( const uint8_t* input, const size_t size,
                       Compressor::Result& result )
{
    if( size == 0 )
    {
        result.setSize( 0 );
        return;
    }

    const T* in = reinterpret_cast< const T* >( input );
    T* tokenOut = reinterpret_cast< T* >( result.getData( ));
    T tokenLast( in[0] );
    T tokenSame( 1 );
    T token(0);

    for( size_t i = 1; i < size; ++i )
    {
        token = in[i];
        COMPRESS( token );
    }

    WRITE_OUTPUT( token );
    result.setSize( (tokenOut - reinterpret_cast< T* >( result.getData( ))) *
                     sizeof( T ));
#ifndef PRESSION_AGGRESSIVE_CACHING
    result.pack();
#endif
}

template< typename T >
inline void _decompress( const uint8_t* const input, uint8_t* const output,
                         const size_t nElems )
{
    T token(0);
    T tokenLeft(0);
    const T* in = reinterpret_cast< const T*>( input );
    T* out = reinterpret_cast< T* >( output );

    for( size_t i = 0; i < nElems ; ++i )
    {
        if( tokenLeft == 0 )
        {
            token = *in; ++in;
            if( token == _rleMarker )
            {
                token     = *in; ++in;
                tokenLeft = *in; ++in;
            }
            else // single symbol
                tokenLeft = 1;
        }

        --tokenLeft;
        out[i] = token;
    }
}

}

void CompressorRLE::compress( const uint8_t* data, size_t size,
                               Result& output )
{
    if( !_initialized )
        return;

    if( (size & 0x7) == 0 )
        _compress< uint64_t >( data, size>>3, output );
    else if( (size & 0x3) == 0 )
        _compress< uint32_t >( data, size>>2, output );
    else if( (size & 0x1) == 0 )
        _compress< uint16_t >( data, size>>1, output );
    else
        _compress< uint8_t >( data, size, output );
}

void CompressorRLE::decompress( const uint8_t* const input, const size_t,
                                uint8_t* const data, const size_t size )
{
    if( !_initialized )
        return;

    if( (size & 0x7) == 0 )
        _decompress< uint64_t >( input, data, size>>3 );
    else if( (size & 0x3) == 0 )
        _decompress< uint32_t >( input, data, size>>2 );
    else if( (size & 0x1) == 0 )
        _decompress< uint16_t >( input, data, size>>1 );
    else
        _decompress< uint8_t >( input, data, size );
}

}
}
