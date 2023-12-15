#include <cstdlib>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

#include <utils.hpp>

namespace
{
    struct CubeDraw
    {
        int numRed;
        int numGreen;
        int numBlue;
    };

    struct Game
    {
        int id;
        std::vector< CubeDraw > draws;

        long getPower() const
        {
            auto maxRed = 0;
            auto maxGreen = 0;
            auto maxBlue = 0;
            for( auto const& draw : draws )
            {
                maxRed = std::max( maxRed, draw.numRed );
                maxGreen = std::max( maxGreen, draw.numGreen );
                maxBlue = std::max( maxBlue, draw.numBlue );
            }

            return maxRed * maxGreen * maxBlue;
        }
    };

    std::vector< std::string > split( const std::string& s, char delim );

    std::vector< Game > parseInput( std::istream& inputStream );

    bool isValidGame( Game const& game );
}


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 2286 },
    { "input_final.txt", 71220 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto const games = parseInput( inputStream );

    auto sum = 0;
    for( auto const& game : games )
    {
        sum += game.getPower();
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
