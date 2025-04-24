#include "DescriptorPoolVk.h"
#include "ContextVk.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
//
// -------------------------------------------------------------------------
// --- DESCRIPTOR POOL -----------------------------------------------------
//
void DescriptorPoolVk::GetDefaultDescriptorPoolState(
    VkDescriptorPoolCreateInfo &poolInfo, uint32_t maxDSets,
    std::vector<VkDescriptorPoolSize> &poolSizes) {
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = maxDSets;
  poolInfo.flags = 0;
};

void DescriptorPoolVk::CreateDescriptorPool(
    const VkDescriptorPoolCreateInfo &poolInfo, VkDescriptorPool *pool) {
  auto &device = ContextVk::Device()->GetDevice();
  if (vkCreateDescriptorPool(device, &poolInfo, nullptr, pool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

void DescriptorPoolVk::DestroyDescriptorPool(VkDescriptorPool &pool) {
  auto &device = ContextVk::Device()->GetDevice();
  vkDestroyDescriptorPool(device, pool, nullptr);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
