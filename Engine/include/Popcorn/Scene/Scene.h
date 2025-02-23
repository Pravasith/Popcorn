#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"
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
  inline void Add(GameObject *node);

  // Update nodes
  inline void OnUpdate();

  void RegisterMaterial(Material *materialPtr);
  void UnRegisterMaterial(Material *materialPtr);

  [[nodiscard]] inline const std::vector<Material *> &
  GetSceneMaterials() const {
    return m_sceneMaterials;
  };

private:
  static uint32_t s_sceneId;

  SceneData m_sceneData{}; // like ambient lights data, env map etc.
  std::vector<GameObject *> m_nodes;
  std::vector<Material *> m_sceneMaterials;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
