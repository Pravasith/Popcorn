#include "RenderPassVk.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void RenderPassVk::GetDefaultCmdBeginRenderPassInfo(
    const VkFramebuffer &frameBuffer, const VkExtent2D &frameExtent,
    const VkRenderPass &renderPass, std::vector<VkClearValue> &clearValues,
    VkRenderPassBeginInfo &renderPassInfo) {
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass;
  renderPassInfo.framebuffer = frameBuffer;
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = frameExtent;

  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
