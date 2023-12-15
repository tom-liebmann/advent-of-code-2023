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


    auto sum = 0L;
    while( std::getline( fileStream, word, ',' ) )
    {
        fmt::print( "{}\n", word );

        auto value = 0L;

        for( auto c : word )
        {
            if( c == '\n' )
            {
                continue;
            }
            value += c;
            value *= 17;
            value %= 256;
        }

        fmt::print( "Value: {}\n", value );

        sum += value;
    }

    fmt::print( "Value: {}\n", sum );

    return EXIT_SUCCESS;
}

namespace
{
}
