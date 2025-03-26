#pragma once

#include "GlobalMacros.h"
#include "PipelineFactoryVk.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class DescriptorFactoryVk {

public:
  template <Pipelines T> constexpr void GetPipelineLayout() {
    if constexpr (T == Pipelines::Base) {
      //  1. Global Descriptors - UBO - DSetLayout 0
      //      - View & proj (mat4)s
      //  2. Mesh specific Descriptors - Dynamic UBO - DSetLayout 1
      //      - Model matrix (mat4)
      //  3. Material specific Descriptors - UBO - DSetLayout 2
      //      - Albedo (vec4)
      //      - Roughness (float)
      //      - Metalness (float)
    }
  };

public:
  [[nodiscard]] inline static DescriptorFactoryVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new DescriptorFactoryVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN(
          "Trying to destroy a non-existant instance of DescriptorFactoryVk")
    };
  };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  DescriptorFactoryVk(const DescriptorFactoryVk &) = delete;
  DescriptorFactoryVk &operator=(const DescriptorFactoryVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  DescriptorFactoryVk(DescriptorFactoryVk &&) = delete;
  DescriptorFactoryVk &operator=(DescriptorFactoryVk &&) = delete;

private:
  DescriptorFactoryVk() {
    PC_PRINT("CREATED", TagType::Constr, "DescriptorFactoryVk.h")
  };
  ~DescriptorFactoryVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorFactoryVk.h")
  };

private:
  static DescriptorFactoryVk *s_instance;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
