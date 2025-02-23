#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Mesh;
class Material;

// TODO: Make this class handle batch rendering with Scene-Mesh-Material ECS
// system
// TODO: Handle the case of duplicating meshes & materials
class SceneManager {
public:
  static const SceneManager *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new SceneManager();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of SceneManager")
    };
  };

  void RegisterMaterial(Material *materialPtr) const;
  void UnRegisterMaterial(Material *materialPtr) const;

private:
  SceneManager() { PC_PRINT("CREATED", TagType::Constr, "SceneManager") };
  ~SceneManager() {
    s_materialsLibrary.clear();
    PC_PRINT("DESTROYED", TagType::Destr, "SceneManager")
  };

private:
  static SceneManager *s_instance;
  static std::vector<Material *> s_materialsLibrary;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
