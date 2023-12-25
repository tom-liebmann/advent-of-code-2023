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
    struct Pos
    {
        int x;
        int y;

        bool operator==( Pos const& rhs ) const
        {
            return x == rhs.x && y == rhs.y;
        }
    };

    std::vector< Pos >
        computeLongestPath( Grid< char > const& grid, Pos const& start, Pos const& target );
}

template <>
struct std::hash< Pos >
{
    std::size_t operator()( Pos const& pos ) const noexcept
    {
        auto hasher = HashComputer{};
        hasher.push( pos.x );
        hasher.push( pos.y );
        return hasher.getValue();
    }
};


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 51 },
    { "input_final.txt", 7987 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto const grid = readGrid( inputStream );
    auto const path =
        computeLongestPath( grid, Pos{ 1, 0 }, Pos{ grid.getWidth() - 2, grid.getHeight() - 1 } );

    auto outGrid = grid;
    for( auto const& pos : path )
    {
        fmt::print( "Pos: {} {}\n", pos.x, pos.y );
        outGrid( pos.x, pos.y ) = 'O';
    }

    for( int y = 0; y < outGrid.getHeight(); ++y )
    {
        for( int x = 0; x < outGrid.getWidth(); ++x )
        {
            fmt::print( "{}", outGrid( x, y ) );
        }
        fmt::print( "\n" );
    }

    return path.size() - 1;
}


namespace
{
    void computeLongestPath( Grid< char > const& grid,
                             Pos const& target,
                             std::unordered_set< Pos >& visited,
                             std::vector< Pos >& path )
    {
        while( true )
        {
            auto const& current = path.back();

            fmt::print( "Current: {} {}\n", current.x, current.y );
            if( current == target )
            {
                return;
            }

            auto neighbors = std::vector< Pos >{};

            if( grid.isInside( current.x - 1, current.y ) &&
                ( grid( current.x - 1, current.y ) == '.' ||
                  grid( current.x - 1, current.y ) == '<' ) &&
                !visited.contains( Pos{ current.x - 1, current.y } ) )
            {
                neighbors.push_back( Pos{ current.x - 1, current.y } );
            }

            if( grid.isInside( current.x + 1, current.y ) &&
                ( grid( current.x + 1, current.y ) == '.' ||
                  grid( current.x + 1, current.y ) == '>' ) &&
                !visited.contains( Pos{ current.x + 1, current.y } ) )
            {
                neighbors.push_back( Pos{ current.x + 1, current.y } );
            }

            if( grid.isInside( current.x, current.y - 1 ) &&
                ( grid( current.x, current.y - 1 ) == '.' ||
                  grid( current.x, current.y - 1 ) == '^' ) &&
                !visited.contains( Pos{ current.x, current.y - 1 } ) )
            {
                neighbors.push_back( Pos{ current.x, current.y - 1 } );
            }

            if( grid.isInside( current.x, current.y + 1 ) &&
                ( grid( current.x, current.y + 1 ) == '.' ||
                  grid( current.x, current.y + 1 ) == 'v' ) &&
                !visited.contains( Pos{ current.x, current.y + 1 } ) )
            {
                neighbors.push_back( Pos{ current.x, current.y + 1 } );
            }

            if( neighbors.size() == 1 )
            {
                path.push_back( neighbors[ 0 ] );
                visited.insert( neighbors[ 0 ] );
            }
            else
            {
                auto bestPath = std::vector< Pos >{};

                for( auto const& neighbor : neighbors )
                {
                    auto newVisited = visited;
                    auto newPath = path;
                    newVisited.insert( neighbor );
                    newPath.push_back( neighbor );

                    computeLongestPath( grid, target, newVisited, newPath );

                    if( newPath.back() == target && newPath.size() > bestPath.size() )
                    {
                        bestPath = newPath;
                    }
                }

                path = bestPath;

                return;
            }
        }
    }

    std::vector< Pos >
        computeLongestPath( Grid< char > const& grid, Pos const& start, Pos const& target )
    {
        auto visited = std::unordered_set< Pos >{};
        auto path = std::vector< Pos >{};

        path.push_back( start );
        visited.insert( start );

        computeLongestPath( grid, target, visited, path );

        return path;
    }
}
