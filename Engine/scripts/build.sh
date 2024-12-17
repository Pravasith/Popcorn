#!/bin/bash

# Ask the user for the target platform
read -p "Are you compiling for Linux or Windows? (Enter 'L' for Linux or 'W' for Windows): " platform

# Convert the input to lowercase for case-insensitive comparison
platform=$(echo "$platform" | tr '[:upper:]' '[:lower:]')
src_dir="$PWD"


# Check the user input and print a message accordingly
if [ "$platform" = "l" ]; then
    echo "You are compiling for Linux."
    mkdir -p "$src_dir"/build
    mkdir -p "$src_dir"/build/linux

    cd "$src_dir"/build/linux

    read -p "Clean build? y/N " clean_build_prompt
    clean_build_prompt=$(echo "$clean_build_prompt" | tr '[:upper:]' '[:lower:]')

    if [ "$clean_build_prompt" = 'y' ]; then
        echo "Performing a clean build..."
        rm -rf *
    elif [ "$clean_build_prompt" = 'n' ]; then
        echo "Performing a non-clean build..."
    else
        echo "Performing a non-clean build..."
    fi

    cmake \
        -D CMAKE_INSTALL_PREFIX="$src_dir/build/linux" \
        -D CMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -D CMAKE_BUILD_TYPE=Debug \
        -S ../../ \
        -B .

    cp compile_commands.json "$src_dir"/compile_commands.json

    cmake --build . --target install
    # make

    echo $PWD
    cd $src_dir

elif [ "$platform" = "w" ]; then
    echo "You are compiling for Windows."
    mkdir -p "$src_dir"/build
    mkdir -p "$src_dir"/build/windows

    cd "$src_dir"/build/windows

    read -p "Clean build? y/N " clean_build_prompt
    clean_build_prompt=$(echo "$clean_build_prompt" | tr '[:upper:]' '[:lower:]')

    if [ "$clean_build_prompt" = 'y' ]; then
        echo "Performing a clean build..."
        rm -rf *
    elif [ "$clean_build_prompt" = 'n' ]; then
        echo "Performing a non-clean build..."
    else
        echo "Performing a non-clean build..."
    fi

    cmake \
        -D CMAKE_TOOLCHAIN_FILE=../../toolchain-win.cmake \
        -D CMAKE_BUILD_TYPE=Release \
        ../../
        # ADD BELOW FLAG FOR RELEASE BUILDS
        # -D CMAKE_BUILD_TYPE=Release \

    make
    echo $PWD
    cd $src_dir

else
    echo "Invalid input. Please enter 'L' for Linux or 'W' for Windows."
fi
