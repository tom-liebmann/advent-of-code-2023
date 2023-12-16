#include <grid.hpp>

#include <stream_utils.hpp>


Grid< char > readGrid( std::istream& stream )
{
    auto values = std::vector< char >{};
    auto width = 0uz;
    auto height = 0uz;
    for( auto const& line : readLines( stream ) )
    {
        width = line.length();
        ++height;
        values.insert( std::end( values ), std::begin( line ), std::end( line ) );
    }
    return { width, height, std::move( values ) };
}
