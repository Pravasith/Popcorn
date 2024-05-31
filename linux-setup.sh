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
cd "$curr_dir"

echo "Building GLFW for Linux complete"

echo "Building GLFW for Windows..."
# CREATE GLFW SUBMODULE BUILD DIR FOR WINDOWS
glfw_submodule_build_windows_dir="$glfw_submodule_build_dir/windows"
check_create_folder "$glfw_submodule_build_windows_dir"

# CREATE GLFW VENDOR DIR FOR WINDOWS
glfw_vendor_windows_dir="$vendor_windows_dir/glfw"
check_create_folder "$glfw_vendor_windows_dir"

cmake -DCMAKE_INSTALL_PREFIX="$glfw_vendor_windows_dir" -S "$glfw_submodule_dir" -B "$glfw_submodule_build_windows_dir"

cd "$glfw_submodule_build_windows_dir"
make install
cd "$curr_dir"

echo "Building GLFW for Windows complete"
echo "Building GLFW complete"

echo "Installing vendor/third-party submodules complete"

# INSTALL GLAD
echo "Installing GLAD..."
glad_submodule_dir="$submodules_dir/glad"

# COPY GLAD FILES FROM SUBMODULES TO VENDOR
cp -r "$glad_submodule_dir" "$vendor_linux_dir"
cp -r "$glad_submodule_dir" "$vendor_windows_dir"

echo "Installing GLAD complete"

# COPY CMAKELISTS FROM SUBMODULES TO VENDOR
echo "Copying CMakeLists.txt to third-party..."
cp "$submodules_dir/CMakeLists.txt" "$vendor_linux_dir"
cp "$submodules_dir/CMakeLists.txt" "$vendor_windows_dir"
echo "Copying CMakeLists.txt to third-party complete"

echo "--- Environment setup for Linux based systems complete"
