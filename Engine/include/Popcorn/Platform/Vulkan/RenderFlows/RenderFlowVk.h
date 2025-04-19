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

class RenderFlowVk {
public:
  RenderFlowVk() { PC_PRINT("CREATED", TagType::Constr, "RenderFlowVk") };
  virtual ~RenderFlowVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "RenderFlowVk")
  };

  using BasicSubmeshGroupType = std::vector<Submesh<MaterialTypes::BasicMat> *>;
  using PbrSubmeshGroupType = std::vector<Submesh<MaterialTypes::PbrMat> *>;

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
  virtual void AllocateVMABuffers() {
    // Basic Material --
    for (auto &[matId, submeshes] : s_basicSubmeshGroups) {
      for (auto &submesh : submeshes) {
        // Basic submesh
        // TODO: Allocate VMA VBOs, IBOs, and Material
      }
    };

    // Pbr Material --
    for (auto &[matId, submeshes] : s_pbrSubmeshGroups) {
      for (auto &submesh : submeshes) {
        // Basic submesh
      }
    };
  };

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
      PC_ValidateAndAddSubmesh(submesh, s_basicSubmeshGroups[materialId]);
    } else if constexpr (T == MaterialTypes::PbrMat) {
      uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
      PC_ValidateAndAddSubmesh(submesh, s_pbrSubmeshGroups[materialId]);
    }
  };

  template <MaterialTypes T> static void RemoveSubmesh(Submesh<T> *submesh) {
    if constexpr (T == MaterialTypes::BasicMat) {
      uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
      PC_ValidateAndRemoveSubmesh(submesh, s_basicSubmeshGroups[materialId]);
    } else if constexpr (T == MaterialTypes::PbrMat) {
      uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
      PC_ValidateAndRemoveSubmesh(submesh, s_pbrSubmeshGroups[materialId]);
    }
  };

protected:
  // Camera
  Camera *camera;

  static std::unordered_map<MaterialHashType, BasicSubmeshGroupType>
      s_basicSubmeshGroups;
  static std::unordered_map<MaterialHashType, PbrSubmeshGroupType>
      s_pbrSubmeshGroups;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
