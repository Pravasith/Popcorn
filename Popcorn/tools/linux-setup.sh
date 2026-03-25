#!/bin/bash

# -----------------------------------------------------------------------
# IMPORTANT NOTE BEFORE YOU RUN THIS SCRIPT
# -----------------------------------------------------------------------
# -----------------------------------------------------------------------
# INSTALL THESE EXTERNAL DEPENDENCIES FOR LINUX ONLY
#
# Compilers and cross-compilers
# 1. MingW compilers
#       sudo apt install mingw-w64
#
# 2. Display server stuff - used by GLFW for Linux
#       sudo apt install libwayland-dev libxkbcommon-dev xorg-dev
#
# RUN CMD TO INSTALL THESE DEPS FOR GLFW
# --------------------------------------
#
# RUN CMD TO INSTALL THESE DEPS FOR VULKAN
# -----------------------------------------
#       sudo apt-get install git build-essential libx11-xcb-dev \
#           libxkbcommon-dev libwayland-dev libxrandr-dev
#
# FIND & INSTALL THESE YOURSELF
# ------------------------------
# 1. Vulkan SDK
#    - Easily found with a web search
# 2. glslc
#    - This comes with Vulkan SDK, so no need to install
# 3. RenderDoc
#    - Only if you want to use the profiling system. It is
#      available on be installed from https://renderdoc.org/
#      To use the profiling system turn on the flag
#      in ../CMakeLists.txt
#
# -----------------------------------------------------------------------

echo "--- Starting environment setup for Linux based systems..."

# -----------------------------------------------------------------------
# DIRS
# -----------------------------------------------------------------------
#
curr_dir="${PWD}"
engine_dir="$curr_dir/Engine"
editor_dir="$curr_dir/Editor"
#
# raw github cloned submodules
submodules_dir="$curr_dir/submodules" && mkdir -p "$submodules_dir"
#
# third party root dirs
vendor_dir="$curr_dir/third-party" && mkdir -p "$vendor_dir"
vendor_linux_dir="$vendor_dir/linux" && mkdir -p "$vendor_linux_dir"
vendor_windows_dir="$vendor_dir/windows" && mkdir -p "$vendor_windows_dir"
vendor_mac_os_dir="$vendor_dir/mac-os" && mkdir -p "$vendor_mac_os_dir"
vendor_platform_agnostic_dir="$vendor_dir/platform-agnostic" &&
    mkdir -p "$vendor_platform_agnostic_dir"
#
# glfw specific dirs
glfw_sm_dir="$submodules_dir/glfw" && mkdir -p "$glfw_sm_dir"
glfw_sm_build_dir="$glfw_sm_dir/build" && mkdir -p "$glfw_sm_build_dir"
glfw_sm_build_linux_dir="$glfw_sm_build_dir/linux" &&
    mkdir -p "$glfw_sm_build_linux_dir"
glfw_sm_install_linux_dir="$vendor_linux_dir/glfw" &&
    mkdir -p "$glfw_sm_install_linux_dir"
glfw_sm_build_windows_dir="$glfw_sm_build_dir/windows" &&
    mkdir -p "$glfw_sm_build_windows_dir"
glfw_sm_install_windows_dir="$vendor_windows_dir/glfw" &&
    mkdir -p "$glfw_sm_install_windows_dir"

# -----------------------------------------------------------------------
# INSTALL SUBMODULES
# -----------------------------------------------------------------------

echo "Initing submodules..."
git submodule init && git submodule update
echo "Initing submodules complete"

echo "Installing GLFW for linux and windows..."

# building glfw for linux
cmake                                                           \
    -D CMAKE_INSTALL_PREFIX="$glfw_sm_install_linux_dir"        \
    -D CMAKE_BUILD_TYPE=Release                                 \
    -S "$glfw_sm_dir"                                           \
    -B "$glfw_sm_build_linux_dir"

# building glfw for windows
cmake                                                           \
    -D CMAKE_INSTALL_PREFIX="$glfw_sm_install_windows_dir"      \
    -D CMAKE_TOOLCHAIN_FILE=CMake/x86_64-w64-mingw32.cmake      \
    -D CMAKE_BUILD_TYPE=Release                                 \
    -S "$glfw_sm_dir"                                           \
    -B "$glfw_sm_build_windows_dir"

# installing glfw for linux
cmake                                                           \
    --build "$glfw_sm_build_linux_dir"                          \
    --target install

echo "Installing GLFW for linux and windows complete"
