#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// Singleton
class CommandPoolVk {
public:
  [[nodiscard]] static inline CommandPoolVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new CommandPoolVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of CommandPoolVk")
    };
  };

  [[nodiscard]] inline const VkCommandPool &GetVkCommandPool() const {
    return m_commandPool;
  };

  void GetDefaultCommandBufferAllocInfo(VkCommandBufferAllocateInfo &allocInfo);

  void AllocCommandBuffers(const VkCommandBufferAllocateInfo &allocInfo,
                           VkCommandBuffer *commandBuffers);

  void BeginCommandBuffer(const VkCommandBuffer &commandBuffer) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }
  };

  void EndCommandBuffer(const VkCommandBuffer &commandBuffer) {
    PC_VK_NULL_CHECK(commandBuffer)

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  };

  void CreateCommandPool();
  void CleanUp();

private:
private:
  CommandPoolVk() { PC_PRINT("CREATED", TagType::Constr, "CommandPoolVk") };
  ~CommandPoolVk() { PC_PRINT("DESTROYED", TagType::Destr, "CommandPoolVk") };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  CommandPoolVk(const CommandPoolVk &) = delete;
  CommandPoolVk &operator=(const CommandPoolVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  CommandPoolVk(CommandPoolVk &&) = delete;
  CommandPoolVk &operator=(CommandPoolVk &&) = delete;

private:
  static CommandPoolVk *s_instance;

  VkCommandPool m_commandPool = VK_NULL_HANDLE;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
