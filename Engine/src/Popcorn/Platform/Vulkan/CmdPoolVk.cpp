#include "CmdPoolVk.h"
#include "CommonVk.h"
#include "GlobalMacros.h"
#include <cstdint>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
void CmdPoolVk::CreateCmdPool(const QueueFamilyIndices &qFamIndices,
                              const VkDevice &dev) {
  VkCommandPoolCreateInfo cmdPoolInfo{};
  cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  cmdPoolInfo.queueFamilyIndex = qFamIndices.graphicsFamily.value();

  if (vkCreateCommandPool(dev, &cmdPoolInfo, nullptr, &m_cmdPool) !=
      VK_SUCCESS) {
    throw std::runtime_error("ERROR: FAILED TO CREATE COMMAND POOL!");
  };
};

void CmdPoolVk::CreateCmdBfrs(const VkDevice &dev) {
  VkCommandBufferAllocateInfo cmdBfrAllocInfo{};
  cmdBfrAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmdBfrAllocInfo.commandPool = m_cmdPool;
  cmdBfrAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  m_cmdBfrs.resize(MAX_FRAMES_IN_FLIGHT);
  cmdBfrAllocInfo.commandBufferCount = static_cast<uint32_t>(m_cmdBfrs.size());

  if (vkAllocateCommandBuffers(dev, &cmdBfrAllocInfo, m_cmdBfrs.data()) !=
      VK_SUCCESS) {
    throw std::runtime_error("FAILED TO ALLOCATE COMMAND BUFFERS!");
  }
};

void CmdPoolVk::RecordCmdBfrFtr::operator()(VkCommandBuffer &cmdBfr,
                                            uint32_t imgIdx) {
#ifdef PC_DEBUG
  if (!m_rndrPass ||
      m_frameBfrs.size() == 0
      // || !m_swpChnExt
      || !m_pipeline) {
    throw std::runtime_error("RecordCmdBfr FUNCTOR NULL MEMBERS");
  };
#endif

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(cmdBfr, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("FAILED TO BEGIN RECORDING COMMAND BUFFER!");
  };

  // STARTING A RENDER PASS
  VkRenderPassBeginInfo rndrPassBeginInfo{};
  rndrPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  rndrPassBeginInfo.renderPass = m_rndrPass;
  rndrPassBeginInfo.framebuffer = m_frameBfrs[imgIdx];
  rndrPassBeginInfo.renderArea.offset = {0, 0};
  rndrPassBeginInfo.renderArea.extent = m_swpChnExt;

  VkClearValue clearClr = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  rndrPassBeginInfo.clearValueCount = 1;
  rndrPassBeginInfo.pClearValues = &clearClr;

  vkCmdBeginRenderPass(cmdBfr, &rndrPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

  // BIND GRAPHICS PIPELINE
  vkCmdBindPipeline(cmdBfr, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

  VkViewport vwprt{};
  vwprt.x = 0.0f;
  vwprt.y = 0.0f;
  vwprt.width = static_cast<float>(m_swpChnExt.width);
  vwprt.height = static_cast<float>(m_swpChnExt.height);
  vwprt.minDepth = 0.0f;
  vwprt.maxDepth = 1.0f;

  vkCmdSetViewport(cmdBfr, 0, 1, &vwprt);

  VkRect2D scssr{};
  scssr.offset = {0, 0};
  scssr.extent = m_swpChnExt;
  vkCmdSetScissor(cmdBfr, 0, 1, &scssr);

  vkCmdDraw(cmdBfr, 3, 1, 0, 0);

  vkCmdEndRenderPass(cmdBfr);

  if (vkEndCommandBuffer(cmdBfr) != VK_SUCCESS) {
    throw std::runtime_error("FAILED TO RECORD COMMAND BUFFER!");
  };
}

void CmdPoolVk::CleanUp(const VkDevice &dev) {
  vkDestroyCommandPool(dev, m_cmdPool, nullptr);
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
