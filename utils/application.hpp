#pragma once

#include <filesystem>


class Application
{
public:
    static std::filesystem::path getAppImplFile();

    static long computeResult( std::istream& inputStream );
};

int main( int argc, char** argv );
