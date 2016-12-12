
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

#include "downloader.h"

#include "log.h"
#include "plugin.h"
#include "pluginInstance.h"
#include "pluginRegistry.h"
#include "pluginVisitor.h"

#include <limits>

namespace pression
{
namespace detail
{
class Downloader : public PluginInstance
{
public:
    Downloader() {}

    Downloader( const uint32_t name, const GLEWContext* gl )
    {
        LBCHECK( setup( name, gl ));
    }

    ~Downloader()
    {
        clear();
    }

    void clear()
    {
        if( plugin && instance )
            plugin->deleteDecompressor( instance );
        PluginInstance::clear();
    }

    bool setup( const uint32_t name, const GLEWContext* gl )
    {
        if( name == info.name )
        {
            LBASSERT( isGood() && instance );
            return true;
        }

        clear();

        if( name <= EQ_COMPRESSOR_NONE )
        {
            LBASSERT( isGood() && instance );
            return true;
        }

        plugin = pression::PluginRegistry::getInstance().findPlugin( name );
        LBASSERT( plugin );
        if( !plugin )
        {
            LBWARN << "Plugin for downloader 0x" << std::hex << name << std::dec
                   << " not found" << std::endl;
            return false;
        }

        if( !gl )
            LBWARN << "Can't verify plugin compatibility, no GLEWContext given"
                   << std::endl;
        else if( !plugin->isCompatible( name, gl ))
        {
            LBWARN << "Plugin for downloader 0x" << std::hex << name << std::dec
                   << " not compatible with OpenGL implementation" << std::endl;
            return false;
        }

        instance = plugin->newDecompressor( name );
        info = plugin->findInfo( name );
        LBASSERT( isGood( ));
        LBASSERT( instance );
        LBASSERT( info.name == name );
        LBLOG( LOG_PLUGIN ) << "Instantiated downloader " << info << std::endl;
        return instance;
    }

};
}

Downloader::Downloader()
    : impl_( new detail::Downloader )
{
    LB_TS_THREAD( _thread );
}

Downloader::Downloader( const uint32_t name, const GLEWContext* gl )
    : impl_( new detail::Downloader( name, gl ))
{
    LB_TS_THREAD( _thread );
}

Downloader::~Downloader()
{
    LBASSERTINFO( impl_->plugin == 0,
                  "Clear downloader while GL context is still active" );
    delete impl_;
}

bool Downloader::isGood() const
{
    LB_TS_SCOPED( _thread );
    return impl_->isGood() && impl_->instance;
}

bool Downloader::uses( const uint32_t name ) const
{
    return isGood() && impl_->info.name == name;
}

bool Downloader::supports( const uint32_t inputToken, const bool noAlpha,
                           const uint64_t capabilities ) const
{
    return isGood() && impl_->info.tokenType == inputToken &&
           (impl_->info.capabilities & capabilities) == capabilities &&
           ( noAlpha ||
             !(impl_->info.capabilities & EQ_COMPRESSOR_IGNORE_ALPHA) );
}

namespace
{
class Finder : public ConstPluginVisitor
{
public:
    Finder( const uint32_t internalFormat, const float minQuality,
            const bool ignoreAlpha, const uint64_t capabilities,
            const GLEWContext* gl )
        : internalFormat_( internalFormat )
        , minQuality_( minQuality )
        , ignoreAlpha_( ignoreAlpha )
        , capabilities_( capabilities )
        , gl_( gl )
    {
        LBASSERT( gl );
        current.name = EQ_COMPRESSOR_NONE;
        current.speed = 0.f;
        current.ratio = std::numeric_limits< float >::max();
    }

    virtual VisitorResult visit( const Plugin& plugin,
                                 const EqCompressorInfo& info )
    {
        if(( (info.capabilities & capabilities_) == capabilities_ ) &&
           info.tokenType == internalFormat_ && info.quality >= minQuality_ &&
           ( ignoreAlpha_ ||
             !(info.capabilities & EQ_COMPRESSOR_IGNORE_ALPHA) ) &&
           plugin.isCompatible( info.name, gl_ ) &&
           ( current.ratio > info.ratio ||
             ( current.ratio == info.ratio && current.speed < info.speed )))
        {
            LBLOG( LOG_PLUGIN ) << "Pick "  << info << std::endl;
            current = info;
        }
        return TRAVERSE_CONTINUE;
    }

    EqCompressorInfo current;

private:
    const uint32_t internalFormat_;
    const float minQuality_;
    const bool ignoreAlpha_;
    const uint64_t capabilities_;
    const GLEWContext* gl_;
};
}

uint32_t Downloader::choose( const uint32_t internalFormat,
                             const float minQuality, const bool ignoreAlpha,
                             const uint64_t capabilities,
                             const GLEWContext* gl )
{
    Finder finder( internalFormat, minQuality, ignoreAlpha, capabilities, gl );
    PluginRegistry::getInstance().accept( finder );
    return finder.current.name;
}

const EqCompressorInfo& Downloader::getInfo() const
{
    return impl_->info;
}

bool Downloader::setup( const uint32_t name, const GLEWContext* gl )
{
    return impl_->setup( name, gl );
}

bool Downloader::setup( const uint32_t internalFormat,
                        const float minQuality, const bool ignoreAlpha,
                        const uint64_t capabilities,const GLEWContext* gl)
{
    return impl_->setup( choose( internalFormat, minQuality, ignoreAlpha,
                                 capabilities, gl ), gl );
}

void Downloader::clear()
{
    impl_->clear();
}

bool Downloader::start( void** buffer, const uint64_t inDims[4],
                        const uint64_t flags, uint64_t outDims[4],
                        const unsigned source, const GLEWContext* gl )
{
    LBASSERT( gl );
    if( impl_->info.capabilities & EQ_COMPRESSOR_USE_ASYNC_DOWNLOAD )
    {
        impl_->plugin->startDownload( impl_->instance, impl_->info.name, gl,
                                      inDims, source,
                                      flags | EQ_COMPRESSOR_USE_ASYNC_DOWNLOAD);
        return true;
    }

    impl_->plugin->download( impl_->instance, impl_->info.name, gl, inDims,
                             source, flags, outDims, buffer );
    return false;
}

void Downloader::finish( void** buffer, const uint64_t inDims[4],
                         const uint64_t flags, uint64_t outDims[4],
                         const GLEWContext* gl )
{
    LBASSERT( impl_->plugin );
    LBASSERT( impl_->instance );
    LBASSERT( gl );
    if( !impl_->plugin )
        return;

    if( impl_->info.capabilities & EQ_COMPRESSOR_USE_ASYNC_DOWNLOAD )
    {
        impl_->plugin->finishDownload( impl_->instance, impl_->info.name, gl,
                                       inDims,
                                       flags | EQ_COMPRESSOR_USE_ASYNC_DOWNLOAD,
                                       outDims, buffer );
    }
}

}
