#include "DescriptorPoolsVk.h"
#include "ContextVk.h"
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
                                      .descriptorCount =
                                          1 * ContextVk::MAX_FRAMES_IN_FLIGHT};

    VkDescriptorPoolSize poolSize1 = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        .descriptorCount = 1 * ContextVk::MAX_FRAMES_IN_FLIGHT};

    VkDescriptorPoolSize poolSize2 = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        .descriptorCount = 1 * ContextVk::MAX_FRAMES_IN_FLIGHT};

    VkDescriptorPoolSize poolSize3 = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        .descriptorCount = 1 * ContextVk::MAX_FRAMES_IN_FLIGHT};

    VkDescriptorPoolSize poolSizes[4]{poolSize0, poolSize1, poolSize2,
                                      poolSize3};

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.poolSizeCount = 4;
    poolInfo.maxSets = 4 * ContextVk::MAX_FRAMES_IN_FLIGHT;
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

template <>
VkDescriptorPool &DescriptorPoolsVk::GetPool<DescriptorPools::LightingPool>() {
  if (!m_pools[DescriptorPools::LightingPool]) {
    VkDescriptorPoolSize poolSize0 = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        .descriptorCount = 1 * ContextVk::MAX_FRAMES_IN_FLIGHT}; // Lights UBO

    VkDescriptorPoolSize poolSize1 = {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount =
            1 * ContextVk::MAX_FRAMES_IN_FLIGHT}; // Albedo image + sampler

    VkDescriptorPoolSize poolSize2 = {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount =
            1 * ContextVk::MAX_FRAMES_IN_FLIGHT}; // Depth image + sampler

    VkDescriptorPoolSize poolSize3 = {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount =
            1 * ContextVk::MAX_FRAMES_IN_FLIGHT}; // Normals image + sampler

    VkDescriptorPoolSize poolSizes[4]{poolSize0, poolSize1, poolSize2,
                                      poolSize3};

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.poolSizeCount = 4;
    poolInfo.maxSets = 4 * ContextVk::MAX_FRAMES_IN_FLIGHT;
    poolInfo.flags = 0;

    VkDescriptorPool pool;
    if (vkCreateDescriptorPool(ContextVk::Device()->GetDevice(), &poolInfo,
                               nullptr, &pool) != VK_SUCCESS) {
      throw std::runtime_error("Descriptor pool not created!!");
    };

    m_pools[DescriptorPools::LightingPool] = pool;
  };

  return m_pools[DescriptorPools::LightingPool];
}

template <>
VkDescriptorPool &DescriptorPoolsVk::GetPool<DescriptorPools::CompositePool>() {
  if (!m_pools[DescriptorPools::CompositePool]) {

    VkDescriptorPoolSize poolSize0 = {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount =
            1 * ContextVk::MAX_FRAMES_IN_FLIGHT}; // Lights image + sampler

    VkDescriptorPoolSize poolSizes[1]{poolSize0};

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.poolSizeCount = 1;
    poolInfo.maxSets = 1 * ContextVk::MAX_FRAMES_IN_FLIGHT;
    poolInfo.flags = 0;

    VkDescriptorPool pool;
    if (vkCreateDescriptorPool(ContextVk::Device()->GetDevice(), &poolInfo,
                               nullptr, &pool) != VK_SUCCESS) {
      throw std::runtime_error("Descriptor pool not created!!");
    };

    m_pools[DescriptorPools::CompositePool] = pool;
  };

  return m_pools[DescriptorPools::CompositePool];
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
