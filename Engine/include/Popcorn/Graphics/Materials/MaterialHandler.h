#pragma once

#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "Renderer.h"
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
    };
  };

  void PrepareMaterialForRender() const {
    if (Renderer::GetAPI() == RendererType::Vulkan) {
      PC_WARN("Running smoothly")
    } else {
      // TODO: Other renderer
    };
  };

  void Draw() {};

private:
  MaterialHandler() { PC_PRINT("CREATED", TagType::Constr, "MaterialHandler") };
  ~MaterialHandler() {
    PC_PRINT("DESTROYED", TagType::Destr, "MaterialHandler")
  };

private:
  static MaterialHandler *s_instance;
  static std::vector<Material &> s_materials;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
