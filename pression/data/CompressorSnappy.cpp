
/* Copyright (c) 2012-2016, Stefan Eilemann <eile@eyescale.ch>
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

#include "CompressorSnappy.h"

#include <pression/data/Registry.h>
#include <lunchbox/buffer.h>
#include "snappy/snappy.h"

namespace pression
{
namespace data
{
namespace
{
const bool _initialized =
    Registry::getInstance().registerEngine< CompressorSnappy >(
        { "pression::data::CompressorSnappy", .69f, .70f });
}

size_t CompressorSnappy::getCompressBound( const size_t size ) const
{
    return snappy::MaxCompressedLength( size );
}

void CompressorSnappy::compress( const uint8_t* const data, size_t size,
                                 Result& output )
{
    if( !_initialized )
        return;

    snappy::RawCompress( (const char*)data, size,
                         (char*)output.getData(), &size );
    output.setSize( size );
}

void CompressorSnappy::decompress( const uint8_t* const input,
                                   const size_t inputSize, uint8_t* const data,
                                   size_t )
{
    if( _initialized )
        snappy::RawUncompress( (const char*)input, inputSize, (char*)data );
}

}
}
