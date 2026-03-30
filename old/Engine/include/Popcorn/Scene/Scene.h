#pragma once

#include "Animation.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Time.h"
#include <cstddef>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Scene {
public:
  Scene();
  virtual ~Scene();

  [[nodiscard]] std::vector<GameObject *> &GetGameObjects() { return m_nodes; };

  [[nodiscard]] AnimationTrack &GetAnimationTrack(uint32_t trackIndex) {
    return m_animationTracks.at(trackIndex);
  }
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

  void AddAnimationTrack(AnimationTrack &&track) {
    auto [it, _] =
        m_animationTracks.emplace(m_animTrackSize++, std::move(track));
    Time::Get()->Subscribe(&it->second);

    PC_WARN(m_animationTracks.size() << " TRACK SIZE AFTER SETTING IN SCENE")
  }

  // Animation tracks - for GltfLoader only
  void SetAnimationTracks(const std::vector<AnimationTrack> &tracks) {
    if (tracks.empty()) {
      PC_WARN("Scene: Scene has no animation tracks, so skipping")
      return;
    }

    m_animationTracks.clear();
    m_animTrackSize = 0;

    for (const auto &track : tracks) {
      auto [it, _] = m_animationTracks.emplace(m_animTrackSize++, track);
      Time::Get()->Subscribe(&it->second);
    }

    PC_WARN(m_animationTracks.size() << " TRACK SIZE AFTER SETTING IN SCENE")
  }

private:
  static uint32_t s_sceneId;

  SceneData m_sceneData{}; // like ambient lights data, env map etc.
  std::vector<GameObject *> m_nodes;

  std::unordered_map<std::string, GameObject *> m_gameObjectsByName;
  std::map<uint32_t, AnimationTrack> m_animationTracks;
  uint32_t m_animTrackSize = 0;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
