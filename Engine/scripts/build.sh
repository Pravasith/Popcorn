#!/bin/bash

# Ask the user for the target platform
read -p "Are you compiling for Linux or Windows? (Enter 'L' for Linux or 'W' for Windows): " platform

# Convert the input to lowercase for case-insensitive comparison
platform=$(echo "$platform" | tr '[:upper:]' '[:lower:]')
src_dir="$PWD"

# Check the user input and print a message accordingly
if [ "$platform" = "l" ]; then
    echo "You are compiling for Linux."

    build_dir="$src_dir"/build/linux
    install_dir="$src_dir/dist/linux"

    mkdir -p "$build_dir"
    mkdir -p "$install_dir"

    read -p "Clean build? y/N " clean_build_prompt
    clean_build_prompt=$(echo "$clean_build_prompt" | tr '[:upper:]' '[:lower:]')

    if [ "$clean_build_prompt" = 'y' ]; then
        echo "Performing a clean build..."
        rm -rf "$build_dir"
        rm -rf "$install_dir"
    elif [ "$clean_build_prompt" = 'n' ]; then
        echo "Performing a non-clean build..."
    else
        echo "Performing a non-clean build..."
    fi

    mkdir -p "$build_dir"
    mkdir -p "$install_dir"
    cd "$build_dir"

    cmake \
        -D CMAKE_INSTALL_PREFIX="$install_dir" \
        -D CMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -D CMAKE_BUILD_TYPE=Debug \
        -S ../../ \
        -B .

    cp compile_commands.json "$src_dir"/compile_commands.json

    cmake --build . --target install

    echo $PWD
    cd $src_dir

elif [ "$platform" = "w" ]; then
    echo "You are compiling for Windows."

    build_dir="$src_dir"/build/windows
    install_dir="$src_dir/dist/windows"

    mkdir -p "$build_dir"
    mkdir -p "$install_dir"

    read -p "Clean build? y/N " clean_build_prompt
    clean_build_prompt=$(echo "$clean_build_prompt" | tr '[:upper:]' '[:lower:]')

    if [ "$clean_build_prompt" = 'y' ]; then
        echo "Performing a clean build..."
        rm -rf "$build_dir"
        rm -rf "$install_dir"
    elif [ "$clean_build_prompt" = 'n' ]; then
        echo "Performing a non-clean build..."
    else
        echo "Performing a non-clean build..."
    fi

    mkdir -p "$build_dir"
    mkdir -p "$install_dir"
    cd "$build_dir"

    cmake \
        -D CMAKE_INSTALL_PREFIX="$install_dir" \
        -D CMAKE_TOOLCHAIN_FILE=../../toolchain-win.cmake \
        -D CMAKE_BUILD_TYPE=Release \
        -S ../../ \
        -B .
        # ADD BELOW FLAG FOR RELEASE BUILDS
        # -D CMAKE_BUILD_TYPE=Release \

    cmake --build . --target install

    echo $PWD
    cd $src_dir

else
    echo "Invalid input. Please enter 'L' for Linux or 'W' for Windows."
fi
