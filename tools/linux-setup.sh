#!/bin/bash

# -----------------------------------------------------------------------
# IMPORTANT NOTE BEFORE YOU RUN THIS SCRIPT
# -----------------------------------------------------------------------
# 1. REAAAAD THE README!!!!!!!! IT TAKES 2 MINS
#
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



echo ""
echo "------------------------------------------------------------------"
echo "--- STARTING ENVIRONMENT SETUP FOR LINUX BASED SYSTEMS..."
echo "------------------------------------------------------------------"
echo ""

# -----------------------------------------------------------------------
# MAKE DIRS
# -----------------------------------------------------------------------
#
curr_dir="${PWD}"
engine_dir="$curr_dir/Engine"
editor_dir="$curr_dir/Editor"
submodules_dir="$curr_dir/submodules" && mkdir -p "$submodules_dir"
#
# --- third party root dirs ---------------------------------------------
vendor_dir="$curr_dir/third-party" && mkdir -p "$vendor_dir"
vendor_linux_dir="$vendor_dir/linux" && mkdir -p "$vendor_linux_dir"
vendor_windows_dir="$vendor_dir/windows" && mkdir -p "$vendor_windows_dir"
vendor_mac_os_dir="$vendor_dir/mac-os" && mkdir -p "$vendor_mac_os_dir"
vendor_platform_agnostic_dir="$vendor_dir/platform-agnostic" &&         \
    mkdir -p "$vendor_platform_agnostic_dir"
#
# --- GLFW specific dirs ------------------------------------------------
glfw_sm_dir="$submodules_dir/glfw" && mkdir -p "$glfw_sm_dir"
glfw_sm_build_dir="$glfw_sm_dir/build" && mkdir -p "$glfw_sm_build_dir"
glfw_sm_build_linux_dir="$glfw_sm_build_dir/linux" &&                   \
    mkdir -p "$glfw_sm_build_linux_dir"
glfw_sm_install_linux_dir="$vendor_linux_dir/glfw" &&                   \
    mkdir -p "$glfw_sm_install_linux_dir"
glfw_sm_build_windows_dir="$glfw_sm_build_dir/windows" &&               \
    mkdir -p "$glfw_sm_build_windows_dir"
glfw_sm_install_windows_dir="$vendor_windows_dir/glfw" &&               \
    mkdir -p "$glfw_sm_install_windows_dir"
imgui_sm_install_pfmagn_dir="$vendor_platform_agnostic_dir/imgui" &&  \
    mkdir -p "$imgui_sm_install_pfmagn_dir"
#
# --- Submodules --------------------------------------------------------
vma_sm_dir="$submodules_dir/vma"
imgui_sm_dir="$submodules_dir/imgui"
imgui_sm_backends_dir="$imgui_sm_dir/backends"

# -----------------------------------------------------------------------
# INSTALL SUBMODULES
# -----------------------------------------------------------------------

echo "Initing submodules..."
git submodule init && git submodule update
echo "Initing submodules complete"

echo "Installing GLFW for linux and windows..."


#
# --- GLFW -------------------------------------------------------------
#
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

# installing glfw for windows
cmake                                                           \
    --build "$glfw_sm_build_windows_dir"                        \
    --target install

echo "Installing GLFW for linux and windows complete"

#
# --- VMA ---------------------------------------------------------------
#
echo "Installing VMA..."
cp -r "$vma_sm_dir" "$vendor_platform_agnostic_dir"
# the cmake config is in third-party CML
echo "Installing VMA complete"


#
# --- ImGui -------------------------------------------------------------
#
echo "Installing ImGui..."
# the cmake config is in third-party CML
echo "Installing ImGui complete"

echo ""
echo "------------------------------------------------------------------"
echo "--- ENVIRONMENT SETUP FOR LINUX BASED SYSTEMS COMPLETE"
echo "------------------------------------------------------------------"
echo ""
