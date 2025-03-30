#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Mesh;

enum class MaterialTypes {
  BasicMat = 1,
  PbrMat
  // Add more
};

enum ShaderStages {
  None = 0,
  //
  // Graphics types
  VertexBit = 1,
  TesselationBit = shift_l(1),
  GeometryBit = shift_l(2),
  FragmentBit = shift_l(3),
  //
  // Compute types
  ComputeBit = shift_l(4)
  //
  // Ray tracing types
  // TODO: Fill it out when time comes
};

struct MaterialData {
  //
  int enabledShadersMask = 0;
  std::vector<const char *> shaderFiles{};

  //
  bool doubleSided = false;
  // bool hasBaseColorTexture = false;
  // bool hasNormalTexture = false;
  // bool hasMetallicRoughnessTexture = false;
  float metallicFactor = 1.0f;
  float roughnessFactor = 1.0f;
  float alphaCutoff = 0.5f;
  glm::vec4 baseColorFactor = glm::vec4(1.0f);
};

template <MaterialTypes T> class Material {
public:
  static constexpr MaterialTypes materialType_value = T;

  Material(MaterialData &matData) : m_materialData(matData) {
    PC_PRINT("CREATED", TagType::Constr, "Material.h");
    LoadShaders();
  };

  virtual ~Material() { PC_PRINT("DESTROYED", TagType::Destr, "Material.h"); };

  [[nodiscard]] inline std::vector<Buffer> &GetShaders() { return m_shaders; };
  virtual void Bind() = 0;

private:
  void LoadShaders();

protected:
  MaterialData &m_materialData;
  std::vector<Buffer> m_shaders;
};

//
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// --- UTIL FUNCTIONS ---------------------------------------------------------

template <MaterialTypes T>
void PC_ValidateAndAddMaterial(Material<T> *materialPtr,
                               std::vector<Material<T> *> &materials) {
  auto ptr = std::find(materials.begin(), materials.end(), materialPtr);

  if (ptr != materials.end()) {
    PC_WARN("Material already exists in the material library!")
    return;
  };

  materials.emplace_back(materialPtr);
};

template <MaterialTypes T>
void PC_ValidateAndRemoveMaterial(Material<T> *materialPtr,
                                  std::vector<Material<T> *> &materials) {
  auto ptr = std::find(materials.begin(), materials.end(), materialPtr);

  if (ptr == materials.end()) {
    PC_WARN("Material not found!")
    return;
  };

  materials.erase(ptr);
};

template <MaterialTypes T>
void PC_AddMaterialByType(Material<T> *materialPtr,
                          std::vector<Material<T> *> &materials) {
  switch (materialPtr->GetMaterialType()) {
  case Popcorn::Gfx::MaterialTypes::BasicMat: {
    PC_ValidateAndAddMaterial(materialPtr, materials);
    break;
  }
  case Popcorn::Gfx::MaterialTypes::PbrMat: {
    break;
  }
  default:
    PC_WARN("material type not found") { break; }
  }
}

template <MaterialTypes T>
void PC_RemoveMaterialByType(Material<T> *materialPtr,
                             std::vector<Material<T> *> &materials) {
  switch (materialPtr->GetMaterialType()) {
  case Popcorn::Gfx::MaterialTypes::BasicMat: {
    PC_ValidateAndRemoveMaterial(materialPtr, materials);
    break;
  }
  case Popcorn::Gfx::MaterialTypes::PbrMat: {
    break;
  }
  default:
    PC_WARN("material type not found") { break; }
  }
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
