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

        void flipPoint( std::size_t x, std::size_t y );

        Pattern( std::vector< std::string > rowValues );
    };

    std::vector< std::size_t > getMirrorPoints( std::string const& row );

    std::optional< std::size_t > findMirror( std::vector< std::string > const& rows,
                                             std::optional< std::size_t > exclude = {} );

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
                     [ & ]( auto pattern )
                     {
                         auto const oRowMirror = findMirror( pattern.rows );
                         auto const oColMirror = findMirror( pattern.cols );

                         auto found = false;
                         for( std::size_t r = 0; r < pattern.height; ++r )
                         {
                             for( std::size_t c = 0; c < pattern.width; ++c )
                             {
                                 pattern.flipPoint( c, r );

                                 auto const rowMirror = findMirror( pattern.rows, oRowMirror );
                                 if( rowMirror )
                                 {
                                     sum += rowMirror.value();
                                     found = true;
                                 }

                                 auto const colMirror = findMirror( pattern.cols, oColMirror );
                                 if( colMirror )
                                 {
                                     sum += colMirror.value() * 100;
                                     found = true;
                                 }
                                 pattern.flipPoint( c, r );

                                 if( found )
                                 {
                                     break;
                                 }
                             }
                             if( found )
                             {
                                 break;
                             }
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

        for( std::size_t i = 1; i < row.length(); ++i )
        {
            auto isMirror = true;

            for( std::size_t j = 0; j < std::min( row.size() - i, i ); ++j )
            {
                if( row[ i + j ] != row[ i - 1 - j ] )
                {
                    isMirror = false;
                    break;
                }
            }

            if( isMirror )
            {
                mirrorPoints.push_back( i );
            }
        }

        return mirrorPoints;
    }

    std::optional< std::size_t > findMirror( std::vector< std::string > const& rows,
                                             std::optional< std::size_t > exclude )
    {
        auto mirrors = std::vector< std::size_t >( rows[ 0 ].size(), 0 );
        std::iota( std::begin( mirrors ), std::end( mirrors ), 0 );

        if( exclude )
        {
            mirrors.erase(
                std::remove( std::begin( mirrors ), std::end( mirrors ), exclude.value() ),
                std::end( mirrors ) );
        }

        auto newMirrors = std::vector< std::size_t >{};
        for( auto const& row : rows )
        {
            auto const rowMirrors = getMirrorPoints( row );
            std::set_intersection( std::begin( rowMirrors ),
                                   std::end( rowMirrors ),
                                   std::begin( mirrors ),
                                   std::end( mirrors ),
                                   std::back_inserter( newMirrors ) );
            mirrors = std::move( newMirrors );
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

    void Pattern::flipPoint( std::size_t x, std::size_t y )
    {
        rows[ y ][ x ] = rows[ y ][ x ] == '.' ? '#' : '.';
        cols[ x ][ y ] = cols[ x ][ y ] == '.' ? '#' : '.';
    }
}
