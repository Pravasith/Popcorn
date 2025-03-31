#pragma once

#include "GlobalMacros.h"
#include "Material.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class PbrMaterial : public Material {
public:
  PbrMaterial() { PC_PRINT("CREATED", TagType::Constr, "PbrMaterial.h"); };
  virtual ~PbrMaterial() override {
    PC_PRINT("DESTROYED", TagType::Destr, "PbrMaterial.h");
  };

  virtual void SetData(const MaterialData &materialData) override {};

private:
  virtual void SetType(MaterialTypes matType) override {
    m_type = MaterialTypes::PbrMat;
  };

  // FOR VULKAN -- Create basic material's descriptor set layout
  // Each type of material can only have one DSet layout(mostly)
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
