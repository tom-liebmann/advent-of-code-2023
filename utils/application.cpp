#include <application.hpp>

#include <fstream>

#include <fmt/core.h>


int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        fmt::print( stderr, "Missing parameter: <input file>\n" );
        return EXIT_FAILURE;
    }

    auto const inputFile = Application::APP_IMPL_FILE.parent_path() / argv[ 1 ];
    auto inputStream = std::ifstream{ inputFile };

    auto const result = Application::computeResult( inputStream );

    fmt::print( "Result: {}\n", result );

    return EXIT_SUCCESS;
}
