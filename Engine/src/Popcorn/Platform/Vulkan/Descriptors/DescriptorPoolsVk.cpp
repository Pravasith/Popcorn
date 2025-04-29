#include "DescriptorPoolsVk.h"
#include "ContextVk.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void DPoolVk::Create(VkDescriptorPoolSize *poolSizes, uint32_t poolSizesCount,
                     uint32_t maxSets) {
  if (m_pool != VK_NULL_HANDLE) {
    throw std::runtime_error("DPool already exists");
  };

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.pPoolSizes = poolSizes;
  poolInfo.poolSizeCount = poolSizesCount;
  poolInfo.maxSets = maxSets;
  poolInfo.flags = 0;

  if (vkCreateDescriptorPool(ContextVk::Device()->GetDevice(), &poolInfo,
                             nullptr, &m_pool) != VK_SUCCESS) {
    m_pool = VK_NULL_HANDLE;
    throw std::runtime_error("Descriptor pool not created!!");
  }

  m_maxSets = maxSets;
}

//
// Don't modify these partial template methods! Could've moved these to header
// file but ContextVK has a circular dep

void DPoolVk::CleanUp() {
  auto &device = ContextVk::Device()->GetDevice();
  vkDestroyDescriptorPool(device, m_pool, nullptr);
  m_pool = VK_NULL_HANDLE;
}

//
//
// -------------------------------------------------------------------------
// --- DESCRIPTOR POOLS ----------------------------------------------------
//
//
template <>
DPoolVk &DescriptorPoolsVk::GetPool<DescriptorPools::GBufferPool>() {
  if (m_pools.find(DescriptorPools::GBufferPool) == m_pools.end()) {
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

    DPoolVk dPool{};
    dPool.Create(poolSizes, 4, 4 * ContextVk::MAX_FRAMES_IN_FLIGHT);

    m_pools[DescriptorPools::GBufferPool] = dPool;
  }

  return m_pools[DescriptorPools::GBufferPool];
}

template <>
DPoolVk &DescriptorPoolsVk::GetPool<DescriptorPools::LightingPool>() {
  if (m_pools.find(DescriptorPools::LightingPool) == m_pools.end()) {
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

    DPoolVk dPool{};
    dPool.Create(poolSizes, 4, 4 * ContextVk::MAX_FRAMES_IN_FLIGHT);

    m_pools[DescriptorPools::LightingPool] = dPool;
  }

  return m_pools[DescriptorPools::LightingPool];
}

template <>
DPoolVk &DescriptorPoolsVk::GetPool<DescriptorPools::CompositePool>() {
  if (m_pools.find(DescriptorPools::CompositePool) == m_pools.end()) {

    VkDescriptorPoolSize poolSize0 = {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount =
            1 * ContextVk::MAX_FRAMES_IN_FLIGHT}; // Lights image + sampler

    VkDescriptorPoolSize poolSizes[1]{poolSize0};

    DPoolVk dPool{};
    dPool.Create(poolSizes, 1, 1 * ContextVk::MAX_FRAMES_IN_FLIGHT);

    m_pools[DescriptorPools::CompositePool] = dPool;
  }

  return m_pools[DescriptorPools::CompositePool];
}

void DescriptorPoolsVk::CleanUp() {
  auto &device = ContextVk::Device()->GetDevice();

  for (auto &[_, dPool] : m_pools) {
    dPool.CleanUp();
  }

  m_pools.clear();
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
