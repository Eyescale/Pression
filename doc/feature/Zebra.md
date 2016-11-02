Restriping the output of a compressor for storage
============

This document specifies the transformation of the output of a data
compressor into a smaller set of larger output chunks. The primary use
case is as a backend of the memcached keyv::Map, which has a maximum
value size of one megabyte.

## Requirements

* @subpage data
* Input: data::CompressorInfo, uncompressed data, max output chunk size
* Output: n output chunks of size <= max output chunk size
* Output of uncompressed data if data is uncompressible with zero-copy
  during compression and decompression

and

* Input: n output chunks
* Output: uncompressed data

## API

    namespace pression
    {
    namespace data
    {
    class Zebra
    {
        struct Result { uint8_t* data; uint32_t size; };
        typedef std::vector< Result > Results; //!< Set of result chunks
        typedef std::vector< uint32_t > ResultSizes; //!< Remaining chunk sizes

        Zebra( const CompressorInfo& compressor );

        // returned pointers are valid until next compress(), delete of
        // input data, or dtor of Zebra called
        Results&& compress( const uint8_t* data, size_t size,
                            uint32_t chunkSize );

        // input: first chunk
        ResultSizes&& getRemainingSizes( const uint8_t* data, uint32_t size );
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
* one zero-copy chunk if size <= chunkSize
* or:
    * one header chunk: 16 byte magic 'uncompressed', 8 byte input size,
      4 byte chunk size
    * n zero-copy output chunks of chunkSize, pointing to input data memory

Compressibly output is returned as:
* one header chunk: 16 byte magic 'compressed', 16 byte compressor name hash,
  8 byte input size, 4 byte nChunks, nChunks * 4 byte chunkSizes
* nChunks: 4 byte offset to next compressor chunk (or 0) + compressor chunk

First implementation throws if header size exceeds chunkSize for
compressed output.

## Examples

    void Keyv::memcached::Plugin::insert( const std::string& key,
                                          const void* ptr, const size_t size )
    {
        const auto data = _zebra.compress( ptr, size, LB_1MB );
        const std::string& hash = servus::make_uint128( key ).getString();

        for( const auto& chunk : data )
        {
            memcached_set( _instance, hash.c_str(), hash.length(),
                           chunk.data, chunk.size, (time_t)0, (uint32_t)0 );
            ++hash;
        }
    }

    std::string Keyv::memcached::Plugin::operator [] ( const std::string& key )
    {
        const std::string& hash = servus::make_uint128( key ).getString();
        pression::data::Zebra::Results chunks( 1 );
        chunks[0].data = memcached_get( _instance, hash.c_str(), hash.length(),
                                        &chunks[0].size );

        const auto remaining = _zebra.getRemainingSizes( chunk[0].data,
                                                         chunk[0].size );
        chunks.append( takeValues( hash, remaining ));

        std::string value( _zebra.getDecompressedSize( chunk[0].data,
                                                       chunk[0].size ));
        _zebra.decompress( chunks, value.data(), value.length( ));
        return value;
    }

## Issues

### Issue 1: What is the maximum allowed chunk size?

_Resolution: 4GB_

It is unlikely that a chunked storage system uses larger chunks.
Memcached has a limit of one megabyte.
