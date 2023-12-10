#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <regex>
#include <unordered_set>


#include <stream_utils.hpp>
#include <string_utils.hpp>


namespace
{
    struct Map
    {
        int width;
        int height;
        std::vector< std::string > rows;

        char getTile( int x, int y ) const
        {
            return rows.at( y )[ x ];
        }

        char getTileType( int x, int y ) const;

        std::pair< int, int > findStart() const;

        long computePathLen() const;
    };

    Map parseMap( std::istream& stream );
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

    auto const map = parseMap( fileStream );

    auto const pathLen = map.computePathLen();

    fmt::print( "Pathlen: {}\n", ( pathLen + 1 ) / 2 );

    return EXIT_SUCCESS;
}

namespace
{
    Map parseMap( std::istream& stream )
    {
        auto rows = std::vector< std::string >{};

        iterateLines( stream,
                      [ & ]( auto const& line )
                      {
                          rows.push_back( line );
                      } );

        return { rows[ 0 ].length(), rows.size(), rows };
    }

    std::pair< int, int > Map::findStart() const
    {
        for( int y = 0; y < height; ++y )
        {
            for( int x = 0; x < width; ++x )
            {
                if( getTile( x, y ) == 'S' )
                    return std::make_pair( x, y );
            }
        }

        throw std::runtime_error( "No start found" );
    }

    long Map::computePathLen() const
    {
        auto pathLen = 0L;
        auto [ x, y ] = findStart();

        auto lastDir = 0;
        switch( getTileType( x, y ) )
        {
            case '|':
                --y;
                lastDir = 2;
                break;
            case '-':
                ++x;
                lastDir = 0;
                break;
            case 'L':
                ++x;
                lastDir = 0;
                break;
            case 'J':
                --y;
                lastDir = 2;
                break;
            case '7':
                ++y;
                lastDir = 3;
                break;
            case 'F':
                ++y;
                lastDir = 3;
                break;
        }

        do
        {
            fmt::print( "Current: {} {}\n", x, y );
            auto const tileType = getTileType( x, y );

            auto newDir = lastDir;

            switch( tileType )
            {
                case '|':
                    newDir = lastDir == 1 ? 1 : 3;
                    break;
                case '-':
                    newDir = lastDir == 0 ? 0 : 2;
                    break;
                case 'L':
                    newDir = lastDir == 2 ? 1 : 0;
                    break;
                case 'J':
                    newDir = lastDir == 3 ? 2 : 1;
                    break;
                case '7':
                    newDir = lastDir == 0 ? 3 : 2;
                    break;
                case 'F':
                    newDir = lastDir == 2 ? 3 : 0;
                    break;
            }

            switch( newDir )
            {
                case 0:
                    ++x;
                    break;
                case 1:
                    --y;
                    break;
                case 2:
                    --x;
                    break;
                case 3:
                    ++y;
                    break;
            }

            lastDir = newDir;
            ++pathLen;
        } while( getTile( x, y ) != 'S' );

        return pathLen;
    }

    char Map::getTileType( int x, int y ) const
    {
        auto const value = getTile( x, y );

        if( value == 'S' )
        {
            auto const top = y > 0 && ( getTile( x, y - 1 ) == '|' || getTile( x, y - 1 ) == '7' ||
                                        getTile( x, y - 1 ) == 'F' );
            auto const right =
                x < width - 1 && ( getTile( x + 1, y ) == '-' || getTile( x + 1, y ) == '7' ||
                                   getTile( x + 1, y ) == 'J' );
            auto const left = x > 0 && ( getTile( x - 1, y ) == '-' || getTile( x - 1, y ) == '7' ||
                                         getTile( x - 1, y ) == 'J' );
            auto const bottom =
                y < height - 1 && ( getTile( x, y + 1 ) == '|' || getTile( x, y + 1 ) == '7' ||
                                    getTile( x, y + 1 ) == 'F' );

            if( top && bottom )
            {
                return '|';
            }

            if( left && right )
            {
                return '-';
            }

            if( top && right )
            {
                return 'L';
            }

            if( top && left )
            {
                return 'J';
            }

            if( bottom && left )
            {
                return '7';
            }

            if( bottom && right )
            {
                return 'F';
            }
        }

        return value;
    }
}
