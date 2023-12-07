#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <regex>
#include <unordered_set>


#include <stream_utils.hpp>
#include <string_utils.hpp>


namespace
{
    class Card
    {
    public:
        char value;
        int rank;

        Card( char valueValue ) : value{ valueValue }, rank{ getRank() }
        {
        }

    private:
        int getRank() const
        {
            auto const RANKS =
                std::map< char, int >{ { 'A', 0 },  { 'K', 1 },  { 'Q', 2 }, { 'J', 3 }, { 'T', 4 },
                                       { '9', 5 },  { '8', 6 },  { '7', 7 }, { '6', 8 }, { '5', 9 },
                                       { '4', 10 }, { '3', 11 }, { '2', 12 } };
            return RANKS.at( value );
        }
    };

    bool operator<( Card const& lhs, Card const& rhs )
    {
        return lhs.rank < rhs.rank;
    }

    class Hand
    {
    public:
        enum class Type : int
        {
            FIVE_OF_A_KIND,
            FOUR_OF_A_KIND,
            FULL_HOUSE,
            THREE_OF_A_KIND,
            TWO_PAIR,
            ONE_PAIR,
            HIGH_CARD,
        };

        std::vector< Card > cards;
        Type type;
        long bid;

        Hand( std::vector< Card > cardsValue, long bidValue )
            : cards{ std::move( cardsValue ) }, type{ getType() }, bid{ bidValue }
        {
        }

    private:
        Type getType() const
        {
            auto sorted = cards;
            std::sort( std::begin( sorted ), std::end( sorted ) );

            auto streaks = std::vector< int >{};

            streaks.push_back( 1 );

            for( int i = 1; i < sorted.size(); ++i )
            {
                auto const& card = sorted[ i ];
                auto const& prevCard = sorted[ i - 1 ];
                if( card.value == prevCard.value )
                {
                    ++streaks.back();
                }
                else
                {
                    streaks.push_back( 1 );
                }
            }

            std::sort( std::begin( streaks ), std::end( streaks ), std::greater<>{} );

            if( streaks.size() == 5 )
            {
                return Type::HIGH_CARD;
            }

            if( streaks.size() == 1 )
            {
                return Type::FIVE_OF_A_KIND;
            }

            if( streaks.size() == 2 && streaks[ 0 ] == 4 )
            {
                return Type::FOUR_OF_A_KIND;
            }

            if( streaks.size() == 2 && streaks[ 0 ] == 3 )
            {
                return Type::FULL_HOUSE;
            }

            if( streaks.size() == 3 && streaks[ 0 ] == 3 )
            {
                return Type::THREE_OF_A_KIND;
            }

            if( streaks.size() == 3 && streaks[ 0 ] == 2 )
            {
                return Type::TWO_PAIR;
            }

            return Type::ONE_PAIR;
        }
    };

    bool operator<( Hand const& lhs, Hand const& rhs )
    {
        if( lhs.type != rhs.type )
        {
            return lhs.type < rhs.type;
        }

        for( int i = 0; i < 5; ++i )
        {
            if( lhs.cards[ i ].rank != rhs.cards[ i ].rank )
            {
                return lhs.cards[ i ].rank < rhs.cards[ i ].rank;
            }
        }

        return false;
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

    auto const cardPattern = std::regex{ R"(^([AKQJT98765432]{5})\s+(\d+)$)" };

    auto hands = std::vector< Hand >{};

    iterateLines( fileStream,
                  [ & ]( auto const& line )
                  {
                      auto match = std::smatch{};
                      if( !std::regex_match( line, match, cardPattern ) )
                      {
                          throw std::runtime_error( fmt::format( "Line did not match: {}", line ) );
                      }

                      auto const cardMatch = match[ 1 ].str();
                      auto const bidMatch = match[ 2 ].str();

                      auto cards = std::vector< Card >{ { cardMatch[ 0 ] },
                                                        { cardMatch[ 1 ] },
                                                        { cardMatch[ 2 ] },
                                                        { cardMatch[ 3 ] },
                                                        { cardMatch[ 4 ] } };

                      auto bid = std::stol( bidMatch );

                      auto hand = Hand{ std::move( cards ), bid };

                      hands.emplace_back( hand );
                  } );

    std::sort( std::begin( hands ), std::end( hands ) );

    auto result = 0L;
    for( int i = 0; i < hands.size(); ++i )
    {
        result += ( hands.size() - i ) * hands[ i ].bid;
    }

    std::cout << result << '\n';

    return EXIT_SUCCESS;
}
