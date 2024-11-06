#!/bin/bash
#

# -----------------------------------------------------------------------
# UTIL FUNCS --- START
# -----------------------------------------------------------------------
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
# -----------------------------------------------------------------------
# UTIL FUNCS --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# INSTALL THESE EXTERNAL DEPENDENCIES
#
# FOR GLFW:
# sudo apt install libwayland-dev libxkbcommon-dev xorg-dev
#
# FOR VULKAN:
# sudo apt-get install git build-essential libx11-xcb-dev \
#     libxkbcommon-dev libwayland-dev libxrandr-dev
#
# SHADER COMPILERS:
# -----------------------------------------------------------------------

#

echo "--- Starting environment setup for Linux based systems..."
# DIR -- PROJECT PARENT SRC DIR
curr_dir=${PWD}

# DIR -- RAW GITHUB CLONED SUBMODULES
submodules_dir="$curr_dir/submodules"
# NOTE: THIS DIR ALREADY EXISTS
# check_create_folder "$submodules_dir"

# DIR -- BASE -- 3RD PARTY INCL. & IMPL. FILES
vendor_dir="$curr_dir/third-party"
check_create_folder "$vendor_dir"

# DIR -- PLATFORM SPECIFIC -- INCL. FILES
include_platform_dir="$curr_dir/include/Popcorn/Platform"
check_create_folder "$include_platform_dir"

# DIR -- PLATFORM SPECIFIC -- IMPL. FILES
src_platform_dir="$curr_dir/src/Popcorn/Platform"
check_create_folder "$src_platform_dir"

# DIR -- PLATFORM SPECIFIC 3RD PARTY INCL & SRC FILES
vendor_windows_dir="$vendor_dir/windows"
check_create_folder "$vendor_windows_dir"

vendor_linux_dir="$vendor_dir/linux"
check_create_folder "$vendor_linux_dir"

vendor_mac_os_dir="$vendor_dir/mac-os"
check_create_folder "$vendor_mac_os_dir"

# DIR -- PLAFORM AGNOSTIC 3RD PARTY INCL & SRC FILES
vendor_platform_agnostic_dir="$vendor_dir/platform-agnostic"
check_create_folder "$vendor_platform_agnostic_dir"

#

# -----------------------------------------------------------------------
# CLONE SUBMODULE REPOS --- START
# -----------------------------------------------------------------------
echo "Cloning vendor/third-party submodules..."
git submodule init
git submodule update
echo "Cloning vendor/third-party submodules complete"
# -----------------------------------------------------------------------
# CLONE SUBMODULE REPOS --- END
# -----------------------------------------------------------------------


#

# -----------------------------------------------------------------------
# INSTALL SUBMODULES --- START
# -----------------------------------------------------------------------
echo "Installing vendor/third-party submodules..."

#

# -----------------------------------------------------------------------
# SUBMODULE INSTALL - GLFW --- START
# -----------------------------------------------------------------------
echo "Building GLFW..."
glfw_sm_dir="$submodules_dir/glfw"

glfw_sm_build_dir="$glfw_sm_dir/build"
check_create_folder "$glfw_sm_build_dir"

# LINUX
# -----------------------------------------------------------------------
echo "Building GLFW for Linux..."
# CREATE GLFW SUBMODULE BUILD DIR FOR LINUX
glfw_sm_build_linux_dir="$glfw_sm_build_dir/linux"
check_create_folder "$glfw_sm_build_linux_dir"

# CREATE GLFW VENDOR DIR FOR LINUX
glfw_vendor_linux_dir="$vendor_linux_dir/glfw"
check_create_folder "$glfw_vendor_linux_dir"

cmake -DCMAKE_INSTALL_PREFIX="$glfw_vendor_linux_dir" \
    -S "$glfw_sm_dir" -B "$glfw_sm_build_linux_dir"

cd "$glfw_sm_build_linux_dir"
make install
cd "$curr_dir"

echo "Building GLFW for Linux complete"

