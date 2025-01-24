#!/bin/bash

src_dir="$PWD"
sh_src_dir="$src_dir/assets/shaders"

sh_dest_dir_lnx="$src_dir/dist/linux/bin/shaders"
sh_dest_dir_win="$src_dir/dist/windows/bin/shaders"

sh_list=$(ls "$sh_src_dir"/*.vert "$sh_src_dir"/*.frag)

for file in $sh_list; do
    # The $ sign inside sed only matches the end of the string
    new_fpath=$(ls "$file" | sed -e 's/.vert$/_vert.spv/' -e 's/.frag$/_frag.spv/')
    glslc "$file" -o "$new_fpath"

if [[ ! -r "$new_fpath" ]]; then
    echo "SHADER COMPILATION ERROR: Shader files don't exist";
fi
    mkdir -p "$sh_dest_dir_lnx"
    mkdir -p "$sh_dest_dir_win"

    cp "$new_fpath" "$sh_dest_dir_lnx"
    cp "$new_fpath" "$sh_dest_dir_win"
done
