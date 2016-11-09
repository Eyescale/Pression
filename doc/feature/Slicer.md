Restriping the output of a compressor for storage
============

This document specifies the transformation of the output of a data
compressor into a smaller set of larger output slices. The primary use
case is as a backend of the memcached keyv::Map, which has a maximum
value size of one megabyte.

## Requirements

The new compression plugin API: @subpage data

For an input of:
* data::CompressorInfo
* uncompressed data
* max output slice size

The slicer produces:
* n output slices of size <= max output slice size
* Output of uncompressed data if data is uncompressible with zero-copy
  during compression and decompression

For an input of n output slices (see above), the slicer produces the
uncompressed data

## API

    namespace pression
    {
    namespace data
    {
    class Slicer
    {
        struct Result { uint8_t* data; uint32_t size; };
        typedef std::vector< Result > Results; //!< Set of result slices
        typedef std::vector< uint32_t > ResultSizes; //!< Remaining slice sizes

        Slicer( const CompressorInfo& compressor );

        // returned pointers are valid until next compress(), delete of
        // input data, or dtor of Slicer called
        Results&& compress( const uint8_t* data, size_t size,
                            uint32_t sliceSize );

        // input: first slice, output: remaining slice sizes
        ResultSizes&& getRemainingSizes( const uint8_t* data, uint32_t size );

        // input: first slice, output: total decompressed data size
        size_t getDecompressedSize( const uint8_t* data, uint32_t size );

        /** @overload convenience wrapper */
        void decompress( const Results& input, uint8_t* data );
     };
     }
     }

## Implementation

compress() allocates a compressor and compresses the input data. Output
is uncompressible if pression::getDataSize() exceeds input size minus
header overhead

Uncompressibly output is returned as:
* one zero-copy slice if size <= sliceSize
* or:
    * one header slice: 16 byte magic 'uncompressed', 8 byte input size,
      4 byte slice size
    * n zero-copy output slices of sliceSize, pointing to input data memory

Compressibly output is returned as:
* one header slice: 16 byte magic 'compressed', 16 byte compressor name hash,
  8 byte input size, 4 byte nChunks, nChunks * 4 byte chunkSizes
* nSlices: complete, compressed chunks up to sliceSize

First implementation throws if header size exceeds sliceSize for
compressed output and if a chunk is bigger than a slice.

## Examples

    void Keyv::memcached::Plugin::insert( const std::string& key,
                                          const void* ptr, const size_t size )
    {
        const auto data = _slicer.compress( ptr, size, LB_1MB );
        const std::string& hash = servus::make_uint128( key ).getString();

        for( const auto& slice : data )
        {
            ++hash;
            memcached_set( _instance, hash.c_str(), hash.length(),
                           slice.data, slice.size, (time_t)0, (uint32_t)0 );
        }
    }

    std::string Keyv::memcached::Plugin::operator [] ( const std::string& key )
    {
        const std::string& hash = servus::make_uint128( key ).getString();
        pression::data::Slicer::Results slices( 1 );
        slices[0].data = memcached_get( _instance, hash.c_str(), hash.length(),
                                        &slices[0].size );

        const auto remaining = _slicer.getRemainingSizes( slice[0].data,
                                                         slice[0].size );
        slices.append( takeValues( hash, remaining ));

        std::string value( _slicer.getDecompressedSize( slice[0].data,
                                                       slice[0].size ));
        _slicer.decompress( slices, value.data(), value.length( ));
        return value;
    }

## Issues

### Issue 1: What is the maximum allowed slice size?

_Resolution: 4GB_

It is unlikely that a storage system uses larger slices. Memcached has
a recommended limit of one megabyte.
