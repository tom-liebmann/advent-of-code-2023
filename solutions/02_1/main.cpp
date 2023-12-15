#include <cstdlib>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

#include <utils.hpp>


namespace
{
    constexpr auto MAX_RED = 12;
    constexpr auto MAX_GREEN = 13;
    constexpr auto MAX_BLUE = 14;

    struct CubeDraw
    {
        int numRed;
        int numGreen;
        int numBlue;

        bool isValid() const
        {
            return numRed <= MAX_RED && numGreen <= MAX_GREEN && numBlue <= MAX_BLUE;
        }
    };

    struct Game
    {
        int id;
        std::vector< CubeDraw > draws;

        bool isValid() const
        {
            for( auto const& draw : draws )
            {
                if( !draw.isValid() )
                {
                    return false;
                }
            }

            return true;
        }
    };

    std::vector< Game > parseInput( std::istream& inputStream );

    bool isValidGame( Game const& game );
}


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 8 },
    { "input_final.txt", 2377 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto const games = parseInput( inputStream );

    auto sum = 0;
    for( auto const& game : games )
    {
        if( game.isValid() )
        {
            sum += game.id;
        }
    }

    return sum;
}


namespace
{
    std::vector< Game > parseInput( std::istream& inputStream )
    {
        auto const gamePattern = std::regex{ R"(^Game (\d+): (.*)$)" };
        auto const cubePattern = std::regex{ R"(^\s*(\d*)\s*(red|blue|green)\s*$)" };

        auto games = std::vector< Game >{};

        auto line = std::string{};
        while( std::getline( inputStream, line ) )
        {
            auto match = std::smatch{};
            if( !std::regex_match( line, match, gamePattern ) )
            {
                throw std::runtime_error{ fmt::format( "Line does not match regex: {}", line ) };
            }

            auto gameObj = Game{};
            gameObj.id = std::stoi( match[ 1 ] );
            auto const draws = split( match[ 2 ], ';' );
            for( auto const& draw : draws )
            {
                auto drawObj = CubeDraw{};
                auto const cubes = split( draw, ',' );
                for( auto cube : cubes )
                {
                    if( !std::regex_match( cube, match, cubePattern ) )
                    {
                        throw std::runtime_error(
                            fmt::format( "Draw does not match pattern: {}", draw ) );
                    }

                    auto const num = std::stoi( match[ 1 ] );
                    auto const& type = match[ 2 ];

                    if( type == "red" )
                    {
                        drawObj.numRed = num;
                    }
                    else if( type == "green" )
                    {
                        drawObj.numGreen = num;
                    }
                    else if( type == "blue" )
                    {
                        drawObj.numBlue = num;
                    }
                }

                gameObj.draws.push_back( drawObj );
            }

            games.push_back( gameObj );
        }

        return games;
    }
}
