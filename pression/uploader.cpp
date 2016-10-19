
/* Copyright (C) 2013-2016, Stefan.Eilemann@epfl.ch
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

#include "uploader.h"

#include "log.h"
#include "plugin.h"
#include "pluginInstance.h"
#include "pluginRegistry.h"
#include "pluginVisitor.h"

namespace pression
{
namespace detail
{
class Uploader : public PluginInstance
{
public:
    Uploader() {}

    Uploader( const uint32_t name )
    {
        setup( name );
    }

    ~Uploader()
    {
        clear();
    }

    void clear()
    {
        if( plugin && instance )
            plugin->deleteDecompressor( instance );
        PluginInstance::clear();
    }

    bool setup( const uint32_t name )
    {
        if( instance && name == info.name )
            return true;

        clear();

        if( name <= EQ_COMPRESSOR_NONE )
            return true;

        plugin = pression::PluginRegistry::getInstance().findPlugin( name );
        LBASSERT( plugin );
        if( !plugin )
            return false;

        instance = plugin->newCompressor( name );
        info = plugin->findInfo( name );
        LBASSERT( instance );
        LBASSERT( info.name == name );
        LBASSERT( info.capabilities & EQ_COMPRESSOR_TRANSFER );
        LBLOG( LOG_PLUGIN ) << "Instantiated uploader of type 0x" << std::hex
                            << name << std::dec << std::endl;
        return instance;
    }
};
}

Uploader::Uploader()
    : impl_( new detail::Uploader )
{
    LB_TS_THREAD( _thread );
}

Uploader::Uploader( const uint32_t name )
    : impl_( new detail::Uploader( name ))
{
    LB_TS_THREAD( _thread );
}

Uploader::~Uploader()
{
    LBASSERTINFO( impl_->plugin == 0,
                  "Clear uploader while GL context is still active" );
    delete impl_;
}

bool Uploader::isGood( const GLEWContext* gl ) const
{
    LB_TS_SCOPED( _thread );
    return impl_->isGood() && impl_->instance &&
        impl_->plugin->isCompatible( impl_->info.name, gl );
}

bool Uploader::uses( const uint32_t name ) const
{
    return impl_->isGood() && impl_->instance && impl_->info.name == name;
}

bool Uploader::supports( const uint32_t externalFormat,
                         const uint32_t internalFormat,
                         const uint64_t capabilities,
                         const GLEWContext* gl ) const
{
    return isGood( gl ) && impl_->info.outputTokenType == externalFormat &&
           (impl_->info.capabilities & capabilities) == capabilities &&
           impl_->info.tokenType == internalFormat;
}

namespace
{
class Finder : public ConstPluginVisitor
{
public:
    Finder( const uint32_t externalFormat, const uint32_t internalFormat,
            const uint64_t capabilities, const GLEWContext* gl )
        : externalFormat_( externalFormat )
        , internalFormat_( internalFormat )
        , capabilities_( capabilities )
        , gl_( gl )
    {
        current.name = EQ_COMPRESSOR_NONE;
        current.speed = 0.f;
    }

    virtual VisitorResult visit( const Plugin& plugin,
                                 const EqCompressorInfo& info )
    {
        if( (info.capabilities & capabilities_) == capabilities_ &&
            info.outputTokenType == externalFormat_ &&
            info.tokenType == internalFormat_ &&
            plugin.isCompatible( info.name, gl_ ) &&
            current.speed < info.speed )
        {
            current = info;
        }
        return TRAVERSE_CONTINUE;
    }

    EqCompressorInfo current;

private:
    const uint32_t externalFormat_;
    const uint32_t internalFormat_;
    const uint64_t capabilities_;
    const GLEWContext* gl_;
};
}

uint32_t Uploader::choose( const uint32_t externalFormat,
                           const uint32_t internalFormat,
                           const uint64_t capabilities, const GLEWContext* gl )
{
    Finder finder( externalFormat, internalFormat, capabilities, gl );
    PluginRegistry::getInstance().accept( finder );
    return finder.current.name;
}

const EqCompressorInfo& Uploader::getInfo() const
{
    return impl_->info;
}

bool Uploader::setup( const uint32_t name )
{
    return impl_->setup( name );
}

bool Uploader::setup( const uint32_t externalFormat,
                      const uint32_t internalFormat,
                      const uint64_t capabilities, const GLEWContext* gl )
{
    return impl_->setup( choose( externalFormat, internalFormat, capabilities,
                                 gl ));
}

void Uploader::clear()
{
    impl_->clear();
}

void Uploader::upload( const void* buffer, const uint64_t inDims[4],
                       const uint64_t flags, const uint64_t outDims[4],
                       const unsigned destination, const GLEWContext* gl )
{
    LBASSERT( isGood( gl ));
    impl_->plugin->upload( impl_->instance, impl_->info.name, gl, buffer,
                           inDims, flags, outDims, destination );
}

}
