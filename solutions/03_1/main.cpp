#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <regex>


namespace
{
    using LineCallback =
        std::function< void( std::string const&, std::string const&, std::string const& ) >;

    void iterateLines( std::string const& fileName, LineCallback const& callback );

    using NumberCallback = std::function< void( int, int, int ) >;

    void iterateNumbers( std::string const& line, NumberCallback const& callback );

    using SymbolCallback = std::function< void( int ) >;

    void iterateSymbols( std::string const& line, SymbolCallback const& callback );
}


int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        std::cerr << "Missing parameter: <input file>\n";
        return EXIT_FAILURE;
    }

    auto const fileName = std::string{ argv[ 1 ] };

    auto sum = 0L;

    iterateLines( fileName,
                  [ & ]( auto const& prevLine, auto const& curLine, auto const& nextLine )
                  {
                      std::cout << "Checking line: " << curLine << '\n';
                      iterateNumbers( curLine,
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
                                      } );
                  } );

    std::cout << "Sum: " << sum << '\n';

    return EXIT_SUCCESS;
}


namespace
{
    void iterateLines( std::string const& fileName, LineCallback const& callback )
    {
        auto fileStream = std::ifstream{ fileName };
        auto prevLine = std::string{};
        auto curLine = std::string{};
        auto nextLine = std::string{};

        auto line = std::string{};
        while( std::getline( fileStream, line ) )
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

    void iterateNumbers( std::string const& line, NumberCallback const& callback )
    {
        auto const pattern = std::regex{ R"(\d+)" };

        auto begin = std::sregex_iterator{ std::begin( line ), std::end( line ), pattern };
        auto end = std::sregex_iterator{};

        for( auto i = begin; i != end; ++i )
        {
            auto const match = ( *i );
            callback( std::stoi( match.str() ), match.position(), match.length() );
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
