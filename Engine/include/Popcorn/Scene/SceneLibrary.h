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
  };

  void Add(Scene *scene) {
    m_scenes.emplace_back(scene);
    PC_PRINT("Scene added to library", TagType::Print, "SceneLibrary")
  };

  void Remove(Scene *node) {
    auto it = std::find(m_scenes.begin(), m_scenes.end(), node);
    if (it != m_scenes.end()) {
      m_scenes.erase(it);

      PC_PRINT("Scene removed from library", TagType::Print, "SceneLibrary")
    };
  };

private:
  std::vector<Scene *> m_scenes;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
