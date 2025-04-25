#pragma once

#include "Camera.h"
#include "GlobalMacros.h"
#include "MaterialTypes.h"
#include "Mesh.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <cstring>
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

using PcBasicSubmeshes = std::vector<Submesh<MaterialTypes::BasicMat> *>;
using PcPbrSubmeshes = std::vector<Submesh<MaterialTypes::PbrMat> *>;

using PcBasicSubmeshGroups =
    std::unordered_map<MaterialHashType, PcBasicSubmeshes>;
using PcPbrSubmeshGroups = std::unordered_map<MaterialHashType, PcPbrSubmeshes>;

class RenderFlowVk {
public:
  RenderFlowVk() { PC_PRINT("CREATED", TagType::Constr, "RenderFlowVk") };
  virtual ~RenderFlowVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "RenderFlowVk")
  };

  //
  // NEW ----------------------------------------------------------------
  static void CreateAndAllocateVBOsAndIBOs(); // Only once
  static void DestroyVBOsAndIBOs();           // Only once

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

  template <MaterialTypes T> static void AddSubmesh(Submesh<T> *submesh) {
    if constexpr (T == MaterialTypes::BasicMat) {
      uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
      if (PC_ValidateAndAddSubmesh(submesh, s_basicSubmeshGroups[materialId])) {
        ++s_submeshCount;
      };
    } else if constexpr (T == MaterialTypes::PbrMat) {
      uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
      if (PC_ValidateAndAddSubmesh(submesh, s_pbrSubmeshGroups[materialId])) {
        ++s_submeshCount;
      };
    }
  };

  template <MaterialTypes T> static void RemoveSubmesh(Submesh<T> *submesh) {
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
  // Camera
  Camera *camera = nullptr;

  static PcBasicSubmeshGroups s_basicSubmeshGroups;
  static PcPbrSubmeshGroups s_pbrSubmeshGroups;

  static uint64_t s_submeshCount;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
