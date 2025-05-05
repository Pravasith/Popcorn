#pragma once

#include "GlobalMacros.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/fwd.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
UNIFORM_DEFS_NAMESPACE_BEGIN

enum UniformLayouts {
  Submesh = 1, // TODO: per-MESH matrix instead of per-submesh
  BasicMat,
  PbrMat,
  Light,
  Camera,
  Empty,
  // add more..
};

template <UniformLayouts L, size_t S> struct Uniform {
  static constexpr UniformLayouts layout = L;
  // Internally aligned size
  static constexpr size_t size = S;
};

struct alignas(16) SubmeshUniform
    : public Uniform<UniformLayouts::Submesh, 64> { // aligned to 64B (of 256)
                                                    //
  glm::mat4 worldMatrix{1.0f};                      // 64B
                                                    // 192B free
};

struct BasicMaterialUniform : public Uniform<UniformLayouts::BasicMat,
                                             16> { // aligned to 16B (of 256)
                                                   //
  glm::vec4 baseColorFactor{1.0f};                 // 16B
                                                   // 240B free !!!
};

struct alignas(16) PbrMaterialUniform
    : public Uniform<UniformLayouts::PbrMat,
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

struct alignas(16) CameraUniform
    : public Uniform<UniformLayouts::Camera,
                     192> {    // aligned to 192B (of 256B)
                               //
  glm::mat4 worldMatrix{1.0f}; // 64B
  glm::mat4 viewMatrix{1.0f};  // 64B
  glm::mat4 projMatrix{1.0f};  // 64B
                               //
                               // 64B free
};

struct alignas(16) LightUniform
    : public Uniform<UniformLayouts::Light,
                     80> {     // aligned to 80B (of 256B)
                               //
  glm::vec3 position{0.0f};    // 12B
  float pad0;                  // 4B (padding)
                               //
  glm::mat4 worldMatrix{1.0f}; // 64B
                               //
                               // 176B free
};

struct alignas(16) EmptyUniform
    : public Uniform<UniformLayouts::Empty,
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
