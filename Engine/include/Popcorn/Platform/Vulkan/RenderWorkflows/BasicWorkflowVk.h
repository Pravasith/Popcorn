#pragma once

#include "CommandPoolVk.h"
#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "RenderPassVk.h"
#include "RenderWorkflowVk.h"
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class BasicRenderWorkflowVk : public RenderWorkflowVk {
public:
  BasicRenderWorkflowVk() {
    auto *commandBufferVkStn = CommandPoolVk::Get();
    commandBufferVkStn->CreateCommandPool();

    PC_PRINT("CREATED", TagType::Constr, "BasicWorkflowVk")
  };
  ~BasicRenderWorkflowVk() override {
    CleanUp();
    PC_PRINT("DESTROYED", TagType::Destr, "BasicWorkflowVk")
  };

  virtual void CreateWorkflowResources(Material *materialPtr) override {
    PC_WARN("Expensive initialization operation: Creating workflow resources! "
            "Should only be done once per workflow object init.")

    CreateRenderPass();
    CreateVkPipeline(*materialPtr);
    CreateFramebuffers();
  };

private:
  virtual void CreateRenderPass() override;
  virtual void CreateVkPipeline(Material &) override;
  virtual void CreateFramebuffers() override;
  virtual void CreateCommandBuffer() override;

  virtual void CleanUp() override;

private:
  RenderPassVk m_basicRenderPassVk;
  GfxPipelineVk m_basicGfxPipelineVk;

  std::vector<VkFramebuffer> m_swapchainFramebuffers;
  VkCommandBuffer m_commandBuffer;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
