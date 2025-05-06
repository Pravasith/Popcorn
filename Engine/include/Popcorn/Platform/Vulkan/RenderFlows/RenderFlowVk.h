#pragma once

#include "Camera.h"
#include "Empty.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Light.h"
#include "Material.h"
#include "MaterialTypes.h"
#include "Mesh.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <cstring>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum RenderFlows { GBuffer = 1, Lighting, Composite };

using BasicSubmeshes = std::vector<Submesh<MaterialTypes::BasicMat> *>;
using PbrSubmeshes = std::vector<Submesh<MaterialTypes::PbrMat> *>;

using BasicMaterialSubmeshes =
    std::unordered_map<MaterialHashType, BasicSubmeshes>;
using PbrMaterialSubmeshes = std::unordered_map<MaterialHashType, PbrSubmeshes>;

template <MaterialTypes T>
using Submeshes =
    std::unordered_map<MaterialHashType, std::vector<Submesh<T> *>>;

template <MaterialTypes T>
using Materials = std::unordered_map<MaterialHashType, Material<T> *>;

// TODO: Refactor
//       - Vulkan specific for now.
//       - Make it platform agnostic
class RenderFlowVk {
public:
  RenderFlowVk() { PC_PRINT("CREATED", TagType::Constr, "RenderFlowVk") };
  virtual ~RenderFlowVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "RenderFlowVk")
  };

  //
  // NEW ----------------------------------------------------------------
  static void AllocMemory(); // Only once
  static void FreeMemory();  // Only once

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
      if (PC_ValidateAndAddSubmesh(submesh, s_basicSubmeshGroups[materialId])) {
        s_basicMaterials[materialId] = submesh->GetMaterial();
        ++s_submeshCount;
      };
    } else if constexpr (T == MaterialTypes::PbrMat) {
      uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
      if (PC_ValidateAndAddSubmesh(submesh, s_pbrSubmeshGroups[materialId])) {
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
                                      s_basicSubmeshGroups[materialId])) {
        --s_submeshCount;
      };
    } else if constexpr (T == MaterialTypes::PbrMat) {
      uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
      if (PC_ValidateAndRemoveSubmesh(submesh,
                                      s_pbrSubmeshGroups[materialId])) {
        --s_submeshCount;
      };
    }
  };

protected:
  static std::vector<Camera *> s_cameras;
  static std::vector<Empty *> s_emptys;
  static std::vector<Light *> s_lights;

  static BasicMaterialSubmeshes s_basicSubmeshGroups;
  static PbrMaterialSubmeshes s_pbrSubmeshGroups;

  static Materials<MaterialTypes::BasicMat> s_basicMaterials;
  static Materials<MaterialTypes::PbrMat> s_pbrMaterials;

  static uint64_t s_submeshCount;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
