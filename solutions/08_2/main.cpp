#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <iostream>
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

    auto sharedFactors = std::unordered_map< long, long >{};

    for( auto const& node : nodes )
    {
        auto const& name = node.first;
        if( name[ name.size() - 1 ] != 'A' )
        {
            continue;
        }
        auto const pathLen = computePathLen( instructions, nodes, name );
        auto const factors = computePrimeFactors( pathLen );

        for( auto const& f : factors )
        {
            sharedFactors[ f.first ] = std::max( sharedFactors[ f.first ], f.second );
        }
    }

    auto result = 1L;
    for( auto const& f : sharedFactors )
    {
        result *= std::pow( f.first, f.second );
    }

    std::cout << "Node count: " << result << '\n';

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
        iterateLines( stream,
                      [ & ]( auto const& line )
                      {
                          auto node = Node::parse( line );
                          nodes[ node.name ] = node;
                      } );

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
