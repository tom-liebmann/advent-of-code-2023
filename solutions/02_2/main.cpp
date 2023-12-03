#include <cstdlib>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>


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

    std::vector< Game > parseInput( std::string const& fileName );

    bool isValidGame( Game const& game );
}


int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        std::cerr << "Missing parameter: <input file>\n";
        return EXIT_FAILURE;
    }

    auto const fileName = std::string{ argv[ 1 ] };

    auto const games = parseInput( fileName );

    auto sum = 0;
    for( auto const& game : games )
    {
        sum += game.getPower();
    }

    std::cout << "Sum: " << sum << '\n';
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

    std::vector< Game > parseInput( std::string const& fileName )
    {
        auto const gamePattern = std::regex{ R"(^Game (\d+): (.*)$)" };
        auto const cubePattern = std::regex{ R"(^\s*(\d*)\s*(red|blue|green)\s*$)" };

        auto games = std::vector< Game >{};

        auto fileStream = std::ifstream{ fileName };
        auto line = std::string{};
        while( std::getline( fileStream, line ) )
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
