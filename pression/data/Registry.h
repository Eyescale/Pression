
/* Copyright (c) 2016, Stefan.Eilemann@epf.ch
 *
 * This file is part of Pression <https://github.com/Eyescale/Pression>
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

#include <pression/data/api.h>
#include <pression/data/CompressorInfo.h> // used inline
#include <pression/data/types.h>

#include <boost/functional/factory.hpp> // used inline
#include <string>

namespace pression
{
namespace data
{

/** A registry for loaded plugins. */
class Registry
{
public:
    PRESSIONDATA_API static Registry& getInstance();

    template< class P > bool registerEngine( CompressorInfo info )
    {
        info.create = std::bind( boost::factory< P* >( ));
        return _registerEngine( info );
    }

    PRESSIONDATA_API const CompressorInfos& getCompressorInfos() const;

    PRESSIONDATA_API CompressorInfo chooseCompressor();
    PRESSIONDATA_API CompressorInfo findCompressor( const std::string& name );
    //@}

private:
    Registry();
    ~Registry();

    Registry( const Registry& ) = delete;
    Registry( Registry&& ) = delete;
    Registry& operator=( const Registry& ) = delete;
    Registry& operator=( Registry&& ) = delete;

    class Impl;
    std::unique_ptr< Impl > _impl;

    PRESSIONDATA_API bool _registerEngine( const CompressorInfo& info );
};
}
}
