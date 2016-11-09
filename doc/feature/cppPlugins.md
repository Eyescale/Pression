C++ Plugin API for CPU compressors  {#data}
============

This document specifies a modern API for the in-memory C plugin API for
lossless byte data compressors. It will not replace the 2D image
compressors and CPU-GPU transfer plugins.

## Requirements

* Support all features used by current lossless byte compressors:
    * Lossless compression of binary blobs
    * Unified API for all compression engines
    * Other components can add compression engines
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

    class CompressorInfo
    {
    public:
        std::string name;
        float ratio; //!< Normalized 0..1 size after compression
        float speed; //!< Relative speed compared to RLE compressor

        typedef std::function< Compressor* ()> Constructor;
        Constructor create;
    };
    typedef std::vector< CompressorInfo > CompressorInfos;

    class Registry
    {
    public:
        static Registry& getInstance();
        template< class P > bool registerEngine( CompressorInfo info )

        const CompressorInfos& getInfos() const;
        CompressorInfo choose();
        CompressorInfo find( const std::string& name );
    };

    class Compressor
    {
    public:
        typedef lunchbox::Bufferb Result;
        typedef std::vector< Result > Results;

        virtual const Results& compress( const uint8_t* data, size_t size ) = 0;

        // data points to pre-allocated memory of size decompressed result
        virtual void decompress( const Results& input, uint8_t* data,
                                 size_t size ) = 0;
    protected:
        Results compressed;
    };


## Examples

    CompressorInfo Registry::choose()
    {
        CompressorInfo candidate;
        float rating = powf( 1.0f, .3f );

        for( const CompressorInfo& info : _impl->compressorInfos )
        {
            float newRating = powf( info.speed, .3f ) / info.ratio;
            if( newRating > rating )
    	{
                candidate = info;
                rating = newRating;
         	}
        }
        return candidate;
    }


Compressor implementation:

    #include "zstd/lib/zstd.h"

    namespace pression
    {
    namespace data
    {
    namespace
    {
    static const bool LB_UNUSED _initialized = []()
    {
        Registry::registerEngine( { "pression::CompressorZSTD",
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
        return _results;
    }

    void CompressorZSTD::decompress( const Results& input, uint8_t* data,
                                     size_t size );
    {
        if( input.empty( ))
            return;

        ZSTD_decompress( data, size,
                         input[0].getData(), input[0].getSize( ));
    }
    }
    }

## Issues
