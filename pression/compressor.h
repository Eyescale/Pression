
/* Copyright (c)      2010, Cedric Stalder <cedric.stalder@gmail.com>
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

#ifndef PRESSION_COMPRESSOR_H
#define PRESSION_COMPRESSOR_H

#include <pression/api.h>
#include <pression/types.h>
#include <lunchbox/thread.h>         // thread-safety macros

namespace pression
{
namespace detail { class Compressor; }

/**
 * A C++ class to handle one compressor plugin instance.
 *
 * Example: @include tests/compressor.cpp
 */
class Compressor
{
    typedef detail::Compressor* const Compressor::*bool_t;

public:
    /** Construct a new, invalid compressor instance. @version 1.7.1 */
    PRESSION_API Compressor();

    /**
     * Construct a new, named compressor instance.
     *
     * @param name the name of the compressor
     * @version 2.0
     */
    PRESSION_API Compressor( const uint32_t name );

    /** Destruct the compressor. @version 1.7.1 */
    PRESSION_API virtual ~Compressor();

    /** @return true if the instance is usable. @version 1.7.1 */
    PRESSION_API bool isGood() const;

    /**
     * @return true if the instance is usable, false otherwise.
     * @version 1.9.1
     */
    operator bool_t() const { return isGood() ? &Compressor::impl_ : 0; }

    /** @return true if the instance is not usable. @version 1.9.1 */
    bool operator ! () const { return !isGood(); }

    /**
     * @return true if the instance is usable for the given name.
     * @version 1.7.1
     */
    PRESSION_API bool uses( const uint32_t name ) const;

    /** @return the information about the allocated instance. @version 1.7.1 */
    PRESSION_API const EqCompressorInfo& getInfo() const;

    /**
     * Find the best compressor in all plugins for the given parameters.
     *
     * This convenience method searches all compressors in all plugins to
     * find the compressor which matches best the given parameters.
     *
     * @param tokenType the structure of the data to compress.
     * @param minQuality minimal quality of the compressed data, with 0 = no
     *                   quality and 1 = full quality, no loss.
     * @param ignoreMSE the most-significant element of a four-element token can
     *                  be ignored, typically the alpha channel of an image.
     * @return the name of the chosen compressor.
     * @version 2.0
     */
    static PRESSION_API uint32_t choose( const uint32_t tokenType,
                                         const float minQuality,
                                         const bool ignoreMSE );

    /**
     * Set up a new, named compressor instance.
     *
     * @param name the name of the compressor.
     * @return true on success, false otherwise.
     * @version 2.0
     */
    PRESSION_API bool setup( const uint32_t name );

    /**
     * Set up a new, auto-selected compressor instance.
     * @sa choose() for parameters.
     * @version 1.7.1
     */
    PRESSION_API bool setup( const uint32_t tokenType, const float minQuality,
                             const bool ignoreMSE );

    /** Reallocate the current instance. @version 1.7.1 */
    PRESSION_API bool realloc();

    /** Reset to EQ_COMPRESSOR_NONE. @version 1.7.1 */
    PRESSION_API void clear();

    /**
     * Compress one-dimensional data.
     *
     * @param in the pointer to the input data.
     * @param inDims the dimensions of the input data
     * @version 1.7.1
     */
    PRESSION_API void compress( void* const in, const uint64_t inDims[2] );

    /**
     * Compress two-dimensional data.
     *
     * @param in the pointer to the input data.
     * @param pvp the dimensions of the input data
     * @param flags capability flags for the compression
     * @version 1.7.1
     */
    PRESSION_API void compress( void* const in, const uint64_t pvp[4],
                                const uint64_t flags );

    /** @deprecated use new getResult()
     * @return the number of compressed chunks of the last compression.
     * @version 1.7.1
     */
    PRESSION_API unsigned getNumResults() const LB_DEPRECATED;

    /**
     * @return the result of the last compression.
     * @version 1.9.1
     */
    PRESSION_API CompressorResult getResult() const;

    /** @deprecated use new getResult() */
    PRESSION_API void getResult( const unsigned i, void** const out,
                                 uint64_t* const outSize ) const LB_DEPRECATED;

private:
    Compressor( const Compressor& );
    Compressor operator=( const Compressor& );
    detail::Compressor* const impl_;
    LB_TS_VAR( _thread );
};
}
#endif  // PRESSION_COMPRESSOR_H
