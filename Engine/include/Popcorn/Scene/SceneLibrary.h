#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "Scene.h"
#include <algorithm>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class SceneLibrary {
public:
  SceneLibrary() { PC_PRINT("CREATED", TagType::Constr, "SceneLibrary"); };
  ~SceneLibrary() { PC_PRINT("DESTROYED", TagType::Destr, "SceneLibrary"); };

  [[nodiscard]] inline const std::vector<Scene *> &GetScenes() const {
    return m_scenes;
  }

  void Add(Scene *scene) {
    m_scenes.emplace_back(scene);
    PC_PRINT("Scene added to library", TagType::Print, "SceneLibrary")
  }

  void Remove(Scene *node) {
    auto it = std::find(m_scenes.begin(), m_scenes.end(), node);
    if (it != m_scenes.end()) {
      m_scenes.erase(it);

      PC_PRINT("Scene removed from library", TagType::Print, "SceneLibrary")
    }
  }

  void PrintScenes() {
    PC_PRINT("Printing Scenes: ------------------------ \n", TagType::Print,
             "SceneLib");

    for (auto &scene : m_scenes) {
      auto &gameObjs = scene->GetGameObjects();

      std::function<void(GameObject *)> printNode = [&](GameObject *node) {
        if (!node)
          return;

        auto &pos = node->GetPosition();
        auto objType = node->GetGameObjectType();

        switch (objType) {
        case GameObjectTypes::None:
          PC_PRINT("None: ", TagType::Print, "Scene");
          break;
        case GameObjectTypes::Mesh:
          PC_PRINT("Mesh: ", TagType::Print, "Scene");
          break;
        case GameObjectTypes::Camera:
          // node->SetPosition({2.f, 2.f, 2.f});
          PC_PRINT("Camera: ", TagType::Print, "Scene");
          break;
        case GameObjectTypes::Empty:
          PC_PRINT("Empty: ", TagType::Print, "Scene");
          break;
        case GameObjectTypes::Light:
          PC_PRINT("Light: ", TagType::Print, "Scene");
          break;
        }

        PC_PRINT(pos.x << ", " << pos.y << ", " << pos.z << '\n',
                 TagType::Print, "Position")

        for (auto &child : node->GetChildren()) {
          printNode(child);
        }
      };

      // Start traversal for each root object
      for (auto *obj : gameObjs) {
        printNode(obj);
      }
    }
  }

private:
  std::vector<Scene *> m_scenes;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
