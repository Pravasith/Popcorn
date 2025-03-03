#include "Material.h"
#include "GlobalMacros.h"
#include "Shader.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void Material::LoadShaders() {
  PC_WARN("BUFFER: Shader files are loaded here -- expensive if not handled "
          "properly")
  m_shaders.reserve(m_materialData.shaderFiles.size());

  for (auto &filename : m_materialData.shaderFiles) {
    m_shaders.emplace_back(Shader::ReadSpvFile(filename));
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
