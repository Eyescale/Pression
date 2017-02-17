
/* Copyright (c) 2013, Stefan.Eilemann@epfl.ch
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

#ifndef PRESSION_PLUGINVISITOR_H
#define PRESSION_PLUGINVISITOR_H

#include <pression/types.h> // enum

namespace pression
{
/**
 * Visitor for all plugins and compressors of a Plugin or PluginRegistry.
 * @version 1.7.1
 */
template <class P, class I>
class PluginVisitorT
{
public:
    PluginVisitorT() {}          //!< Construct a new visitor @version 1.7.1
    virtual ~PluginVisitorT() {} //!< Desctruct this visitor @version 1.7.1
    /** Visit a plugin. @version 1.7.1 */
    virtual VisitorResult visit(P& /*plugin*/) { return TRAVERSE_CONTINUE; }
    /** Visit a compressor of a plugin. @version 1.7.1 */
    virtual VisitorResult visit(P& /*plugin*/, I& /*compressorInfo*/)
    {
        return TRAVERSE_CONTINUE;
    }
};
}
#endif // PRESSION_PLUGINVISITOR_H
