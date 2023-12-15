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
    struct Lens
    {
        std::string key;
        int focalLength;
    };

    struct Box
    {
        std::vector< Lens > lenses;
    };

    int computeHash( std::string const& word )
    {
        auto hash = 0L;

        for( auto c : word )
        {
            if( c == '\n' )
            {
                continue;
            }
            hash += c;
            hash *= 17;
            hash %= 256;
        }

        return hash;
    }
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

    auto word = std::string{};

    auto boxes = std::vector< Box >( 256 );

    while( std::getline( fileStream, word, ',' ) )
    {
        fmt::print( "{}\n", word );

        auto const pattern = std::regex{ R"(^(\w+)([=-])(\d*)[\n]?)" };
        auto match = std::smatch{};
        if( !std::regex_match( word, match, pattern ) )
        {
            continue;
        }

        auto const key = match[ 1 ].str();
        auto const instruction = match[ 2 ].str();

        auto const hash = computeHash( key );
        fmt::print( "Hash: {} {}\n", key, hash );
        auto& box = boxes[ hash ];

        if( instruction[ 0 ] == '=' )
        {
            auto const focalLength = std::stoi( match[ 3 ] );

            auto const iter = std::find_if( std::begin( box.lenses ),
                                            std::end( box.lenses ),
                                            [ & ]( auto const& lens )
                                            {
                                                return lens.key == key;
                                            } );

            if( iter == std::end( box.lenses ) )
            {
                box.lenses.push_back( Lens{ key, focalLength } );
            }
            else
            {
                ( *iter ).key = key;
                ( *iter ).focalLength = focalLength;
            }
        }
        else
        {
            box.lenses.erase( std::remove_if( std::begin( box.lenses ),
                                              std::end( box.lenses ),
                                              [ & ]( auto const& lens )
                                              {
                                                  return lens.key == key;
                                              } ),
                              std::end( box.lenses ) );
        }
    }

    auto sum = 0L;
    for( int b = 0; b < boxes.size(); ++b )
    {
        auto const& box = boxes[ b ];
        fmt::print( "Box\n" );
        for( int i = 0; i < box.lenses.size(); ++i )
        {
            fmt::print( "Lens {} {}\n", i, box.lenses[ i ].key );
            sum += ( b + 1 ) * ( i + 1 ) * box.lenses[ i ].focalLength;
        }
    }

    fmt::print( "Value: {}\n", sum );

    return EXIT_SUCCESS;
}

namespace
{
}
