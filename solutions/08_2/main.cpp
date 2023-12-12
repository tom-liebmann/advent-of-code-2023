#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <ranges>
#include <regex>
#include <unordered_set>

#include <utils.hpp>


namespace
{
    struct Node
    {
        std::string name;
        std::string leftName;
        std::string rightName;

        static Node parse( std::string const& line );
    };

    std::unordered_map< std::string, Node > parseNodes( std::istream& stream );

    long computePathLen( std::string const& instructions,
                         std::unordered_map< std::string, Node > const& nodes,
                         std::string const& node );
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

    auto instructions = std::string{};
    std::getline( fileStream, instructions );

    auto const nodes = parseNodes( fileStream );

    auto const isStart = []( auto const& node )
    {
        return node.first.back() == 'A';
    };

    auto const computeNodePathLen = [ & ]( auto const& node )
    {
        return computePathLen( instructions, nodes, node.first );
    };

    auto pathLengths =
        nodes | std::views::filter( isStart ) | std::views::transform( computeNodePathLen );

    auto const result = std::reduce( pathLengths.begin(), pathLengths.end(), 1L, lcm );

    fmt::print( "Result: {}\n", result );

    return EXIT_SUCCESS;
}

namespace
{
    long computePathLen( std::string const& instructions,
                         std::unordered_map< std::string, Node > const& nodes,
                         std::string const& node )
    {
        auto currentNode = node;
        auto nodeCount = 0L;

        while( true )
        {
            for( auto const c : instructions )
            {
                if( currentNode[ currentNode.size() - 1 ] == 'Z' )
                {
                    return nodeCount;
                }
                if( c == 'L' )
                {
                    currentNode = nodes.at( currentNode ).leftName;
                }
                else
                {
                    currentNode = nodes.at( currentNode ).rightName;
                }

                ++nodeCount;
            }
        }
    }

    std::unordered_map< std::string, Node > parseNodes( std::istream& stream )
    {
        auto nodes = std::unordered_map< std::string, Node >{};

        for( auto const& line : readLines( stream ) )
        {
            auto node = Node::parse( line );
            nodes[ node.name ] = node;
        }

        return nodes;
    }

    Node Node::parse( std::string const& line )
    {
        auto const pattern = std::regex{ R"((\w{3})\s*=\s*\((\w{3})\s*,\s*(\w{3})\))" };
        auto match = std::smatch{};
        if( !std::regex_match( line, match, pattern ) )
        {
            throw std::runtime_error( fmt::format( "Line did not match: {}", line ) );
        }

        return { match[ 1 ], match[ 2 ], match[ 3 ] };
    }
}
