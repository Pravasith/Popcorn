#pragma once
#include "CommonVk.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class CmdPoolVk {
  friend class RendererVk;

  CmdPoolVk() { PC_PRINT("CREATED", TagType::Constr, "COMMAND-POOL-VK") };
  ~CmdPoolVk() { PC_PRINT("DESTROYED", TagType::Destr, "COMMAND-POOL-VK") };

  void CreateCmdPool(const QueueFamilyIndices &, const VkDevice &);
  void CreateCmdBfr(const VkDevice &);
  void RecordCmdBfr(VkCommandBuffer, uint32_t imgIdx, const VkRenderPass &,
                    const std::vector<VkFramebuffer> &, const VkExtent2D &,
                    const VkPipeline &);
  void CleanUp(const VkDevice &dev);

private:
  VkCommandPool m_cmdPool;
  VkCommandBuffer m_cmdBfr;
};

ENGINE_NAMESPACE_END
