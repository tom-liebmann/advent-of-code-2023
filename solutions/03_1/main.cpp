#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <regex>

#include <fmt/core.h>

#include <utils.hpp>


namespace
{
    using LineCallback =
        std::function< void( std::string const&, std::string const&, std::string const& ) >;

    void iterateLines( std::istream& inputStream, LineCallback const& callback );

    using SymbolCallback = std::function< void( int ) >;

    void iterateSymbols( std::string const& line, SymbolCallback const& callback );
}


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 4351 },
    { "input_final.txt", 557705 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto sum = 0L;

    iterateLines( inputStream,
                  [ & ]( auto const& prevLine, auto const& curLine, auto const& nextLine )
                  {
                      std::cout << "Checking line: " << curLine << '\n';
                      iterateNumbers(
                          curLine,
                          [ & ]( auto num, auto pos, auto len )
                          {
                              auto isValid = false;

                              auto const updateValidity = [ & ]( auto symbPos )
                              {
                                  if( symbPos >= pos - 1 && symbPos <= pos + len )
                                  {
                                      isValid = true;
                                  }
                              };

                              iterateSymbols( prevLine, updateValidity );
                              iterateSymbols( curLine, updateValidity );
                              iterateSymbols( nextLine, updateValidity );

                              if( isValid )
                              {
                                  std::cout << "Adding " << num << '\n';
                                  sum += num;
                              }
                          },
                          false );
                  } );

    return sum;
}


namespace
{
    void iterateLines( std::istream& inputStream, LineCallback const& callback )
    {
        auto prevLine = std::string{};
        auto curLine = std::string{};
        auto nextLine = std::string{};

        auto line = std::string{};
        while( std::getline( inputStream, line ) )
        {
            prevLine = curLine;
            curLine = nextLine;
            nextLine = line;

            if( !curLine.empty() )
            {
                callback( prevLine, curLine, nextLine );
            }
        }

        if( !curLine.empty() )
        {
            callback( curLine, nextLine, "" );
        }
    }

    void iterateSymbols( std::string const& line, SymbolCallback const& callback )
    {
        for( std::size_t i = 0; i < line.size(); ++i )
        {
            auto const c = line[ i ];
            if( !std::isdigit( c ) && c != '.' )
            {
                callback( i );
            }
        }
    }
}
