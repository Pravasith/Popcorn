#pragma once

#include "GlobalMacros.h"
#include <unordered_map>

ENGINE_NAMESPACE_BEGIN
//
// --------------------------------------------------------------------------
// ALL SOURCES ARE RELATIVE TO THE FINAL DIST BUILD BINARIES ----------------
//
GFX_NAMESPACE_BEGIN
//
// --------------------------------------------------------------------------
// ----- GENERAL RESOURCES --------------------------------------------------
enum class ShaderFiles { VertShaderTriangle = 1, FragShaderTriangle };

static std::unordered_map<ShaderFiles, const char *> PC_SHADER_SOURCE_MAP = {
    {ShaderFiles::VertShaderTriangle, "shaders/tri_vert.spv"},
    {ShaderFiles::FragShaderTriangle, "shaders/tri_frag.spv"},
};
// ----- GENERAL RESOURCES --------------------------------------------------
// --------------------------------------------------------------------------
//

//
// --------------------------------------------------------------------------
// ----- VULKAN RESOURCES ---------------------------------------------------
enum class PipelineIds {

};
// ----- VULKAN RESOURCES ---------------------------------------------------
// --------------------------------------------------------------------------
//
GFX_NAMESPACE_END

ENGINE_NAMESPACE_END
