
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

#ifndef PRESSION_PLUGIN_COMPRESSORLZF
#define PRESSION_PLUGIN_COMPRESSORLZF

#include <pression/dataCompressor.h>

namespace pression
{
namespace plugin
{

class CompressorLZF : public DataCompressor
{
public:
    CompressorLZF() : DataCompressor() {}
    virtual ~CompressorLZF() {}

    size_t getCompressBound( const size_t size ) const override
        { return size_t( float( size ) * 1.1f ) + 8; }
    void compress( const uint8_t* data, size_t size, Result& result ) override;
    void decompress( const Result& input, uint8_t* const data,
                     size_t size ) override;
};
}
}
#endif
