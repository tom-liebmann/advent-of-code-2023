#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <regex>
#include <set>
#include <thread>
#include <unordered_set>

#include <omp.h>

#include <utils.hpp>


namespace
{
    enum class Direction
    {
        NORTH,
        WEST,
        SOUTH,
        EAST,
    };

    struct Grid
    {
        int width;
        int height;
        std::vector< std::string > rows;
        std::size_t hash;

        int getWidth( Direction dir ) const
        {
            switch( dir )
            {
                case Direction::NORTH:
                case Direction::SOUTH:
                    return width;
                case Direction::WEST:
                case Direction::EAST:
                    return height;
                default:
                    throw std::runtime_error( "Unhandled direction" );
            }
        }

        int getHeight( Direction dir ) const
        {
            switch( dir )
            {
                case Direction::NORTH:
                case Direction::SOUTH:
                    return height;
                case Direction::WEST:
                case Direction::EAST:
                    return width;
                default:
                    throw std::runtime_error( "Unhandled direction" );
            }
        }

        char getValue( int x, int y ) const
        {
            return rows[ y ][ x ];
        }

        void setValue( int x, int y, Direction dir, char value )
        {
            switch( dir )
            {
                case Direction::NORTH:
                    rows[ y ][ x ] = value;
                    break;
                case Direction::WEST:
                    rows[ x ][ y ] = value;
                    break;
                case Direction::SOUTH:
                    rows[ height - 1 - y ][ x ] = value;
                    break;
                case Direction::EAST:
                    rows[ x ][ height - 1 - y ] = value;
                    break;
                default:
                    throw std::runtime_error( "Unhandled direction" );
            }
        }

        char getValue( int x, int y, Direction dir ) const
        {
            switch( dir )
            {
                case Direction::NORTH:
                    return rows[ y ][ x ];
                case Direction::WEST:
                    return rows[ x ][ y ];
                case Direction::SOUTH:
                    return rows[ height - 1 - y ][ x ];
                case Direction::EAST:
                    return rows[ x ][ height - 1 - y ];
                default:
                    throw std::runtime_error( "Unhandled direction" );
            }
        }

        void moveRock( int ox, int oy, int nx, int ny, Direction dir )
        {
            setValue( ox, oy, dir, '.' );
            setValue( nx, ny, dir, 'O' );
        }

        Grid( std::vector< std::string > rowsV )
            : width{ rowsV[ 0 ].length() }, height{ rowsV.size() }, rows{ std::move( rowsV ) }
        {
            auto hasher = HashComputer{};
            for( int x = 0; x < width; ++x )
            {
                for( int y = 0; y < height; ++y )
                {
                    if( getValue( x, y ) == 'O' )
                    {
                        hasher.push( x );
                        hasher.push( y );
                    }
                }
            }
            hash = hasher.getValue();
        }

        void print() const
        {
            for( auto const& row : rows )
            {
                fmt::print( "{}\n", row );
            }
            fmt::print( "\n" );
        }
    };

    Grid loadGrid( std::istream& stream );

    void shiftRocks( Grid& grid, Direction dir, std::vector< int >& buffer );

    long computeLoad( Grid const& grid );
}


std::filesystem::path Application::getAppImplFile()
{
    return __FILE__;
}

std::unordered_map< std::string, long > Application::expectedResults()
{
    return {
        { "input_example_1.txt", 64 },
        { "input_final.txt", 100876 },
    };
}

long Application::computeResult( std::istream& inputStream )
{
    auto grid = loadGrid( inputStream );

    grid.print();

    auto rockPosCache = std::unordered_map< std::size_t, int >{};
    auto buffer = std::vector< int >{};

    auto finished = false;
    for( int i = 0; i < 1000000000; ++i )
    {
        shiftRocks( grid, Direction::NORTH, buffer );
        shiftRocks( grid, Direction::WEST, buffer );
        shiftRocks( grid, Direction::SOUTH, buffer );
        shiftRocks( grid, Direction::EAST, buffer );

        auto const previousVisit = rockPosCache.find( grid.hash );

        if( previousVisit != std::end( rockPosCache ) )
        {
            fmt::print( "Got loop: {} {}\n", i, previousVisit->second );
            auto const cycle = i - previousVisit->second;
            while( i + cycle < 1000000000 )
            {
                i += cycle;
            }
        }

        rockPosCache.insert( { grid.hash, i } );

        fmt::print( "{} {}\n", i, rockPosCache.size() );
    }

    return computeLoad( grid );
}

namespace
{
    Grid loadGrid( std::istream& stream )
    {
        auto rows = std::vector< std::string >{};
        iterateLines( stream,
                      [ & ]( auto const& line )
                      {
                          rows.push_back( line );
                      } );
        return Grid{ std::move( rows ) };
    }

    void shiftRocks( Grid& grid, Direction dir, std::vector< int >& buffer )
    {
        auto const width = grid.getWidth( dir );
        auto const height = grid.getHeight( dir );

        if( buffer.size() < width )
        {
            buffer.resize( width, 0 );
        }

        std::fill( std::begin( buffer ), std::end( buffer ), 0 );

        auto hasher = HashComputer{};

        for( int y = 0; y < height; ++y )
        {
            for( int x = 0; x < width; ++x )
            {
                auto const value = grid.getValue( x, y, dir );

                switch( value )
                {
                    case 'O':
                        grid.moveRock( x, y, x, y - buffer[ x ], dir );
                        hasher.push( x );
                        hasher.push( y - buffer[ x ] );
                        break;
                    case '.':
                        ++buffer[ x ];
                        break;
                    case '#':
                        buffer[ x ] = 0;
                        break;
                }
            }
        }

        grid.hash = hasher.getValue();
    }

    long computeLoad( Grid const& grid )
    {
        auto load = 0L;

        for( int y = 0; y < grid.height; ++y )
        {
            for( int x = 0; x < grid.width; ++x )
            {
                if( grid.getValue( x, y ) == 'O' || grid.getValue( x, y ) == 'U' )
                {
                    load += grid.height - y;
                }
            }
        }

        return load;
    }
}
