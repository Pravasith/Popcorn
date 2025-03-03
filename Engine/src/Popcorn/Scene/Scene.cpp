#include "Scene.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Mesh.h"
#include "Renderer.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

uint32_t Scene::s_sceneId = 0;
Renderer *Scene::s_rendererStn = nullptr;

Scene::Scene() {
  PC_PRINT("CREATED with id: " << m_sceneData.sceneId, TagType::Constr,
           "Scene");
  ++s_sceneId;
  m_sceneData.sceneId = s_sceneId;
  s_rendererStn = &Renderer::Get();
};

Scene::~Scene() {
  m_nodes.clear();
  --s_sceneId;

  PC_PRINT("DESTROYED", TagType::Destr, "Scene");
};

// Adds a node
void Scene::Add(GameObject *node) {
  node->OnAttach(m_sceneData);
  m_nodes.push_back(node);

  if (node->GetType() == GameObjectTypes::Mesh) {
    auto *meshPtr = (Mesh *)node;
    s_rendererStn->AddMeshToWorkflow(meshPtr);
  };
};

// Update nodes
void Scene::OnUpdate() {
  // Updates m_sceneData{}; // like ambient lights data, env map etc.
  for (auto &node : m_nodes) {
    node->OnUpdate(m_sceneData);
  }
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
