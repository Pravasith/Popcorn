#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// TODO: MAKE A PROPER SCENE GRAPH
// For now, the scene graph contains a linear list of nodes
class Scene {
public:
  Scene() {
    ++s_sceneId;
    m_sceneData.sceneId = s_sceneId;
    PC_PRINT("CREATED with id: " << m_sceneData.sceneId, TagType::Constr,
             "Scene");
  };
  virtual ~Scene() {
    m_nodes.clear();
    --s_sceneId;
    PC_PRINT("DESTROYED", TagType::Destr, "Scene");
  };

  [[nodiscard]] inline const std::vector<GameObject *> &GetNodes() const {
    return m_nodes;
  };

  // Adds a node
  inline void Add(GameObject *node) {
    node->OnAttach(m_sceneData);
    m_nodes.push_back(node);
  };

  // Update nodes
  inline void OnUpdate() {
    // Updates m_sceneData{}; // like ambient lights data, env map etc.
    for (auto &node : m_nodes) {
      node->OnUpdate(m_sceneData);
    }
  };

  // Renders all nodes
  inline void OnRender() {
    for (auto &node : m_nodes) {
      node->OnRender(m_sceneData);
    }
  };

private:
  static uint32_t s_sceneId;

  std::vector<GameObject *> m_nodes{};
  SceneData m_sceneData{}; // like ambient lights data, env map etc.
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
