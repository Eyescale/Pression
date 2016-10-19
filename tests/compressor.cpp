
/* Copyright (c) 2010-2016, Cedric Stalder <cedric.stalder@gmail.com>
 *                          Stefan Eilemann <eile@eyescale.ch>
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

#define TEST_RUNTIME 600 // seconds
#include <lunchbox/test.h>

#include <pression/compressor.h>
#include <pression/compressorInfo.h>
#include <pression/compressorResult.h>
#include <pression/decompressor.h>
#include <pression/plugin.h>
#include <pression/pluginRegistry.h>

#include <lunchbox/buffer.h>
#include <lunchbox/clock.h>
#include <lunchbox/file.h>
#include <lunchbox/memoryMap.h>
#include <lunchbox/rng.h>

#include <algorithm>

using namespace pression;

void _testFile();
void _testRandom();
void _testData( const uint32_t nameCompressor, const std::string& name,
                const uint8_t* data, const uint64_t size );

Strings getFiles( Strings& files, const std::string& ext );

PluginRegistry& registry = PluginRegistry::getInstance();
uint64_t _result = 0;
uint64_t _size = 0;
float _compressionTime = 0;
float _decompressionTime = 0;
float _baseTime = 0.f;

int main( int, char** )
{
    registry.loadDirectory( std::string( PRESSION_BUILD_DIR ) + "/lib" );
    registry.loadDirectory( "../bin" );
    registry.loadDirectory( "../lib" );
    registry.loadDirectory( "../../install/bin" );
    registry.loadDirectory( "../../install/lib" );

    _testFile();
    _testRandom();

    Compressor compressor;
    TEST( !compressor.isGood( ));
    TEST( !compressor );

    Decompressor decompressor;
    TEST( !decompressor.isGood( ));
    TEST( !decompressor );
    return EXIT_SUCCESS;
}

std::vector< uint32_t > getCompressorNames( const uint32_t tokenType )
{
    const Plugins& plugins = registry.getPlugins();
    std::vector< uint32_t > names;

    for( PluginsCIter i = plugins.begin(); i != plugins.end(); ++i )
    {
        const CompressorInfos& infos = (*i)->getInfos();
        for( CompressorInfosCIter j = infos.begin(); j != infos.end(); ++j )
        {
            if ( (*j).tokenType == tokenType )
                names.push_back( (*j).name );
        }
    }

    std::sort( names.begin(), names.end( ));
    return names;
}

void _testData( const uint32_t compressorName, const std::string& name,
                const uint8_t* data, const uint64_t size )
{
    Compressor compressor( compressorName );
    Decompressor decompressor( compressorName );
    TEST( compressor.isGood( ));
    TEST( compressor );
    TESTINFO( decompressor.isGood(), compressorName );
    TESTINFO( decompressor, compressorName );

    const uint64_t flags = EQ_COMPRESSOR_DATA_1D;
    uint64_t inDims[2]  = { 0, size };

    compressor.compress( const_cast<uint8_t*>(data), inDims, flags );
    lunchbox::Clock clock;
    compressor.compress( const_cast<uint8_t*>(data), inDims, flags );
    const float compressTime = clock.getTimef();

    const CompressorResult& compressed = compressor.getResult();
    const uint64_t compressedSize = compressed.getSize();
    std::vector< void * > chunks;
    std::vector< uint64_t > chunkSizes;
    chunks.resize( compressed.chunks.size( ));
    chunkSizes.resize( chunks.size( ));

    for( unsigned i = 0; i < chunks.size(); ++i )
    {
        chunks[ i ] = compressed.chunks[i].data;
        chunkSizes[ i ] =  compressed.chunks[i].getNumBytes();
    }

    lunchbox::Bufferb result;
    result.resize( size );
    uint8_t* outData = result.getData();

    decompressor.decompress( &chunks.front(), &chunkSizes.front(),
                             unsigned( chunks.size( )), outData, inDims );

    clock.reset();
    decompressor.decompress( &chunks.front(), &chunkSizes.front(),
                             unsigned(chunks.size()), outData, inDims);
    const float decompressTime = clock.getTimef();

    TEST( memcmp( outData, data, size ) == 0 );
    std::cout  << std::setw(20) << name << ", 0x" << std::setw(8)
               << std::setfill( '0' ) << std::hex << compressorName << std::dec
               << std::setfill(' ') << ", " << std::setw(10) << size << ", "
               << std::setw(10) << compressedSize << ", " << std::setw(10)
               << float(size) / 1024.f / 1024.f * 1000.f / compressTime
               << ", " << std::setw(10)
               << float(size) / 1024.f / 1024.f * 1000.f / decompressTime
               << std::endl;
    _size += size;
    _result += compressedSize;
    _compressionTime += compressTime;
    _decompressionTime += decompressTime;
}

void _testFile()
{
    std::vector< uint32_t > compressorNames =
        getCompressorNames( EQ_COMPRESSOR_DATATYPE_BYTE );

    std::vector< std::string > files;

    getFiles( files, ".*\\.dll" );
    getFiles( files, ".*\\.exe" );
    getFiles( files, ".*\\.so" );
    getFiles( files, ".*\\.a" );
    getFiles( files, ".*\\.dylib" );
    getFiles( files, ".*\\.rgb" );
    getFiles( files, ".*\\.bin" );
    getFiles( files, ".*\\.ply" );
    getFiles( files, ".*\\.bbp" );

#if 1
    // Limit to 30 files using a pseudo-random selection for reproducability
    const size_t maxFiles = 30;
    if( files.size() > maxFiles )
    {
        const size_t cut = files.size() - maxFiles;
        for( size_t i = 0; i < cut; ++i )
            files.erase( files.begin() + (i * 997 /*prime*/) % files.size( ));
    }
