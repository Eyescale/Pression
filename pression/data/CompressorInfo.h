
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

#pragma once

#include <pression/data/api.h>
#include <pression/data/types.h>
#include <functional>

namespace pression
{
namespace data
{
struct CompressorInfo
{
    CompressorInfo()
        : ratio( 1.f )
        , speed( 1.f )
        , create( []{ return nullptr; })
    {}

    CompressorInfo( const std::string& n, const float r, const float s )
        : name( n ), ratio( r ), speed( s ), create( []{ return nullptr; }) {}

    bool operator == ( const CompressorInfo& rhs ) const
        { return name == rhs.name; }

    std::string name;
    float ratio; //!< Normalized 0..1 size after compression
    float speed; //!< Relative speed compared to RLE compressor

    typedef std::function< Compressor* ()> Constructor;
    Constructor create;
};
}
}
