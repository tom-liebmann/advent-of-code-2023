#pragma once

#include <functional>
#include <string>
#include <vector>


std::vector< std::string > split( std::string const& s, char delim );


using NumberCallback = std::function< void( long, std::size_t, std::size_t ) >;

void iterateNumbers( std::string const& line,
                     NumberCallback const& callback,
                     bool withNegatives = true );
