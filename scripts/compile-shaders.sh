#!/bin/bash

src_dir="$PWD"

glslc "$src_dir/assets/Shaders/Vulkan/src/tri.vert" -o "$src_dir/assets/Shaders/Vulkan/bytecode/tri_vert.spv"
glslc "$src_dir/assets/Shaders/Vulkan/src/tri.frag" -o "$src_dir/assets/Shaders/Vulkan/bytecode/tri_frag.spv"
