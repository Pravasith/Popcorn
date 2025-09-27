#!/bin/bash

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

ask_which_platform() {
    read -p "Are you compiling for Linux or Windows? \
(Enter 'L' for Linux or 'W' for Windows): " platform

    # Lower case convert
    platform=$(echo -e "$platform" | tr '[:upper:]' '[:lower:]')

    if [ "$platform" = "l" ]; then
        :
    elif [ "$platform" = "w" ]; then
        :
    else
        echo -e "${RED}Invalid input. Please enter 'L' for Linux or \
'W' for Windows.${NC}"
        exit 1
    fi
}

make_required_dirs() {
    src_dir="$PWD"

    if [ "$platform" = "l" ]; then
        build_dir="$src_dir/build/linux"
        install_dir="$src_dir/dist/linux"
    else
        build_dir="$src_dir"/build/windows
        install_dir="$src_dir/dist/windows"
    fi

    # Build & install dirs
    mkdir -p "$build_dir"
    mkdir -p "$install_dir"

    # Make asset dirs
    echo -e "${YELLOW}Making asset folders...${NC}"
    mkdir -p "$src_dir/assets/models" "$src_dir/assets/shaders" \
        "$src_dir/assets/curves"
    echo -e "${GREEN}Making asset folders done${NC}"
}

ask_if_clean_build_needed() {
    read -p "Do you want a clean build? y/N " clean_build_prompt
    clean_build_prompt=$(echo -e "$clean_build_prompt" \
        | tr '[:upper:]' '[:lower:]')

    if [ "$clean_build_prompt" = 'y' ]; then
        echo -e "Performing a clean build..."
        # Nuke shit and remake
        rm -rf "$build_dir"
        rm -rf "$install_dir"
        make_required_dirs
    elif [ "$clean_build_prompt" = 'n' ]; then
        echo -e "${YELLOW}Performing a non-clean build...${NC}"
    else
        echo -e "${YELLOW}Performing a non-clean build...${NC}"
    fi
}

compile_game() {
    if [ "$platform" = "l" ]; then
        echo -e "${YELLOW}Compiling your game for Linux machines...${NC}"

        cd "$build_dir"

        cmake \
            -D CMAKE_INSTALL_PREFIX="$install_dir" \
            -D CMAKE_EXPORT_COMPILE_COMMANDS=ON \
            -D CMAKE_BUILD_TYPE=Debug \
            -S ../../ \
            -B .

        cp compile_commands.json "$src_dir"/compile_commands.json
        cmake --build . --target install
        echo -e "${GREEN}Compiling your game for Linux machines done${NC}"
    else
        echo -e "${YELLOW}Compiling your game for Windows machines...${NC}"

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
        echo -e "${GREEN}Compiling your game for Windows machines done${NC}"
    fi
}

copy_assets() {
    echo -e "${YELLOW}Copying assets to dist...${NC}"

    # Engine shaders
    if [ "$platform" = "l" ]; then
        engine_shaders_src="$src_dir/../../Engine/dist/linux/assets/shaders"
    else
        engine_shaders_src="$src_dir/../../Engine/dist/windows/assets/shaders"
    fi

    # Custom shaders
    custom_shaders_src="$src_dir/assets/shaders"

    shader_dest="$install_dir/assets/shaders"
    mkdir -p "$shader_dest"

    cp -r "$engine_shaders_src/." "$custom_shaders_src/." "$shader_dest/"

    # Copy models to dist folder
    models_src="$src_dir/assets/models"
    models_dest="$install_dir/assets/models"

    mkdir -p "$models_dest"
    cp -r "$models_src/." "$models_dest"

    # Copy curves to dist folder
    curves_src="$src_dir/assets/curves"
    curves_dest="$install_dir/assets/curves"

    mkdir -p "$curves_dest"
    cp -r "$curves_src/." "$curves_dest"

    echo -e "${GREEN}Copying assets to dist done${NC}"
}

run_script() {
    echo -e "${YELLOW}Running script...${NC}"

    ask_which_platform
    make_required_dirs
    ask_if_clean_build_needed
    compile_game
    copy_assets

    echo -e "${GREEN}Running script done${NC}"
}

run_script

cd "$src_dir" || exit 1
