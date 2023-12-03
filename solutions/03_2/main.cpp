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

                                          iterateNumbers( prevLine, collectNumbers );
                                          iterateNumbers( curLine, collectNumbers );
                                          iterateNumbers( nextLine, collectNumbers );

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
            if( c == '*' )
            {
                callback( i );
            }
        }
    }
}
