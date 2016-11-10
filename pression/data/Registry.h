
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
    /** @return the global instance for accessing compression engines. */
    PRESSIONDATA_API static Registry& getInstance();

    /**
     * Register a new compression engine.
     *
     * The create method in the given CompressorInfo will be set to the class
     * default ctor.
     */
    template< class P > bool registerEngine( CompressorInfo info )
    {
        info.create = std::bind( boost::factory< P* >( ));
        return _registerEngine( info );
    }

    /** @return the information for all registered compression engines. */
    PRESSIONDATA_API const CompressorInfos& getInfos() const;

    /** @return the recommended compression engine for network transmission */
    PRESSIONDATA_API CompressorInfo choose();

    /** @return the information on the named compression engine */
    PRESSIONDATA_API CompressorInfo find( const std::string& name );
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
