#pragma once

#include <filesystem>
#include <unordered_map>


class Application
{
public:
    static std::filesystem::path getAppImplFile();

    static std::unordered_map< std::string, long > expectedResults();

    static long computeResult( std::istream& inputStream );
};

int main( int argc, char** argv );
