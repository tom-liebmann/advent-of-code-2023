# Description

This repository contains solutions to the [Advent of Code 2023](https://adventofcode.com/2023).

# Build Instructions

Execute the following commands to build all solutions in this repository:
```console
# Create build directory
mkdir build
cd build

# Download and build dependencies
conan install .. --output-folder=. --build=missing

# Generate Makefiles
cmake ..

# Build solutions
make -j$(nproc)
```
