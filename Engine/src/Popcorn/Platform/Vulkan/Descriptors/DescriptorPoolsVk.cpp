#include "DescriptorPoolsVk.h"
#include "ContextVk.h"
#include "RendererVk.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
//
// -------------------------------------------------------------------------
// --- DESCRIPTOR POOLS ----------------------------------------------------
//
//
template <>
VkDescriptorPool &DescriptorPoolsVk::GetPool<DescriptorPools::GBufferPool>() {
  if (!m_pools[DescriptorPools::GBufferPool]) {

    VkDescriptorPoolSize poolSize0 = {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                      .descriptorCount = 1};

    VkDescriptorPoolSize poolSize1 = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        .descriptorCount = 1};

    VkDescriptorPoolSize poolSize2 = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        .descriptorCount = 1};

    VkDescriptorPoolSize poolSize3 = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        .descriptorCount = 1};

    VkDescriptorPoolSize poolSizes[4]{poolSize0, poolSize1, poolSize2,
                                      poolSize3};

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.poolSizeCount = 4;
    poolInfo.maxSets = 4 * RendererVk::MAX_FRAMES_IN_FLIGHT;
    poolInfo.flags = 0;

    VkDescriptorPool pool;
    if (vkCreateDescriptorPool(ContextVk::Device()->GetDevice(), &poolInfo,
                               nullptr, &pool) != VK_SUCCESS) {
      throw std::runtime_error("Descriptor pool not created!!");
    };

    m_pools[DescriptorPools::GBufferPool] = pool;
  };

  return m_pools[DescriptorPools::GBufferPool];
}

void DescriptorPoolsVk::CleanUp() {
  auto &device = ContextVk::Device()->GetDevice();

  for (auto &[a, pool] : m_pools) {
    vkDestroyDescriptorPool(device, pool, nullptr);
  };

  m_pools.clear();
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
