#pragma once

#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "RenderPassVk.h"
#include "RenderWorkflowVk.h"
#include "Scene.h"
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class BasicRenderWorkflowVk : public RenderWorkflowVk {
public:
  BasicRenderWorkflowVk() {
    // auto *commandBufferVkStn = CommandPoolVk::Get();
    // commandBufferVkStn->CreateCommandPool();
    PC_PRINT("CREATED", TagType::Constr, "BasicWorkflowVk")
  };
  ~BasicRenderWorkflowVk() override {
    PC_PRINT("DESTROYED", TagType::Destr, "BasicWorkflowVk")
  };

  virtual void CreateWorkflowResources(Material *materialPtr) override {
    PC_WARN("Expensive initialization operation: Creating workflow Vulkan "
            "resources! Should only be done once per workflow object init.")

    CreateRenderPass();
    CreateVkPipeline(*materialPtr);
    CreateFramebuffers();
  };

  virtual const VkRenderPass &GetRenderPass() const override {
    return m_basicRenderPassVk.GetVkRenderPass();
    ;
  };

  virtual void CreateRenderPass() override;
  virtual void CreateVkPipeline(Material &) override;
  virtual void CreateFramebuffers() override;

  virtual void RecordRenderCommands(const Scene &scene,
                                    const VkCommandBuffer &commandBuffer,
                                    const uint32_t imageIndex) override;
  virtual void CleanUp() override;

private:
  RenderPassVk m_basicRenderPassVk;
  GfxPipelineVk m_colorPipelineVk;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
