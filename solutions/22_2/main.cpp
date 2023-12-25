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
    struct Vec3i
    {
        int x;
        int y;
        int z;
    };

    struct Block
    {
        Vec3i min;
        Vec3i max;

        std::unordered_set< Block* > above;
        std::unordered_set< Block* > below;

        static Block parse( std::string const& line );
    };
}


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 51 },
    { "input_final.txt", 7987 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto blocks = std::vector< Block >{};

    for( auto const& line : readLines( inputStream ) )
    {
        blocks.push_back( Block::parse( line ) );
    }

    auto max = Vec3i{
        std::numeric_limits< int >::min(),
        std::numeric_limits< int >::min(),
        std::numeric_limits< int >::min(),
    };
    auto min = Vec3i{
        std::numeric_limits< int >::max(),
        std::numeric_limits< int >::max(),
        std::numeric_limits< int >::max(),
    };

    for( auto const& block : blocks )
    {
        max.x = std::max( max.x, block.max.x );
        max.y = std::max( max.y, block.max.y );
        max.z = std::max( max.z, block.max.z );

        min.x = std::min( min.x, block.min.x );
        min.y = std::min( min.y, block.min.y );
        min.z = std::min( min.z, block.min.z );
    }

    struct Node
    {
        int height;
        Block* block;
    };

    auto grid = Grid< Node >{ max.x - min.x + 1, max.y - min.y + 1, Node{ 0, nullptr } };

    std::sort( std::begin( blocks ),
               std::end( blocks ),
               []( auto const& lhs, auto const& rhs )
               {
                   return lhs.min.z < rhs.min.z;
               } );

    for( auto& block : blocks )
    {
        fmt::print( "Free: {} {} {}\n", block.min.x, block.min.y, block.min.z );
        auto maxZ = 0;
        auto blocksBelow = std::unordered_set< Block* >{};

        for( int x = block.min.x; x <= block.max.x; ++x )
        {
            for( int y = block.min.y; y <= block.max.y; ++y )
            {
                auto const& gridNode = grid( x, y );

                if( gridNode.block == nullptr )
                {
                    continue;
                }

                if( gridNode.height > maxZ )
                {
                    maxZ = gridNode.height;
                    blocksBelow.clear();
                    blocksBelow.insert( gridNode.block );
                }
                else if( gridNode.height == maxZ )
                {
                    blocksBelow.insert( gridNode.block );
                }
            }
        }

        block.below = blocksBelow;

        for( auto const below : block.below )
        {
            below->above.insert( &block );
        }

        for( int x = block.min.x; x <= block.max.x; ++x )
        {
            for( int y = block.min.y; y <= block.max.y; ++y )
            {
                grid( x, y ).height = maxZ + block.max.z - block.min.z + 1;
                grid( x, y ).block = &block;
            }
        }
    }

    auto sum = 0L;
    for( int i = 0; i < blocks.size(); ++i )
    {
        auto falling = std::unordered_set< Block* >{};
        falling.insert( &blocks[ i ] );
        for( int j = i + 1; j < blocks.size(); ++j )
        {
            if( blocks[ j ].below.empty() )
            {
                continue;
            }

            auto allFalling = true;
            for( auto const below : blocks[ j ].below )
            {
                if( !falling.contains( below ) )
                {
                    allFalling = false;
                    break;
                }
            }

            if( allFalling )
            {
                ++sum;
                falling.insert( &blocks[ j ] );
            }
        }
        fmt::print( "Falling: {} {}\n", i, falling.size() );
    }

    return sum;
}


namespace
{
    Block Block::parse( std::string const& line )
    {
        auto const PATTERN =
            std::regex{ R"(^([-\d]+),([-\d]+),([-\d]+)~([-\d]+),([-\d]+),([-\d]+)$)" };

        auto match = std::smatch{};
        std::regex_match( line, match, PATTERN );

        return Block{
            { std::stoi( match[ 1 ] ), std::stoi( match[ 2 ] ), std::stoi( match[ 3 ] ) },
            { std::stoi( match[ 4 ] ), std::stoi( match[ 5 ] ), std::stoi( match[ 6 ] ) },
        };
    }
}
