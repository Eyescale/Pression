
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

#include <pression/data/Registry.h>
#include <lunchbox/buffer.h>
#include "zstd/lib/zstd.h"

namespace pression
{
namespace data
{
namespace
{
const bool _initialized =
    Registry::getInstance().registerEngine< CompressorZSTD >(
        { "pression::data::CompressorZSTD", .47f, .25f });
}

size_t CompressorZSTD::getCompressBound( const size_t size ) const
{
    return ZSTD_compressBound( size );
}

void CompressorZSTD::compress( const uint8_t* const data, const size_t size,
                               Result& output )
{
    if( !_initialized )
        return;

    output.setSize(
        ZSTD_compress( output.getData(), output.getMaxSize(), data, size, 2 ));
}

void CompressorZSTD::decompress( const Result& input, uint8_t* const data,
                                 const size_t size )
{
    if( _initialized )
        ZSTD_decompress( data, size, input.getData(), input.getSize( ));
}

}
}
