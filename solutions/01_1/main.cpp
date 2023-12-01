#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>

namespace
{
    bool isDigit( char c );

    int digitToNumber( char c );
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

    auto sum = 0L;
    auto line = std::string{};
    while( std::getline( fileStream, line ) )
    {
        auto const firstDigit = std::find_if( std::begin( line ), std::end( line ), isDigit );
        auto const lastDigit = std::find_if( std::rbegin( line ), std::rend( line ), isDigit );

        sum += digitToNumber( *firstDigit ) * 10 + digitToNumber( *lastDigit );
    }

    std::cout << "Sum: " << sum << '\n';

    return EXIT_SUCCESS;
}


namespace
{
    bool isDigit( char c )
    {
        return std::isdigit( c );
    }

    int digitToNumber( char c )
    {
        return c - '0';
    }
}
