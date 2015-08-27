
/* Copyright (c) 2010, Cedric Stalder <cedric.stalder@gmail.com>
 *               2010-2014, Stefan Eilemann <eile@eyescale.ch>
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

#ifndef PRESSION_PLUGINREGISTRY_H
#define PRESSION_PLUGINREGISTRY_H

#include <pression/api.h>
#include <pression/types.h>

#include <string>

namespace pression
{
namespace detail { class PluginRegistry; }

/**
 * A registry for loaded plugins.
 *
 * Downstream projects such as Collage and Equalizer use and initialize a global
 * plugin registry in their respective initialization calls by adding
 * directories in co::init(). This internal plugin registry can be obtained
 * using co::Global::getPluginRegistry().
 *
 * Example: @include tests/compressor.cpp
 */
class PluginRegistry
{
public:
    /** Construct a new plugin registry. @version 1.7.1 */
    PRESSION_API PluginRegistry();

    /** Destruct this plugin registry. @version 1.7.1 */
    PRESSION_API ~PluginRegistry();

    /**
     * Add a new directory to search for compressor DSOs during init().
     * @version 1.0 in Collage
     * @version 1.7.1 in Lunchbox
     */
    PRESSION_API void addDirectory( const std::string& path );

    /** Remove a plugin directory. @version 1.0 */
    PRESSION_API void removeDirectory( const std::string& path );

    /**
     * @return all directories to search for compressor DSOs during init().
     * @version 1.0 in Collage
     * @version 1.7.1 in Lunchbox
     */
    PRESSION_API const Strings& getDirectories() const;

    /**
     * Add the lunchbox library plugins to this registry.
     * @return true on success, false otherwise.
     * @version 1.7.1
     */
    PRESSION_API bool addLunchboxPlugins();

    /** @internal Search all plugin directories and register found DSOs */
    PRESSION_API void init();

    /** @internal Exit all DSOs and free all plugins */
    PRESSION_API void exit();

    /**
     * Visit all plugins and compressors.
     *
     * @return TRAVERSE_TERMINATE immediately when one visit method returned
     *         terminate, TRAVERSE_PRUNE if at least one visit method returned
     *         prune, TRAVERSE_CONTINUE otherwise.
     * @version 1.7.1
     */
    PRESSION_API VisitorResult accept( PluginVisitor& visitor );

    /** Visit all plugins and compressors. @version 1.7.1 */
    PRESSION_API VisitorResult accept( ConstPluginVisitor& visitor ) const;

    /** @internal @return all registered compressor plugins */
    PRESSION_API const Plugins& getPlugins() const;

    /** @internal @return the plugin containing the given compressor. */
    PRESSION_API Plugin* findPlugin( const uint32_t name );

    /** @internal @return the plugin containing the given compressor. */
    PRESSION_API const Plugin* findPlugin( const uint32_t name ) const;

    /** @internal Add a single DSO before init(). @return true if found. */
    PRESSION_API bool addPlugin( const std::string& filename );

private:
    PluginRegistry( const PluginRegistry& );
    PluginRegistry operator=( const PluginRegistry& );
    detail::PluginRegistry* const impl_;
};
}
#endif // PRESSION_PLUGINREGISTRY_H
