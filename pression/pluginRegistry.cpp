
/* Copyright (c) 2009-2016, Cedric Stalder <cedric.stalder@gmail.com>
 *                          Stefan Eilemann <eile@eyescale.ch>
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

#include "pluginRegistry.h"

#include "compressorInfo.h"
#include "log.h"
#include "plugin.h"
#include "pluginVisitor.h"

#include <lunchbox/algorithm.h>
#include <lunchbox/debug.h>
#include <lunchbox/file.h>

#ifdef _WIN32
#  include <lunchbox/os.h> // GetModuleFileName
#  include <direct.h>
#  define getcwd _getcwd
#else
#  include <unistd.h>   // for getcwd
#endif

#ifndef MAXPATHLEN
#  define MAXPATHLEN 1024
#endif

namespace pression
{
namespace
{
Plugin* _loadPlugin( const std::string& filename )
{
    Plugin* plugin = new Plugin( filename );
    if( plugin->isGood( ))
        return plugin;

    delete plugin;
    return nullptr;
}
}

namespace detail
{
class PluginRegistry
{
public:
    PluginRegistry()
    {
#ifdef PRESSION_DSO_NAME
        loadFile( PRESSION_DSO_NAME );
        loadFile( std::string( PRESSION_BUILD_DIR ) + "lib/" +
                  PRESSION_DSO_NAME );
#  ifdef NDEBUG
        loadFile( std::string( PRESSION_BUILD_DIR ) + "lib/Release/" +
                  PRESSION_DSO_NAME );
#  else
        loadFile( std::string( PRESSION_BUILD_DIR ) + "lib/Debug/" +
                  PRESSION_DSO_NAME );
#  endif
#endif

        char cwd[MAXPATHLEN];
        loadDirectory( getcwd( cwd, MAXPATHLEN ));

        char* env = getenv( "PRESSION_PLUGIN_PATH" );
        std::string envString;
        if( env )
            envString = env;

#ifdef _WIN32
        const char separator = ';';
        if( GetModuleFileName( 0, cwd, MAXPATHLEN ) > 0 )
            loadDirectory( lunchbox::getDirname( cwd ));
#else
        const char separator = ':';
#  ifdef Darwin
        env = getenv( "DYLD_LIBRARY_PATH" );
#  else
        env = getenv( "LD_LIBRARY_PATH" );
#  endif
        if( env )
            envString += std::string( ":" ) + env;
#  endif

        while( !envString.empty( ))
        {
            size_t nextPos = envString.find( separator );
            if ( nextPos == std::string::npos )
                nextPos = envString.size();

            std::string path = envString.substr( 0, nextPos );
            if( nextPos == envString.size( ))
                envString = "";
            else
                envString = envString.substr( nextPos + 1, envString.size() );

            if( !path.empty( ))
                loadDirectory( path );
        }
    }

    ~PluginRegistry()
    {
        for( pression::Plugin* plugin : plugins )
            delete plugin;
    }

    size_t loadDirectory( const std::string& dir )
    {
        LBLOG( LOG_PLUGIN ) << "Searching plugins in " << dir << std::endl;

#ifdef _WIN32
        const auto& files =
            lunchbox::searchDirectory( dir, ".*Compressor.*\\.dll" );
        const char SEP = '\\';
#elif defined (Darwin)
        const auto& files =
            lunchbox::searchDirectory( dir, "lib.*Compressor.*\\.dylib" );
        const char SEP = '/';
#else
        const auto& files =
            lunchbox::searchDirectory( dir, "lib.*Compressor.*\\.so" );
        const char SEP = '/';
#endif

        size_t loaded = 0;
        for( const auto& file : files )
            loaded += loadFile( dir.empty() ? file : dir + SEP + file );
        return loaded;
    }

    bool loadFile( const std::string& filename )
    {
        pression::Plugin* plugin = _loadPlugin( filename );
        if( !plugin )
            return false;

        const CompressorInfos& infos = plugin->getInfos();
        for( pression::Plugin* plugin2 : plugins )
        {
            const CompressorInfos& infos2 = plugin2->getInfos();

            // Simple test to avoid loading the same dll twice
            if( infos.front().name == infos2.front().name )
            {
                delete plugin;
                return false;
            }
        }

        plugins.push_back( plugin );
        LBLOG( LOG_PLUGIN ) << "Found " << plugin->getInfos().size()
                            << " compression engines in " << filename
                            << std::endl;
        return true;
    }

    Plugins plugins;
};
}

PluginRegistry& PluginRegistry::getInstance()
{
    static PluginRegistry _instance;
    return _instance;
}

PluginRegistry::PluginRegistry()
    : _impl( new detail::PluginRegistry )
{}

PluginRegistry::~PluginRegistry()
{
    delete _impl;
}

size_t PluginRegistry::loadDirectory( const std::string& dir )
{
    return _impl->loadDirectory( dir );
}

bool PluginRegistry::loadFile( const std::string& filename )
{
    return _impl->loadFile( filename );
}

namespace
{
template< class P,  class I > class Finder : public PluginVisitorT< P, I >
{
public:
    explicit Finder( const uint32_t name ) : plugin( 0 ), name_( name ) {}
    virtual VisitorResult visit( P& candidate, I& info )
    {
        if( info.name != name_ )
            return TRAVERSE_CONTINUE;

        plugin = &candidate;
        return TRAVERSE_TERMINATE;
    }

    P* plugin;
private:
    const uint32_t name_;
};
}

Plugin* PluginRegistry::findPlugin( const uint32_t name )
{
    Finder< Plugin, EqCompressorInfo > finder( name );
    accept( finder );
    return finder.plugin;
}

const Plugin* PluginRegistry::findPlugin( const uint32_t name ) const
{
    Finder< const Plugin, const EqCompressorInfo > finder( name );
    accept( finder );
    return finder.plugin;
}

VisitorResult PluginRegistry::accept( PluginVisitor& visitor )
{
    VisitorResult result = TRAVERSE_CONTINUE;
    for( PluginsCIter i = _impl->plugins.begin(); i != _impl->plugins.end(); ++i )
        switch( (*i)->accept( visitor ))
        {
        case TRAVERSE_TERMINATE:
            return TRAVERSE_TERMINATE;
        case TRAVERSE_PRUNE:
            result = TRAVERSE_PRUNE;
        default:
            break;
        }

    return result;
}
VisitorResult PluginRegistry::accept( ConstPluginVisitor& visitor ) const
{
    VisitorResult result = TRAVERSE_CONTINUE;
    for( PluginsCIter i = _impl->plugins.begin(); i != _impl->plugins.end(); ++i )
        switch( (*i)->accept( visitor ))
        {
        case TRAVERSE_TERMINATE:
            return TRAVERSE_TERMINATE;
        case TRAVERSE_PRUNE:
            result = TRAVERSE_PRUNE;
        case TRAVERSE_CONTINUE:
            break;
        }

    return result;
}

const Plugins& PluginRegistry::getPlugins() const
{
    return _impl->plugins;
}

}
