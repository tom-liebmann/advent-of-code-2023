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
    Grid< int > readHeatLossGrid( std::istream& stream );

    long computeShortestPath( Grid< int > const& grid, int sx, int sy, int tx, int ty );
}


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 51 },
    { "input_final.txt", 7987 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto const grid = readHeatLossGrid( inputStream );
    return computeShortestPath( grid, 0, 0, grid.getWidth() - 1, grid.getHeight() - 1 );
}

namespace
{
    enum class Dir : std::uint8_t
    {
        NONE,
        RIGHT,
        UP,
        LEFT,
        DOWN,
    };

    struct Node
    {
        int x;
        int y;
        int straightCount;
        Dir dir;
        long g;
        double f;
    };

    bool operator==( Node const& lhs, Node const& rhs )
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.straightCount == rhs.straightCount &&
               lhs.dir == rhs.dir;
    }
}

template <>
struct std::hash< Node >
{
    std::size_t operator()( Node const& node ) const noexcept
    {
        auto hasher = HashComputer{};
        hasher.push( node.x );
        hasher.push( node.y );
        hasher.push( node.straightCount );
        hasher.push( static_cast< std::uint8_t >( node.dir ) );
        return hasher.getValue();
    }
};

namespace
{
    Grid< int > readHeatLossGrid( std::istream& stream )
    {
        auto const charGrid = readGrid( stream );
        auto intGrid = Grid< int >{ charGrid.getWidth(), charGrid.getHeight(), 0 };

        for( auto const& [ x, y, v ] : charGrid.getElements() )
        {
            intGrid( x, y ) = v - '0';
        }

        return intGrid;
    }

    struct CompareF
    {
        bool operator()( Node const& lhs, Node const& rhs ) const
        {
            return lhs.f < rhs.f;
        }
    };

    using SuccessorCallback = std::function< void( Node const& ) >;
    void iterateSuccessors( Grid< int > const& grid,
                            Node const& node,
                            Grid< double > const& h,
                            SuccessorCallback const& callback )
    {
        if( node.x > 0 && node.dir != Dir::RIGHT )
        {
            callback( Node{ node.x - 1,
                            node.y,
                            node.dir == Dir::LEFT ? node.straightCount + 1 : 1,
                            Dir::LEFT,
                            node.g + grid( node.x - 1, node.y ),
                            node.g + grid( node.x - 1, node.y ) + h( node.x - 1, node.y ) } );
        }

        if( node.y > 0 && node.dir != Dir::DOWN )
        {
            callback( Node{ node.x,
                            node.y - 1,
                            node.dir == Dir::UP ? node.straightCount + 1 : 1,
                            Dir::UP,
                            node.g + grid( node.x, node.y - 1 ),
                            node.g + grid( node.x, node.y - 1 ) + h( node.x, node.y - 1 ) } );
        }

        if( node.x < grid.getWidth() - 1 && node.dir != Dir::LEFT )
        {
            callback( Node{ node.x + 1,
                            node.y,
                            node.dir == Dir::RIGHT ? node.straightCount + 1 : 1,
                            Dir::RIGHT,
                            node.g + grid( node.x + 1, node.y ),
                            node.g + grid( node.x + 1, node.y ) + h( node.x + 1, node.y ) } );
        }

        if( node.y < grid.getHeight() - 1 && node.dir != Dir::UP )
        {
            callback( Node{ node.x,
                            node.y + 1,
                            node.dir == Dir::DOWN ? node.straightCount + 1 : 1,
                            Dir::DOWN,
                            node.g + grid( node.x, node.y + 1 ),
                            node.g + grid( node.x, node.y + 1 ) + h( node.x, node.y + 1 ) } );
        }
    }

    Grid< double > computeHeuristic( Grid< int > const& grid, int tx, int ty )
    {
        auto result = Grid< double >{ grid.getWidth(),
                                      grid.getHeight(),
                                      std::numeric_limits< double >::infinity() };

        struct Pos
        {
            int x;
            int y;
            double value;
        };

        auto queue = std::queue< Pos >{};

        queue.push( Pos{ tx, ty, 0.0f } );

        while( !queue.empty() )
        {
            auto const current = queue.front();
            queue.pop();

            if( !grid.isInside( current.x, current.y ) )
            {
                continue;
            }

            if( result( current.x, current.y ) <= current.value )
            {
                continue;
            }

            result( current.x, current.y ) = current.value;

            queue.push(
                Pos{ current.x - 1, current.y, current.value + grid( current.x, current.y ) } );
            queue.push(
                Pos{ current.x + 1, current.y, current.value + grid( current.x, current.y ) } );
            queue.push(
                Pos{ current.x, current.y - 1, current.value + grid( current.x, current.y ) } );
            queue.push(
                Pos{ current.x, current.y + 1, current.value + grid( current.x, current.y ) } );
        }

        return result;
    }

    long computeShortestPath( Grid< int > const& grid, int sx, int sy, int tx, int ty )
    {
        struct Pos
        {
            int x;
            int y;
        };

        auto prev = std::unordered_map< Node, Node >{};
        auto heuristic = computeHeuristic( grid, tx, ty );
        auto openlist = std::unordered_set< Node >{};
        auto visited = std::unordered_set< Node >{};

        openlist.insert( Node{ sx, sy, 0, Dir::NONE, 0, heuristic( sx, sy ) } );

        while( !openlist.empty() )
        {
            auto const minIter =
                std::min_element( std::begin( openlist ), std::end( openlist ), CompareF{} );
            auto const current = ( *minIter );
            openlist.erase( minIter );

            //            fmt::print( "Visiting {} {} {} {} {} {}\n",
            //                        current.x,
            //                        current.y,
            //                        current.straightCount,
            //                        static_cast< int >( current.dir ),
            //                        current.g,
            //                        current.f );

            if( current.x == tx && current.y == ty && current.straightCount >= 4 )
            {
                auto iter = prev.find( current );
                while( iter != std::end( prev ) )
                {
                    fmt::print( "Path: {} {}\n", iter->second.x, iter->second.y );
                    iter = prev.find( iter->second );
                }
                return current.g;
            }

            visited.insert( current );

            iterateSuccessors( grid,
                               current,
                               heuristic,
                               [ & ]( auto const& successor )
                               {
                                   if( successor.straightCount > 10 )
                                   {
                                       return;
                                   }

                                   if( current.dir != Dir::NONE && successor.dir != current.dir &&
                                       current.straightCount < 4 )
                                   {
                                       return;
                                   }

                                   if( visited.contains( successor ) )
                                   {
                                       return;
                                   }

                                   auto const iter = openlist.find( successor );
                                   if( iter != std::end( openlist ) )
                                   {
                                       if( successor.g >= iter->g )
                                       {
                                           return;
                                       }
                                       else
                                       {
                                           openlist.erase( iter );
                                       }
                                   }

                                   prev.insert( { successor, current } );
                                   openlist.insert( successor );
                               } );
        }

        throw std::runtime_error( "No path" );
    }
}
