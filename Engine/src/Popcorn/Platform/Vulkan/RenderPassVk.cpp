#include "RenderPassVk.h"
#include "GlobalMacros.h"
#include <array>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void RenderPassVk::GetDefaultCmdBeginRenderPassInfo(
    const VkFramebuffer &frameBuffer, const VkExtent2D &frameExtent,
    const VkRenderPass &renderPass, VkRenderPassBeginInfo &renderPassInfo) {
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass;
  renderPassInfo.framebuffer = frameBuffer;
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = frameExtent;

  static constexpr VkClearValue clearAlbedo = {{0.0f, 0.0f, 0.0f, 1.0f}};
  static constexpr VkClearValue clearDepth = {1.0f, 0.0f};
  static constexpr VkClearValue clearNormal = {{0.0f, 0.0f, 1.0f, 0.0f}};
  static constexpr VkClearValue clearRoughnessMetallic = {
      {0.0f, 0.0f, 0.0f, 0.0f}};

  static constexpr std::array<VkClearValue, 4> clearValues{
      clearAlbedo, clearDepth, clearNormal, clearRoughnessMetallic};

  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