#endif

    std::cout.setf( std::ios::right, std::ios::adjustfield );
    std::cout.precision( 5 );
    std::cout << "                File, Compressor, Uncompress, "
              << "Compressed,   comp MB/s, decomp MB/s" << std::endl;
    for( std::vector< uint32_t >::const_iterator i = compressorNames.begin();
         i != compressorNames.end(); ++i )
    {
        _result = 0;
        _size = 0;
        _compressionTime = 0;
        _decompressionTime = 0;

        for( StringsCIter j = files.begin(); j != files.end(); ++j )
        {
            lunchbox::MemoryMap file;
            const uint8_t* data = static_cast<const uint8_t*>( file.map( *j ));

            if( !data )
            {
                LBERROR << "Can't mmap " << *j << std::endl;
                continue;
            }

            const size_t size = file.getSize();
            if( size > LB_1GB )
                continue;

            const std::string name = lunchbox::getFilename( *j );

            _testData( *i, name, data, size );
        }
        if( _baseTime == 0.f )
            _baseTime = _compressionTime + _decompressionTime;

        std::cout << std::setw(24) << "Total, 0x" << std::setw(8)
                  << std::setfill( '0' ) << std::hex << *i << std::dec
                  << std::setfill(' ') << ", " << std::setw(10) << _size << ", "
                  << std::setw(10) << _result << ", " << std::setw(10)
                  << float(_size) / 1024.f / 1024.f * 1000.f / _compressionTime
                  << ", " << std::setw(10)
                  << float(_size) / 1024.f / 1024.f * 1000.f /_decompressionTime
                  << std::endl
                  << "    info->ratio = " << float(_result) / float(_size)
                  << "f;" << std::endl
                  << "    info->speed = " << float(_baseTime) /
                                    float(_compressionTime + _decompressionTime)
                  << "f;" << std::endl << std::endl;
    }
}

void _testRandom()
{
    ssize_t size = LB_10MB;
    uint8_t* data = new uint8_t[size];
    lunchbox::RNG rng;
#pragma omp parallel for
    for( ssize_t i = 0; i < size; ++i )
        data[i] = rng.get< uint8_t >();

    std::vector< uint32_t >compressorNames =
        getCompressorNames( EQ_COMPRESSOR_DATATYPE_BYTE );
    _result = 0;
    _size = 0;
    _compressionTime = 0;
    _decompressionTime = 0;

    for( std::vector<uint32_t>::const_iterator i = compressorNames.begin();
         i != compressorNames.end(); ++i )
    {
        size = LB_10MB;
        for( size_t j = 0; j<8; ++j ) // test all granularities between mod 8..1
        {
            _testData( *i, "Random data", data, size );
            --size;
        }
        std::cout << std::setw(24) << "Total, 0x" << std::setw(8)
                  << std::setfill( '0' ) << std::hex << *i << std::dec
                  << std::setfill(' ') << ", " << std::setw(10) << _size << ", "
                  << std::setw(10) << _result << ", " << std::setw(10)
                  << float(_size) / 1024.f / 1024.f * 1000.f / _compressionTime
                  << ", " << std::setw(10)
                  << float(_size) / 1024.f / 1024.f * 1000.f /_decompressionTime
                  << std::endl;
   }

    delete [] data;
}

Strings getFiles( Strings& files, const std::string& ext )
{
    const Strings paths = {
        { std::string( PRESSION_BUILD_DIR ) + "/lib" },
        { "../bin" },
        { "../lib" },
        { "../../install/bin" },
        { "../../install/lib" },
        { "images" },
        { "/nfs4/bbp.epfl.ch/visualization/resources/meshes/mediumPly/" },
        { "/nfs4/bbp.epfl.ch/visualization/circuits/KaustCircuit/meshes" },
        { "/nfs4/bbp.epfl.ch/visualization/circuits/KaustCircuit/simulations/run_1k/20.02.13/" }};

    for( const auto& path : paths )
        for( const auto& file : lunchbox::searchDirectory( path, ext ))
            files.push_back( path + '/' + file );
    return files;
}
