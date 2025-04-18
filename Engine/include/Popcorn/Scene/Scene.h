#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"
#include <cstdint>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Scene {
public:
  Scene();
  virtual ~Scene();

  [[nodiscard]] inline std::vector<GameObject *> &GetGameObjects() {
    return m_nodes;
  };

  // Adds a node
  void AddGameObject(GameObject *node);
  void RemoveGameObject(GameObject *node);

  // Update nodes
  void Update();

private:
  static uint32_t s_sceneId;

  SceneData m_sceneData{}; // like ambient lights data, env map etc.
  std::vector<GameObject *> m_nodes;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
