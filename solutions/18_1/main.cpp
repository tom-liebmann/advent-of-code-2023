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

    std::generator< std::tuple< Pos, Dir, Dir > >
        traverse( std::vector< Instruction > const& instructions, Pos start );
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

    auto max = Pos{ 0, 0 };
    auto min = Pos{ 0, 0 };
    for( auto const& [ pos, inDir, outDir ] : traverse( instructions, { 0, 0 } ) )
    {
        min.x = std::min( min.x, pos.x );
        min.y = std::min( min.y, pos.y );
        max.x = std::max( max.x, pos.x );
        max.y = std::max( max.y, pos.y );
    }

    auto const DIRS = std::map< std::pair< Dir, Dir >, char >{
        { { Dir::LEFT, Dir::LEFT }, '-' }, { { Dir::LEFT, Dir::DOWN }, 'F' },
        { { Dir::LEFT, Dir::UP }, 'L' },   { { Dir::RIGHT, Dir::RIGHT }, '-' },
        { { Dir::RIGHT, Dir::UP }, '/' },  { { Dir::RIGHT, Dir::DOWN }, '7' },
        { { Dir::UP, Dir::UP }, '|' },     { { Dir::UP, Dir::LEFT }, '7' },
        { { Dir::UP, Dir::RIGHT }, 'F' },  { { Dir::DOWN, Dir::DOWN }, '|' },
        { { Dir::DOWN, Dir::LEFT }, '/' }, { { Dir::DOWN, Dir::RIGHT }, 'L' },
    };

    auto lagoon = Grid< char >{ max.x - min.x + 1, max.y - min.y + 1, '.' };
    for( auto const& [ pos, inDir, outDir ] : traverse( instructions, { 0, 0 } ) )
    {
        // fmt::print( "{} {}\n", static_cast< int >( inDir ), static_cast< int >( outDir ) );
        lagoon( pos.x - min.x, pos.y - min.y ) = DIRS.at( std::make_pair( inDir, outDir ) );
    }

    auto sum = 0L;
    for( int y = 0; y < lagoon.getHeight(); ++y )
    {
        auto inside = false;
        for( int x = 0; x < lagoon.getWidth(); ++x )
        {
            if( lagoon( x, y ) == '.' )
            {
                if( inside )
                {
                    lagoon( x, y ) = '#';
                    ++sum;
                }
            }
            else
            {
                switch( lagoon( x, y ) )
                {
                    case '7':
                    case '|':
                    case 'F':
                        inside = !inside;
                }
                ++sum;
            }
            fmt::print( "{}", lagoon( x, y ) );
        }
        fmt::print( "\n" );
    }

    return sum;
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

    std::generator< std::tuple< Pos, Dir, Dir > >
        traverse( std::vector< Instruction > const& instructions, Pos start )
    {
        auto prevDir = instructions[ instructions.size() - 1 ].dir;
        for( auto const& instr : instructions )
        {
            for( int i = 0; i < instr.len; ++i )
            {
                co_yield std::make_tuple( start, prevDir, instr.dir );
                switch( instr.dir )
                {
                    case Dir::RIGHT:
                        ++start.x;
                        break;
                    case Dir::UP:
                        --start.y;
                        break;
                    case Dir::LEFT:
                        --start.x;
                        break;
                    case Dir::DOWN:
                        ++start.y;
                }
                prevDir = instr.dir;
            }
        }
    }
}
