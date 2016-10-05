
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

#include "zstd/lib/zstd.h"

namespace pression
{
namespace plugin
{
namespace
{
static void _getInfo( EqCompressorInfo* const info )
{
    info->version = EQ_COMPRESSOR_VERSION;
    info->capabilities = EQ_COMPRESSOR_DATA_1D | EQ_COMPRESSOR_DATA_2D;
    info->quality = 1.f;
    info->ratio   = .47f;
    info->speed   = .25f;
    info->name = EQ_COMPRESSOR_ZSTD_BYTE;
    info->tokenType = EQ_COMPRESSOR_DATATYPE_BYTE;
}

static bool _register()
{
    Compressor::registerEngine(
        Compressor::Functions( EQ_COMPRESSOR_ZSTD_BYTE,
                               _getInfo,
                               CompressorZSTD::getNewCompressor,
                               CompressorZSTD::getNewDecompressor,
                               CompressorZSTD::decompress, 0 ));
    return true;
}

static const bool LB_UNUSED _initialized = _register();
}

void CompressorZSTD::compress( const void* const inData,
                               const eq_uint64_t nPixels,
                               const bool /*alpha*/ )
{
    _nResults = 1;
    if( _results.size() < _nResults )
        _results.push_back( new pression::plugin::Compressor::Result );
    size_t size = ZSTD_compressBound( nPixels );
    _results[0]->reserve( size );

    size = ZSTD_compress( _results[0]->getData(), size, inData, nPixels, 2 );
    assert( size != 0 );
    _results[0]->setSize( size );
}

void CompressorZSTD::decompress( const void* const* inData,
                                   const eq_uint64_t* const inSizes,
                                   const unsigned nInputs,
                                   void* const outData,
                                   eq_uint64_t* const outDims,
                                   const eq_uint64_t flags, void* const )
{
    if( nInputs == 0 )
        return;

    const eq_uint64_t nPixels = ( flags & EQ_COMPRESSOR_DATA_1D) ?
                                    outDims[1] : outDims[1] * outDims[3];
    ZSTD_decompress( outData, nPixels, inData[0], inSizes[0] );
}

}
}
