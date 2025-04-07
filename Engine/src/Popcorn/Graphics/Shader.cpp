#include "Shader.h"
#include "GlobalMacros.h"
#include "Sources.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

ShaderLibrary *ShaderLibrary::s_instance = nullptr;

void ShaderLibrary::LoadShaders() {
  for (const auto &[shader, src] : PC_SHADER_SOURCE_MAP) {
    m_shaders[shader] = std::move(Shader::ReadSpvFile(src));
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
