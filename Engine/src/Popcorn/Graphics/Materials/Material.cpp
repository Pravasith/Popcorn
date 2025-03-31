#include "Material.h"
#include "GlobalMacros.h"
#include "Shader.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void Material::LoadShaders() {
  PC_WARN("BUFFER: Shader files are loaded here -- expensive if not handled "
          "properly")
  m_shaders.reserve(m_materialData->shaderFiles.size());

  for (auto &filename : m_materialData->shaderFiles) {
    m_shaders.emplace_back(Shader::ReadSpvFile(filename));
  };
};

void PC_ValidateAndAddMaterial(Material *materialPtr,
                               std::vector<Material *> &materials) {
  auto ptr = std::find(materials.begin(), materials.end(), materialPtr);

  if (ptr != materials.end()) {
    PC_WARN("Material already exists in the material library!")
    return;
  };

  materials.emplace_back(materialPtr);
};

void PC_ValidateAndRemoveMaterial(Material *materialPtr,
                                  std::vector<Material *> &materials) {
  auto ptr = std::find(materials.begin(), materials.end(), materialPtr);

  if (ptr == materials.end()) {
    PC_WARN("Material not found!")
    return;
  };

  materials.erase(ptr);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
