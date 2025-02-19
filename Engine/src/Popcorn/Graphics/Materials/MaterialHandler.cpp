#include "MaterialHandler.h"

#include "Renderer.h"
#include <algorithm>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

MaterialHandler *MaterialHandler::s_instance = nullptr;
std::vector<Material *> MaterialHandler::s_materialsLibrary = {};

void MaterialHandler::RegisterMaterial(Material *materialPtr) const {
  auto ptr = std::find(s_materialsLibrary.begin(), s_materialsLibrary.end(),
                       materialPtr);
  if (ptr != s_materialsLibrary.end()) {
    PC_WARN("Material already exists in the material library!")
    return;
  };

  s_materialsLibrary.push_back(materialPtr);
}

void MaterialHandler::UnRegisterMaterial(Material *materialPtr) const {
  auto ptr = std::find(s_materialsLibrary.begin(), s_materialsLibrary.end(),
                       materialPtr);
  if (ptr == s_materialsLibrary.end()) {
    PC_WARN("Material doesn't exist in the material library!")
    return;
  };

  s_materialsLibrary.erase(ptr);
}

void MaterialHandler::CreateRenderWorkflow() {
  // BASIC WORKFLOW

};

// if (Renderer::GetAPI() == RendererType::Vulkan) {
//   // Init pipeline according to the settings
// } else {
//   // TODO: Other renderer
// };

void MaterialHandler::Draw() {};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
