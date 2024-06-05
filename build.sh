#!/bin/bash

# Ask the user for the target platform
read -p "Are you compiling for Linux or Windows? (Enter 'L' for Linux or 'W' for Windows): " platform

# Convert the input to lowercase for case-insensitive comparison
platform=$(echo "$platform" | tr '[:upper:]' '[:lower:]')
src_dir=$PWD

# Check the user input and print a message accordingly
if [ "$platform" = "l" ]; then
    echo "You are compiling for Linux."
    mkdir -p "$src_dir"/build
    mkdir -p "$src_dir"/build/linux

    cd "$src_dir"/build/linux

    rm -rf *
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../../
    cp compile_commands.json "$src_dir"/compile_commands.json
    make
    echo $PWD
    cd $src_dir

elif [ "$platform" = "w" ]; then
    echo "You are compiling for Windows."
    mkdir -p "$src_dir"/build
    mkdir -p "$src_dir"/build/windows

    cd "$src_dir"/build/windows

    rm -rf *
    cmake -DCMAKE_TOOLCHAIN_FILE=../../tc-windows.cmake ../../
    make
    echo $PWD
    cd $src_dir

else
    echo "Invalid input. Please enter 'L' for Linux or 'W' for Windows."
fi
