#include "Material.h"
#include "GlobalMacros.h"
#include "Mesh.h"
#include "Shader.h"
#include <algorithm>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void Material::LinkMesh(const Mesh *meshPtr) {
  m_linkedMeshes.push_back(meshPtr);
  PC_WARN("Linking size " << m_linkedMeshes.size())
};

void Material::UnlinkMesh(const Mesh *meshPtr) {
  auto itr = std::find(m_linkedMeshes.begin(), m_linkedMeshes.end(), meshPtr);
  if (itr != m_linkedMeshes.end()) {
    m_linkedMeshes.erase(itr);
  } else {
    PC_ERROR("Mesh is not found in m_linkedMeshes", "Material.h")
  };
};

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
