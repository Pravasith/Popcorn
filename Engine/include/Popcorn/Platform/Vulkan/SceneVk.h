#pragma once

#include "GlobalMacros.h"
#include "Scene.h"
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// TODO: MAKE A PROPER SCENE GRAPH
// For now, the scene graph contains a linear list of nodes
class SceneVk : public Scene {
public:
  SceneVk() { PC_PRINT("CREATED", TagType::Constr, "SceneVk"); };
  virtual ~SceneVk() override {
    PC_PRINT("DESTROYED", TagType::Destr, "SceneVk");
  };

  // Adds a node
  virtual void Add(GameObject *node) override {
    for (auto *node : m_nodes) {
      node->OnAttach();
    }
  };

  // Update nodes
  virtual void Update() override {};

  // Renders all nodes
  virtual void Render() override {
    for (auto *node : m_nodes) {
      node->OnRender();
    }
  };

private:
  std::vector<GameObject *> m_nodes;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
