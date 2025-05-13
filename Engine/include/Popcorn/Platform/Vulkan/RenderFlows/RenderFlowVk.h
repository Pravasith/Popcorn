#pragma once

#include "Camera.h"
#include "Empty.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include "Light.h"
#include "Material.h"
#include "MaterialTypes.h"
#include "Mesh.h"
#include "Popcorn/Core/Base.h"
#include "SamplerVk.h"
#include <cstdint>
#include <cstring>
#include <vector>
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

// TODO: Refactor
//       - Vulkan specific for now.
//       - Make it platform agnostic
class RenderFlowVk {
public:
  RenderFlowVk() { PC_PRINT("CREATED", TagType::Constr, "RenderFlowVk") };
  virtual ~RenderFlowVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "RenderFlowVk")
  };

  static void AllocMemory();
  static void FreeMemory();
  static void CreateSamplers();
  static void DestroySamplers();

  void Prepare() {
    CreateAttachments();
    CreateRenderPass();
    CreateFramebuffer();
  };

  void CleanUp() {
    DestroyFramebuffer();
    DestroyRenderPass();
    DestroyAttachments();
  };

private:
  virtual void CreateAttachments() = 0;
  virtual void CreateRenderPass() = 0;
  virtual void CreateFramebuffer() = 0;

  virtual void DestroyFramebuffer() = 0;
  virtual void DestroyRenderPass() = 0;
  virtual void DestroyAttachments() = 0;

public:
  virtual void CreateAndAllocDescriptors() = 0;
  virtual void DestroyDescriptors() = 0;

  virtual void CreatePipelines() = 0;
  static void ProcessSceneUpdates(const uint32_t currentFrame); // Only once
  static void
  RecordRenderCommands(const uint32_t frameIndex, const uint32_t currentFrame,
                       VkCommandBuffer &currentFrameCommandBuffer); // Only once

  //
  //
  //
  //
  //
  //

  //
  // OLD ----------------------------------------------------------------
  // virtual void AllocateVkVertexBuffers() {};
  // virtual void AllocateVkIndexBuffers() {};
  // virtual void AllocateVkUniformBuffers() {};
  // virtual void CreateCommandBuffer() {};
  // virtual void CreateDescriptorSetLayouts() {};
  // virtual void CreateDescriptorPool() {};
  // virtual void CreateDescriptorSets() {};
  // --------------------------------------------------------------------
  //

  static void AddCamera(Camera *camera) {
    PC_ValidateAndAddGameObject(camera, s_cameras);
  };
  static void AddEmpty(Empty *empty) {
    PC_ValidateAndAddGameObject(empty, s_emptys);
  };
  static void AddLight(Light *light) {
    PC_ValidateAndAddGameObject(light, s_lights);
  };

  static void RemoveCamera(Camera *camera) {
    PC_ValidateAndRemoveGameObject(camera, s_cameras);
  };
  static void RemoveEmpty(Empty *empty) {
    PC_ValidateAndRemoveGameObject(empty, s_emptys);
  };
  static void RemoveLight(Light *light) {
    PC_ValidateAndRemoveGameObject(light, s_lights);
  };

  template <MaterialTypes T>
  static void RegisterMaterialAndSubmesh(Submesh<T> *submesh) {
    if constexpr (T == MaterialTypes::BasicMat) {
      uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
      if (PC_ValidateAndAddSubmesh(submesh,
                                   s_basicMatSubmeshesMap[materialId])) {
        s_basicMaterials[materialId] = submesh->GetMaterial();
        ++s_submeshCount;
      };
    } else if constexpr (T == MaterialTypes::PbrMat) {
      uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
      if (PC_ValidateAndAddSubmesh(submesh, s_pbrMatSubmeshesMap[materialId])) {
        s_pbrMaterials[materialId] = submesh->GetMaterial();
        ++s_submeshCount;
      };
    }
  };

  template <MaterialTypes T>
  static void UnregisterMaterialAndSubmesh(Submesh<T> *submesh) {
    if constexpr (T == MaterialTypes::BasicMat) {
      uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
      if (PC_ValidateAndRemoveSubmesh(submesh,
                                      s_basicMatSubmeshesMap[materialId])) {
        --s_submeshCount;
      };
    } else if constexpr (T == MaterialTypes::PbrMat) {
      uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
      if (PC_ValidateAndRemoveSubmesh(submesh,
                                      s_pbrMatSubmeshesMap[materialId])) {
        --s_submeshCount;
      };
    }
  };

public:
  struct SamplersVk {
    SamplerVk frameSampler;
  };

protected:
  //
  // Game object refs -------------------------------------------------------
  static PcMaterialSubmeshesMap<MaterialTypes::BasicMat> s_basicMatSubmeshesMap;
  static PcMaterialSubmeshesMap<MaterialTypes::PbrMat> s_pbrMatSubmeshesMap;

  static std::vector<Light *> s_lights;
  static std::vector<Camera *> s_cameras;
  static std::vector<Empty *> s_emptys; // useless for now

  //
  // Material values --------------------------------------------------------
  static PcMaterialMap<MaterialTypes::BasicMat> s_basicMaterials;
  static PcMaterialMap<MaterialTypes::PbrMat> s_pbrMaterials;

  // Images
  static PcRenderFlowImages<GBuffer> s_gBufferImages;
  static PcRenderFlowImages<Lighting> s_lightingImages;
  static PcRenderFlowImages<Composite> s_compositeImages;

  //
  // Samplers ---------------------------------------------------------------
  static SamplersVk s_samplersVk;

  //
  // ------------------------------------------------------------------------
  static uint64_t s_submeshCount;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
