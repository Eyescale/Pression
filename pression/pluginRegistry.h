
/* Copyright (c) 2010-2016, Cedric Stalder <cedric.stalder@gmail.com>
 *                          Stefan Eilemann <eile@eyescale.ch>
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
 * Downstream projects such as Collage and Equalizer use and initialize the
 * global plugin registry in their respective initialization calls by loading
 * directories or files.
 *
 * Example: @include tests/compressor.cpp
 */
class PluginRegistry
{
public:
    PRESSION_API static PluginRegistry& getInstance();

    /** Add a single plugin DSO. @return true if found. */
    PRESSION_API bool loadFile( const std::string& filename );

    /** Add all plugins in a directory. @return the number of plugins loaded */
    PRESSION_API size_t loadDirectory( const std::string& dir );

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

private:
    PluginRegistry();
    ~PluginRegistry();

    PluginRegistry( const PluginRegistry& ) = delete;
    PluginRegistry( PluginRegistry&& ) = delete;
    PluginRegistry& operator=( const PluginRegistry& ) = delete;
    PluginRegistry& operator=( PluginRegistry&& ) = delete;

    detail::PluginRegistry* const _impl;
};
}
#endif // PRESSION_PLUGINREGISTRY_H
