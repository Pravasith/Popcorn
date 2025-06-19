#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"
#include <cstdint>
#include <unordered_map>
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

  GameObject *FindObjectByName(const std::string &name) {
    auto it = m_gameObjectsByName.find(name); // O(1) time
    if (it != m_gameObjectsByName.end()) {
      return it->second;
    } else {
      PC_WARN("GameObject with name " << name << " not found in the scene");
      return nullptr;
    }
  }

  void AddGameObjectToNamedLookUp(const std::string &name,
                                  GameObject *gameObj) {
    m_gameObjectsByName[name] = gameObj;
  }

private:
  static uint32_t s_sceneId;

  SceneData m_sceneData{}; // like ambient lights data, env map etc.
  std::vector<GameObject *> m_nodes;

  std::unordered_map<std::string, GameObject *> m_gameObjectsByName;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
