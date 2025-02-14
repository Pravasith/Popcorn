#include "Material.h"
#include "GlobalMacros.h"
#include "Materials/MaterialVk.h"
#include "Renderer.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void Material::Init(std::vector<ShaderFiles> shaderFiles) {
  if (Renderer::GetAPI() == RendererType::Vulkan) {
    m_instance = new MaterialVk(shaderFiles);
  } else {
    // m_instance = new MaterialOpenGL(shaderFiles);
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
