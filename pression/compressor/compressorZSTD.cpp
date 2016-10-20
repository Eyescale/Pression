
/* Copyright (c) 2016, Stefan Eilemann <eile@eyescale.ch>
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

#include "compressorZSTD.h"

#include <pression/pluginRegistry.h>
#include <lunchbox/buffer.h>
#include "zstd/lib/zstd.h"

namespace pression
{
namespace plugin
{
namespace
{
const bool _initialized =
    PluginRegistry::getInstance().registerEngine< CompressorZSTD >(
        { "pression::CompressorZSTD", .47f, .25f });
}

const CompressorZSTD::Results&
CompressorZSTD::compress( const uint8_t* const data, const size_t inSize )
{
    if( !_initialized )
        return compressed;

    compressed.resize( 1 );
    size_t size = ZSTD_compressBound( inSize );
    compressed[0].reserve( size );

    size = ZSTD_compress( compressed[0].getData(), size, data, inSize, 2 );
    compressed[0].setSize( size );
    return compressed;
}

void CompressorZSTD::decompress( const Results& input, uint8_t* const data,
                                 const size_t size )
{
    if( input.empty() || !_initialized )
        return;

    ZSTD_decompress( data, size, input[0].getData(), input[0].getSize( ));
}

}
}
