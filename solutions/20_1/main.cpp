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
    class Module;

    enum class PulseType
    {
        HIGH,
        LOW,
    };

    struct PulseInput
    {
        Module* source;
        Module* target;
        PulseType type;
    };

    struct Context
    {
        std::unordered_map< std::string, std::unique_ptr< Module > > modules;
        std::queue< PulseInput > pulseQueue;
    };

    class Module
    {
    public:
        static std::unique_ptr< Module > load( std::string const& name );

        Module( std::string name ) : m_name{ std::move( name ) }
        {
        }

        virtual ~Module() = default;

        std::string const& getName() const
        {
            return m_name;
        }

        virtual void addInput( Module* mod )
        {
            m_inputs.push_back( mod );
        }

        void addOutput( Module* mod )
        {
            m_outputs.push_back( mod );
        }

        virtual void processPulse( PulseInput const& input, Context& context ) = 0;

    protected:
        std::string m_name;
        std::vector< Module* > m_inputs;
        std::vector< Module* > m_outputs;
    };

    class Broadcaster : public Module
    {
    public:
        Broadcaster( std::string name ) : Module{ std::move( name ) }
        {
        }

        virtual void processPulse( PulseInput const& input, Context& context ) override
        {
            for( auto const output : m_outputs )
            {
                context.pulseQueue.push( PulseInput{ this, output, input.type } );
            }
        }
    };

    class FlipFlop : public Module
    {
    public:
        FlipFlop( std::string name ) : Module{ std::move( name ) }
        {
        }

        virtual void processPulse( PulseInput const& input, Context& context ) override
        {
            if( input.type == PulseType::HIGH )
            {
                return;
            }

            m_isOn = !m_isOn;

            for( auto const output : m_outputs )
            {
                context.pulseQueue.push(
                    PulseInput{ this, output, m_isOn ? PulseType::HIGH : PulseType::LOW } );
            }
        }

    private:
        bool m_isOn = false;
    };

    class Conjunction : public Module
    {
    public:
        Conjunction( std::string name ) : Module{ std::move( name ) }
        {
        }

        virtual void processPulse( PulseInput const& input, Context& context ) override
        {
            m_lastPulse.insert_or_assign( input.source, input.type );

            auto allHigh = true;

            for( auto const input : m_inputs )
            {
                auto const iter = m_lastPulse.find( input );
                auto const lastPulse =
                    iter == std::end( m_lastPulse ) ? PulseType::LOW : iter->second;

                if( lastPulse == PulseType::LOW )
                {
                    allHigh = false;
                    break;
                }
            }

            for( auto const output : m_outputs )
            {
                context.pulseQueue.push(
                    PulseInput{ this, output, allHigh ? PulseType::LOW : PulseType::HIGH } );
            }
        }

    private:
        std::unordered_map< Module*, PulseType > m_lastPulse;
    };

    class Untyped : public Module
    {
    public:
        Untyped( std::string name ) : Module{ std::move( name ) }
        {
        }

        virtual void processPulse( PulseInput const& input, Context& context ) override
        {
            // Do nothing
        }
    };

    Context load( std::istream& stream );
}


std::filesystem::path Application::APP_IMPL_FILE = __FILE__;

ExpectedResults Application::EXPECTED_RESULTS = {
    { "input_example.txt", 51 },
    { "input_final.txt", 7987 },
};

long Application::computeResult( std::istream& inputStream )
{
    auto context = load( inputStream );

    auto highCount = 0L;
    auto lowCount = 0L;

    for( int i = 0; i < 1000; ++i )
    {
        context.pulseQueue.push( PulseInput{
            nullptr,                                    // source
            context.modules.at( "broadcaster" ).get(),  // target
            PulseType::LOW                              // type
        } );


        while( !context.pulseQueue.empty() )
        {
            auto const input = context.pulseQueue.front();
            context.pulseQueue.pop();

            // fmt::print( "Processing {} {} {}\n",
            //             input.source == nullptr ? "Button" : input.source->getName(),
            //             input.target->getName(),
            //             input.type == PulseType::HIGH ? "high" : "low" );

            input.target->processPulse( input, context );

            if( input.type == PulseType::HIGH )
            {
                ++highCount;
            }
            else
            {
                ++lowCount;
            }
        }
    }

    return highCount * lowCount;
}


namespace
{
    Context load( std::istream& stream )
    {
        struct Connection
        {
            std::string source;
            std::string target;
        };

        auto const LINE_PATTERN = std::regex{ R"(^([^\s-]+)\s*->\s*(.*)$)" };
        auto const NAME_PATTERN = std::regex{ R"([^,\s]+)" };

        auto context = Context{};
        auto connections = std::vector< Connection >{};

        for( auto const line : readLines( stream ) )
        {
            auto match = std::smatch{};
            std::regex_match( line, match, LINE_PATTERN );
            auto mod = Module::load( match[ 1 ].str() );
            auto const targetNames = match[ 2 ].str();

            for( auto const& targetMatch : iterateMatches( targetNames, NAME_PATTERN ) )
            {
                connections.push_back( Connection{ mod->getName(), targetMatch.str() } );
            }

            auto const modName = mod->getName();
            context.modules.insert( { modName, std::move( mod ) } );
        }

        for( auto const& conn : connections )
        {
            auto const& source = context.modules.at( conn.source );
            auto const targetIter = context.modules.find( conn.target );

            if( targetIter == std::end( context.modules ) )
            {
                auto mod = std::make_unique< Untyped >( conn.target );
                context.modules.insert( { conn.target, std::move( mod ) } );
            }
            auto const& target = context.modules.at( conn.target );

            source->addOutput( target.get() );
            target->addInput( source.get() );
        }

        return context;
    }

    std::unique_ptr< Module > Module::load( std::string const& name )
    {
        if( name == "broadcaster" )
        {
            return std::make_unique< Broadcaster >( name );
        }

        if( name[ 0 ] == '%' )
        {
            return std::make_unique< FlipFlop >( name.substr( 1 ) );
        }

        if( name[ 0 ] == '&' )
        {
            return std::make_unique< Conjunction >( name.substr( 1 ) );
        }

        throw std::runtime_error( fmt::format( "Unknown module type {}", name ) );
    }
}
