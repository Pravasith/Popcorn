#pragma once

#include "Global_Macros.h"
#include <unordered_map>

ENGINE_NAMESPACE_BEGIN

enum ShaderFiles { VkTriVert = 0, VkTriFrag };

static std::unordered_map<ShaderFiles, const char *> PC_src_map = {
    // Vulkan
    {VkTriVert, "shaders/tri_vert.spv"},
    // OpenGL
    {VkTriFrag, "shaders/tri_frag.spv"},
    // ADD MORE
};

ENGINE_NAMESPACE_END
