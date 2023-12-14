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
    struct Rock
    {
        int x;
        int y;
    };

    bool operator==( Rock const& lhs, Rock const& rhs )
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    bool operator<( Rock const& lhs, Rock const& rhs )
    {
        if( lhs.x != rhs.x )
        {
            return lhs.x < rhs.x;
        }

        return lhs.y < rhs.y;
    }

    struct Grid
    {
        int width;
        int height;
        std::vector< std::string > rows;
        std::vector< Rock > rocks;

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
            for( int x = 0; x < width; ++x )
            {
                for( int y = 0; y < height; ++y )
                {
                    if( value( x, y ) == 'O' )
                    {
                        rocks.push_back( Rock{ x, y } );
                    }
                }
            }
        }

        void fixRock( int x, int y )
        {
            rocks.erase( std::remove( std::begin( rocks ), std::end( rocks ), Rock{ x, y } ),
                         std::end( rocks ) );

            value( x, y ) = 'U';
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

    void shiftRocks( Grid& grid, int dx, int dy );

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

    grid.print();

    auto prevRocks = std::vector< std::vector< Rock > >{};

    auto finished = false;
    for( int i = 0; i < 1000000000; ++i )
    {
        shiftRocks( grid, 0, -1 );
        shiftRocks( grid, -1, 0 );
        shiftRocks( grid, 0, 1 );
        shiftRocks( grid, 1, 0 );

        for( auto j = 0; j < prevRocks.size(); ++j )
        {
            auto const& r = prevRocks[ prevRocks.size() - 1 - j ];
            auto rockdiff = std::vector< Rock >{};
            std::set_intersection( std::begin( grid.rocks ),
                                   std::end( grid.rocks ),
                                   std::begin( r ),
                                   std::end( r ),
                                   std::back_inserter( rockdiff ) );

            if( rockdiff.empty() )
            {
                fmt::print( "Finished after {} {}\n", i, j );
                finished = true;
                break;
            }
            else
            {
                fmt::print( "Diff: {}\n", rockdiff.size() );
            }
        }

        if( finished )
        {
            break;
        }

        prevRocks.push_back( grid.rocks );
        grid.print();

        // std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

        fmt::print( "{} {}\n", i, prevRocks.size() );
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

    void shiftRocks( Grid& grid, int dx, int dy )
    {
        std::sort( std::begin( grid.rocks ),
                   std::end( grid.rocks ),
                   [ & ]( auto const& lhs, auto const& rhs )
                   {
                       return lhs.x * dx + lhs.y * dy > rhs.x * dx + rhs.y * dy;
                   } );

        for( auto& r : grid.rocks )
        {
            auto x = r.x;
            auto y = r.y;

            auto minDist = 0;
            auto nx = x + dx;
            auto ny = y + dy;
            while( true )
            {
                if( nx < 0 || nx >= grid.width || ny < 0 || ny >= grid.height )
                {
                    break;
                }

                if( grid.value( nx, ny ) == '.' )
                {
                    ++minDist;
                }
                else
                {
                    break;
                }

                nx += dx;
                ny += dy;
            }

            grid.value( x, y ) = '.';
            grid.value( x + dx * minDist, y + dy * minDist ) = 'O';
            r.x = x + dx * minDist;
            r.y = y + dy * minDist;
        }
    }

    long computeLoad( Grid const& grid )
    {
        auto load = 0L;

        for( int y = 0; y < grid.height; ++y )
        {
            for( int x = 0; x < grid.width; ++x )
            {
                if( grid.value( x, y ) == 'O' || grid.value( x, y ) == 'U' )
                {
                    load += grid.height - y;
                }
            }
        }

        return load;
    }
}
