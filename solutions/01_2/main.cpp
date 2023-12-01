#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <ranges>
#include <regex>
#include <string>


namespace
{
    auto const DIGITS = std::map< std::string, int >{
        { "zero", 0 }, { "one", 1 }, { "two", 2 },   { "three", 3 }, { "four", 4 },
        { "five", 5 }, { "six", 6 }, { "seven", 7 }, { "eight", 8 }, { "nine", 9 },
        { "0", 0 },    { "1", 1 },   { "2", 2 },     { "3", 3 },     { "4", 4 },
        { "5", 5 },    { "6", 6 },   { "7", 7 },     { "8", 8 },     { "9", 9 },
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

    auto sum = 0L;
    auto line = std::string{};
    while( std::getline( fileStream, line ) )
    {
        auto const digitFound = [ &line ]( auto const& pair ) {
            return pair.first != std::end( line );
        };

        auto const firstDigit =
            std::ranges::min( DIGITS | std::views::transform( [ &line ]( auto const& digitEntry ) {
                                  return std::make_pair(
                                      std::ranges::search( line, digitEntry.first ).begin(),
                                      digitEntry.second );
                              } ) |
                              std::views::filter( digitFound ) )
                .second;

        auto const lastDigit =
            std::ranges::max( DIGITS | std::views::transform( [ &line ]( auto const& digitEntry ) {
                                  return std::make_pair(
                                      std::ranges::find_end( line, digitEntry.first ).begin(),
                                      digitEntry.second );
                              } ) |
                              std::views::filter( digitFound ) )
                .second;

        std::cout << "Line: " << line << '\n';
        std::cout << "Min max: " << firstDigit << " " << lastDigit << '\n';

        sum += firstDigit * 10 + lastDigit;
    }

    std::cout << "Sum: " << sum << '\n';

    return EXIT_SUCCESS;
}
