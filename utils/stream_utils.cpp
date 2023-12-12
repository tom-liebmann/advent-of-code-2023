#include <stream_utils.hpp>

void iterateLines( std::istream& stream, LineCallback const& callback )
{
    auto line = std::string{};
    while( std::getline( stream, line ) )
    {
        callback( line );
    }
}

std::generator< std::string const& > readLines( std::istream& stream )
{
    auto line = std::string{};

    while( std::getline( stream, line ) )
    {
        co_yield line;
    }
}
