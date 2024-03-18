#!/bin/bash

echo "Starting installation for Linux based systems..."

check_create_folder() {
    local folder="$1"

    # Check if the folder exists
    if [ ! -d "$folder" ]; then
        # If the folder doesn't exist, create it
        mkdir "$folder"
        echo "Folder '$folder' created."
    else
        echo "Folder '$folder' already exists. Skipped creating."
    fi
}

# INSTALL THIRD PARTY LIBS
tp_folder=third-party-libs
# rm -rf "$tp_folder"
check_create_folder "$tp_folder"

# CLONE GLFW
echo "Cloning GLFW..."

glfw_dir="$tp_folder/glfw"
check_create_folder "$glfw_dir"

git clone "git@github.com:glfw/glfw.git" "$glfw_dir"
echo "Cloning GLFW complete"

# INSTALL GLFW
echo "Installing GLFW..."
glfw_build_dir="$glfw_dir/build"

sudo apt install libwayland-dev libxkbcommon-dev xorg-dev

check_create_folder "$glfw_build_dir"
cd "$glfw_build_dir"
cmake ..
make install
echo "Installing GLFW complete"



echo "Installation successful."
