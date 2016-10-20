
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

#ifndef PRESSION_DATACOMPRESSORINFO_H
#define PRESSION_DATACOMPRESSORINFO_H

#include <pression/api.h>
#include <pression/types.h>
#include <functional>

namespace pression
{
struct DataCompressorInfo
{
    DataCompressorInfo()
        : ratio( 1.f )
        , speed( 1.f )
        , create( []{ return nullptr; })
    {}

    DataCompressorInfo( const std::string& n, const float r, const float s )
        : name( n ), ratio( r ), speed( s ) {}

    std::string name;
    float ratio; //!< Normalized 0..1 size after compression
    float speed; //!< Relative speed compared to RLE compressor

    typedef std::function< DataCompressor* ()> Constructor;
    Constructor create;
};
}

#endif
