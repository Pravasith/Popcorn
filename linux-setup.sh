#!/bin/bash

echo "Starting installation for Linux based systems..."

check_create_folder() {
    local folder="$1"

    # Check if the folder exists
    if [ ! -d "$folder" ]; then
        # If the folder doesn't exist, create it
        sudo mkdir "$folder"
        echo "Folder '$folder' created."
    else
        echo "Folder '$folder' already exists. Skipped creating."
    fi
}

# INSTALL THIRD PARTY LIBS
curr_dir=${PWD}
vendor="$curr_dir/third-party"
repos_dir="$curr_dir/repos"

# rm -rf "$vendor"
check_create_folder "$vendor"

# CLONE GLFW
echo "Cloning GLFW..."

glfw_src_dir="$vendor/glfw"
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
echo "Installing GLFW complete"

# INSTALL GLAD
echo "Installing GLAD..."
glad_src_dir="$vendor/glad"
check_create_folder "$glad_src_dir"

cp "$repos_dir/glad/src/glad.c" "$glad_src_dir/glad.c"

glad_install_dir="/usr/include/glad"
khr_install_dir="/usr/include/glad"

check_create_folder "$glad_install_dir"
sudo cp "$repos_dir/glad/include/glad/glad.h" "$glad_install_dir/glad.h"

echo "Installing GLAD complete"

echo "Installation successful."
