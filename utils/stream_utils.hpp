#pragma once

#include <functional>
#include <iostream>

#include <std_generator.hpp>


using LineCallback = std::function< void( std::string const& ) >;

void iterateLines( std::istream& stream, LineCallback const& callback );

std::generator< std::string const& > readLines( std::istream& stream );
