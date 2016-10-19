
/* Copyright (c) 2013-2016, Stefan.Eilemann@epfl.ch
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

#ifndef PRESSION_DOWNLOADER_H
#define PRESSION_DOWNLOADER_H

#include <pression/api.h>
#include <pression/types.h>
#include <lunchbox/thread.h>         // thread-safety macros

namespace pression
{
namespace detail { class Downloader; }

/** A C++ class to handle one downloader plugin instance. */
class Downloader
{
public:
    /** Construct a new, invalid downloader instance. @version 1.7.1 */
    PRESSION_API Downloader();

    /**
     * Construct a new, named downloader instance.
     *
     * @param name the name of the downloader.
     * @param gl the OpenGL function table to verify that the plugin is
     *           compatible with this GL implementation.
     * @version 2.0
     */
    PRESSION_API Downloader( const uint32_t name, const GLEWContext* gl = 0 );

    /** Destruct this downloader. @version 1.7.1 */
    PRESSION_API virtual ~Downloader();

    /** @return true if the instance is usable. @version 1.7.1 */
    PRESSION_API bool isGood() const;

    /**
     * @return true if the instance is usable for the given name.
     * @version 1.7.1
     */
    PRESSION_API bool uses( const uint32_t name ) const;

    /**
     * @return true if the download supports the given parameters.
     * @version 1.7.1
     */
    PRESSION_API bool supports( const uint32_t inputToken, const bool noAlpha,
                                const uint64_t capabilities ) const;

    /**
     * Find the best downloader in all plugins for the given parameters.
     *
     * This convenience method searches all compressors in all plugins to find
     * the downloader which supports the given parameters and provides the
     * highest speed.
     *
     * @param internalFormat the input token type to the downloader.
     * @param minQuality the minimum quality.
     * @param ignoreAlpha true if the downloader may drop the alpha channel.
     * @param capabilities the capabilities required by the downloader.
     * @param gl the OpenGL function table.
     * @return the name of the chosen downloader.
     * @version 2.0
     */
    static PRESSION_API uint32_t choose( const uint32_t internalFormat,
                                         const float minQuality,
                                         const bool ignoreAlpha,
                                         const uint64_t capabilities,
                                         const GLEWContext* gl );

    /** @return the information about the allocated instance. @version 1.7.1 */
    PRESSION_API const EqCompressorInfo& getInfo() const;

    /**
     * Set up a new, named downloader instance.
     *
     * @param name the name of the downloader.
     * @param gl the OpenGL function table to verify that the plugin is
     *           compatible with this GL implementation.
     * @return true on success, false otherwise.
     * @version 2.0
     */
    PRESSION_API bool setup( const uint32_t name, const GLEWContext* gl = 0 );

    /**
     * Set up a new, auto-selected downloader instance.
     * @sa choose() for parameters.
     * @version 2.0
     */
    PRESSION_API bool setup( const uint32_t internalFormat,
                             const float minQuality, const bool ignoreAlpha,
                             const uint64_t capabilities,
                             const GLEWContext* gl );

    /** Reset to EQ_COMPRESSOR_NONE. @version 1.7.1 */
    PRESSION_API void clear();

    /**
     * Start downloading data from the GPU to the CPU
     *
     * @param buffer data destination buffer.
     * @param inDims the dimensions of the input data.
     * @param flags capability flags for the compression.
     * @param outDims return value for the dimensions of the output data.
     * @param source the source texture name, or 0 for framebuffer.
     * @param gl the OpenGL function table.
     * @return true if finish is needed, false if a synchronous download was
     *         performed.
     * @version 1.7.1
     */
    PRESSION_API bool start( void** buffer, const uint64_t inDims[4],
                             const uint64_t flags, uint64_t outDims[4],
                             const unsigned source, const GLEWContext* gl );

    /**
     * Finish download data from the GPU to the CPU
     *
     * @param buffer data destination buffer.
     * @param inDims the dimensions of the input data.
     * @param flags capability flags for the compression.
     * @param outDims return value for the dimensions of the output data.
     * @param gl the OpenGL function table.
     * @version 1.7.1
     */
    PRESSION_API void finish( void** buffer, const uint64_t inDims[4],
                              const uint64_t flags, uint64_t outDims[4],
                              const GLEWContext* gl );

private:
    Downloader( const Downloader& );
    Downloader operator=( const Downloader& );
    detail::Downloader* const impl_;
    LB_TS_VAR( _thread );
};
}
#endif  // PRESSION_DOWNLOADER_H
