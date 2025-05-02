#pragma once

#include "GlobalMacros.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/fwd.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
UNIFORM_DEFS_NAMESPACE_BEGIN

template <size_t Size> struct UniformBuffer {
  // Internally aligned size
  static constexpr size_t size = Size;
};

struct CameraUniform : public UniformBuffer<128> { // aligned to 128B (of 256)
  glm::mat4 viewMatrix{1.0f}; // 64B - each column-vector is 16B
  glm::mat4 projMatrix{1.0f}; // 64B - each column-vector is 16B
};

struct GameObjectUniform : public UniformBuffer<64> { // aligned to 64B (of 256)
  glm::mat4 worldMatrix{1.0f};                        // 64B
};

struct BasicMaterialUniform
    : public UniformBuffer<16> {   // aligned to 16B (of 256)
  glm::vec4 baseColorFactor{1.0f}; // 16B
  // TODO: Add other material properties in the future cause right
  // now a lot of space is wasted (256 - 16)!!
};

struct PbrMaterialUniform
    : public UniformBuffer<16 + 16 + 16> { // aligned to 48B (of 256)
  glm::vec4 baseColorFactor{1.0f};         // 16B
                                           //
  float metallicFactor = 1.0f;             // 4B
  float roughnessFactor = 1.0f;            // 4B
  float alphaCutoff = 0.5f;                // 4B
  int hasBaseColorTexture = 0;             // 4B
                                           //
  int hasNormalTexture = 0;                // 4B
  int hasMetallicRoughnessTexture = 0;     // 4B
                                           // padding - 8B
};

struct LightsUniform : public UniformBuffer<16> { // aligned to 16B (of 256)
  glm::vec3 position{};                           // 16B
};

UNIFORM_DEFS_NAMESPACE_END
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
