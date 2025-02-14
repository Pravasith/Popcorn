#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Sources.h"
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class MaterialTypes { BasicMat = 1, BasicMat1 };

// TODO: Make a material map
class Material {
public:
  Material(std::vector<ShaderFiles> shaderFiles)
      : m_shaderFilenames(shaderFiles) {
    PC_PRINT("CREATED", TagType::Constr, "Material.h");
    Init(shaderFiles);
  };
  virtual ~Material() { PC_PRINT("DESTROYED", TagType::Destr, "Material.h"); };

  virtual void Bind() { PC_WARN("Bind material hasn't been defined") };

private:
  void Init(std::vector<ShaderFiles> shaderFiles);

protected:
  std::vector<ShaderFiles> m_shaderFilenames;

private:
  Material *m_instance = nullptr;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
