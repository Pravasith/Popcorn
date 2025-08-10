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

  template <ShaderStages S> static void SetShader(Buffer *spirVShaderCode) {
    s_shaderByteCodeMap[S] = spirVShaderCode;
  };

  [[nodiscard]] static const Buffer &GetShader(ShaderStages stage) {
    return *s_shaderByteCodeMap[stage];
  };

  [[nodiscard]] const DeriveMaterialDataType<T>::type &GetMaterialData() const {
    return m_data;
  };

  // Shaders, uniform data (roughness, metallic etc)
  void SetData(const DeriveMaterialDataType<T>::type &materialData) {
    m_data = materialData;
  };

protected:
  // Every Material "Type" has it's own shaders (vert, frag, ...)
  static std::unordered_map<ShaderStages, Buffer *> s_shaderByteCodeMap;
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

template <MaterialTypes T>
static MaterialHashType PC_HashMaterialGroups(Material<T> *material) {
  MaterialHashType uid = 0;

  const auto &matData = material->GetMaterialData();

  if constexpr (T == MaterialTypes::BasicMat) {
    uid ^= PC_FloatToUint32(matData.baseColorFactor.x);
    uid ^= PC_FloatToUint32(matData.baseColorFactor.y) >> 3;
    uid ^= PC_FloatToUint32(matData.baseColorFactor.z) >> 6;
    uid *= 2654435761u; // Knuth’s multiplicative hash
  } else if constexpr (T == MaterialTypes::PbrMat) {
    uid ^= PC_FloatToUint32(matData.baseColorFactor.x);
    uid ^= PC_FloatToUint32(matData.baseColorFactor.y) >> 3;
    uid ^= PC_FloatToUint32(matData.baseColorFactor.z) >> 6;
    uid ^= PC_FloatToUint32(matData.metallicFactor) << 2;
    uid ^= PC_FloatToUint32(matData.roughnessFactor) << 3;
    uid ^= matData.hasBaseColorTexture ? 0x1 : 0;
    uid ^= matData.hasNormalTexture ? 0x2 : 0;
    uid ^= matData.hasMetallicRoughnessTexture ? 0x4 : 0;
    uid *= 2654435761u;
  }

  return uid;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