# WINDOWS
# -----------------------------------------------------------------------
echo "Building GLFW for Windows..."
# CREATE GLFW SUBMODULE BUILD DIR FOR WINDOWS
glfw_sm_build_windows_dir="$glfw_sm_build_dir/windows"
check_create_folder "$glfw_sm_build_windows_dir"

# CREATE GLFW VENDOR DIR FOR WINDOWS
glfw_vendor_windows_dir="$vendor_windows_dir/glfw"
check_create_folder "$glfw_vendor_windows_dir"

cmake -DCMAKE_INSTALL_PREFIX="$glfw_vendor_windows_dir" \
    -S "$glfw_sm_dir" -B "$glfw_sm_build_windows_dir" \
    -D CMAKE_TOOLCHAIN_FILE=CMake/x86_64-w64-mingw32.cmake

cd "$glfw_sm_build_windows_dir"
make install
cd "$curr_dir"

echo "Building GLFW for Windows complete"
echo "Building GLFW complete"
# -----------------------------------------------------------------------
# SUBMODULE INSTALL - GLFW --- END
# -----------------------------------------------------------------------

#


# -----------------------------------------------------------------------
# SUBMODULE INSTALL - GLAD --- START
# -----------------------------------------------------------------------
echo "Installing GLAD..."
glad_sm_dir="$submodules_dir/glad"

# COPY GLAD FILES FROM SUBMODULES TO VENDOR
#
# -----------------------------------------------------------------------
cp -r "$glad_sm_dir" "$vendor_platform_agnostic_dir"

echo "Installing GLAD complete"
# -----------------------------------------------------------------------
# SUBMODULE INSTALL - GLAD --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# SUBMODULE INSTALL - IMGUI --- START
# -----------------------------------------------------------------------

echo "Installing ImGui..."
imgui_sm_dir="$submodules_dir/imgui"
imgui_sm_backends_dir="$imgui_sm_dir/backends"

imgui_vendor_platform_agnostic_dir="$vendor_platform_agnostic_dir/imgui"
check_create_folder "$imgui_vendor_platform_agnostic_dir"

# COPY IMGUI FILES FROM SUBMODULES TO VENDOR
#
# LINUX & WINDOWS
# -----------------------------------------------------------------------
cp -a "$imgui_sm_dir/."  \
      "$imgui_vendor_platform_agnostic_dir"


echo "Installing ImGui complete"

# -----------------------------------------------------------------------
# SUBMODULE INSTALL - IMGUI --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# SUBMODULE INSTALL - VULKAN-HEADERS --- BEGIN
# -----------------------------------------------------------------------

echo "Installing Vulkan-Headers ..."
vulkan_headers_sm_dir="$submodules_dir/vulkan-headers"

# CREATE VULKAN-HEADERS SUBMODULES BUILD DIR

vulkan_headers_sm_build_dir="$vulkan_headers_sm_dir/build"
check_create_folder "$vulkan_headers_sm_build_dir"

# CREATE VULKAN-HEADERS VENDOR DIR
vulkan_headers_vendor_dir="$vendor_platform_agnostic_dir/vulkan-headers"
check_create_folder "$vulkan_headers_vendor_dir"

cmake -S "$vulkan_headers_sm_dir" -B "$vulkan_headers_sm_build_dir"
cmake --install "$vulkan_headers_sm_build_dir" --prefix "$vulkan_headers_vendor_dir"

cd "$curr_dir"

echo "Installing Vulkan-Headers complete"

# -----------------------------------------------------------------------
# SUBMODULE INSTALL - VULKAN-HEADERS --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# SUBMODULE INSTALL - VULKAN-LOADER  --- BEGIN
# -----------------------------------------------------------------------

echo "Installing Vulkan-Loader..."

vulkan_loader_sm_dir="$submodules_dir/vulkan-loader"

