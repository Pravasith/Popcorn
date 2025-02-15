#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Sources.h"
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

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
  std::vector<ShaderFiles> shaderFiles{};
  // std::vector<PCImage> textures; // albedo, normals, roughness ..etc
  // UniformData uniformData;   // color, metalness, roughness ..etc
  // RenderStates renderStates; // blending, depth testing  ..etc
};

// TODO: Make a material map
class Material {
public:
  Material(MaterialData &matData) : m_materialData(matData) {
    PC_PRINT("CREATED", TagType::Constr, "Material.h");
    // Init(shaderFiles);
  };
  virtual ~Material() { PC_PRINT("DESTROYED", TagType::Destr, "Material.h"); };

  virtual void Bind() = 0;

protected:
  MaterialData &m_materialData;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
