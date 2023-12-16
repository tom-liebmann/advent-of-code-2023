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
#include <queue>
#include <regex>
#include <set>
#include <thread>
#include <unordered_set>

#include <omp.h>

#include <utils.hpp>


namespace
{
    enum class Dir : std::uint8_t
    {
        RIGHT = 1,
        UP = 2,
        LEFT = 4,
        DOWN = 8,
    };

    struct Grid
    {
        int width;
        int height;
        std::vector< std::string > rows;

        std::vector< bool > energized;
        std::vector< std::uint8_t > visited;

        bool inGrid( int x, int y ) const
        {
            return x >= 0 && x < width && y >= 0 && y < height;
        }

        std::size_t idx( int x, int y ) const
        {
            return x + y * width;
        }

        Grid( std::vector< std::string > rows )
            : width{ static_cast< int >( rows[ 0 ].size() ) }
            , height{ static_cast< int >( rows.size() ) }
            , rows{ std::move( rows ) }
        {
            energized.resize( width * height, false );
            visited.resize( width * height, 0 );
        }

        static Grid load( std::istream& stream );
    };
}


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 46 },
    { "input_final.txt", 7788 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto grid = Grid::load( inputStream );

    struct Tile
    {
        Dir dir;
        int x;
        int y;
    };

    auto tileQueue = std::queue< Tile >{};
    tileQueue.push( Tile{ Dir::RIGHT, 0, 0 } );

    while( !tileQueue.empty() )
    {
        auto const tile = tileQueue.front();
        tileQueue.pop();

        if( !grid.inGrid( tile.x, tile.y ) )
        {
            continue;
        }

        if( grid.visited[ grid.idx( tile.x, tile.y ) ] & static_cast< std::uint8_t >( tile.dir ) )
        {
            continue;
        }

        grid.energized[ grid.idx( tile.x, tile.y ) ] = true;
        grid.visited[ grid.idx( tile.x, tile.y ) ] |= static_cast< std::uint8_t >( tile.dir );

        auto const value = grid.rows[ tile.y ][ tile.x ];

        switch( tile.dir )
        {
            case Dir::RIGHT:
                if( value == '-' || value == '.' )
                    tileQueue.push( Tile{ tile.dir, tile.x + 1, tile.y } );
                if( value == '/' || value == '|' )
                    tileQueue.push( Tile{ Dir::UP, tile.x, tile.y - 1 } );
                if( value == '\\' || value == '|' )
                    tileQueue.push( Tile{ Dir::DOWN, tile.x, tile.y + 1 } );
                break;
            case Dir::UP:
                if( value == '|' || value == '.' )
                    tileQueue.push( Tile{ tile.dir, tile.x, tile.y - 1 } );
                if( value == '\\' || value == '-' )
                    tileQueue.push( Tile{ Dir::LEFT, tile.x - 1, tile.y } );
                if( value == '/' || value == '-' )
                    tileQueue.push( Tile{ Dir::RIGHT, tile.x + 1, tile.y } );
                break;
            case Dir::LEFT:
                if( value == '-' || value == '.' )
                    tileQueue.push( Tile{ tile.dir, tile.x - 1, tile.y } );
                if( value == '\\' || value == '|' )
                    tileQueue.push( Tile{ Dir::UP, tile.x, tile.y - 1 } );
                if( value == '/' || value == '|' )
                    tileQueue.push( Tile{ Dir::DOWN, tile.x, tile.y + 1 } );
                break;
            case Dir::DOWN:
                if( value == '|' || value == '.' )
                    tileQueue.push( Tile{ tile.dir, tile.x, tile.y + 1 } );
                if( value == '/' || value == '-' )
                    tileQueue.push( Tile{ Dir::LEFT, tile.x - 1, tile.y } );
                if( value == '\\' || value == '-' )
                    tileQueue.push( Tile{ Dir::RIGHT, tile.x + 1, tile.y } );
                break;
        }
    }

    auto sum = 0L;
    for( int y = 0; y < grid.height; ++y )
    {
        for( int x = 0; x < grid.width; ++x )
        {
            if( grid.energized[ grid.idx( x, y ) ] )
            {
                ++sum;
            }
        }
    }

    return sum;
}

namespace
{
    Grid Grid::load( std::istream& stream )
    {
        auto rows = std::vector< std::string >{};
        iterateLines( stream,
                      [ & ]( std::string const& line )
                      {
                          rows.push_back( line );
                      } );
        return { std::move( rows ) };
    }
}
