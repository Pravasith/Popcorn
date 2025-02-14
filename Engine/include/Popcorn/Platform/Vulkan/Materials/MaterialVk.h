#pragma once

#include "GfxPipelineVk.h"
#include "Material.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// TODO: Make a material map
class MaterialVk : public Material {
public:
  MaterialVk(std::vector<ShaderFiles> shaderFiles) : Material(shaderFiles) {
    PC_PRINT("CREATED", TagType::Constr, "MaterialVk.h");
  };
  virtual ~MaterialVk() override {
    PC_PRINT("DESTROYED", TagType::Destr, "MaterialVk.h");
  };

  virtual void CreatePipeline() {};
  virtual void Bind() override {};

private:
protected:
  GfxPipelineVk m_pipelineVk;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
