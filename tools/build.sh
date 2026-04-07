#!/bin/bash

mkdir -p build

cmake --preset Popcorn
cmake -DCMAKE_BUILD_TYPE=DEBUG -S . -B ./build

cmake --build build
cmake --install build
