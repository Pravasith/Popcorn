#pragma once

#include "Camera.h"
#include "CommonVk.h"
#include "Empty.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Light.h"
#include "MaterialTypes.h"
#include "Mesh.h"
#include "RenderFlowDefs.h"
#include "SamplerVk.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// TODO: Refactor
//       - Vulkan specific for now.
//       - Make it platform agnostic
class RenderFlowVk {
public:
  RenderFlowVk();
  virtual ~RenderFlowVk();

  static void AllocMemory();
  static void FreeMemory();

  static void CreateSamplers();
  static void DestroySamplers();

  static void CopyDynamicUniformsToMemory(const uint32_t currentFrame);

  static void AllocShaders();
  static void FreeShaders();

  static void AllocDescriptorsGlobal();
  static void UpdateDescriptorSetsGlobal();

public:
#ifdef PC_DEBUG
  virtual void PrintVboIbo() {};
#endif

public:
  virtual void AllocDescriptorsLocal() = 0; // Automatically destroyed
  virtual void UpdateDescriptorSetsLocal() = 0;

  virtual void CreatePipelines() = 0;
  virtual void DestroyPipelines() = 0;

  virtual void OnSwapchainInvalidCb() = 0;
  virtual void RecordCommandBuffer(const uint32_t frameIndex,
                                   const uint32_t currentFrame) = 0;

  virtual std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> &
  GetCommandBuffers() = 0;

private:
  virtual void CreateAttachments() = 0;
  virtual void CreateImageBarriers() = 0;
  virtual void CreateRenderPass() = 0;
  virtual void CreateFramebuffers() = 0;

  virtual void CreateCommandBuffers() = 0;

  virtual void DestroyFramebuffers() = 0;
  virtual void DestroyRenderPass() = 0;
  virtual void DestroyAttachments() = 0;

public:
  void Prepare() {
    PC_WARN("Preparing Renderflow............................")

    CreateAttachments();
    CreateImageBarriers();
    CreateRenderPass();
    CreateFramebuffers();
    CreateCommandBuffers();
  };

  void CleanUp() {
    DestroyFramebuffers();
    DestroyRenderPass();
    DestroyAttachments();
  };

public:
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
  static void RegisterMaterialAndSubmesh(Submesh<T> *submesh);
  template <MaterialTypes T>
  static void UnregisterMaterialAndSubmesh(Submesh<T> *submesh);

public:
  struct SamplersVk {
    SamplerVk colorSampler;
    SamplerVk depthSampler;
  };

  struct DescriptorSetsVkStatic {
    PcFramesDescriptorSets<MAX_FRAMES_IN_FLIGHT> cameraSets{};
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
  static PcRenderFlowImages<GBuffer, MAX_FRAMES_IN_FLIGHT> s_gBufferImages;
  static PcRenderFlowImages<Lighting, MAX_FRAMES_IN_FLIGHT> s_lightingImages;
  static PcRenderFlowImages<Composite>
      s_compositeImages; // this is a vector, and the others above are arrays

  //
  // Samplers ---------------------------------------------------------------
  static SamplersVk s_samplersVk;

  //
  // ------------------------------------------------------------------------
  static uint64_t s_submeshCount;

  static DescriptorSetsVkStatic s_commonDescriptorSets;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
