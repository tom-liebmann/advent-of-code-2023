#pragma once

#include <functional>
#include <iostream>


using LineCallback = std::function< void( std::string const& ) >;

void iterateLines( std::istream& stream, LineCallback const& callback );
