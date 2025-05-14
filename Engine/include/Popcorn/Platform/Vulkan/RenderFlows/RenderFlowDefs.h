#pragma once

#include "Camera.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include "MaterialTypes.h"
#include "Memory/MemoryDefsVk.h"
#include "Uniforms.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum RenderFlows { GBuffer = 1, Lighting, Composite };

template <RenderFlows T> struct PcRenderFlowImages {
  virtual ~PcRenderFlowImages() = default;
  static constexpr RenderFlows type_value = T;
};

template <> struct PcRenderFlowImages<RenderFlows::GBuffer> {
  ImageVk albedoImage{};
  ImageVk depthImage{};
  ImageVk normalImage{};
};

template <> struct PcRenderFlowImages<RenderFlows::Lighting> {
  ImageVk lightImage{};
};

template <> struct PcRenderFlowImages<RenderFlows::Composite> {
  ImageVk presentImage{};
};

using namespace UniformDefs;

template <Uniforms U> struct PcCopyUniformToMemory;

template <> struct PcCopyUniformToMemory<Uniforms::BasicMat> {
public:
  PcCopyUniformToMemory(BasicMaterialData &data, PcBufferViews &bfrViews,
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
  BasicMaterialUniform uniform;
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
  PbrMaterialUniform uniform;
};

// template <> struct PcCopyUniformToMemory<Uniforms::Light> {
// public:
//   PcCopyUniformToMemory(Light *light, PcBufferViews &bfrViews,
//                         PcBufferOffsets &bfrOffsets)
//       : bufferViews(bfrViews), bufferOffsets(bfrOffsets) {
//     uniform.position = light->GetPosition();
//     uniform.worldMatrix = light->GetWorldMatrix();
//   }
//
//   void operator()(void *ssboMapping, size_t index) {
//     byte_t *dst = (byte_t *)ssboMapping + bufferViews.lightsSsbo.offset +
//                   bufferOffsets.lightsWorldMatrixOffsets[index];
//     memcpy(dst, &uniform, UniformDefs::LightUniform::size);
//   }
//
// private:
//   PcBufferViews &bufferViews;
//   PcBufferOffsets &bufferOffsets;
//   LightUniform uniform;
// };

template <> struct PcCopyUniformToMemory<Uniforms::Camera> {
public:
  PcCopyUniformToMemory(Camera *camera, PcBufferViews &bfrViews,
                        PcBufferOffsets &bfrOffsets)
      : bufferViews(bfrViews), bufferOffsets(bfrOffsets) {
    uniform.worldMatrix = camera->GetWorldMatrix();
    uniform.viewMatrix = camera->GetViewMatrix();
    uniform.projMatrix = camera->GetProjectionMatrix();
  }

  void operator()(void *uboMapping, size_t index) {
    byte_t *dst = (byte_t *)uboMapping + bufferViews.camerasUbo.offset +
                  bufferOffsets.camerasWorldMatrixOffsets[index];
    memcpy(dst, &uniform, UniformDefs::CameraUniform::size);
  }

private:
  PcBufferViews &bufferViews;
  PcBufferOffsets &bufferOffsets;
  CameraUniform uniform;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
