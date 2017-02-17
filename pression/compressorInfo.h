
/* Copyright (c) 2010-2013, Stefan Eilemann <eile@eyescale.ch>
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

#ifndef PRESSION_COMPRESSORINFO_H
#define PRESSION_COMPRESSORINFO_H

#include <pression/plugins/compressor.h> // base struct
#include <pression/types.h>

#include <iostream>

namespace pression
{
/** @internal Augment the plugin information with additional data. */
struct CompressorInfo : public EqCompressorInfo
{
    CompressorInfo()
        : ratingAlpha(0.f)
        , ratingNoAlpha(0.f)
    {
    }

    float ratingAlpha;   //!< f( speed, ratio ) heuristic, bigger is better
    float ratingNoAlpha; //!< f( speed, ratio ) heuristic
};

inline std::ostream& operator<<(std::ostream& os, const CompressorInfo& info)
{
    return os << static_cast<const EqCompressorInfo&>(info) << " rating "
              << info.ratingAlpha;
}
}

#endif // PRESSION_COMPRESSORINFO_H
