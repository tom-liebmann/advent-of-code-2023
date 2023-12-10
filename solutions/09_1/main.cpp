#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <regex>
#include <unordered_set>


#include <stream_utils.hpp>
#include <string_utils.hpp>


namespace
{
    struct History
    {
    public:
        std::vector< std::vector< long > > rows;

        History( std::vector< long > row )
        {
            rows.push_back( row );
            while( !std::all_of( std::begin( row ),
                                 std::end( row ),
                                 []( auto v )
                                 {
                                     return v == 0;
                                 } ) )
            {
                row = computeDiff( row );
                fmt::print( "Pushing: {}\n", row );
                rows.push_back( row );
            }
        }

        void extendByOne()
        {
            rows.back().push_back( 0 );
            for( std::size_t i = 0; i < rows.size() - 1; ++i )
            {
                auto const& nextRow = rows[ rows.size() - i - 1 ];
                auto& row = rows[ rows.size() - i - 2 ];

                row.push_back( row.back() + nextRow.back() );
            }
        }

    private:
        static std::vector< long > computeDiff( std::vector< long > row )
        {
            auto diff = std::vector< long >{};

            for( std::size_t i = 1; i < row.size(); ++i )
            {
                diff.push_back( row[ i ] - row[ i - 1 ] );
            }

            return diff;
        }
    };
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
                      auto firstRow = std::vector< long >{};
                      iterateNumbers( line,
                                      [ & ]( auto num, auto start, auto end )
                                      {
                                          firstRow.push_back( num );
                                      } );

                      auto hist = History{ std::move( firstRow ) };
                      hist.extendByOne();

                      sum += hist.rows[ 0 ].back();
                  } );

    std::cout << "Result: " << sum << '\n';

    return EXIT_SUCCESS;
}

namespace
{
}
