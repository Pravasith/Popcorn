#pragma once

#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// TODO: Make this class handle batch rendering with Scene-Mesh-Material ECS
// system
// TODO: Handle the case of duplicating meshes & materials
class MaterialHandler {
public:
  static const MaterialHandler *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new MaterialHandler();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of MaterialHandler")
    };
  };

  void RegisterMaterial(Material *materialPtr) const;
  void UnRegisterMaterial(Material *materialPtr) const;

  void Draw();

private:
  MaterialHandler() { PC_PRINT("CREATED", TagType::Constr, "MaterialHandler") };
  ~MaterialHandler() {
    s_materialsLibrary.clear();
    PC_PRINT("DESTROYED", TagType::Destr, "MaterialHandler")
  };

private:
  static MaterialHandler *s_instance;
  static std::vector<Material *> s_materialsLibrary;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
