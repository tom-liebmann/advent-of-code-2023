#include <cstdlib>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <regex>
#include <unordered_set>


namespace
{
    struct MapRange
    {
        long srcStart;
        long dstStart;
        long len;

        static MapRange parse( std::string const& line );

        std::pair< bool, long > mapValue( long value ) const;
    };

    struct Map
    {
        std::string name;

        std::vector< MapRange > ranges;

        long mapValue( long value ) const;

        static Map parse( std::string const& name, std::istream& stream );
    };

    std::vector< long > parseSeeds( std::istream& stream );

    std::vector< Map > parseMaps( std::istream& stream );

    using NumberCallback = std::function< void( long, int, int ) >;
    void iterateNumbers( std::string const& line, NumberCallback const& callback );
}


int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        std::cerr << "Missing parameter: <input file>\n";
        return EXIT_FAILURE;
    }

    auto const fileName = std::string{ argv[ 1 ] };
    auto fileStream = std::ifstream{ fileName };

    auto const seeds = parseSeeds( fileStream );
    auto const maps = parseMaps( fileStream );

    auto lowestSeed = std::numeric_limits< long >::max();

    for( auto const seed : seeds )
    {
        auto value = seed;
        for( auto const& map : maps )
        {
            value = map.mapValue( value );
        }

        lowestSeed = std::min( value, lowestSeed );
    }

    std::cout << "Lowest seed: " << lowestSeed << '\n';

    return EXIT_SUCCESS;
}


namespace
{
    MapRange MapRange::parse( std::string const& line )
    {
        auto const pattern = std::regex{ R"(^\s*(\d+)\s+*(\d+)\s+(\d+)\s*$)" };
        auto match = std::smatch{};

        if( !std::regex_match( line, match, pattern ) )
        {
            throw std::runtime_error( fmt::format( "Range does not match: {}", line ) );
        }

        return MapRange{ std::stol( match[ 2 ] ),
                         std::stol( match[ 1 ] ),
                         std::stol( match[ 3 ] ) };
    }

    std::pair< bool, long > MapRange::mapValue( long value ) const
    {
        if( value >= srcStart && value < srcStart + len )
        {
            return { true, dstStart + value - srcStart };
        }

        return { false, value };
    }

    Map Map::parse( std::string const& name, std::istream& stream )
    {
        auto ranges = std::vector< MapRange >{};
        auto line = std::string{};
        while( std::getline( stream, line ) )
        {
            if( line.empty() )
            {
                break;
            }

            ranges.push_back( MapRange::parse( line ) );
        }

        return Map{ name, ranges };
    }

    long Map::mapValue( long value ) const
    {
        for( auto const& range : ranges )
        {
            auto [ mapped, newValue ] = range.mapValue( value );

            value = newValue;

            if( mapped )
            {
                break;
            }
        }

        return value;
    }

    std::vector< long > parseSeeds( std::istream& stream )
    {
        auto line = std::string{};
        if( !std::getline( stream, line ) )
        {
            throw std::runtime_error{ "Seeds missing" };
        }

        auto seeds = std::vector< long >{};
        iterateNumbers( line,
                        [ & ]( auto num, auto start, auto len )
                        {
                            seeds.push_back( num );
                        } );

        return seeds;
    }

    std::vector< Map > parseMaps( std::istream& stream )
    {
        auto maps = std::vector< Map >{};

        auto const mapPattern = std::regex{ R"(^(.*) map:$)" };
        auto line = std::string{};
        while( std::getline( stream, line ) )
        {
            auto match = std::smatch{};
            if( std::regex_match( line, match, mapPattern ) )
            {
                maps.push_back( Map::parse( match[ 1 ], stream ) );
            }
        }

        return maps;
    }

    void iterateNumbers( std::string const& line, NumberCallback const& callback )
    {
        auto const pattern = std::regex{ R"(\d+)" };

        auto begin = std::sregex_iterator{ std::begin( line ), std::end( line ), pattern };
        auto end = std::sregex_iterator{};

        for( auto i = begin; i != end; ++i )
        {
            auto const match = ( *i );
            callback( std::stol( match.str() ), match.position(), match.length() );
        }
    }
}
