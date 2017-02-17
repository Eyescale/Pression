
/* Copyright (c) 2016, Stefan.Eilemann@epfl.ch
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

#include <lunchbox/types.h>
#include <pression/defines.h>

namespace pression
{
/**
 * Lossless byte compression plugin API.
 *
 * Custom compressors inherit the Compressor API, either overriding the public
 * compress and decompress methods (if the used algorithm is parallel), or the
 * protected ones if the algorithm is serial. They use the static initialization
 * Registry::registerEngine() to register themselves. The CompressorInfo
 * describes the characteristics of the plugin. See CompressorSnappy for
 * an example implementation.
 *
 * Provided with pression are a very low overhead RLE compressor, two LZ
 * variants, Snappy and ZStandard compression plugins.
 */
namespace data
{
class Compressor;
struct CompressorInfo;

typedef std::vector<CompressorInfo> CompressorInfos;
}
}
