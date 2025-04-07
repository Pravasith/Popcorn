#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
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

template <MaterialTypes T> struct MaterialData {
  int enabledShadersMask = 0;
  bool doubleSided = false;

  MaterialData();
  virtual ~MaterialData() = 0;
};

struct BasicMaterialData : public MaterialData<MaterialTypes::BasicMat> {
  glm::vec4 baseColorFactor = glm::vec4(1.0f);
};

struct PbrMaterialData : public MaterialData<MaterialTypes::PbrMat> {
  glm::vec4 baseColorFactor = glm::vec4(1.0f);
  float metallicFactor = 1.0f;
  float roughnessFactor = 1.0f;
  float alphaCutoff = 0.5f;
  bool hasBaseColorTexture = false;
  bool hasNormalTexture = false;
  bool hasMetallicRoughnessTexture = false;
};

template <MaterialTypes T> struct DeriveMaterialDataType;
template <> struct DeriveMaterialDataType<MaterialTypes::BasicMat> {
  using type = BasicMaterialData;
};
template <> struct DeriveMaterialDataType<MaterialTypes::PbrMat> {
  using type = PbrMaterialData;
};

template <MaterialTypes T> class Material {
public:
  Material() { PC_PRINT("CREATED", TagType::Constr, "Material.h"); };
  virtual ~Material() { PC_PRINT("DESTROYED", TagType::Destr, "Material.h"); };

  static constexpr MaterialTypes type_value = T;

  [[nodiscard]] inline const Buffer &GetShader() const { return m_shader; };
  void SetShader(Buffer &&spirVShaderCode) {
    m_shader = std::move(spirVShaderCode);
  };

  // Shaders, uniform data (roughness, metallic etc)
  void SetData(const DeriveMaterialDataType<T>::type &materialData) {
    m_data = materialData;
  };

protected:
  Buffer m_shader;
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
