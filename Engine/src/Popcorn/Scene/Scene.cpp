#include "Scene.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
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
  for (GameObject *node : m_nodes) {
    delete node;
  }

  m_nodes.clear();
  m_gameObjectsByName.clear();
  --s_sceneId;

  m_animationTracks.clear();

  PC_PRINT("DESTROYED", TagType::Destr, "Scene");
};

// Adds a node
void Scene::AddGameObject(GameObject *node) {
  auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
  if (it == m_nodes.end()) {
    m_nodes.push_back(node);
    m_gameObjectsByName[node->GetName()] = node;
    // node->OnAttach();
  } else {
    PC_WARN("GameObject " << node << " already added")
  };
};

// Deletes node
void Scene::RemoveGameObject(GameObject *node) {
  auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
  if (it != m_nodes.end()) {
    m_nodes.erase(it);
    m_gameObjectsByName.erase(node->GetName());
    delete node;
  } else {
    PC_WARN("GameObject " << node << " not found in the scene library")
  };
};

// Update nodes
void Scene::Update() {
  // Updates m_sceneData{}; // like ambient lights data, env map etc.
  for (auto &node : m_nodes) {
    node->OnUpdate();
  }
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
