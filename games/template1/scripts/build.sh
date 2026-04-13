#!/bin/bash

mkdir -p build/linux
mkdir -p build/windows

build_dir="./build/linux"
build_type="Debug"

read -p "Are you compiling for Windows? Defaults to Linux. y/N: " \
    platform
platform=$(echo "$platform" | tr ':upper:' ':lower:')

read -p "Do you want a Release build? Defaults to Debug. y/N: " \
    release_build_flag
release_build_flag=$(echo "$release_build_flag" | tr ':upper:' ':lower:')
if [ "$release_build_flag" = "y" ]; then
    build_type="Release"
fi

if [ "$platform" = "y" ]; then
    cmake --preset GameWindowsCross -DCMAKE_BUILD_TYPE="$build_type"
    build_dir="./build/windows"
else
    cmake --preset GameLinux -DCMAKE_BUILD_TYPE="$build_type"
fi

cmake --debug-find -S . -B "$build_dir"
cmake --build "$build_dir"
cmake --install "$build_dir"

# TODO: Remove this, only need this for testing with wine.
# Should work out of the box without this in actual windows machine
cp /usr/lib/gcc/x86_64-w64-mingw32/13-win32/lib*.dll \
   ./games/template1/dist/windows/bin
