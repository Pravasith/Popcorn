#include "SceneManager.h"
#include "Material.h"
#include "Renderer.h"
#include <algorithm>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

SceneManager *SceneManager::s_instance = nullptr;
std::vector<Material *> SceneManager::s_materialsLibrary = {};

void SceneManager::RegisterMaterial(Material *materialPtr) const {
  auto ptr = std::find(s_materialsLibrary.begin(), s_materialsLibrary.end(),
                       materialPtr);
  if (ptr != s_materialsLibrary.end()) {
    PC_WARN("Material already exists in the material library!")
    return;
  };

  //
  // ADD TO MATERIAL LIBRARY ---------------------------------------------
  s_materialsLibrary.push_back(materialPtr);

  //
  // INIT MATERIAL PIPELINES ---------------------------------------------
  auto &rendererStn = Renderer::Get();
  rendererStn.PrepareMaterialForRender(materialPtr);
}

void SceneManager::UnRegisterMaterial(Material *materialPtr) const {
  auto ptr = std::find(s_materialsLibrary.begin(), s_materialsLibrary.end(),
                       materialPtr);
  if (ptr == s_materialsLibrary.end()) {
    PC_WARN("Material doesn't exist in the material library!")
    return;
  };

  s_materialsLibrary.erase(ptr);
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
