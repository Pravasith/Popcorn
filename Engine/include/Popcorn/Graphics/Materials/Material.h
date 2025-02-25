#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
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
  // std::vector<PCImage> textures; // albedo, normals, roughness ..etc
  // UniformData uniformData;   // color, metalness, roughness ..etc
  // RenderStates renderStates; // blending, depth testing  ..etc
};

class Material {
public:
  Material(MaterialData &matData) : m_materialData(matData) {
    PC_PRINT("CREATED", TagType::Constr, "Material.h");
    LoadShaders();
  };
  virtual ~Material() { PC_PRINT("DESTROYED", TagType::Destr, "Material.h"); };

  [[nodiscard]] inline std::vector<Buffer> &GetShaders() { return m_shaders; };
  [[nodiscard]] inline std::vector<const Mesh *> &GetLinkedMeshes() {
    return m_linkedMeshes;
  };

  void LinkMesh(const Mesh *meshPtr);
  void UnlinkMesh(const Mesh *meshPtr);

  virtual const MaterialTypes GetMaterialType() const = 0;
  virtual void Bind() = 0;

private:
  void LoadShaders();

protected:
  MaterialData &m_materialData;
  std::vector<Buffer> m_shaders;
  std::vector<const Mesh *> m_linkedMeshes;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
