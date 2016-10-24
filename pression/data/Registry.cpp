
/* Copyright (c) 2016, Stefan.Eilemann@epfl.ch
 *
 * This file is part of Collage <https://github.com/Eyescale/Collage>
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

#include "Registry.h"

#include "Compressor.h"

namespace pression
{
namespace data
{

class Registry::Impl
{
public:
    Impl() {}
    ~Impl(){}

    CompressorInfos compressorInfos;
};

Registry& Registry::getInstance()
{
    static Registry _instance;
    return _instance;
}

Registry::Registry()
    : _impl( new Registry::Impl )
{}

Registry::~Registry()
{}

bool Registry::_registerEngine( const CompressorInfo& info )
{
    _impl->compressorInfos.push_back( info );
    return true;
}

const CompressorInfos& Registry::getCompressorInfos() const
{
    return _impl->compressorInfos;
}

CompressorInfo Registry::chooseCompressor()
{
    CompressorInfo candidate;
    float rating = powf( 1.0f, .3f );

    for( const CompressorInfo& info : _impl->compressorInfos )
    {
        float newRating = powf( info.speed, .3f ) / info.ratio;
        if( newRating > rating )
        {
            candidate = info;
            rating = newRating;
        }
    }
    return candidate;
}

CompressorInfo Registry::findCompressor( const std::string& name )
{
    for( const CompressorInfo& info : _impl->compressorInfos )
        if( info.name == name )
            return info;
    return CompressorInfo();
}

}
}
