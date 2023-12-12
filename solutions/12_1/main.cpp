#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <regex>
#include <set>
#include <unordered_set>


#include <stream_utils.hpp>
#include <string_utils.hpp>


namespace
{
    bool matchesGroups( std::string const& pattern, std::vector< int > const& groups );

    void iteratePatterns( std::string row,
                          std::function< void( std::string const& ) > const& callback );
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
    iterateLines( fileStream,
                  [ & ]( auto const& line )
                  {
                      auto const pattern = std::regex{ R"(^([?\.#]+)\s+([\d,]+)$)" };
                      auto match = std::smatch{};
                      std::regex_match( line, match, pattern );

                      fmt::print( "Parsed: {} {}\n", match[ 1 ].str(), match[ 2 ].str() );

                      auto groups = std::vector< int >{};
                      iterateNumbers( match[ 2 ],
                                      [ & ]( auto num, auto s, auto l )
                                      {
                                          groups.push_back( num );
                                      } );

                      fmt::print( "Groups: {}\n", groups );

                      iteratePatterns( match[ 1 ],
                                       [ & ]( auto const& pattern )
                                       {
                                           if( matchesGroups( pattern, groups ) )
                                           {
                                               ++sum;
                                           }
                                       } );
                  } );

    fmt::print( "Sum: {}\n", sum );

    return EXIT_SUCCESS;
}

namespace
{
    bool matchesGroups( std::string const& pattern, std::vector< int > const& groups )
    {
        auto realGroups = std::vector< int >{};

        realGroups.push_back( 0 );

        for( std::size_t i = 0; i < pattern.length(); ++i )
        {
            if( pattern[ i ] == '#' )
            {
                ++realGroups.back();
            }
            else
            {
                realGroups.push_back( 0 );
            }
        }

        realGroups.erase( std::remove( std::begin( realGroups ), std::end( realGroups ), 0 ),
                          std::end( realGroups ) );

        return groups == realGroups;
    }

    void iteratePatterns( std::string row,
                          std::function< void( std::string const& ) > const& callback )
    {
        for( std::size_t i = 0; i < row.length(); ++i )
        {
            if( row[ i ] == '?' )
            {
                row[ i ] = '.';
                iteratePatterns( row, callback );
                row[ i ] = '#';
                iteratePatterns( row, callback );
                return;
            }
        }

        callback( row );
    }
}
