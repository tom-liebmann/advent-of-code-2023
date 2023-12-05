#include <string_utils.hpp>

#include <sstream>


std::vector< std::string > split( std::string const& s, char delim )
{
    auto result = std::vector< std::string >{};
    auto stream = std::stringstream{ s };

    auto item = std::string{};
    while( getline( stream, item, delim ) )
    {
        result.push_back( item );
    }

    return result;
}
