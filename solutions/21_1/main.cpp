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
    };

    Pos findStart( Grid< char > const& grid )
    {
        for( auto const [ x, y, v ] : grid.getElements() )
        {
            if( v == 'S' )
            {
                return { x, y };
            }
        }

        throw std::runtime_error( "No start" );
    }
}


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 51 },
    { "input_final.txt", 7987 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto const grid = readGrid( inputStream );
    auto distances =
        Grid< long >{ grid.getWidth(), grid.getHeight(), std::numeric_limits< long >::max() };


    struct Node
    {
        int x;
        int y;
        long dist;
    };

    auto const start = findStart( grid );

    auto current = std::queue< Node >{};

    current.push( { start.x, start.y, 0 } );

    while( !current.empty() )
    {
        auto const c = current.front();
        current.pop();

        if( !grid.isInside( c.x, c.y ) || grid( c.x, c.y ) == '#' ||
            distances( c.x, c.y ) <= c.dist )
        {
            continue;
        }

        distances( c.x, c.y ) = c.dist;

        current.push( { c.x - 1, c.y, c.dist + 1 } );
        current.push( { c.x + 1, c.y, c.dist + 1 } );
        current.push( { c.x, c.y - 1, c.dist + 1 } );
        current.push( { c.x, c.y + 1, c.dist + 1 } );
    }

    auto reachable = 0L;

    for( auto const [ x, y, v ] : distances.getElements() )
    {
        if( grid( x, y ) != '#' && v <= 64 && v % 2 == 0 )
        {
            ++reachable;
        }
    }

    return reachable;
}


namespace
{
}
