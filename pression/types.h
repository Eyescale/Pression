
/* Copyright (c) 2007-2016, Stefan Eilemann <eile@equalizergraphics.com>
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

#ifndef PRESSION_TYPES_H
#define PRESSION_TYPES_H

#include <pression/defines.h>
#include <lunchbox/visitorResult.h> // enum
#include <lunchbox/types.h>

/** @cond IGNORE */
struct EqCompressorInfo;
typedef std::vector< EqCompressorInfo > EqCompressorInfos;
typedef EqCompressorInfos::const_iterator EqCompressorInfosCIter;
/** @endcond */

namespace pression
{

using lunchbox::Strings;
using lunchbox::StringsCIter;
using lunchbox::VisitorResult;
using lunchbox::TRAVERSE_CONTINUE;
using lunchbox::TRAVERSE_PRUNE;
using lunchbox::TRAVERSE_TERMINATE;

class Plugin;
class PluginRegistry;
class Uploader;

struct CompressorResult;

typedef lunchbox::Array< void > CompressorChunk;
typedef std::vector< CompressorChunk > CompressorChunks;
typedef std::vector< Plugin* > Plugins;
typedef Plugins::const_iterator PluginsCIter;

template< class, class > class PluginVisitorT;
typedef PluginVisitorT< Plugin, EqCompressorInfo > PluginVisitor;
typedef PluginVisitorT< const Plugin,
                        const EqCompressorInfo > ConstPluginVisitor;

/** @cond IGNORE */
struct CompressorInfo;
typedef std::vector< CompressorInfo > CompressorInfos;
typedef CompressorInfos::const_iterator CompressorInfosCIter;
typedef CompressorInfos::iterator CompressorInfosIter;
/** @endcond */
}

#endif //PRESSION_TYPES_H
