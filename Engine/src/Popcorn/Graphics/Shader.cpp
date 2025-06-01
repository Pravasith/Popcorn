#include "Shader.h"
#include "GlobalMacros.h"
#include "Renderer.h"
#include "Sources.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

ShaderLibrary *ShaderLibrary::s_instance = nullptr;

std::unordered_map<ShaderFiles, Buffer> ShaderLibrary::s_shaders{};

template <> void ShaderLibrary::LoadShaders<RendererType::Vulkan>() {
  if (m_shadersLoaded) {
    PC_WARN("Attempt to load already loaded shaders")
    return;
  }

  for (const auto &[shader, src] : PC_SHADER_SOURCE_MAP) {
    s_shaders[shader] = std::move(Shader::ReadSpvFile(src));
  };
};

template <> void ShaderLibrary::UnloadShaders<RendererType::Vulkan>() {
  for (const auto &[shader, src] : PC_SHADER_SOURCE_MAP) {
    s_shaders.clear();
  };

  m_shadersLoaded = false;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
