#include <cstdlib>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <regex>
#include <unordered_set>

#include <utils.hpp>


namespace
{
    struct Card
    {
        std::unordered_set< int > winningNumbers;
        std::unordered_set< int > myNumbers;

        long getWorth() const
        {
            auto numWinning = 0;
            for( auto const num : myNumbers )
            {
                if( winningNumbers.contains( num ) )
                {
                    ++numWinning;
                }
            }
            return numWinning;
        }
    };

    std::vector< Card > parseInput( std::istream& inputStream );
}


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 30 },
    { "input_final.txt", 7013204 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto const cards = parseInput( inputStream );
    auto cardAmount = std::vector< int >( cards.size(), 1 );

    auto sum = 0L;

    for( std::size_t i = 0; i < cards.size(); ++i )
    {
        auto const& card = cards[ i ];
        sum += cardAmount[ i ];

        std::cout << "Amount: " << cardAmount[ i ] << '\n';

        auto const worth = card.getWorth();

        std::cout << "Worth: " << worth << '\n';

        for( std::size_t j = i + 1; j < i + 1 + worth; ++j )
        {
            cardAmount[ j ] += cardAmount[ i ];
        }
    }

    return sum;
}


namespace
{
    std::vector< std::string > split( const std::string& s, char delim )
    {
        auto result = std::vector< std::string >{};
        auto stream = std::stringstream{ s };

        auto item = std::string{};
        while( getline( stream, item, delim ) )
        {
            result.push_back( item );
        }

        return result;
    }

    std::vector< Card > parseInput( std::istream& inputStream )
    {
        auto cards = std::vector< Card >{};
        auto const pattern = std::regex{ R"(^Card\s+\d+:(.*)\|(.*)$)" };
        auto line = std::string{};
        while( std::getline( inputStream, line ) )
        {
            auto match = std::smatch{};
            if( !std::regex_match( line, match, pattern ) )
            {
                throw std::runtime_error( fmt::format( "Line does not match: {}", line ) );
            }

            auto card = Card{};

            iterateNumbers( match[ 1 ],
                            [ &card ]( int num, int pos, int len )
                            {
                                card.winningNumbers.insert( num );
                            } );

            iterateNumbers( match[ 2 ],
                            [ &card ]( int num, int pos, int len )
                            {
                                card.myNumbers.insert( num );
                            } );

            cards.push_back( card );
        }

        return cards;
    }
}