# LINUX
# -----------------------------------------------------------------------
echo "Building VULKAN-LOADER for Linux..."
vulkan_loader_sm_lnx_build_dir="$vulkan_loader_sm_dir/build-linux"
check_create_folder "$vulkan_loader_sm_lnx_build_dir"

# CREATE VULKAN-LOADER VENDOR DIR
vulkan_loader_vendor_lnx_dir="$vendor_linux_dir/vulkan-loader"
check_create_folder "$vulkan_loader_vendor_lnx_dir"

cmake \
    -D CMAKE_INSTALL_PREFIX="$vulkan_loader_vendor_lnx_dir" \
    -D UPDATE_DEPS=ON \
    -D CMAKE_BUILD_TYPE=Release \
    -S "$vulkan_loader_sm_dir" \
    -B "$vulkan_loader_sm_lnx_build_dir"
    # -D VULKAN_HEADERS_INSTALL_DIR="$vulkan_headers_vendor_dir" \

cd "$vulkan_loader_sm_lnx_build_dir"
make install
cd "$curr_dir"

echo "Building VULKAN-LOADER for Linux complete"

# WINDOWS
# -----------------------------------------------------------------------
echo "Building VULKAN-LOADER for Windows..."
vulkan_loader_sm_win_build_dir="$vulkan_loader_sm_dir/build-win"
check_create_folder "$vulkan_loader_sm_win_build_dir"

# CREATE VULKAN-LOADER VENDOR DIR
vulkan_loader_vendor_win_dir="$vendor_windows_dir/vulkan-loader"
check_create_folder "$vulkan_loader_vendor_win_dir"

win_toolchain_file="$submodules_dir/tc-vendor-win.cmake"

cmake \
    -D CMAKE_TOOLCHAIN_FILE="$win_toolchain_file" \
    -D UPDATE_DEPS=ON \
    -D CMAKE_INSTALL_PREFIX="$vulkan_loader_vendor_win_dir" \
    -D CMAKE_BUILD_TYPE=Release \
    -S "$vulkan_loader_sm_dir" \
    -B "$vulkan_loader_sm_win_build_dir"
    # -D VULKAN_HEADERS_INSTALL_DIR="$vulkan_headers_vendor_dir" \

cd "$vulkan_loader_sm_win_build_dir"
make install
cd "$curr_dir"

echo "Building VULKAN-LOADER for Windows complete"

echo "Installing Vulkan-Loader complete"

# -----------------------------------------------------------------------
# SUBMODULE INSTALL - VULKAN-LOADER  --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# SUBMODULE INSTALL - GLM --- BEGIN
# -----------------------------------------------------------------------

echo "Installing GLM..."

glm_sm_dir="$submodules_dir/glm"

glm_sm_build_dir="$glm_sm_dir/build"
check_create_folder "$glm_sm_build_dir"

# CREATE GLM VENDOR DIR
glm_vendor_dir="$vendor_platform_agnostic_dir/glm"
check_create_folder "$glm_vendor_dir"

cmake \
    -D GLM_BUILD_TESTS=OFF \
    -D BUILD_SHARED_LIBS=OFF \
    -S "$glm_sm_dir" -B "$glm_sm_build_dir"

cmake --build "$glm_sm_build_dir" --target all
cmake --install "$glm_sm_build_dir" --prefix "$glm_vendor_dir"

cd "$curr_dir"

echo "Installing GLM complete"

# -----------------------------------------------------------------------
# SUBMODULE INSTALL - GLM --- END
# -----------------------------------------------------------------------

#

echo "Installing vendor/third-party submodules complete"
# -----------------------------------------------------------------------
# INSTALL SUBMODULES --- END
# -----------------------------------------------------------------------

#

# COPY CMAKELISTS FROM SUBMODULES TO VENDOR
echo "Copying CMakeLists.txt to third-party..."
cp "$submodules_dir/CMakeLists.txt" "$vendor_dir"
echo "Copying CMakeLists.txt to third-party complete"

echo "--- Environment setup for Linux based systems complete"
