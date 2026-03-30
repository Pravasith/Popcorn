#include "Material.h"
#include "GlobalMacros.h"
#include "MaterialTypes.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <>
std::unordered_map<ShaderStages, Buffer *>
    Material<MaterialTypes::BasicMat>::s_shaderByteCodeMap{};

template <>
std::unordered_map<ShaderStages, Buffer *>
    Material<MaterialTypes::PbrMat>::s_shaderByteCodeMap{};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
