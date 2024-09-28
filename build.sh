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

    clean_build_prompt=$(echo "$clean_build_prompt" | tr '[:upper:]' '[:lower:]')
    read -p "Clean build? Y / N " clean_build_prompt

    if [ "$clean_build_prompt" = 'y' ]; then
        echo "Performing a clean build..."
        rm -rf *
    elif [ "$clean_build_prompt" = 'n' ]; then
        echo "Performing a non-clean build..."
    else
        echo "Invalid input. Y / N"
    fi

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

    clean_build_prompt=$(echo "$clean_build_prompt" | tr '[:upper:]' '[:lower:]')
    read -p "Clean build? Y / N " clean_build_prompt

    if [ "$clean_build_prompt" = 'y' ]; then
        echo "Performing a clean build..."
        rm -rf *
    elif [ "$clean_build_prompt" = 'n' ]; then
        echo "Performing a non-clean build..."
    else
        echo "Invalid input. Y / N"
    fi

    cmake -DCMAKE_TOOLCHAIN_FILE=../../tc-windows.cmake ../../
    make
    echo $PWD
    cd $src_dir

else
    echo "Invalid input. Please enter 'L' for Linux or 'W' for Windows."
fi
