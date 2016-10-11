C++ Plugin API for CPU compressors
============

This document specifies a modern API for the in-memory C plugin API for
lossless byte data compressors. It will not replace the 2D image
compressors and CPU-GPU transfer plugins.

## Requirements

* Support all features used by current lossless byte compressors
    * Lossless compression of binary blobs
    * Multiple compressors per plugin DSO
* Information provided for each compressor:
    * Compression speed (relative to RLE compressor)
    * Compression ratio (relative to uncompressed data)
* Compressors may produce multiple output chunks
* Compressor engines are instantiated to allow reuse of internal state
  and memory allocations
* Easy to use API for plugin implementers

## Dependency Changes

* New: Using Lunchbox plugin API for loading plugins

## API

    const Plugins& Lunchbox::PluginFactory<..>::getPlugins();

    class PluginInfo
    {
    public:
        std::string name;
        float ratio; //!< Normalized 0..1 size after compression
        float speed; //!< Relative speed compared to RLE compressor
    };
    typedef std::vector< PluginInfo > PluginInfos;

    class PluginRegistry
    {
    public:
        typedef std::unique_ptr< Compressor > CompressorPtr;
        CompressorPtr choose();
        CompressorPtr create( const std::string& name );
    };

    class Compressor
    {
    public:
        typedef lunchbox::Bufferb Result;
        typedef std::vector< Result > Results;

        virtual const Results& compress( const void* const data,
                                         const size_t size ) = 0;
        virtual const Result& decompress( const Results& input,
                                          const size_t size ) = 0;
    protected:
        Results compressed;
        Result uncompressed;
    };


## Examples

    CompressorPtr PluginRegistry::choose()
    {
        float rating = powf( 1.0f, .3f );
        Plugin* candidate = nullptr;

        const Plugins& plugins = PluginFactory::getInstance().getPlugins();
        for( const Plugin* plugin : plugins )
        {
            for( const PluginInfo& info : plugin->getInfo( ))
            {
                float newRating = powf( info.speed, .3f ) / info.ratio;
                if( newRating > rating )
                {
                    candidate = plugin;
                    rating = newRating;
                }
            }
        }
        if( candidate )
            return candidate.construct()
        return nullptr;
    }

Compressor implementation:

    #include "zstd/lib/zstd.h"

    namespace pression
    {
    namespace plugin
    {
    namespace
    {
    static const bool LB_UNUSED _initialized = []()
    {
        PluginRegistry::registerEngine( { "pression::CompressorZSTD",
                                         .47f, .25f },
                                        CompressorZSTD::newInstance );
        return true;
    }();
    }

    const Results& CompressorZSTD::compress( const void* const data,
                                             const size_t inSize )
    {
        if( _results.empty( ))
            _results.push_back( new pression::plugin::Compressor::Result );
        _results[0]->reserve( ZSTD_compressBound( inSize ));

        const size_t size = ZSTD_compress( _results[0]->getData(),
                                           _results[0]->getSize(), inData,
                                            inSize, 2 /* level */ );
        _results[0]->setSize( size );
        return results;
    }

    const Result& CompressorZSTD::decompress( const Results& input,
                                              const size_t size );
    {
        if( input.empty( ))
            return;

        _result.resize( size );
        ZSTD_decompress( _result.getData(), _result.getSize(),
                         input[0].getData(), input[0].getSize( ));
    }
    }
    }

## Issues

### Issue 1: How do we select compression engines?

_Resolution: Make InitDataT& parameter given to handles() mutable_

In the current Lunchbox plugin API, the plugin decides if it can handle
a given request. Based on the return value of handles(), the first
plugin available is typically instantiated. This self-decision process
is not applicable to this spec, since the decision can only be made by
an outside entity. We do have the following options to acquire the
necessary information:

* Extend handles() with additional, mutable Information parameter
* Make current InitDataT parameter mutable
* Introduce new getInformation() function
* Provide information at plugin registration time

Making the templated InitDataT& given to the static handles() function
mutable will allow our implementation to both return true/false if the
plugin is compatible, and to fill in the plugin information into the
given init data. Based on this information, Pression can select all
plugins for a given request, and then select the best of the compatible
ones. The lunchbox::PluginFactory needs to be introspectable.

A given plugin DSO needs to fill in one information structure for each
contained engine.
