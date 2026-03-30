#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <vector>
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
    m_renderPass = VK_NULL_HANDLE;
  };

  static void GetDefaultCmdBeginRenderPassInfo(
      const VkFramebuffer &frameBuffer, const VkExtent2D &frameExtent,
      const VkRenderPass &renderPass, std::vector<VkClearValue> &clearValues,
      VkRenderPassBeginInfo &renderPassInfo);

  void BeginRenderPass(const VkCommandBuffer &commandBuffer,
                       const VkRenderPassBeginInfo &renderPassBeginInfo) {
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
  };

  void EndRenderPass(const VkCommandBuffer &commandBuffer) {
    vkCmdEndRenderPass(commandBuffer);
  };

private:
  VkRenderPass m_renderPass = VK_NULL_HANDLE;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
