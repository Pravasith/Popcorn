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

enum class RenderFlows {
  Deferred = 0,
  Lights,
};

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

  virtual void ProcessSceneUpdates(const uint32_t currentFrame) = 0;
  virtual void
  RecordRenderCommands(const uint32_t frameIndex, const uint32_t currentFrame,
                       VkCommandBuffer &currentFrameCommandBuffer) = 0;
  virtual void Prepare() = 0;
  virtual void CleanUp() = 0;

  //
  //
  //
  //
  //
  // NEW -- NEW -- NEW -- NEW -- NEW -- NEW -- NEW -- NEW --
  //
  virtual void CreateAttachments() {};
  virtual void CreateRenderPass() {};
  virtual void CreateFramebuffer() {};
  virtual void CreatePipelines() {};

  // Allocates VBOs, IBOs, and UBOs
  static void AllocateVMABuffers();

  //
  //
  //
  //
  //
  //

  // COMMON
  virtual void AllocateVkVertexBuffers() {};
  virtual void AllocateVkIndexBuffers() {};
  virtual void AllocateVkUniformBuffers() {};
  virtual void CreateCommandBuffer() {};
  virtual void CreateDescriptorSetLayouts() {};
  virtual void CreateDescriptorPool() {};
  virtual void CreateDescriptorSets() {};

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
