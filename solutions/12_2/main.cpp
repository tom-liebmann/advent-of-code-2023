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
    long countValid( std::string const& row,
                     int rowOffset,
                     int current,
                     std::vector< int > const& groups,
                     int groupOffset,
                     std::vector< int > const& groupSums );
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

                      auto groups = std::vector< int >{};
                      iterateNumbers( match[ 2 ],
                                      [ & ]( auto num, auto s, auto l )
                                      {
                                          groups.push_back( num );
                                      } );

                      auto realGroups = groups;
                      for( int i = 0; i < 4; ++i )
                      {
                          for( auto j : groups )
                          {
                              realGroups.push_back( j );
                          }
                      }

                      auto row = match[ 1 ].str();
                      row = row + "?" + row + "?" + row + "?" + row + "?" + row;

                      fmt::print( "Row: {}\n", row );

                      auto groupSums = std::vector< int >( realGroups.size(), 0 );

                      for( int i = 0; i < realGroups.size(); ++i )
                      {
                          for( int j = 0; j <= i; ++j )
                          {
                              groupSums[ j ] += realGroups[ i ];
                          }
                      }

                      auto const lineValid = countValid( row, 0, 0, realGroups, 0, groupSums );
                      fmt::print( "Valid: {}\n", lineValid );
                      sum += lineValid;
                  } );

    fmt::print( "Sum: {}\n", sum );

    return EXIT_SUCCESS;
}

namespace
{
    long countValid( std::string const& row,
                     int rowOffset,
                     int current,
                     std::vector< int > const& groups,
                     int groupOffset,
                     std::vector< int > const& groupSums )
    {
        if( groupOffset > groups.size() )
        {
            return 0;
        }

        if( rowOffset == row.length() )
        {
            if( ( current == 0 && groupOffset == groups.size() ) ||
                ( groupOffset == groups.size() - 1 && current == groups.back() ) )
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }

        if( groupOffset < groups.size() &&
            ( groups.size() - groupOffset ) * 2 - 1 > row.length() - rowOffset + 1 )
        {
            return 0;
        }

        if( groupOffset < groups.size() &&
            groupSums[ groupOffset ] - current + ( groups.size() - groupOffset - 1 ) >
                row.length() - rowOffset )
        {
            return 0;
        }

        auto const r = row[ rowOffset ];

        if( r == '.' )
        {
            if( current == 0 )
            {
                return countValid( row, rowOffset + 1, 0, groups, groupOffset, groupSums );
            }
            else
            {
                if( groupOffset < groups.size() && current == groups[ groupOffset ] )
                {
                    return countValid( row, rowOffset + 1, 0, groups, groupOffset + 1, groupSums );
                }
                else
                {
                    return 0;
                }
            }
        }

        if( r == '#' )
        {
            if( groupOffset >= groups.size() )
            {
                return 0;
            }

            if( current + 1 <= groups[ groupOffset ] )
            {
                return countValid(
                    row, rowOffset + 1, current + 1, groups, groupOffset, groupSums );
            }
            else
            {
                return 0;
            }
        }

        if( r == '?' )
        {
            auto result = 0L;

            // '#'
            if( groupOffset < groups.size() && current + 1 <= groups[ groupOffset ] )
            {
                result +=
                    countValid( row, rowOffset + 1, current + 1, groups, groupOffset, groupSums );
            }

            // '.'
            if( current == 0 )
            {
                result += countValid( row, rowOffset + 1, 0, groups, groupOffset, groupSums );
            }
            else
            {
                if( groupOffset < groups.size() && current == groups[ groupOffset ] )
                {
                    result +=
                        countValid( row, rowOffset + 1, 0, groups, groupOffset + 1, groupSums );
                }
            }

            return result;
        }

        throw std::runtime_error( "Unhandled" );
    }
}
