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
thirdparty_dir="$curr_dir/third-party" && mkdir -p "$thirdparty_dir"

#
# --- Submodules --------------------------------------------------------
glfw_dir="$thirdparty_dir/glfw"
vma_dir="$thirdparty_dir/vma"
imgui_dir="$thirdparty_dir/imgui"
imgui_backends_dir="$imgui_dir/backends"
glm_dir="$thirdparty_dir/glm"

# -----------------------------------------------------------------------
# INSTALL SUBMODULES
# -----------------------------------------------------------------------

echo "Initing submodules..."
git submodule init && git submodule update
echo "Initing submodules complete"

#
# --- VMA ---------------------------------------------------------------
#
echo "Installing VMA..."
# cp -r "$vma_sm_dir" "$vendor_platform_agnostic_dir"
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
