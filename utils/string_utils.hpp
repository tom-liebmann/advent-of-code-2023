#pragma once

#include <functional>
#include <regex>
#include <string>
#include <vector>

#include <std_generator.hpp>


std::vector< std::string > split( std::string const& s, char delim );


std::generator< std::smatch const& > iterateMatches( std::string const& line,
                                                     std::regex const& pattern );

using NumberCallback = std::function< void( long, std::size_t, std::size_t ) >;

void iterateNumbers( std::string const& line,
                     NumberCallback const& callback,
                     bool withNegatives = true );
