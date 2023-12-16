#include <algorithm>
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

    long computeEnergized( Grid< char > const& grid, Dir dir, int x, int y );
}


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 51 },
    { "input_final.txt", 7987 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto const grid = readGrid( inputStream );

    auto max = 0L;

    for( int i = 0; i < grid.getWidth(); ++i )
    {
        max = std::max( max, computeEnergized( grid, Dir::DOWN, i, 0 ) );
        max = std::max( max, computeEnergized( grid, Dir::UP, i, grid.getHeight() - 1 ) );
    }

    for( int i = 0; i < grid.getHeight(); ++i )
    {
        max = std::max( max, computeEnergized( grid, Dir::RIGHT, 0, i ) );
        max = std::max( max, computeEnergized( grid, Dir::LEFT, grid.getWidth() - 1, i ) );
    }

    return max;
}

namespace
{
    long computeEnergized( Grid< char > const& grid, Dir dir, int x, int y )
    {
        static auto const NEXT_DIRS = std::map< Dir, std::map< char, std::vector< Dir > > >{
            { Dir::RIGHT,
              {
                  { '-', { Dir::RIGHT } },
                  { '|', { Dir::UP, Dir::DOWN } },
                  { '.', { Dir::RIGHT } },
                  { '/', { Dir::UP } },
                  { '\\', { Dir::DOWN } },
              } },
            { Dir::UP,
              {
                  { '-', { Dir::LEFT, Dir::RIGHT } },
                  { '|', { Dir::UP } },
                  { '.', { Dir::UP } },
                  { '/', { Dir::RIGHT } },
                  { '\\', { Dir::LEFT } },
              } },
            { Dir::LEFT,
              {
                  { '-', { Dir::LEFT } },
                  { '|', { Dir::UP, Dir::DOWN } },
                  { '.', { Dir::LEFT } },
                  { '/', { Dir::DOWN } },
                  { '\\', { Dir::UP } },
              } },
            { Dir::DOWN,
              {
                  { '-', { Dir::LEFT, Dir::RIGHT } },
                  { '|', { Dir::DOWN } },
                  { '.', { Dir::DOWN } },
                  { '/', { Dir::LEFT } },
                  { '\\', { Dir::RIGHT } },
              } },
        };

        auto energized = Grid< std::uint8_t >{ grid.getWidth(), grid.getHeight(), false };
        auto visited = Grid< std::uint8_t >{ grid.getWidth(), grid.getHeight(), 0 };

        struct Tile
        {
            Dir dir;
            int x;
            int y;
        };

        auto tileQueue = std::queue< Tile >{};
        tileQueue.push( Tile{ dir, x, y } );

        while( !tileQueue.empty() )
        {
            auto const tile = tileQueue.front();
            tileQueue.pop();

            if( !grid.isInside( tile.x, tile.y ) )
            {
                continue;
            }

            if( visited( tile.x, tile.y ) & static_cast< std::uint8_t >( tile.dir ) )
            {
                continue;
            }

            energized( tile.x, tile.y ) = true;
            visited( tile.x, tile.y ) |= static_cast< std::uint8_t >( tile.dir );

            auto const value = grid( tile.x, tile.y );

            for( auto const nextDir : NEXT_DIRS.at( tile.dir ).at( value ) )
            {
                switch( nextDir )
                {
                    case Dir::RIGHT:
                        tileQueue.push( Tile{ nextDir, tile.x + 1, tile.y } );
                        break;
                    case Dir::UP:
                        tileQueue.push( Tile{ nextDir, tile.x, tile.y - 1 } );
                        break;
                    case Dir::LEFT:
                        tileQueue.push( Tile{ nextDir, tile.x - 1, tile.y } );
                        break;
                    case Dir::DOWN:
                        tileQueue.push( Tile{ nextDir, tile.x, tile.y + 1 } );
                        break;
                }
            }
        }

        return std::ranges::count( energized.getValues(), true );
    }
}
