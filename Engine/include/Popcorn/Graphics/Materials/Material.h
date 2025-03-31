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
  int enabledShadersMask = 0;
  std::vector<const char *> shaderFiles{};
  bool doubleSided = false;

protected:
  MaterialData();
  ~MaterialData();
};

struct BasicMaterialData : public MaterialData {
  glm::vec4 baseColorFactor = glm::vec4(1.0f);
};

struct PbrMaterialData : public MaterialData {
  glm::vec4 baseColorFactor = glm::vec4(1.0f);
  float metallicFactor = 1.0f;
  float roughnessFactor = 1.0f;
  float alphaCutoff = 0.5f;
  bool hasBaseColorTexture = false;
  bool hasNormalTexture = false;
  bool hasMetallicRoughnessTexture = false;
};

class Material {
public:
  Material() {
    PC_PRINT("CREATED", TagType::Constr, "Material.h");
    LoadShaders();
  };
  virtual ~Material() { PC_PRINT("DESTROYED", TagType::Destr, "Material.h"); };

  [[nodiscard]] inline const std::vector<Buffer> &GetShaders() const {
    return m_shaders;
  };
  [[nodiscard]] const MaterialTypes &GetType() {
    if (m_type != MaterialTypes::BasicMat | m_type != MaterialTypes::PbrMat) {
      PC_ERROR("Material type not set!", "Material")
    };

    return m_type;
  };

  void LoadShaders();
  virtual void SetData(const MaterialData &materialData) = 0;

private:
  virtual void SetType(MaterialTypes matType) = 0;

protected:
  MaterialData *m_materialData;
  MaterialTypes m_type;
  std::vector<Buffer> m_shaders;
};

//
//
// ----------------------------------------------------------------------------
// --- UTIL FUNCTIONS (GLOBAL) ------------------------------------------------
void PC_ValidateAndAddMaterial(Material *materialPtr,
                               std::vector<Material *> &materials);
void PC_ValidateAndRemoveMaterial(Material *materialPtr,
                                  std::vector<Material *> &materials);

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
