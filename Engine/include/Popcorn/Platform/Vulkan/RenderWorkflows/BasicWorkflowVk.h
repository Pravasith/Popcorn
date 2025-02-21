#pragma once

#include "DeviceVk.h"
#include "FramebuffersVk.h"
#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "RenderPassVk.h"
#include "RenderWorkflowVk.h"
#include "SwapchainVk.h"
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class BasicRenderWorkflowVk : public RenderWorkflowVk {
public:
  BasicRenderWorkflowVk() {
    CreateSwapchainFramebuffers();
    PC_PRINT("CREATED", TagType::Constr, "BasicWorkflowVk")
  };
  ~BasicRenderWorkflowVk() override {
    CleanUp();
    PC_PRINT("DESTROYED", TagType::Destr, "BasicWorkflowVk")
  };

  virtual void CreateRenderPass() override;
  virtual void CreateVkPipeline(Material &) override;

  virtual void CreateSwapchainFramebuffers() override {
    auto *deviceVk = DeviceVk::Get();
    auto *swapchainVkStn = SwapchainVk::Get();
    auto *framebuffersVkStn = FramebuffersVk::Get();

    auto &swapchainImgViews = swapchainVkStn->GetSwapchainImageViews();
    auto &swapchainExtent = swapchainVkStn->GetSwapchainExtent();

    VkFramebufferCreateInfo createInfo;

    m_swapchainFramebuffers.resize(swapchainImgViews.size());

    for (size_t i = 0; i < swapchainImgViews.size(); ++i) {
      VkImageView attachments[] = {swapchainImgViews[i]};

      framebuffersVkStn->GetDefaultFramebufferState(createInfo);
      createInfo.renderPass = m_basicRenderPass.GetVkRenderPass();
      createInfo.pAttachments = attachments;
      createInfo.width = swapchainExtent.width;
      createInfo.height = swapchainExtent.height;

      // CREATE VK FRAMEBUFFER
      framebuffersVkStn->CreateVkFramebuffer(deviceVk->GetDevice(), createInfo,
                                             m_swapchainFramebuffers[i]);
    };
  };

private:
  virtual void CleanUp() override {
    PC_WARN(m_swapchainFramebuffers.size() << " NORM")

    if (m_swapchainFramebuffers.size() == 0) {
      PC_ERROR("Tried to clear m_swapchainFramebuffers but size is 0!",
               "BasicWorkFlow")
    };

    auto *deviceSingleton = DeviceVk::Get();
    auto &device = deviceSingleton->GetDevice();
    auto *framebuffersVkStn = FramebuffersVk::Get();

    // Cleanup framebuffers
    for (auto &framebuffer : m_swapchainFramebuffers) {
      framebuffersVkStn->DestroyVkFramebuffer(device, framebuffer);
    };

    // Cleanup pipelines
    m_basicGfxPipeline.Destroy(device);

    // Cleanup render passes
    m_basicRenderPass.Destroy(device);
  };

private:
  RenderPassVk m_basicRenderPass;
  GfxPipelineVk m_basicGfxPipeline;
  std::vector<VkFramebuffer> m_swapchainFramebuffers;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
