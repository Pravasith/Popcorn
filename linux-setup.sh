#!/bin/bash

echo "Starting installation for Linux based systems..."

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

# INSTALL THIRD PARTY LIBS
curr_dir=${PWD}
vendor="$curr_dir/third-party"
repos_dir="$curr_dir/ext-repos"
win64_dir="$curr_dir/win64"

check_create_folder "$vendor"
check_create_folder "$win64_dir"

# CLONE GLFW
echo "Cloning GLFW..."

glfw_src_dir="$repos_dir/glfw"
check_create_folder "$glfw_src_dir"

git clone "git@github.com:glfw/glfw.git" "$glfw_src_dir"
echo "Cloning GLFW complete"

# INSTALL GLFW
echo "Installing GLFW..."
glfw_build_dir="$glfw_src_dir/build"

sudo apt install libwayland-dev libxkbcommon-dev xorg-dev

check_create_folder "$glfw_build_dir"
cd "$glfw_build_dir"
cmake ..
sudo make install
cd "$curr_dir"

# INSTALL GLFW FOR WINDOWS
echo "Generating Win64 Binaries..."
glfw_win64_build_dir="$glfw_src_dir/win64-build"
glfw_win64_install_dir="$win64_dir/glfw"

check_create_folder "$glfw_win64_build_dir"
cmake -DCMAKE_INSTALL_PREFIX="$glfw_win64_install_dir" -S "$glfw_src_dir" -B "$glfw_win64_build_dir" -D CMAKE_TOOLCHAIN_FILE=CMake/x86_64-w64-mingw32.cmake

check_create_folder "$glfw_win64_install_dir"
cd "$glfw_win64_build_dir"
sudo make install

echo "Generating Win64 Binaries complete"
cd "$curr_dir"

echo "Installing GLFW complete"

# INSTALL GLAD
echo "Installing GLAD..."
glad_src_dir="$vendor/glad"
check_create_folder "$glad_src_dir"

cp "$repos_dir/glad/src/glad.c" "$glad_src_dir/glad.c"

glad_install_dir="/usr/include/glad"
khr_install_dir="/usr/include/glad"

# 2nd arg 1 means sudo
check_create_folder "$glad_install_dir" 1
sudo cp "$repos_dir/glad/include/glad/glad.h" "$glad_install_dir/glad.h"

echo "Installing GLAD complete"

echo "Copying CMakeLists.txt to third-party..."
cp "$repos_dir/glad-cmake.txt" "$vendor/CMakeLists.txt"
echo "Copying CMakeLists.txt to third-party complete"

echo "Installation successful."
