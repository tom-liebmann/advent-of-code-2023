#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <regex>
#include <set>
#include <unordered_set>

#include <omp.h>

#include <utils.hpp>


namespace
{
    struct Pattern
    {
        std::size_t width;
        std::size_t height;
        std::vector< std::string > rows;
        std::vector< std::string > cols;

        std::optional< std::size_t > findMirrorRow() const;

        std::optional< std::size_t > findMirrorCol() const;

        Pattern( std::vector< std::string > rowValues );
    };

    std::vector< std::size_t > getMirrorPoints( std::string const& row );

    std::optional< std::size_t > findMirror( std::vector< std::string > const& rows );

    void iteratePatterns( std::istream& stream,
                          std::function< void( Pattern const& ) > const& callback );
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

    auto sum = 0L;
    iteratePatterns( fileStream,
                     [ & ]( auto const& pattern )
                     {
                         fmt::print( "{} {}\n", pattern.width, pattern.height );

                         fmt::print( "Rows\n" );
                         auto const rowMirror = findMirror( pattern.rows );

                         if( rowMirror )
                         {
                             sum += rowMirror.value();
                         }
                         else
                         {
                             fmt::print( "Cols\n" );
                             auto const colMirror = findMirror( pattern.cols );
                             sum += colMirror.value() * 100;
                         }
                     } );

    fmt::print( "Sum: {}\n", sum );

    return EXIT_SUCCESS;
}

namespace
{
    void iteratePatterns( std::istream& stream,
                          std::function< void( Pattern const& ) > const& callback )
    {
        auto rows = std::vector< std::string >{};
        auto line = std::string{};
        while( std::getline( stream, line ) )
        {
            if( line.length() == 0 && !rows.empty() )
            {
                callback( Pattern{ std::move( rows ) } );
            }
            else
            {
                rows.push_back( line );
            }
        }

        if( !rows.empty() )
        {
            callback( Pattern{ std::move( rows ) } );
        }
    }

    Pattern::Pattern( std::vector< std::string > rowValues )
        : width{ rowValues[ 0 ].size() }, height{ rowValues.size() }, rows{ std::move( rowValues ) }
    {
        for( std::size_t c = 0; c < width; ++c )
        {
            auto s = std::ostringstream{};
            for( std::size_t r = 0; r < height; ++r )
            {
                s << rows[ r ][ c ];
            }
            cols.push_back( s.str() );
        }
    }

    std::vector< std::size_t > getMirrorPoints( std::string const& row )
    {
        auto mirrorPoints = std::vector< std::size_t >{};

        // fmt::print( "Checking {}\n", row );

        for( std::size_t i = 1; i < row.length(); ++i )
        {
            auto isMirror = true;

            for( std::size_t j = 0; j < std::min( row.size() - i, i ); ++j )
            {
                if( row[ i + j ] != row[ i - 1 - j ] )
                {
                    // fmt::print( "No mirror: {} {} {}\n", j, row[ i - 1 - j ], row[ i + j ] );
                    isMirror = false;
                    break;
                }
            }

            if( isMirror )
            {
                // fmt::print( "Mirror: {}\n", i );
                mirrorPoints.push_back( i );
            }
        }

        return mirrorPoints;
    }

    std::optional< std::size_t > findMirror( std::vector< std::string > const& rows )
    {
        auto mirrors = std::vector< std::size_t >( rows[ 0 ].size(), 0 );
        auto newMirrors = std::vector< std::size_t >{};
        std::iota( std::begin( mirrors ), std::end( mirrors ), 0 );
        for( auto const& row : rows )
        {
            auto const rowMirrors = getMirrorPoints( row );
            std::set_intersection( std::begin( rowMirrors ),
                                   std::end( rowMirrors ),
                                   std::begin( mirrors ),
                                   std::end( mirrors ),
                                   std::back_inserter( newMirrors ) );
            mirrors = std::move( newMirrors );
            fmt::print( "Mirrors: {}\n", mirrors );
        }

        if( mirrors.empty() )
        {
            return {};
        }

        if( mirrors.size() > 1 )
        {
            throw std::runtime_error( "Multiple mirrors!" );
        }

        return mirrors[ 0 ];
    }
}
