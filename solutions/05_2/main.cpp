#include <cstdlib>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <regex>
#include <unordered_set>


namespace
{
    struct Range
    {
        long start;
        long len;
    };

    struct MapRange
    {
        long srcStart;
        long dstStart;
        long len;

        static MapRange parse( std::string const& line );
    };

    using RangeCallback = std::function< void( Range const& ) >;

    struct Map
    {
        std::string name;

        std::vector< MapRange > ranges;

        void mapRange( Range range, RangeCallback const& callback ) const;

        static Map parse( std::string const& name, std::istream& stream );
    };

    std::vector< Range > parseSeeds( std::istream& stream );

    std::vector< Map > parseMaps( std::istream& stream );

    using NumberCallback = std::function< void( long, long ) >;
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
        auto seedRanges = std::vector< Range >{ seed };

        for( auto const& map : maps )
        {
            auto newSeedRanges = std::vector< Range >{};

            for( auto const& seedRange : seedRanges )
            {
                map.mapRange( seedRange,
                              [ & ]( auto const& newRange )
                              {
                                  newSeedRanges.push_back( newRange );
                              } );
            }

            std::swap( seedRanges, newSeedRanges );
        }

        for( auto const& seedRange : seedRanges )
        {
            lowestSeed = std::min( lowestSeed, seedRange.start );
        }
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

        std::sort( std::begin( ranges ),
                   std::end( ranges ),
                   []( auto const& lhs, auto const& rhs )
                   {
                       return lhs.srcStart < rhs.srcStart;
                   } );

        return Map{ name, ranges };
    }

    void Map::mapRange( Range range, RangeCallback const& callback ) const
    {
        for( auto const& mapRange : ranges )
        {
            if( range.len == 0 )
            {
                return;
            }

            if( range.start + range.len <= mapRange.srcStart )
            {
                callback( range );
                return;
            }

            if( range.start + range.len <= mapRange.srcStart + mapRange.len )
            {
                if( range.start >= mapRange.srcStart )
                {
                    //     |       |
                    //     |    |
                    callback(
                        Range{ range.start - mapRange.srcStart + mapRange.dstStart, range.len } );
                }
                else
                {
                    //     |       |
                    //   |     |
                    callback( Range{ range.start, mapRange.srcStart - range.start } );
                    callback(
                        Range{ mapRange.dstStart, range.start + range.len - mapRange.srcStart } );
                }
                return;
            }

            if( range.start < mapRange.srcStart + mapRange.len )
            {
                //       |       |
                //            |       |
                callback( Range{ range.start - mapRange.srcStart + mapRange.dstStart,
                                 mapRange.srcStart + mapRange.len - range.start } );
                range.len -= mapRange.srcStart + mapRange.len - range.start;
                range.start = mapRange.srcStart + mapRange.len;
            }
        }

        if( range.len != 0 )
        {
            callback( range );
        }
    }

    std::vector< Range > parseSeeds( std::istream& stream )
    {
        auto line = std::string{};
        if( !std::getline( stream, line ) )
        {
            throw std::runtime_error{ "Seeds missing" };
        }

        auto seeds = std::vector< Range >{};
        iterateNumbers( line,
                        [ & ]( auto start, auto len )
                        {
                            seeds.push_back( Range{ start, len } );
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
        auto const pattern = std::regex{ R"((\d+)\s+(\d+))" };

        auto begin = std::sregex_iterator{ std::begin( line ), std::end( line ), pattern };
        auto end = std::sregex_iterator{};

        for( auto i = begin; i != end; ++i )
        {
            auto const match = ( *i );
            callback( std::stol( match[ 1 ] ), std::stol( match[ 2 ] ) );
        }
    }
}
