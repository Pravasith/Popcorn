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
  BasicMat_Vert,
  BasicMat_Frag,

  PbrMat_Vert,
  PbrMat_Frag,

  Lighting_Vert,
  Lighting_Frag,

  Composite_Vert,
  Composite_Frag
};

using PipelineShaders = std::unordered_map<ShaderFiles, const char *>;

static PipelineShaders PC_SHADER_SOURCE_MAP{
    //
    // Material shaders
    {ShaderFiles::BasicMat_Vert, "assets/shaders/basicMat_vert.spv"},
    {ShaderFiles::BasicMat_Frag, "assets/shaders/basicMat_frag.spv"},

    {ShaderFiles::PbrMat_Vert, "assets/shaders/pbrMat_vert.spv"},
    {ShaderFiles::PbrMat_Frag, "assets/shaders/pbrMat_frag.spv"},

    {ShaderFiles::Lighting_Vert, "assets/shaders/lighting_vert.spv"},
    {ShaderFiles::Lighting_Frag, "assets/shaders/lighting_frag.spv"},

    {ShaderFiles::Composite_Vert, "assets/shaders/composite_vert.spv"},
    {ShaderFiles::Composite_Frag, "assets/shaders/composite_frag.spv"},
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
