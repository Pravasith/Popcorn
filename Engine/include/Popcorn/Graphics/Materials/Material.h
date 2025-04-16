#pragma once

#include "GlobalMacros.h"
#include "MaterialTypes.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
#include "Shader.h"
#include <unordered_map>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Mesh;

template <MaterialTypes T> class Material {
public:
  Material() { PC_PRINT("CREATED", TagType::Constr, "Material.h"); };
  virtual ~Material() { PC_PRINT("DESTROYED", TagType::Destr, "Material.h"); };

  static constexpr MaterialTypes type_value = T;

  [[nodiscard]] static const std::unordered_map<ShaderStages, Buffer &> &
  GetShaderMap() {
    return s_shaderByteCodeMap;
  };

  void SetShader(ShaderStages stage, Buffer &spirVShaderCode) {
    s_shaderByteCodeMap[stage] = spirVShaderCode;
  };

  // Shaders, uniform data (roughness, metallic etc)
  void SetData(const DeriveMaterialDataType<T>::type &materialData) {
    m_data = materialData;
  };

protected:
  // Every Material "Type" has it's own shaders (vert, frag, ...)
  static std::unordered_map<ShaderStages, Buffer &> s_shaderByteCodeMap;
  DeriveMaterialDataType<T>::type m_data;
};

//
//
// ----------------------------------------------------------------------------
// --- UTIL FUNCTIONS (GLOBAL) ------------------------------------------------
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

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
