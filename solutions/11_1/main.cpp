#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <regex>
#include <set>
#include <unordered_set>


#include <stream_utils.hpp>
#include <string_utils.hpp>


namespace
{
    struct Galaxy
    {
        long x;
        long y;
    };

    struct Map
    {
        std::set< long > emptyRows;
        std::set< long > emptyCols;
        std::vector< Galaxy > galaxies;

        long computeDistance( Galaxy const& lhs, Galaxy const& rhs ) const;

        static Map parse( std::istream& stream );
    };
}


int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        std::cerr << "Missing parameter: <input file>\n";
        return EXIT_FAILURE;
    }

    auto const fileName = std::string{ argv[ 1 ] };
    auto fileStream = std::ifstream{ fileName };

    auto const map = Map::parse( fileStream );

    auto sum = 0L;
    for( auto i = 0; i < map.galaxies.size(); ++i )
    {
        for( auto j = i + 1; j < map.galaxies.size(); ++j )
        {
            sum += map.computeDistance( map.galaxies[ i ], map.galaxies[ j ] );
        }
    }

    fmt::print( "Solution: {}\n", sum );

    return EXIT_SUCCESS;
}

namespace
{
    Map Map::parse( std::istream& stream )
    {
        auto rows = std::vector< std::string >{};
        iterateLines( stream,
                      [ & ]( auto const& line )
                      {
                          rows.push_back( line );
                      } );

        auto const width = rows[ 0 ].size();
        auto const height = rows.size();

        auto galaxies = std::vector< Galaxy >{};
        for( auto y = 0; y < height; ++y )
        {
            for( auto x = 0; x < width; ++x )
            {
                if( rows[ y ][ x ] == '#' )
                {
                    galaxies.push_back( Galaxy{ x, y } );
                }
            }
        }

        auto emptyRows = std::set< long >{};
        for( auto y = 0; y < height; ++y )
        {
            auto empty = true;
            for( auto x = 0; x < width; ++x )
            {
                if( rows[ y ][ x ] != '.' )
                {
                    empty = false;
                    break;
                }
            }
            if( empty )
            {
                emptyRows.insert( y );
            }
        }

        auto emptyCols = std::set< long >{};
        for( auto x = 0; x < width; ++x )
        {
            auto empty = true;
            for( auto y = 0; y < height; ++y )
            {
                if( rows[ y ][ x ] != '.' )
                {
                    empty = false;
                    break;
                }
            }
            if( empty )
            {
                emptyCols.insert( x );
            }
        }

        return Map{ std::move( emptyRows ), std::move( emptyCols ), std::move( galaxies ) };
    }

    long Map::computeDistance( Galaxy const& lhs, Galaxy const& rhs ) const
    {
        auto dist = 0L;

        for( auto x = std::min( lhs.x, rhs.x ); x < std::max( lhs.x, rhs.x ); ++x )
        {
            ++dist;
            if( emptyCols.contains( x ) )
            {
                ++dist;
            }
        }

        for( auto y = std::min( lhs.y, rhs.y ); y < std::max( lhs.y, rhs.y ); ++y )
        {
            ++dist;
            if( emptyRows.contains( y ) )
            {
                ++dist;
            }
        }

        return dist;
    }
}
