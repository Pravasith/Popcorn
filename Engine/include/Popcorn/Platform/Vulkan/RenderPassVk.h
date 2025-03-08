#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class RenderPassVk {
public:
  RenderPassVk() { PC_PRINT("CREATED", TagType::Constr, "RenderPassVk"); };
  ~RenderPassVk() { PC_PRINT("DESTROYED", TagType::Destr, "RenderPassVk"); };

  [[nodiscard]] inline const VkRenderPass &GetVkRenderPass() const {
    PC_VK_NULL_CHECK(m_renderPass)
    return m_renderPass;
  };

  //
  // -----------------------------------------------------------------------
  // --- COPY SEMANTICS ----------------------------------------------------
  RenderPassVk(const RenderPassVk &other)
      : m_renderPass(other.m_renderPass) {
          PC_PRINT("COPY CONSTRUCTOR EVOKED", TagType::Constr, "BUFFER")
        };
  RenderPassVk &operator=(const RenderPassVk &other) {
    PC_PRINT("COPY ASSIGNMENT EVOKED", TagType::Print, "BUFFER")

    if (this == &other)
      return *this;

    this->m_renderPass = other.m_renderPass;

    return *this;
  };

  //
  // -----------------------------------------------------------------------
  // --- MOVE SEMANTICS ----------------------------------------------------
  RenderPassVk(RenderPassVk &&other) : m_renderPass(other.m_renderPass) {
    PC_PRINT("MOVE CONSTRUCTOR EVOKED", TagType::Constr, "BUFFER")
    other.m_renderPass = VK_NULL_HANDLE;
  };
  RenderPassVk &operator=(RenderPassVk &&other) {
    PC_PRINT("MOVE ASSIGNMENT EVOKED", TagType::Print, "BUFFER")
    if (this == &other) {
      return *this;
    };

    this->m_renderPass = other.m_renderPass;
    other.m_renderPass = VK_NULL_HANDLE;

    return *this;
  };

  static void
  GetDefaultAttachmentDescription(VkAttachmentDescription &attachment) {
    // Color attachment
    attachment.format = VK_FORMAT_B8G8R8A8_SRGB; // Ex. Swapchain image format
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  };

  static void GetAttachmentRef(VkAttachmentReference &attachmentRef,
                               const uint32_t attachmentIndex) {
    attachmentRef.attachment = attachmentIndex;
    attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  };

  static void
  GetDefaultSubpassDescription(VkSubpassDescription &subpassDescription) {
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    // subpassDescription.pColorAttachments = &attachmentRefs;
    // subpassDescription.pInputAttachments = &attachmentRefs;
    // subpassDescription.pResolveAttachments = &attachmentRefs;
    // subpassDescription.pResolveAttachments = &attachmentRefs;
    // subpassDescription.pDepthStencilAttachment = &attachmentRefs;
    // subpassDescription.pPreserveAttachments = &preserceAttachmentRefs;
  };

  static void
  GetDefaultRenderPassCreateInfo(VkRenderPassCreateInfo &renderPassInfo) {
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = VK_NULL_HANDLE;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = VK_NULL_HANDLE;
  };

  void Create(const VkRenderPassCreateInfo &renderPassInfo,
              const VkDevice &device) {
    PC_VK_NULL_CHECK(device)

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_renderPass) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create render pass!");
    };
  };

  void Destroy(const VkDevice &device) {
    PC_VK_NULL_CHECK(device)
    PC_VK_NULL_CHECK(m_renderPass)

    vkDestroyRenderPass(device, m_renderPass, nullptr);
  };

  void GetDefaultCmdBeginRenderPassInfo(const VkFramebuffer &frameBuffer,
                                        const VkExtent2D &frameExtent,
                                        VkRenderPassBeginInfo &renderPassInfo) {
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.framebuffer = frameBuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = frameExtent;
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
  };

  void RecordBeginRenderPassCommand(
      const VkCommandBuffer &commandBuffer,
      const VkRenderPassBeginInfo &renderPassBeginInfo) {
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
  };

  void RecordEndRenderPassCommand(const VkCommandBuffer &commandBuffer) {
    vkCmdEndRenderPass(commandBuffer);
  };

private:
  VkRenderPass m_renderPass = VK_NULL_HANDLE;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
