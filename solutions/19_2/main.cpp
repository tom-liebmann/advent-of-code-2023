#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <queue>
#include <regex>
#include <set>
#include <thread>
#include <unordered_set>

#include <omp.h>

#include <utils.hpp>


namespace
{
    struct Rule
    {
        std::string prop;
        char op;
        long num;
        std::string target;
    };

    struct Workflow
    {
        std::vector< Rule > rules;
        std::string def;
    };

    struct Object
    {
        std::unordered_map< std::string, long > props;
    };

    long countAccepted( std::unordered_map< std::string, Workflow > const& workflows,
                        std::unordered_map< std::string, std::pair< long, long > >& ranges,
                        std::string const& name );
}


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 51 },
    { "input_final.txt", 7987 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto const PATTERN_WORKFLOW = std::regex{ R"((\w+)\{(.*),(\w+)\})" };
    auto const PATTERN_RULE = std::regex{ R"((\w+)([<>])([-\d]+):(\w+))" };
    auto const PATTERN_PROPS = std::regex{ R"((\w+)=([-\d]+))" };

    auto workflows = std::unordered_map< std::string, Workflow >{};

    for( auto const& line : readLines( inputStream ) )
    {
        auto match = std::smatch{};
        if( !std::regex_match( line, match, PATTERN_WORKFLOW ) )
        {
            break;
        }

        auto const name = match[ 1 ].str();
        auto const ruleStr = match[ 2 ].str();
        auto const def = match[ 3 ].str();

        auto rules = std::vector< Rule >{};

        auto const begin =
            std::sregex_iterator{ std::begin( ruleStr ), std::end( ruleStr ), PATTERN_RULE };
        auto const end = std::sregex_iterator{};
        for( auto iter = begin; iter != end; ++iter )
        {
            auto const prop = ( *iter )[ 1 ].str();
            auto const op = ( *iter )[ 2 ].str();
            auto const num = ( *iter )[ 3 ].str();
            auto const target = ( *iter )[ 4 ].str();

            rules.push_back( Rule{ prop, op[ 0 ], std::stol( num ), target } );
        }

        workflows.insert( { name, Workflow{ std::move( rules ), def } } );
    }

    auto objects = std::vector< Object >{};
    for( auto const& line : readLines( inputStream ) )
    {
        auto props = std::unordered_map< std::string, long >{};

        auto const begin =
            std::sregex_iterator{ std::begin( line ), std::end( line ), PATTERN_PROPS };
        auto const end = std::sregex_iterator{};
        for( auto iter = begin; iter != end; ++iter )
        {
            auto const name = ( *iter )[ 1 ].str();
            auto const value = ( *iter )[ 2 ].str();
            props.insert( { name, std::stol( value ) } );
        }

        objects.push_back( Object{ std::move( props ) } );
    }

    auto ranges = std::unordered_map< std::string, std::pair< long, long > >{
        { "x", { 1, 4000 } },
        { "m", { 1, 4000 } },
        { "a", { 1, 4000 } },
        { "s", { 1, 4000 } },
    };

    return countAccepted( workflows, ranges, "in" );
}


namespace
{
    long countAccepted( std::unordered_map< std::string, Workflow > const& workflows,
                        std::unordered_map< std::string, std::pair< long, long > >& ranges,
                        std::string const& name )
    {
        if( name == "A" )
        {
            auto sum = 1L;
            for( auto range : ranges )
            {
                sum *= std::max( 0L, range.second.second - range.second.first + 1 );
            }
            return sum;
        }

        if( name == "R" )
        {
            return 0L;
        }

        auto const& workflow = workflows.at( name );

        auto myRanges = ranges;

        auto sum = 0L;
        for( auto const& rule : workflow.rules )
        {
            auto& propRange = myRanges.at( rule.prop );
            if( rule.op == '>' )
            {
                auto const oldVal = propRange.first;
                propRange.first = std::max( rule.num + 1, oldVal );
                sum += countAccepted( workflows, myRanges, rule.target );
                propRange.first = oldVal;
                propRange.second = rule.num;
            }
            else
            {
                auto const oldVal = propRange.second;
                propRange.second = std::min( rule.num - 1, oldVal );
                sum += countAccepted( workflows, myRanges, rule.target );
                propRange.second = oldVal;
                propRange.first = rule.num;
            }
        }

        sum += countAccepted( workflows, myRanges, workflow.def );

        return sum;
    }
}
