#include "DescriptorPoolsVk.h"
#include "ContextVk.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

DescriptorPoolsVk *DescriptorPoolsVk::s_instance = nullptr;

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

  PC_WARN("Created Pool with Max sets: " << maxSets)
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
DPoolVk &DescriptorPoolsVk::GetPool<DescriptorPools::GlobalDescriptorsPool>(
    uint32_t count) {
  PC_PRINT("HERE", TagType::Print, "DPoolVk")

  if (m_pools.find(DescriptorPools::GlobalDescriptorsPool) == m_pools.end()) {
    // Camera ubo
    VkDescriptorPoolSize poolSize0 = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        .descriptorCount = 1 * count};

    VkDescriptorPoolSize poolSizes[1]{poolSize0};

    DPoolVk dPool{};
    dPool.Create(poolSizes, 1, 1 * count);

    m_pools[DescriptorPools::GlobalDescriptorsPool] = std::move(dPool);
  };

  return m_pools[DescriptorPools::GlobalDescriptorsPool];
}

template <>
DPoolVk &
DescriptorPoolsVk::GetPool<DescriptorPools::GBufferPool>(uint32_t count) {
  if (m_pools.find(DescriptorPools::GBufferPool) == m_pools.end()) {
    // BasicMat dynamic-ubo
    VkDescriptorPoolSize poolSize0 = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        .descriptorCount = 1 * count};

    // PbrMat dynamic-ubo
    VkDescriptorPoolSize poolSize1 = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        .descriptorCount = 1 * count};

    // Submesh dynamic-ubo
    VkDescriptorPoolSize poolSize2 = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        .descriptorCount = 1 * count};

    VkDescriptorPoolSize poolSizes[3]{poolSize0, poolSize1, poolSize2};

    DPoolVk dPool{};
    // 3 * count bc each set uses a separate descriptor each (and it's not the
    // case with lighting pool - where 1 lighting set uses all descriptors in
    // one set)
    dPool.Create(poolSizes, 3, 3 * count);

    m_pools[DescriptorPools::GBufferPool] = std::move(dPool);
  }

  return m_pools[DescriptorPools::GBufferPool];
}

template <>
DPoolVk &
DescriptorPoolsVk::GetPool<DescriptorPools::LightingPool>(uint32_t count) {
  if (m_pools.find(DescriptorPools::LightingPool) == m_pools.end()) {
    VkDescriptorPoolSize poolSize0 = {.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                      .descriptorCount =
                                          1 * count}; // Lights SSBO

    VkDescriptorPoolSize poolSize1 = {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1 * count}; // Albedo image + sampler

    VkDescriptorPoolSize poolSize2 = {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1 * count}; // Depth image + sampler

    VkDescriptorPoolSize poolSize3 = {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1 * count}; // Normals image + sampler

    VkDescriptorPoolSize poolSize4 = {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1 * count}; // Roughness + Metallic image + sampler

    VkDescriptorPoolSize poolSizes[5]{poolSize0, poolSize1, poolSize2,
                                      poolSize3, poolSize4};

    DPoolVk dPool{};
    dPool.Create(poolSizes, 5, count);

    m_pools[DescriptorPools::LightingPool] = std::move(dPool);
  }

  return m_pools[DescriptorPools::LightingPool];
}

template <>
DPoolVk &
DescriptorPoolsVk::GetPool<DescriptorPools::CompositePool>(uint32_t count) {
  if (m_pools.find(DescriptorPools::CompositePool) == m_pools.end()) {

    VkDescriptorPoolSize poolSize0 = {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1 * count}; // Lights image + sampler

    VkDescriptorPoolSize poolSizes[1]{poolSize0};

    DPoolVk dPool{};
    dPool.Create(poolSizes, 1, 1 * count);

    m_pools[DescriptorPools::CompositePool] = std::move(dPool);
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
