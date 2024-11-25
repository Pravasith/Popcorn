#!/bin/bash

src_dir="$PWD"

sh_src_dir="$src_dir/assets/Shaders/Vulkan/src"
sh_dest_dir="$src_dir/assets/Shaders/Vulkan/bytecode"

sh_build_dir_lnx="$src_dir/build/linux/shaders"
sh_build_dir_win="$src_dir/build/windows/shaders"

glslc \
    "$sh_src_dir/tri.vert" \
    -o "$sh_dest_dir/tri_vert.spv"

glslc \
    "$sh_src_dir/tri.frag" \
    -o "$sh_dest_dir/tri_frag.spv"

if [[ ! -r "$sh_dest_dir/tri_vert.spv" || \
        ! -r "$sh_dest_dir/tri_frag.spv" ]]; then
    echo "ERROR: SHADER FILES DO NOT EXIST";
else
    if [[ ! -d "$sh_build_dir_lnx" || \
            ! -d "$sh_build_dir_win" ]]; then
        echo "CREATING SHADER BIN DIRS";
        mkdir "$sh_build_dir_lnx"
        mkdir "$sh_build_dir_win"
    else
        cp "$sh_dest_dir"/* "$sh_build_dir_lnx"
        cp "$sh_dest_dir"/* "$sh_build_dir_win"
    fi
fi
