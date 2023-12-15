#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include <utils.hpp>

namespace
{
    bool isDigit( char c );

    int digitToNumber( char c );
}


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 142 },
    { "input_final.txt", 54916 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto sum = 0L;
    auto line = std::string{};
    while( std::getline( inputStream, line ) )
    {
        auto const firstDigit = std::find_if( std::begin( line ), std::end( line ), isDigit );
        auto const lastDigit = std::find_if( std::rbegin( line ), std::rend( line ), isDigit );

        sum += digitToNumber( *firstDigit ) * 10 + digitToNumber( *lastDigit );
    }

    return sum;
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
