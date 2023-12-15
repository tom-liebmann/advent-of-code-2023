#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <regex>

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
    { "input_example.txt", 467835 },
    { "input_final.txt", 84266818 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto sum = 0L;

    iterateLines( inputStream,
                  [ & ]( auto const& prevLine, auto const& curLine, auto const& nextLine )
                  {
                      iterateSymbols( curLine,
                                      [ & ]( auto symPos )
                                      {
                                          auto numbers = std::vector< int >{};

                                          auto const collectNumbers =
                                              [ & ]( auto num, auto pos, auto len )
                                          {
                                              if( pos <= symPos + 1 && pos + len >= symPos )
                                              {
                                                  numbers.push_back( num );
                                              }
                                          };

                                          iterateNumbers( prevLine, collectNumbers, false );
                                          iterateNumbers( curLine, collectNumbers, false );
                                          iterateNumbers( nextLine, collectNumbers, false );

                                          if( numbers.size() == 2 )
                                          {
                                              auto product = 1;
                                              for( auto const num : numbers )
                                              {
                                                  product *= num;
                                              }

                                              sum += product;
                                          }
                                      } );
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
            if( c == '*' )
            {
                callback( i );
            }
        }
    }
}
