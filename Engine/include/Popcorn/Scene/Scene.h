#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"
#include "Renderer.h"
#include <cstdint>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Material;

// TODO: MAKE A PROPER SCENE GRAPH
// For now, the scene graph contains a linear list of nodes
class Scene {
public:
  Scene();
  virtual ~Scene();

  [[nodiscard]] inline const std::vector<GameObject *> &GetNodes() const {
    return m_nodes;
  };

  // Adds a node
  void Add(GameObject *node);

  // Update nodes
  void OnUpdate();

private:
  static uint32_t s_sceneId;
  static Renderer *s_rendererStn;

  SceneData m_sceneData{}; // like ambient lights data, env map etc.
  std::vector<GameObject *> m_nodes;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
