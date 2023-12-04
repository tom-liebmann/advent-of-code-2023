#include <cstdlib>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <regex>
#include <unordered_set>


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
            return numWinning == 0 ? 0 : ( 1L << ( numWinning - 1 ) );
        }
    };

    using NumberCallback = std::function< void( int, int, int ) >;

    void iterateNumbers( std::string const& line, NumberCallback const& callback );

    std::vector< Card > parseInput( std::string const& fileName );
}


int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        std::cerr << "Missing parameter: <input file>\n";
        return EXIT_FAILURE;
    }

    auto const fileName = std::string{ argv[ 1 ] };

    auto const cards = parseInput( fileName );

    auto sum = 0L;

    for( auto const& card : cards )
    {
        sum += card.getWorth();
    }

    std::cout << "Sum: " << sum << '\n';

    return EXIT_SUCCESS;
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

    void iterateNumbers( std::string const& line, NumberCallback const& callback )
    {
        auto const pattern = std::regex{ R"(\d+)" };

        auto begin = std::sregex_iterator{ std::begin( line ), std::end( line ), pattern };
        auto end = std::sregex_iterator{};

        for( auto i = begin; i != end; ++i )
        {
            auto const match = ( *i );
            callback( std::stoi( match.str() ), match.position(), match.length() );
        }
    }

    std::vector< Card > parseInput( std::string const& fileName )
    {
        auto cards = std::vector< Card >{};
        auto const pattern = std::regex{ R"(^Card\s+\d+:(.*)\|(.*)$)" };
        auto fileStream = std::ifstream{ fileName };
        auto line = std::string{};
        while( std::getline( fileStream, line ) )
        {
            auto match = std::smatch{};
            if( !std::regex_match( line, match, pattern ) )
            {
                throw std::runtime_error( fmt::format( "Line does not match: {}", line ) );
            }

            std::cout << "Line: " << line << '\n';
            auto card = Card{};
            std::cout << "Match: " << match[ 0 ] << '\n';
            std::cout << "Match: " << match[ 1 ] << '\n';
            std::cout << "Match: " << match[ 2 ] << '\n';

            iterateNumbers( match[ 1 ],
                            [ &card ]( int num, int pos, int len )
                            {
                                std::cout << "Num: " << num << '\n';
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
