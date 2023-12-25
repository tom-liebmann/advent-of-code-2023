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
#include <stack>
#include <thread>
#include <unordered_set>

#include <omp.h>

#include <utils.hpp>


namespace
{
    struct Pos
    {
        int x;
        int y;

        bool operator==( Pos const& rhs ) const
        {
            return x == rhs.x && y == rhs.y;
        }
    };

    struct Edge
    {
        bool valid;
        std::size_t len;
        std::array< std::size_t, 2 > points;

        void replaceNode( std::size_t oldNode, std::size_t newNode )
        {
            if( points[ 0 ] == oldNode )
            {
                points[ 0 ] = newNode;
            }

            if( points[ 1 ] == oldNode )
            {
                points[ 1 ] = newNode;
            }
        }

        std::size_t getNeighbor( std::size_t node ) const
        {
            return points[ 0 ] == node ? points[ 1 ] : points[ 0 ];
        }
    };

    struct Node
    {
        bool valid;
        Pos pos;
        std::vector< std::size_t > edges;

        void replaceEdge( std::size_t oldEdge, std::size_t newEdge )
        {
            edges.erase( std::remove( std::begin( edges ), std::end( edges ), oldEdge ),
                         std::end( edges ) );
            edges.push_back( newEdge );
        }
    };

    struct Graph
    {
        std::vector< Node > nodes;
        std::vector< Edge > edges;
        std::size_t startNode;
        std::size_t targetNode;
    };

    Graph constructGraph( Grid< char > const& grid, Pos const& start, Pos const& target );

    void simplifyGraph( Graph& graph );

    long computeLongestPathLen( Graph const& graph );
}

template <>
struct std::hash< Pos >
{
    std::size_t operator()( Pos const& pos ) const noexcept
    {
        auto hasher = HashComputer{};
        hasher.push( pos.x );
        hasher.push( pos.y );
        return hasher.getValue();
    }
};


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 51 },
    { "input_final.txt", 7987 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto const grid = readGrid( inputStream );

    auto graph =
        constructGraph( grid, Pos{ 1, 0 }, Pos{ grid.getWidth() - 2, grid.getHeight() - 1 } );

    fmt::print( "Nodes: {}\n", graph.nodes.size() );

    simplifyGraph( graph );

    fmt::print( "Nodes: {}\n",
                std::ranges::count_if( graph.nodes,
                                       []( auto const& n )
                                       {
                                           return n.valid;
                                       } ) );

    return computeLongestPathLen( graph );
}


namespace
{
    Graph constructGraph( Grid< char > const& grid, Pos const& start, Pos const& target )
    {
        auto nodeGrid = Grid< std::size_t >{ grid.getWidth(), grid.getHeight(), 0 };
        auto nodes = std::vector< Node >{};

        for( auto [ x, y, v ] : grid.getElements() )
        {
            if( v == '#' )
            {
                continue;
            }

            nodeGrid( x, y ) = nodes.size();
            nodes.push_back( Node{ true, Pos{ x, y }, {} } );
        }

        auto edges = std::vector< Edge >{};
        for( auto [ x, y, v ] : grid.getElements() )
        {
            if( v == '#' )
            {
                continue;
            }

            for( auto [ dx, dy ] :
                 std::vector< std::pair< int, int > >{ { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } } )
            {
                auto const nx = x + dx;
                auto const ny = y + dy;

                if( !grid.isInside( nx, ny )  // Only consider neighbors inside bounds
                    || grid( nx, ny ) == '#'  // Do not connect to walls
                    || nodeGrid( x, y ) > nodeGrid( nx, ny ) )  // Avoid duplicating edges
                {
                    continue;
                }

                nodes[ nodeGrid( x, y ) ].edges.push_back( edges.size() );
                nodes[ nodeGrid( nx, ny ) ].edges.push_back( edges.size() );

                edges.push_back( Edge{ true, 1, nodeGrid( x, y ), nodeGrid( nx, ny ) } );
            }
        }

        return Graph{ std::move( nodes ),
                      std::move( edges ),
                      nodeGrid( start.x, start.y ),
                      nodeGrid( target.x, target.y ) };
    }

    void simplifyGraph( Graph& graph )
    {
        for( std::size_t nodeId = 0; nodeId < graph.nodes.size(); ++nodeId )
        {
            auto& node = graph.nodes[ nodeId ];

            if( node.edges.size() == 2 )
            {
                // Remove regular node
                node.valid = false;

                auto& edge1 = graph.edges[ node.edges[ 0 ] ];
                auto& edge2 = graph.edges[ node.edges[ 1 ] ];

                // Remove edge 2
                edge2.valid = false;

                auto const neighborId = edge2.getNeighbor( nodeId );
                auto& neighbor = graph.nodes[ neighborId ];

                edge1.replaceNode( nodeId, neighborId );
                edge1.len += edge2.len;

                neighbor.replaceEdge( node.edges[ 1 ], node.edges[ 0 ] );
            }
        }
    }

    long computeLongestPathLen( Graph const& graph )
    {
        auto longestLen = 0L;

        struct StackElement
        {
            std::size_t node;
            std::size_t nodeEdge;
            long edgeLen;
        };

        auto nodeStack = std::stack< StackElement >{};
        auto visited = std::vector< bool >( graph.nodes.size(), false );

        nodeStack.push( StackElement{ graph.startNode, 0, 0 } );

        auto pathLen = 0L;
        while( true )
        {
            if( nodeStack.empty() )
            {
                return longestLen;
            }

            auto& top = nodeStack.top();

            if( top.node == graph.targetNode )
            {
                if( pathLen > longestLen )
                {
                    longestLen = pathLen;
                }
                visited[ top.node ] = false;
                nodeStack.pop();
                pathLen -= top.edgeLen;
                continue;
            }

            auto const& node = graph.nodes[ top.node ];

            if( top.nodeEdge >= node.edges.size() )
            {
                visited[ top.node ] = false;
                nodeStack.pop();
                pathLen -= top.edgeLen;
                continue;
            }

            auto const edgeId = node.edges[ top.nodeEdge ];
            auto const& edge = graph.edges[ edgeId ];
            auto const neighborId = edge.getNeighbor( top.node );

            if( visited[ neighborId ] )
            {
                ++top.nodeEdge;
                continue;
            }

            visited[ neighborId ] = true;
            nodeStack.push( StackElement{ neighborId, 0, edge.len } );
            ++top.nodeEdge;
            pathLen += edge.len;
        }
    }
}
