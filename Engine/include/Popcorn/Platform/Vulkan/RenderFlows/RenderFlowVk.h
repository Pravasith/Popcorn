#pragma once

#include "GlobalMacros.h"
#include "Material.h"
#include "Mesh.h"
#include "PipelineFactoryVk.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class RenderFlows {
  Basic = 0, // Basic colors
  Lights,
};

class RenderFlowVk {
public:
  RenderFlowVk() {
    m_pipelineFactory = PipelineFactoryVk::Get();
    PC_PRINT("CREATED", TagType::Constr, "RenderFlowVk")
  };
  virtual ~RenderFlowVk() {
    PipelineFactoryVk::Destroy();
    m_pipelineFactory = nullptr;
    PC_PRINT("DESTROYED", TagType::Destr, "RenderFlowVk")
  };

  //
  // PURE -------------------------------------------------------------------
  virtual const VkRenderPass &GetRenderPass() const = 0;

  virtual void
  RecordRenderCommands(const uint32_t frameIndex, const uint32_t currentFrame,
                       VkCommandBuffer &currentFrameCommandBuffer) = 0;
  virtual void ProcessSceneUpdates(const uint32_t currentFrame) = 0;
  virtual void CreatePipelines() = 0;
  virtual void CleanUp() = 0;

  //
  // VIRTUAL ----------------------------------------------------------------
  virtual void CreateRenderPass() {};
  virtual void CreateFramebuffers() {};
  virtual void AllocateVkVertexBuffers() {};
  virtual void AllocateVkIndexBuffers() {};
  virtual void AllocateVkUniformBuffers() {};
  virtual void CreateCommandBuffer() {};
  virtual void CreateDescriptorSetLayouts() {};
  virtual void CreateDescriptorPool() {};
  virtual void CreateDescriptorSets() {};

  template <MaterialTypes T> static void AddSubmesh(Submesh<T> *submesh) {
    if constexpr (T == MaterialTypes::BasicMat) {
      PC_ValidateAndAddSubmesh(submesh, s_basicSubmeshes);
    } else if constexpr (T == MaterialTypes::PbrMat) {
      PC_ValidateAndAddSubmesh(submesh, s_basicSubmeshes);
    }
  };
  template <MaterialTypes T> static void RemoveSubmesh(Submesh<T> *submesh) {
    if constexpr (T == MaterialTypes::BasicMat) {
      PC_ValidateAndRemoveSubmesh(submesh, s_basicSubmeshes);
    } else if constexpr (T == MaterialTypes::PbrMat) {
      PC_ValidateAndRemoveSubmesh(submesh, s_pbrSubmeshes);
    }
  };

protected:
  PipelineFactoryVk *m_pipelineFactory;

  // Submeshes for batching (sorta)
  static std::vector<Submesh<MaterialTypes::BasicMat> *> s_basicSubmeshes;
  static std::vector<Submesh<MaterialTypes::PbrMat> *> s_pbrSubmeshes;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
