#include "CommandPoolVk.h"

#include "CommonVk.h"
#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "SurfaceVk.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

CommandPoolVk *CommandPoolVk::s_instance = nullptr;

void CommandPoolVk::CreateCommandPool() {
  auto *deviceVkStn = DeviceVk::Get();
  auto *surfaceVkStn = SurfaceVk::Get();
  QueueFamilyIndices indices =
      deviceVkStn->GetQueueFamilyIndices(surfaceVkStn->GetSurface());

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = indices.graphicsFamily.value();

  if (vkCreateCommandPool(deviceVkStn->GetDevice(), &poolInfo, nullptr,
                          &m_commandPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
};

void CommandPoolVk::CleanUp() {
  auto *deviceVkStn = DeviceVk::Get();
  vkDestroyCommandPool(deviceVkStn->GetDevice(), m_commandPool, nullptr);
};

void CommandPoolVk::GetDefaultCommandBufferAllocInfo(
    VkCommandBufferAllocateInfo &allocInfo) {
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = m_commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;
};

void CommandPoolVk::AllocCommandBuffer(
    const VkCommandBufferAllocateInfo &allocInfo,
    VkCommandBuffer &commandBuffer) {
  auto *deviceVkStn = DeviceVk::Get();
  if (vkAllocateCommandBuffers(deviceVkStn->GetDevice(), &allocInfo,
                               &commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
