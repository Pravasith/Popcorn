#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "Camera.h"
#include "Empty.h"
#include "GlobalMacros.h"
#include "Light.h"
#include "MaterialTypes.h"
#include "Memory/MemoryDefsVk.h"
#include "Uniforms.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

using namespace UniformDefs;

template <Uniforms U> struct PcCopyUniformToMemory;

template <> struct PcCopyUniformToMemory<Uniforms::BasicMat> {
public:
  PcCopyUniformToMemory(const BasicMaterialData &data, PcBufferViews &bfrViews,
                        PcBufferOffsets &bfrOffsets)
      : bufferViews(bfrViews), bufferOffsets(bfrOffsets) {
    uniform.baseColorFactor = data.baseColorFactor;
  };
  ~PcCopyUniformToMemory() = default;

  void operator()(void *uboMapping, MaterialHashType matId) {
    byte_t *basicMatUboMapping =
        (byte_t *)uboMapping + bufferViews.basicMatUbo.offset +
        bufferOffsets.materialOffsets[matId]; // all offsets are aligned
    uint64_t uniformSize = UniformDefs::BasicMaterialUniform::size;
    memcpy(basicMatUboMapping, &uniform, uniformSize);
  };

private:
  PcBufferViews &bufferViews;
  PcBufferOffsets &bufferOffsets;
  BasicMaterialUniform uniform{};
};

template <> struct PcCopyUniformToMemory<Uniforms::PbrMat> {
public:
  PcCopyUniformToMemory(const PbrMaterialData &data, PcBufferViews &bfrViews,
                        PcBufferOffsets &bfrOffsets)
      : bufferViews(bfrViews), bufferOffsets(bfrOffsets) {
    uniform.baseColorFactor = data.baseColorFactor;
    uniform.metallicFactor = data.metallicFactor;
    uniform.roughnessFactor = data.roughnessFactor;
    uniform.alphaCutoff = data.alphaCutoff;
    uniform.hasBaseColorTexture = data.hasBaseColorTexture;
    uniform.hasNormalTexture = data.hasNormalTexture;
    uniform.hasMetallicRoughnessTexture = data.hasMetallicRoughnessTexture;
  }

  void operator()(void *uboMapping, MaterialHashType matId) {
    byte_t *dst = (byte_t *)uboMapping + bufferViews.pbrMatUbo.offset +
                  bufferOffsets.materialOffsets[matId];
    memcpy(dst, &uniform, UniformDefs::PbrMaterialUniform::size);
  }

private:
  PcBufferViews &bufferViews;
  PcBufferOffsets &bufferOffsets;
  PbrMaterialUniform uniform{};
};

template <> struct PcCopyUniformToMemory<Uniforms::Submesh> {
public:
  PcCopyUniformToMemory(const glm::mat4 &worldMatrix,
                        const glm::mat4 &normalMatrix, PcBufferViews &bfrViews,
                        PcBufferOffsets &bfrOffsets)
      : bufferViews(bfrViews), bufferOffsets(bfrOffsets) {
    uniform.worldMatrix = worldMatrix;
    uniform.normalMatrix = normalMatrix;
  }

  void operator()(void *uboMapping, MaterialHashType matId,
                  size_t submeshIndex) {
    const auto &offsetTriple =
        bufferOffsets.submeshesOffsets[matId][submeshIndex];
    byte_t *dst = (byte_t *)uboMapping + bufferViews.submeshUbo.offset +
                  offsetTriple.uboOffset;
    memcpy(dst, &uniform, UniformDefs::SubmeshUniform::size);
  }

private:
  PcBufferViews &bufferViews;
  PcBufferOffsets &bufferOffsets;
  SubmeshUniform uniform{};
};

template <> struct PcCopyUniformToMemory<Uniforms::Light> {
public:
  PcCopyUniformToMemory(Light *light, Camera *activeCamera,
                        PcBufferViews &bfrViews, PcBufferOffsets &bfrOffsets)
      : bufferViews(bfrViews), bufferOffsets(bfrOffsets) {
    const LightData &lightData = light->GetLightData();
    const glm::vec3 &worldPos = light->GetPosition();

    // PC_WARN("LIGHT POSITION BEFORE COPY " << worldPos.x << ", " << worldPos.y
    //                                       << ", " << worldPos.z)

    // TODO: change direction (currently hardcoded)
    const glm::vec3 &worldDir = light->GetLookAtDirection();

    uniform.worldPos = worldPos;
    uniform.worldDir = worldDir;
    uniform.color = lightData.color;
    uniform.lightType = static_cast<float>(lightData.type);
    uniform.intensity =
        lightData.intensity; // TEMP_DEBUG ~ treating intensity as power
    uniform.innerConeAngle = lightData.innerConeAngle;
    uniform.outerConeAngle = lightData.outerConeAngle;
  }

  void operator()(void *ssboMapping, size_t index) {
    byte_t *lightSsboMapping =
        (byte_t *)ssboMapping + bufferViews.lightsSsbo.offset +
        bufferOffsets.lightsOffsets[index]; // aligned offset

    constexpr uint64_t uniformSize = UniformDefs::LightUniform::size;
    std::memcpy(lightSsboMapping, &uniform, uniformSize);
  }

private:
  PcBufferViews &bufferViews;
  PcBufferOffsets &bufferOffsets;
  LightUniform uniform{};
};

template <> struct PcCopyUniformToMemory<Uniforms::Camera> {
public:
  PcCopyUniformToMemory(Camera *camera, PcBufferViews &bfrViews,
                        PcBufferOffsets &bfrOffsets)
      : bufferViews(bfrViews), bufferOffsets(bfrOffsets) {
    const glm::mat4 &view = camera->GetViewMatrix();
    const glm::mat4 &proj = camera->GetProjectionMatrix();
    const glm::mat4 viewProj = proj * view;

    uniform.viewMatrix = view;
    uniform.projMatrix = proj;
    uniform.viewProjMatrix = viewProj;
    uniform.invViewProjMatrix = glm::inverse(viewProj);
  }

  void operator()(void *uboMapping, size_t index) {
    byte_t *dst = (byte_t *)uboMapping + bufferViews.camerasUbo.offset +
                  bufferOffsets.camerasOffsets[index];
    memcpy(dst, &uniform, UniformDefs::CameraUniform::size);
  }

private:
  PcBufferViews &bufferViews;
  PcBufferOffsets &bufferOffsets;
  CameraUniform uniform{};
};

template <> struct PcCopyUniformToMemory<Uniforms::Empty> {
public:
  PcCopyUniformToMemory(Empty *empty, PcBufferViews &bfrViews,
                        PcBufferOffsets &bfrOffsets)
      : bufferViews(bfrViews), bufferOffsets(bfrOffsets) {
    uniform.position = empty->GetPosition();
    uniform.worldMatrix = empty->GetWorldMatrix();
  }

  void operator()(void *uboMapping, size_t index) {
    byte_t *dst = (byte_t *)uboMapping + bufferViews.emptysUbo.offset +
                  bufferOffsets.emptysOffsets[index];
    memcpy(dst, &uniform, UniformDefs::EmptyUniform::size);
  }

private:
  PcBufferViews &bufferViews;
  PcBufferOffsets &bufferOffsets;
  EmptyUniform uniform;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
