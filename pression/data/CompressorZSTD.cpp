
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

#include "CompressorZSTD.h"

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
    Registry::getInstance().registerEngine< CompressorZSTD< 1 >>(
        { .552f, .180f }) &&
    Registry::getInstance().registerEngine< CompressorZSTD< 2 >>(
        { .550f, .145f }) &&
    Registry::getInstance().registerEngine< CompressorZSTD< 3 >>(
        { .548f, .105f }) &&
    Registry::getInstance().registerEngine< CompressorZSTD< 4 >>(
        { .521f, .091f }) &&
    Registry::getInstance().registerEngine< CompressorZSTD< 5 >>(
        { .520f, .084f }) &&
    Registry::getInstance().registerEngine< CompressorZSTD< 10 >>(
        { .516f, .045f }) &&
    Registry::getInstance().registerEngine< CompressorZSTD< 19 >>(
        { .469f, .013f });
}

template< int level > std::string CompressorZSTD< level >::getName()
{
    return std::string( "pression::data::CompressorZSTD" +
                        std::to_string( level ));
}

template< int level >
size_t CompressorZSTD< level >::getCompressBound( const size_t size ) const
{
    return ZSTD_compressBound( size );
}

template< int level >
void CompressorZSTD< level >::compressChunk( const uint8_t* const data,
                                             const size_t size, Result& output )
{
    if( !_initialized )
        return;

    output.setSize(
        ZSTD_compress( output.getData(), output.getMaxSize(), data, size,
                       level ));
}

template< int level > void
CompressorZSTD< level >::decompressChunk( const uint8_t* const input,
                                          const size_t inputSize,
                                          uint8_t* const data,
                                          const size_t size )
{
    if( _initialized )
        ZSTD_decompress( data, size, input, inputSize );
}

}
}

template class pression::data::CompressorZSTD< 1 >;
template class pression::data::CompressorZSTD< 2 >;
template class pression::data::CompressorZSTD< 3 >;
template class pression::data::CompressorZSTD< 4 >;
template class pression::data::CompressorZSTD< 5 >;
template class pression::data::CompressorZSTD< 10 >;
template class pression::data::CompressorZSTD< 19 >;
