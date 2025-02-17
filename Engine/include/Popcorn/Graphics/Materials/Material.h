#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
#include "Shader.h"
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
  std::vector<const char *> shaderFiles{};
  // std::vector<PCImage> textures; // albedo, normals, roughness ..etc
  // UniformData uniformData;   // color, metalness, roughness ..etc
  // RenderStates renderStates; // blending, depth testing  ..etc
};

// TODO: Make a material map
class Material {
public:
  Material(MaterialData &matData) : m_materialData(matData) {
    PC_PRINT("CREATED", TagType::Constr, "Material.h");
    LoadShaders();
  };

  virtual ~Material() { PC_PRINT("DESTROYED", TagType::Destr, "Material.h"); };

  virtual void Bind() = 0;

private:
  void LoadShaders() {
    PC_WARN("BUFFER: Shader files are loaded here -- expensive if not handled "
            "properly")
    m_shaders.reserve(m_materialData.shaderFiles.size());

    for (auto &filename : m_materialData.shaderFiles) {
      m_shaders.emplace_back(Shader::ReadSpvFile(filename));
    };

    PC_WARN("BUFFER: shaders are loaded!")
  };

protected:
  MaterialData &m_materialData;
  std::vector<Buffer> m_shaders;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
