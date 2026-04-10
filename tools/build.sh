#!/bin/bash

mkdir -p build/linux
mkdir -p build/windows

read -p "Are you compiling for Windows? Defaults to Linux. y/N" platform
platform=$(echo "$platform" | tr ':upper:' ':lower:')
build_dir="./build/linux"

if [ "$platform" = "y" ]; then
    cmake --preset PopcornWindowsCross
    build_dir="./build/windows"
else
    cmake --preset PopcornLinux
fi

cmake -DCMAKE_BUILD_TYPE=DEBUG -S . -B "$build_dir"
cmake --build "$build_dir"
cmake --install "$build_dir"
