#pragma once

#include "GlobalMacros.h"
#include <unordered_map>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
//
// --- All sources are relative to the final dist build binaries ------------

//
// --- SHADERS --------------------------------------------------------------
enum class ShaderFiles {
  BasicMat_Vert = 1,
  BasicMat_Frag,
  PbrMat_Vert,
  PbrMat_Frag,
};

static std::unordered_map<ShaderFiles, const char *> PC_SHADER_SOURCE_MAP = {
    //
    // Material shaders
    {ShaderFiles::BasicMat_Vert, "assets/shaders/basicMat_vert.spv"},
    {ShaderFiles::BasicMat_Frag, "assets/shaders/basicMat_frag.spv"},
    {ShaderFiles::PbrMat_Vert, "assets/shaders/pbrMat_vert.spv"},
    {ShaderFiles::PbrMat_Frag, "assets/shaders/pbrMat_frag.spv"},
};
GFX_NAMESPACE_END

ENGINE_NAMESPACE_END
