#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <regex>
#include <unordered_set>


#include <string_utils.hpp>


int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        std::cerr << "Missing parameter: <input file>\n";
        return EXIT_FAILURE;
    }

    auto const fileName = std::string{ argv[ 1 ] };
    auto fileStream = std::ifstream{ fileName };

    auto line = std::string{};
    std::getline( fileStream, line );
    auto times = std::vector< long >{};
    iterateNumbers( line,
                    [ &times ]( auto num, auto start, auto len )
                    {
                        times.push_back( num );
                    } );

    std::getline( fileStream, line );
    auto records = std::vector< long >{};
    iterateNumbers( line,
                    [ &records ]( auto num, auto start, auto len )
                    {
                        records.push_back( num );
                    } );

    auto result = 1L;
    for( std::size_t i = 0; i < times.size(); ++i )
    {
        auto const time = times[ i ];
        auto const record = records[ i ];

        auto const a1 =
            static_cast< long >( time / 2.0 - std::sqrt( std::pow( time / 2.0, 2.0 ) - record ) );
        auto const a2 = static_cast< long >(
            std::ceil( time / 2.0 + std::sqrt( std::pow( time / 2.0, 2.0 ) - record ) ) );

        auto const numWins = a2 - a1 - 1;

        std::cout << time << " " << record << '\n';
        std::cout << numWins << '\n';

        result *= numWins;
    }

    std::cout << result << '\n';

    return EXIT_SUCCESS;
}
