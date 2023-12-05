#include <stream_utils.hpp>

void iterateLines( std::istream& stream, LineCallback const& callback )
{
    auto line = std::string{};
    while( std::getline( stream, line ) )
    {
        callback( line );
    }
}
