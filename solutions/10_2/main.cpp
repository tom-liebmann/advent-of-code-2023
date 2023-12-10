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
        std::vector< bool > partOfLoop;

        char getTile( int x, int y ) const
        {
            return rows.at( y )[ x ];
        }

        char getTileType( int x, int y ) const;

        std::pair< int, int > findStart() const;

        void markLoop();

        long countInner() const;

        void traverseTiles( std::function< void( int, int ) > const& callback );

        void print() const;
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

    auto map = parseMap( fileStream );

    map.markLoop();
    map.print();
    auto const inner = map.countInner();

    fmt::print( "Solution: {}\n", inner );

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

    void Map::markLoop()
    {
        partOfLoop.resize( width * height, false );
        traverseTiles(
            [ this ]( int x, int y )
            {
                partOfLoop[ x + y * width ] = true;
            } );

        for( int y = 0; y < height; ++y )
        {
            for( int x = 0; x < width; ++x )
            {
                if( !partOfLoop[ x + y * width ] )
                {
                    rows.at( y )[ x ] = '.';
                }
            }
        }
    }

    long Map::countInner() const
    {
        auto total = 0L;

        for( int y = 0; y < height; ++y )
        {
            auto rowTiles = 0;

            auto inside = false;

            for( int x = 0; x < width; ++x )
            {
                auto const isLoop = partOfLoop[ x + y * width ];

                fmt::print( "Tile: {} {} {} {}\n", x, y, inside, isLoop );

                if( inside && !isLoop )
                {
                    fmt::print( "Tile: {} {}\n", x, y );
                    ++total;
                }

                if( isLoop )
                {
                    auto const tile = getTileType( x, y );

                    switch( tile )
                    {
                        case '|':
                            inside = !inside;
                            break;
                        case '7':
                            inside = !inside;
                            break;
                        case 'F':
                            inside = !inside;
                            break;
                    }
                }
            }
        }

        return total;
    }

    void Map::traverseTiles( std::function< void( int, int ) > const& callback )
    {
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

        callback( x, y );

        do
        {
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

            callback( x, y );
        } while( getTile( x, y ) != 'S' );
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
            auto const left = x > 0 && ( getTile( x - 1, y ) == '-' || getTile( x - 1, y ) == 'L' ||
                                         getTile( x - 1, y ) == 'F' );
            auto const bottom =
                y < height - 1 && ( getTile( x, y + 1 ) == '|' || getTile( x, y + 1 ) == 'L' ||
                                    getTile( x, y + 1 ) == 'J' );

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

    void Map::print() const
    {
        for( int y = 0; y < height; ++y )
        {
            fmt::print( "{}\n", rows[ y ] );
        }
    }
}
