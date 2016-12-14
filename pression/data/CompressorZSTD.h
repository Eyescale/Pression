
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

#pragma once

#include <pression/data/Compressor.h>

namespace pression
{
namespace data
{

template< int level > class CompressorZSTD : public Compressor
{
public:
    CompressorZSTD() : Compressor() {}
    virtual ~CompressorZSTD() {}

    static std::string getName();
    size_t getCompressBound( const size_t size ) const override;
    void compressChunk( const uint8_t* data, size_t size, Result& output )final;
    void decompressChunk( const uint8_t* input, size_t inputSize,
                          uint8_t* const data, size_t size ) final;
};
}
}
