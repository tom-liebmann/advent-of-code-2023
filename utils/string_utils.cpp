#include <string_utils.hpp>

#include <regex>
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


void iterateNumbers( std::string const& line, NumberCallback const& callback )
{
    auto const pattern = std::regex{ R"([-\d]+)" };

    auto begin = std::sregex_iterator{ std::begin( line ), std::end( line ), pattern };
    auto end = std::sregex_iterator{};

    for( auto match = begin; match != end; ++match )
    {
        callback( std::stol( match->str() ), match->position(), match->length() );
    }
}
