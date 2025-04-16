#pragma once

#include "GlobalMacros.h"
#include <glm/glm.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

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

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
