#include <application.hpp>

#include <fstream>

#include <fmt/core.h>


int main( int argc, char** argv )
{
    for( auto const& [ filename, expectedResult ] : Application::EXPECTED_RESULTS )
    {
        auto const inputFile = Application::APP_IMPL_FILE.parent_path() / filename;
        auto inputStream = std::ifstream{ inputFile };
        auto const result = Application::computeResult( inputStream );

        if( result != expectedResult )
        {
            fmt::print( stderr, "Wrong result. Got {}, expected {}.\n", result, expectedResult );
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
