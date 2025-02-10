#pragma once

#include "GlobalMacros.h"
#include <unordered_map>

ENGINE_NAMESPACE_BEGIN

enum ShaderFiles { VertShaderTriangle = 1, FragShaderTriangle };

// ALL SOURCES ARE RELATIVE TO THE FINAL DIST BUILD BINARIES
static std::unordered_map<ShaderFiles, const char *> PC_SHADER_SOURCE_MAP = {
    {VertShaderTriangle, "shaders/tri_vert.spv"},
    {FragShaderTriangle, "shaders/tri_frag.spv"},
};

ENGINE_NAMESPACE_END
