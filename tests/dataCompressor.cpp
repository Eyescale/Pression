
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

#include <pression/dataCompressor.h>
#include <pression/pluginRegistry.h>

#include <lunchbox/buffer.h>
#include <lunchbox/clock.h>
#include <lunchbox/file.h>
#include <lunchbox/memoryMap.h>
#include <lunchbox/rng.h>

#include <algorithm>

using lunchbox::Strings;

void _testFile();
void _testRandom();
void _testData( const std::string& name, uint8_t* data, uint64_t size );
Strings getFiles( Strings& files, const std::string& ext );

pression::PluginRegistry& registry = pression::PluginRegistry::getInstance();
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
    return EXIT_SUCCESS;
}

pression::DataCompressorInfos getCompressors()
{
    auto infos = registry.getDataCompressorInfos();
    for( auto i = infos.begin(); i != infos.end(); ++i )
        if( i->name == "pression::CompressorRLEB" ) // move RLE to front
        {
            std::swap( *i, *infos.begin( ));
            return infos;
        }

    LBUNREACHABLE;
    return infos;
}

void _testData( const pression::DataCompressorInfo& info,
                const std::string& name,
                const uint8_t* data, const uint64_t size )
{
    std::unique_ptr< pression::DataCompressor > compressor( info.create( ));

    compressor->compress( data, size );
    lunchbox::Clock clock;
    const auto& compressed = compressor->compress( data, size );
    const float compressTime = clock.getTimef();

    uint64_t compressedSize = 0;
    std::vector< const void * > chunks( compressed.size( ));
    std::vector< uint64_t > chunkSizes( compressed.size( ));

    for( unsigned i = 0; i < chunks.size(); ++i )
    {
        chunks[ i ] = compressed[i].getData();
        chunkSizes[ i ] = compressed[i].getSize();
        compressedSize += compressed[i].getSize();
    }

    pression::DataCompressor::Result result( size );
    compressor->decompress( compressed, result.getData(), size );

    clock.reset();
    compressor->decompress( compressed, result.getData(), size );
    const float decompressTime = clock.getTimef();

    TEST( ::memcmp( result.getData(), data, size ) == 0 );
    std::cout  << std::setw(20) << name << ", " << info.name
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

    // Limit to 30 files using a pseudo-random selection for reproducability
    const size_t maxFiles = 30;
    if( files.size() > maxFiles )
    {
        const size_t cut = files.size() - maxFiles;
        for( size_t i = 0; i < cut; ++i )
            files.erase( files.begin() + (i * 997 /*prime*/) % files.size( ));
    }

    std::cout.setf( std::ios::right, std::ios::adjustfield );
    std::cout.precision( 5 );
    std::cout << "                File, Compressor, Uncompress, "
              << "Compressed,   comp MB/s, decomp MB/s" << std::endl;
    const auto& infos = getCompressors();
    for( const auto& info : infos )
    {
        _result = 0;
        _size = 0;
        _compressionTime = 0;
        _decompressionTime = 0;

        for( const auto& file : files )
        {
            lunchbox::MemoryMap map( file );
            const uint8_t* data = map.getAddress< const uint8_t >();
            const size_t size = map.getSize();

            if( !data || size > LB_1GB )
                continue;

            const std::string name = lunchbox::getFilename( file );
            _testData( info, name, data, size );
        }
        if( _baseTime == 0.f )
            _baseTime = _compressionTime + _decompressionTime;

        std::cout << std::setw(24) << "Total, " << info.name
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

    _result = 0;
    _size = 0;
    _compressionTime = 0;
    _decompressionTime = 0;

    const auto& infos = getCompressors();
    for( const auto& info : infos )
    {
        size = LB_10MB;
        for( size_t j = 0; j<8; ++j ) // test all granularities between mod 8..1
        {
            _testData( info, "Random data", data, size );
            --size;
        }
        std::cout << std::setw(22) << "Total, " << info.name
                  << std::setfill(' ') << ", " << std::setw(10) << _size
                  << ", " << std::setw(10) << _result << ", " << std::setw(10)
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
