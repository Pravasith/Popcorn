#pragma once

#include "GlobalMacros.h"
#include "Material.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class BasicMaterial : public Material {
public:
  BasicMaterial(MaterialData &matData) : Material(matData) {
    PC_PRINT("CREATED", TagType::Constr, "BasicMaterial.h");
  };
  virtual ~BasicMaterial() override {
    PC_PRINT("DESTROYED", TagType::Destr, "BasicMaterial.h");
  };

  virtual const MaterialTypes GetMaterialType() const override {
    return MaterialTypes::BasicMat;
  };

  virtual void Bind() override {
    // TODO: Do some bind stuff..
  };

  // FOR VULKAN -- Create basic material's descriptor set layout
  // Each type of material can only have one DSet layout(mostly)
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
