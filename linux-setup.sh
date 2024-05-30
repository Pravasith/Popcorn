#!/bin/bash


check_create_folder() {
    local folder="$1"

    # Check if the folder exists
    if [ ! -d "$folder" ]; then
        # If the folder doesn't exist, create it
        if [ $# -eq 1 ]; then
            mkdir "$folder"
            echo "Folder '$folder' created."
        else
            if [ "$2" -eq 1 ]; then
                sudo mkdir "$folder"
                echo "Folder '$folder' created."
            fi
        fi
    else
        echo "Folder '$folder' already exists. Skipped creating."
    fi
}

# EXTERNAL DEPENDENCIES
# sudo apt install libwayland-dev libxkbcommon-dev xorg-dev

echo "--- Starting environment setup for Linux based systems..."
# DIR -- PROJECT PARENT SRC DIR
curr_dir=${PWD}

# DIR -- RAW GITHUB CLONED SUBMODULES
submodules_dir="$curr_dir/submodules"
# NOTE: THIS DIR ALREADY EXISTS
# check_create_folder "$submodules_dir"

# DIR -- BASE -- 3RD PARTY INCL & SRC FILES
vendor_dir="$curr_dir/third-party"
check_create_folder "$vendor_dir"

# DIR -- PLATFORM SPECIFIC 3RD PARTY INCL & SRC FILES
vendor_windows_dir="$vendor_dir/windows"
check_create_folder "$vendor_windows_dir"

vendor_linux_dir="$vendor_dir/linux"
check_create_folder "$vendor_linux_dir"

vendor_mac_os_dir="$vendor_dir/mac-os"
check_create_folder "$vendor_mac_os_dir"

# INSTALL SUBMODULES
echo "Cloning vendor/third-party submodules..."
git submodule init
git submodule update
echo "Cloning vendor/third-party submodules complete"

echo "Installing vendor/third-party submodules..."

echo "Building GLFW..."
glfw_submodule_dir="$submodules_dir/glfw"

glfw_submodule_build_dir="$glfw_submodule_dir/build"
check_create_folder "$glfw_submodule_build_dir"

echo "Building GLFW for Linux..."

# CREATE GLFW SUBMODULE BUILD DIR FOR LINUX
glfw_submodule_build_linux_dir="$glfw_submodule_build_dir/linux"
check_create_folder "$glfw_submodule_build_linux_dir"

# CREATE GLFW VENDOR DIR FOR LINUX
glfw_vendor_linux_dir="$vendor_linux_dir/glfw"
check_create_folder "$glfw_vendor_linux_dir"

cmake -DCMAKE_INSTALL_PREFIX="$glfw_vendor_linux_dir" -S "$glfw_submodule_dir" -B "$glfw_submodule_build_linux_dir"

cd "$glfw_submodule_build_linux_dir"
make install

echo "Building GLFW for Linux complete"
echo "Building GLFW complete"

echo "Installing vendor/third-party submodules complete"


# # INSTALL GLFW
# echo "Installing GLFW..."
# glfw_build_dir="$glfw_src_dir/build"
#
#
# check_create_folder "$glfw_build_dir"
# cd "$glfw_build_dir"
# cmake ..
# sudo make install
# cd "$curr_dir"
#
# # INSTALL GLFW FOR WINDOWS
# echo "Generating Win64 Binaries..."
# glfw_win64_build_dir="$glfw_src_dir/win64-build"
# glfw_win64_install_dir="$win64_dir/glfw"
#
# check_create_folder "$glfw_win64_build_dir"
# cmake -DCMAKE_INSTALL_PREFIX="$glfw_win64_install_dir" -S "$glfw_src_dir" -B "$glfw_win64_build_dir" -D CMAKE_TOOLCHAIN_FILE=CMake/x86_64-w64-mingw32.cmake
#
# check_create_folder "$glfw_win64_install_dir"
# cd "$glfw_win64_build_dir"
# sudo make install
#
# echo "Generating Win64 Binaries complete"
# cd "$curr_dir"
#
# echo "Installing GLFW complete"
#
# # INSTALL GLAD
# echo "Installing GLAD..."
# glad_src_dir="$vendor/glad"
# glad_win64_dir="$win64_dir/glad"
#
# check_create_folder "$glad_src_dir"
# check_create_folder "$glad_win64_dir"
#
# cp "$repos_dir/glad/src/glad.c" "$glad_src_dir/glad.c"
# cp -r "$repos_dir/glad/include" "$glad_win64_dir"
#
# glad_install_dir="/usr/include/glad"
# khr_install_dir="/usr/include/glad"
#
# # 2nd arg 1 means sudo
# check_create_folder "$glad_install_dir" 1
# sudo cp "$repos_dir/glad/include/glad/glad.h" "$glad_install_dir/glad.h"
#
# echo "Installing GLAD complete"
#
# echo "Copying CMakeLists.txt to third-party..."
# cp "$repos_dir/glad-cmake.txt" "$vendor/CMakeLists.txt"
# echo "Copying CMakeLists.txt to third-party complete"
#
#
# echo "Creating build directories..."
# build_dir="$curr_dir/build"
# win64_build_dir="$build_dir/win64"
# linux_build_dir="$build_dir/linux"
#
# check_create_folder "$build_dir"
# check_create_folder "$win64_build_dir"
# check_create_folder "$linux_build_dir"
# echo "Creating build directories complete"
#
# echo "--- Environment setup for Linux based systems complete"
