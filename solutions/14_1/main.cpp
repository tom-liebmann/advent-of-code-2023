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
#include <unordered_set>

#include <omp.h>

#include <utils.hpp>


namespace
{
    struct Grid
    {
        int width;
        int height;
        std::vector< std::string > rows;

        char& value( int x, int y )
        {
            return rows[ y ][ x ];
        }

        char value( int x, int y ) const
        {
            return rows[ y ][ x ];
        }

        Grid( std::vector< std::string > rowsV )
            : width{ rowsV[ 0 ].length() }, height{ rowsV.size() }, rows{ std::move( rowsV ) }
        {
        }
    };

    Grid loadGrid( std::istream& stream );

    void shiftRocks( Grid& grid );

    long computeLoad( Grid const& grid );
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


    auto grid = loadGrid( fileStream );

    shiftRocks( grid );

    for( auto const& row : grid.rows )
    {
        fmt::print( "{}\n", row );
    }

    auto const load = computeLoad( grid );

    fmt::print( "Load: {}\n", load );

    return EXIT_SUCCESS;
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

    void shiftRocks( Grid& grid )
    {
        for( int y = 0; y < grid.height; ++y )
        {
            for( int x = 0; x < grid.width; ++x )
            {
                if( grid.value( x, y ) != 'O' )
                {
                    continue;
                }

                auto minY = y;
                for( int ny = y - 1; ny >= 0; --ny )
                {
                    if( grid.value( x, ny ) == '.' )
                    {
                        minY = ny;
                    }
                    else
                    {
                        break;
                    }
                }

                grid.value( x, y ) = '.';
                grid.value( x, minY ) = 'O';
            }
        }
    }

    long computeLoad( Grid const& grid )
    {
        auto load = 0L;

        for( int y = 0; y < grid.height; ++y )
        {
            for( int x = 0; x < grid.width; ++x )
            {
                if( grid.value( x, y ) == 'O' )
                {
                    load += grid.height - y;
                }
            }
        }

        return load;
    }
}
