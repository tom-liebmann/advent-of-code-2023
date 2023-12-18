#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <queue>
#include <regex>
#include <set>
#include <thread>
#include <unordered_set>

#include <omp.h>

#include <utils.hpp>


namespace
{
    enum class Dir : int
    {
        RIGHT,
        UP,
        LEFT,
        DOWN,
    };

    Dir dirFromChar( char c );

    struct Instruction
    {
        Dir dir;
        int len;
        std::string color;

        static Instruction parse( std::string const& line );
    };

    struct Pos
    {
        int x;
        int y;
    };

    std::generator< Pos > traverse( std::vector< Instruction > const& instructions, Pos start );
}


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 51 },
    { "input_final.txt", 7987 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto instructions = std::vector< Instruction >{};

    for( auto const& line : readLines( inputStream ) )
    {
        instructions.push_back( Instruction::parse( line ) );
    }

    auto prevPos = Pos{ 0, 0 };

    auto area = 0L;
    auto tiles = 0L;
    for( auto const& pos : traverse( instructions, Pos{ 0, 0 } ) )
    {
        area += ( prevPos.y + pos.y ) * ( prevPos.x - pos.x );
        tiles += std::abs( ( pos.x - prevPos.x ) + ( pos.y - prevPos.y ) );
        prevPos = pos;
    }

    auto const innerTiles = std::abs( area / 2 ) - ( tiles - 4 ) / 2 - 1;
    auto const total = innerTiles + tiles;

    fmt::print( "Area: {} {}\n", area, tiles );
    fmt::print( "Result: {} {}\n", innerTiles, total );

    return total;
}

namespace
{
    Dir dirFromChar( char c )
    {
        auto const DIRS = std::unordered_map< char, Dir >{
            { 'R', Dir::RIGHT },
            { 'U', Dir::UP },
            { 'L', Dir::LEFT },
            { 'D', Dir::DOWN },
        };
        return DIRS.at( c );
    }

    Instruction Instruction::parse( std::string const& line )
    {
        auto const PATTERN = std::regex{ R"(^([RULD])\s+(\d+)\s+\(#([a-f0-9]{6})\)\n?$)" };
        auto match = std::smatch{};
        if( !std::regex_match( line, match, PATTERN ) )
        {
            throw std::runtime_error( fmt::format( "Mismatch: {}\n", line ) );
        }

        auto const dir = dirFromChar( match[ 1 ].str()[ 0 ] );
        return Instruction{ dir, std::stoi( match[ 2 ] ), match[ 3 ] };
    }

    std::generator< Pos > traverse( std::vector< Instruction > const& instructions, Pos start )
    {
        for( auto const& instr : instructions )
        {
            switch( instr.dir )
            {
                case Dir::RIGHT:
                    start.x += instr.len;
                    break;
                case Dir::UP:
                    start.y -= instr.len;
                    break;
                case Dir::LEFT:
                    start.x -= instr.len;
                    break;
                case Dir::DOWN:
                    start.y += instr.len;
            }
            co_yield start;
        }
    }
}
