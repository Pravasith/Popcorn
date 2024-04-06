#!/bin/bash

# Ask the user for the target platform
read -p "Are you compiling for Linux or Windows? (Enter 'L' for Linux or 'W' for Windows): " platform

# Convert the input to lowercase for case-insensitive comparison
platform=$(echo "$platform" | tr '[:upper:]' '[:lower:]')

# Check the user input and print a message accordingly
if [ "$platform" = "l" ]; then
    echo "You are compiling for Linux."
    cd "$PWD"/build/linux
    rm -rf *
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../../
    make
elif [ "$platform" = "w" ]; then
    echo "You are compiling for Windows."
    cd "$PWD"/build/win64
    rm -rf *
    cmake -DCMAKE_TOOLCHAIN_FILE=../../tc-windows.cmake ../../
    make
else
    echo "Invalid input. Please enter 'L' for Linux or 'W' for Windows."
fi
