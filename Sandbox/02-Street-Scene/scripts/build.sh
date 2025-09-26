#!/bin/bash

# Ask the user for the target platform
read -p "Are you compiling for Linux or Windows? (Enter 'L' for Linux or 'W' for Windows): " platform

# Convert the input to lowercase for case-insensitive comparison
platform=$(echo "$platform" | tr '[:upper:]' '[:lower:]')
src_dir="$PWD"

# Make asset dirs
cd $src_dir
echo "Making asset folders..."
mkdir -p "$src_dir/assets/models" "$src_dir/assets/shaders"
echo "Making asset folders done"

# Check the user input and print a message accordingly
if [ "$platform" = "l" ]; then
    echo "You are compiling for Linux."

    build_dir="$src_dir/build/linux"
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

    # Copy shaders from Popcorn dist to Sandbox bin
    shader_src="$src_dir/../../Engine/dist/linux/assets/shaders"
    shader_dest="$install_dir/assets/shaders"

    echo "Copying shaders from $shader_src to $shader_dest"
    mkdir -p "$shader_dest"
    cp -r "$shader_src/"* "$shader_dest/"

    echo -e "\033[36mShaders copied to Sandbox assets directory (Linux)\033[0m"

    # Copy models to dist folder
    models_src="$src_dir/assets/models"
    models_dest="$install_dir/assets/models"

    echo "Copying models from $models_src to $models_dest"
    mkdir -p "$models_dest"
    cp -r "$models_src/"* "$models_dest/"

    echo -e "\033[36mModels copied to Sandbox assets directory (Linux)\033[0m"

    echo $PWD

    echo -e "\033[32mProgram compiled successfully for linux\033[0m"

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

    # Copy shaders from Popcorn dist to Sandbox bin
    shader_src="$src_dir/../../Engine/dist/windows/assets/shaders"
    shader_dest="$install_dir/assets/shaders"

    echo "Copying shaders from $shader_src to $shader_dest"
    mkdir -p "$shader_dest"
    cp -r "$shader_src/"* "$shader_dest/"

    echo -e "\033[36mShaders copied to Sandbox assets directory (Windows)\033[0m"

    # Copy models to dist folder
    models_src="$src_dir/assets/models"
    models_dest="$install_dir/assets/models"
    mkdir -p "$models_dest"
    cp -r "$models_src/"* "$models_dest/"

    echo -e "\033[36mModels copied to Sandbox assets directory (Windows)\033[0m"

    echo $PWD

    echo -e "\033[32mProgram compiled successfully for windows\033[0m"

else
    echo "Invalid input. Please enter 'L' for Linux or 'W' for Windows."
fi

cd $src_dir
