#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"
#include <glm/fwd.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum Lights {
  PointLight = 0,
  SpotLight,
  DirectionalLight,
};

struct LightData {
  Lights type = Lights::SpotLight;
  glm::vec3 color = glm::vec3(1.0f, 0.95f, 0.9f); // slightly warm white light
  float intensity =
      50.0f; // glTF uses lumen-like units, ~50 is reasonable indoors

  float range = 15.0f; // affects attenuation; 0 = infinite in some engines

  float innerConeAngle = glm::radians(15.0f); // fully lit
  float outerConeAngle = glm::radians(30.0f); // falloff region ends here
};

class Light : public GameObject {
public:
  Light() { PC_PRINT("CREATED", TagType::Constr, "Light"); }
  ~Light() { PC_PRINT("DESTROYED", TagType::Destr, "Light"); }

  virtual constexpr GameObjectTypes GetGameObjectType() const override {
    return GameObjectTypes::Light;
  };

  [[nodiscard]] const glm::vec3 &GetWorldDirection() const {
    return m_worldDirection;
  };
  [[nodiscard]] const LightData &GetLightData() const { return m_lightData; };

  virtual void OnAttach() {};
  // virtual void OnUpdate() {};
  // virtual void OnRender() {};

private:
  glm::vec3 m_worldDirection = glm::normalize(
      glm::vec3(0.0f, -1.0f, -0.5f)); // pointing downward and slightly forward
  LightData m_lightData{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
