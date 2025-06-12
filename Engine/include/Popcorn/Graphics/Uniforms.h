#pragma once

#include "GlobalMacros.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
UNIFORM_DEFS_NAMESPACE_BEGIN

enum class Uniforms {
  Submesh, // TODO: per-MESH matrix instead of per-submesh
  BasicMat,
  PbrMat,
  Light,
  Camera,
  Empty,
  // add more..
};

template <Uniforms L, size_t S> struct Uniform {
  static constexpr Uniforms layout = L;
  // Internally aligned size
  static constexpr size_t size = S;
};

struct alignas(16) CameraUniform
    : public Uniform<Uniforms::Camera,
                     256> {          // aligned to 256B (of 256)
                                     //
  glm::mat4 viewMatrix{1.0f};        // 64B
  glm::mat4 projMatrix{1.0f};        // 64B
  glm::mat4 viewProjMatrix{1.0f};    // 64B
  glm::mat4 invViewProjMatrix{1.0f}; // 64B
};

struct alignas(16) SubmeshUniform
    : public Uniform<Uniforms::Submesh, 128> { // aligned to 128B (of 256)
                                               //
  glm::mat4 worldMatrix{1.0f};                 // 64B
  glm::mat4 normalMatrix{1.0f};                // 64B
                                               // 128B free
};

struct BasicMaterialUniform : public Uniform<Uniforms::BasicMat,
                                             16> { // aligned to 16B (of 256)
                                                   //
  glm::vec4 baseColorFactor{1.0f};                 // 16B
                                                   // 240B free !!!
};

struct alignas(16) PbrMaterialUniform
    : public Uniform<Uniforms::PbrMat,
                     16 + 16 + 16> {   // aligned to 48B (of 256)
                                       //
  glm::vec4 baseColorFactor{1.0f};     // 16B
                                       //
  float metallicFactor = 1.0f;         // 4B
  float roughnessFactor = 1.0f;        // 4B
  float alphaCutoff = 0.5f;            // 4B
  int hasBaseColorTexture = 0;         // 4B
                                       //
  int hasNormalTexture = 0;            // 4B
  int hasMetallicRoughnessTexture = 0; // 4B
  float pad0;                          // 4B (padding)
  float pad1;                          // 4B (padding)
                                       //
                                       // 208B free
};

// Note: this is an SSBO in vulkan
struct alignas(16) LightUniform
    : public Uniform<Uniforms::Light, 64> {   // aligned to 64B
  glm::vec3 worldPos{0.0f};                   // 12B
  float pad0;                                 // 4B (optional padding)
                                              //
  glm::vec3 worldDir{0.0f};                   // 12B
  float pad1;                                 // 4B (optional padding)
                                              //
  glm::vec3 color{1.0f, 0.95f, 0.9f};         // 12B
  float pad2;                                 // 4B (optional padding)
                                              //
  float lightType = 1.0f;                     // 4B
  float intensity = 50.0f;                    // 4B
  float innerConeAngle = glm::radians(15.0f); // 4B
  float outerConeAngle = glm::radians(30.0f); // 4B
};

struct alignas(16) EmptyUniform
    : public Uniform<Uniforms::Empty,
                     80> {     // aligned to 80B (of 256B)
                               //
  glm::vec3 position{0.0f};    // 12B
  float pad0;                  // 4B (padding)
                               //
  glm::mat4 worldMatrix{1.0f}; // 64B
                               //
                               // 176B free
};

UNIFORM_DEFS_NAMESPACE_END
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
