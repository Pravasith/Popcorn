#include "Scene.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Mesh.h"
#include "Renderer.h"
#include <algorithm>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

uint32_t Scene::s_sceneId = 0;

Scene::Scene() {
  PC_PRINT("CREATED with id: " << m_sceneData.sceneId, TagType::Constr,
           "Scene");
  ++s_sceneId;
  m_sceneData.sceneId = s_sceneId;
};

Scene::~Scene() {
  m_sceneMaterials.clear();
  m_nodes.clear();
  --s_sceneId;

  PC_PRINT("DESTROYED", TagType::Destr, "Scene");
};

// Adds a node
void Scene::Add(GameObject *node) {
  node->OnAttach(m_sceneData);
  m_nodes.push_back(node);

  if (node->GetType() == GameObjectTypes::Mesh) {
    RegisterMaterial(&static_cast<Mesh *>(node)->GetMaterial());
  };
};

// Update nodes
void Scene::OnUpdate() {
  // Updates m_sceneData{}; // like ambient lights data, env map etc.
  for (auto &node : m_nodes) {
    node->OnUpdate(m_sceneData);
  }
};

void Scene::RegisterMaterial(Material *materialPtr) {
  auto ptr =
      std::find(m_sceneMaterials.begin(), m_sceneMaterials.end(), materialPtr);
  if (ptr != m_sceneMaterials.end()) {
    PC_WARN("Material already exists in the material library!")
    return;
  };

  //
  // ADD TO MATERIAL LIBRARY ---------------------------------------------
  m_sceneMaterials.push_back(materialPtr);

  //
  // INIT MATERIAL PIPELINES ---------------------------------------------
  auto &rendererStn = Renderer::Get();
  rendererStn.PrepareMaterialForRender(materialPtr);
}

void Scene::UnRegisterMaterial(Material *materialPtr) {
  auto ptr =
      std::find(m_sceneMaterials.begin(), m_sceneMaterials.end(), materialPtr);

  if (ptr == m_sceneMaterials.end()) {
    PC_WARN("Material doesn't exist in the material library!")
    return;
  };

  m_sceneMaterials.erase(ptr);
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
