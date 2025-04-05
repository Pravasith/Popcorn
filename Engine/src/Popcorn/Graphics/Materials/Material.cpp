#include "Material.h"
#include "GlobalMacros.h"
#include "Renderer.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <MaterialTypes T>
MaterialHandler *Material<T>::s_materialHandler = nullptr;

template <MaterialTypes T> void Material<T>::CreateRenderResources() {
  if (Renderer::GetAPI() == RendererType::Vulkan) {
    s_materialHandler->CreateVulkanResources();
  } else {
    // s_materialHandler->CreateOpenGlResources();
  };
};

// void Material::LoadShaders() {
//   PC_WARN("BUFFER: Shader files are loaded here -- expensive if not handled "
//           "properly")
//   m_shaders.reserve(m_materialData->shaderFiles.size());
//
//   for (auto &filename : m_materialData->shaderFiles) {
//     m_shaders.emplace_back(Shader::ReadSpvFile(filename));
//   };
// };

template class Material<MaterialTypes::BasicMat>;
template class Material<MaterialTypes::PbrMat>;

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
