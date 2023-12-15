#pragma once

#include <filesystem>
#include <unordered_map>


using ExpectedResults = std::unordered_map< std::string, long >;

class Application
{
public:
    static std::filesystem::path APP_IMPL_FILE;

    static ExpectedResults EXPECTED_RESULTS;

    static long computeResult( std::istream& inputStream );
};

int main( int argc, char** argv );
