#include <application.hpp>

#include <fstream>

#include <fmt/core.h>


int main( int argc, char** argv )
{
    auto const implFile = Application::getAppImplFile();

    for( auto const& [ filename, expectedResult ] : Application::expectedResults() )
    {
        auto const inputFile = implFile.parent_path() / filename;
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
